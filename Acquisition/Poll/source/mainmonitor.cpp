/** \file mainmonitor.cpp
 *
 * \brief Receives and decodes rate packets from StatsHandler, and rebroadcasts packets to the submonitors
 *
 * \author T.T. King, Cory R. Thornsberry, K. Smith, and S. Burcher
 *
 * \date Feb 19, 2023
 *
 */

#include "mainmonitor.hpp"

// TODO test what happens when subs are ctrlC'd first then try to quit main
// TODO also figureout best way to handle reading in the Max_Num_submontor var
//  Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum) {
    std::cout << "\n    SIGINT:: Sending KILL_SOCKET to the submonitors\n";
    if (!DumMode) {
        for (int i = 0; i < 5; ++i) {
            Submonitor_Client_List->at(i).SendMessage((char *)"$KILL_SOCKET", 13);  // Terminate program
        }
    }
#ifdef PAASS_USE_PROMETHEUS
    if (g_mmon) {
        std::cout << std::endl << "    SIGINT:: Flushing Prometheus metrics, and Setting run state to NONE\n" << std::endl;
        g_mmon->SetRunState(mainmonitor::RUN_STATE_NONE);
        sleep(2);  // Sleep to allow prometheus metrics to flush before process termination
    }
#endif
    exit(signum);
}

#ifdef PAASS_USE_PROMETHEUS
void mainmonitor::InitPrometheus(const std::string &address, int port) {
    promExposer_ = std::unique_ptr<prometheus::Exposer>(
        new prometheus::Exposer(address + ":" + std::to_string(port)));
    promRegistry_ = std::make_shared<prometheus::Registry>();
    runtimeFamily_ = &prometheus::BuildGauge()
                          .Name("poll2_monitor_runtime_seconds")
                          .Help("Run time in seconds from the most recent poll2 packet")
                          .Register(*promRegistry_);
    dataRateGauge_ = &prometheus::BuildGauge()
                          .Name("poll2_monitor_data_rate")
                          .Help("Global data rate in bytes per second from the most recent poll2 packet")
                          .Register(*promRegistry_)
                          .Add({});
    scalarRateFamily_ = &prometheus::BuildGauge()
                             .Name("poll2_monitor_scalars")
                             .Help("Per-module, per-channel scalar rates in Hz")
                             .Register(*promRegistry_);
    scalarTotalsFamily_ = &prometheus::BuildGauge()
                               .Name("poll2_monitor_scalar_totals")
                               .Help("Per-module, per-channel scalar total counts")
                               .Register(*promRegistry_);
    promExposer_->RegisterCollectable(promRegistry_);
    runtimeGauge_ = &runtimeFamily_->Add({});
    // poll2-level metrics (direct pointers)
    pulseCounter_ = &prometheus::BuildCounter()
                         .Name("poll2_monitor_pulse_total")
                         .Help("Total number of full-system digitizer scalar pulses received")
                         .Register(*promRegistry_)
                         .Add({});
    lastPulseGauge_ = &prometheus::BuildGauge()
                          .Name("poll2_monitor_last_pulse_unix_seconds")
                          .Help("Unix timestamp (seconds) of the most recently received digitizer scalar pulse")
                          .Register(*promRegistry_)
                          .Add({});
    interpulseSecondsGauge_ = &prometheus::BuildGauge()
                                  .Name("poll2_monitor_interpulse_seconds")
                                  .Help("Time between consecutive full-system digitizer scalar pulses in seconds")
                                  .Register(*promRegistry_)
                                  .Add({});
    interpulseSecondsGauge_->Set(0.0);
    runStateCodeGauge_ = &prometheus::BuildGauge()
                             .Name("poll2_monitor_run_state_code")
                             .Help("Run state code from poll2 control messages (0=NONE, 1=VME, 2=RUN)")
                             .Register(*promRegistry_)
                             .Add({});
    runStateCodeGauge_->Set(static_cast<double>(runState_));
    lastBeginGauge_ = &prometheus::BuildGauge()
                             .Name("poll2_monitor_last_begin_unix_seconds")
                             .Help("Unix timestamp (seconds) when the current run started; 0 when not running")
                             .Register(*promRegistry_)
                             .Add({});
    lastBeginGauge_->Set(0.0);
}

