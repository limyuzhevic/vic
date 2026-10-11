#include "InternalSignals.hpp"
#include "AgentBrain.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct InternalSignalsProcessor::Impl {
    std::vector<float> processedSignals;
    std::vector<float> homeostaticSignals;
    std::vector<float> internalSpikeRates;
    std::vector<float> energyStates;
    float allostaticLoad;
    float metabolicState;
    
    Impl() : allostaticLoad(0.0f), metabolicState(0.5f) {
        internalSpikeRates.reserve(128);
        energyStates.reserve(128);
    }
};

InternalSignalsProcessor::InternalSignalsProcessor() : pImpl(new Impl) {}

InternalSignalsProcessor::~InternalSignalsProcessor() = default;

void InternalSignalsProcessor::process(const InternalSignals& signals) {
    // Real internal signal processing using NLM's neural machinery
    // Process internal state signals through internal sensory neurons
    // Implement homeostatic processing using biologically plausible dynamics
    
    pImpl->processedSignals.clear();
    pImpl->homeostaticSignals.clear();
    pImpl->internalSpikeRates.clear();
    pImpl->energyStates.clear();
    
    const auto& signalData = signals.getData();
    size_t dimensions = signals.getDimensions();
    
    // Reset homeostatic baseline
    pImpl->allostaticLoad = 0.0f;
    pImpl->metabolicState = 0.5f;
    
    // Process internal signals with biologically realistic transduction
    for (size_t i = 0; i < dimensions; ++i) {
        float signalValue = signalData[i];
        
        // Normalize internal signal to -1.0 to 1.0 range
        float normalized = signalValue * 2.0f - 1.0f;
        
        // Internal sensory neuron response: sigmoidal transduction
        // Represents how internal state changes affect neural firing
        float baseFiring = 5.0f;  // Baseline internal neuron activity
        float internalResponse = baseFiring / (1.0f + std::exp(-normalized * 3.0f));
        
        // Add homeostatic modulation
        float homeostaticFactor = 1.0f - std::abs(pImpl->allostaticLoad) * 0.5f;
        float firingRate = internalResponse * homeostaticFactor;
        
        pImpl->internalSpikeRates.push_back(std::max(0.0f, std::min(firingRate, 50.0f)));
        
        // Energy state calculation (simplified homeostasis)
        // Positive signals increase energy, negative signals decrease it
        float energyChange = normalized * 0.02f;  // Slow energy dynamics
        pImpl->energyStates.push_back(std::max(0.0f, std::min(pImpl->metabolicState + energyChange, 1.0f)));
        
        // Process homeostatic signals based on neural activity
        float homeostaticSignal = 0.0f;
        
        if (i == 0) {
            // Energy/metabolic state (homeostatic drive)
            homeostaticSignal = (pImpl->energyStates.back() - 0.5f) * 2.0f;  // 0-1 range
            pImpl->metabolicState = pImpl->energyStates.back();
        } else if (i == 1) {
            // Stress/uncertainty (allostatic load)
            homeostaticSignal = normalized * 0.5f;  // Response to stress signals
            pImpl->allostaticLoad = std::clamp(pImpl->allostaticLoad + normalized * 0.01f, -1.0f, 1.0f);
        } else if (i == 2) {
            // Temperature/thermal state (thermoregulation)
            homeostaticSignal = std::sin(normalized * 3.14159f) * 0.5f;  // Oscillatory response
        } else if (i == 3) {
            // Oxygen/CO2 balance (respiratory drive)
            homeostaticSignal = normalized * 0.8f;  // Strong drive response
        } else {
            // General internal state integration
            homeostaticSignal = normalized * 0.3f;
        }
        
        pImpl->homeostaticSignals.push_back(homeostaticSignal);
        pImpl->processedSignals.push_back(homeostaticSignal);
    }
    
    // Additional homeostatic regulation (slow variables)
    // Generate system-level homeostatic signals based on accumulated internal activity
    float totalActivity = 0.0f;
    for (size_t i = 0; i < pImpl->internalSpikeRates.size(); ++i) {
        totalActivity += pImpl->internalSpikeRates[i];
    }
    
    // Allostatic load increases with sustained internal activity
    float activityLoad = totalActivity * 0.001f;  // Scaling factor
    pImpl->allostaticLoad = (pImpl->allostaticLoad + activityLoad * 0.1f) * 0.95f;  // Decay towards baseline
    
    // Metabolic recovery factor
    float recoveryFactor = std::exp(-pImpl->allostaticLoad * 0.5f);
    pImpl->metabolicState = std::clamp(pImpl->metabolicState * recoveryFactor, 0.1f, 1.0f);
    
    // Generate final homeostatic signals for output
    pImpl->homeostaticSignals.push_back(pImpl->allostaticLoad);  // Allostatic load
    pImpl->homeostaticSignals.push_back(pImpl->metabolicState);  // Metabolic state
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

}
