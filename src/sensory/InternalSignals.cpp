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
    // IMPROVED: Enhanced internal signal processing with homeostatic regulation
    pImpl->processedSignals.clear();
    pImpl->homeostaticSignals.clear();
    
    const auto& inputData = signals.getData();
    if (inputData.empty()) {
        NLM_LOG_WARNING("InternalSignalsProcessor: Empty input data");
        
        // Set default homeostatic state
        pImpl->homeostaticSignals.push_back(1.0f);  // Allostatic load
        pImpl->homeostaticSignals.push_back(0.5f);  // Metabolic state
        pImpl->homeostaticSignals.push_back(0.8f);  // Stress level
        return;
    }
    
    // Process internal signals with homeostatic regulation
    float sum = std::accumulate(inputData.begin(), inputData.end(), 0.0f);
    float mean = sum / inputData.size();
    
    // Calculate homeostasis indicators
    float range = 0.0f;
    for (float val : inputData) {
        range = std::max(range, std::abs(val));
    }
    
    // Calculate activity statistics
    float variance = 0.0f;
    for (float val : inputData) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= inputData.size();
    
    // Generate homeostatic signals based on internal state
    float allostaticLoad = std::min(1.0f, range * 2.0f);  // Load from signal range
    float metabolicState = std::clamp(mean * 0.5f + 0.5f, 0.1f, 1.0f);  // Normalized metabolic activity
    float stressLevel = std::min(1.0f, variance * 5.0f);  // Stress from variability
    
    // Apply homeostatic regulation
    float regulationFactor = 1.0f - allostaticLoad * 0.3f;  // Homeostatic correction
    regulationFactor = std::clamp(regulationFactor, 0.2f, 1.0f);
    
    // Process signals with regulation
    for (float val : inputData) {
        // Apply homeostatic regulation to internal signals
        float regulated = val * regulationFactor + mean * (1.0f - regulationFactor) * 0.1f;
        pImpl->processedSignals.push_back(regulated);
    }
    
    // Add homeostatic state indicators
    pImpl->homeostaticSignals.push_back(allostaticLoad);
    pImpl->homeostaticSignals.push_back(metabolicState);
    pImpl->homeostaticSignals.push_back(stressLevel);
    
    NLM_LOG_INFO("InternalSignalsProcessor: Processed " + std::to_string(inputData.size()) + 
                 " signals with homeostatic regulation");
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

} // namespace nlm
