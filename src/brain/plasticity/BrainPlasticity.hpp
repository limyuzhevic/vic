#pragma once

// brain/plasticity/BrainPlasticity.hpp
// Plasticity system management for the brain
// Extracts plasticity systems from monolithic Brain.cpp

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class Dopamine;
class Curiosity;
class Novelty;
class RandomGenerator;

// Brain plasticity system manager
class BrainPlasticity {
public:
    // Constructor
    explicit BrainPlasticity(std::shared_ptr<Config> config);
    
    // Destructor
    ~BrainPlasticity();
    
    // Disable copying
    BrainPlasticity(const BrainPlasticity&) = delete;
    BrainPlasticity& operator=(const BrainPlasticity&) = delete;
    
    // Configuration and initialization
    bool initialize(const std::vector<Neuron*>& neurons);
    void reset();
    
    // Plasticity system access
    SpikeSystem* getSpikeSystem() { return spikeSystem_.get(); }
    STDP* getSTDP() { return stdp_.get(); }
    Hebbian* getHebbian() { return hebbian_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Plasticity operations
    void updatePlasticity(double timestep);
    void applySynapticPlasticity(const std::vector<Neuron*>& neurons);
    void applySpikeTimingDependentPlasticity(double timestep);
    void applyHebbianLearning(double timestep);
    void applyStructuralPlasticity(double timestep);
    
    // Learning modulation
    void modulateLearningWithNeuromodulation(float dopamine, float curiosity, float novelty);
    void updateEligibilityTraces(const std::vector<Neuron*>& neurons, double timestep);
    
    // Statistics
    size_t getActiveSynapses() const { return activeSynapses_; }
    float getAverageSynapticWeight() const { return averageSynapticWeight_; }
    float getPlasticityStrength() const { return plasticityStrength_; }
    
    // Configuration utilities
    static bool validatePlasticityConfig(const std::shared_ptr<const Config>& config);
    static void applyDefaultPlasticitySettings(std::shared_ptr<Config> config);
    
    // Plasticity event tracking
    struct PlasticityEvent {
        std::chrono::steady_clock::time_point timestamp;
        std::string type;  // "STDP", "Hebbian", "Structural"
        size_t synapseCount;
        float weightChange;
        std::string source;  // "dopamine", "curiosity", "novelty"
    };
    
    // Get recent plasticity events
    std::vector<PlasticityEvent> getRecentPlasticityEvents(size_t limit = 100) const { return recentPlasticityEvents_; }
    
    // Plasticity simulation
    void simulatePlasticity(double timestep, const std::vector<Neuron*>& neurons);
    
    // Learning rate management
    void updateLearningRates(double timestep);
    void adaptLearningRatesBasedOnNeuromodulation(float dopamine, float curiosity);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core components
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    
    // Plasticity systems
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    
    // Neuromodulation systems for plasticity modulation
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    
    // Plasticity state
    std::map<size_t, float> eligibilityTraces_;
    std::vector<PlasticityEvent> recentPlasticityEvents_;
    
    // Statistics
    size_t activeSynapses_;
    float averageSynapticWeight_;
    float plasticityStrength_;
    
    // Learning rate management
    float learningRate_;
    float stdpLearningRate_;
    float hebbianLearningRate_;
    float structuralLearningRate_;
    
    // Plasticity thresholds
    float weightChangeThreshold_;
    float potentiationThreshold_;
    float depressionThreshold_;
};

} // namespace nlm
