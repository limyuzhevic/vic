#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    
    // Receptor dynamics
    float d1Level;           // D1 receptor activation (postsynaptic)
    float d2Level;           // D2 receptor activation (presynaptic)
    float d3Level;           // D3 receptor (modulatory)
    
    // Neuromodulatory state
    float concentration;     // Synaptic dopamine concentration
    float basalTonic;        // Baseline tonic firing (0.1-2 Hz)
    float burstPhasic;       // Phasic burst amplitude (0-20 Hz)
    
    // Plasticity modulation
    float plasticityScaling; // Scales STDP/Hebbian rates
    float learningRate;      // Learning rate from reward signals
    
    // Temporal dynamics
    float timeConstant;      // Decay time constant (ms)
    float recoveryRate;      // Recovery from depletion
    
    // Computational model
    float predictionError;   // Reward prediction error
    float integratedReward;  // Integrated reward signal
    float eligibilityTrace;  // For reward-modulated learning
    
    // Burst dynamics
    float lastBurstTime;     // Time of last dopamine burst
    float burstDuration;     // Duration of current burst
    float burstFrequency;    // Current burst frequency
    
    // State variables
    float adaptation;        // Response adaptation
    float habituation;       // Response habituation
    
    // Random generator for burst timing
    std::mt19937 rng;
    
    Impl() 
        : brain(nullptr)
        , d1Level(0.0f), d2Level(0.0f), d3Level(0.0f)
        , concentration(0.1f), basalTonic(0.5f), burstPhasic(0.0f)
        , plasticityScaling(1.0f), learningRate(0.01f)
        , timeConstant(500.0f), recoveryRate(0.01f)
        , predictionError(0.0f), integratedReward(0.0f), eligibilityTrace(0.0f)
        , lastBurstTime(0.0f), burstDuration(0.0f), burstFrequency(0.0f)
        , adaptation(0.0f), habituation(0.0f) {
        
        // Initialize random generator
        std::random_device rd;
        rng.seed(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    }
};

Dopamine::Dopamine()
    : pImpl(new Impl)
    , timeConstant_(500.0f)
    , baseline_(0.1f)
    , learningRate_(0.01f)
    , burstFrequency_(0.0f)
    , adaptation_(0.0f)
    , habituation_(0.0f)
    , lastBurstTime_(0.0f)
    , integrationTime_(0.0f)
{
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->concentration;
}

