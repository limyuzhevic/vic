#include "EnhancedSensoryProcessors.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct InternalSignalsProcessor::Impl {
    std::vector<float> processedSignals;
    std::vector<float> homeostaticSignals;
    
    // Neural processing state
    std::vector<float> neuralActivation;
    std::vector<float> spikeHistory;
    
    // Processing parameters
    float allostaticThreshold;
    float metabolicEfficiency;
    float neuralGain;
    
    Impl() : allostaticThreshold(2.0f), metabolicEfficiency(0.8f), neuralGain(1.5f) {}
};

InternalSignalsProcessor::InternalSignalsProcessor() : pImpl(std::make_unique<Impl>()) {}

InternalSignalsProcessor::~InternalSignalsProcessor() = default;

const char* InternalSignalsProcessor::getTypeName() const {
    return "InternalSignalsProcessor";
}

bool InternalSignalsProcessor::validateInput(const SensoryInput& input) const {
    if (!SensoryProcessor::validateInput(input)) {
        return false;
    }
    
    const auto& data = input.getData();
    return !data.empty();
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

void InternalSignalsProcessor::process(const SensoryInput& input) {
    // Validate input
    if (!validateInput(input)) {
        throw std::runtime_error("Invalid internal signals input for processing");
    }
    
    reset();
    
    const auto& data = input.getData();
    pImpl->processedSignals = data;
    
    // Apply neural processing pipeline
    computeInternalStateDynamics(pImpl->processedSignals, pImpl->neuralActivation);
    applyHomeostaticRegulation(pImpl->neuralActivation, pImpl->processedSignals);
    computeNeuralCoherence(pImpl->processedSignals, pImpl->neuralActivation);
    
    // Extract meaningful features through neural operations
    processHomeostaticSignals();
    computeAllostaticLoad();
    extractMetabolicState();
    applyInternalStateMapping();
    
    // Normalize features
    normalizeFeatures();
}

void InternalSignalsProcessor::processHomeostaticSignals() {
    // Process homeostatic signals using neural mechanisms
    std::vector<float> homeostatic;
    
    // Compute homeostatic variables from neural activity
    float avgActivation = 0.0f;
    for (float val : pImpl->neuralActivation) {
        avgActivation += val;
    }
    avgActivation /= pImpl->neuralActivation.size();
    
    // Homeostatic variables
    homeostatic.push_back(avgActivation);  // Neural activity level
    homeostatic.push_back(std::sqrt(avgActivation * (1.0f - avgActivation)));  // Excitatory/inhibitory balance
    homeostatic.push_back(pImpl->allostaticThreshold - avgActivation);  // Allostatic load
    
    // Apply sigmoidal transformation for biological plausibility
    for (float& val : homeostatic) {
        val = 1.0f / (1.0f + std::exp(-5.0f * (val - 0.5f)));
    }
    
    // Add homeostatic signals
    pImpl->homeostaticSignals.insert(pImpl->homeostaticSignals.end(), 
                                     homeostatic.begin(), homeostatic.end());
    if (pImpl->homeostaticSignals.size() > 10) {
        pImpl->homeostaticSignals.erase(pImpl->homeostaticSignals.begin(),
                                        pImpl->homeostaticSignals.begin() + 
                                        (pImpl->homeostaticSignals.size() - 10));
    }
}

void InternalSignalsProcessor::computeAllostaticLoad() {
    // Compute allostatic load using neural mechanisms
    float totalLoad = 0.0f;
    
    // Load based on deviation from homeostasis
    for (float val : pImpl->neuralActivation) {
        totalLoad += std::abs(val - 0.5f);  // Deviation from ideal 0.5
    }
    
    totalLoad /= pImpl->neuralActivation.size();
    
    // Add load to processed signals
    pImpl->processedSignals.push_back(totalLoad * pImpl->allostaticThreshold);
}

void InternalSignalsProcessor::extractMetabolicState() {
    // Extract metabolic state variables
    std::vector<float> metabolic;
    
    // Metabolic efficiency based on neural efficiency
    float neuralEfficiency = 0.0f;
    for (float val : pImpl->neuralActivation) {
        neuralEfficiency += val * (1.0f - val);  // Optimal at 0.5
    }
    neuralEfficiency /= pImpl->neuralActivation.size();
    
    metabolic.push_back(neuralEfficiency * pImpl->metabolicEfficiency);
    metabolic.push_back(1.0f - neuralEfficiency);  // Energy cost
    metabolic.push_back(std::sqrt(neuralEfficiency));  // Energy storage
    
    // Add metabolic features
    pImpl->processedSignals.insert(pImpl->processedSignals.end(), 
                                  metabolic.begin(), metabolic.end());
}

void InternalSignalsProcessor::applyInternalStateMapping() {
    // Apply internal state mapping to produce final features
    std::vector<float> mappedSignals;
    
    for (float val : pImpl->neuralActivation) {
        // Apply nonlinear transformation for state mapping
        float mapped = 0.5f * (1.0f + std::tanh(pImpl->neuralGain * (val - 0.5f)));
        mappedSignals.push_back(mapped);
    }
    
    // Add mapped signals to processed signals
    pImpl->processedSignals.insert(pImpl->processedSignals.end(), 
                                  mappedSignals.begin(), mappedSignals.end());
}

void InternalSignalsProcessor::computeInternalStateDynamics(const std::vector<float>& input, std::vector<float>& output) {
    // Compute internal state dynamics using neural mechanisms
    output.clear();
    output.reserve(input.size());
    
    // Simulate internal state dynamics with time delay and feedback
    if (!pImpl->spikeHistory.empty()) {
        for (size_t i = 0; i < input.size(); ++i) {
            float prevState = pImpl->spikeHistory[i % pImpl->spikeHistory.size()];
            
            // Differential equation approximation: dx/dt = -x + input
            float dx = -0.1f * prevState + 0.05f * input[i];
            float newState = prevState + dx;
            
            // Apply neuronal activation function
            float activation = 1.0f / (1.0f + std::exp(-10.0f * (newState - 0.5f)));
            output.push_back(activation);
        }
    } else {
        output = input;
    }
    
    // Update spike history
    pImpl->spikeHistory.assign(input.begin(), input.end());
    if (pImpl->spikeHistory.size() > 100) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                                 pImpl->spikeHistory.begin() + 
                                 (pImpl->spikeHistory.size() - 100));
    }
}

