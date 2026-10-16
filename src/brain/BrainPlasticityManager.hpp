#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

class BrainPlasticityManager {
public:
    // Create plasticity manager with configuration
    explicit BrainPlasticityManager(std::shared_ptr<Config> config);
    
    ~BrainPlasticityManager();
    
    // Initialize plasticity manager
    bool initialize();
    
    // Apply plasticity rules to synapses
    void update(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                SimulationStep currentStep, Timestamp currentTime,
                TimestepDuration timestep, float plasticityMod = 1.0f);
    
    // Apply neuromodulation effects on plasticity
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Apply developmental effects on plasticity
    void applyDevelopment(const DevelopmentalStage& stage);
    
    // Structural plasticity (synaptogenesis, pruning)
    void updateStructuralPlasticity(BrainNeuralCore* neuralCore,
                                    std::unique_ptr<RandomGenerator>& rng);
    
    // Get access to plasticity components
    STDP* getSTDP();
    const STDP* getSTDP() const;
    
    Hebbian* getHebbian();
    const Hebbian* getHebbian() const;
    
    StructuralPlasticity* getStructuralPlasticity();
    const StructuralPlasticity* getStructuralPlasticity() const;
    
    // Plasticity rule configuration
    void configureSTDP(float ltpWeight, float ltdWeight, float tau);
    void setHebbianLearningRate(float rate);
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    
    // Get plasticity parameters
    float getLTPWeight() const;
    float getLTDWeight() const;
    float getTimeConstant() const;
    float getHebbianLearningRate() const;
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    
    // Reset plasticity state
    void reset();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Log plasticity manager status
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm