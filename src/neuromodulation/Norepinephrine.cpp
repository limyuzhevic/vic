#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Norepinephrine::Impl {
    // Core norepinephrine dynamics
    float level;
    float baseline;              // Tonic NE baseline
    float peak;                  // Maximum achievable level
    
    // Arousal and vigilance control
    float arousalLevel;          // Current arousal level
    float vigilanceLevel;        // Current vigilance level
    float locusCoeruleusActivity; // LC neuron firing rate
    
    // Stress response system
    float stressLevel;            // Current stress level
    float adaptationLevel;        // Adaptation to stress
    float recoveryRate;           // Recovery from stress
    
    // Sustained attention mechanism
    bool isInVigilanceMode;      // Whether in sustained attention mode
    float vigilanceStartTime;    // Time when vigilance mode began
    float vigilanceDuration;     // Duration of current vigilance session
    float attentionalFocus;      // Current attentional focus
    
    // Temporal dynamics
    float arousalDecay;          // Arousal decay rate
    float vigilanceDecay;        // Vigilance decay rate
    float stressDecay;            // Stress decay rate
    float adaptationGrowth;       // Adaptation growth rate
    
    // State variables
    float tonicFiring;            // Baseline firing rate
    float phasicFiring;           // Phasic firing bursts
    float recoveryTimer;          // Recovery timer
    float stressResponseTimer;    // Stress response timer
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Historical stress tracking
    std::vector<float> stressHistory;
    float maxStressHistory;       // Maximum stress recorded
    
    // Neural gain modulation
    float neuralGain;             // Overall neural gain modulation
    float burstGain;              // Gain during phasic bursts
    float sustainGain;            // Sustained gain level
    
    Impl() : level(0.05f), baseline(0.05f), peak(1.0f)
           , arousalLevel(0.05f), vigilanceLevel(0.05f), locusCoeruleusActivity(0.05f)
           , stressLevel(0.0f), adaptationLevel(0.0f), recoveryRate(0.03f)
           , isInVigilanceMode(false), vigilanceStartTime(0.0f), vigilanceDuration(0.0f)
           , attentionalFocus(0.5f), arousalDecay(0.1f), vigilanceDecay(0.05f)
           , stressDecay(0.08f), adaptationGrowth(0.01f)
           , tonicFiring(0.1f), phasicFiring(0.0f), recoveryTimer(0.0f), stressResponseTimer(0.0f)
           , neuralGain(1.0f), burstGain(2.0f), sustainGain(1.5f) {
        rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 0.1f);
        stressHistory.reserve(20);
    }
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update related state variables
    pImpl->arousalLevel = std::min(pImpl->arousalLevel, level);
    pImpl->vigilanceLevel = std::min(pImpl->vigilanceLevel, level);
}

