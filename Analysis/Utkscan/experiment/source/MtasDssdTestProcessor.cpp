/** @file MtasDssdTestProcessor.cpp
 *  @brief Testing MTAS DSSD processors
 *  @authors T.T. King
 *  @date 12/01/2021
*/

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "DammPlotIds.hpp"
#include "MtasDssdTestProcessor.hpp"



namespace dammIds {
    namespace mtasdssd {
        const int DD_Multiplicties = 0; 

        const unsigned int DSSDNumber_offset = 10 ;
        const unsigned int Uncalibrated_Offset = 100;

        const int DD_F_Strip_Vs_StripEnergy = 0;
        const int DD_B_HG_Strip_Vs_StripEnergy = 1;
        const int DD_B_LG_Strip_Vs_StripEnergy = 2;
        const int DD_FBImage = 3;
    }
}
using namespace std;
using namespace dammIds::mtasdssd;
    

MtasDssdTestProcessor::MtasDssdTestProcessor(const int &numDSSDs, const double &res) : EventProcessor() {
    associatedTypes.insert("mtasdssd");
    numberOfDssds = numDSSDs;
    plotRes = res;
}

bool MtasDssdTestProcessor::PreProcess(RawEvent &event) {
    if (!EventProcessor::PreProcess(event)){
        return false;
    }
    return true;
}
void MtasDssdTestProcessor::DeclarePlots(void) {

    const int energyBins = SE;
    // const int energyBins2 = SB;
    const int fBins = S6;
    const int bBins = S7;

    DeclareHistogram2D(DD_Multiplicties, S7, S3, "Multi plots F/BHG/BLG");

    // Starting from 1 because of the DSSD naming convention set by Krzysztof
    for (int curDSSD = 1; curDSSD <= numberOfDssds; ++curDSSD) {
        string DSSDprefix = "DSSD" + to_string(curDSSD);
        int dssd_offset = ReturnPlottingOffsets(curDSSD);

        // Uncalibrated Energy Based Histograms
        DeclareHistogram2D(DD_F_Strip_Vs_StripEnergy + dssd_offset + Uncalibrated_Offset , fBins, energyBins,(DSSDprefix + " Front strip vs Raw Energy/10").c_str());
        DeclareHistogram2D(DD_B_HG_Strip_Vs_StripEnergy + dssd_offset + Uncalibrated_Offset , bBins, energyBins,(DSSDprefix + " Back HG strip vs Raw Energy/10").c_str());
        DeclareHistogram2D(DD_B_LG_Strip_Vs_StripEnergy + dssd_offset + Uncalibrated_Offset , bBins, energyBins,(DSSDprefix + " Back LG strip vs Raw Energy/10").c_str());

        // Calibrated Energy Based Histrogramss
        DeclareHistogram2D(DD_F_Strip_Vs_StripEnergy + dssd_offset, fBins, energyBins, (DSSDprefix + " Front strip vs Cal Energy/10").c_str());
        DeclareHistogram2D(DD_B_HG_Strip_Vs_StripEnergy + dssd_offset, bBins, energyBins, (DSSDprefix + " Back HG strip vs Cal Energy/10").c_str());
        DeclareHistogram2D(DD_B_LG_Strip_Vs_StripEnergy + dssd_offset, bBins, energyBins, (DSSDprefix + " Back LG strip vs Cal Energy/10").c_str());
    }
}

bool MtasDssdTestProcessor::Process(RawEvent &event) {
    if (!EventProcessor::Process(event)){
        return false;
    }

    static const auto &Events = event.GetSummary("mtasdssd", true)->GetList();
    vector<multipliciesStruc> multiContainer_(numberOfDssds,multipliciesStrucDefault);
    

    for (auto it = Events.begin(); it != Events.end(); ++it) {

        string DSSDNumberString_(1,(*it)->GetChanID().GetSubtype().back());

        //! Check that we set the subtype right (as "dssd1"/"dssd2"). The last char of the subtype is understood to be the dssd number 
        if (!(DSSDNumberString_.find_first_not_of( "0123456789" ) == std::string::npos)){
            cout<<"ERROR  MtasDssdTestProcessor::Process Last Char of subtype != a number for Mod:Channel "<< (*it)->GetModuleNumber() << ":" << (*it)->GetChannelNumber() <<". Please fix XML" <<endl;
            return false;
        }

        bool isFront_ = false, isBack_ = false;
        bool isDSSD_1_ = false, isDSSD_2_ = false;
        bool isLowGain_ = false, isHighGain_ = false;

        int DSSDNumber_ = stoi(DSSDNumberString_);

        int stripNumber_ = (*it)->GetChanID().GetLocation();  //! NOTE::: This is dependent on the type:subtype  parsing order from the XML. Which is useful for testing, but we need a better way here. Maybe move is dssd1 or dssd2 up to subtype, and front, back as tags then we can use group for strip number? but we also need to split HG vs LG for the back which also should go in tags

        if (DSSDNumber_ == 1){
            isDSSD_1_ = true;
        } else if (DSSDNumber_ == 2){
            isDSSD_2_ = true;
        }


        if ((*it)->GetChanID().HasTag("front")){
            isFront_ = true;
            multiContainer_.at(DSSDNumber_).numberOfFires_f_++;
        } else if ((*it)->GetChanID().HasTag("back") && (*it)->GetChanID().HasTag("lowGain")){
            isBack_= true;
            isLowGain_ = true;
            multiContainer_.at(DSSDNumber_).numberOfFires_blg_++;
        }else if ((*it)->GetChanID().HasTag("back") && (*it)->GetChanID().HasTag("highGain")){
            isBack_= true;
            isHighGain_ = true;
            multiContainer_.at(DSSDNumber_).numberOfFires_bhg_++;
        }

        int DSSD_Plotting_Offset_ = ReturnPlottingOffsets(DSSDNumber_);

        double calStripEnergy_ = (*it)->GetCalibratedEnergy();
        double rawStripEnergy_ = (*it)->GetEnergy();

        if (isFront_) {
            plot(DD_F_Strip_Vs_StripEnergy + DSSD_Plotting_Offset_ + Uncalibrated_Offset, stripNumber_, rawStripEnergy_ / 10.0);
            plot(DD_F_Strip_Vs_StripEnergy + DSSD_Plotting_Offset_, stripNumber_, calStripEnergy_ / 10.0);
        } else if (isBack_ && isLowGain_) {
            plot(DD_B_LG_Strip_Vs_StripEnergy + DSSD_Plotting_Offset_ + Uncalibrated_Offset, stripNumber_, rawStripEnergy_ / 10.0);
            plot(DD_B_LG_Strip_Vs_StripEnergy + DSSD_Plotting_Offset_, stripNumber_, calStripEnergy_ / 10.0);

        } else if (isBack_ && isHighGain_) {
            plot(DD_B_HG_Strip_Vs_StripEnergy + DSSD_Plotting_Offset_ + Uncalibrated_Offset, stripNumber_, rawStripEnergy_ / 10.0);
            plot(DD_B_HG_Strip_Vs_StripEnergy + DSSD_Plotting_Offset_, stripNumber_, calStripEnergy_ / 10.0);
        }
    }
    


    return true;
}

 unsigned int MtasDssdTestProcessor::ReturnPlottingOffsets( const int &dssdNum ) {
    return ((dssdNum) * DSSDNumber_offset);
    // if (subtype == "dssd1") {
    //     return (make_pair(, Uncalibrated_Offset));
    // }
    // return make_pair(numeric_limits<unsigned int>::max(), numeric_limits<unsigned int>::max());
}
