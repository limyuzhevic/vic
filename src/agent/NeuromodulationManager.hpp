#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Neuromodulation configuration parameters
struct NeuromodulationConfig {
    // Reward modulation
    float rewardLearningRate_ = 0.05f;        // Learning rate for reward prediction
    float dopamineGain_ = 1.0f;              // Dopamine signal scaling
    float rewardPredictionDecay_ = 0.95f;     // Decay of expected reward
    
    // Novelty detection
    float noveltyThreshold_ = 0.1f;          // Novelty detection threshold
    float noveltyDecayRate_ = 0.99f;         // Decay rate of novelty signal
    float noveltyHistorySize_ = 100;         // Number of past observations to store
    
    // Curiosity
    float curiosityGain_ = 2.0f;            // Curiosity response to novelty
    float predictionErrorWeight_ = 0.5f;     // Weight of prediction error in curiosity
    float explorationThreshold_ = 0.3f;      // Curiosity threshold for exploration
    
    // Development interaction
    float plasticityGain_ = 1.0f;            // How much neuromodulators affect plasticity
    bool enableDevelopmentalNeuromodulation_ = true; // Neuromodulation during development
    
    // Configuration flags
    bool enableRewardModulation_ = true;     // Enable reward-based modulation
    bool enableNoveltyDetection_ = true;      // Enable novelty detection
    bool enableCuriosityDriven_ = true;       // Enable curiosity-driven exploration
};

// Neuromodulation signal types
enum class NeuromodulatorType {
    Dopamine,      // Reward prediction error
    Serotonin,     // Aversive prediction error
    Acetylcholine, // Attention/arousal
    Norepinephrine, // Alertness/arousal
    Orexin        // Wake/alertness
};

// Neuromodulator signal structure
struct NeuromodulatorSignal {
    NeuromodulatorType type;
    float strength;        // Signal strength (-1.0 to 1.0)
    float duration;        // Signal duration in simulation steps
    float timestamp;       // When signal was generated
    
    NeuromodulatorSignal() : type(NeuromodulatorType::Dopamine), 
                              strength(0.0f), duration(0.0f), timestamp(0.0f) {}
    
    NeuromodulatorSignal(NeuromodulatorType t, float s, float d, float ts)
        : type(t), strength(s), duration(d), timestamp(ts) {}
};

// NeuromodulationManager: Handles all neuromodulatory signals and their effects
// Separates neuromodulation logic from AgentBrain for better modularity
class NeuromodulationManager {
public:
    NeuromodulationManager(const NeuromodulationConfig& config = NeuromodulationConfig());
    ~NeuromodulationManager();
    
    // Configure neuromodulation
    void configure(const NeuromodulationConfig& config) { config_ = config; }
    
    // Update neuromodulation state
    void update(double timestep);
    
    // Process reward signal
    void processReward(float reward, float predictedReward);
    
    // Process novelty signal from sensory input
    void processNovelty(const std::vector<float>& currentInput, 
                        const std::vector<std::vector<float>>& history);
    
    // Get current neuromodulation levels
    float getDopamineLevel() const { return dopamineLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getPredictionError() const { return predictionError_; }
    
    // Get neuromodulator signals
    const std::vector<NeuromodulatorSignal>& getActiveSignals() const { return activeSignals_; }
    
    // Calculate plasticity modifier based on neuromodulators
    float calculatePlasticityModifier() const;
    
    // Get developmental stage modifier
    float getDevelopmentalStageModifier(DevelopmentalStage stage) const;
    
    // Reset neuromodulation state
    void reset();
    
    // Validate state
    bool isValid() const;
    
    // Get configuration
    const NeuromodulationConfig& getConfig() const { return config_; }
    
    // Enable/disable specific neuromodulation types
    void enableRewardModulation(bool enable) { config_.enableRewardModulation_ = enable; }
    void enableNoveltyDetection(bool enable) { config_.enableNoveltyDetection_ = enable; }
    void enableCuriosityDriven(bool enable) { config_.enableCuriosityDriven_ = enable; }
    
    bool isRewardModulationEnabled() const { return config_.enableRewardModulation_; }
    bool isNoveltyDetectionEnabled() const { return config_.enableNoveltyDetection_; }
    bool isCuriosityDrivenEnabled() const { return config_.enableCuriosityDriven_; }
    
private:
    // Reward-related signals
    void computeRewardPredictionError(float reward, float predictedReward);
    void updateExpectedReward(float reward);
    void generateDopamineSignal();
    
    // Novelty-related signals
    void computeNoveltyLevel(const std::vector<float>& currentInput);
    void updateNoveltyHistory(const std::vector<float>& input);
    void generateCuriositySignal();
    
    // Signal management
    void addSignal(NeuromodulatorType type, float strength, float duration);
    void decayActiveSignals(double timestep);
    void applySignalEffects();
    
    // Effect calculation
    float calculateDopamineEffect() const;
    float calculateNoveltyEffect() const;
    
    // State variables
    float dopamineLevel_;        // Current dopamine level (-1.0 to 1.0)
    float noveltyLevel_;         // Current novelty level (0.0 to 1.0)
    float curiosityLevel_;       // Current curiosity level (0.0 to 1.0)
    float predictionError_;      // Reward prediction error
    float expectedReward_;       // Expected future reward (moving average)
    
    // History for novelty detection
    std::vector<std::vector<float>> noveltyHistory_;
    
    // Active neuromodulator signals
    std::vector<NeuromodulatorSignal> activeSignals_;
    
    // Configuration
    NeuromodulationConfig config_;
    
    // Timestep tracking
    double currentTime_;
};

} // namespace nlm