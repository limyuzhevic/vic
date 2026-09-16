#pragma once

/**
 * Abstract Brain Interface
 * 
 * This provides an abstraction layer over the concrete Brain implementation,
 * following the Abstract Factory pattern. It allows for better testability,
 * extensibility, and adherence to the Single Responsibility Principle.
 * 
 * The abstract interface defines the core operations of a neural substrate
 * without exposing implementation details. Concrete implementations (like Brain)
 * provide the actual neural computation capabilities.
 */

#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuronId.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "dynamics/SpikeSystem.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

/**
 * AbstractBrain: Defines the interface for all neural processing substrates
 * 
 * This abstract base class encapsulates the core capabilities required
 * for any neural system, including sensing, action, memory, and plasticity.
 * It provides a clean abstraction that can be implemented by different
 * brain architectures (LIF, Hodgkin-Huxley, etc.) while maintaining a
 * consistent interface.
 */
class AbstractBrain {
public:
    virtual ~AbstractBrain() = default;
    
    // === Core Identity & Configuration ===
    
    /**
     * Get the unique identifier for this brain instance
     */
    virtual BrainId getId() const = 0;
    
    /**
     * Get the developmental stage of the brain
     */
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    
    /**
     * Get the total neuron count
     */
    virtual size_t getTotalNeuronCount() const = 0;
    
    /**
     * Get the total synapse count
     */
    virtual size_t getTotalSynapseCount() const = 0;
    
    /**
     * Get the configuration for this brain
     */
    virtual std::shared_ptr<const Config> getConfig() const = 0;
    
    // === Neural State Management ===
    
    /**
     * Initialize the brain with configuration
     * 
     * @return true if initialization succeeds, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * Reset brain to initial state
     */
    virtual void reset() = 0;
    
    /**
     * Perform one simulation step
     * 
     * @param step Current simulation step
     * @param time Current simulation time
     */
    virtual void step(SimulationStep step, Timestamp time) = 0;
    
    // === Neural Activity Monitoring ===
    
    /**
     * Get the current average firing rate
     */
    virtual float getAverageFiringRate() const = 0;
    
    /**
     * Get the total spike count
     */
    virtual size_t getTotalSpikeCount() const = 0;
    
    /**
     * Get the count of active neurons
     */
    virtual size_t getActiveNeuronCount() const = 0;
    
    /**
     * Get the count of firing neurons (currently active)
     */
    virtual size_t getFiringNeuronCount() const = 0;
    
    /**
     * Get the excitation-inhibition ratio
     */
    virtual float getExcitationInhibitionRatio() const = 0;
    
    // === I/O Interfaces ===
    
    /**
     * Process sensory input from environment
     * 
     * @param input Sensory information to process
     */
    virtual void receiveSensoryInput(const SensoryInput& input) = 0;
    
    /**
     * Inject current directly into a specific neuron
     * 
     * @param neuronId Target neuron ID
     * @param current Current magnitude to inject
     */
    virtual void injectCurrent(NeuronId neuronId, MembranePotential current) = 0;
    
    /**
     * Inject current into all neurons of a specific type
     * 
     * @param type Type of neurons to target
     * @param current Current magnitude to inject
     */
    virtual void injectCurrentToNeurons(NeuronType type, MembranePotential current) = 0;
    
    /**
     * Produce motor action based on current neural state
     * 
     * @return Unique pointer to motor action
     */
    virtual std::unique_ptr<Action> produceAction() = 0;
    
    // === Memory Systems ===
    
    /**
     * Get working memory interface
     */
    virtual NeuralWorkingMemory* getWorkingMemory() = 0;
    
    /**
     * Get episodic memory interface
     */
    virtual NeuralEpisodicMemory* getEpisodicMemory() = 0;
    
    /**
     * Get associative memory interface
     */
    virtual NeuralAssociativeMemory* getAssociativeMemory() = 0;
    
    // === Neuromodulation Systems ===
    
    /**
     * Get dopamine (reward prediction error) system
     */
    virtual Dopamine* getDopamine() = 0;
    
    /**
     * Get curiosity (exploration drive) system
     */
    virtual Curiosity* getCuriosity() = 0;
    
    /**
     * Get novelty (change detection) system
     */
    virtual Novelty* getNovelty() = 0;
    
