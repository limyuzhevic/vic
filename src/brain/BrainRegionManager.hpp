// Brain Region Manager - Handles region creation and management
// Separates region management logic from the main Brain class

#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

/**
 * @brief Manages neural regions, neurons, and their connectivity
 * 
 * This class separates the region management logic from the main Brain class,
 * making it more modular and testable. It handles:
 * - Region creation and population
 * - Neuron population management
 * - Synaptic connectivity initialization
 * - Sensory and motor neuron collection
 * - Region access and statistics
 * 
 * @note This is a key part of the refactoring effort to reduce Brain.cpp size
 * and improve code organization by using the manager pattern. It encapsulates
 * all region-specific logic in a dedicated class.
 */
class BrainRegionManager {
public:
    /**
     * @brief Add a new region with specified name
     * 
     * Creates a new neural region with the given name and returns its ID.
     * 
     * @param brain Shared pointer to the Brain instance
     * @param name Optional name for the region (empty for auto-generated name)
     * @return RegionId The ID of the newly created region
     */
    RegionId addRegion(std::shared_ptr<Brain> brain, const std::string& name = "");
    
    /**
     * @brief Get region by ID
     * 
     * Retrieves a region from the brain by its ID.
     * 
     * @param brain Shared pointer to the Brain instance
     * @param id Region ID to retrieve
     * @return NeuralRegion* Pointer to the region, or nullptr if not found
     */
    NeuralRegion* getRegion(std::shared_ptr<Brain> brain, RegionId id);
    
    /**
     * @brief Get region by ID (const version)
     * 
     * Retrieves a region from the brain by its ID (const version).
     * 
     * @param brain Shared pointer to the Brain instance
     * @param id Region ID to retrieve
     * @return const NeuralRegion* Pointer to the region, or nullptr if not found
     */
    const NeuralRegion* getRegion(std::shared_ptr<Brain> brain, RegionId id) const;
    
    /**
     * @brief Get all regions
     * 
     * Returns a vector of all regions in the brain.
     * 
     * @param brain Shared pointer to the Brain instance
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions(std::shared_ptr<Brain> brain) const;
    
    /**
     * @brief Get region count
     * 
     * Returns the number of regions in the brain.
     * 
     * @param brain Shared pointer to the Brain instance
     * @return size_t Number of regions
     */
    size_t getRegionCount(std::shared_ptr<Brain> brain) const;
    
    /**
     * @brief Create neurons in a region with specified populations
     * 
     * Creates neurons of different types (sensory, internal, motor) in the specified
     * region with appropriate population sizes and initializes their connectivity.
     * 
     * @param brain Shared pointer to the Brain instance
     * @param regionId ID of the region to populate
     * @param neuronCount Total number of neurons to create
     * @param regionCount Number of regions in the brain
     * @param connectionProbability Probability of synaptic connections
     * @param rng Random generator for initialization
     */
    void createNeuronsInRegion(std::shared_ptr<Brain> brain, RegionId regionId,
                               size_t neuronCount, size_t regionCount,
                               float connectionProbability,
                               std::shared_ptr<RandomGenerator> rng);
    
    /**
     * @brief Collect sensory and motor neurons for I/O
     * 
     * Identifies and collects sensory and motor neurons from a region for use
     * in sensory input and motor output processing.
     * 
     * @param brainImpl Shared pointer to the Brain instance
     * @param regionId ID of the region to analyze
     */
    void collectSensoryAndMotorNeurons(std::shared_ptr<Brain> brainImpl, RegionId regionId);
    
private:
    /**
     * @brief Helper method to get the Impl pointer from Brain
     * 
     * Provides a static helper method to access the internal implementation
     * pointer of the Brain class.
     * 
     * @param brain Shared pointer to the Brain instance
     * @return Brain::Impl* Pointer to the Brain implementation
     */
    static Brain::Impl* getImpl(std::shared_ptr<Brain> brain);
};

} // namespace nlm
