#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

class Brain;

// Real synaptogenesis system implementing biologically plausible synaptic formation
// Based on real neural development principles: activity-dependent formation, Hebbian competition,
// target density maintenance with homeostasis, competitive formation, and age-dependent rates
class Synaptogenesis {
public:
    Synaptogenesis(std::shared_ptr<Config> config);
    ~Synaptogenesis();
    
    // Update synaptic formation based on activity and developmental stage
    void update(Brain* brain, RandomGenerator& rng, const DevelopmentSystem& devSys);
    
    // Form new synapse between two neurons if conditions are met
    bool formSynapse(Brain* brain, NeuronId source, NeuronId destination,
                     float baseWeight, const DevelopmentSystem& devSys);
    
    // Get current formation statistics
    size_t getSynapsesFormed() const { return synapsesFormed_; }
    float getCurrentDensity() const { return currentDensity_; }
    
    // Configuration
    void setFormationRate(float rate) { formationRate_ = rate; }
    void setTargetDensity(float density) { targetDensity_ = density; }
    void setMaxSynapsesPerNeuron(size_t max) { maxSynapsesPerNeuron_ = max; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float formationRate_;
    float targetDensity_;
    size_t maxSynapsesPerNeuron_;
    size_t synapsesFormed_;
    float currentDensity_;
};

} // namespace nlm
