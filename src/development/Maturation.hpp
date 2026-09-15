#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

class Brain;

// Neural maturation system implementing biologically plausible developmental processes
// Based on real neural development principles: activity-dependent maturation, Hebbian consolidation,
// critical period dynamics, and age-dependent plasticity reduction
class Maturation {
public:
    Maturation(std::shared_ptr<Config> config);
    ~Maturation();
    
    // Update neuronal maturation properties based on activity and developmental stage
    void update(Brain* brain, SimulationStep currentStep, RandomGenerator& rng);
    
    // Gradually adjust neuron properties toward mature values
    void matureNeuron(Neuron* neuron, float progress, const DevelopmentSystem& devSys);
    
    // Get current maturation progress (0-1)
    float getProgress() const { return progress_; }
    
    // Activity-dependent maturation rate calculation
    float calculateMaturationRate(float activity, DevelopmentalStage stage, float age) const;
    
    // Get mature properties
    MembranePotential getMatureThreshold() const { return matureThreshold_; }
    MembranePotential getMatureRestingPotential() const { return matureRestingPotential_; }
    float getMatureTimeConstant() const { return matureTimeConstant_; }
    
    // Configuration
    void setMaturationRate(float rate) { maturationRate_ = rate; }
    void setConsolidationRate(float rate) { consolidationRate_ = rate; }
    void setHebbianLearningRate(float rate) { hebbianLearningRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float progress_;
    float maturationRate_;
    float consolidationRate_;
    float hebbianLearningRate_;
    MembranePotential matureThreshold_;
    MembranePotential matureRestingPotential_;
    float matureTimeConstant_;
};

} // namespace nlm
