/** \file monitor.cpp
 *
 * \brief Monitor base class.
 *
 * \author T.T. King, Cory R. Thornsberry, K. Smith, and S. Burcher
 *
 * \date Feb 19th, 2023
 *
 */

#include "monitor.hpp"

#include <cerrno>
#include <cstdlib>

namespace {

bool ParsePositivePort(const char *input, int &port) {
    if (input == NULL || *input == '\0') {
        return false;
    }

    char *end = NULL;
    errno = 0;
    long parsed = strtol(input, &end, 10);
    if (errno != 0 || end == input || *end != '\0' || parsed < 1 || parsed > 65535) {
        return false;
    }

    port = static_cast<int>(parsed);
    return true;
}
}  // namespace

// Return the order of magnitude of a number
double monitor::GetOrder(unsigned int base, unsigned int &power) {
    double test = 1;
    for (unsigned int i = 0; i < 100; i++) {
        if (base / test <= 1) {
            power = i;
            return test;
        }
        test *= 10.0;
    }
    return 1;
}

// Expects input rate in Hz
std::pair<std::string, monitor::ColorCode> monitor::GetChanRateString(double chanRateInput, const colorThresholds &cThresh) {
    if (chanRateInput < 0.0) {
        chanRateInput *= -1;
    }
    int power = std::log10(chanRateInput);
    auto colorToUse = FG_DEFAULT;
    std::stringstream stream;
    stream << std::setprecision(2) << std::fixed;
    if (power >= 6) {
        stream << chanRateInput / 1E6 << "M";
    }  // MHz
    else if (power >= 3) {
        stream << chanRateInput / 1E3 << "k";
    }  // kHz
    else if (chanRateInput == 0) {
        stream << "   0 ";

    } else {
        stream << chanRateInput << " ";
    }  // Hz

    std::string output = stream.str();

    output = output.substr(0, output.find_last_not_of(".", 3) + 1) + output.substr(output.length() - 1, 1);

    colorToUse = GetColorFromThresholds(chanRateInput, cThresh);

    return std::make_pair(output, colorToUse);
}

std::pair<std::string, monitor::ColorCode> monitor::GetChanTotalString(unsigned int chanTotalString) {
    std::stringstream stream;
    unsigned int power = 0;
    double order = GetOrder(chanTotalString, power);

    if (power >= 4) {
        stream << 10 * chanTotalString / order;
    } else {
        stream << chanTotalString;
    }

    // Limit to 2 decimal place due to space constraints
    std::string output = stream.str();
    size_t find_index = output.find('.');
    if (find_index != std::string::npos) {
        std::string temp;
        temp = output.substr(0, find_index);
        temp += output.substr(find_index, 3);
        output = temp;
    }

    if (power >= 4) {
        std::stringstream stream2;
        stream2 << output << "E" << power - 1;
        output = stream2.str();
    }

    return std::make_pair(output, FG_DEFAULT);
}

// Expects input rate in B/s
std::string monitor::GetRateString(double p2DataRate, const bool &useColorOut) {
    if (p2DataRate < 0.0 && p2DataRate != -9999) {
        p2DataRate *= -1;
    }

    std::stringstream stream;
    if (p2DataRate == -9999) {
        stream << p2DataRate;
    } else {
        if (p2DataRate / GIGABYTE > 1) {
            stream << p2DataRate / GIGABYTE << " GB/s\n";
        }  // GB/s
        else if (p2DataRate / MEGABYTE > 1) {
            stream << p2DataRate / MEGABYTE << " MB/s\n";
        }  // MB/s
        else if (p2DataRate / KILOBYTE > 1) {
            stream << p2DataRate / KILOBYTE << " kB/s\n";
        }  // kB/s
        else {
            stream << p2DataRate << " B/s\n";
        }  // B/s
    }

    return stream.str();
}

// Expects input time in seconds
std::string monitor::GetTimeString(double input_) {
    if (input_ < 0.0) {
        input_ *= -1;
    }

    long long time = (long long)input_;
    int hr = time / 3600;
    int min = (time % 3600) / 60;
    int sec = (time % 3600) % 60;
    int rem = (int)(100 * (input_ - time));

    std::stringstream stream;
    if (hr < 10) {
        stream << "0" << hr;
    } else {
        stream << hr;
    }
    stream << ":";
    if (min < 10) {
        stream << "0" << min;
    } else {
        stream << min;
    }
    stream << ":";
    if (sec < 10) {
        stream << "0" << sec;
    } else {
        stream << sec;
    }
    stream << ".";
    if (rem < 10) {
        stream << "0" << rem;
    } else {
        stream << rem;
    }

    return stream.str();
}

