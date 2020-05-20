///@file VandleDefaultConfig.hpp
///@brief Contains some default values for VANDLE Detectors
///@author T.T. King
///@date May 14, 2020

#ifndef PAASS_VANDLEDEFAULTCONFIGURATIONVALUES_HPP
#define PAASS_VANDLEDEFAULTCONFIGURATIONVALUES_HPP

#include "NewDefaultConfigurationValues.hpp"

class VandleDefaultConfig : public DefaultDetectorConfig {
   public:
    ///@brief Default constructor
    VandleDefaultConfig() : DefaultDetectorConfig("vandle"){};

    ///@brief Init function. That populates the maps with values. Seperated from the constructor for clarity.
    void Init() {
        MapofFitParameters = {
            {{"small:ignore"}, {{"fitBeta", 0.32969}, {"fitGamma", 0.212945}}},
            {{"medium:ignore"}, {{"fitBeta", 0.254373}, {"fitGamma", 0.208072}}},
            {{"big:ignore"}, {{"fitBeta", 0.32969}, {"fitGamma", 0.212945}}}};
        MapofTraceParameters = {
            {{"small:ignore"}, {{"waveformLow", 5}, {"waveformHigh", 10}}},
            {{"medium:ignore"}, {{"waveformLow", 5}, {"waveformHigh", 10}}},
            {{"big:ignore"}, {{"waveformLow", 5}, {"waveformHigh", 10}}}};
        MapofCfdParameters = {
            {{"small:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"medium:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"big:ignore"}, {{"cfdF", 0.45}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}}};
        MapofFilterParameters = {
            {{"small:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"medium:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"big:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}}};
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