void mainmonitor::UpdatePrometheusMetrics(const monitor::poll2_UDP_msg &msg, int numModules) {
    EnsurePrometheusCapacity(numModules);
    runtimeGauge_->Set(msg.time_in_sec);
    dataRateGauge_->Set(msg.data_rate);
    for (int mod = 0; mod < numModules; ++mod) {
        for (int chan = 0; chan < 16; ++chan) {
            icr_[mod][chan]->Set(msg.ICR[mod][chan]);
            ocr_[mod][chan]->Set(msg.OCR[mod][chan]);
            data_[mod][chan]->Set(msg.Data[mod][chan]);
            totals_[mod][chan]->Set(static_cast<double>(msg.Totals[mod][chan]));
        }
    }
    // system-level pulse metrics
    pulseCounter_->Increment();
    const double now = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()) / 1000.0;

    if (interpulseSecondsGauge_) {
        if (previousPulseUnixSeconds_ >= 0.0 && now >= previousPulseUnixSeconds_) {
            interpulseSecondsGauge_->Set(now - previousPulseUnixSeconds_);
        } else {
            interpulseSecondsGauge_->Set(0.0);
        }
    }
    previousPulseUnixSeconds_ = now;

    lastPulseGauge_->Set(now);

    // run state already flushed in SetRunState(); nothing to do here
}

void mainmonitor::ZeroRatesPrometheus() {
    for (int mod = 0; mod < static_cast<int>(icr_.size()); ++mod) {
        for (int chan = 0; chan < 16; ++chan) {
            if (icr_[mod][chan]) icr_[mod][chan]->Set(0.0);
            if (ocr_[mod][chan]) ocr_[mod][chan]->Set(0.0);
        }
    }
}

void mainmonitor::EnsurePrometheusCapacity(int numModules) {
    while (static_cast<int>(icr_.size()) < numModules) {
        const int mod = static_cast<int>(icr_.size());
        icr_.emplace_back(16, nullptr);
        ocr_.emplace_back(16, nullptr);
        data_.emplace_back(16, nullptr);
        totals_.emplace_back(16, nullptr);
        for (int chan = 0; chan < 16; ++chan) {
            const std::string moduleLabel = std::to_string(mod);
            const std::string channelLabel = std::to_string(chan);
            icr_[mod][chan] = &scalarRateFamily_->Add({{"module", moduleLabel}, {"channel", channelLabel}, {"type", "icr"}});
            ocr_[mod][chan] = &scalarRateFamily_->Add({{"module", moduleLabel}, {"channel", channelLabel}, {"type", "ocr"}});
            data_[mod][chan] = &scalarRateFamily_->Add({{"module", moduleLabel}, {"channel", channelLabel}, {"type", "data"}});
            totals_[mod][chan] = &scalarTotalsFamily_->Add({{"module", moduleLabel}, {"channel", channelLabel}});
        }
    }
}

void mainmonitor::SetRunState(const RunState &state){
    runState_ = state;
    if (runStateCodeGauge_){
        runStateCodeGauge_->Set(static_cast<double>(runState_));
    }
    if (lastBeginGauge_) {
        if (runState_ == RUN_STATE_RUN || runState_ == RUN_STATE_VME){
            // We will get the current time in UNIX timestamp, cast to milliseconds and send it to the guage
            const double now = static_cast<double>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
            lastBeginGauge_->Set(now);
        }else if (runState_ == RUN_STATE_STOPPED || runState_ == RUN_STATE_NONE || runState_ == RUN_STATE_UNKNOWN_RUNNING){
            lastBeginGauge_->Set(0.0);
        }
    }
}
#endif  // PAASS_USE_PROMETHEUS

