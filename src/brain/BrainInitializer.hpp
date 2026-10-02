// Brain Initializer - Handles brain initialization and configuration
// Separates initialization logic from the main Brain class

#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "Brain.hpp"

namespace nlm {

// Forward declarations for brain components
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class Dopamine;
class Curiosity;
class PredictionError;
class Novelty;
class CheckpointManager;

/**
 * @brief Manages the initialization of all brain systems and components
 * 
 * This class separates the complex initialization logic from the main Brain class,
 * making it more modular and testable. It handles:
 * - Random generator configuration
 * - Plasticity system setup
 * - Memory system initialization
 * - Cognition system configuration
 * - Neuromodulation system setup
 * - Checkpoint system configuration
 * - Neural region creation and population
 * 
 * @note This is a key part of the refactoring effort to reduce Brain.cpp size
 * and improve code organization by using the manager pattern.
 */
class BrainInitializer {
public:
    /**
     * @brief Constructor
     * 
     * @param config Shared pointer to the brain configuration
     */
    explicit BrainInitializer(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~BrainInitializer() = default;
    
    /**
     * @brief Initialize all brain systems
     * 
     * This method orchestrates the initialization of all integrated brain systems
     * using the provided brain instance.
     * 
     * @param brain Shared pointer to the Brain instance
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Configure the random generator
     * 
     * Sets up the random number generator using the seed from configuration.
     * 
     * @param rng Shared pointer to the random generator
     */
    void configureRandomGenerator(std::shared_ptr<RandomGenerator> rng);
    
    /**
     * @brief Configure plasticity systems
     * 
     * Sets up STDP, Hebbian, and structural plasticity systems with appropriate
     * parameters from configuration.
     * 
     * @param stdp Shared pointer to the STDP system
     * @param hebbian Shared pointer to the Hebbian system
     * @param structural Shared pointer to the structural plasticity system
     */
    void configurePlasticitySystems(std::shared_ptr<STDP> stdp, 
                                    std::shared_ptr<Hebbian> hebbian,
                                    std::shared_ptr<StructuralPlasticity> structural);
    
    /**
     * @brief Configure memory systems
     * 
     * Sets up working memory, episodic memory, and associative memory systems.
     * 
     * @param working Shared pointer to the working memory system
     * @param episodic Shared pointer to the episodic memory system
     * @param associative Shared pointer to the associative memory system
     */
    void configureMemorySystems(std::shared_ptr<NeuralWorkingMemory> working,
                                std::shared_ptr<NeuralEpisodicMemory> episodic,
                                std::shared_ptr<NeuralAssociativeMemory> associative);
    
    /**
     * @brief Configure cognition systems
     * 
     * Sets up neural planner, concept formation, and attention systems.
     * 
     * @param planner Shared pointer to the neural planner
     * @param concept Shared pointer to the concept formation system
     * @param attention Shared pointer to the attention system
     */
    void configureCognitionSystems(std::shared_ptr<NeuralPlanner> planner,
                                    std::shared_ptr<ConceptFormation> concept,
                                    std::shared_ptr<AttentionalSelection> attention);
    
    /**
     * @brief Configure neuromodulation systems
     * 
     * Sets up dopamine, curiosity, prediction error, and novelty systems.
     * 
     * @param dopamine Shared pointer to the dopamine system
     * @param curiosity Shared pointer to the curiosity system
     * @param predictionError Shared pointer to the prediction error system
     * @param novelty Shared pointer to the novelty system
     */
    void configureNeuromodulationSystems(std::shared_ptr<Dopamine> dopamine,
                                          std::shared_ptr<Curiosity> curiosity,
                                          std::shared_ptr<PredictionError> predictionError,
                                          std::shared_ptr<Novelty> novelty);
    
    /**
     * @brief Create neural regions with given parameters
     * 
     * Creates the specified number of regions and populates them with neurons
     * of different types (sensory, internal, motor).
     * 
     * @param brain Shared pointer to the Brain instance
     * @param neuronCount Total number of neurons to create
     * @param regionCount Number of regions to create
     * @param connectionProbability Probability of synaptic connections
     * @param rng Random generator for initialization
     */
    void createRegions(std::shared_ptr<Brain> brain, size_t neuronCount, 
                       size_t regionCount, float connectionProbability,
                       std::shared_ptr<RandomGenerator> rng);
    
    /**
     * @brief Initialize spike system handlers
     * 
     * Sets up event-driven spike processing by registering handlers for both
     * immediate and delayed spike events.
     * 
     * @param spikeSystem Shared pointer to the spike system
     * @param brain Shared pointer to the Brain instance
     */
    void setupSpikeSystemHandlers(std::shared_ptr<SpikeSystem> spikeSystem,
                                  std::shared_ptr<Brain> brain);
    
    /**
     * @brief Configure checkpoint system
     * 
     * Sets up the checkpoint manager for saving and loading brain state.
     * 
     * @param checkpointManager Shared pointer to the checkpoint manager
     */
    void configureCheckpointSystem(std::shared_ptr<CheckpointManager> checkpointManager);
    
private:
    /**
     * @brief Shared configuration for the brain
     */
    std::shared_ptr<Config> config_;
};

} // namespace nlm
