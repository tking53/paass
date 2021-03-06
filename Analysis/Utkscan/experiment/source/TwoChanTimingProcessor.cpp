/** \file TwoChanTimingProcessor.cpp
 * \brief Analyzes data from a simple Two channel Timing setup
 * \author S. V. Paulauskas
 * \date July 10, 2009
 */
#include <fstream>

#include "DammPlotIds.hpp"
#include "Globals.hpp"
#include "HighResTimingData.hpp"
#include "RawEvent.hpp"
#include "TwoChanTimingProcessor.hpp"

#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>
#include <TH2I.h>

static HighResTimingData::HrtRoot rstart;
static HighResTimingData::HrtRoot rstop;

TFile *rootfile;
TTree *tree;
TH1I *codes;
TH2I *startTraces;
TH2I *startAmpDistribution;
TH2I *stopTraces;
TH2I *stopAmpDistribution;

enum CODES {
    PROCESS_CALLED,
    WRONG_NUM
};

namespace dammIds{
  namespace experiment{
    const int DD_PHASEPHASE = 0;
  }
}

using namespace std;
using namespace dammIds::experiment;

TwoChanTimingProcessor::TwoChanTimingProcessor() :
        EventProcessor(OFFSET, RANGE, "TwoChanTimingProcessor") {
    associatedTypes.insert("pulser");

    rootfile = new TFile(Globals::get()->AppendOutputPath(Globals::get()->GetOutputFileName() + ".root").c_str(), "RECREATE");

    tree = new TTree("timing", "");
    tree->Branch("start", &rstart, "qdc/D:time:snr:wtime:phase:abase:sbase:id/b");
    tree->Branch("stop", &rstop, "qdc/D:time:snr:wtime:phase:abase:sbase:id/b");
    codes = new TH1I("codes", "", 20, 0, 20);
    startTraces = new TH2I("startTraces", "", 250, 0, 250, 2000, 0, 2000);
    stopTraces = new TH2I("stopTraces", "", 250, 0, 250, 2000, 0, 2000);
    startAmpDistribution = new TH2I("startAmpDistribution", "", 250, 0, 250, 16384, 0, 16384);
    stopAmpDistribution = new TH2I("stopAmpDistribution", "", 250, 0, 250, 16384, 0, 16384);
}

TwoChanTimingProcessor::~TwoChanTimingProcessor() {
    codes->Write();
    rootfile->Write();
    rootfile->Close();
}

void TwoChanTimingProcessor::DeclarePlots(void){
  DeclareHistogram2D(DD_PHASEPHASE,SD,SD,"Start Phase vs Stop Phase");
}

bool TwoChanTimingProcessor::Process(RawEvent &event) {
    if (!EventProcessor::Process(event))
        return false;

    //Define a map to hold the information
    TimingMap pulserMap;

    //plot the number of times we called the function
    codes->Fill(PROCESS_CALLED);

    static const vector<ChanEvent *> &pulserEvents =
            event.GetSummary("pulser")->GetList();

    for (vector<ChanEvent *>::const_iterator itPulser = pulserEvents.begin();
         itPulser != pulserEvents.end(); itPulser++) {
        int location = (*itPulser)->GetChanID().GetLocation();
        string subType = (*itPulser)->GetChanID().GetSubtype();

        TimingDefs::TimingIdentifier key(location, subType);
        pulserMap.insert(make_pair(key, HighResTimingData(*(*itPulser))));
    }

    if (pulserMap.empty() || pulserMap.size() % 2 != 0) {
        //If the map is empty or size isn't even we return and increment
        // error code
        codes->Fill(WRONG_NUM);
        EndProcess();
        return false;
    }

    HighResTimingData start =
            (*pulserMap.find(make_pair(0, "start"))).second;
    HighResTimingData stop =
            (*pulserMap.find(make_pair(0, "stop"))).second;

    static int trcCounter = 0;
    int bin = 0;
    for (vector<unsigned int>::const_iterator it = start.GetTrace().begin();
         it != start.GetTrace().end(); it++) {
        bin = (int) (it - start.GetTrace().begin());
        startTraces->Fill(bin, trcCounter, *it);
        startAmpDistribution->Fill(bin, *it);
    }

    for (vector<unsigned int>::const_iterator it = stop.GetTrace().begin();
         it != stop.GetTrace().end(); it++) {
        bin = (int) (it - stop.GetTrace().begin());
        stopTraces->Fill(bin, trcCounter, *it);
        stopAmpDistribution->Fill(bin, *it);
    }

    trcCounter++;

    //We only plot and analyze the data if the data is validated
    if (start.GetIsValid() && stop.GetIsValid()) {
        start.FillRootStructure(rstart);
        stop.FillRootStructure(rstop);
        tree->Fill();
        start.ZeroRootStructure(rstart);
        stop.ZeroRootStructure(rstop);

	//damm plots here
	plot(DD_PHASEPHASE,start.GetPhaseInNs(),stop.GetPhaseInNs());
    }
    EndProcess();
    return true;
}
