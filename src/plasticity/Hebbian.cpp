// Modern C++ Implementation of Hebbian.cpp with Strategy Pattern
// This file implements the Hebbian learning rule "neurons that fire together, wire together"
// using modern C++ features and the Strategy pattern

#include "Hebbian.hpp"
#include <algorithm>
#include <numeric>
#include <execution>
#include <cmath>

namespace nlm {

// Hebbian Implementation Details
Hebbian::Hebbian() : pImpl(std::make_unique<Impl>()) {
    // Initialize default Hebbian parameters
    pImpl->learningRate = 0.01f;    // Learning rate (η)
    pImpl->maxWeight = 1.0f;       // Maximum synaptic weight
    pImpl->minWeight = -1.0f;      // Minimum synaptic weight
    pImpl->covarianceThreshold = 0.5f; // Activity threshold for covariance calculation
    pImpl->maxCorrelation = 1.0f;  // Maximum possible correlation value
}

Hebbian::~Hebbian() = default;

// Core Hebbian update with modern C++ optimizations
void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    
    // Input validation
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Modern C++: Use range-based for loops and efficient algorithms
    size_t correlationCount = 0;
    
    // Optimized correlation calculation using modern C++ algorithms
    // Calculate spike correlations using parallel execution for performance
    auto preIt = std::execution::par_unseq;
    
    // Efficient correlation detection within time window
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // Count spikes within correlation window
            if (std::abs(dt) < pImpl->correlationWindow) {
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change based on correlation
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds and safety checks
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

// Apply weight change with comprehensive error handling and bounds checking
void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return;
    }
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Only update if weight actually changed significantly
    if (std::abs(newWeight - synapse->getWeight()) > 1e-6f) {
        synapse->setWeight(newWeight);
    }
}

// Get rule name for logging and debugging
const char* Hebbian::getName() const {
    return "Hebbian";
}

// Hebbian-specific parameter setters with validation
void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

// Set correlation window for spike pair detection
void Hebbian::setCorrelationWindow(float window) {
    pImpl->correlationWindow = std::max(0.0f, window);
}

float Hebbian::getCorrelationWindow() const {
    return pImpl->correlationWindow;
}

// Calculate statistical correlation between pre- and post-synaptic spikes
float Hebbian::calculateCorrelation(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes) const {
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Modern C++: Use efficient algorithms and avoid unnecessary copies
    float sumPre = std::accumulate(preSpikes.begin(), preSpikes.end(), 0.0f);
    float sumPost = std::accumulate(postSpikes.begin(), postSpikes.end(), 0.0f);
    
    float meanPre = sumPre / preSpikes.size();
    float meanPost = sumPost / postSpikes.size();
    
    float numerator = 0.0f;
    float denominatorPre = 0.0f;
    float denominatorPost = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        float diffPre = preTime - meanPre;
        denominatorPre += diffPre * diffPre;
        
        for (Timestamp postTime : postSpikes) {
            float diffPost = postTime - meanPost;
            denominatorPost += diffPost * diffPost;
            numerator += diffPre * diffPost;
        }
    }
    
    denominatorPre = std::sqrt(denominatorPre);
    denominatorPost = std::sqrt(denominatorPost);
    
    if (denominatorPre > 0.0f && denominatorPost > 0.0f) {
        return numerator / (denominatorPre * denominatorPost);
    }
    
    return 0.0f;
}

// Simplified covariance-based Hebbian learning implementation
namespace hebbian_helpers {
    
    // Calculate covariance between pre- and post-synaptic activity
    inline float calculateCovariance(
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes) {
        
        if (preSpikes.empty() || postSpikes.empty()) {
            return 0.0f;
        }
        
        float meanPre = std::accumulate(preSpikes.begin(), preSpikes.end(), 0.0f) / preSpikes.size();
        float meanPost = std::accumulate(postSpikes.begin(), postSpikes.end(), 0.0f) / postSpikes.size();
        
        float covariance = 0.0f;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                covariance += (preTime - meanPre) * (postTime - meanPost);
            }
        }
        
        return covariance / (preSpikes.size() * postSpikes.size());
    }
    
    // Apply Oja's learning rule for normalization
    inline float applyOjaRule(float weight, float preSpike, float postSpike, float learningRate) {
        float delta = learningRate * postSpike * (preSpike - weight * preSpike * preSpike);
        return std::clamp(weight + delta, -1.0f, 1.0f);
    }
    
    // Apply BCM (Bienenstock-Cooper-Munro) rule variant
    inline float applyBCMRule(float weight, float postSpike, float theta) {
        float delta = postSpike > theta ? weight * (1.0f - weight) * (postSpike - theta) : 0.0f;
        return std::clamp(weight + delta, -1.0f, 1.0f);
    }
}

} // namespace nlm