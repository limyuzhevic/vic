#pragma once

// BrainPlasticityIntegration.hpp - STDP, Hebbian, structural plasticity integration
// Contains plasticity system integration functionality

#include "BrainCore.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class BrainCore;

// Brain Plasticity Integration Implementation
class BrainPlasticityIntegration {
public:
    struct Impl;
    Impl* pImpl;
    
    // Create brain plasticity integration with reference to core brain
    explicit BrainPlasticityIntegration(BrainCore* core);
    
    ~BrainPlasticityIntegration();
    
    // Disable copying, enable moving
    BrainPlasticityIntegration(const BrainPlasticityIntegration&) = delete;
    BrainPlasticityIntegration& operator=(const BrainPlasticityIntegration&) = delete;
    BrainPlasticityIntegration(BrainPlasticityIntegration&&) noexcept;
    BrainPlasticityIntegration& operator=(BrainPlasticityIntegration&&) noexcept;
    
    // Initialize plasticity systems
    bool initialize();
    
    // Update plasticity systems
    void update(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                TimestepDuration dt, float plasticityMod, SimulationStep currentStep, Timestamp currentTime);
    
    // Get plasticity system references
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Apply plasticity rules
    void applySTDP();
    void applyHebbianLearning();
    void applyStructuralPlasticity();
    
    // Configure plasticity parameters
    void configureSTDP(float ltpWeight, float ltdWeight, float tau);
    void configureStructuralPlasticity(float synaptogenesisRate, float pruningRate);
    
    // Clear plasticity state
    void clear();
    
    // Get statistics
    size_t getSTDPUpdates() const;
    size_t getHebbianUpdates() const;
    size_t getStructuralPlasticityEvents() const;
    float getTotalPlasticity() const;
    
private:
    // Internal helper methods
    void applySTDPToRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    void applyHebbianToRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    void updateStructuralPlasticity(BrainCore* brain, RandomGenerator& rng);
    void updateSynaptogenesis(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    void updatePruning(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Utility methods
    void calculatePlasticityStats() const;
    
    struct Impl {
        BrainCore* brainCore;
        
        // Plasticity system references
        STDP* stdp;
        Hebbian* hebbian;
        StructuralPlasticity* structuralPlasticity;
        
        // Plasticity state
        std::vector<float> weightHistory;
        std::vector<size_t> plasticityEvents;
        
        // Statistics
        size_t stdpUpdates;
        size_t hebbianUpdates;
        size_t structuralPlasticityEvents;
        
        Impl(BrainCore* core);
    };
};

} // namespace nlm

