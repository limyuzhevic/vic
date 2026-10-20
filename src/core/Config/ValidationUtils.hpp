#pragma once

#include "Validation.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {

// Configuration category helper functions
namespace ConfigCategories {

ConfigCategory stringToCategory(const std::string& categoryStr) {
    std::string lowerStr = categoryStr;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    
    if (lowerStr.find("neuromod") != std::string::npos) {
        return ConfigCategory::Neuromodulation;
    } else if (lowerStr.find("simulation") != std::string::npos) {
        return ConfigCategory::Simulation;
    } else if (lowerStr.find("development") != std::string::npos) {
        return ConfigCategory::Development;
    } else if (lowerStr.find("memory") != std::string::npos) {
        return ConfigCategory::Memory;
    } else if (lowerStr.find("environment") != std::string::npos) {
        return ConfigCategory::Environment;
    } else if (lowerStr.find("log") != std::string::npos) {
        return ConfigCategory::Logging;
    } else if (lowerStr.find("visualization") != std::string::npos) {
        return ConfigCategory::Visualization;
    } else if (lowerStr.find("reward") != std::string::npos) {
        return ConfigCategory::Reward;
    } else {
        return ConfigCategory::Unknown;
    }
}

std::string categoryToString(ConfigCategory category) {
    switch (category) {
        case ConfigCategory::Neuromodulation: return "neuromodulation";
        case ConfigCategory::Simulation: return "simulation";
        case ConfigCategory::Development: return "development";
        case ConfigCategory::Memory: return "memory";
        case ConfigCategory::Environment: return "environment";
        case ConfigCategory::Logging: return "logging";
        case ConfigCategory::Visualization: return "visualization";
        case ConfigCategory::Reward: return "reward";
        case ConfigCategory::Unknown: return "unknown";
        default: return "unknown";
    }
}

std::vector<std::string> getCategoryFields(ConfigCategory category) {
    const auto& schemaManager = ConfigSchemaManager::getInstance();
    const auto& schema = schemaManager.getSchema(category);
    
    std::vector<std::string> fields;
    for (const auto& pair : schema.rules) {
        fields.push_back(pair.first);
    }
    return fields;
}

} // namespace ConfigCategories

} // namespace nlm