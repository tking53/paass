/**Created by T.T. King 06/22/2018
 * */
/** PixTreeEvent class added by R. Yokoyama 08/31/2018 **/
#ifndef PAASS_PROCESSORSTRUC_HPP
#define PAASS_PROCESSORSTRUC_HPP

#include <TObject.h>
#include <TString.h>

namespace processor_struct {
    struct VANDLES {
        double tof=0;
        double corTof=0;
        double qdcPos=0;
        double qdc=0;
        int barNum=0;
        std::string barType="";
        double tdiff=-999;
        unsigned int sNum =0; //start detector number
        int vMulti=0;
        double sTime = -999;
        double sQdc = -999;

    } ;
    static const VANDLES  VANDLES_DEFAULT_STRUCT;
    
    struct GAMMASCINT{
        double energy = -999;
        double rawEnergy = -999;
        bool isDynodeOut = false;
        int detNum = -999;
        double time = -999;
        TString group = "";
        TString subtype = "";
    } ;
    static const GAMMASCINT GAMMASCINT_DEFAULT_STRUCT;

    struct DOUBLEBETA{
        int detNum = -999;
        double energy = -999;
        double rawEnergy = -999;
        double timeAvg = -999;
        double timeDiff = -999;
        double timeL = -999;
        double timeR = -999;
        double barQdc = -999;
        double tMaxValL = -999;
        double tMaxValR = -999;
        bool isLowResBeta = false;
        bool isHighResBeta = false;
    };
    static const DOUBLEBETA DOUBLEBETA_DEFAULT_STRUCT;

    struct CLOVERS{
        double energy = -999;
        double rawEnergy =-999;
        double time = -999;
        int detNum = -999;
        int cloverNum = -999;
    };

    static const CLOVERS CLOVERS_DEFAULT_STRUCT;

    struct LOGIC{
        bool tapeCycleStatus = false;
        bool beamStatus= false;
        bool tapeMoving = false;

        double lastTapeCycleStartTime = -999;
        double lastBeamOnTime = -999;
        double lastBeamOffTime = -999;
        double lastTapeMoveStartTime = -999;
        double lastProtonPulseTime = -999;
        double lastSuperCycleTime = -999;

        double cycleNum = -999;
    };
    static const LOGIC LOGIC_DEFAULT_STRUCT;

    struct PSPMT { 
        double energy = -999;
        double time = -999;
        TString subtype = "";
        TString tag = "";
    };
    static const PSPMT  PSPMT_DEFAULT_STRUCT;
}    

class PixTreeEvent : public TObject
{
public:

    PixTreeEvent(){}

    /* copy constructor */
    PixTreeEvent( const PixTreeEvent &obj ):TObject(obj)
    {
        externalTS1 = obj.externalTS1;
        externalTS2 = obj.externalTS2;
        eventNum = obj.eventNum;
        fileName = obj.fileName;
        clover_vec_ = obj.clover_vec_;
        doublebeta_vec_ = obj.doublebeta_vec_;
        gamma_scint_vec_ = obj.gamma_scint_vec_;
        logic_vec_ = obj.logic_vec_;
        pspmt_vec_ = obj.pspmt_vec_;
        vandle_vec_ = obj.vandle_vec_;
    }

    virtual ~PixTreeEvent(){}

    /* clear vectors and init all the values */
    virtual void Clear()
    {
        externalTS1 = 0;
        externalTS2 = 0;
        eventNum = 0;
        fileName = "";
        clover_vec_.clear();
        doublebeta_vec_.clear();
        gamma_scint_vec_.clear();
        logic_vec_.clear();
        pspmt_vec_.clear();
        vandle_vec_.clear();

    }

    /* data structures to be filled in the ROOT TTree */
    ULong64_t externalTS1 = 0;
    ULong64_t externalTS2 = 0;
    Double_t eventNum = 0;
    std::string fileName = "";
    std::vector<processor_struct::CLOVERS> clover_vec_;
    std::vector<processor_struct::DOUBLEBETA> doublebeta_vec_;
    std::vector<processor_struct::GAMMASCINT> gamma_scint_vec_;
    std::vector<processor_struct::LOGIC> logic_vec_;
    std::vector<processor_struct::PSPMT> pspmt_vec_;
    std::vector<processor_struct::VANDLES> vandle_vec_;

    ClassDef(PixTreeEvent,1)
};

#endif //PAASS_PROCESSORSTRUC_HPP