    /**
     * Get prediction error system
     */
    virtual PredictionError* getPredictionError() = 0;
    
    // === Plasticity Systems ===
    
    /**
     * Get STDP (Spike-Timing Dependent Plasticity) system
     */
    virtual STDP* getSTDP() = 0;
    
    /**
     * Get Hebbian plasticity system
     */
    virtual Hebbian* getHebbian() = 0;
    
    /**
     * Get structural plasticity system
     */
    virtual StructuralPlasticity* getStructuralPlasticity() = 0;
    
    // === Development System ===
    
    /**
     * Get development system interface
     */
    virtual DevelopmentSystem* getDevelopmentSystem() = 0;
    
    /**
     * Update developmental state
     * 
     * @param timestep Time step for development
     */
    virtual void develop(TimestepDuration timestep) = 0;
    
    // === Prediction System ===
    
    /**
     * Get prediction system interface
     */
    virtual PredictionSystem* getPredictionSystem() = 0;
    
    // === Cognition Systems ===
    
    /**
     * Get neural planner interface
     */
    virtual NeuralPlanner* getPlanner() = 0;
    
    /**
     * Get concept formation interface
     */
    virtual ConceptFormation* getConceptFormation() = 0;
    
    /**
     * Get attentional selection interface
     */
    virtual AttentionalSelection* getAttention() = 0;
    
    // === State Management ===
    
    /**
     * Save current brain state to checkpoint
     * 
     * @param filepath Path to save checkpoint
     * @return true if successful, false otherwise
     */
    virtual bool save(const std::string& filepath) const = 0;
    
    /**
     * Load brain state from checkpoint
     * 
     * @param filepath Path to checkpoint file
     * @return true if successful, false otherwise
     */
    virtual bool load(const std::string& filepath) = 0;
    
    /**
     * Get spike system interface
     */
    virtual SpikeSystem* getSpikeSystem() = 0;
    
    /**
     * Apply neuromodulatory signal
     * 
     * @param signal Neuromodulator signal
     */
    virtual void applyNeuromodulation(const Neuromodulator& signal) = 0;
    
    /**
     * Update plasticity rules
     */
    virtual void updatePlasticity() = 0;
    
    // === Statistics & Monitoring ===
    
    /**
     * Get average synaptic weight across the network
     */
    virtual float getAverageSynapticWeight() const = 0;
    
    /**
     * Get spike system statistics
     */
    virtual size_t getPendingSpikeEventCount() const = 0;
    
    /**
     * Get most active neurons
     * 
     * @param count Number of most active neurons to return
     */
    virtual std::vector<NeuronId> getMostActiveNeurons(size_t count) const = 0;
    
    /**
     * Get complete statistics summary
     */
    virtual std::string getStatisticsSummary() const = 0;
    
protected:
    /**
     * Virtual constructor for concrete implementations
     */
    virtual void doInitialize() {}
};

/**
 * Brain Factory: Creates brain instances based on configuration
 * 
 * This factory pattern allows for easy creation of different brain
 * implementations while maintaining a consistent interface. It supports
 * configuration-based brain type selection and proper initialization.
 */
class BrainFactory {
public:
    /**
     * Create a brain of the specified type
     * 
     * @param type Brain type to create
     * @param config Configuration for the brain
     * @return Unique pointer to created brain
     */
    static std::unique_ptr<AbstractBrain> createBrain(BrainType type, std::shared_ptr<Config> config);
    
    /**
     * Create a standard NLM brain
     * 
     * @param config Configuration for the brain
     * @return Unique pointer to NLM brain
     */
    static std::unique_ptr<AbstractBrain> createStandardBrain(std::shared_ptr<Config> config);
    
    /**
     * Create a developmental brain (starts with high plasticity)
     * 
     * @param config Configuration for the brain
     * @return Unique pointer to developmental brain
     */
    static std::unique_ptr<AbstractBrain> createDevelopmentalBrain(std::shared_ptr<Config> config);
    
    /**
     * Create an adult brain (low plasticity, stable)
     * 
     * @param config Configuration for the brain
     * @return Unique pointer to adult brain
     */
    static std::unique_ptr<AbstractBrain> createAdultBrain(std::shared_ptr<Config> config);
};

} // namespace nlm