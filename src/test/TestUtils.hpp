#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <cassert>
#include <iostream>

namespace nlm {
namespace test {

/**
 * @brief Brain Factory - Creates configured brains for testing
 * 
 * This class provides factory methods for creating Brain objects
 * with consistent, biologically plausible configurations for testing.
 * It reduces boilerplate code and ensures test consistency.
 */
class BrainFactory {
public:
    /**
     * @brief Create a basic test brain with specified parameters
     * 
     * @param neuronCount Number of neurons to create (default: 100)
     * @param regionCount Number of brain regions (default: 1)
     * @param connectionProbability Synaptic connection probability (default: 0.1f)
     * @param seed Random seed for reproducible tests (default: 42)
     * @return Unique pointer to created Brain
     * 
     * @note Creates a biologically plausible configuration with reasonable
     *       default values for most neural parameters.
     */
    static std::unique_ptr<Brain> createBasicBrain(
        size_t neuronCount = 100,
        size_t regionCount = 1,
        float connectionProbability = 0.1f,
        SimulationStep seed = 42) {
        
        Config config = createBasicConfig(neuronCount, regionCount, connectionProbability);
        config.set("random_seed", static_cast<int64_t>(seed), ConfigSource::Runtime);
        
        return createBrainWithConfig(config);
    }
    
    /**
     * @brief Create a brain with realistic biological parameters
     * 
     * @param numRegions Number of brain regions (default: 3)
     * @param neuronsPerRegion Number of neurons per region (default: 50)
     * @param withRandomSeed Whether to include a random seed (default: true)
     * @return Unique pointer to created Brain
     * 
     * @note Creates a more realistic configuration based on biological
     *       data and typical neural network properties.
     */
    static std::unique_ptr<Brain> createRealisticBrain(
        size_t numRegions = 3,
        size_t neuronsPerRegion = 50,
        bool withRandomSeed = true) {
        
        Config config = createRealisticConfig(numRegions, neuronsPerRegion);
        
        if (withRandomSeed) {
            config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
        }
        
        return createBrainWithConfig(config);
    }
    
    /**
     * @brief Create a brain with minimal configuration
     * 
     * @param neuronCount Number of neurons (default: 50)
     * @param regionCount Number of regions (default: 1)
     * @return Unique pointer to created Brain
     * 
     * @note Creates a configuration with only essential parameters,
     *       suitable for fast unit tests.
     */
    static std::unique_ptr<Brain> createMinimalBrain(
        size_t neuronCount = 50,
        size_t regionCount = 1) {
        
        Config config = createMinimalConfig(neuronCount, regionCount);
        return createBrainWithConfig(config);
    }
    
    /**
     * @brief Create a brain with a specific configuration
     * 
     * @param config Configuration to use
     * @return Unique pointer to created Brain
     * 
     * @note The configuration must be valid and contain all necessary
     *       parameters for brain initialization.
     */
    static std::unique_ptr<Brain> createBrainWithConfig(const Config& config) {
        return std::make_unique<Brain>(std::make_shared<Config>(config));
    }
    