void Dopamine::setLevel(float level) {
    pImpl->concentration = std::clamp(level, 0.0f, 10.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine modulation of plasticity:
    // - D1 receptors enhance LTP and promote stability
    // - D2 receptors suppress LTD and promote flexibility
    
    float d1Effect = pImpl->d1Level * 0.8f;     // Strong LTP promotion
    float d2Effect = pImpl->d2Level * 0.4f;     // Moderate LTD suppression
    
    // D1 dominance increases stability, D2 dominance increases learning
    float plasticityFactor = 1.0f + d1Effect - d2Effect * 0.5f;
    
    // Ensure reasonable bounds
    return std::clamp(plasticityFactor, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float time = static_cast<float>(dt) * 0.001f;  // Convert to seconds
    
    // Decay towards baseline
    pImpl->concentration += (baseline_ - pImpl->concentration) * time * pImpl->recoveryRate;
    
    // Decay burst effects
    if (pImpl->burstPhasic > 0.0f) {
        float burstDecay = std::exp(-time / (pImpl->timeConstant / 1000.0f));
        pImpl->burstPhasic *= burstDecay;
    }
    
    // Update receptor activation based on concentration
    pImpl->d1Level = sigmoid(pImpl->concentration / 1.0f, 0.5f);  // EC50 = 1.0
    pImpl->d2Level = sigmoid(pImpl->concentration / 2.0f, 0.3f);  // EC50 = 2.0
    pImpl->d3Level = sigmoid(pImpl->concentration / 5.0f, 0.8f);  // EC50 = 5.0
    
    // Track burst dynamics
    if (pImpl->burstFrequency > 0.0f) {
        integrationTime_ += time;
        if (integrationTime_ > 100.0f) {  // 100ms integration window
            integrationTime_ = 0.0f;
            // Decay adaptation/habituation
            adaptation_ *= 0.95f;
            habituation_ *= 0.9f;
        }
    }
}

void Dopamine::signalReward(float reward, float predictionError) {
    // Real reward signaling based on computational neuroscience
    // David Redgrave & Peter Dayan (2001) reward prediction error model
    
    // Primary reward signal
    float rewardSignal = reward * learningRate_;
    
    // Reward prediction error modulation
    float rpe = reward - predictionError;
    
    // Dopamine burst response
    if (rpe > 0.0f) {
        // Reward prediction error -> phasic burst
        float burstAmplitude = 1.0f + rpe * 2.0f;  // Bell curve shape in reality
        pImpl->burstPhasic += burstAmplitude;
        pImpl->lastBurstTime = 0.0f;
        
        // Increase firing frequency
        burstFrequency_ = std::min(20.0f, burstFrequency_ + rpe * 10.0f);
        
        // Record successful prediction
        adaptation_ = std::max(adaptation_, 0.5f * rpe);
        
        NLM_LOG_INFO("Dopamine burst: reward = " + std::to_string(reward) + 
                    " RPE = " + std::to_string(rpe));
    } else {
        // Omission or worse-than-expected outcome -> dip
        pImpl->burstPhasic -= std::abs(rpe) * 0.5f;
        pImpl->burstPhasic = std::max(0.0f, pImpl->burstPhasic);
        
        // Decrease firing
        burstFrequency_ = std::max(0.0f, burstFrequency_ - std::abs(rpe) * 2.0f);
        
        // Negative prediction error -> habituation
        habituation_ = std::max(habituation_, 0.3f * std::abs(rpe));
        
        NLM_LOG_INFO("Dopamine dip: reward = " + std::to_string(reward) + 
                    " RPE = " + std::to_string(rpe));
    }
    
    // Update concentration with burst contribution
    pImpl->concentration += rewardSignal * 0.5f;
    
    // Update plasticity scaling
    pImpl->plasticityScaling = 1.0f + rpe * 0.5f;
}

void Dopamine::signalUnexpectedOutcome(float outcome, float expected) {
    // For when outcomes are not predicted (surprise)
    float surprise = std::abs(outcome - expected);
    
    if (surprise > 0.1f) {
        // High surprise -> increased dopamine for exploration
        float noveltySignal = surprise * 0.5f;
        pImpl->burstPhasic += noveltySignal;
        
        // Boost learning rate
        pImpl->learningRate = std::min(0.1f, pImpl->learningRate + surprise * 0.01f);
        
        // Increase plasticity factor for flexible learning
        pImpl->plasticityScaling = std::max(pImpl->plasticityScaling, 1.5f);
        
        NLM_LOG_INFO("Dopamine novelty signal: surprise = " + std::to_string(surprise));
    }
}

void Dopamine::signalTemporalDifference(float tdError) {
    // Temporal difference error (Sutton-Barto learning)
    pImpl->predictionError = tdError;
    
    // Phasic response to TD error
    if (tdError != 0.0f) {
        float tdSignal = tdError * learningRate_;
        if (tdError > 0.0f) {
            pImpl->burstPhasic += tdSignal;
        } else {
            pImpl->burstPhasic -= std::abs(tdError) * 0.5f;
            pImpl->burstPhasic = std::max(0.0f, pImpl->burstPhasic);
        }
        
        // Update eligibility trace
        pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + std::abs(tdError));
    }
}

void Dopamine::engageVolumeTransmission() {
    // Diffuse modulation affecting many brain regions
    pImpl->concentration = std::max(pImpl->concentration, baseline_ * 1.5f);
    
    // Broad receptor activation
    pImpl->d1Level = std::min(1.0f, pImpl->d1Level + 0.1f);
    pImpl->d2Level = std::min(1.0f, pImpl->d2Level + 0.1f);
}

void Dopamine::engagePointToPoint() {
    // Targeted synaptic release for specific learning
    pImpl->burstPhasic += 2.0f;  // Strong localized signal
    pImpl->burstFrequency = std::max(pImpl->burstFrequency, 15.0f);
}

void Dopamine::decayToBaseline(TimestepDuration dt) {
    // Natural decay to baseline levels
    float time = static_cast<float>(dt) * 0.001f;
    pImpl->concentration = baseline_ + (pImpl->concentration - baseline_) * 
        std::exp(-time / (pImpl->timeConstant / 1000.0f));
}

// Helper function for sigmoid activation
inline float Dopamine::sigmoid(float x, float k) {
    return 1.0f / (1.0f + std::exp(-k * x));
}

} // namespace nlm