int monitor::ParseCliFlags(int &argc, char *argv[], monitor *obj) {
    struct option longOpts[] = {
        {"color", optional_argument, NULL, 'c'},
        {"grafana", no_argument, NULL, 'g'},
        {"prometheus-port", required_argument, NULL, 'p'},
        {"prometheus-address", required_argument, NULL, 'a'},
        {"rows", required_argument, NULL, 'r'},
        {"socket", required_argument, NULL, 's'},
        {"help", no_argument, NULL, 'h'},
        {"dummy", no_argument, NULL, 'd'},
        {"?", no_argument, NULL, 0},
        {NULL, no_argument, NULL, 0}};
    int idx = 0;
    int retval = 0;
    bool overrodePrometheusOptions = false;
    while ((retval = getopt_long(argc, argv, "c::gp:a:r:s:hd", longOpts, &idx)) != -1) {
        switch (retval) {
            case 'd':
                if (strcmp(obj->GetName(), "submonitor") == 0) {
                    std::cout << " Submonitor does not support DummyMode" << std::endl;
                    return 1;
                }
                obj->SetDummyMode(true);
                break;
            case 'c':
                if (optarg == NULL && optind < argc && argv[optind][0] != '-') {
                    optarg = argv[optind++];
                }
                if (optarg != NULL) {
                    obj->SetColorThreshGroup(optarg);
                }
                obj->SetColorOut(1);
                break;
            case 'g':
                if (strcmp(obj->GetName(), "mainmonitor") != 0) {
                    std::cout << " Grafana export is only supported from mainmonitor" << std::endl;
                    return 1;
                }
                obj->SetGrafanaEnabled(true);
                break;
            case 'p': {
                if (strcmp(obj->GetName(), "mainmonitor") != 0) {
                    std::cout << " Prometheus export is only supported from mainmonitor" << std::endl;
                    return 1;
                }

                int port = 0;
                if (!ParsePositivePort(optarg, port)) {
                    std::cout << " Invalid Prometheus port: " << (optarg ? optarg : "(null)") << std::endl;
                    return 1;
                }

                obj->SetPrometheusPort(port);
                overrodePrometheusOptions = true;
                break;
            }
            case 'a':
                if (strcmp(obj->GetName(), "mainmonitor") != 0) {
                    std::cout << " Prometheus export is only supported from mainmonitor" << std::endl;
                    return 1;
                }
                if (optarg == NULL || *optarg == '\0') {
                    std::cout << " Prometheus bind address cannot be empty" << std::endl;
                    return 1;
                }
                obj->SetPrometheusBindAddress(optarg);
                overrodePrometheusOptions = true;
                break;
            case 's':
                if (strcmp(obj->GetName(), "mainmonitor") == 0) {
                    std::cout << " MainMonitor does not support the socket flag" << std::endl;
                    return 1;
                }
                if ((atoi(optarg) - obj->GetPredefinedPoll2Port()) > 0 && (atoi(optarg) - obj->GetPredefinedPoll2Port()) <= obj->GetMaxNumSubMonitors()) {
                    obj->SetSocketToUse(atoi(optarg));
                    break;
                }
                std::cout << " Unavailable socket:: Pick another" << std::endl;
                return 1;
            case 'r':
                if (optarg == NULL && optind < argc && argv[optind][0] != '-') {
                    optarg = argv[optind++];
                }
                if (optarg != NULL) {
                    obj->SetNumberOfRowsForModuleList(atoi(optarg));
                }
                break;
            case 'h':
                obj->help(obj->GetName());
                return 2;
            case '?':
                obj->help(obj->GetName());
                return 2;
            default:
                break;
        };
    };
    if (!GetGrafanaEnabled()) {
        if( overrodePrometheusOptions ) {
            std::cout << " Prometheus options are only valid if Grafana export is enabled" << std::endl;
            return 1;
        }
    }
    return 0;
}

/* Print help dialogue for command line options. */
void monitor::help(const char *progName) {
    std::cout << "\n SYNTAX: " << progName << " [options]\n";
    std::cout << "  --color (-c)          | Use Colors\n";
    if (strcmp(progName, "submonitor") == 0) {
        int minport = PREDEFINED_POLL2_PORT + 1;
        int maxPort = PREDEFINED_POLL2_PORT + MAX_NUM_SUBMONITORS;
        std::cout << "  --socket (-s)         | Submonitor socket to use (Current range is " 
        << std::to_string(minport) << "-" << std::to_string(maxPort) << ")\n";
    } else if (strcmp(progName, "mainmonitor") == 0) {
        std::cout << "  --grafana (-g)        | Enable Grafana metrics export\n";
        std::cout << "  --prometheus-port (-p)    | Prometheus listen port (default 9101)\n";
        std::cout << "  --prometheus-address (-a)    | Prometheus bind address (default localhost)\n";
    }
    std::cout << "  --rows (-r)           | Number of rows\n";
    std::cout << "  --help (-h)           | Display this help dialogue.\n\n";
}

