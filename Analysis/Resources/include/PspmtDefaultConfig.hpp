///@file VandleDefaultConfig.hpp
///@brief Contains some default values for VANDLE Detectors
///@author T.T. King
///@date May 14, 2020

#ifndef PAASS_PSPMTDEFAULTCONFIGURATIONVALUES_HPP
#define PAASS_PSPMTDEFAULTCONFIGURATIONVALUES_HPP

#include "NewDefaultConfigurationValues.hpp"

class PspmtDefaultConfig : public DefaultDetectorConfig {
   public:
    ///@brief Default constructor
    PspmtDefaultConfig() : DefaultDetectorConfig("pspmt"){};

    ///@brief Init function. That populates the maps with values. Seperated from the constructor for clarity.
    void Init() {
        MapofFitParameters = {
            {{"dynode_high:ignore"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}},
            {{"dynode_low:ignore"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}},
            {{"anode_high:ignore"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}},
            {{"anode_low:ignore"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}},
            {{"dynode_high:singles"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}},
            {{"f11:ignore"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}},
            {{"ion:ignore"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}},
            {{"veto:ignore"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}},
            {{"desi:ignore"}, {{"fitBeta", 0.125515810164938}, {"fitGamma", 0.173426623471837}}}
            };
        MapofTraceParameters = {
            {{"dynode_high:ignore"}, {{"waveformLow", 7}, {"waveformHigh", 25}}},
            {{"dynode_low:ignore"}, {{"waveformLow", 7}, {"waveformHigh", 25}}},
            {{"anode_high:ignore"}, {{"waveformLow", 7}, {"waveformHigh", 40}}},
            {{"anode_low:ignore"}, {{"waveformLow", 7}, {"waveformHigh", 40}}},
            {{"dynode_high:singles"}, {{"waveformLow", 7}, {"waveformHigh", 25}}},
            {{"f11:ignore"}, {{"waveformLow", 7}, {"waveformHigh", 25}}},
            {{"ion:ignore"}, {{"waveformLow", 7}, {"waveformHigh", 25}}},
            {{"veto:ignore"}, {{"waveformLow", 7}, {"waveformHigh", 25}}},
            {{"desi:ignore"}, {{"waveformLow", 7}, {"waveformHigh", 25}}}
            };
        MapofCfdParameters = {
            {{"dynode_high:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"dynode_low:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"anode_high:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"anode_low:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"dynode_high:singles"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"f11:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"ion:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"veto:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"desi:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}}
            };
        MapofFilterParameters = {
            {{"dynode_high:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"dynode_low:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"anode_high:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"anode_low:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"dynode_high:singles"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            };
    };

    //! //////////////////////////////////////////////////////////////////////////////////////////////
    //!  Map Retrieval Functions.
    //! //////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Function to retrieve the relevent map from the derived class
     * \return Returns the respective map of default xml parameters
    */
    map<string, map<string, double>> GetMapofFitParameters() {
        return MapofFitParameters;
    };

    /** \brief Function to retrieve the relevent map from the derived class
     * \return Returns the respective map of default xml parameters
    */
    map<string, map<string, double>> GetMapofTraceParameters() {
        return MapofTraceParameters;
    };

    /** \brief Function to retrieve the relevent map from the derived class
     * \return Returns the respective map of default xml parameters
    */
    map<string, map<string, double>> GetMapofCfdParameters() {
        return MapofCfdParameters;
    };

    /** \brief Function to retrieve the relevent map from the derived class
     * \return Returns the respective map of default xml parameters
    */
    map<string, map<string, unsigned int>> GetMapofFilterParameters() {
        return MapofFilterParameters;
    };

   private:
    std::map<std::string, std::map<std::string, double>> MapofFitParameters;
    std::map<std::string, std::map<std::string, double>> MapofTraceParameters;
    std::map<std::string, std::map<std::string, double>> MapofCfdParameters;
    std::map<std::string, std::map<std::string, unsigned>> MapofFilterParameters;
};

#endif
