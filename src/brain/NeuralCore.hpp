#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>

namespace nlm {

// NeuralCore: Handles the core neural computation (LIF dynamics, spikes, plasticity)
// This represents the "brain" in the artificial brain architecture
class NeuralCore {
public:
    NeuralCore();
    ~NeuralCore();
    
    // Initialize with configuration
    bool initialize(std::shared_ptr<Config> config, 
                   const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Main neural computation step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Inject current into specific neurons
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Get statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Get spike system
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Get plasticity systems
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Process all regions' neurons
    void processNeurons(SimulationStep currentStep, Timestamp currentTime);
    
    // Handle spike propagation
    void processSpikes(SimulationStep currentStep);
    
    // Apply all plasticity rules
    void applyPlasticity(float plasticityMod, Timestamp currentTime);
    
    // Reset neural state
    void reset();
    
    // Get reference to regions for access
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Store working memory traces
    void storeWorkingMemoryTraces();
    
    // Update development effects
    void applyDevelopmentEffects(const DevelopmentSystem& devSystem, 
                                DevelopmentalStage stage, float plasticityMod);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm