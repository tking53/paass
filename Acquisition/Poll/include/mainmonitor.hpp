#ifndef __MAIN_MONITOR_HPP
#define __MAIN_MONITOR_HPP

#include "monitor.hpp"
#include "poll2_socket.h"


#ifdef PAASS_USE_PROMETHEUS
#include <chrono>
#include <memory>
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/gauge.h>
#include <prometheus/registry.h>
#endif  // PAASS_USE_PROMETHEUS

class mainmonitor : public monitor {
   public:
    enum RunState {
        RUN_STATE_NONE = 0,
        RUN_STATE_STOPPED = 1,
        RUN_STATE_VME = 2,
        RUN_STATE_RUN = 3,
        RUN_STATE_UNKNOWN_RUNNING = 4  // for when we get stats packets but missed the run state message, so we're not sure if we're in VME or RUN, but we know we're not in NONE
    };

    mainmonitor();
    ~mainmonitor();
    void OpenSubmonitorSockets();
    std::vector<Client>* GetSubClientVec() { return &Submonitor_Clients; };  // return the vec of submonitor poll clients
    Server* GetPollServer() { return poll_server; };                    // return the poll server

    void RelayPoll2msg(char* buffer, size_t msg_size);

    void CloseSubmonitors();
#ifdef PAASS_USE_PROMETHEUS
    void SetRunState(const RunState &state);
    RunState GetRunState() const { return runState_; };
    void InitPrometheus(const std::string &address, int port);
    void UpdatePrometheusMetrics(const monitor::poll2_UDP_msg &msg, int numModules);
    void ZeroRatesPrometheus();
#endif

   private:
#ifdef PAASS_USE_PROMETHEUS
    void EnsurePrometheusCapacity(int numModules);
    std::unique_ptr<prometheus::Exposer> promExposer_;
    std::shared_ptr<prometheus::Registry> promRegistry_;
    // per-channel scalar families
    prometheus::Family<prometheus::Gauge> *runtimeFamily_ = nullptr;
    prometheus::Family<prometheus::Gauge> *scalarRateFamily_ = nullptr;
    prometheus::Family<prometheus::Gauge> *scalarTotalsFamily_ = nullptr;
    prometheus::Gauge *runtimeGauge_ = nullptr;
    prometheus::Gauge *dataRateGauge_ = nullptr;  // direct gauge, not a family
    std::vector<std::vector<prometheus::Gauge *>> icr_;
    std::vector<std::vector<prometheus::Gauge *>> ocr_;
    std::vector<std::vector<prometheus::Gauge *>> data_;
    std::vector<std::vector<prometheus::Gauge *>> totals_;
    // system-level pulse metrics (no module/channel labels, direct pointers)
    prometheus::Counter *pulseCounter_ = nullptr;
    prometheus::Gauge *lastPulseGauge_ = nullptr;
    prometheus::Gauge *interpulseSecondsGauge_ = nullptr;
    prometheus::Gauge *runStateCodeGauge_ = nullptr;
    prometheus::Gauge *lastBeginGauge_ = nullptr;
    double previousPulseUnixSeconds_ = -1.0;
#endif
    RunState runState_ = RUN_STATE_NONE;
    Server* poll_server;
    std::string name;
    std::vector<Client> Submonitor_Clients;
}; // end class mainmonitor


std::vector<Client>* Submonitor_Client_List;  // must be global for signal_handlers
bool DumMode = false;                         // must be global for signal_handlers
mainmonitor* g_mmon = nullptr;               // must be global for signal_handlers


#endif // __MAIN_MONITOR_HPP