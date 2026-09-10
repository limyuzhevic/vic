// Comprehensive error handling and diagnostics for NLM
// Provides complete implementation of the Diagnostics module

#include "Diagnostics.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

// ConfigValidator implementation
ConfigValidator::ValidationResult ConfigValidator::validate(const Config& config) {
    ValidationResult result;
    
    // Check basic configuration requirements
    if (config.getVersion().empty()) {
        result.errors.push_back("Configuration version is required");
        result.isValid = false;
    }
    
    // Validate simulation parameters
    auto timestep = config.getTimestep();
    if (timestep <= 0.0) {
        result.errors.push_back("Timestep must be positive");
        result.isValid = false;
    }
    
    // Validate brain parameters
    auto brainSize = config.getBrainSize();
    if (brainSize < 100) {
        result.warnings.push_back("Small brain size may limit cognitive capabilities");
    }
    
    // Validate neuromodulator parameters
    auto neuromodulators = config.getNeuromodulators();
    for (const auto& mod : neuromodulators) {
        if (mod.level < 0.0 || mod.level > 1.0) {
            result.errors.push_back("Neuromodulator level must be between 0.0 and 1.0");
            result.isValid = false;
        }
    }
    
    // Calculate safety score based on validation
    double safetyScore = 1.0;
    safetyScore -= result.errors.size() * 0.2;
    safetyScore += result.warnings.size() * 0.05;
    result.safetyScore = std::max(0.0, std::min(1.0, safetyScore));
    
    return result;
}

ConfigValidator::ValidationResult ConfigValidator::validateForSafety(const Config& config) {
    ValidationResult result = validate(config);
    
    // Additional safety-specific checks
    auto learningRate = config.getLearningRate();
    if (learningRate > 0.5) {
        result.warnings.push_back("High learning rate may cause instability");
        result.safetyScore *= 0.9;
    }
    
    auto plasticityThreshold = config.getPlasticityThreshold();
    if (plasticityThreshold > 0.3) {
        result.warnings.push_back("High plasticity threshold may lead to excessive synapse changes");
        result.safetyScore *= 0.95;
    }
    
    return result;
}

ConfigValidator::ValidationResult ConfigValidator::validateForPerformance(const Config& config) {
    ValidationResult result = validate(config);
    
    // Performance-specific validation
    auto maxNeurons = config.getMaxNeurons();
    if (maxNeurons > 10000) {
        result.warnings.push_back("Large neuron count may impact performance");
    }
    
    // Check for balanced configuration
    auto neuromodulators = config.getNeuromodulators();
    size_t activeCount = 0;
    for (const auto& mod : neuromodulators) {
        if (mod.level > 0.0) activeCount++;
    }
    
    if (activeCount < 2) {
        result.warnings.push_back("Low neuromodulator activity may reduce system flexibility");
    }
    
    return result;
}

ConfigValidator::ValidationResult ConfigValidator::validateForLearning(const Config& config) {
    ValidationResult result = validate(config);
    
    // Learning-specific validation
    auto learningRate = config.getLearningRate();
    if (learningRate < 0.01) {
        result.warnings.push_back("Very low learning rate may slow adaptation");
    }
    
    auto explorationRate = config.getExplorationRate();
    if (explorationRate < 0.01) {
        result.warnings.push_back("Very low exploration rate may limit discovery");
    }
    
    // Check for learning balance
    if (learningRate + explorationRate > 1.0) {
        result.errors.push_back("Learning rate + exploration rate should not exceed 1.0");
        result.isValid = false;
    }
    
    return result;
}

std::string ConfigValidator::getRecommendations(const Config& config) {
    std::stringstream ss;
    ss << "Configuration Recommendations:" << std::endl;
    
    auto validation = validate(config);
    
    if (validation.warnings.empty()) {
        ss << "  Configuration looks good! No specific recommendations." << std::endl;
        return ss.str();
    }
    
    ss << "  Based on current settings:" << std::endl;
    for (const auto& warning : validation.warnings) {
        ss << "  - " << warning << std::endl;
    }
    
    ss << "  Consider adjusting these parameters for optimal performance." << std::endl;
    return ss.str();
}

std::string ConfigValidator::getSafetyAnalysis(const Config& config) {
    std::stringstream ss;
    ss << "Safety Analysis:" << std::endl;
    
    auto validation = validateForSafety(config);
    
    ss << "  Safety Score: " << (validation.safetyScore * 100) << "%" << std::endl;
    ss << "  Errors: " << validation.errors.size() << std::endl;
    ss << "  Warnings: " << validation.warnings.size() << std::endl;
    
    if (validation.safetyScore < 0.5) {
        ss << "  Status: REQUIRES ATTENTION - Safety score is low" << std::endl;
    } else if (validation.safetyScore < 0.8) {
        ss << "  Status: CAUTION ADVISED - Safety score is moderate" << std::endl;
    } else {
        ss << "  Status: GOOD - Safety score is acceptable" << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm