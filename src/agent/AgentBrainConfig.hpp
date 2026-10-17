#pragma once

#include "AgentBody.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace nlm {

// Configuration system for AgentBrain - allows runtime customization
class AgentBrainConfig {
public:
    // Neural scaling factors
    float visionScaleFactor = 5.0f;
    float touchScaleFactor = 8.0f;
    float internalCenterScale = 2.0f;
    float internalScaleFactor = 5.0f;
    float proprioceptionCenterScale = 2.0f;
    float proprioceptionScaleFactor = 3.0f;
    
    // Novelty detection
    float noveltyDecayRate = 0.99f;
    float curiosityNoveltyMultiplier = 2.0f;
    float curiosityErrorMultiplier = 0.5f;
    
    // Motor decoding
    float motorActivityThreshold = 0.5f;
    float explorationBaseProbability = 0.3f;
    float curiosityThreshold = 0.3f;
    float highCuriosityThreshold = 0.5f;
    
    // Reward modulation
    float predictionErrorLearningRate = 0.05f;
    float eligibilityTraceDecay = 0.1f;
    float plasticityBaseValue = 0.5f;
    float plasticityRange = 1.5f;
    
    // Development
    double developmentalStage1Age = 60.0;  // Initial
    double developmentalStage2Age = 300.0; // CriticalPeriod
    double developmentalStage3Age = 900.0; // Maturation
    float synaptogenesisBaseRate = 0.0001f;
    float pruningBaseRate = 0.00001f;
    
    // State management
    float dopamineClampMin = -1.0f;
    float dopamineClampMax = 1.0f;
    float plasticityMin = 0.1f;
    float plasticityMax = 2.0f;
    
    // Configuration registry - allows custom parameters
    using ConfigValidator = std::function<bool(const std::string&, const std::string&)>;
    using ConfigSetter = std::function<void(float, const std::string&)>;
    std::unordered_map<std::string, float> customParameters;
    std::unordered_map<std::string, ConfigValidator> parameterValidators;
    
    // Get parameter value (with custom parameter support)
    float getParameter(const std::string& name, float defaultValue = 0.0f) const {
        auto it = customParameters.find(name);
        if (it != customParameters.end()) return it->second;
        return defaultValue;
    }
    
    // Set parameter value (with validation)
    bool setParameter(const std::string& name, float value) {
        if (parameterValidators.count(name)) {
            if (!parameterValidators[name](name, std::to_string(value))) {
                return false;
            }
        }
        customParameters[name] = value;
        return true;
    }
    
    // Reset to defaults
    void resetToDefaults() {
        customParameters.clear();
    }
    
    // Export configuration for serialization
    std::string exportToString() const {
        std::string result = "AgentBrainConfig{\n";
        result += "  visionScaleFactor = " + std::to_string(visionScaleFactor) + ",\n";
        result += "  touchScaleFactor = " + std::to_string(touchScaleFactor) + ",\n";
        result += "  noveltyDecayRate = " + std::to_string(noveltyDecayRate) + ",\n";
        // Add more parameters as needed...
        result += "};\n";
        return result;
    }
    
    // Import configuration from string
    bool importFromString(const std::string& configStr) {
        // Basic parsing - in production, use proper serialization
        try {
            // Simple key=value parsing
            size_t pos;
            if ((pos = configStr.find("visionScaleFactor =")) != std::string::npos) {
                size_t start = configStr.find('=', pos) + 1;
                size_t end = configStr.find(',', start);
                visionScaleFactor = std::stof(configStr.substr(start, end - start));
            }
            return true;
        } catch (...) {
            return false;
        }
    }
};

// Parameter validators
inline bool validatePositive(const std::string&, const std::string& value) {
    try {
        float val = std::stof(value);
        return val >= 0.0f;
    } catch (...) {
        return false;
    }
}

inline bool validateRange(const std::string&, const std::string& value) {
    try {
        float val = std::stof(value);
        return val >= -1.0f && val <= 1.0f;
    } catch (...) {
        return false;
    }
}

inline bool validatePositiveFloat(const std::string&, const std::string& value) {
    try {
        float val = std::stof(value);
        return val > 0.0f;
    } catch (...) {
        return false;
    }
}

} // namespace nlm