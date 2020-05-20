///@file PixieCfd.cpp
///@brief Same CFD algorithm implemented by Xia LLC but offline. Based on XiaCfd but rewritten for modern PAASS 
///@author T. T. King,  S. V. Paulauskas
///@date Feb 06, 2020

#include "HelperFunctions.hpp" 
#include "PixieCfd.hpp"

using std::map;
using std::vector;
using std::pair;

/// Perform CFD analysis on the waveform
double PixieCfd::CalculatePhase(const std::vector<double> &data,
                          const std::map<string, double> &pars,
                          const std::pair<unsigned int, unsigned int> &range,
                          const std::pair<double, double> baseline) {

    // Check if we passed an empty trace by accident (should be caught before we get to here)
    // also check if the avg baseline is negative (implies a very malformed trace capture)
    if (data.size() == 0 || baseline.first < 0){
        return -9999;
    }

    double thresh = pars.find("cfdT")->second;
    double delay = pars.find("cfdD")->second;
    double scale = pars.find("cfdL")->second;

    unsigned int TraceDelay = range.first;
    unsigned int WaveformRangeHigh = range.second;
    vector<double> cfdTrace (data.size(),0.0);

    for (unsigned int index=0; index < TraceDelay + WaveformRangeHigh ; ++index){
        if (index+delay < data.size() ){
            cfdTrace.at(index) = data.at(index) - (data.at(index+delay) *scale  )  ;
        } else {
            cfdTrace.at(index) = 0 ;
        }
    }
    //Find that zero 
    pair<int,int> stradlePoints = {0,0};
    bool looking = false;
    for (unsigned int it =0; it < TraceDelay + WaveformRangeHigh ;++it){
        if (cfdTrace.at(it) < thresh * baseline.second){
            looking = true;
            continue;  
        }else if(looking){
            if (cfdTrace.at(it) > 0){
                stradlePoints.second = it;
                stradlePoints.first = it - 1;
                break;
            }
        }
    }
    if (stradlePoints.first != 0 && stradlePoints.second != 0) {
        double slope = (cfdTrace.at(stradlePoints.second) - cfdTrace.at(stradlePoints.first)) / (stradlePoints.second - stradlePoints.first);
        double intercept = cfdTrace.at(stradlePoints.second) - slope * stradlePoints.second;
        double phase = (-1 * intercept) / slope;

        return phase;
    } else {
        return -1;
    }
} //end CalculatePhase()
