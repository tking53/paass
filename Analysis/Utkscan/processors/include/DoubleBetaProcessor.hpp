/** \file DoubleBetaProcessor.hpp
 * \brief A Template class to be used to build others.
 * \author S. V. Paulauskas
 * \date October 26, 2014
 */
#ifndef __DOUBLEBETAPROCESSOR_HPP__
#define __DOUBLEBETAPROCESSOR_HPP__

#include "BarDetector.hpp"
#include "EventProcessor.hpp"
#include "HighResTimingData.hpp"

#ifdef useroot

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1D.h>
#include <TH2D.h>

#endif

//! A generic processor to be used as a template for others
class DoubleBetaProcessor : public EventProcessor {
public:
    /** Default Constructor */
    DoubleBetaProcessor();
    /** Default Destructor */
    ~DoubleBetaProcessor(){
        rootFName3_->Write();
        rootFName3_->Close();
        delete (rootFName3_);
    };

    /** Declares the plots for the processor */
    virtual void DeclarePlots(void);
    /** Performs the preprocessing, which cannot depend on other processors
    * \param [in] event : the event to process
    * \return true if preprocessing was successful */
    virtual bool PreProcess(RawEvent &event);
    /** Performs the main processsing, which may depend on other processors
    * \param [in] event : the event to process
    * \return true if processing was successful */
    virtual bool Process(RawEvent &event);

    /** \return The map of the bars that had high resolution timing */
    BarMap GetBars(void){return(bars_);}
    /** \return the map of the bars that had low resolution timing */
    std::map<unsigned int, std::pair<double,double> > GetLowResBars(void) {
	return(lrtbars_);
    }
private:
    BarMap bars_; //!< Map holding all the bars we found 
    std::map<unsigned int, std::pair<double,double> > lrtbars_; //!< map holding low res bars

    TFile *rootFName3_;
    TTree *BWaveT;
    TBranch *BwaveBranch;
    TH2D *CenterL;
    TH2D *CenterR;
    TH2D *LeftL;
    TH2D *LeftR;
    TH2D *RightL;
    TH2D *RightR;

    struct BWave{
        double Ltrace[131];
        double Rtrace[131];
        double Lbaseline;
        double Rbaseline;
        double LmaxLoc;
        double RmaxLoc;
        double Lamp;
        double Ramp;
        double BarQdc;
        double Lsnr;
        double Rsnr;
        double Lqdc;
        double Rqdc;
        double Lphase;
        double Rphase;
        double Tdiff;
        double TdiffD;

    } Bwave;

    void rootBWaveInit(BWave &strutName);

};
#endif // __DOUBLEBETAPROCESSOR_HPP__
