// Neuromodulator Factory for Phase 6
// Centralizes neuromodulator creation and configuration

#pragma once

#include "Neuromodulator.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

class NeuromodulatorFactory {
public:
    NeuromodulatorFactory();
    ~ NeuromodulatorFactory();
    
    // Create all standard neuromodulators
    void initializeAll(Brain* brain);
    
    // Get neuromodulator by type
    Dopamine* getDopamine() { return dopamine_.get(); }
    Acetylcholine* getAcetylcholine() { return acetylcholine_.get(); }
    Norepinephrine* getNorepinephrine() { return norepinephrine_.get(); }
    Serotonin* getSerotonin() { return serotonin_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    PredictionError* getPredictionError() { return predictionError_.get(); }
    Reward* getReward() { return reward_.get(); }
    
    // Get all neuromodulators for iteration
    std::vector<Neuromodulator*> getAllNeuromodulators() {
        std::vector<Neuromodulator*> result;
        if (dopamine_) result.push_back(dopamine_.get());
        if (acetylcholine_) result.push_back(acetylcholine_.get());
        if (norepinephrine_) result.push_back(norepinephrine_.get());
        if (serotonin_) result.push_back(serotonin_.get());
        if (curiosity_) result.push_back(curiosity_.get());
        if (novelty_) result.push_back(novelty_.get());
        if (predictionError_) result.push_back(predictionError_.get());
        if (reward_) result.push_back(reward_.get());
        return result;
    }
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt) {
        for (auto* nm : neuromodulators_) {
            if (nm) nm->update(dt);
        }
    }
    
    // Configure neuromodulators from config
    void configureFromConfig(std::shared_ptr<Config> config) {
        // Configure each neuromodulator from config values
        // This allows users to customize neuromodulation parameters
    }
    
    // Reset all neuromodulators
    void resetAll() {
        for (auto* nm : neuromodulators_) {
            if (nm) {
                // Call reset if available
                if (auto* resettable = dynamic_cast<Resettable*>(nm)) {
                    resettable->reset();
                }
            }
        }
    }
    
private:
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Acetylcholine> acetylcholine_;
    std::unique_ptr<Norepinephrine> norepinephrine_;
    std::unique_ptr<Serotonin> serotonin_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Reward> reward_;
    
    std::vector<Neuromodulator*> neuromodulators_;
};

// Marker interface for neuromodulators that can be reset
class Resettable {
public:
    virtual ~Resettable() = default;
    virtual void reset() = 0;
};

} // namespace nlm