mainmonitor::mainmonitor() : monitor("mainmonitor") {
    poll_server = new Server;
    Submonitor_Client_List = &Submonitor_Clients;
}
mainmonitor::~mainmonitor() {
    if (this->GetDummyMode() && poll_server) {
        delete[] poll_server;
    }
#ifdef PAASS_USE_PROMETHEUS
    // Set run state to NONE before the exposer shuts down to hopefully ensure that prometheus grabs the NONE state so we can know it shutdown rather than crashed. 
    SetRunState(RUN_STATE_NONE);
#endif
}
void mainmonitor::CloseSubmonitors() {
    if (!GetDummyMode()) {
        for (int i = 0; i < GetMaxNumSubMonitors(); ++i) {
            GetSubClientVec()->at(i).Close();
        }
    }
}
void mainmonitor::RelayPoll2msg(char *buffer, size_t msg_size) {
    for (int i = 0; i < MAX_NUM_SUBMONITORS; ++i) {
        Submonitor_Clients.at(i).SendMessage(buffer, msg_size);
    }
}

void mainmonitor::OpenSubmonitorSockets() {
    std::cout << " Opening relay ports for submonitors" << std::endl;
    std::string submonitorPortMessage = " Opened Client on ports ";
    for (int i = 0; i < MAX_NUM_SUBMONITORS; ++i) {
        Submonitor_Clients.emplace_back(Client());
        int port2open = PREDEFINED_POLL2_PORT + i + 1;
        if (Submonitor_Clients.at(i).Init("127.0.0.1", port2open)) {
            submonitorPortMessage = submonitorPortMessage + std::to_string(port2open);
            if (i == MAX_NUM_SUBMONITORS - 1) {
                submonitorPortMessage += ".";
            } else {
                submonitorPortMessage += ", ";
            }
        } else {
            std::cout << "FAILED to open Client on port " << port2open << std::endl;
        }
    }
    std::cout << submonitorPortMessage.c_str() << std::endl;
}

