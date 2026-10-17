#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

/**
 * BrainBuilder - Fluent interface for configuring and creating NLM brains
 * 
 * Provides a clean, intuitive API for creating brains with specific
 * configurations, from minimal test brains to full research-grade systems.
 * 
 * Usage examples:
 * 
 * // Minimal brain for testing
 * auto brain = BrainBuilder()
 *     .buildMinimal();
 * 
 * // Standard research brain
 * auto brain = BrainBuilder()
 *     .addRegion("Primary", 500)
 *     .addRegion("Association", 200)
 *     .enableSTDP(true)
 *     .enableHebbian(true)
 *     .enableMemorySystem(true)
 *     .enablePredictionSystem(true)
 *     .enableCognition(true)
 *     .enableNeuromodulation(true)
 *     .buildStandard();
 * 
 * // Learning-focused brain
 * auto brain = BrainBuilder()
 *     .buildLearning();
 */

class BrainBuilder {
public:
    BrainBuilder();
    ~BrainBuilder();
    
    // Disable copying
    BrainBuilder(const BrainBuilder&) = delete;
    BrainBuilder& operator=(const BrainBuilder&) = delete;
    
    // Enable moving
    BrainBuilder(BrainBuilder&&) noexcept;
    BrainBuilder& operator=(BrainBuilder&&) noexcept;
    
    /**
     * Add a brain region with specified number of neurons
     * 
     * @param name Name for the region (for logging/debugging)
     * @param neuronCount Number of neurons in this region
     * @return Builder reference for method chaining
     */
    BrainBuilder& addRegion(const std::string& name, size_t neuronCount);
    
    /**
     * Set the total number of neurons (distributed across regions)
     * 
     * @param neuronCount Total neurons for the brain
     * @return Builder reference for method chaining
     */
    BrainBuilder& setNeuronCount(size_t neuronCount);
    
    /**
     * Set the number of brain regions
     * 
     * @param regionCount Number of regions (default: 1)
     * @return Builder reference for method chaining
     */
    BrainBuilder& setRegionCount(size_t regionCount);
    
    /**
     * Set connection probability between neurons
     * 
     * @param probability Probability (0.0 to 1.0)
     * @return Builder reference for method chaining
     */
    BrainBuilder& setConnectionProbability(float probability);
    
    /**
     * Enable or disable STDP (Spike-Timing-Dependent Plasticity)
     * 
     * @param enable Whether to enable STDP
     * @return Builder reference for method chaining
     */
    BrainBuilder& enableSTDP(bool enable);
    
    /**
     * Enable or disable Hebbian learning
     * 
     * @param enable Whether to enable Hebbian learning
     * @return Builder reference for method chaining
     */
    BrainBuilder& enableHebbian(bool enable);
    
    /**
     * Enable or disable structural plasticity
     * 
     * @param enable Whether to enable structural plasticity
     * @return Builder reference for method chaining
     */
    BrainBuilder& enableStructuralPlasticity(bool enable);
    
    /**
     * Enable or disable memory systems
     * 
     * @param enable Whether to enable memory systems
     * @return Builder reference for method chaining
     */
    BrainBuilder& enableMemorySystem(bool enable);
    
    /**
     * Enable or disable prediction system
     * 
     * @param enable Whether to enable prediction system
     * @return Builder reference for method chaining
     */
    BrainBuilder& enablePredictionSystem(bool enable);
    
    /**
     * Enable or disable cognition systems
     * 
     * @param enable Whether to enable cognition systems
     * @return Builder reference for method chaining
     */
    BrainBuilder& enableCognition(bool enable);
    
    /**
     * Enable or disable neuromodulation systems
     * 
     * @param enable Whether to enable neuromodulation systems
     * @return Builder reference for method chaining
     */
    BrainBuilder& enableNeuromodulation(bool enable);
    
    /**
     * Enable or disable development system
     * 
     * @param enable Whether to enable development system
     * @return Builder reference for method chaining
     */
    BrainBuilder& enableDevelopment(bool enable);
    
