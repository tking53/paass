///@file NewDefaultConfigurationValues.cpp
///@brief Contains some default values for use in the scan codes in the event that a user forgets something in the
/// configuration file. Change to class - T.T. King 05/14/2020
///@author T.T. King
///@date May 14, 2020

#include "NewDefaultConfigurationValues.hpp"

#include "BetaDefaultConfig.hpp"
#include "TemplateDetectorConfig.hpp"
#include "VandleDefaultConfig.hpp"

/** Default Constructor */
DefaultDetectorConfig::DefaultDetectorConfig() {
    ConfigMap.emplace("beta", new BetaDefaultConfig());
    ConfigMap.emplace("vandle", new VandleDefaultConfig());
    ConfigMap.emplace("template", new TemplateDetectorConfig());
}

/** Default Destructor */
DefaultDetectorConfig::~DefaultDetectorConfig() {
    ConfigMap.clear();
}

/** Initialize all type classes in the ConfigMap */
void DefaultDetectorConfig::Initialize() {
    for (auto it = ConfigMap.begin(); it != ConfigMap.end(); it++) {
        it->second->Init();
    }
}

bool DefaultDetectorConfig::HasTypeDefaults(const string& type) {
    auto it = ConfigMap.find(type);
    if (it != ConfigMap.end()) {
        return (true);
    } else {
        return (false);
    }
}

