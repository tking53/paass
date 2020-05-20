/** \file CfdAnalyzer.cpp
 * \brief Uses a Digital CFD to obtain waveform phases
 *
 * This code will obtain the phase of a waveform using a digital CFD.
 * Currently the only method is a polynomial fit to the crossing point.
 * For 100-250 MHz systems, this is not going to produce good timing.
 * This code was originally written by S. Padgett.
 *
 * \author S. V. Paulauskas
 * \date 22 July 2011
 */
#include <iostream>
#include <vector>
#include <utility>

#include "CfdAnalyzer.hpp"
#include "PixieCfd.hpp"
#include "PolynomialCfd.hpp"
#include "TraditionalCfd.hpp"

using namespace std;

CfdAnalyzer::CfdAnalyzer(const std::string &s, const int &ptype, const std::set<std::string> &ignoredTypes) : TraceAnalyzer() {
    name = "CfdAnalyzer";
    if (s == "polynomial" || s == "poly"){
        driver_ = new PolynomialCfd(ptype);
        tuplePars_ = false;
    }else if (s == "traditional" || s == "trad"){
        driver_ = new TraditionalCfd();
        tuplePars_ = false;
    }else if (s == "pixie" || s == "xia"){
        cout<<"Creating PixieCfd"<<endl;
        driver_ = new PixieCfd(); 
        tuplePars_ = true;
    }else {
        driver_ = NULL;
    }
    ignoredTypes_ = ignoredTypes;
}

void CfdAnalyzer::Analyze(Trace &trace, const ChannelConfiguration &cfg) {
    TraceAnalyzer::Analyze(trace, cfg);

    if (!driver_) {
        EndAnalyze();
        return;
    }
     //non destructive return if we have already done timing analysis on this channel
    if (trace.HasValidTimingAnalysis()){
        return;
    }

    if (trace.IsSaturated() || trace.empty() || !trace.HasValidWaveformAnalysis() || IsIgnored(ignoredTypes_,cfg)) {
        trace.SetHasValidTimingAnalysis(false);
        EndAnalyze();
        return;
    }


    const map<string,double> pars = cfg.GetCfdParameters();

    double phase = -9999;
    if (tuplePars_){
        phase = driver_->CalculatePhase(trace.GetTraceSansBaseline(), pars, make_pair(cfg.GetTraceDelayInSamples(), cfg.GetWaveformBoundsInSamples().second), trace.GetBaselineInfo());
        if (phase == -1) {
            trace.SetPhase(0);
            trace.SetHasValidTimingAnalysis(false);
        } else {
            trace.SetPhase(phase);
            trace.SetHasValidTimingAnalysis(true);
        }
    } else { 
        phase = driver_->CalculatePhase(trace.GetTraceSansBaseline(), pars, trace.GetExtrapolatedMaxInfo(), trace.GetBaselineInfo());
        trace.SetPhase(phase);
        trace.SetHasValidTimingAnalysis(true);
    }

   
    EndAnalyze();
}