float Norepinephrine::getPlasticityFactor() const {
    // Biologically plausible NE-modulated plasticity factor
    // NE enhances plasticity during stress/arousal for adaptive learning
    float arousalComponent = pImpl->arousalLevel * 0.6f;
    float vigilanceComponent = pImpl->vigilanceLevel * 0.4f;
    
    // Combined effect with stress modulation
    float stressModulation = 1.0f + pImpl->stressLevel * 0.5f;
    stressModulation = std::min(stressModulation, 2.0f);
    
    float plasticityFactor = (arousalComponent + vigilanceComponent) * stressModulation;
    
    // Apply sigmoid function for bounded response
    plasticityFactor = 1.0f / (1.0f + std::exp(-10.0f * (plasticityFactor - 0.5f)));
    
    return plasticityFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Update arousal dynamics
    pImpl->arousalLevel = std::max(pImpl->baseline, 
                                  pImpl->arousalLevel - pImpl->arousalDecay * dtFloat);
    
    // Update vigilance dynamics (slower decay than arousal)
    if (pImpl->isInVigilanceMode) {
        pImpl->vigilanceLevel = std::min(pImpl->peak, pImpl->vigilanceLevel + 0.02f * dtFloat);
        
        pImpl->vigilanceStartTime += dtFloat;
        if (pImpl->vigilanceStartTime >= pImpl->vigilanceDuration) {
            resetVigilance();
        }
    } else {
        pImpl->vigilanceLevel *= (1.0f - pImpl->vigilanceDecay * dtFloat);
    }
    
    // Update stress dynamics
    if (pImpl->stressLevel > 0.0f) {
        pImpl->stressLevel *= (1.0f - pImpl->stressDecay * dtFloat);
        pImpl->adaptationLevel += pImpl->adaptationGrowth * dtFloat;
    }
    
    // Update recovery timer
    if (pImpl->recoveryTimer > 0.0f) {
        pImpl->recoveryTimer -= dtFloat;
    }
    
    // Update stress response timer
    if (pImpl->stressResponseTimer > 0.0f) {
        pImpl->stressResponseTimer -= dtFloat;
    }
    
    // Calculate locus coeruleus activity
    float tonicComponent = pImpl->tonicFiring * pImpl->level;
    float phasicComponent = pImpl->phasicFiring * pImpl->stressLevel;
    pImpl->locusCoeruleusActivity = tonicComponent + phasicComponent;
    pImpl->locusCoeruleusActivity = std::min(pImpl->locusCoeruleusActivity, 1.0f);
    
    // Calculate neural gain
    pImpl->neuralGain = 1.0f + pImpl->arousalLevel * 1.5f + pImpl->vigilanceLevel * 0.5f;
    pImpl->neuralGain = std::min(pImpl->neuralGain, 3.0f);
    
    // Update stress history
    if (pImpl->stressLevel > pImpl->maxStressHistory) {
        pImpl->maxStressHistory = pImpl->stressLevel;
    }
    
    // Maintain stress history (rolling buffer)
    if (pImpl->stressHistory.size() >= 20) {
        pImpl->stressHistory.erase(pImpl->stressHistory.begin());
    }
    pImpl->stressHistory.push_back(pImpl->stressLevel);
}

void Norepinephrine::increaseArousal(float amount) {
    // Increase arousal level (e.g., due to salient stimuli)
    pImpl->arousalLevel = std::min(pImpl->peak, pImpl->arousalLevel + amount);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.5f);
    
    // Trigger stress response if arousal is high
    if (pImpl->arousalLevel > 0.7f) {
        triggerStressResponse(pImpl->arousalLevel - 0.7f);
    }
}

void Norepinephrine::decreaseArousal(float amount) {
    // Decrease arousal level (e.g., due to relaxation)
    pImpl->arousalLevel = std::max(pImpl->baseline, pImpl->arousalLevel - amount);
    
    // Gradual decrease in overall level
    if (pImpl->level > pImpl->baseline) {
        pImpl->level *= (1.0f - amount);
    }
}

void Norepinephrine::triggerVigilance(boost::optional<float> duration) {
    // Enter vigilance (sustained attention) mode
    if (!pImpl->isInVigilanceMode) {
        pImpl->isInVigilanceMode = true;
        pImpl->vigilanceStartTime = 0.0f;
        
        // Set duration
        if (duration) {
            pImpl->vigilanceDuration = *duration;
        } else {
            pImpl->vigilanceDuration = 30.0f; // Default 30 seconds
        }
        
        // Increase arousal and vigilance
        pImpl->vigilanceLevel = 1.0f;
        pImpl->arousalLevel = std::min(pImpl->peak, pImpl->arousalLevel + 0.3f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
        
        NLM_LOG_INFO("NE: Entering vigilance mode for " + std::to_string(pImpl->vigilanceDuration) + " seconds");
    }
}

void Norepinephrine::resetVigilance() {
    if (pImpl->isInVigilanceMode) {
        pImpl->isInVigilanceMode = false;
        pImpl->vigilanceLevel *= 0.5f; // Gradual decrease
        pImpl->vigilanceStartTime = 0.0f;
        
        NLM_LOG_INFO("NE: Exiting vigilance mode");
    }
}

void Norepinephrine::triggerStressResponse(float intensity) {
    // Trigger stress response based on high arousal
    if (intensity > 0.0f) {
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + intensity);
        pImpl->stressResponseTimer = 60.0f; // 60 seconds stress response
        
        // Increase locus coeruleus phasic firing
        pImpl->phasicFiring = intensity * 3.0f;
        
        // Enhance arousal
        pImpl->arousalLevel = std::min(pImpl->peak, pImpl->arousalLevel + intensity * 0.5f);
        
        NLM_LOG_INFO("NE: Stress response triggered (intensity: " + std::to_string(intensity) + ")");
    }
}

