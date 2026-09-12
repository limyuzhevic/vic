// Brain initialization module - handles brain setup and configuration
// 
// This module provides functions for initializing the NLM brain, creating neural regions,
// setting up plasticity systems, and configuring integrated cognitive and memory systems.
// It serves as the primary interface for brain setup operations.
#pragma once

#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainInitialization
 * @brief Handles brain initialization and setup operations
 * 
 * The BrainInitialization class provides static methods for initializing the NLM brain
 * with configuration, creating neural regions, setting up plasticity systems, and
 * configuring integrated cognitive and memory systems. This module serves as the
 * primary interface for brain setup operations.
 * 
 * @details This class implements the initialization procedures required for Phase 6:
 * - Neural region creation based on configuration parameters
 * - Plasticity system configuration (STDP, Hebbian, structural)
 * - Memory system initialization (working, episodic, associative)
 * - Prediction system setup
 * - Cognitive system initialization (planning, concept formation, attention)
 * - Neuromodulation system setup (dopamine, curiosity, novelty)
 * - Spike system event handler registration
 * - Checkpoint system configuration
 * 
 * @note All initialization methods are static and operate on Brain* pointers,
 * providing a clean interface for brain setup without requiring full Brain
 * object construction during initialization.
 */
class BrainInitialization {
public:
    /**
     * @brief Initialize brain with configuration and set up neural architecture
     * 
     * This method performs the complete brain initialization sequence according to
     * Phase 6 specifications. It creates neural regions, initializes all integrated
     * systems, and configures the simulation environment for neural computation.
     * 
     * @param brain Pointer to Brain instance to initialize
     * @param config Configuration containing neural parameters and settings
     * @return bool True if initialization successful, false otherwise
     * 
     * @pre Brain instance must not be null
     * @post All brain systems are initialized and ready for simulation
     * 
     * @throws May throw exceptions during file operations or memory allocation
     */
    static bool initialize(Brain* brain, std::shared_ptr<Config> config);
    
    /**
     * @brief Create neural regions based on configuration
     * 
     * This method creates the neural regions specified in the configuration,
     * distributing neurons across regions and initializing connectivity patterns.
     * Each region contains sensory, internal, and motor neuron populations.
     * 
     * @param brain Pointer to Brain instance
     * @param neuronCount Total number of neurons to distribute
     * @param regionCount Number of regions to create
     * @param connectionProbability Probability of synaptic connections between neurons
     * 
     * @pre brain must not be null
     * @post Neural regions created with appropriate neuron populations
     * 
     * @note Regions are named Region_1, Region_2, etc. based on creation order
     */
    static void createRegions(Brain* brain, size_t neuronCount, 
                             size_t regionCount, float connectionProbability);
    
    /**
     * @brief Initialize plasticity systems with configuration parameters
     * 
     * This method configures the plasticity systems (STDP, Hebbian, structural)
     * using parameters from the configuration object.
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null
     * @post Plasticity systems configured and ready for learning
     */
    static void initializePlasticitySystems(Brain* brain);
    
    /**
     * @brief Initialize integrated cognitive and memory systems
     * 
     * This method initializes all integrated systems that are part of Phase 6:
     * memory systems (working, episodic, associative), prediction system,
     * cognitive systems (planning, concept formation, attention), and
     * neuromodulation systems (dopamine, curiosity, novelty).
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null
     * @post All integrated systems initialized and ready for simulation
     */
    static void initializeIntegratedSystems(Brain* brain);
    
    /**
     * @brief Configure spike system event handlers
     * 
     * This method registers event handlers for spike system events to enable
     * event-driven neural processing. Handlers are registered for both immediate
     * and delayed spike events.
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null
     * @post Spike system event handlers configured
     */
    static void configureSpikeSystem(Brain* brain);
    
    /**
     * @brief Configure checkpoint management system
     * 
     * This method configures the checkpoint management system using parameters
     * from the configuration object.
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null
     * @post Checkpoint system configured
     */
    static void configureCheckpointManager(Brain* brain);
};

} // namespace nlm