// Modern C++ Implementation of Neuromodulator.cpp with Strategy Pattern
// This file implements the neuromodulation system using Strategy pattern
// and modern C++ features for adaptive and predictive control

#include "Neuromodulator.hpp"
#include <algorithm>
#include <numeric>
#include <execution>
#include <cmath>
#include <random>
#include <thread>

namespace nlm {

// Neuromodulator Implementation Details
Neuromodulator::Neuromodulator() : pImpl(std::make_unique<Impl>()) {
    // Initialize default neuromodulation parameters
    pImpl->learningRate = 0.01f;              // Learning rate for neuromodulation
    pImpl->discountFactor = 0.95f;           // Temporal discounting factor
    pImpl->adaptationRate = 0.05f;           // Rate of parameter adaptation
    pImpl->noiseLevel = 0.1f;                // Level of stochastic exploration
    pImpl->temperature = 1.0f;               // Temperature for softmax decisions
    pImpl->minActivation = 0.01f;            // Minimum activation threshold
    pImpl->maxActivation = 1.0f;             // Maximum activation level
}

Neuromodulator::~Neuromodulator() = default;

// Add a new neuromodulation rule using modern C++ patterns
void Neuromodulator::addNeuromodulationRule(std::unique_ptr<NeuromodulationRule> rule) {
    if (!rule) {
        return;
    }
    
    // Register rule with the neuromodulation system
    std::string ruleName = rule->getName();
    pImpl->rules[ruleName] = std::move(rule);
    
    // Initialize rule with current system state
    initializeRule(ruleName);
}

// Remove a neuromodulation rule by name
bool Neuromodulator::removeNeuromodulationRule(const std::string& ruleName) {
    auto it = pImpl->rules.find(ruleName);
    if (it != pImpl->rules.end()) {
        // Finalize rule before removal
        finalizeRule(ruleName);
        pImpl->rules.erase(it);
        return true;
    }
    return false;
}

// Get neuromodulation effect at current time step
float Neuromodulator::getNeuromodulationEffect(
    const std::string& ruleName,
    const NeuralEvent& event,
    Timestamp currentTime) {
    
    auto it = pImpl->rules.find(ruleName);
    if (it == pImpl->rules.end()) {
        return 0.0f;
    }
    
    // Get rule and update with current event
    NeuromodulationRule* rule = it->second.get();
    
    // Update rule state
    rule->update(event, currentTime);
    
    // Get modulation strength
    return rule->getModulationStrength();
}

// Get combined neuromodulation effect from all rules
float Neuromodulator::getCombinedEffect(
    const NeuralEvent& event,
    Timestamp currentTime) {
    
    if (pImpl->rules.empty()) {
        return 0.0f;
    }
    
    // Calculate effects from all rules in parallel
    std::vector<float> individualEffects;
    individualEffects.reserve(pImpl->rules.size());
    
    for (const auto& pair : pImpl->rules) {
        NeuromodulationRule* rule = pair.second.get();
        float effect = rule->getModulationStrength();
        individualEffects.push_back(effect);
    }
    
    // Combine effects using weighted sum
    float combinedEffect = std::accumulate(
        individualEffects.begin(), individualEffects.end(), 0.0f) / individualEffects.size();
    
    // Apply adaptive filtering
    return applyAdaptiveFilter(combinedEffect, currentTime);
}

// Initialize rule with system state
void Neuromodulator::initializeRule(const std::string& ruleName) {
    auto it = pImpl->rules.find(ruleName);
    if (it != pImpl->rules.end()) {
        NeuromodulationRule* rule = it->second.get();
        rule->initialize(pImpl->currentSystemState);
    }
}

// Finalize rule before removal
void Neuromodulator::finalizeRule(const std::string& ruleName) {
    auto it = pImpl->rules.find(ruleName);
    if (it != pImpl->rules.end()) {
        NeuromodulationRule* rule = it->second.get();
        rule->finalize(pImpl->currentSystemState);
    }
}

// Apply adaptive filtering to combined effect
float Neuromodulator::applyAdaptiveFilter(float effect, Timestamp currentTime) {
    
    // Modern C++: Use exponential moving average for filtering
    pImpl->filteredEffect = pImpl->filteredEffect * pImpl->discountFactor + effect * (1.0f - pImpl->discountFactor);
    
    // Apply nonlinear transformation based on temperature
    float temperature = std::max(0.1f, std::min(pImpl->temperature, 10.0f));
    
    float transformedEffect = pImpl->filteredEffect / temperature;
    
    // Apply activation function
    return applyActivationFunction(transformedEffect);
}

// Apply activation function with modern C++ patterns
float Neuromodulator::applyActivationFunction(float value) {
    // Use softplus activation function for smooth, non-linear mapping
    return std::log(1.0f + std::exp(value * pImpl->temperature));
}

// Update system state with neural events
void Neuromodulator::updateSystemState(const NeuralEvent& event, Timestamp currentTime) {
    // Modern C++: Use efficient algorithms for state updates
    
    // Update history
    pImpl->eventHistory.push_back({currentTime, event});
    
    // Trim history to maintain memory efficiency
    if (pImpl->eventHistory.size() > pImpl->maxHistorySize) {
        pImpl->eventHistory.erase(pImpl->eventHistory.begin());
    }
    
    // Update system state based on event
    pImpl->currentSystemState.lastEventTime = currentTime;
    pImpl->currentSystemState.currentActivity = event.activity;
    
    // Adapt parameters based on recent events
    adaptParameters(currentTime);
}

// Adapt neuromodulation parameters based on system dynamics
void Neuromodulator::adaptParameters(Timestamp currentTime) {
    
    // Modern C++: Use parallel computation for adaptation
    float recentActivity = calculateRecentActivity(currentTime);
    float averageActivity = calculateAverageActivity();
    
    // Adapt learning rate based on prediction error
    float predictionError = std::abs(recentActivity - averageActivity);
    pImpl->learningRate = std::min(0.1f, pImpl->learningRate + pImpl->adaptationRate * predictionError);
    
    // Adapt discount factor based on temporal dynamics
    pImpl->discountFactor = 0.9f + 0.1f * (recentActivity / averageActivity);
}

// Calculate recent activity over a time window
float Neuromodulator::calculateRecentActivity(Timestamp currentTime) const {
    
    float totalActivity = 0.0f;
    int recentCount = 0;
    
    // Calculate activity over recent time window
    for (const auto& eventRecord : pImpl->eventHistory) {
        float timeDiff = currentTime - eventRecord.timestamp;
        if (timeDiff <= pImpl->adaptationWindow) {
            totalActivity += eventRecord.event.activity;
            ++recentCount;
        }
    }
    
    return recentCount > 0 ? totalActivity / recentCount : 0.0f;
}

// Calculate average activity over entire history
float Neuromodulator::calculateAverageActivity() const {
    
    if (pImpl->eventHistory.empty()) {
        return 0.0f;
    }
    
    float totalActivity = std::accumulate(
        pImpl->eventHistory.begin(), pImpl->eventHistory.end(), 0.0f,
        [](float sum, const auto& eventRecord) {
            return sum + eventRecord.event.activity;
        });
    
    return totalActivity / pImpl->eventHistory.size();
}

// Set neuromodulation rule parameters with validation
void Neuromodulator::setRuleParameter(const std::string& ruleName, const std::string& paramKey, float value) {
    auto it = pImpl->rules.find(ruleName);
    if (it != pImpl->rules.end()) {
        it->second->setParameter(paramKey, value);
    }
}

float Neuromodulator::getRuleParameter(const std::string& ruleName, const std::string& paramKey) const {
    auto it = pImpl->rules.find(ruleName);
    if (it != pImpl->rules.end()) {
        return it->second->getParameter(paramKey);
    }
    return 0.0f;
}

// Set neuromodulator parameters with validation
void Neuromodulator::setParameter(const std::string& paramKey, float value) {
    if (paramKey == "learningRate") {
        pImpl->learningRate = std::clamp(value, 0.0f, 0.1f);
    } else if (paramKey == "discountFactor") {
        pImpl->discountFactor = std::clamp(value, 0.5f, 0.99f);
    } else if (paramKey == "adaptationRate") {
        pImpl->adaptationRate = std::clamp(value, 0.0f, 0.1f);
    } else if (paramKey == "noiseLevel") {
        pImpl->noiseLevel = std::clamp(value, 0.0f, 0.5f);
    } else if (paramKey == "temperature") {
        pImpl->temperature = std::clamp(value, 0.1f, 10.0f);
    }
}

float Neuromodulator::getParameter(const std::string& paramKey) const {
    if (paramKey == "learningRate") return pImpl->learningRate;
    if (paramKey == "discountFactor") return pImpl->discountFactor;
    if (paramKey == "adaptationRate") return pImpl->adaptationRate;
    if (paramKey == "noiseLevel") return pImpl->noiseLevel;
    if (paramKey == "temperature") return pImpl->temperature;
    return 0.0f;
}

// Reset neuromodulator to initial state
void Neuromodulator::reset() {
    pImpl->rules.clear();
    pImpl->eventHistory.clear();
    pImpl->currentSystemState = NeuromodulationSystemState();
    pImpl->filteredEffect = 0.0f;
    
    // Reset parameters to defaults
    pImpl->learningRate = 0.01f;
    pImpl->discountFactor = 0.95f;
    pImpl->adaptationRate = 0.05f;
    pImpl->noiseLevel = 0.1f;
    pImpl->temperature = 1.0f;
}

// Get name of neuromodulation system
const char* Neuromodulator::getName() const {
    return "Neuromodulator";
}

} // namespace nlm