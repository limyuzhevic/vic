#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHierarchy.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f) {}
};

Novelty::Novelty() : pImpl(new Impl) {
    try {
        // Initialize with error handling
        reset();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize Novelty neuromodulator: " + std::string(e.what()));
        // Continue with default initialization
    }
}

Novelty::~Novelty() {
    delete pImpl;
}

void Novelty::initialize(Brain* brain) {
    try {
        if (!brain) {
            throw NLMNeuromodulationError("Brain pointer is null",
                                          NLMNeuromodulationError::ErrorType::NoveltyError,
                                          "Novelty::initialize");
        }
        
        pImpl->brain = brain;
        NLM_LOG_INFO("Novelty detection initialized");
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in initialize: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error initializing novelty: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to initialize novelty",
                                      NLMNeuromodulationError::ErrorType::NoveltyError,
                                      "Novelty::initialize");
    }
}

float Novelty::getLevel() const {
    try {
        return std::max(0.0f, pImpl->level); // Ensure non-negative
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting novelty level: " + std::string(e.what()));
        return 0.0f;
    }
}

void Novelty::setLevel(float level) {
    try {
        if (std::isnan(level) || std::isinf(level)) {
            throw NLMNeuromodulationError("Invalid novelty level: " + std::to_string(level),
                                          NLMNeuromodulationError::ErrorType::NoveltyError,
                                          "Novelty::setLevel");
        }
        
        pImpl->level = std::max(0.0f, std::min(level, 1.0f));  // Clamp to [0, 1]
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in setLevel: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error setting novelty level: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to set novelty level",
                                      NLMNeuromodulationError::ErrorType::NoveltyError,
                                      "Novelty::setLevel");
    }
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    try {
        // Validate observation inputs
        if (!observation.isValid()) {
            NLM_LOG_ERROR("Invalid observation provided to novelty detection");
            return;
        }
        
        // Extract features from observations and compare
        // Simple implementation: just set to a placeholder
        setLevel(1.0f);
        pImpl->history.push_back(getLevel());
        
    } catch (const NLMEnvironmentError& e) {
        NLM_LOG_ERROR("Environment error in novelty detection: " + std::string(e.what()));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in novelty detection: " + std::string(e.what()));
        // Continue with degraded functionality
    }
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    try {
        // Validate input patterns
        if (currentPattern.empty()) {
            NLM_LOG_ERROR("Current pattern is empty for novelty detection");
            setLevel(0.0f);
            return;
        }
        
        if (previousPattern.empty()) {
            NLM_LOG_ERROR("Previous pattern is empty for novelty detection");
            setLevel(1.0f);  // Treat as novel if no previous pattern
            return;
        }
        
        // Validate pattern values
        for (size_t i = 0; i < currentPattern.size(); ++i) {
            if (std::isnan(currentPattern[i]) || std::isinf(currentPattern[i])) {
                throw NLMNeuromodulationError("Invalid value in current pattern at index " + std::to_string(i),
                                              NLMNeuromodulationError::ErrorType::NoveltyError,
                                              "Novelty::detectNovelty");
            }
        }
        
        for (size_t i = 0; i < previousPattern.size(); ++i) {
            if (std::isnan(previousPattern[i]) || std::isinf(previousPattern[i])) {
                throw NLMNeuromodulationError("Invalid value in previous pattern at index " + std::to_string(i),
                                              NLMNeuromodulationError::ErrorType::NoveltyError,
                                              "Novelty::detectNovelty");
            }
        }
        
        // Compute difference between patterns
        float totalDiff = 0.0f;
        size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
        
        if (compareLen == 0) {
            setLevel(1.0f);  // Treat as novel if nothing to compare
            return;
        }
        
        for (size_t i = 0; i < compareLen; ++i) {
            float diff = std::abs(currentPattern[i] - previousPattern[i]);
            totalDiff += diff;
        }
        
        // Normalize by pattern size
        float avgDiff = totalDiff / compareLen;
        
        // Validate average difference
        if (std::isnan(avgDiff) || std::isinf(avgDiff)) {
            throw NLMNeuromodulationError("Invalid average difference calculation",
                                          NLMNeuromodulationError::ErrorType::NoveltyError,
                                          "Novelty::detectNovelty");
        }
        
        // Update novelty level based on difference
        float novelLevel = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
        setLevel(novelLevel);
        
        pImpl->history.push_back(getLevel());
        
        // Store current pattern for next comparison
        pImpl->lastPattern = currentPattern;
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in detectNovelty: " + std::string(e.what()));
        throw;
    } catch (const NLMEnvironmentError& e) {
        NLM_LOG_ERROR("Environment error in detectNovelty: " + std::string(e.what()));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in detectNovelty: " + std::string(e.what()));
        // Continue with degraded functionality
    }
}

void Novelty::update(TimestepDuration dt) {
    try {
        // Validate timestep
        if (dt <= 0.0f) {
            NLM_LOG_WARNING("Invalid timestep for novelty update: " + std::to_string(dt));
            return;
        }
        
        // Decay novelty
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dt);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error updating novelty: " + std::string(e.what()));
    }
}

const std::vector<float>& Novelty::getHistory() const {
    try {
        return pImpl->history;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting novelty history: " + std::string(e.what()));
        static const std::vector<float> emptyVector;
        return emptyVector;
    }
}

void Novelty::clearHistory() {
    try {
        pImpl->history.clear();
        NLM_LOG_INFO("Novelty history cleared");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error clearing novelty history: " + std::string(e.what()));
    }
}

} // namespace nlm