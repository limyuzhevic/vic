// NLM Brain Initialization Helper
// Phase 2: Real Neural Computation
//
// This file provides helper classes for brain initialization and setup
// focusing on Phase 2: real spiking neural computation with integrated systems

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>

namespace nlm {

/**
 * @class BrainInitializationHelper
 * @brief Helper class for brain initialization and setup
 * 
 * This class encapsulates all brain initialization operations including:
 * - Region setup and configuration
 * - Population creation and type assignment
 * - Initial connectivity establishment
 * - Integration system initialization
 * - Error handling and recovery during initialization
 * 
 * Follows Single Responsibility Principle by focusing solely on brain setup
 */
class BrainInitializationHelper {
public:
    /**
     * @brief Exception thrown when brain initialization errors occur
     */
    class InitializationError : public std::runtime_error {
    public:
        explicit InitializationError(const std::string& message) : std::runtime_error(message) {}
    };
    
    /**
     * @brief Brain initialization status result
     */
    struct InitializationStatus {
        bool success;
        std::string errorMessage;
        std::vector<std::string> warnings;
        
        InitializationStatus() : success(false), errorMessage(), warnings() {}
        InitializationStatus(bool success, const std::string& error) 
            : success(success), errorMessage(error), warnings() {}
    };
    
    /**
     * @brief Setup regions for the brain based on configuration
     * 
     * @param brain Brain instance to configure
     * @param config Configuration containing region parameters
     * @return std::vector<RegionId> Created region IDs
     * 
     * @throws InitializationError if region setup fails
     */
    static std::vector<RegionId> setupRegions(Brain& brain, const Config& config);
    
    /**
     * @brief Create populations within regions
     * 
     * @param brain Brain instance to modify
     * @param neuronsPerRegion Number of neurons per region
     * @return bool true if populations created successfully
     */
    static bool createPopulations(Brain& brain, size_t neuronsPerRegion);
    
    /**
     * @brief Establish initial connectivity and synapse weights
     * 
     * @param brain Brain instance to configure
     * @param connectionProbability Probability of connection formation
     * @return bool true if connectivity established successfully
     */
    static bool establishConnectivity(Brain& brain, float connectionProbability);
    
    /**
     * @brief Initialize integration systems (memory, prediction, cognition, neuromodulation)
     * 
     * @param brain Brain instance to configure
     * @return bool true if all systems initialized successfully
     */
    static bool initializeIntegrationSystems(Brain& brain);
    
    /**
     * @brief Configure plasticity systems with parameters from configuration
     * 
     * @param brain Brain instance to configure
     * @param config Configuration containing plasticity parameters
     */
    static void configurePlasticitySystems(Brain& brain, const Config& config);
    
    /**
     * @brief Initialize the brain with all required systems
     * 
     * @param brain Brain instance to initialize
     * @param config Configuration for initialization
     * @return InitializationStatus with success/failure details
     */
    static InitializationStatus initializeBrain(Brain& brain, const Config& config);
    
    /**
     * @brief Reset brain to clean state for testing
     * 
     * @param brain Brain instance to reset
     */
    static void resetBrainForTesting(Brain& brain);
    
    /**
     * @brief Get brain initialization configuration summary
     * 
     * @param config Configuration to summarize
     */
    static void logInitializationConfiguration(const Config& config);
    
private:
    /**
     * @brief Setup individual region with neurons and populations
     * 
     * @param brain Brain instance to modify
     * @param regionId Region ID to setup
     * @param neuronsPerRegion Number of neurons for this region
     * @param neuronType Type of neurons to create (Sensory, Internal, Motor)
     */
    static void setupRegion(Brain& brain, RegionId regionId, size_t neuronsPerRegion, 
                           const std::vector<NeuronType>& neuronTypes);
    
    /**
     * @brief Create sensory and motor neurons for input/output
     * 
     * @param brain Brain instance to modify
     * @param regionId Region ID where neurons will be created
     * @param neuronsPerRegion Total neurons per region
     */
    static void createInputOutputNeurons(Brain& brain, RegionId regionId, size_t neuronsPerRegion);
    
    /**
     * @brief Initialize plasticity system components with configuration
     * 
     * @param brain Brain instance to configure
     * @param config Configuration containing plasticity parameters
     */
    static void initializePlasticityComponents(Brain& brain, const Config& config);
    
    /**
     * @brief Validate initialization parameters
     * 
     * @param config Configuration to validate
     * @return InitializationStatus with validation results
     */
    static InitializationStatus validateInitializationParameters(const Config& config);
};

} // namespace nlm
