/** \file DoubleBetaProcessor.cpp
 *\brief A DoubleBeta processor class that can be used to analyze double
 * beta detectors.
 *\author S. V. Paulauskas
 *\date October 26, 2014
 */
#include "BarBuilder.hpp"
#include "DammPlotIds.hpp"
#include "DoubleBetaProcessor.hpp"
#include "Globals.hpp"
#include "RawEvent.hpp"
#include "TimingMapBuilder.hpp"
#include "TreeCorrelator.hpp"

namespace dammIds {
    namespace doublebeta {
        const int DD_SINGLESQDC = 0;//!< ID for the singles QDC
        const int DD_QDC  = 1; //!< ID for the Bar QDC of the double beta detector
        const int DD_TDIFF = 2;//!< ID to plot the Time Difference between ends
        const int DD_PP = 3;//!< ID to plot the phase-phase for your favorite bar (0)
        const int DD_QDCTDIFF = 5;//!< QDC vs. TDiff for your favorite bar (0)
        const int DD_TRACESLL = 6; //!<Lwing Traces  Left Array
        const int DD_TRACESLR = 7; //!<Lwing Traces Right Array
        const int DD_TRACESCL = 8; //!<Center Traces  Left Array
        const int DD_TRACESCR = 9; //!<Center Traces Right Array
        const int DD_TRACESRL = 10; //!<Rwing Traces  Left Array
        const int DD_TRACESRR = 11; //!<Rwing Traces Right Array
    }
}

using namespace std;
using namespace dammIds::doublebeta;



void DoubleBetaProcessor::rootBWaveInit(BWave &strutName) { //Zeros the WaveForm Structures
    fill(strutName.Ltrace, strutName.Ltrace + 131, 0);
    fill(strutName.Rtrace, strutName.Rtrace + 131, 0);
    strutName.Lbaseline = -999;
    strutName.Rbaseline = -999;
    strutName.LmaxLoc = -999;
    strutName.RmaxLoc = -999;
    strutName.Lamp = -999;
    strutName.Ramp = -999;
    strutName.BarQdc = -999;
    strutName.Rsnr = -999;
    strutName.Lsnr = -999;
    strutName.Lqdc =-999;
    strutName.Rqdc = -999;
    strutName.Rphase = -999999;
    strutName.Lphase = -999999;
    strutName.Tdiff = -99999;
    strutName.TdiffD = -99999;
}

DoubleBetaProcessor::DoubleBetaProcessor():
    EventProcessor(OFFSET, RANGE, "DoubleBetaProcessor") {
    associatedTypes.insert("beta");

    string hisfilename = Globals::get()->outputFile();
    string rootname3 = hisfilename + "-Bwave.root";

    rootFName3_ = new TFile(rootname3.c_str(),"RECREATE");
    BWaveT = new TTree("Wave","Tree for Waveform Analyzer Debugging");
    BwaveBranch = BWaveT->Branch("Bwave",&Bwave,"Ltrace[131]/D:Rtrace[131]:Lbaseline:Rbaseline:LmaxLoc:RmaxLoc:Lamp:Ramp:BarQdc:Lsnr:Rsnr:Lqdc:Rqdc:Lphase:Rphase:Tdiff:TdiffD");
    CenterL = new TH2D("CenterL","Center Spike Left Array",1000,-100,900,4096,0,4096);
    CenterR = new TH2D("CenterR","Center Spike Right Array",1000,-100,900,4096,0,4096);
    LeftL = new TH2D("LeftL","Left Spike Left Array",1000,-100,900,4096,0,4096);
    LeftR = new TH2D("LeftR","Left Spike Right Array",1000,-100,900,4096,0,4096);
    RightL = new TH2D("RightL","Right Spike Left Array",1000,-100,900,4096,0,4096);
    RightR = new TH2D("RightR","Right Spike Right Array",1000,-100,900,4096,0,4096);

    rootBWaveInit(Bwave);
}

