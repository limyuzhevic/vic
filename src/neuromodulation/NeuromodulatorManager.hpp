#pragma once

#include "../core/Types/Types.hpp"
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "Reward.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace nlm {

class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    // Initialize all neuromodulators with brain reference
    void initialize(class Brain* brain);
    
    // Update all neuromodulators
    void update(TimestepDuration dt);
    
    // Add a neuromodulator by name
    template<typename T>
    void addNeuromodulator(const std::string& name);
    
    // Get neuromodulator by name
    Neuromodulator* getNeuromodulator(const std::string& name);
    const Neuromodulator* getNeuromodulator(const std::string& name) const;
    
    // Get dopamine neuromodulator (convenience method)
    Dopamine* getDopamine();
    const Dopamine* getDopamine() const;
    
    // Get curiosity neuromodulator (convenience method)
    Curiosity* getCuriosity();
    const Curiosity* getCuriosity() const;
    
    // Get novelty neuromodulator (convenience method)
    Novelty* getNovelty();
    const Novelty* getNovelty() const;
    
    // Get prediction error neuromodulator (convenience method)
    PredictionError* getPredictionError();
    const PredictionError* getPredictionError() const;
    
    // Get reward neuromodulator (convenience method)
    Reward* getReward();
    const Reward* getReward() const;
    
    // Get all neuromodulator names
    std::vector<std::string> getNeuromodulatorNames() const;
    
    // Process events for neuromodulators
    void processRewardEvent(float reward);
    void processPredictionErrorEvent(float error);
    void processSensoryInput(const std::vector<float>& pattern);
    void processObservation(const Observation& observation);
    
    // Reset all neuromodulators
    void reset();
    
    // Configuration
    void setLearningRate(float rate);
    void setExplorationBonus(float bonus);
    void setPredictionErrorDecay(float decay);
    void setNoveltyThreshold(float threshold);
    
    // Statistics and diagnostics
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
