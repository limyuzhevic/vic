// Phase 2 Test Utilities
#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {
namespace phase2 {

/**
 * Test utilities namespace
 */
namespace TestUtils {
    
    /**
     * Create a brain with specified configuration
     */
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
    
    /**
     * Apply input current to a specific number of neurons
     */
    void applyInputCurrent(std::shared_ptr<Brain> brain, size_t neuronCount, 
                          NeuronType type = NeuronType::Sensory, 
                          double current = PHASE2_CURRENT_STRENGTH);
    
    /**
     * Measure network statistics
     */
    struct NetworkStats {
        uint64_t totalNeurons;
        uint64_t activeNeurons;
        uint64_t totalSpikes;
        double averageFiringRate;
        double averageWeight;
        std::vector<float> neuronWeights;
    };
    
    NetworkStats calculateNetworkStats(std::shared_ptr<Brain> brain);
    
    /**
     * Format a vector of numbers for output
     */
    std::string formatVector(const std::vector<float>& values, int precision = PHASE2_OUTPUT_PRECISION);
    
    /**
     * Check if two floating point values are approximately equal
     */
    bool approximatelyEqual(double a, double b, double epsilon = 1e-6);
    
    /**
     * Check if a learning event occurred
     */
    bool learningOccurred(const std::vector<float>& initialWeights, 
                         const std::vector<float>& finalWeights,
                         double threshold = PHASE2_LEARNING_THRESHOLD);
    
    /**
     * Calculate weight change statistics
     */
    struct WeightChangeStats {
        float initialMean;
        float finalMean;
        float change;
        size_t strengthened;
        size_t weakened;
        size_t unchanged;
    };
    
    WeightChangeStats calculateWeightChange(const std::vector<float>& initialWeights,
                                           const std::vector<float>& finalWeights);
    
    /**
     * Create input pattern for testing
     */
    void createInputPattern(std::shared_ptr<Brain> brain, size_t neuronCount, 
                           double current = PHASE2_CURRENT_STRENGTH);
    
    /**
     * Run simulation for specified number of steps
     */
    void runSimulation(std::shared_ptr<Brain> brain, uint32_t steps, 
                      double timestep = PHASE2_SIMULATION_TIMESTEP);
    
    /**
     * Print brain status information
     */
    void printBrainStatus(std::shared_ptr<Brain> brain, const std::string& title = "Brain Status");
    
    /**
     * Save brain state to checkpoint file
     */
    bool saveBrainCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    /**
     * Load brain state from checkpoint file
     */
    std::shared_ptr<Brain> loadBrainCheckpoint(const std::string& filepath, 
                                               std::shared_ptr<Config> config);
    
    /**
     * Measure execution time
     */
    template<typename Func>
    double measureExecutionTime(Func&& func);
    
    /**
     * Print formatted output with consistent formatting
     */
    void printFormatted(const std::string& label, double value, 
                       int precision = PHASE2_OUTPUT_PRECISION);
    
    /**
     * Print formatted output with consistent formatting
     */
    void printFormatted(const std::string& label, uint64_t value);
    
    /**
     * Print formatted output with consistent formatting
     */
    void printFormatted(const std::string& label, bool value);
    
    /**
     * Print formatted output with consistent formatting
     */
    void printFormatted(const std::string& label, const std::string& value);
    
    /**
     * Get current timestamp for logging
     */
    std::string getTimestamp();
    
    /**
     * Generate unique test identifier
     */
    std::string generateTestId(const std::string& prefix = "test");
    
    /**
     * Validate configuration parameters
     */
    bool validateConfig(const Config& config, std::vector<std::string>& errors);
    
    /**
     * Create default configuration for Phase 2 tests
     */
    std::shared_ptr<Config> createDefaultConfig();
    
    /**
     * Merge configuration with defaults
     */
    void mergeConfigWithDefaults(Config& config);
    
    /**
     * Check if brain is in valid state for testing
     */
    bool isBrainValid(std::shared_ptr<Brain> brain);
    
    /**
     * Calculate synaptic connectivity statistics
     */
    struct ConnectivityStats {
        uint64_t totalSynapses;
        uint64_t excitatorySynapses;
        uint64_t inhibitorySynapses;
        double averageWeight;
        double connectionDensity;
    };
    
    ConnectivityStats calculateConnectivityStats(std::shared_ptr<Brain> brain);
    
} // namespace TestUtils

} // namespace phase2
} // namespace nlm