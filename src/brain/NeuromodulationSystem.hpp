// NeuromodulationSystem.h - Integrated neuromodulation and motivation system

#pragma once

#include "BrainImpl.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include <memory>
#include <functional>

namespace nlm {

class NeuromodulationSystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem() = default;
    
    // Initialize with brain reference
    void initialize(BrainImpl* brain);
    
    // Core neuromodulation update
    void update(double timestep);
    
    // Signal processing
    void processReward(float reward, float predictedReward);
    void processNovelty(const std::vector<float>& sensoryInput);
    void processCuriosity(double uncertainty, double surprise);
    void processPredictionError(float prediction, float outcome);
    
    // Neuromodulation effects
    void applyDopamineEffects();
    void applyCuriosityEffects();
    void applyNoveltyEffects();
    void applyPredictionErrorEffects();
    
    // Configuration
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setPredictionErrorSensitivity(float sensitivity) { predictionErrorSensitivity_ = sensitivity; }
    void setNoveltyDetectionThreshold(float threshold) { noveltyThreshold_ = threshold; }
    
    // State access
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    
    // Statistics
    float getPlasticityFactor() const { return plasticityFactor_; }
    size_t getActiveMotivationalDrive() const { return motivationalDrives_.size(); }
    
    // Configuration
    void configure(float learningRate, float explorationRate, 
                   float predictionErrorSensitivity, float noveltyThreshold);
    
    // Motivation management
    void addMotivationalDrive(const std::string& drive, float strength);
    void updateMotivationalDrives(double timestep);
    float getMotivationalDrive(const std::string& drive) const;
    
private:
    BrainImpl* brain_ = nullptr;
    
    // Neuromodulation subsystems
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    
    // Integrated signal state
    float dopamineLevel_;
    float curiosityLevel_;
    float noveltyLevel_;
    float predictionError_;
    float plasticityFactor_;
    
    // Motivation and drive management
    std::unordered_map<std::string, float> motivationalDrives_;
    std::vector<std::string> activeDrives_;
    
    // Configuration parameters
    float learningRate_;
    float explorationRate_;
    float predictionErrorSensitivity_;
    float noveltyThreshold_;
    
    // Learning and adaptation
    std::vector<float> predictionHistory_;
    std::vector<float> rewardHistory_;
    double lastUpdateTime_;
    
    // Signal processing
    void computeIntegratedMotivation();
    void applyNeuromodulationToPlasticity();
    void updateLearningRates();
    void adaptToEnvironment(const std::vector<float>& environmentSignals);
    void resetDriveStates();
    
    // Drive management
    void evaluateDriveConflicts();
    void resolveDriveConflicts();
    float computeDrivePriorities() const;
    
    friend class BrainImpl;
};

} // namespace nlm