    /**
     * Enable or disable development stages
     * 
     * @param enable Whether to enable development stages
     * @return Builder reference for method chaining
     */
    BrainBuilder& enableDevelopmentStages(bool enable);
    
    /**
     * Set STDP parameters
     * 
     * @param ltpWeight LTP weight parameter
     * @param ltdWeight LTD weight parameter
     * @param tau Time constant
     * @return Builder reference for method chaining
     */
    BrainBuilder& setSTDPParameters(float ltpWeight, float ltdWeight, float tau);
    
    /**
     * Set structural plasticity parameters
     * 
     * @param synaptogenesisRate Rate of new synapse formation
     * @param pruningRate Rate of synapse removal
     * @return Builder reference for method chaining
     */
    BrainBuilder& setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate);
    
    /**
     * Set development parameters
     * 
     * @param enableCriticalPeriod Whether to include critical period
     * @param maturationRate Rate of maturation
     * @return Builder reference for method chaining
     */
    BrainBuilder& setDevelopmentParameters(bool enableCriticalPeriod, float maturationRate);
    
    /**
     * Configure for learning-focused research
     * 
     * Creates a brain optimized for rapid learning and adaptation
     * 
     * @return Builder reference for method chaining
     */
    BrainBuilder& configureForLearning();
    
    /**
     * Configure for cognitive research
     * 
     * Creates a brain optimized for cognitive tasks and planning
     * 
     * @return Builder reference for method chaining
     */
    BrainBuilder& configureForCognition();
    
    /**
     * Configure for performance testing
     * 
     * Creates a brain with minimal components for fast testing
     * 
     * @return Builder reference for method chaining
     */
    BrainBuilder& configureForPerformance();
    
    /**
     * Configure for development studies
     * 
     * Creates a brain with extended developmental stages
     * 
     * @return Builder reference for method chaining
     */
    BrainBuilder& configureForDevelopment();
    
    /**
     * Configure for standard research use
     * 
     * Creates a well-balanced brain suitable for most research scenarios
     * 
     * @return Builder reference for method chaining
     */
    BrainBuilder& configureForStandardResearch();
    
    /**
     * Build a minimal brain for testing
     * 
     * Creates a brain with only essential components:
     * - Basic neuron dynamics
     * - Minimal connectivity
     * - No plasticity (except basic)
     * - No memory systems
     * - No prediction/cognition/neuromodulation
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> buildMinimal();
    
    /**
     * Build a standard brain for research
     * 
     * Creates a complete brain with all major systems enabled
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> buildStandard();
    
    /**
     * Build a learning-focused brain
     * 
     * Creates a brain optimized for rapid adaptation and learning
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> buildLearning();
    
    /**
     * Build a cognitive brain
     * 
     * Creates a brain optimized for cognitive tasks and planning
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> buildCognition();
    
    /**
     * Build a brain for development studies
     * 
     * Creates a brain with extended developmental stages and maturation
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> buildDevelopment();
    
    /**
     * Build a brain with performance optimizations
     * 
     * Creates a brain optimized for speed and efficiency
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> buildPerformance();
    
    /**
     * Build a research-grade brain
     * 
     * Creates a comprehensive brain with all advanced features
     * suitable for research and experimentation
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> buildResearch();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Convenience factory functions for common brain configurations
 * 
 * These functions provide simple access to predefined brain configurations
 * for common use cases, reducing the need to use the builder interface directly.
 */

namespace BrainFactory {
    /**
     * Create a minimal brain for testing
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> createMinimal();
    
    /**
     * Create a standard brain for general research
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> createStandard();
    
    /**
     * Create a brain optimized for learning research
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> createLearning();
    
    /**
     * Create a brain optimized for cognitive research
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> createCognition();
    
    /**
     * Create a brain for development studies
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> createDevelopment();
    
    /**
     * Create a brain with performance optimizations
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> createPerformance();
    
    /**
     * Create a research-grade brain
     * 
     * @return Shared pointer to new Brain instance
     */
    std::shared_ptr<Brain> createResearch();
}

} // namespace nlm