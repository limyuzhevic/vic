#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;                    // Current NE concentration
    float baselineLevel;             // Baseline NE level
    float arousalLevel;             // Current arousal/vigilance level (0-1)
    float stressLevel;              // Current stress level (0-1)
    float timeConstant;              // Time constant for dynamics (ms)
    
    // Alertness and vigilance
    float alertness;                 // Alertness level
    float vigilance;                 // Vigilance level (monitoring)
    
    // Cognitive control
    float cognitiveControl;          // Cognitive control strength
    float executiveFunction;         // Executive function modulation
    
    // Fight-or-flight response
    float fightOrFlight;             // Fight-or-flight activation (0-1)
    
    // Memory modulation
    float memoryEncoding;            // Memory encoding modulation
    float memoryRetrieval;           // Memory retrieval modulation
    
    // Learning rate modulation
    float learningRate;              // Overall learning rate modulation
    float explorationBias;           // Bias toward exploration vs exploitation
    
    // Fatigue and resource depletion
    float cognitiveResources;        // Available cognitive resources (0-1)
    float fatigueLevel;              // Fatigue level (0-1)
    
    Impl() : level(0.0f)
        , baselineLevel(0.1f)
        , arousalLevel(0.5f)
        , stressLevel(0.0f)
        , timeConstant(80.0f)
        , alertness(0.5f)
        , vigilance(0.5f)
        , cognitiveControl(0.5f)
        , executiveFunction(0.5f)
        , fightOrFlight(0.0f)
        , memoryEncoding(1.0f)
        , memoryRetrieval(1.0f)
        , learningRate(1.0f)
        , explorationBias(0.5f)
        , cognitiveResources(1.0f)
        , fatigueLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates multiple cognitive and arousal functions
    float plasticityFactor = 1.0f;
    
    // Arousal modulation: moderate arousal improves learning, high arousal impairs it
    if (pImpl->arousalLevel < 0.7f) {
        // Optimal arousal zone: enhanced plasticity
        plasticityFactor *= (1.0f + pImpl->arousalLevel * 1.2f);
    } else {
        // High arousal zone: impaired plasticity (fight-or-flight response)
        plasticityFactor *= std::max(0.2f, 1.0f - (pImpl->arousalLevel - 0.7f) * 2.0f);
    }
    
    // Alertness and vigilance: enhanced focus on task-relevant information
    plasticityFactor *= (1.0f + pImpl->alertness * 0.6f);
    
    // Cognitive control: improved executive function
    plasticityFactor *= (1.0f + pImpl->cognitiveControl * 0.8f);
    
    // Learning rate: NE modulates overall learning rate
    plasticityFactor *= pImpl->learningRate;
    
    // Exploration bias: higher NE increases exploration
    plasticityFactor *= (1.0f + (pImpl->explorationBias - 0.5f) * 0.3f);
    
    // Cognitive resources: available resources affect plasticity
    plasticityFactor *= pImpl->cognitiveResources;
    
    // Apply neuromodulator-specific factors
    plasticityFactor *= (1.0f + pImpl->level * 0.8f);  // Higher NE = more modulation
    plasticityFactor = std::clamp(plasticityFactor, 0.0f, 5.0f);
    
    return plasticityFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real NE dynamics with multiple components
    
    // Convert dt to milliseconds
    float dtMs = static_cast<float>(dt) * 1000.0f;
    
    // Update arousal level based on current NE level
    pImpl->arousalLevel = 0.3f + pImpl->level * 0.4f;
    pImpl->arousalLevel = std::clamp(pImpl->arousalLevel, 0.1f, 1.0f);
    
    // Update stress level (builds up with high NE and cognitive load)
    if (pImpl->level > 0.3f) {
        pImpl->stressLevel += 0.02f * dtMs / 100.0f;
    }
    pImpl->stressLevel *= std::exp(-dtMs / 400.0f);  // Stress decays
    pImpl->stressLevel = std::clamp(pImpl->stressLevel, 0.0f, 1.0f);
    
    // Update fight-or-flight response
    if (pImpl->stressLevel > 0.7f) {
        pImpl->fightOrFlight = std::min(pImpl->fightOrFlight + 0.05f * dtMs / 100.0f, 1.0f);
    } else {
        pImpl->fightOrFlight *= std::exp(-dtMs / 600.0f);
    }
    
    // Update alertness and vigilance based on arousal
    pImpl->alertness = pImpl->arousalLevel * (1.0f - pImpl->fightOrFlight * 0.5f);
    pImpl->vigilance = 0.5f + pImpl->stressLevel * 0.5f;
    
    // Update cognitive control and executive function
    if (pImpl->fightOrFlight < 0.5f) {
        // Normal cognitive control
        pImpl->cognitiveControl = 0.6f + pImpl->arousalLevel * 0.3f;
        pImpl->executiveFunction = 0.7f + pImpl->alertness * 0.3f;
    } else {
        // Fight-or-flight impairs higher cognitive functions
        pImpl->cognitiveControl = std::max(0.2f, 0.6f - pImpl->fightOrFlight * 0.4f);
        pImpl->executiveFunction = std::max(0.3f, 0.7f - pImpl->fightOrFlight * 0.5f);
    }
    
    // Update learning rate based on arousal
    if (pImpl->arousalLevel < 0.6f) {
        // Low arousal: slower learning
        pImpl->learningRate = 0.8f + pImpl->arousalLevel * 0.5f;
    } else if (pImpl->arousalLevel < 0.8f) {
        // Optimal arousal: enhanced learning
        pImpl->learningRate = 1.2f + (pImpl->arousalLevel - 0.6f) * 2.0f;
    } else {
        // High arousal: impaired learning
        pImpl->learningRate = std::max(0.5f, 1.5f - (pImpl->arousalLevel - 0.8f) * 3.0f);
    }
    
    // Update exploration bias
    pImpl->explorationBias = 0.5f + pImpl->stressLevel * 0.3f - pImpl->fightOrFlight * 0.2f;
    pImpl->explorationBias = std::clamp(pImpl->explorationBias, 0.2f, 0.8f);
    
    // Update cognitive resources (depletion and recovery)
    if (pImpl->fightOrFlight > 0.5f) {
        // Fight-or-flight depletes resources
        pImpl->cognitiveResources -= 0.01f * dtMs / 100.0f;
    }
    
    // Recovery of cognitive resources
    pImpl->cognitiveResources += 0.005f * dtMs / 100.0f;
    pImpl->cognitiveResources = std::clamp(pImpl->cognitiveResources, 0.3f, 1.0f);
    
    // Update fatigue level
    pImpl->fatigueLevel = 1.0f - pImpl->cognitiveResources;
    
    // Update memory modulation
    pImpl->memoryEncoding = 1.0f + pImpl->arousalLevel * 0.3f - pImpl->fatigueLevel * 0.4f;
    pImpl->memoryRetrieval = 1.0f - pImpl->fightOrFlight * 0.3f + pImpl->vigilance * 0.2f;
    
    // Smooth NE level update with gradual changes
    float targetLevel = pImpl->baselineLevel + (pImpl->level * 0.3f);  // Bias toward baseline
    float alpha = 0.12f;  // Smoothing factor
    float smoothedLevel = pImpl->level * (1.0f - alpha) + targetLevel * alpha;
    
    // Apply stress modulation
    float stressModulation = 1.0f + pImpl->stressLevel * 0.3f;
    pImpl->level = smoothedLevel * stressModulation;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
}

} // namespace nlm
