///@file TemplateDetectorConfig.hpp
///@brief Contains some default values for a Template Detector Configuration. These are clones of the VANDLE medium since thats what our defaults were before.
///@author T.T. King
///@date May 14, 2020

#ifndef PAASS_TEMPLATEDETECTORCONFIGURATIONVALUES_HPP
#define PAASS_TEMPLATEDETECTORCONFIGURATIONVALUES_HPP

#include "NewDefaultConfigurationValues.hpp"

class TemplateDetectorConfig : public DefaultDetectorConfig {
   public:
    ///@brief Default constructor
    TemplateDetectorConfig() : DefaultDetectorConfig("template"){};

    ///@brief Init function. That populates the maps with values. Seperated from the constructor for clarity.
    void Init() {
        MapofFitParameters = {
            {{"subtype:group"}, {{"fitBeta", 0.254373}, {"fitGamma", 0.208072}}},
            {{"generic:ignore"}, {{"fitBeta", 0.254373}, {"fitGamma", 0.208072}}}};
        MapofTraceParameters = {
            {{"subtype:group"}, {{"waveformLow", 5}, {"waveformHigh", 10}}},
            {{"generic:ignore"}, {{"waveformLow", 5}, {"waveformHigh", 10}}}};
        MapofCfdParameters = {
            {{"subtype:group"}, {{"cfdF", 5}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}},
            {{"generic:ignore"}, {{"cfdF", 5}, {"cfdD", 5}, {"cfdL", 0.6}, {"cfdT", 5}}}};
        MapofFilterParameters = {
            {{"subtype:group"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}},
            {{"generic:ignore"}, {{"filterL", 100}, {"filterG", 100}, {"filterT", 1}}}};
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
    std::map<std::string, std::map<std::string, unsigned int>> MapofFilterParameters;
};

#endif