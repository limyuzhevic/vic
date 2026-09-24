#pragma once

#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "Reward.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Neuromodulation: Integrated system coordinating all neuromodulatory signals
// Implements a biologically plausible neuromodulation network with cross-talk between systems
class Neuromodulation {
public:
    Neuromodulation();
    ~Neuromodulation();
    
    // Initialize with brain reference for integrated operation
    bool initialize(class Brain* brain);
    
    // Update all neuromodulators with current simulation state
    void update(TimestepDuration dt);
    
    // Signal events to neuromodulators
    void signalReward(float rewardValue);
    void signalPredictionError(float error);
    void signalNovelty(float novelty);
    void signalAttention(float attention);
    void signalPunishment(float punishment);
    
    // Get individual neuromodulator references
    const Dopamine& getDopamine() const;
    const Acetylcholine& getAcetylcholine() const;
    const Norepinephrine& getNorepinephrine() const;
    const Serotonin& getSerotonin() const;
    
    // Get auxiliary neuromodulation systems
    const Curiosity& getCuriosity() const;
    const Novelty& getNovelty() const;
    const PredictionError& getPredictionError() const;
    const Reward& getReward() const;
    
    // Calculate overall neuromodulatory effect on neural plasticity
    float getOverallNeuromodulatoryEffect() const;
    
    // Log system status for debugging
    void logStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm