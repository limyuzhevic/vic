#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>

namespace nlm {

struct Dopamine::Impl {
    // Neuroscience-based parameters
    static constexpr float DEFAULT_BASELINE = 0.25f;           // 25% of peak
    static constexpr float DEFAULT_PEAK_AMPLITUDE = 3.0f;      // Phasic burst: 3x baseline
    static constexpr float DEFAULT_NEGATIVE_DIP = 0.3f;        // Negative RPE: 30% of baseline
    static constexpr float DEFAULT_DECAY_TIME_CONSTANT = 3.0f;  // seconds
    static constexpr float DEFAULT_RECOVERY_TIME_CONSTANT = 2.0f; // seconds
    static constexpr float DEFAULT_ADAPTATION_TIME_CONSTANT = 7.0f; // seconds
    static constexpr float DEFAULT_STDP_MODULATION = 1.2f;      // Dopamine scaling for STDP
    static constexpr float DEFAULT_NOISE_LEVEL = 0.02f;         // Biological noise
    static constexpr float DEFAULT_REWARD_THRESHOLD = 0.5f;     // Minimum reward for response
    static constexpr float DEFAULT_MIN_DOSE = 0.01f;            // Minimum effective concentration
    
    // State variables
    float level;                 // Current dopamine level (normalized 0-1)
    float baseline;              // Baseline dopamine level
    float targetBaseline;        // Target baseline (can adapt)
    float tonic;                 // Tonic (baseline) component
    float phasic;                // Phasic (burst/dip) component
    float expectedValue;         // Expected value for RPE calculation
    float learningRate;          // Current learning rate
    float adaptation;            // Adaptation state for homeostasis
    float burstAmplitude;        // Current burst amplitude (variable)
    float lastRewardTime;        // Time of last reward
    float refractoryTime;        // Refractory period after burst
    
    // STDP-related
    float stdpWeight;            // STDP weight modulation factor
    float eligibilityTrace;      // STDP eligibility trace
    
    // Lateral inhibition
    std::vector<float> inhibitionSignals; // Lateral inhibition from other neurons
    float lateralInhibitionStrength;      // Strength of lateral inhibition
    
    // History for analysis
    std::vector<float> levelHistory;
    std::vector<float> rewardHistory;
    std::vector<float> errorHistory;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Random generator for biological variability
    RandomGenerator rng;
    
    Impl() 
        : level(DEFAULT_BASELINE), baseline(DEFAULT_BASELINE), targetBaseline(DEFAULT_BASELINE)
        , tonic(DEFAULT_BASELINE), phasic(0.0f), expectedValue(0.0f)
        , learningRate(0.5f), adaptation(0.0f), burstAmplitude(1.0f)
        , lastRewardTime(-1000.0f), refractoryTime(0.0f)
        , stdpWeight(1.0f), eligibilityTrace(0.0f)
        , lateralInhibitionStrength(0.8f)
        , levelHistory()
        , rewardHistory()
        , errorHistory()
        , lastUpdateTime(std::chrono::steady_clock::now())
        , rng() {
        
        // Initialize lateral inhibition signals
        inhibitionSignals.resize(20, 0.0f); // Simulate ~20 dopamine neurons
        
        // Seed with deterministic but variable seed for biological realism
        uint64_t seed = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
        rng.seed(seed ^ 0x9e3779b97f4a7c15); // Combine with constant for uniqueness
        
        NLM_LOG_INFO("Dopamine system initialized with baseline = " + std::to_string(baseline) +
                    ", peak amplitude = " + std::to_string(DEFAULT_PEAK_AMPLITUDE));
    }
    
    ~Impl() {
        NLM_LOG_INFO("Dopamine system destroyed");
    }
    
    // Update dopamine dynamics with neuroscience-based equations
    void update(float dt, float externalInput = 0.0f) {
        // Record current time
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<float>(now - lastUpdateTime).count();
        lastUpdateTime = now;
        
        // Apply lateral inhibition from other neurons
        float lateralInhibition = 0.0f;
        for (float signal : inhibitionSignals) {
            lateralInhibition += signal;
        }
        lateralInhibition *= lateralInhibitionStrength / static_cast<float>(inhibitionSignals.size());
        
        // Add biological noise
        float noise = rng.normal(0.0, DEFAULT_NOISE_LEVEL);
        
        // Calculate decay towards baseline (exponential decay)
        float decayFactor = std::exp(-dt / DEFAULT_DECAY_TIME_CONSTANT);
        tonic = tonic * decayFactor + baseline * (1.0f - decayFactor);
        
        // Calculate phasic component recovery
        float recoveryFactor = std::exp(-dt / DEFAULT_RECOVERY_TIME_CONSTANT);
        phasic = phasic * recoveryFactor; // Phasic component decays over time
        
        // Apply adaptation (homeostatic mechanism)
        float adaptationFactor = std::exp(-dt / DEFAULT_ADAPTATION_TIME_CONSTANT);
        adaptation = adaptation * adaptationFactor;
        targetBaseline = std::clamp(targetBaseline, 0.1f, 0.5f); // Keep in reasonable bounds
        baseline = baseline * adaptationFactor + targetBaseline * (1.0f - adaptationFactor);
        
        // Calculate total level with lateral inhibition and noise
        float newLevel = tonic + phasic - lateralInhibition + externalInput + noise;
        
        // Ensure numerical stability and bounds
        newLevel = std::max(0.0f, std::min(1.0f, newLevel));
        
        // Update state
        level = newLevel;
        learningRate = calculatePlasticityFactor();
        
        // Update history
        levelHistory.push_back(level);
        if (levelHistory.size() > 1000) { // Keep only recent history
            levelHistory.erase(levelHistory.begin());
        }
        
        // Log significant changes
        if (std::abs(level - tonic) > 0.1f || std::abs(phasic) > 0.1f) {
            NLM_LOG_INFO("Dopamine level updated: " + std::to_string(level) +
                        ", tonic: " + std::to_string(tonic) +
                        ", phasic: " + std::to_string(phasic) +
                        ", baseline: " + std::to_string(baseline));
        }
    }
    
