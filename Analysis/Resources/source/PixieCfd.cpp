///@file PixieCfd.cpp
///@brief Same CFD algorithm implemented by Xia LLC but offline. Based on XiaCfd but rewritten for modern PAASS 
///@author T. T. King,  S. V. Paulauskas
///@date Feb 06, 2020

#include "HelperFunctions.hpp" 
#include "PixieCfd.hpp"

using namespace std;
/// Perform CFD analysis on the waveform
double PixieCfd::CalculatePhase(const std::vector<double> &data,
                          const std::tuple<double, double, double> &pars,
                          const std::pair<unsigned int, unsigned int> &range,
                          const std::pair<double, double> baseline) {

    // Check if we passed an empty trace by accident (should be caught before we get to here)
    // also check if the avg baseline is negative (implies a very malformed trace capture)
    if (data.size() == 0 || baseline.first < 0){
        return -9999;
    }

    double thresh = get<0>(pars);
    double delay = get<1>(pars);
    double scale = get<2>(pars);
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
/* OLD Copy over from XiaCfd.cpp
    if (size == 0 || baseline < 0) { return -9999; }
    if (!cfdvals)
        cfdvals = new double[size];

    double cfdMinimum = 9999;
    size_t cfdMinIndex = 0;

    phase = -9999;

    // Compute the cfd waveform.
    for (size_t cfdIndex = 0; cfdIndex < size; ++cfdIndex) {
        cfdvals[cfdIndex] = 0.0;
        if (cfdIndex >= L_ + D_ - 1) {
            for (size_t i = 0; i < L_; i++)
                cfdvals[cfdIndex] +=
                        F_ * (event->adcTrace[cfdIndex - i] - baseline) -
                        (event->adcTrace[cfdIndex - i - D_] - baseline);
        }
        if (cfdvals[cfdIndex] < cfdMinimum) {
            cfdMinimum = cfdvals[cfdIndex];
            cfdMinIndex = cfdIndex;
        }
    }

    // Find the zero-crossing.
    if (cfdMinIndex > 0) {
        // Find the zero-crossing.
        for (size_t cfdIndex = cfdMinIndex - 1; cfdIndex >= 0; cfdIndex--) {
            if (cfdvals[cfdIndex] >= 0.0 && cfdvals[cfdIndex + 1] < 0.0) {
                phase = cfdIndex - cfdvals[cfdIndex] /
                                   (cfdvals[cfdIndex + 1] - cfdvals[cfdIndex]);
                break;
            }
        }
    }

    return phase;
}
*/
