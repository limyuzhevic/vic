// Brain.hpp - Main Brain interface coordinating all subsystems
//
// This is the unified interface for the NLM Brain, providing access to all
// integrated subsystems while maintaining a clean, simplified API for users.
//
// The Brain class uses a composite pattern to manage multiple specialized
// subsystems, each responsible for a specific aspect of brain function:
// - BrainCore: Core brain management and initialization
// - BrainMemory: Memory systems (working, episodic, associative)
// - BrainNeuromodulation: Neuromodulation systems (dopamine, curiosity, etc.)
// - BrainPlasticity: Plasticity systems (STDP, Hebbian, structural)
// - BrainDevelopment: Development system
// - BrainCognition: Cognition systems (planning, concepts, attention)
// - BrainSimulation: Simulation loop and step processing
// - BrainCheckpoint: Checkpoint and persistence
//
// This separation allows for better modularity, testability, and maintainability
// while providing a unified interface to the complex brain architecture.

#ifndef NLM_BRAIN_HPP
#define NLM_BRAIN_HPP

#include <memory>
#include <string>
#include <vector>

#include "core/Config/Config.hpp"
#include "brain/BrainCore.hpp"
#include "brain/BrainMemory.hpp"
#include "brain/BrainNeuromodulation.hpp"
#include "brain/BrainPlasticity.hpp"
#include "brain/BrainDevelopment.hpp"
#include "brain/BrainCognition.hpp"
#include "brain/BrainSimulation.hpp"
#include "brain/BrainCheckpoint.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"

namespace nlm {

class Brain {
public:
    // Constructor and destructor
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Prevent copying
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Allow moving
    Brain(Brain&& other) noexcept;
    Brain& operator=(Brain&& other) noexcept;
    
    // Core brain operations
    bool initialize();
    void reset();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Basic accessors
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getRegionCount() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    float getExcitationInhibitionRatio() const;
    std::unique_ptr<Action> produceAction();
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Configuration access
    std::shared_ptr<Config> getConfig() const { return brainCore_->getConfig(); }
    
    // State management
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // System access (for integration with other subsystems)
    SpikeSystem* getSpikeSystem();
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    DevelopmentSystem* getDevelopmentSystem();
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    PredictionError* getPredictionError();
    Novelty* getNovelty();
    
    // Statistics and logging
    void logStatus() const;
    void save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Development and learning
    void develop();
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    
    // Helper methods
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    Synapse* getSynapse(SynapseId id);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get most active neurons (for monitoring)
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
private:
    // Subsystem instances
    std::unique_ptr<BrainCore> brainCore_;
    std::unique_ptr<BrainMemory> brainMemory_;
    std::unique_ptr<BrainNeuromodulation> brainNeuromodulation_;
    std::unique_ptr<BrainPlasticity> brainPlasticity_;
    std::unique_ptr<BrainDevelopment> brainDevelopment_;
    std::unique_ptr<BrainCognition> brainCognition_;
    std::unique_ptr<BrainSimulation> brainSimulation_;
    std::unique_ptr<BrainCheckpoint> brainCheckpoint_;
};

} // namespace nlm

#endif // NLM_BRAIN_HPP
