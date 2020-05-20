///@file VandleDefaultConfig.hpp
///@brief Contains some default values for VANDLE Detectors
///@author T.T. King
///@date May 14, 2020

#ifndef PAASS_BETADEFAULTCONFIGURATIONVALUES_HPP
#define PAASS_BETADEFAULTCONFIGURATIONVALUES_HPP

#include "NewDefaultConfigurationValues.hpp"

class BetaDefaultConfig : public DefaultDetectorConfig {
   public:
    ///@brief Default constructor
    BetaDefaultConfig() : DefaultDetectorConfig("beta"){};

    ///@brief Init function. That populates the maps with values. Seperated from the constructor for clarity.
    void Init() {
        MapofFitParameters = {
            {{"single:ignore"}, {{"fitBeta", 0.254373}, {"fitGamma", 0.208072}}},
            {{"double:ignore"}, {{"fitBeta", 0.254373}, {"fitGamma", 0.208072}}},
            {{"double:ornl2016"}, {{"fitBeta", 0.004265}, {"fitGamma", 0.1544}}},
            {{"double:isolde"}, {{"fitBeta", 0.491724}, {"fitGamma", 0.335260}}}};
        MapofTraceParameters = {
            {{"single:ignore"}, {{"waveformLow", 5}, {"waveformHigh", 10}}},
            {{"double:ignore"}, {{"waveformLow", 5}, {"waveformHigh", 10}}},
            {{"double:ornl2016"}, {{"waveformLow", 15}, {"waveformHigh", 5}}},
            {{"double:isolde"}, {{"waveformLow", 5}, {"waveformHigh", 10}}},
            {{"double:timing"}, {{"waveformLow", 5}, {"waveformHigh", 5}}}};
        MapofCfdParameters = {
            {{"single:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"double:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"double:ornl2016"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"double:isolde"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"double:timing"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}}};
        MapofFilterParameters = {
            {{"single:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"double:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"double:ornl2016"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"double:isolde"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"double:timing"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}}};
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
