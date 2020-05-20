///@file NewDefaultConfigurationValues.hpp
///@brief Contains some default values for use in the scan codes in the event that a user forgets something in the
/// configuration file. Change to class - T.T. King 05/14/2020
///@author T.T. King
///@date May 14, 2020

#ifndef PAASS_NEWDEFAULTCONFIGURATIONVALUES_HPP
#define PAASS_NEWDEFAULTCONFIGURATIONVALUES_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "DefaultConfigurationValues.hpp"
#include "StringManipulationFunctions.hpp"

//! Class that takes type:subtype and returns sane defaults for that detector
using std::string;
using std::map;
using std::vector;

class DefaultDetectorConfig {
   public:
    /** Default Constructor */
    DefaultDetectorConfig();

    /** Type Init Constructor */
    DefaultDetectorConfig(std::string type_name) : name(type_name) {};

    /** Default Destructor */
    virtual ~DefaultDetectorConfig();

    /** Virtual function to init the Type Parameters, used by the derived type specific classes **/
    virtual void Init(){};

    /** Void Initalized all types in the Config Map **/
    void Initialize();

    /** \brief Check if this type has a set of defaults
     * \param [in] type: Type to check 
     * \return true if we have a set of defaults for the type */
    bool HasTypeDefaults(const std::string& type);

    /** \return The requested Type based class of default configs
     * \param [in] name : the name of the Det type to return */
    DefaultDetectorConfig* GetTypeConfig(std::string name) {
        auto it = ConfigMap.find(name);
        if (it != ConfigMap.end()) {
            return (it->second);
        } else {
            return (ConfigMap.find("template")->second);
        }
    }

    /** \brief Get the name of the config
    * \return Name of the config */
    string GetName(void) const {
        return (name);
    }

    /** \brief Function to retrieve the relevent map from the derived class
    */
    virtual map<string, map<string, double>> GetMapofFitParameters(){
        return MapofFitParameters;
    };

    /** \brief Function to retrieve the relevent map from the derived class
    */
    virtual map<string, map<string, double>> GetMapofTraceParameters(){
        return MapofTraceParameters;
    };

    /** \brief Function to retrieve the relevent map from the derived class
    */
    virtual map<string, map<string, double>> GetMapofCfdParameters(){
        return MapofCfdParameters;
    };

    /** \brief Function to retrieve the relevent map from the derived class
    */
    virtual map<string, map<string, unsigned int>> GetMapofFilterParameters(){
        return MapofFilterParameters;
    };

    /** \brief Generic Search function for the nested parameter maps. 
 * \param [in] typeMap2Search : Base level parameter map to look through. It refers to the Cfd map, the Fit map, or the Trace map. So the channel sub nodes each have their own map. 
 * \param [in] subtypeKey : the subtype:group key too look up in the typeMap3Search. Example would be "medium:ignore" for a vandle:medium (since we dont use group for VANDLE) or "dynode_high:singles" for the "pspmt:dynode_high:singels" which is the split beta for a fragmentation exp
 * \param [in] parameter : This is the parameter to search for. since we are carrying forward the naming scheme from the old style this would be "fitBeta", "fitGamma", "waveformHigh", "wavefromLow", etc.
 * \param [out] returnParameter : the Value of the parameter we are looking for.
 * \return : Return true if we found the value
 * 
 * This was implemented to reduce the complexity of adding more parameters. If we find the subtypekey ("<subtype>:<group>") then just pull that "inner" map if we don''t then retry with the standard ("<subtype>:ignore") 
 * which is what we get if we do not set a group in the XML, it will also allow us to set subtype specifics without requiring the group to be anything in particular. 
    */
    virtual bool SearchMapsForParameter(const map<string, map<string, double>>& typeMap2Search, const string& subtypeKey, const string& parameter, double& returnParameter) {
        auto typeMapIterator = typeMap2Search.find(subtypeKey);
        map<string, double> subtypeMap;
        std::cout << "map size is " << typeMap2Search.size() << "  subtypeKey passed for "<< parameter << " is "<< subtypeKey << std::endl;
        if (typeMapIterator != typeMap2Search.end()) {
            std::cout<<"Generic Lookup found"<<std::endl;
            subtypeMap = typeMapIterator->second;
        } else {
            vector<string> splitName = StringManipulation::TokenizeString(subtypeKey, ":");
            string backupKey = splitName.at(0) + ":ignore";
            auto itbackup = typeMap2Search.find(backupKey);
            if (itbackup != typeMap2Search.end()) {
                subtypeMap = itbackup->second;
            } else {
                return (false);
            }
        }
        auto itIN = subtypeMap.find(parameter);
        if (itIN != subtypeMap.end()) {
            returnParameter = itIN->second;
        } else {
            return false;
        }
        return true;
    }

