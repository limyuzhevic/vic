#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
// Create brain with configuration
     explicit Brain(std::shared_ptr<Config> config);
     
     ~Brain();
     
     // Disable copying, enable moving
     Brain(const Brain&) = delete;
     Brain& operator=(const Brain&) = delete;
     Brain(Brain&&) noexcept;
     Brain& operator=(Brain&&) noexcept;
     
     /**
      * @brief Initialize brain with configuration
      * 
      * Sets up all neural components and systems based on the configuration.
      * This must be called before any other operations on the brain.
      * 
      * @return true if initialization succeeded, false otherwise
      *         (e.g., invalid config, memory allocation failure)
      */
     bool initialize();
     
     /**
      * @brief Check if the brain has been initialized
      * 
      * @return true if the brain is initialized and ready for simulation
      */
     bool isInitialized() const;
     
     // Main simulation step
     void step(SimulationStep currentStep);
     
     // Simulation time step
     void step(SimulationStep currentStep, Timestamp currentTime);
     
     /**
      * @brief Receive sensory input from environment
      * 
      * Injects current into sensory neurons based on input pattern.
      * This method validates the input before processing.
      * 
      * @param input The sensory input to process. Must not be null and
      *              must be properly initialized.
      * @throws std::invalid_argument if input is null
      * @throws std::runtime_error if input validation fails
      */
     void receiveSensoryInput(const class SensoryInput& input);
     
     /**
      * @brief Inject current directly into a specific neuron
      * 
      * Directly stimulates a neuron with the specified current.
      * This method validates both the neuron ID and current value.
      * 
      * @param neuron The ID of the neuron to inject current into
      * @param current The current value to inject (must be finite)
      * @throws std::invalid_argument if neuron ID is invalid
      * @throws std::invalid_argument if current is not finite
      */
     void injectCurrent(NeuronId neuron, MembranePotential current);
     
     /**
      * @brief Inject current into all neurons of a specific type
      * 
      * Directly stimulates all neurons of the specified type.
      * This method validates the neuron type and current value.
      * 
      * @param type The type of neurons to inject current into
      * @param current The current value to inject (must be finite)
      * @throws std::invalid_argument if current is not finite
      */
     void injectCurrentToNeurons(NeuronType type, MembranePotential current);
     
     // Spike system access
     SpikeSystem* getSpikeSystem();
     const SpikeSystem* getSpikeSystem() const;
     
     // Plasticity system access
     STDP* getSTDP();
     Hebbian* getHebbian();
     StructuralPlasticity* getStructuralPlasticity();
     
     // Statistics
     float getExcitationInhibitionRatio() const;
     size_t getTotalSpikeCount() const;
     size_t getPendingSpikeEventCount() const;
     
     /**
      * @brief Produce motor/action output based on motor neuron activity
      * 
      * Analyzes current neural activity in motor neurons and generates
      * a motor command for the agent to execute.
      * 
      * @return Pointer to the generated action. Returns nullptr if no
      *         motor neurons are active or if brain is not initialized.
      */
     std::unique_ptr<class Action> produceAction();
     
     /**
      * @brief Apply neuromodulatory signals
      * 
      * Applies neuromodulatory signals (dopamine, curiosity, etc.) to
      * influence neural plasticity and behavior.
      * 
      * @param signal The neuromodulatory signal to apply. Must not be null.
      * @throws std::invalid_argument if signal is null
      */
     void applyNeuromodulation(const class Neuromodulator& signal);
     
     /**
      * @brief Update plasticity rules (called automatically in step)
      * 
      * Applies synaptic plasticity rules based on current neural activity.
      * This includes STDP, Hebbian learning, and other plasticity mechanisms.
      */
     void updatePlasticity();
     
     /**
      * @brief Apply developmental changes (called automatically in step)
      * 
      * Advances the agent's developmental stage, adjusting plasticity rates
      * and other developmental parameters according to the current simulation age.
      * 
      * @param timestep Time elapsed since previous update (must be positive)
      * @throws std::invalid_argument if timestep is not positive
      */
     void develop(double timestep);
     
     /**
      * @brief Reset brain state
      * 
      * Resets all neural state to initial conditions, preparing for
      * a new simulation episode.
      */
     void reset();
     
     /**
      * @brief Save brain state to file (checkpointing)
      * 
      * Saves the current brain state to a file for later restoration.
      * This is used for checkpointing and resuming simulations.
      * 
      * @param filepath Path to the file to save to. Must be valid and writable.
      * @return true if save succeeded, false otherwise
      * @throws std::invalid_argument if filepath is empty or invalid
      * @throws std::runtime_error if file cannot be written
      */
     bool save(const std::string& filepath) const;
     
     /**
      * @brief Load brain state from file
      * 
      * Restores the brain state from a saved file.
      * This is used for resuming simulations from checkpoints.
      * 
      * @param filepath Path to the file to load from. Must be valid and readable.
      * @return true if load succeeded, false otherwise
      * @throws std::invalid_argument if filepath is empty or invalid
      * @throws std::runtime_error if file cannot be read or is corrupted
      */
     bool load(const std::string& filepath);
     
     // Region management
     RegionId addRegion(const std::string& name = "");
     NeuralRegion* getRegion(RegionId id);
     const NeuralRegion* getRegion(RegionId id) const;
     size_t getRegionCount() const;
     std::vector<RegionId> getRegionIds() const;
     
     // Get all regions
     const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
     
     /**
      * @brief Add an inter-region connection (long-range connectivity)
      * 
      * Creates a connection between two brain regions for long-range
      * communication. This connection can have its own plasticity rules.
      * 
      * @param source Region ID of the source neuron
      * @param target Region ID of the target neuron
      * @param weight Initial connection weight (default: 0.0)
      * @param delay Transmission delay in time steps (default: 1)
      * @throws std::invalid_argument if source or target region IDs are invalid
      */
     void addInterRegionConnection(RegionId source, RegionId target, 
                                   float weight = 0.0f, Delay delay = 1);
     
     /**
      * @brief Remove an inter-region connection
      * 
      * Removes a previously established connection between two regions.
      * 
      * @param source Region ID of the source neuron
      * @param target Region ID of the target neuron
      * @throws std::invalid_argument if source or target region IDs are invalid
      */
     void removeInterRegionConnection(RegionId source, RegionId target);
     
     // Global statistics
     size_t getTotalNeuronCount() const;
     size_t getTotalSynapseCount() const;
     size_t getActiveNeuronCount() const;
     size_t getFiringNeuronCount() const;
     float getAverageFiringRate() const;
     
     // ========== MEMORY SYSTEMS ==========
     
     // Working memory - transient active information
     NeuralWorkingMemory* getWorkingMemory();
     
     // Episodic memory - experience storage
     NeuralEpisodicMemory* getEpisodicMemory();
     
     // Associative memory - pattern associations
     NeuralAssociativeMemory* getAssociativeMemory();
     
     // ========== PREDICTION SYSTEM ==========
     
     // Prediction system for sensory prediction and error computation
     PredictionSystem* getPredictionSystem();
     
     // ========== COGNITION SYSTEMS ==========
     
     // Neural planner for action planning
     NeuralPlanner* getPlanner();
     
     // Concept formation for pattern discovery
     ConceptFormation* getConceptFormation();
     
     // Attentional selection for focus
     AttentionalSelection* getAttention();
     
     // ========== DEVELOPMENT SYSTEM ==========
     
     DevelopmentSystem* getDevelopmentSystem();
     DevelopmentalStage getDevelopmentalStage() const;
     void setDevelopmentalStage(DevelopmentalStage stage);
     
     // ========== NEUROMODULATION SYSTEMS ==========
     
     // Dopamine - reward and reinforcement
     Dopamine* getDopamine();
     
     // Curiosity - exploration motivation
     Curiosity* getCuriosity();
     
     // Novelty - novelty detection
     Novelty* getNovelty();
     
     // Prediction error signal
     PredictionError* getPredictionErrorSignal();
     
     /**
      * @brief Get current configuration
      * 
      * @return Shared pointer to the brain's configuration. Returns nullptr
      *         if brain is not initialized.
      */
     std::shared_ptr<const Config> getConfig() const;
     
     /**
      * @brief Get random generator
      * 
      * @return Pointer to the brain's random generator. Returns nullptr
      *         if brain is not initialized.
      */
     RandomGenerator* getRandomGenerator();
     
     /**
      * @brief Learned preference trait (Phase 5): a single scalar, -1..1,
      *        updated only by sandbox reward through AgentBrain, saved in the
      *        checkpoint (SimulationState section) and used as a small bias in
      *        later action choice. This is the measured "personality" step: a
      *        preference learned from experience that survives a reload and
      *        changes later behaviour. It is not a language-model persona and
      *        it does not, on its own, make the brain a human brain.
      * 
      * @param preference The new preference value (-1.0 to 1.0)
      * @throws std::invalid_argument if preference is not in valid range
      */
     float getLearnedPreference() const;
     void setLearnedPreference(float preference);
     
     /**
      * @brief Logging
      * 
      * Logs the current status of the brain for debugging and monitoring.
      */
     void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
