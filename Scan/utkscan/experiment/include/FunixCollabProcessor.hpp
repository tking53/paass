/** \file FunixCollabProcessor.hpp
 * \brief Class to perform simple two channel timing experiments
 * \author S. V. Paulauskas
 * \date July 10, 2009
 */

#ifndef __FUNIXCOLLABPROCESSOR_HPP_
#define __FUNIXCOLLABPROCESSOR_HPP_

#include "EventProcessor.hpp"

//! A class that handles a simple timing experiment
class FunixCollabProcessor : public EventProcessor {
 public:
    /** Default Constructor */
    FunixCollabProcessor();
    /** Default Destructor */
    ~FunixCollabProcessor();
    /** Declares the plots for the processor */
    virtual void DeclarePlots(void);
    /** Performs the main processsing, which may depend on other processors
    * \param [in] event : the event to process
    * \return true if processing was successful */
    virtual bool Process(RawEvent &event);
};
#endif // __FUNIXCOLLABPROCESSOR_HPP_
