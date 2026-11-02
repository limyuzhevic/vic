#pragma once

#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declarations
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class Acetylcholine;
class Norepinephrine;
class Serotonin;

class NeuromodulationManager {
public:
    // Create neuromodulation manager
    NeuromodulationManager();
    ~ NeuromodulationManager();
    
    // Disable copying
    NeuromodulationManager(const NeuromodulationManager&) = delete;
    NeuromodulationManager& operator=(const NeuromodulationManager&) = delete;
    
    // Enable moving
    NeuromodulationManager(NeuromodulationManager&&) noexcept;
    NeuromodulationManager& operator=(NeuromodulationManager&&) noexcept;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update all neuromodulators
    void update(TimestepDuration dt);
    
    // Get neuromodulator levels
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionErrorLevel_; }
    float getAcetylcholineLevel() const { return acetylcholineLevel_; }
    float getNorepinephrineLevel() const { return norepinephrineLevel_; }
    float getSerotoninLevel() const { return serotoninLevel_; }
    
    // Get neuromodulators by type
    Neuromodulator* getNeuromodulator(const std::string& name);
    const Neuromodulator* getNeuromodulator(const std::string& name) const;
    
    // Get all neuromodulators
    const std::vector<std::string>& getNeuromodulatorNames() const { return neuromodulatorNames_; }
    
    // Signal events to neuromodulators
    void signalReward(float reward) { dopamine_->signalReward(reward); }
    void signalRewardPredictionError(float error) { dopamine_->signalRewardPredictionError(error); }
    void detectNovelty(const std::vector<float>& currentState);
    void updateEnvironmentComplexity(float complexity);
    
    // Configure neuromodulation parameters
    void setDopamineLearningRate(float rate) { dopamineLearningRate_ = rate; }
    void setCuriosityScaling(float scaling) { curiosityScaling_ = scaling; }
    void setNoveltyThreshold(float threshold) { noveltyThreshold_ = threshold; }
    void setPredictionErrorDecay(float decay) { predictionErrorDecay_ = decay; }
    
    // Get current configuration
    float getDopamineLearningRate() const { return dopamineLearningRate_; }
    float getCuriosityScaling() const { return curiosityScaling_; }
    float getNoveltyThreshold() const { return noveltyThreshold_; }
    float getPredictionErrorDecay() const { return predictionErrorDecay_; }
    
    // Performance and monitoring
    float getAveragePlasticityFactor() const { return averagePlasticityFactor_; }
    size_t getTotalNeuromodulationEvents() const { return totalNeuromodulationEvents_; }
    void resetMetrics();
    
    // Custom hooks and observers
    void addNeuromodulationObserver(std::function<void(const std::string&, float)> observer) {
        observers_.push_back(std::move(observer));
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Individual neuromodulator instances
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Acetylcholine> acetylcholine_;
    std::unique_ptr<Norepinephrine> norepinephrine_;
    std::unique_ptr<Serotonin> serotonin_;
    
    // Current levels
    float dopamineLevel_;
    float curiosityLevel_;
    float noveltyLevel_;
    float predictionErrorLevel_;
    float acetylcholineLevel_;
    float norepinephrineLevel_;
    float serotoninLevel_;
    
    // Configuration
    float dopamineLearningRate_;
    float curiosityScaling_;
    float noveltyThreshold_;
    float predictionErrorDecay_;
    
    // Performance metrics
    float averagePlasticityFactor_;
    size_t totalNeuromodulationEvents_;
    
    // Observers
    std::vector<std::function<void(const std::string&, float)>> observers_;
    
    // List of available neuromodulator names
    std::vector<std::string> neuromodulatorNames_;
};

} // namespace nlm
