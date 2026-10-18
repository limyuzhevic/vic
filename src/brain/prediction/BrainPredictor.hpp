#pragma once

// brain/prediction/BrainPredictor.hpp
// Prediction system management for the brain
// Extracts prediction system from monolithic Brain.cpp

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../environment/Action.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class SensoryInput;

// Brain prediction system manager
class BrainPredictor {
public:
    // Constructor
    explicit BrainPredictor(std::shared_ptr<Config> config);
    
    // Destructor
    ~BrainPredictor();
    
    // Disable copying
    BrainPredictor(const BrainPredictor&) = delete;
    BrainPredictor& operator=(const BrainPredictor&) = delete;
    
    // Configuration and initialization
    bool initialize();
    void reset();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Prediction operations
    void updatePrediction(const SensoryInput& sensoryInput);
    float getPredictionError() const;
    void setTargetAction(const class Action& action);
    
    // Learning operations
    void learnFromOutcome(float outcome);
    void updatePredictionWeights(float learningRate);
    
    // Statistics
    size_t getPredictionCount() const { return predictionCount_; }
    float getAveragePredictionAccuracy() const { return averagePredictionAccuracy_; }
    
    // Configuration utilities
    static bool validatePredictionConfig(const std::shared_ptr<const Config>& config);
    static std::string getDefaultPredictionAlgorithm();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core components
    std::shared_ptr<Config> config_;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem_;
    
    // Prediction state
    std::vector<float> predictionHistory_;
    std::vector<float> outcomeHistory_;
    size_t predictionCount_;
    float averagePredictionAccuracy_;
    
    // Target and learning
    std::unique_ptr<class Action> targetAction_;
    float predictionError_;
};

} // namespace nlm
