#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "StructuralPlasticity.hpp"
#include <cmath>

namespace nlm {

class Synapse;
class Neuron;
class SpikeSystem;
class Config;
class RandomGenerator;

class BrainPlasticity {
public:
    // Forward declaration for friendship
    friend class Brain;
    
    BrainPlasticity() = default;
    ~BrainPlasticity() = default;
    
    // STDP (Spike-Timing Dependent Plasticity) operations
    void applySTDP(Synapse* syn, const std::vector<float>& preSpikes, 
                   const std::vector<float>& postSpikes, TimestepDuration timestep, float neuromodulationFactor = 1.0f);
    void configureSTDP(float ltpWeight, float ltdWeight, float tau);
    
    // Hebbian learning operations
    void applyHebbian(Synapse* syn, const std::vector<float>& preSpikes,
                     const std::vector<float>& postSpikes, TimestepDuration timestep);
    
    // Structural plasticity operations
    void updateStructuralPlasticity(const class Brain* brain, RandomGenerator& rng);
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Plasticity statistics
    float getExcitationInhibitionRatio(const class Brain* brain) const;
    size_t getTotalSynapseCount(const class Brain* brain) const;
    
private:
    // Helper functions for plasticity calculations
    void calculateSTDPWeightChange(float& weightChange, float preTime, float postTime, 
                                  float ltpWeight, float ltdWeight, float tau, bool isPreFirst);
    void updateEligibilityTraces(Synapse* syn, float preTime, float postTime, TimestepDuration timestep);
    void performSynaptogenesis(class Brain* brain, RandomGenerator& rng);
    void performPruning(class Brain* brain, RandomGenerator& rng);
    
    // Internal state
    float currentLTPWeight = 0.01f;
    float currentLTDWeight = 0.012f;
    float currentTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
};

} // namespace nlm