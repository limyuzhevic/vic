#pragma once

#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Neuron.hpp"
#include <memory>

namespace nlm {

class Brain;

class BrainPlasticity {
public:
    BrainPlasticity() = default;
    ~BrainPlasticity() = default;
    
    // Configuration and setup
    void configureFromConfig(const Config* config);
    void setLearningRates(float stdpLtp, float stdpLtd, float hebbian);
    void setStructuralPlasticityRates(float synaptogenesis, float pruning);
    
    // Plasticity updates (called during step)
    void updateAllPlasticity(Brain* brain, TimestepDuration timestep);
    void updateSTDP(Brain* brain, TimestepDuration timestep);
    void updateHebbian(Brain* brain, TimestepDuration timestep);
    void updateStructuralPlasticity(Brain* brain, RandomGenerator& rng);
    
    // Configuration access
    STDP* getSTDP() { return &stdp_; }
    Hebbian* getHebbian() { return &hebbian_; }
    StructuralPlasticity* getStructuralPlasticity() { return &structuralPlasticity_; }
    SpikeSystem* getSpikeSystem() { return &spikeSystem_; }
    
    // Plasticity statistics and diagnostics
    void collectPlasticityStats(Brain* brain);
    float getAverageWeightChange() const { return averageWeightChange_; }
    size_t getPlasticityEventCount() const { return plasticityEventCount_; }
    
    // Configuration for testing and experimentation
    void enableHebbian(bool enable) { hebbian_.setLearningRate(enable ? 0.01f : 0.0f); }
    void enableSTDP(bool enable);
    void enableStructuralPlasticity(bool enable);
    void setPlasticityThreshold(float threshold) { covarianceThreshold_ = threshold; }
    
private:
    STDP stdp_;
    Hebbian hebbian_;
    StructuralPlasticity structuralPlasticity_;
    SpikeSystem spikeSystem_;
    
    // Plasticity statistics
    float averageWeightChange_ = 0.0f;
    size_t plasticityEventCount_ = 0;
    
    // Configuration parameters
    float covarianceThreshold_ = 0.0f;
    
    // Helper methods
    void processPlasticityEvents(Brain* brain, TimestepDuration timestep);
    void validatePlasticityState(Brain* brain) const;
    float calculatePlasticityModulation(Brain* brain) const;
    
    friend class BrainCore; // For internal access
};

} // namespace nlm