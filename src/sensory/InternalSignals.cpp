#include "InternalSignals.hpp"

namespace nlm {

struct InternalSignalsProcessor::Impl {
    std::vector<float> processedSignals;
    std::vector<float> homeostaticSignals;
    
    Impl() = default;
};

InternalSignalsProcessor::InternalSignalsProcessor() : pImpl(new Impl) {}

InternalSignalsProcessor::~InternalSignalsProcessor() = default;

void InternalSignalsProcessor::process(const InternalSignals& signals) {
    // Real internal signal processing
    // Processes internal physiological and homeostatic signals
    
    const std::vector<float>& inputSignals = signals.getData();
    pImpl->processedSignals = inputSignals;
    
    // Generate homeostatic signals based on input
    pImpl->homeostaticSignals.clear();
    
    // Calculate allostatic load (stress level)
    float stressLevel = 0.0f;
    for (float signal : inputSignals) {
        stressLevel += std::abs(signal);
    }
    stressLevel /= static_cast<float>(inputSignals.size() + 1);
    pImpl->homeostaticSignals.push_back(stressLevel);
    
    // Calculate metabolic state (energy availability)
    float metabolicState = 0.5f;  // Baseline
    // Higher positive signals indicate energy availability
    for (size_t i = 0; i < inputSignals.size(); ++i) {
        if (i % 4 == 0) {  // Every 4th signal is metabolic
            metabolicState += inputSignals[i] * 0.1f;
        }
    }
    metabolicState = std::clamp(metabolicState, 0.0f, 1.0f);
    pImpl->homeostaticSignals.push_back(metabolicState);
    
    // Calculate arousal level
    float arousal = std::tanh(stressLevel * 2.0f - 0.5f) * metabolicState;
    pImpl->homeostaticSignals.push_back(arousal);
    
    // Apply homeostatic regulation to signals
    for (float& signal : pImpl->processedSignals) {
        // Slowing signals based on homeostatic state
        float regulation = 1.0f - pImpl->homeostaticSignals[0] * 0.3f;
        signal *= regulation;
        
        // Add noise for biological realism
        float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.01f;
        signal += noise;
    }
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

} // namespace nlm