void monitor::DecodeUdpMsg(char *ptr, poll2_UDP_msg &ret, int &num_modules, bool &first_packet) {
    if (first_packet) {
        ret.Data = new double *[num_modules];
        ret.ICR = new double *[num_modules];
        ret.OCR = new double *[num_modules];
        ret.Totals = new unsigned int *[num_modules];
        for (int i = 0; i < num_modules; i++) {
            ret.Data[i] = new double[16];
            ret.ICR[i] = new double[16];
            ret.OCR[i] = new double[16];
            ret.Totals[i] = new unsigned int[16];
        }
        first_packet = false;
    }

    memcpy(&ret.time_in_sec, ptr, 8);
    ptr += 8;
    memcpy(&ret.data_rate, ptr, 8);
    ptr += 8;
    for (int i = 0; i < num_modules; i++) {
        for (int j = 0; j < 16; j++) {
            memcpy(&ret.ICR[i][j], ptr, 8);
            ptr += 8;
            memcpy(&ret.OCR[i][j], ptr, 8);
            ptr += 8;
            memcpy(&ret.Data[i][j], ptr, 8);
            ptr += 8;
            memcpy(&ret.Totals[i][j], ptr, 4);
            ptr += 4;
        }
    }
}

void monitor::DecodeUdpMsg(poll2_UDP_msg &ret, int &num_modules, bool &first_packet, std::vector<std::pair<int, int>> &deadChan) {
    if (first_packet) {
        ret.Data = new double *[num_modules];
        ret.ICR = new double *[num_modules];
        ret.OCR = new double *[num_modules];
        ret.Totals = new unsigned int *[num_modules];
        for (int i = 0; i < num_modules; i++) {
            ret.Data[i] = new double[16];
            ret.ICR[i] = new double[16];
            ret.OCR[i] = new double[16];
            ret.Totals[i] = new unsigned int[16];
        }
        first_packet = false;
    };

    for (int modIT = 0; modIT < num_modules; ++modIT) {
        for (int chanIT = 0; chanIT < 16; ++chanIT) {
            bool breakout_cuz_dead = false;
            for (unsigned deads = 0; deads < deadChan.size(); ++deads) {
                if (chanIT == deadChan.at(deads).second && modIT == deadChan.at(deads).first) {
                    breakout_cuz_dead = true;
                    continue;
                }
            }
            if (breakout_cuz_dead) {
                continue;
            }

            int rand1 = rand() % 10;
            int rand2 = 1 + rand() % 6;
            std::pair<int, int> randoms = std::make_pair(rand1, rand2);  // random dummy rate, o->9 base and 0->3 "E" power
            //! for the sake of speed we will use the same "randoms" for ICR, OCR, and DATA, while totals witll just sum data over time
            ret.ICR[modIT][chanIT] = randoms.first * std::pow(10, randoms.second);
            ret.OCR[modIT][chanIT] = randoms.first * std::pow(10, randoms.second);
            ret.Data[modIT][chanIT] = randoms.first * std::pow(10, randoms.second);
            double div = 1;
            if (randoms.second >= 2 && randoms.second < 4) {
                div = 2;
            } else if (randoms.second > 4 && randoms.second <= 6) {
                div = 4;
            }

            ret.Totals[modIT][chanIT] += (unsigned)((randoms.first * std::pow(10, randoms.second)) / div);
            ;
        }
    }
    ret.time_in_sec = 1;
    ret.data_rate = -9999;
}

void monitor::SetColorThresholdStruct(monitor::colorThresholds &cThresh, const std::string &rateGroup = "defaultRateGroup") {
    if (strcmp(rateGroup.c_str(), "defaultRateGroup") == 0) {
        cThresh.crit_high = 20000;
        cThresh.warn_high = 8000;
        cThresh.warn_low = 1;
        cThresh.crit_low = 0.5;
        cThresh.disabled = 0.0;
    } else if (strcmp(rateGroup.c_str(), "mtas") == 0) {
        cThresh.crit_high = 50000;
        cThresh.warn_high = 2000;
        cThresh.warn_low = 10;
        cThresh.crit_low = 5;
        cThresh.disabled = 0.0;
    } else if (strcmp(rateGroup.c_str(), "vandle") == 0) {
        cThresh.crit_high = 50000;
        cThresh.warn_high = 2000;
        cThresh.warn_low = 10;
        cThresh.crit_low = 5;
        cThresh.disabled = 0.0;
    } else if (strcmp(rateGroup.c_str(), "nshe") == 0) {
        cThresh.crit_high = 500;
        cThresh.warn_high = 200;
        cThresh.warn_low = 0.5;
        cThresh.crit_low = 0.2;
        cThresh.disabled = 0.0;
    } else {
        cThresh.crit_high = 20000;
        cThresh.warn_high = 8000;
        cThresh.warn_low = 1;
        cThresh.crit_low = 0.5;
        cThresh.disabled = 0.0;
    };
}

monitor::ColorCode monitor::GetColorFromThresholds(const double &input, const colorThresholds &cThresh) {
    if (input == -999) {
        return monitor::FG_BLACK;
    } else if (input <= cThresh.crit_low) {
        return monitor::FG_BLUE;
    } else if (input <= cThresh.warn_low && input > cThresh.crit_low) {
        return monitor::FG_MAGENTA;
    } else if (input <= cThresh.warn_high && input > cThresh.warn_low) {
        return monitor::FG_DEFAULT;
    } else if (input <= cThresh.crit_high && input > cThresh.warn_high) {
        return monitor::FG_YELLOW;
    } else if (input > cThresh.crit_high) {
        return monitor::FG_RED;
    } else {
        return monitor::FG_DEFAULT;
    }
}
