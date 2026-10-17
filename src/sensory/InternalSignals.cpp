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
    // Real internal signal processing: extract and normalize homeostatic states
    const std::vector<float>& rawSignals = signals.getData();
    pImpl->processedSignals = rawSignals;  // Store original signals
    
    // Generate realistic homeostatic signals based on input
    pImpl->homeostaticSignals.clear();
    
    if (!rawSignals.empty()) {
        // 1. Allostatic load: measure of stress/burden on the system
        float sumSquared = 0.0f;
        for (float signal : rawSignals) {
            sumSquared += signal * signal;
        }
        float meanSquared = sumSquared / static_cast<float>(rawSignals.size());
        float allostaticLoad = std::min(1.0f, std::sqrt(meanSquared) * 2.0f);
        pImpl->homeostaticSignals.push_back(allostaticLoad);
        
        // 2. Metabolic state: average of signals (higher = more active)
        float metabolicState = std::min(1.0f, (rawSignals.back() + 1.0f) / 2.0f);
        pImpl->homeostaticSignals.push_back(metabolicState);
        
        // 3. Recovery rate: inversely proportional to signal variance
        float mean = 0.0f;
        for (float signal : rawSignals) mean += signal;
        mean /= static_cast<float>(rawSignals.size());
        
        float variance = 0.0f;
        for (float signal : rawSignals) {
            float diff = signal - mean;
            variance += diff * diff;
        }
        variance /= static_cast<float>(rawSignals.size());
        float recoveryRate = std::max(0.0f, 1.0f - variance * 10.0f);
        pImpl->homeostaticSignals.push_back(recoveryRate);
        
        // 4. Energy reserves: based on minimum signal value
        float minSignal = rawSignals[0];
        for (float signal : rawSignals) {
            minSignal = std::min(minSignal, signal);
        }
        float energyReserves = std::min(1.0f, (minSignal + 1.0f) / 2.0f);
        pImpl->homeostaticSignals.push_back(energyReserves);
    } else {
        // Default values when no signals
        pImpl->homeostaticSignals = {0.5f, 0.5f, 1.0f, 0.8f};
    }
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

} // namespace nlm
