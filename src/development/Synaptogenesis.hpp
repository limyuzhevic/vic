#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Neuron.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <memory>

namespace nlm {

// Real synaptogenesis implementation with Hebbian and STDP rules
class Synaptogenesis {
public:
    Synaptogenesis();
    ~Synaptogenesis();
    
    // Get formation rate
    float getFormationRate() const { return formationRate_; }
    void setFormationRate(float rate) { formationRate_ = std::max(0.0f, std::min(rate, 1.0f)); }
    
    // Get target synapse density
    float getTargetDensity() const { return targetDensity_; }
    void setTargetDensity(float density) { targetDensity_ = std::max(0.0f, density); }
    
    // Get spontaneous formation rate
    float getSpontaneousRate() const { return spontaneousRate_; }
    void setSpontaneousRate(float rate) { spontaneousRate_ = std::max(0.0f, rate); }
    
    // Get Hebbian strength
    float getHebbianStrength() const { return hebbianStrength_; }
    void setHebbianStrength(float strength) { hebbianStrength_ = std::max(0.0f, std::min(strength, 1.0f)); }
    
    // Get STDP learning rate
    float getSTDPLearningRate() const { return stdpLearningRate_; }
    void setSTDPLearningRate(float rate) { stdpLearningRate_ = std::max(0.0f, rate); }
    
    // Update synapse formation with biological realism
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt, 
                float neuromodulationFactor = 1.0f);
    
    // Form synapse between two neurons based on Hebbian principles
    bool formHebbianSynapse(NeuronId source, NeuronId target, 
                           Brain* brain, float similarity, 
                           const RandomGenerator& rng);
    
    // Form synapse based on STDP temporal pairing
    bool formSTDPSynapse(NeuronId source, NeuronId target,
                        Timestamp preSpikeTime, Timestamp postSpikeTime,
                        Brain* brain, const RandomGenerator& rng);
    
    // Update synaptic weight using Hebbian learning
    void updateHebbianWeight(Synapse* synapse, float preActivity, 
                            float postActivity, float learningRate,
                            TimestepDuration dt);
    
    // Update synaptic weight using STDP
    void updateSTDPWeight(Synapse* synapse, Timestamp preSpikeTime,
                         Timestamp postSpikeTime, float learningRate,
                         TimestepDuration dt);
    
    // Get statistics
    size_t getTotalSynapsesFormed() const { return totalSynapsesFormed_; }
    float getSynapseDensity() const { return currentDensity_; }
    
    // Reset developmental counters
    void resetCounters() {
        totalSynapsesFormed_ = 0;
        currentDensity_ = 0.0f;
    }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float formationRate_;
    float targetDensity_;
    float spontaneousRate_;
    float hebbianStrength_;
    float stdpLearningRate_;
    size_t totalSynapsesFormed_;
    float currentDensity_;
};

} // namespace nlm