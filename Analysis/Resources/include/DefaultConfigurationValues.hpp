///@file DefaultConfigurationValues.cpp
///@brief Contains some default values for use in the scan codes in the event that a user forgets something in the
/// configuration file. Updated in 2020 by T. T. King to have the new CFD parameter. This file is now used as a final fallback  when parsing the XML
///@author T. T. King / S. V. Paulauskas
///@date May 14, 2020 / June 3, 2017

#ifndef PAASS_DEFAULTCONFIGURATIONVALUES_HPP
#define PAASS_DEFAULTCONFIGURATIONVALUES_HPP

namespace DefaultConfig {
///These are used when reading /Configuration/Map/Module/Channel/Trace
static const unsigned int waveformLow = 5;
static const unsigned int waveformHigh = 10;
static const unsigned int discrimStart = 3;
static const double baselineThreshold = 3.;

///These are used when reading /Configuration/Map/Module/Channel/Trace/Filter/Trigger or .../Filter
static const unsigned int filterL = 100;
static const unsigned int filterG = 100;
static const unsigned int filterT = 1;

///These are used when reading /Configuration/Map/Module/Channel/Fit
static const double fitBeta = 0.254373;
static const double fitGamma = 0.208072;

///These are used when reading /Configuration/Map/Module/Channel/Cfd
static const double cfdF = 0.45;
static const double cfdD = 5;
static const double cfdL = 0.6;
static const double cfdT = 5;
}  // namespace DefaultConfig

#endif  //PAASS_DEFAULTCONFIGURATIONVALUES_HPP