    // Signal a reward event (produces phasic burst)
    void signalReward(float reward) {
        if (reward < DEFAULT_MIN_DOSE) {
            NLM_LOG_DEBUG("Reward too small (" + std::to_string(reward) + ") to trigger dopamine response");
            return;
        }
        
        // Check refractory period
        float currentTime = static_cast<float>(std::chrono::duration<float>(std::chrono::steady_clock::now() - lastUpdateTime).count());
        if (currentTime < refractoryTime) {
            NLM_LOG_DEBUG("Dopamine neuron in refractory period, suppressing response");
            return;
        }
        
        // Calculate burst amplitude based on reward magnitude
        float normalizedReward = std::min(1.0f, reward / DEFAULT_REWARD_THRESHOLD);
        float rewardResponse = DEFAULT_PEAK_AMPLITUDE * normalizedReward;
        
        // Add adaptation to previous responses
        rewardResponse *= (1.0f + adaptation * 0.3f);
        
        // Apply lateral inhibition (other neurons suppress this response)
        float totalInhibition = 0.0f;
        for (float signal : inhibitionSignals) {
            totalInhibition += signal;
        }
        totalInhibition *= lateralInhibitionStrength;
        
        // Generate burst
        phasic += (rewardResponse - phasic) * 0.1f; // Smooth transition to new phasic level
        phasic = std::min(phasic, 1.0f); // Cap at peak
        
        // Record reward
        rewardHistory.push_back(reward);
        if (rewardHistory.size() > 100) {
            rewardHistory.erase(rewardHistory.begin());
        }
        
        // Update refractory period
        lastRewardTime = static_cast<float>(std::chrono::duration<float>(std::chrono::steady_clock::now() - lastUpdateTime).count());
        refractoryTime = std::min(1.0f, 0.5f * std::log(1.0f + reward));
        
        // Stimulate lateral inhibition in other neurons
        for (float& signal : inhibitionSignals) {
            signal += rewardResponse * 0.1f;
            signal = std::min(signal, 1.0f);
        }
        
        NLM_LOG_INFO("Dopamine burst triggered by reward: " + std::to_string(reward) +
                    ", burst amplitude: " + std::to_string(phasic) +
                    ", inhibition: " + std::to_string(totalInhibition));
    }
    
    // Signal reward prediction error (RPE)
    void signalRewardPredictionError(float error) {
        // Positive RPE: burst; Negative RPE: dip
        if (error > 0.0f) {
            // Positive RPE produces burst
            float burstAmplitude = DEFAULT_PEAK_AMPLITUDE * error;
            burstAmplitude = std::min(burstAmplitude, 1.0f); // Cap
            
            // Add burst with adaptation
            phasic += burstAmplitude * (1.0f + adaptation * 0.5f);
            phasic = std::min(phasic, 1.0f);
            
            NLM_LOG_INFO("Positive RPE: " + std::to_string(error) + 
                        ", burst amplitude: " + std::to_string(phasic));
        } else if (error < 0.0f) {
            // Negative RPE produces dip
            float dipAmplitude = DEFAULT_NEGATIVE_DIP * std::abs(error);
            dipAmplitude = std::min(dipAmplitude, baseline); // Can't dip below baseline
            
            phasic -= dipAmplitude;
            phasic = std::max(phasic, 0.0f);
            
            NLM_LOG_INFO("Negative RPE: " + std::to_string(error) + 
                        ", dip amplitude: " + std::to_string(-phasic));
        }
        
        // Update expected value for future RPE calculation
        expectedValue += error * 0.1f; // Slow adaptation of expected value
        
        // Record error
        errorHistory.push_back(error);
        if (errorHistory.size() > 100) {
            errorHistory.erase(errorHistory.begin());
        }
    }
    
