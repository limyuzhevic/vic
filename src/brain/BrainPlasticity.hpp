#pragma once

#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainPlasticity {
public:
    // Constructor
    BrainPlasticity(std::shared_ptr<Config> config);
    ~BrainPlasticity();
    
    // Initialize plasticity systems
    void initialize();
    
    // Configure STDP parameters
    void configureSTDP(float ltpWeight, float ltdWeight, float tau);
    void setLTPWeight(float weight);
    void setLTDWeight(float weight);
    float getLTPWeight() const;
    float getLTDWeight() const;
    
    // Configure structural plasticity parameters
    void configureStructuralPlasticity(float synaptogenesisRate, float pruningRate);
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    
    // Apply STDP plasticity
    void applySTDP(Synapse* syn, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration timestep);
    
    // Apply Hebbian plasticity
    void applyHebbian(Synapse* syn, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration timestep);
    
    // Apply structural plasticity
    void applyStructuralPlasticity(Brain* brain, RandomGenerator& rng);
    
    // Update synaptic weights based on neuromodulation
    void applyNeuromodulationToPlasticity(float modulationFactor);
    
    // Get plasticity system accessors
    STDP* getSTDP() { return stdp.get(); }
    const STDP* getSTDP() const { return stdp.get(); }
    
    Hebbian* getHebbian() { return hebbian.get(); }
    const Hebbian* getHebbian() const { return hebbian.get(); }
    
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    const StructuralPlasticity* getStructuralPlasticity() const { return structuralPlasticity.get(); }
    
    // Get development stage for plasticity modulation
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage; }
    void setDevelopmentalStage(DevelopmentalStage stage) { developmentalStage = stage; }
    
    // Get neuromodulation factor for plasticity
    float getPlasticityModulationFactor() const { return plasticityModulationFactor; }
    void setPlasticityModulationFactor(float factor) { plasticityModulationFactor = factor; }
    
    // Update development effects on plasticity
    void updateDevelopmentEffects();
    
    // Reset plasticity state
    void reset();
    
    // Log plasticity system status
    void logStatus() const;
    
private:
    // Plasticity system components
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Configuration
    std::shared_ptr<Config> config;
    
    // Development stage
    DevelopmentalStage developmentalStage;
    
    // Neuromodulation factor for plasticity
    float plasticityModulationFactor;
    
    // Helper methods
    void configureSTDPFromConfig();
    void configureStructuralPlasticityFromConfig();
    float calculatePlasticityModulation() const;
};

} // namespace nlm