void Norepinephrine::adaptToStress(float adaptationRate) {
    // Adapt to stress level (homeostatic response)
    if (pImpl->stressLevel > 0.3f) {
        pImpl->adaptationLevel += adaptationRate * pImpl->stressLevel;
        pImpl->adaptationLevel = std::min(pImpl->adaptationLevel, 1.0f);
        
        // Reduce arousal based on adaptation
        pImpl->arousalLevel *= (1.0f - pImpl->adaptationLevel * 0.2f);
        
        NLM_LOG_INFO("NE: Adapting to stress (adaptation: " + std::to_string(pImpl->adaptationLevel) + ")");
    }
}

void Norepinephrine::enhanceSensoryProcessing(std::vector<float>& sensoryInput, float gainFactor) {
    // Enhance sensory processing based on NE level
    // NE increases sensory gain, especially during stress/arousal
    
    float currentNELevel = getLevel();
    
    // Calculate gain enhancement
    float sensoryGain = 1.0f + currentNELevel * pImpl->neuralGain * gainFactor;
    
    // Apply gain modulation to sensory input
    for (float& value : sensoryInput) {
        // Enhance signal components
        value *= sensoryGain;
        
        // Apply noise modulation
        if (currentNELevel > 0.5f) {
            // Increase variability for high arousal
            float noise = normalDist(pImpl->rng) * 0.05f;
            value += noise;
        }
        
        // Clip to reasonable range
        value = std::max(-10.0f, std::min(10.0f, value));
    }
}

void Norepinephrine::modulateAlertness(float currentArousal) {
    // Modulate alertness based on current arousal level
    // Higher arousal increases alertness and reaction speed
    
    float alertnessLevel = std::min(1.0f, currentArousal * 1.5f);
    
    // Store alertness level for use by other systems
    pImpl->attentionalFocus = alertnessLevel;
}

void Norepinephrine::maintainVigilance(std::vector<float>& neuralActivity, float threshold) {
    // Maintain vigilance by enhancing relevant neural activity
    // Suppresses irrelevant activity, enhances task-relevant signals
    
    float currentNELevel = getLevel();
    
    // Calculate vigilance enhancement factor
    float vigilanceFactor = currentNELevel * (pImpl->isInVigilanceMode ? 1.5f : 1.0f);
    
    // Apply vigilance modulation
    for (float& activity : neuralActivity) {
        if (std::abs(activity) > threshold) {
            // Enhance salient activities
            activity *= (1.0f + vigilanceFactor * 0.5f);
        } else {
            // Suppress sub-threshold activities
            if (pImpl->isInVigilanceMode) {
                activity *= 0.5f;
            }
        }
    }
}

void Norepinephrine::resetNeuralGain(std::vector<float>& neuralActivity) {
    // Reset neural gain towards baseline
    // Gradual normalization of neural activity levels
    
    for (float& activity : neuralActivity) {
        // Apply gentle decay towards zero
        activity *= 0.95f;
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getLocusCoeruleusActivity() const {
    return pImpl->locusCoeruleusActivity;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getAdaptationLevel() const {
    return pImpl->adaptationLevel;
}

float Norepinephrine::getSustainedAttention() const {
    return pImpl->attentionalFocus;
}

} // namespace nlm