    /**
     * @brief Create a brain and initialize it
     * 
     * @param neuronCount Number of neurons (default: 100)
     * @param regionCount Number of regions (default: 1)
     * @return Unique pointer to initialized Brain
     * 
     * @note This convenience method creates and immediately initializes
     *       the brain, ready for simulation.
     */
    static std::unique_ptr<Brain> createAndInitializeBrain(
        size_t neuronCount = 100,
        size_t regionCount = 1) {
        
        auto brain = createBasicBrain(neuronCount, regionCount);
        bool success = brain->initialize();
        assert(success && "Brain initialization failed");
        return brain;
    }
private:
    /**
     * @brief Create a basic configuration for testing
     * 
     * @param neuronCount Number of neurons
     * @param regionCount Number of regions
     * @param connectionProbability Synaptic connection probability
     * @return Configuration object
     */
    static Config createBasicConfig(
        size_t neuronCount,
        size_t regionCount,
        float connectionProbability) {
        
        Config config;
        
        config.set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config.set("connection_probability", connectionProbability, ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        config.set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config.set("stdp_time_constant", 20.0f, ConfigSource::Default);
        config.set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
        config.set("development_pruning_rate", 0.0001f, ConfigSource::Default);
        
        return config;
    }
    
    /**
     * @brief Create a realistic configuration for testing
     * 
     * @param numRegions Number of regions
     * @param neuronsPerRegion Number of neurons per region
     * @return Configuration object
     */
    static Config createRealisticConfig(
        size_t numRegions,
        size_t neuronsPerRegion) {
        
        Config config;
        
        // Biological parameters based on cortical neuron data
        config.set("neuron_count", static_cast<int64_t>(numRegions * neuronsPerRegion), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(numRegions), ConfigSource::Default);
        config.set("connection_probability", 0.05f, ConfigSource::Default);  // Lower connectivity
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Plasticity parameters
        config.set("plasticity_learning_rate", 0.02f, ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.025f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.027f, ConfigSource::Default);
        config.set("stdp_time_constant", 25.0f, ConfigSource::Default);
        config.set("plasticity_hebbian_enable", true, ConfigSource::Default);
        config.set("plasticity_stdp_enable", true, ConfigSource::Default);
        
        // Development parameters
        config.set("development_synaptogenesis_rate", 0.002f, ConfigSource::Default);
        config.set("development_pruning_rate", 0.0002f, ConfigSource::Default);
        config.set("development_enable", true, ConfigSource::Default);
        
        // Neuromodulation
        config.set("neuromod_dopamine_scale", 1.0f, ConfigSource::Default);
        config.set("neuromod_curiosity_enable", true, ConfigSource::Default);
        config.set("neuromod_novelty_enable", true, ConfigSource::Default);
        
        return config;
    }
    
    /**
     * @brief Create a minimal configuration for fast testing
     * 
     * @param neuronCount Number of neurons
     * @param regionCount Number of regions
     * @return Configuration object
     */
    static Config createMinimalConfig(
        size_t neuronCount,
        size_t regionCount) {
        
        Config config;
        
        config.set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config.set("connection_probability", 0.1f, ConfigSource::Default);
        
        return config;
    }
};

/**
 * @brief Config Factory - Creates test configurations
 * 
 * This class provides factory methods for creating Configuration objects
 * with various purposes and complexity levels for testing.
 */
class ConfigFactory {
public:
    /**
     * @brief Create a minimal configuration
     * 
     * @return Configuration object with only essential parameters
     */
    static Config createMinimal() {
        Config config;
        config.set("neuron_count", 100, ConfigSource::Default);
        config.set("region_count", 1, ConfigSource::Default);
        return config;
    }
    
