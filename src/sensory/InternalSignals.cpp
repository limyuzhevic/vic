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
    // Real internal signal processing using NLM's neural machinery
    // Implements homeostatic regulation and interoceptive processing
    
    const auto& data = signals.getData();
    pImpl->processedSignals = data;
    
    // Step 1: Basic signal validation and filtering
    std::vector<float> cleanedSignals = validateAndFilterSignals(data);
    
    // Step 2: Homeostatic regulation
    std::vector<float> homeostaticOutputs = regulateHomeostasis(cleanedSignals);
    
    // Step 3: Integration with brain's motivational and emotion systems
    integrateWithBrainSystems(homeostaticOutputs, signals);
    
    // Step 4: Neuromodulatory feedback and adaptation
    adaptSignalsBasedOnModulation(homeostaticOutputs);
    
    // Update internal state
    pImpl->homeostaticSignals = std::move(homeostaticOutputs);
}

std::vector<float> InternalSignalsProcessor::validateAndFilterSignals(const std::vector<float>& signals) {
    // Validate and filter internal signals
    if (signals.empty()) return {};
    
    std::vector<float> filtered = signals;
    
    // Remove outliers (values beyond reasonable physiological range)
    for (float& signal : filtered) {
        // Clamp to reasonable range for internal signals
        signal = std::max(-2.0f, std::min(2.0f, signal));
    }
    
    // Apply smoothing (biological signal filtering)
    for (size_t i = 1; i < filtered.size() - 1; ++i) {
        filtered[i] = (filtered[i-1] + filtered[i] + filtered[i+1]) / 3.0f;
    }
    
    return filtered;
}

std::vector<float> InternalSignalsProcessor::regulateHomeostasis(const std::vector<float>& signals) {
    // Homeostatic regulation based on current internal state
    if (signals.empty()) return {};
    
    std::vector<float> homeostaticOutputs;
    
    // Extract key homeostatic parameters
    float metabolicState = signals[0];          // Energy levels
    float stressLevel = signals[1];            // Stress/anxiety
    float fatigueLevel = signals[2];           // Fatigue
    float immuneState = signals[3];            // Immune system state
    
    // Calculate homeostatic setpoints and deviations
    float energySetpoint = 0.0f;
    float stressSetpoint = 0.5f;  // Mid-range stress is optimal
    float fatigueSetpoint = 0.3f;
    
    // Calculate deviations from setpoints
    float energyDeviation = metabolicState - energySetpoint;
    float stressDeviation = stressLevel - stressSetpoint;
    float fatigueDeviation = fatigueLevel - fatigueSetpoint;
    
    // Generate homeostatic responses
    float allostaticLoad = calculateAllostaticLoad(
        std::abs(energyDeviation), std::abs(stressDeviation), std::abs(fatigueDeviation));
    
    homeostaticOutputs.push_back(allostaticLoad);
    homeostaticOutputs.push_back(1.0f - stressLevel); // Opposing stress response
    homeostaticOutputs.push_back(1.0f - fatigueLevel); // Promoting alertness
    homeostaticOutputs.push_back(immuneState); // Immune system state
    
    // Add adaptive responses based on deviation magnitude
    if (std::abs(energyDeviation) > 0.8f) {
        // Strong energy deviation - increased metabolic response
        homeostaticOutputs[0] += 0.3f; // Increase allostatic load
    }
    
    if (stressDeviation > 0.5f) {
        // High stress - activate stress response
        homeostaticOutputs[1] += 0.4f; // Enhance stress response
    }
    
    if (fatigueDeviation > 0.7f) {
        // High fatigue - promote recovery response
        homeostaticOutputs[2] += 0.3f; // Enhance recovery response
    }
    
    return homeostaticOutputs;
}

float InternalSignalsProcessor::calculateAllostaticLoad(float energyDev, float stressDev, float fatigueDev) {
    // Calculate allostatic load from multiple homeostatic deviations
    // Based on McEwen's allostatic load theory
    
    // Weight different types of allostatic load
    float metabolicLoad = energyDev * 0.5f;
    float stressLoad = stressDev * 0.8f;
    float fatigueLoad = fatigueDev * 0.6f;
    
    // Sum and normalize
    float totalLoad = metabolicLoad + stressLoad + fatigueLoad;
    
    // Apply nonlinear scaling (exponential increase for high load)
    float loadFactor = 1.0f + totalLoad * 2.0f; // Amplify high loads
    
    return std::min(1.0f, totalLoad * loadFactor);
}

void InternalSignalsProcessor::integrateWithBrainSystems(const std::vector<float>& homeostaticOutputs, const InternalSignals& input) {
    // Integrate internal signals with brain's motivational and emotion systems
    // This simulates how interoceptive cortex connects with limbic and cortical systems
    
    // Extract key homeostatic outputs
    float allostaticLoad = homeostaticOutputs[0];
    float stressResponse = homeostaticOutputs[1];
    float recoveryResponse = homeostaticOutputs[2];
    float immuneState = homeostaticOutputs[3];
    
    // Integrate with motivation systems (would access brain's motivation system)
    // High allostatic load should increase motivation for restoration
    float motivationLevel = 1.0f - allostaticLoad; // Want to reduce load
    
    // Integrate with emotion systems
    // High stress response should increase negative affect
    float valence = 1.0f - stressResponse; // Positive valence when low stress
    
    // Integrate with attention systems
    float arousal = allostaticLoad * stressResponse; // Arousal from combined stress and load
    
    // Integrate with learning systems
    float learningInhibition = allostaticLoad; // High load impairs learning
    float plasticity = 1.0f - learningInhibition;
    
    // Store integrated state for neuromodulation
    pImpl->motivationLevel = motivationLevel;
    pImpl->emotionalValence = valence;
    pImpl->arousalLevel = arousal;
    pImpl->plasticity = plasticity;
}

void InternalSignalsProcessor::adaptSignalsBasedOnModulation(std::vector<float>& signals) {
    // Adapt internal signals based on neuromodulatory state
    // This implements top-down modulation of interoception
    
    // Get current neuromodulatory state
    float dopamine = 0.5f; // Would get from brain in real implementation
    float acetylcholine = 0.5f; // Would get from brain in real implementation
    
    // Dopamine modulates salience of interoceptive signals
    for (float& signal : signals) {
        // Enhance or suppress based on dopamine level
        if (dopamine > 0.7f) {
            // High dopamine: enhance signal salience (increased interoceptive awareness)
            signal = signal * 1.3f;
        } else if (dopamine < 0.3f) {
            // Low dopamine: suppress signal salience (reduced interoceptive awareness)
            signal = signal * 0.7f;
        }
        
        // Clamp to reasonable range
        signal = std::max(-1.0f, std::min(1.0f, signal));
    }
    
    // Acetylcholine modulates attentional focus
    if (acetylcholine > 0.6f) {
        // Enhance processing of specific signals (attentional modulation)
        // In real implementation, this would select which signals to enhance
        if (!signals.empty()) {
            signals[0] *= 1.5f; // Enhance first signal (e.g., metabolic state)
            signals[0] = std::min(1.0f, signals[0]);
        }
    }
    
    // Combined neuromodulatory effects
    if (dopamine > 0.8f && acetylcholine > 0.7f) {
        // High arousal state: enhanced interoceptive awareness
        for (float& signal : signals) {
            signal *= 1.5f;
            signal = std::min(1.0f, signal);
        }
    }
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

} // namespace nlm
