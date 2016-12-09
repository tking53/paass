/** \file FunixCollabProcessor.cpp
 * \brief Analyzes data from a simple Two channel Timing setup
 * \author S. V. Paulauskas
 * \date July 10, 2009
 */
#include <fstream>

#include "Globals.hpp"
#include "HighResTimingData.hpp"
#include "FunixCollabProcessor.hpp"

#include <TFile.h>
#include <TTree.h>
#include <TH1I.h>
#include <TH2I.h>

std::ofstream trcfile;
TFile *rootfile;
TTree *tree;
TH1I *codes;
TH2I *leftTraces;
TH2I *rightTraces;
TH1I *mult;

struct RootData {
    double time;
    double energy;
};

static RootData rleft;
static RootData rright;

enum CODES {
    PROCESS_CALLED,
    WRONG_NUM,
    HAD_LEFT,
    HAD_RIGHT
};

using namespace std;

FunixCollabProcessor::FunixCollabProcessor() :
        EventProcessor(0, 0, "FunixCollabProcessor") {
    associatedTypes.insert("pulser");
    rootfile = new TFile(Globals::get()->outputPath("test00.root").c_str(),
                         "RECREATE");
    tree = new TTree("timing", "");
    tree->Branch("left", &rleft, "time/D:energy");
    tree->Branch("right", &rright, "time/D:energy");
    codes = new TH1I("codes", "Status codes", 40, 0, 40);
    leftTraces = new TH2I("left_traces", "Traces for the left side", 1000, 0,
                          1000,
                          1000, 0, 1000);
    rightTraces = new TH2I("right_traces", "Traces for the right side", 1000, 0,
                           1000, 1000, 0, 1000);
    mult = new TH1I("mult", "Event Multiplicity", 30, 0, 30);
}

FunixCollabProcessor::~FunixCollabProcessor() {
    codes->Write();
    rootfile->Write();
    rootfile->Close();
}

void FunixCollabProcessor::DeclarePlots(void) {
}

bool FunixCollabProcessor::Process(RawEvent &event) {
    if (!EventProcessor::Process(event))
        return false;

    //plot the number of times we called the function
    codes->Fill(PROCESS_CALLED);

    static const vector<ChanEvent *> &pulserEvents =
            event.GetSummary("pulser")->GetList();

    mult->Fill(pulserEvents.size());

    if (pulserEvents.size() != 2) {
        codes->Fill(WRONG_NUM);
        EndProcess();
        return false;
    }

    static unsigned int left_counter;
    static unsigned int right_counter;
    for (vector<ChanEvent *>::const_iterator it = pulserEvents.begin();
         it != pulserEvents.end(); it++) {
        if ((*it)->GetChanID().GetSubtype() == "left") {
            codes->Fill(HAD_LEFT);
            rleft.energy = (*it)->GetEnergy();
            rleft.time = (*it)->GetTime();
            for (vector<int>::const_iterator trcIt = (*it)->GetTrace().begin();
                 trcIt != (*it)->GetTrace().end(); trcIt++) {
                int bin = (int) (trcIt - (*it)->GetTrace().begin());
                leftTraces->Fill(bin, left_counter, *trcIt);
            }
            left_counter++;
        }
        if ((*it)->GetChanID().GetSubtype() == "right") {
            rright.energy = (*it)->GetEnergy();
            rright.time = (*it)->GetTime();
            codes->Fill(HAD_RIGHT);
            for (vector<int>::const_iterator trcIt = (*it)->GetTrace().begin();
                 trcIt != (*it)->GetTrace().end(); trcIt++) {
                int bin = (int) (trcIt - (*it)->GetTrace().begin());
                rightTraces->Fill(bin, right_counter, *trcIt);
            }
            right_counter++;
        }
    }

    tree->Fill();

    rleft.energy = rright.energy = rleft.time = rright.time = 0.0;

    EndProcess();
    return true;
}