    /**
     * @brief Create a comprehensive configuration
     * 
     * @return Configuration object with all possible parameters
     */
    static Config createComprehensive() {
        Config config;
        
        // Basic parameters
        config.set("neuron_count", 1000, ConfigSource::Default);
        config.set("region_count", 10, ConfigSource::Default);
        config.set("connection_probability", 0.15f, ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Plasticity
        config.set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
        config.set("plasticity_hebbian_enable", true, ConfigSource::Default);
        config.set("plasticity_stdp_enable", true, ConfigSource::Default);
        config.set("plasticity_structural_enable", true, ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
        config.set("stdp_time_constant", 20.0f, ConfigSource::Default);
        
        // Development
        config.set("development_enable", true, ConfigSource::Default);
        config.set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
        config.set("development_pruning_rate", 0.0001f, ConfigSource::Default);
        
        // Neuromodulation
        config.set("neuromod_dopamine_scale", 1.0f, ConfigSource::Default);
        config.set("neuromod_curiosity_enable", true, ConfigSource::Default);
        config.set("neuromod_novelty_enable", true, ConfigSource::Default);
        
        // Environment
        config.set("environment_name", "GridWorld", ConfigSource::Default);
        config.set("environment_width", 20, ConfigSource::Default);
        config.set("environment_height", 20, ConfigSource::Default);
        
        // Logging
        config.set("log_level", "INFO", ConfigSource::Default);
        config.set("log_to_file", false, ConfigSource::Default);
        
        return config;
    }
    
    /**
     * @brief Create a configuration with validation schema
     * 
     * @return Configuration object with schema validation
     */
    static Config createValidatedConfig() {
        Config config = createComprehensive();
        
        // Add schema validation entries
        SchemaEntry neuronCountEntry("neuron_count", "int", "Number of neurons", true);
        neuronCountEntry.range = std::make_pair(1, 10000);
        config.addSchemaEntry(neuronCountEntry);
        
        SchemaEntry connectionProbEntry("connection_probability", "double", "Connection probability");
        connectionProbEntry.range = std::make_pair(0.0, 1.0);
        config.addSchemaEntry(connectionProbEntry);
        
        SchemaEntry learningRateEntry("plasticity_learning_rate", "double", "Learning rate");
        learningRateEntry.range = std::make_pair(0.0, 1.0);
        config.addSchemaEntry(learningRateEntry);
        
        return config;
    }
    
    /**
     * @brief Create configuration from file
     * 
     * @param filepath Path to configuration file
     * @return True if configuration was loaded successfully
     */
    static bool createFromFile(const std::string& filepath) {
        // This would typically be used in actual test code
        // For now, return false as this would require actual file parsing
        return false;
    }
};

/**
 * @brief RandomSeedManager - Manages random seed reproducibility
 * 
 * This class provides utilities for creating random generators with
 * specific seeds and verifying reproducibility of random sequences.
 */
class RandomSeedManager {
public:
    /**
     * @brief Create a seeded random generator
     * 
     * @param seed Random seed (default: 42)
     * @return RandomGenerator with specified seed
     */
    static RandomGenerator createSeededGenerator(SimulationStep seed = 42) {
        return RandomGenerator(seed);
    }
    
    /**
     * @brief Verify random sequence reproducibility
     * 
     * @param seed Random seed to test
     * @return True if sequence is reproducible
     */
    static bool verifyReproducibility(SimulationStep seed) {
        RandomGenerator rng1(seed);
        RandomGenerator rng2(seed);
        
        // Generate sequence of random values
        std::vector<float> sequence1;
        std::vector<float> sequence2;
        
        for (size_t i = 0; i < 1000; ++i) {
            sequence1.push_back(rng1.uniformReal(0.0, 1.0));
            sequence2.push_back(rng2.uniformReal(0.0, 1.0));
        }
        
        return sequence1 == sequence2;
    }
    
    /**
     * @brief Get deterministic random values for testing
     * 
     * @param count Number of random values to generate
     * @param seed Random seed (default: 42)
     * @return Vector of deterministic random values
     */
    static std::vector<float> getDeterministicSequence(
        size_t count,
        SimulationStep seed = 42) {
        
        RandomGenerator rng(seed);
        std::vector<float> sequence;
        
        for (size_t i = 0; i < count; ++i) {
            sequence.push_back(rng.uniformReal(0.0, 1.0));
        }
        
        return sequence;
    }
};

/**
 * @brief Benchmark utilities for performance testing
 * 
 * This class provides timing and performance measurement utilities
 * for benchmarking brain operations and system performance.
 */
class BenchmarkUtils {
public:
    /**
     * @brief Time brain initialization
     * 
     * @param neuronCount Number of neurons (default: 1000)
     * @param regionCount Number of regions (default: 5)
     * @return Time in seconds
     */
    static double timeBrainInitialization(
        size_t neuronCount = 1000,
        size_t regionCount = 5) {
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto brain = BrainFactory::createAndInitializeBrain(neuronCount, regionCount);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        return duration.count();
    }
    
    /**
     * @brief Time simulation steps
     * 
     * @param brain Brain to simulate
     * @param steps Number of simulation steps (default: 100)
     * @return Time in seconds
     */
    static double timeSimulationSteps(
        std::unique_ptr<Brain> brain,
        size_t steps = 100) {
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        return duration.count();
    }
    
    /**
     * @brief Measure memory usage (placeholder)
     * 
     * @param brain Brain to measure
     * @return Memory usage in bytes (placeholder implementation)
     */
    static size_t measureMemoryUsage(std::unique_ptr<Brain> brain) {
        // Placeholder implementation - in real implementation this would use
        // system-specific memory measurement functions
        return 0;
    }
};

/**
 * @brief Test assertions and utilities
 * 
 * This class provides enhanced test assertions with better error messages
 * and helper functions for common test scenarios.
 */
class TestAssertions {
public:
    /**
     * @brief Assert brain state is valid
     * 
     * @param brain Brain to validate
     * @throw AssertionError if brain is invalid
     */
    static void assertBrainValid(const Brain& brain) {
        assert(brain.getTotalNeuronCount() > 0 && "Brain should have neurons");
        assert(brain.getTotalSynapseCount() >= 0 && "Synapse count should be non-negative");
        assert(brain.getAverageFiringRate() >= 0.0f && "Firing rate should be non-negative");
        assert(brain.getTotalSpikeCount() >= 0 && "Spike count should be non-negative");
    }
    
    /**
     * @brief Assert configuration is valid
     * 
     * @param config Configuration to validate
     * @throw AssertionError if configuration is invalid
     */
    static void assertConfigValid(const Config& config) {
        auto keys = config.getKeys();
        assert(!keys.empty() && "Configuration should have keys");
        
        for (const auto& key : keys) {
            assert(config.has(key) && "Configuration should contain claimed key");
        }
    }
    
    /**
     * @brief Assert random sequence matches expected
     * 
     * @param rng Random generator
     * @param expectedValues Expected random sequence
     * @throw AssertionError if sequences don't match
     */
    static void assertRandomSequence(
        const RandomGenerator& rng,
        const std::vector<float>& expectedValues) {
        
        for (size_t i = 0; i < expectedValues.size(); ++i) {
            float actual = rng.uniformReal(0.0, 1.0);
            assert(std::abs(actual - expectedValues[i]) < 0.0001f &&
                   "Random sequence should match expected values");
        }
    }
    
    /**
     * @brief Assert approximate equality with tolerance
     * 
     * @tparam T Numeric type
     * @param actual Actual value
     * @param expected Expected value
     * @param tolerance Tolerance for comparison
     * @throw AssertionError if values differ by more than tolerance
     */
    template<typename T>
    static void assertApproxEqual(T actual, T expected, T tolerance) {
        assert(std::abs(actual - expected) <= tolerance &&
               "Values should be approximately equal");
    }
    
    /**
     * @brief Assert value is within bounds
     * 
     * @tparam T Comparable type
     * @param value Value to check
     * @param min Minimum allowed value
     * @param max Maximum allowed value
     * @throw AssertionError if value is out of bounds
     */
    template<typename T>
    static void assertWithinBounds(T value, T min, T max) {
        assert(value >= min && value <= max &&
               "Value should be within specified bounds");
    }
};

} // namespace test
} // namespace nlm

// Convenience macros for common test patterns
#define TEST_BRAIN_CREATION(testName, neuronCount, regionCount) \
    void testName() { \
        auto brain = nlm::test::BrainFactory::createAndInitializeBrain(neuronCount, regionCount); \
        assert(brain->getTotalNeuronCount() == neuronCount); \
        assert(brain->getRegionCount() == regionCount); \
        std::cout << "    " #testName " passed" << std::endl; \
    }

#define TEST_BRAIN_STEP(testName, neuronCount, steps) \
    void testName() { \
        auto brain = nlm::test::BrainFactory::createAndInitializeBrain(neuronCount, 1); \
        for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(steps); ++step) { \
            brain->step(step); \
        } \
        assert(brain->getTotalNeuronCount() == neuronCount); \
        std::cout << "    " #testName " passed" << std::endl; \
    }

#define BENCHMARK_SIMPLE(testName, brain, code) \
    void testName() { \
        auto start = std::chrono::high_resolution_clock::now(); \
        code; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); \
        std::cout << "    " #testName " passed (duration: " << duration << "ms)" << std::endl; \
    }

// Test runner for simple test suites
class SimpleTestRunner {
public:
    template<typename TestFunction>
    static void runTest(TestFunction testFunc, const std::string& testName) {
        std::cout << "Running test: " << testName << std::endl;
        testFunc();
        std::cout << "Test completed: " << testName << std::endl;
    }
    
