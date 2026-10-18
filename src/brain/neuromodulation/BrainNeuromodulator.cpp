#include "brain/neuromodulation/BrainNeuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <ctime>

namespace nlm {

BrainNeuromodulator::BrainNeuromodulator(std::shared_ptr<Config> config)
    : config_(config)
    , globalModulationSignal_(0.0f)
    , noveltyEvents_(0)
    , predictionErrorCount_(0)
    , averagePredictionAccuracy_(0.0f)
    , plasticityModulationLevel_(1.0f)
{
    if (!config_) {
        NLM_LOG_ERROR("Configuration is null in BrainNeuromodulator constructor");
    }
    
    NLM_LOG_INFO("BrainNeuromodulator initialized");
}

BrainNeuromodulator::~BrainNeuromodulator() = default;

bool BrainNeuromodulator::initialize() {
    NLM_LOG_INFO("Initializing brain neuromodulation systems...");
    
    // Initialize neuromodulation systems
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    reward_ = std::make_unique<Reward>();
    
    // Initialize neuromodulation systems with references if available
    if (dopamine_) {
        dopamine_->initialize(nullptr);  // Would pass brain reference
    }
    
    if (curiosity_) {
        curiosity_->initialize(nullptr);  // Would pass brain reference
    }
    
    if (novelty_) {
        novelty_->initialize(nullptr);  // Would pass brain reference
    }
    
    if (predictionError_) {
        predictionError_->initialize(nullptr);  // Would pass brain reference
    }
    
    if (reward_) {
        reward_->initialize(nullptr);  // Would pass brain reference
    }
    
    // Validate configuration
    if (!validateNeuromodulationConfig(config_)) {
        NLM_LOG_ERROR("Neuromodulation system configuration validation failed");
        return false;
    }
    
    NLM_LOG_INFO("Brain neuromodulation systems initialized");
    return true;
}

void BrainNeuromodulator::reset() {
    NLM_LOG_INFO("Resetting brain neuromodulation systems...");
    
    globalModulationSignal_ = 0.0f;
    noveltyEvents_ = 0;
    predictionErrorCount_ = 0;
    averagePredictionAccuracy_ = 0.0f;
    plasticityModulationLevel_ = 1.0f;
    
    rewardPredictions_.clear();
    recentNoveltyEvents_.clear();
    synapseModulationFactors_.clear();
    
    if (dopamine_) {
        // Reset dopamine system
        // Dopamine would have reset method if available
    }
    
    if (predictionError_) {
        // Reset prediction error system
        // PredictionError would have reset method if available
    }
    
    NLM_LOG_INFO("Brain neuromodulation systems reset");
}

void BrainNeuromodulator::updateNeuromodulation(double timestep) {
    // Update all neuromodulation systems
    if (dopamine_) {
        dopamine_->update(timestep);
    }
    
    if (curiosity_) {
        curiosity_->update(timestep);
    }
    
    if (novelty_) {
        novelty_->update(timestep);
    }
    
    if (predictionError_) {
        // Prediction error would be updated by the brain
        // based on prediction vs outcome
    }
    
    if (reward_) {
        reward_->update(timestep);
    }
    
    // Calculate global modulation signal
    float totalModulation = 0.0f;
    int systemCount = 0;
    
    if (dopamine_) {
        totalModulation += std::abs(dopamine_->getLevel());
        systemCount++;
    }
    
    if (curiosity_) {
        totalModulation += curiosity_->getLevel();
        systemCount++;
    }
    
    if (novelty_) {
        totalModulation += novelty_->getLevel();
        systemCount++;
    }
    
    globalModulationSignal_ = systemCount > 0 ? totalModulation / systemCount : 0.0f;
}

void BrainNeuromodulator::applyRewards(float reward, float predictionError) {
    if (!reward_) {
        return;
    }
    
    // Apply reward and prediction error
    reward_->applyReward(reward);
    
    if (predictionError_) {
        predictionError_->update(predictionError);
    }
    
    // Update prediction error count
    predictionErrorCount_++;
    
    // Update dopamine based on reward prediction error
    if (dopamine_) {
        float dopamineLevel = predictionError;  // Simplified: prediction error drives dopamine
        dopamine_->setLevel(dopamineLevel);
    }
    
    // Update reward prediction
    predictRewards({reward});
    updateRewards({reward});
}

void BrainNeuromodulator::updatePredictionError(const std::vector<float>& prediction,
                                                const std::vector<float>& outcome) {
    if (!predictionError_ || prediction.empty() || outcome.empty()) {
        return;
    }
    
    // Calculate prediction error
    float totalError = 0.0f;
    for (size_t i = 0; i < std::min(prediction.size(), outcome.size()); ++i) {
        totalError += std::abs(prediction[i] - outcome[i]);
    }
    
    float averageError = totalError / std::min(prediction.size(), outcome.size());
    
    // Update prediction error system
    predictionError_->update(averageError);
    
    // Update dopamine based on prediction error
    if (dopamine_) {
        float dopamineLevel = -averageError;  // Negative error increases dopamine
        dopamine_->setLevel(dopamineLevel);
    }
    
    // Update prediction accuracy
    float accuracy = 1.0f - averageError;
    averagePredictionAccuracy_ = (averagePredictionAccuracy_ + accuracy) * 0.5f;  // Moving average
}

void BrainNeuromodulator::detectNovelty(const std::vector<float>& input) {
    if (!novelty_ || input.empty()) {
        return;
    }
    
    NoveltyEvent event;
    event.timestamp = std::time(nullptr);
    event.inputChange = input;
    event.source = "sensory_input";
    
    // Calculate novelty score (change from previous input)
    if (!recentNoveltyEvents_.empty()) {
        const NoveltyEvent& lastEvent = recentNoveltyEvents_.back();
        float totalChange = 0.0f;
        for (size_t i = 0; i < std::min(input.size(), lastEvent.inputChange.size()); ++i) {
            totalChange += std::abs(input[i] - lastEvent.inputChange[i]);
        }
        event.noveltyScore = totalChange / std::min(input.size(), lastEvent.inputChange.size());
    } else {
        event.noveltyScore = 1.0f;  // Maximum novelty for first input
    }
    
    // Update novelty detection system
    novelty_->update(event.noveltyScore);
    
    // Store event
    recentNoveltyEvents_.push_back(event);
    noveltyEvents_++;
    
    // Limit stored events
    if (recentNoveltyEvents_.size() > 1000) {
        recentNoveltyEvents_.erase(recentNoveltyEvents_.begin());
    }
}

void BrainNeuromodulator::updateCuriosity(const std::vector<float>& informationGain) {
    if (!curiosity_ || informationGain.empty()) {
        return;
    }
    
    // Calculate information gain (entropy or surprise)
    float totalGain = 0.0f;
    for (float gain : informationGain) {
        totalGain += gain;
    }
    
    float averageGain = totalGain / informationGain.size();
    
    // Update curiosity system
    curiosity_->update(averageGain);
}

void BrainNeuromodulator::modulateLearning(float predictionError, float reward) {
    // Update plasticity modulation level based on neuromodulation
    float neuromodulationLevel = globalModulationSignal_;
    
    // Simple modulation formula: positive reward and low error = high plasticity
    float modulationFactor = 1.0f;
    
    if (reward > 0.0f) {
        modulationFactor *= 1.0f + reward * 0.5f;
    }
    
    if (predictionError < 0.1f) {
        modulationFactor *= 1.2f;  // Low prediction error = high learning
    }
    
    plasticityModulationLevel_ = std::clamp(modulationFactor, 0.1f, 2.0f);
    
    // Apply to dopamine system
    if (dopamine_) {
        dopamine_->setPlasticityFactor(plasticityModulationLevel_);
    }
}

void BrainNeuromodulator::updatePlasticityModulation(float neuromodulationLevel) {
    plasticityModulationLevel_ = neuromodulationLevel;
    
    // Apply to all plasticity systems
    // This would be integrated with the brain's plasticity update
}

void BrainNeuromodulator::predictRewards(const std::vector<float>& expectedOutcomes) {
    RewardPrediction prediction;
    prediction.predictedRewards = expectedOutcomes;
    prediction.actualRewards.clear();
    prediction.predictionCount = 1;
    prediction.predictionError = 0.0f;
    prediction.learningRate = 0.01f;  // Default learning rate
    
    rewardPredictions_.push_back(prediction);
    
    // Limit stored predictions
    if (rewardPredictions_.size() > 100) {
        rewardPredictions_.erase(rewardPredictions_.begin());
    }
}

void BrainNeuromodulator::updateRewards(const std::vector<float>& actualOutcomes) {
    if (rewardPredictions_.empty() || actualOutcomes.empty()) {
        return;
    }
    
    // Match predictions with actual outcomes
    RewardPrediction& prediction = rewardPredictions_.back();
    
    if (!actualOutcomes.empty()) {
        prediction.actualRewards = actualOutcomes;
        
        // Calculate prediction error
        float totalError = 0.0f;
        for (size_t i = 0; i < std::min(prediction.predictedRewards.size(), 
                                      prediction.actualRewards.size()); ++i) {
            totalError += std::abs(prediction.predictedRewards[i] - prediction.actualRewards[i]);
        }
        
        prediction.predictionError = totalError / std::min(prediction.predictedRewards.size(), 
                                                         prediction.actualRewards.size());
        
        // Update learning rate based on prediction error
        prediction.learningRate = std::max(0.001f, 0.1f - prediction.predictionError);
    }
}

bool BrainNeuromodulator::validateNeuromodulationConfig(const std::shared_ptr<const Config>& config) {
    if (!config) {
        return false;
    }
    
    // Validate neuromodulation-related configuration
    auto rewardLearningRate = config->getOr<float>("reward_learning_rate", 0.01f);
    if (rewardLearningRate < 0.0f || rewardLearningRate > 1.0f) {
        NLM_LOG_ERROR("Invalid reward_learning_rate: " + std::to_string(rewardLearningRate));
        return false;
    }
    
    auto curiosityThreshold = config->getOr<float>("curiosity_threshold", 0.5f);
    if (curiosityThreshold < 0.0f || curiosityThreshold > 1.0f) {
        NLM_LOG_ERROR("Invalid curiosity_threshold: " + std::to_string(curiosityThreshold));
        return false;
    }
    
    return true;
}

void BrainNeuromodulator::applyDefaultNeuromodulationSettings(std::shared_ptr<Config> config) {
    if (!config) {
        return;
    }
    
    // Apply default neuromodulation settings
    config->set("reward_learning_rate", 0.01f);
    config->set("curiosity_threshold", 0.5f);
    config->set("novelty_detection_sensitivity", 0.7f);
    config->set("prediction_error_decay", 0.95f);
}

} // namespace nlm