    /// \brief Overload of the SearchMapsForParameter function that works with the map of unsigned ints parameters.
    virtual bool SearchMapsForParameter(const map<string, map<string, unsigned>>& typeMap2Search, const string& subtypeKey, const string& parameter, unsigned& returnParameter) {
        auto typeMapIterator = typeMap2Search.find(subtypeKey);
        map<string, unsigned> subtypeMap;

        if (typeMapIterator != typeMap2Search.end()) {
            subtypeMap = typeMapIterator->second;
        } else {
            vector<string> splitName = StringManipulation::TokenizeString(subtypeKey, ":");
            string backupKey = splitName.at(0) + ":ignore";
            auto itbackup = typeMap2Search.find(backupKey);
            if (itbackup != typeMap2Search.end()) {
                subtypeMap = itbackup->second;
            } else {
                return (false);
            }
        }
        auto itIN = subtypeMap.find(parameter);
        if (itIN != subtypeMap.end()) {
            returnParameter = itIN->second;
        } else {
            return false;
        }
        return true;
    }

    //! //////////////////////////////////////////////////////////////////////////////////////////////
    //!   Fit Node Parameter Functions
    //! //////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Get the Default Fitting parameter, "beta" 
 * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
 * \return the Fit beta parameter
 */
    virtual double GetDefaultFitBeta(string& subtypeKey) {
        double retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofFitParameters(), subtypeKey, "fitBeta", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::fitBeta;
        }
    }

    /** \brief Get the Default Fitting parameter, "gamma" 
 * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
 * \return the Fit gamma parameter
 */
    virtual double GetDefaultFitGamma(string& subtypeKey) {
        double retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofFitParameters(), subtypeKey, "fitGamma", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::fitGamma;
        }
    }

    //! //////////////////////////////////////////////////////////////////////////////////////////////
    //!   Trace Node Parameter Functions 
    //TODO Add the QDC range and the Tail QDC range parameters and functions (NEXT/ PSD dets)
    //! //////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Get the Default Trace parameter, "RangeLow". This is part of the Fitting/Cfd ranges, and if the QDC range is not set then it is also used for the Full qdc range (the timing range and the QDC ranges are being decoupled for NEXT)
 * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
 * \return the Trace RangeLow parameter. 
 */
    virtual unsigned int GetDefaultTraceWaveLow(string& subtypeKey) {
        double retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofTraceParameters(), subtypeKey, "waveformLow", retVal);
        if (foundit_) {
            return (unsigned)retVal;
        } else {
            return DefaultConfig::waveformLow;
        }
    }

    /** \brief Get the Default Trace parameter, "RangeHigh". This is part of the Fitting/Cfd ranges, and if the QDC range is not set then it is also used for the Full qdc range (the timing range and the QDC ranges are being decoupled for NEXT)
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the Trace RangeHigh parameter. 
    */
    virtual unsigned int GetDefaultTraceWaveHigh(string& subtypeKey) {
        double retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofTraceParameters(), subtypeKey, "waveformHigh", retVal);
        if (foundit_) {
            return (unsigned)retVal;
        } else {
            return DefaultConfig::waveformHigh;
        }
    }

    /** \brief Get the Default Trace parameter, "DiscriminationStart". This is not currently used.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the Trace DiscriminationStart parameter. 
    */
    virtual unsigned int GetDefaultTraceDiscrimStart(string& subtypeKey) {
        double retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofTraceParameters(), subtypeKey, "discrimStart", retVal);
        if (foundit_) {
            return (unsigned)retVal;
        } else {
            return DefaultConfig::discrimStart;
        }
    }

    /** \brief Get the Default Trace parameter, "BaselineThreshold". This is not currently used, instead we set the threshold to 15% of the baseline standard deviation of the baseline.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the Trace BaselineThreshold parameter. 
    */
    virtual double GetDefaultTraceBaselineThreshold(string& subtypeKey) {
        double retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofTraceParameters(), subtypeKey, "baselineThreshold", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::baselineThreshold;
        }
    }

    //! //////////////////////////////////////////////////////////////////////////////////////////////
    //!   CFD Node Parameter Functions
    //TODO Upgrade the CfdF functions and parameters to check for the CFD type
    //! //////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Get the Default CFD parameter, "cfdF". This is the "fraction" or "threshold" for the CFDs. This method will need to be expanded to check for the CFD type since they do not share a reasonable value.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the CFD cfdF parameter. 
    */
    virtual double GetDefaultCfdF(string& subtypeKey) {
        double retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofCfdParameters(), subtypeKey, "cfdF", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::cfdF;
        }
    }

    /** \brief Get the Default CFD parameter, "cfdD". This is the "delay" for the CFD analyzers.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the CFD cfdD parameter. 
    */
    virtual double GetDefaultCfdD(string& subtypeKey) {
        double retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofCfdParameters(), subtypeKey, "cfdD", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::cfdD;
        }
    }

    /** \brief Get the Default CFD parameter, "cfdL". This is the "scale" for the CFD analyzers.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the CFD cfdL parameter. 
    */
    virtual double GetDefaultCfdL(string& subtypeKey) {
        double retVal;
        map<string, map<string, double>> FitMap = GetMapofCfdParameters();
        bool foundit_ = SearchMapsForParameter(GetMapofCfdParameters(), subtypeKey, "cfdL", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::cfdL;
        }
    }

    /** \brief Get the Default CFD parameter, "cfdL". This is the "scale" for the CFD analyzers.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the CFD cfdL parameter. 
    */
    virtual double GetDefaultCfdT(string& subtypeKey) {
        double retVal;
        map<string, map<string, double>> FitMap = GetMapofCfdParameters();
        bool foundit_ = SearchMapsForParameter(GetMapofCfdParameters(), subtypeKey, "cfdT", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::cfdT;
        }
    }

    //! //////////////////////////////////////////////////////////////////////////////////////////////
    //!   Filter Node Parameter Functions
    //! //////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Get the Default Filter parameter, "filterL". This is the "RiseTime" ("Length") for the software based Trapezoidal filter.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the Filter filterL parameter. 
    */
    virtual unsigned int GetDefaultFilterL(string& subtypeKey) {
        unsigned retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofFilterParameters(), subtypeKey, "filterL", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::filterL;
        }
    }

    /** \brief Get the Default Filter parameter, "filterG". This is the "Flattop" ("Gap") for the software based Trapezoidal filter.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the Filter filterG parameter. 
    */
    virtual unsigned int GetDefaultFilterG(string& subtypeKey) {
        unsigned retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofFilterParameters(), subtypeKey, "filterG", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::filterG;
        }
    }

    /** \brief Get the Default Filter parameter, "filterT". This is the "Threshold" or "TAU" for the software based Trapezoidal filter. Depending on if we are using the "Trigger" or "Energy" filters. For now, since we do not currently use this, I'm leaving it as this. However eventual if we start using this again, then we can add a second method to separate them, which will require a parameter key name change.
    * \todo Separate the TAU parameter from the Threshold parameter.
    * \param [in] subtypeKey : Subtype key to search for. The key is defined as "<subtype>:<group>"
    * \return the Filter filterT parameter. 
    */
    virtual unsigned int GetDefaultFilterT(string& subtypeKey) {
        unsigned retVal;
        bool foundit_ = SearchMapsForParameter(GetMapofFilterParameters(), subtypeKey, "filterT", retVal);
        if (foundit_) {
            return retVal;
        } else {
            return DefaultConfig::filterT;
        }
    }

   protected:
    map<string, map<string, double>> MapofFitParameters;
    map<string, map<string, double>> MapofTraceParameters;
    map<string, map<string, double>> MapofCfdParameters;
    map<string, map<string, unsigned>> MapofFilterParameters;

    string name;  //!< Type Name of the config

   private:
    map<string, DefaultDetectorConfig*> ConfigMap;
};

#endif