    template<typename... TestFunctions>
    static void runAllTests(TestFunctions... tests) {
        (runTest(tests, #tests), ...);
    }
};

// Test function for BrainFactory (example)
void testBrainFactoryCreation() {
    std::cout << "Testing Brain Factory creation..." << std::endl;
    
    auto brain1 = nlm::test::BrainFactory::createAndInitializeBrain(100, 2);
    assert(brain1->getTotalNeuronCount() == 100);
    
    auto brain2 = nlm::test::BrainFactory::createRealisticBrain(5, 50);
    assert(brain2->getTotalNeuronCount() == 250);
    
    std::cout << "Brain Factory creation test passed" << std::endl;
}

// Example test function for random seed management
void testRandomSeedManagement() {
    std::cout << "Testing random seed management..." << std::endl;
    
    auto sequence1 = nlm::test::RandomSeedManager::getDeterministicSequence(100, 42);
    auto sequence2 = nlm::test::RandomSeedManager::getDeterministicSequence(100, 42);
    
    assert(sequence1 == sequence2 && "Random sequences should be reproducible");
    
    bool isReproducible = nlm::test::RandomSeedManager::verifyReproducibility(123);
    assert(isReproducible && "Seed reproducibility should work");
    
    std::cout << "Random seed management test passed" << std::endl;
}

// Main function for example test suite
void runExampleTestSuite() {
    std::cout << "Running example test suite..." << std::endl;
    
    testBrainFactoryCreation();
    testRandomSeedManagement();
    
    std::cout << "All example tests passed!" << std::endl;
}

// Integration test example
void runIntegrationTests() {
    std::cout << "Running integration tests..." << std::endl;
    
    // Test 1: Brain creation and basic functionality
    auto brain = nlm::test::BrainFactory::createAndInitializeBrain(200, 3);
    brain->step(0);
    assert(brain->getTotalNeuronCount() > 0);
    
    // Test 2: Brain operations
    brain->reset();
    brain->initialize();
    brain->step(1);
    assert(brain->getTotalSynapseCount() >= 0);
    
    // Test 3: Brain statistics
    assert(brain->getAverageFiringRate() >= 0.0f);
    assert(brain->getTotalSpikeCount() >= 0);
    
    std::cout << "Integration tests passed!" << std::endl;
}

// Performance benchmark example
void runBenchmarks() {
    std::cout << "Running benchmarks..." << std::endl;
    
    // Brain initialization benchmark
    auto initTime = nlm::test::BenchmarkUtils::timeBrainInitialization(500, 5);
    std::cout << "Brain initialization (500 neurons, 5 regions): " << initTime << " seconds" << std::endl;
    
    // Simulation steps benchmark
    auto brain = nlm::test::BrainFactory::createAndInitializeBrain(100, 2);
    auto stepTime = nlm::test::BenchmarkUtils::timeSimulationSteps(brain, 50);
    std::cout << "Simulation steps (50 steps): " << stepTime << " seconds" << std::endl;
    
    std::cout << "Benchmarks completed!" << std::endl;
}

// Simple test runner for demonstration
int main() {
    std::cout << "NLM Test Suite Demo" << std::endl;
    std::cout << "====================" << std::endl;
    
    // Run example tests
    runExampleTestSuite();
    
    // Run integration tests
    runIntegrationTests();
    
    // Run benchmarks
    runBenchmarks();
    
    std::cout << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    
    return 0;
}

} // namespace nlm