    // Calculate plasticity factor using phasic-to-tonic conversion
    float calculatePlasticityFactor() const {
        // Neuroscience-based plasticity modulation
        // Phasic component enhances plasticity, tonic provides baseline
        float plasticity = 0.5f; // Baseline plasticity
        
        // Phasic-to-tonic conversion: phasic bursts enhance learning
        float phasicEnhancement = std::pow(phasic, 1.5f) * 0.8f;
        
        // Dopamine level scales plasticity in a non-linear way
        float dopamineScaling = 1.0f + 0.5f * tonic;
        
        // Adaptation reduces plasticity to maintain stability
        float adaptationSuppression = std::max(0.0f, 1.0f - adaptation);
        
        // Combine factors
        plasticity += phasicEnhancement * dopamineScaling * adaptationSuppression;
        
        // Ensure reasonable bounds
        return std::min(2.0f, std::max(0.1f, plasticity));
    }
    
    // Update STDP eligibility trace based on dopamine level
    void updateSTDP(float deltaWeight, float spikeTimeDifference) {
        // Dopamine modulates STDP: positive dopamine strengthens LTP, negative suppresses it
        float dopamineModulation = 1.0f + (level - baseline) * DEFAULT_STDP_MODULATION;
        
        // Apply modulation to eligibility trace
        eligibilityTrace += deltaWeight * dopamineModulation;
        
        // Constrain eligibility trace
        eligibilityTrace = std::min(eligibilityTrace, 1.0f);
        
        // Log significant STDP events
        if (std::abs(deltaWeight) > 0.01f) {
            NLM_LOG_DEBUG("STDP modulation: deltaWeight = " + std::to_string(deltaWeight) +
                         ", dopamine modulation = " + std::to_string(dopamineModulation) +
                         ", eligibility trace = " + std::to_string(eligibilityTrace));
        }
    }
    
    // Get current reward prediction error based on expected vs actual reward
    float getRewardPredictionError(float actualReward) const {
        float rpe = actualReward - expectedValue;
        
        // Add biological variability
        float noise = rng.normal(0.0, 0.05f);
        rpe += noise;
        
        return rpe;
    }
    
    // Adapt baseline based on recent reward history
    void adaptBaseline(float targetLearningRate, float feedbackStrength = 0.1f) {
        // Homeostatic adaptation of baseline level
        float currentPlasticity = calculatePlasticityFactor();
        float plasticityError = targetLearningRate - currentPlasticity;
        
        // Update target baseline based on error
        targetBaseline += plasticityError * feedbackStrength;
        targetBaseline = std::clamp(targetBaseline, 0.1f, 0.5f); // Keep within reasonable bounds
        
        // Adaptation accumulates
        adaptation += std::abs(plasticityError) * feedbackStrength * 0.5f;
        adaptation = std::min(adaptation, 1.0f); // Cap at maximum adaptation
        
        NLM_LOG_DEBUG("Dopamine baseline adaptation: target = " + std::to_string(targetBaseline) +
                     ", adaptation = " + std::to_string(adaptation));
    }
    
    // Get statistics for monitoring
    struct Statistics {
        float averageLevel;
        float peakLevel;
        float minLevel;
        float maxLevel;
        float averagePlasticity;
        float rewardCount;
        float errorCount;
    };
    
    Statistics getStatistics() const {
        Statistics stats;
        stats.averageLevel = 0.0f;
        stats.peakLevel = 0.0f;
        stats.minLevel = 1.0f;
        stats.maxLevel = 0.0f;
        stats.averagePlasticity = 0.0f;
        stats.rewardCount = 0.0f;
        stats.errorCount = 0.0f;
        
        if (!levelHistory.empty()) {
            float sumLevel = 0.0f;
            float sumPlasticity = 0.0f;
            for (float level : levelHistory) {
                sumLevel += level;
                if (level > stats.peakLevel) stats.peakLevel = level;
                if (level < stats.minLevel) stats.minLevel = level;
            }
            stats.averageLevel = sumLevel / static_cast<float>(levelHistory.size());
            
            // Estimate average plasticity from average level
            stats.averagePlasticity = calculatePlasticityFactor();
        }
        
        stats.rewardCount = static_cast<float>(rewardHistory.size());
        stats.errorCount = static_cast<float>(errorHistory.size());
        
        return stats;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    // Validate input
    if (std::isnan(level) || std::isinf(level)) {
        NLM_LOG_ERROR("Invalid dopamine level: " + std::to_string(level));
        return;
    }
    
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->tonic = pImpl->level; // Set tonic component to new level
    NLM_LOG_DEBUG("Dopamine level manually set to " + std::to_string(pImpl->level));
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->calculatePlasticityFactor();
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->update(static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    pImpl->signalReward(reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->signalRewardPredictionError(error);
}

void Dopamine::setExpectedValue(float expected) {
    if (std::isnan(expected) || std::isinf(expected)) {
        NLM_LOG_ERROR("Invalid expected value: " + std::to_string(expected));
        return;
    }
    pImpl->expectedValue = expected;
}

void Dopamine::updateSTDP(float deltaWeight, float spikeTimeDifference) {
    pImpl->updateSTDP(deltaWeight, spikeTimeDifference);
}

void Dopamine::adaptBaseline(float targetLearningRate, float feedbackStrength) {
    pImpl->adaptBaseline(targetLearningRate, feedbackStrength);
}

Dopamine::Statistics Dopamine::getStatistics() const {
    return pImpl->getStatistics();
}

} // namespace nlm