int main(int argc, char *argv[]) {
    mainmonitor mmon;

    int parRetVal = mmon.ParseCliFlags(argc, argv, &mmon);
    if (parRetVal != 0) {
        return parRetVal;
    }

    // needed for sig handler
    DumMode = mmon.GetDummyMode();
    g_mmon = &mmon;

    Server *pserv = mmon.GetPollServer();

    char buffer[mmon.GetPoll2MsgSize()];
    size_t msg_size = mmon.GetPoll2MsgSize();

    monitor::colorThresholds colThreshStruct;
    mmon.SetColorThresholdStruct(colThreshStruct, mmon.GetColorThreshGroup());

    signal(SIGINT, signal_callback_handler);

    int num_modules;
    bool first_packet = true;

    // These are for the dummy mode. I doesnt seem to affect the normal mode startup at all
    int counter = 0;
    time_t curtime = time(NULL);
    srand((unsigned)curtime);

    monitor::poll2_UDP_msg pUdpMsg;

#ifdef PAASS_USE_PROMETHEUS
    if (mmon.GetGrafanaEnabled()) {
        mmon.InitPrometheus(mmon.GetPrometheusBindAddress(), mmon.GetPrometheusPort());
        std::cout << " Prometheus exporter sending on " << mmon.GetPrometheusBindAddress() << ":" << mmon.GetPrometheusPort() << std::endl;
        mmon.SetRunState(mainmonitor::RUN_STATE_NONE);
    }
#endif

    if (pserv->Init(mmon.GetPredefinedPoll2Port()) || mmon.GetDummyMode()) {
        if (!mmon.GetDummyMode()) {
            mmon.OpenSubmonitorSockets();
        };
        std::cout << "\n Waiting for first stats packet...\n";

        if (mmon.GetDummyMode()) {
            num_modules = 11;
            for (int it = 0; it < 10; ++it) {  // 10 dead chans in dummy mode
                mmon.GetDeadChanList()->emplace_back(std::make_pair((rand() % num_modules), (rand() % 16)));
            }
        } else {
            mmon.GetDeadChanList()->emplace_back(std::make_pair(-1, -1));
        }

        while (true) {
            if (counter == 50 && mmon.GetDummyMode()) {
                break;
            }
            std::cout << std::setprecision(2);
            if (!mmon.GetDummyMode()) {
                pserv->RecvMessage(buffer, msg_size);
                char *ptr = buffer;

                mmon.RelayPoll2msg(buffer, msg_size);

                #ifdef PAASS_USE_PROMETHEUS
                if (mmon.GetRunState()== mainmonitor::RUN_STATE_NONE){
                    // If we get a stats packet but we're in NONE state, that means the run state message was missed somehow. This can happen if mainmonitor starts after the BEGIN_RUN message was sent
                    mmon.SetRunState(mainmonitor::RUN_STATE_UNKNOWN_RUNNING);  
                }
                #endif

                if (strcmp(ptr, "$KILL_SOCKET") == 0) {
                    std::cout << "  Received KILL_SOCKET flag...\n\n";
                    #ifdef PAASS_USE_PROMETHEUS
                    mmon.SetRunState(mainmonitor::RUN_STATE_NONE);
                    #endif
                    break;
                } else if (strcmp(ptr, "$BEGIN_RUN") == 0) {
                    #ifdef PAASS_USE_PROMETHEUS
                    mmon.SetRunState(mainmonitor::RUN_STATE_RUN);
                    #endif
                    std::cout << "  Received BEGIN_RUN flag...\n\n";
                    continue;
                } else if (strcmp(ptr, "$BEGIN_VME") == 0) {
                    #ifdef PAASS_USE_PROMETHEUS
                    mmon.SetRunState(mainmonitor::RUN_STATE_VME);
                    #endif
                    std::cout << "  Received BEGIN_VME flag...\n\n";
                    continue;
                }else if (strcmp(ptr, "$END_RUN") == 0) {
                    #ifdef PAASS_USE_PROMETHEUS
                    mmon.SetRunState(mainmonitor::RUN_STATE_STOPPED);
                    if (mmon.GetGrafanaEnabled()) {
                        mmon.ZeroRatesPrometheus();
                    }
                    #endif
                    std::cout << "  Received END_RUN flag...\n\n";
                    continue;
                } else {
                    system("clear");
                }
                /*
                std::cout << " Received:\t" << recv_bytes << " bytes\n";

                Below is the stats packet structure (for N modules)
                ---------------------------------------------------
                4 byte total number of pixie modules (N)
                8 byte total time of run (in seconds)
                8 byte total data rate (in B/s)
                channel 0, 0 rate
                channel 0, 0 total
                channel 0, 1 rate
                channel 0, 1 total
                ...
                channel 0, 15 rate
                channel 0, 15 total
                channel 1, 0 rate
                channel 1, 0 total
                ...
                channel N-1, 15 rate
                channel N-1, 15 total
                */
                memcpy(&num_modules, ptr, 4);
                ptr += 4;

                mmon.DecodeUdpMsg(ptr, pUdpMsg, num_modules, first_packet);
            } else {
                system("clear");
                mmon.DecodeUdpMsg(pUdpMsg, num_modules, first_packet, (*mmon.GetDeadChanList()));
            }

#ifdef PAASS_USE_PROMETHEUS
            if (mmon.GetGrafanaEnabled()) {
                mmon.UpdatePrometheusMetrics(pUdpMsg, num_modules);
            }
#endif

            // Display the rate information
            std::cout << "Run Time: " << mmon.GetTimeString(pUdpMsg.time_in_sec);
            if (num_modules > 1)
                std::cout << "\t";
            else
                std::cout << "\n";
            if (mmon.GetDummyMode()) {
                std::cout << "Data Rate: " << mmon.GetRateString(pUdpMsg.data_rate, mmon.GetColorOut()) << mmon.GetEscSequence(monitor::FG_RED, mmon.GetColorOut())
                     << " DummyMode= " << mmon.GetDummyMode() << mmon.GetEscSequence(monitor::FG_DEFAULT, mmon.GetColorOut()) << std::endl;
            } else {
                std::cout << "Data Rate: " << mmon.GetRateString(pUdpMsg.data_rate, mmon.GetColorOut()) << std::endl;
            }
            int numberOfModsPerRow = (int)ceil((float)num_modules / (float)mmon.GetNumOfModRows());
            int firstModInRow = 0;
            for (int rowNum = 1; rowNum <= mmon.GetNumOfModRows(); ++rowNum) {
                if (rowNum > 1) {
                    std::cout << std::setw(mmon.GetModColumWidth() * numberOfModsPerRow + 9) << std::setfill('-') << "\n";
                }
                std::cout << "   ";
                for (int i = firstModInRow; i < (rowNum * numberOfModsPerRow); i++) {
                    if (i == num_modules) {
                        break;
                    }
                    std::cout << "|"
                         << std::setw((int)((mmon.GetModColumWidth() - 1. + 0.5) / 2))
                         << std::setfill('-') << "M" << std::setw(2)
                         << std::setfill('0') << i
                         << std::setw((int)((mmon.GetModColumWidth() - 2. + 0.5) / 2))
                         << std::setfill('-') << "";
                }
                std::cout << "|\n";

                std::cout << "   | ";
                for (int j = firstModInRow; j < (rowNum * numberOfModsPerRow); j++) {
                    if (j == num_modules) {
                        break;
                    }
                    std::cout << "ICR  ";
                    std::cout << " OCR ";
                    std::cout << " Data ";
                    std::cout << "  Total | ";
                }
                std::cout << "\n";
                std::pair<std::string, monitor::ColorCode> ICR;
                std::pair<std::string, monitor::ColorCode> OCR;
                std::pair<std::string, monitor::ColorCode> DATA;
                std::pair<std::string, monitor::ColorCode> TOTALS;
                for (unsigned int i = 0; i < 16; i++) {
                    std::cout << mmon.GetEscSequence(monitor::FG_DEFAULT, mmon.GetColorOut()) << "C" 
                    << std::setw(2) << std::setfill('0') << i << "|";
                    for (int j = firstModInRow; j < (rowNum * numberOfModsPerRow); j++) {
                        if (j == num_modules) {
                            break;
                        }
                        ICR = mmon.GetChanRateString(pUdpMsg.ICR[j][i], colThreshStruct);
                        OCR = mmon.GetChanRateString(pUdpMsg.OCR[j][i], colThreshStruct);
                        DATA = mmon.GetChanRateString(pUdpMsg.Data[j][i], colThreshStruct);
                        TOTALS = mmon.GetChanTotalString(pUdpMsg.Totals[j][i]);
                        if (strcmp(TOTALS.first.c_str(), "0") == 0 && strcmp(DATA.first.c_str(), "   0 ") == 0) {
                            ICR.second = mmon.GetColorFromThresholds(-999, colThreshStruct);
                            OCR.second = mmon.GetColorFromThresholds(-999, colThreshStruct);
                            DATA.second = mmon.GetColorFromThresholds(-999, colThreshStruct);
                            TOTALS.second = mmon.GetColorFromThresholds(-999, colThreshStruct);
                        }

                        std::cout << mmon.GetEscSequence(ICR.second, mmon.GetColorOut()) << std::setw(5) << std::setfill(' ')
                             << ICR.first << " " << mmon.GetEscSequence(monitor::FG_DEFAULT, mmon.GetColorOut());
                        std::cout << mmon.GetEscSequence(OCR.second, mmon.GetColorOut()) << std::setw(5) << std::setfill(' ')
                             << OCR.first << " " << mmon.GetEscSequence(monitor::FG_DEFAULT, mmon.GetColorOut());
                        std::cout << mmon.GetEscSequence(DATA.second, mmon.GetColorOut()) << std::setw(5) << std::setfill(' ')
                             << DATA.first << " " << mmon.GetEscSequence(monitor::FG_DEFAULT, mmon.GetColorOut());
                        std::cout << std::setw(6) << TOTALS.first << " ";
                        std::cout << "|";
                    }
                    std::cout << "\n";
                }
                firstModInRow = firstModInRow + numberOfModsPerRow;
            }
            if (mmon.GetDummyMode()) {
                sleep(2);
                counter++;
            }
        }
    } else {
        std::cout << " Error: Failed to open poll socket 5556! mainmonitor is probably running. Use the submonitor program.\n";
        return 1;
    }
    if (!mmon.GetDummyMode() && mmon.GetPollServer()) {
        pserv->Close();
    }

    if (pUdpMsg.ICR) {
        delete[] pUdpMsg.ICR;
    }
    if (pUdpMsg.OCR) {
        delete[] pUdpMsg.OCR;
    }
    if (pUdpMsg.Data) {
        delete[] pUdpMsg.Data;
    }
    if (pUdpMsg.Totals) {
        delete[] pUdpMsg.Totals;
    }
    if (Submonitor_Client_List && !mmon.GetDummyMode()) {
        mmon.CloseSubmonitors();
        Submonitor_Client_List = nullptr;
    }
    return 0;
}
