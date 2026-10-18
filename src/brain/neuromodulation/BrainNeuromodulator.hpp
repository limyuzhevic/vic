#pragma once

// brain/neuromodulation/BrainNeuromodulator.hpp
// Neuromodulation system management for the brain
// Extracts neuromodulation systems from monolithic Brain.cpp

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Reward.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace nlm {

// Forward declarations
class Config;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class Reward;

// Brain neuromodulation system manager
class BrainNeuromodulator {
public:
    // Constructor
    explicit BrainNeuromodulator(std::shared_ptr<Config> config);
    
    // Destructor
    ~BrainNeuromodulator();
    
    // Disable copying
    BrainNeuromodulator(const BrainNeuromodulator&) = delete;
    BrainNeuromodulator& operator=(const BrainNeuromodulator&) = delete;
    
    // Configuration and initialization
    bool initialize();
    void reset();
    
    // Neuromodulation system access
    Dopamine* getDopamine() { return dopamine_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    PredictionError* getPredictionError() { return predictionError_.get(); }
    Reward* getReward() { return reward_.get(); }
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Neuromodulation operations
    void updateNeuromodulation(double timestep);
    void applyRewards(float reward, float predictionError);
    void updatePredictionError(const std::vector<float>& prediction,
                              const std::vector<float>& outcome);
    void detectNovelty(const std::vector<float>& input);
    void updateCuriosity(const std::vector<float>& informationGain);
    
    // State management
    float getGlobalModulationSignal() const { return globalModulationSignal_; }
    float getDopamineLevel() const { return dopamine_ ? dopamine_->getLevel() : 0.0f; }
    float getCuriosityLevel() const { return curiosity_ ? curiosity_->getLevel() : 0.0f; }
    float getNoveltyLevel() const { return novelty_ ? novelty_->getLevel() : 0.0f; }
    
    // Learning modulation
    void modulateLearning(float predictionError, float reward);
    void updatePlasticityModulation(float neuromodulationLevel);
    
    // Statistics
    size_t getNoveltyEvents() const { return noveltyEvents_; }
    size_t getPredictionErrorCount() const { return predictionErrorCount_; }
    float getAveragePredictionAccuracy() const { return averagePredictionAccuracy_; }
    
    // Configuration utilities
    static bool validateNeuromodulationConfig(const std::shared_ptr<const Config>& config);
    static void applyDefaultNeuromodulationSettings(std::shared_ptr<Config> config);
    
    // Reward prediction and learning
    struct RewardPrediction {
        std::vector<float> predictedRewards;
        std::vector<float> actualRewards;
        size_t predictionCount;
        float predictionError;
        float learningRate;
    };
    
    // Track reward predictions
    void predictRewards(const std::vector<float>& expectedOutcomes);
    void updateRewards(const std::vector<float>& actualOutcomes);
    
    // Novelty detection and tracking
    struct NoveltyEvent {
        double timestamp;
        std::vector<float> inputChange;
        float noveltyScore;
        std::string source;
    };
    
    // Get recent novelty events
    std::vector<NoveltyEvent> getRecentNoveltyEvents(size_t limit = 100) const { return recentNoveltyEvents_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core components
    std::shared_ptr<Config> config_;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Reward> reward_;
    
    // Neuromodulation state
    float globalModulationSignal_;
    
    // Statistics and tracking
    size_t noveltyEvents_;
    size_t predictionErrorCount_;
    float averagePredictionAccuracy_;
    
    // Reward prediction
    std::vector<RewardPrediction> rewardPredictions_;
    
    // Novelty events
    std::vector<NoveltyEvent> recentNoveltyEvents_;
    
    // Learning modulation
    float plasticityModulationLevel_;
    std::map<size_t, float> synapseModulationFactors_;
};

} // namespace nlm