void InternalSignalsProcessor::applyHomeostaticRegulation(const std::vector<float>& input, std::vector<float>& output) {
    // Apply homeostatic regulation to maintain stability
    output.clear();
    output.reserve(input.size());
    
    // Homeostatic control using feedback
    for (size_t i = 0; i < input.size(); ++i) {
        float error = 0.5f - input[i];  // Deviation from homeostasis
        
        // Proportional-integral control
        static float integral = 0.0f;
        integral += error * 0.01f;
        
        float correction = 0.5f * error + 0.01f * integral;
        float regulated = input[i] + correction;
        
        // Bound and apply sigmoid
        regulated = std::max(0.0f, std::min(1.0f, regulated));
        output.push_back(1.0f / (1.0f + std::exp(-5.0f * (regulated - 0.5f))));
    }
}

void InternalSignalsProcessor::computeNeuralCoherence(const std::vector<float>& input, std::vector<float>& output) {
    // Compute neural coherence (synchrony) measures
    output.clear();
    output.reserve(input.size());
    
    // Compute pairwise correlations (simplified)
    float mean = 0.0f;
    for (float val : input) mean += val;
    mean /= input.size();
    
    float variance = 0.0f;
    for (float val : input) variance += (val - mean) * (val - mean);
    variance /= input.size();
    
    // Coherence as inverse coefficient of variation
    float coherence = variance > 0.0f ? 1.0f / std::sqrt(variance) : 0.0f;
    
    // Apply coherence modulation
    for (float val : input) {
        float coherenceModulated = val * coherence;
        output.push_back(coherenceModulated);
    }
}

const std::vector<float>& InternalSignalsProcessor::getFeatures() const {
    return pImpl->processedSignals;
}

size_t InternalSignalsProcessor::getFeatureDimensions() const {
    return pImpl->processedSignals.size();
}

} // namespace nlm