void DoubleBetaProcessor::DeclarePlots(void) {
    DeclareHistogram2D(DD_QDC, SD, S3, "Location vs. Coincident QDC");
    DeclareHistogram2D(DD_TDIFF, SB, S3, "Location vs. Time Difference");
    DeclareHistogram2D(DD_PP, SC, SC,"Phase vs. Phase - Bar 0 Only");
    DeclareHistogram2D(DD_QDCTDIFF, SC, SE,"TimeDiff vs. Coincident QDC");
    DeclareHistogram2D(DD_TRACESLL, S8, SC, "Left wing traces, Left");
    DeclareHistogram2D(DD_TRACESLR, S8, SC, "Left wing traces, Right");
    DeclareHistogram2D(DD_TRACESCL, S8, SC, "Center traces, Left");
    DeclareHistogram2D(DD_TRACESCR, S8, SC, "Center traces, Right");
    DeclareHistogram2D(DD_TRACESRL, S8, SC, "Right wing traces, Left");
    DeclareHistogram2D(DD_TRACESRR, S8, SC, "Right wing traces, Right");

}

bool DoubleBetaProcessor::PreProcess(RawEvent &event) {
    if (!EventProcessor::PreProcess(event))
        return (false);
    lrtbars_.clear();
    bars_.clear();


    static const vector<ChanEvent *> &events =
            event.GetSummary("beta:double")->GetList();

    BarBuilder builder(events);
    builder.BuildBars();

    lrtbars_ = builder.GetLrtBarMap();
    bars_ = builder.GetBarMap();

    double resolution = 2;
    double offset = 1500;

   // rootBWaveInit(Bwave);

    for (map < unsigned int, pair < double, double > > ::iterator it = lrtbars_.begin();
    it != lrtbars_.end();
    it++) {
        stringstream place;
        place << "DoubleBeta" << (*it).first;
        EventData data((*it).second.first, (*it).second.second, (*it).first);
        TreeCorrelator::get()->place(place.str())->activate(data);
    }

    for (BarMap::const_iterator it = bars_.begin(); it != bars_.end(); it++) {
        unsigned int barNum = (*it).first.first;
        plot(DD_QDC, (*it).second.GetLeftSide().GetTraceQdc(), barNum * 2);
        plot(DD_QDC, (*it).second.GetRightSide().GetTraceQdc(), barNum * 2 + 1);
        plot(DD_TDIFF, (*it).second.GetTimeDifference() * resolution + offset, barNum);
        if (barNum == 0) {
            plot(DD_PP, (*it).second.GetLeftSide().GetPhaseInNs() * resolution,
                 (*it).second.GetRightSide().GetPhaseInNs() * resolution);
            plot(DD_QDCTDIFF, (*it).second.GetTimeDifference() * resolution + offset,
                 (*it).second.GetQdc());
        }

        double tdiffDamm = (*it).second.GetTimeDifference() * resolution + offset;

        Bwave.Lbaseline = (*it).second.GetLeftSide().GetAveBaseline();
        Bwave.Rbaseline = (*it).second.GetRightSide().GetAveBaseline();
        Bwave.BarQdc = (*it).second.GetQdc();
        Bwave.LmaxLoc = (*it).second.GetLeftSide().GetMaximumPosition();
        Bwave.RmaxLoc = (*it).second.GetRightSide().GetMaximumPosition();
        Bwave.Lamp = (*it).second.GetLeftSide().GetMaximumValue();
        Bwave.Ramp = (*it).second.GetRightSide().GetMaximumValue();
        Bwave.Lsnr = (*it).second.GetLeftSide().GetTrace().GetSignalToNoiseRatio();
        Bwave.Rsnr = (*it).second.GetRightSide().GetTrace().GetSignalToNoiseRatio();
        Bwave.Lqdc = (*it).second.GetLeftSide().GetTraceQdc();
        Bwave.Rqdc = (*it).second.GetRightSide().GetTraceQdc();
        Bwave.Lphase = (*it).second.GetLeftSide().GetPhaseInNs();
        Bwave.Rphase = (*it).second.GetRightSide().GetPhaseInNs();
        Bwave.Tdiff = (*it).second.GetTimeDifference();
        Bwave.TdiffD = (*it).second.GetTimeDifference() * resolution + offset;

        int binL = 0;
        for (vector<unsigned int>::const_iterator itTL = (*it).second.GetLeftSide().GetTrace().begin();
             itTL != (*it).second.GetLeftSide().GetTrace().end(); itTL++) {
            Bwave.Ltrace[binL]=(*itTL);
            binL++;
        }

        int binR =0;
        for (vector<unsigned int>::const_iterator PosT = (*it).second.GetRightSide().GetTrace().begin();
             PosT != (*it).second.GetRightSide().GetTrace().end(); PosT++) {
            Bwave.Rtrace[binR]=(*PosT);
            binR++;
        }


        if ( tdiffDamm < 1350 && tdiffDamm >850 ){ //Left Wing

            for (vector<unsigned int>::const_iterator PosT = (*it).second.GetLeftSide().GetTrace().begin();
                 PosT != (*it).second.GetLeftSide().GetTrace().end(); PosT++) {
                double bin = PosT - (*it).second.GetLeftSide().GetTrace().begin();
                plot(DD_TRACESLL, bin, (*PosT));
                LeftL->Fill(bin,(*PosT));

            }
            for (vector<unsigned int>::const_iterator itTR = (*it).second.GetRightSide().GetTrace().begin();
                 itTR != (*it).second.GetRightSide().GetTrace().end(); itTR++) {
                double bin =  itTR - (*it).second.GetRightSide().GetTrace().begin();
                plot(DD_TRACESLR,bin,(*itTR));
                LeftR->Fill(bin,(*itTR));
            }
        }
        if ( tdiffDamm < 1620 && tdiffDamm > 1350 ) { //CenterWing
            for (vector<unsigned int>::const_iterator PosT = (*it).second.GetLeftSide().GetTrace().begin();
                 PosT != (*it).second.GetLeftSide().GetTrace().end(); PosT++) {
                double bin = PosT - (*it).second.GetLeftSide().GetTrace().begin();
                plot(DD_TRACESCL, bin, (*PosT));
                CenterL->Fill(bin, (*PosT));
            }
            for (vector<unsigned int>::const_iterator itTR = (*it).second.GetRightSide().GetTrace().begin();
                 itTR != (*it).second.GetRightSide().GetTrace().end(); itTR++) {
                double bin = itTR - (*it).second.GetRightSide().GetTrace().begin();
                plot(DD_TRACESCR, bin, (*itTR));
                CenterR->Fill(bin, (*itTR));
            }
        }
        if ( tdiffDamm < 2200 && tdiffDamm > 1620 ){ //RightWing
            for (vector<unsigned int>::const_iterator PosT = (*it).second.GetLeftSide().GetTrace().begin();
                 PosT != (*it).second.GetLeftSide().GetTrace().end(); PosT++) {
                double bin = PosT - (*it).second.GetLeftSide().GetTrace().begin();
                plot(DD_TRACESRL, bin, (*PosT));
                RightL->Fill(bin,(*PosT));
            }
            for (vector<unsigned int>::const_iterator itTR = (*it).second.GetRightSide().GetTrace().begin();
                 itTR != (*it).second.GetRightSide().GetTrace().end(); itTR++) {
                double bin = itTR - (*it).second.GetRightSide().GetTrace().begin();
                plot(DD_TRACESRR, bin, (*itTR));
                RightR->Fill(bin, (*itTR));
            }

        }


    }
    BWaveT->Fill();
    return (true);
}

bool DoubleBetaProcessor::Process(RawEvent &event) {
    if (!EventProcessor::Process(event))
        return(false);
    EndProcess();
    return(true);
}
