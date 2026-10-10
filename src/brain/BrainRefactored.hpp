// BrainRefactored.hpp - Refactored Brain with component-based architecture
#pragma once

#include "BrainComponent.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace nlm {

// Forward declarations for brain components
class NeuralDynamicsComponent;
class PlasticityComponent;
class MemoryComponent;
class PredictionComponent;
class SpikeSystem;

// Refactored Brain class with component-based architecture
// The Brain now acts as a coordinator that composes and manages different components
class BrainRefactored {
public:
    // Create brain with configuration
    explicit BrainRefactored(std::shared_ptr<Config> config);
    
    ~BrainRefactored();
    
    // Disable copying, enable moving
    BrainRefactored(const BrainRefactored&) = delete;
    BrainRefactored& operator=(const BrainRefactored&) = delete;
    BrainRefactored(BrainRefactored&&) noexcept;
    BrainRefactored& operator=(BrainRefactored&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Component management
    bool addComponent(std::unique_ptr<BrainComponent> component);
    BrainComponent* getComponent(const std::string& name);
    const BrainComponent* getComponent(const std::string& name) const;
    std::vector<BrainComponent*> getComponents() const;
    bool removeComponent(const std::string& name);
    
    // Component type management
    bool registerComponentType(const std::string& type, 
                              const std::string& name,
                              std::function<std::unique_ptr<BrainComponent>()> creator);
    
    // Component activation/deactivation
    void activateComponent(const std::string& name, bool activate);
    bool isComponentActive(const std::string& name) const;
    
    // Statistics and access to components
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules
    void updatePlasticity();
    
    // Apply developmental changes
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    class RandomGenerator* getRandomGenerator();
    
    // Get component by type (convenience method)
    NeuralDynamicsComponent* getNeuralDynamics() { return neuralDynamicsComponent_.get(); }
    PlasticityComponent* getPlasticity() { return plasticityComponent_.get(); }
    MemoryComponent* getMemory() { return memoryComponent_.get(); }
    PredictionComponent* getPrediction() { return predictionComponent_.get(); }
    
    const NeuralDynamicsComponent* getNeuralDynamics() const { return neuralDynamicsComponent_.get(); }
    const PlasticityComponent* getPlasticity() const { return plasticityComponent_.get(); }
    const MemoryComponent* getMemory() const { return memoryComponent_.get(); }
    const PredictionComponent* getPrediction() const { return predictionComponent_.get(); }
    
    // Get brain status for logging
    std::string getStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core components (managed by the refactored Brain)
    std::unique_ptr<NeuralDynamicsComponent> neuralDynamicsComponent_;
    std::unique_ptr<PlasticityComponent> plasticityComponent_;
    std::unique_ptr<MemoryComponent> memoryComponent_;
    std::unique_ptr<PredictionComponent> predictionComponent_;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Component registry
    std::unordered_map<std::string, std::unique_ptr<BrainComponent>> components_;
    
    // Helper methods
    bool initializeDefaultComponents();
    void setupComponentRelationships();
    void loadConfiguration();
};

} // namespace nlm
