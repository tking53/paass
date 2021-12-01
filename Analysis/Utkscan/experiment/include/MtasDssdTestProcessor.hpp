/**@file MtasDssdTestProcessor.hpp
*@brief  Inital MTAS DSSD processor     
*@authors T.T. King 
*@date 12/01/2021
*/
#ifndef PAASS_MtasDssdTestProcessor_H
#define PAASS_MtasDssdTestProcessor_H

#include "EventProcessor.hpp"
#include "RawEvent.hpp"



class MtasDssdTestProcessor : public EventProcessor {
   public:
    /**Constructor */
    MtasDssdTestProcessor();
    
   ///Constructor taking arguments
    ///@param [in] numDSSDs : number of dssds present
    ///@param [in] res : The resolution of the DAMM histograms
    MtasDssdTestProcessor(const int &numDSSDs, const double &res);

    /** Deconstructor */
    ~MtasDssdTestProcessor() = default;
    
    /** Declare plots */
    virtual void DeclarePlots();

    /** Preprocess the event
    * \param [in] event : the event to preprocess
    * \return true if successful
    */
    virtual bool PreProcess(RawEvent &event);

    /** Process the event
     * \param [in] event : the event to process
     * \return true if successful
     */
    virtual bool Process(RawEvent &event);

   unsigned int ReturnPlottingOffsets( const int &dssdNum);

   private:
   int numberOfDssds;
   int plotRes;

   typedef struct  {
       int numberOfFires_f_ = 0;
       int numberOfFires_bhg_ = 0;
       int numberOfFires_blg_ = 0;
   } multipliciesStruc;

   static multipliciesStruc multipliciesStrucDefault;
};

#endif  //PAASS_MtasDssdTestProcessor_H
