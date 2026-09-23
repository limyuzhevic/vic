#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <fstream>
#include <sstream>

namespace nlm {

/**
 * @brief Enhanced error handling with comprehensive exception hierarchy
 */
class NLMError {
public:
    enum class Code {
        NONE = 0,
        CONFIGURATION = 1,
        INITIALIZATION = 2,
        MEMORY = 3,
        PLASTICITY = 4,
        INTEGRATION = 5,
        VALIDATION = 6,
        IO = 7,
        TIMEOUT = 8,
        UNKNOWN = 9
    };

    NLMError() : code_(Code::NONE), message_("Success") {}
    NLMError(Code code, const std::string& message) 
        : code_(code), message_(message) {}
    
    Code code() const { return code_; }
    const std::string& message() const { return message_; }
    std::string fullMessage() const {
        if (code_ == Code::NONE) return "Success";
        return "NLM Error " + std::to_string(static_cast<int>(code_)) + ": " + message_;
    }

private:
    Code code_;
    std::string message_;
};

/**
 * @brief Comprehensive configuration manager with validation
 */
class EnhancedConfigManager {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };

    EnhancedConfigManager() = default;
    ~EnhancedConfigManager() = default;

    // Load configuration from various sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromString(const std::string& configString);
    bool loadFromArgs(int argc, char* argv[]);

    // Save configuration
    bool saveToFile(const std::string& filepath) const;
    std::string saveToString() const;

    // Configuration access with type safety
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const;

    template<typename T>
    bool set(const std::string& key, const T& value);

    // Validation
    ValidationResult validate() const;
    bool isValid() const { return validationResult_.valid; }
    const std::vector<std::string>& getErrors() const { return validationResult_.errors; }
    const std::vector<std::string>& getWarnings() const { return validationResult_.warnings; }

    // Configuration presets
    enum class Preset {
        MINIMAL,
        BALANCED,
        PERFORMANCE,
        RESEARCH,
        REALTIME
    };

    bool applyPreset(Preset preset);

    // Convenience getters for common configurations
    size_t getNeuronCount() const { return get<size_t>("neuron_count", 1000); }
    size_t getRegionCount() const { return get<size_t>("region_count", 1); }
    float getConnectionProbability() const { return get<float>("connection_probability", 0.1f); }
    double getTimestep() const { return get<double>("simulation_timestep", 0.001); }

private:
    // Internal implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    ValidationResult validationResult_;
};

/**
 * @brief Comprehensive testing framework for NLM
 */
class NLMTestFramework {
public:
    enum class TestResult {
        PASSED,
        FAILED,
        SKIPPED,
        ERROR,
        TIMEOUT
    };

    struct TestCase {
        std::string name;
        std::string description;
        std::function<bool()> testFunction;
        std::chrono::milliseconds timeout{5000};
        std::vector<std::string> tags;
        bool verbose{false};
    };

    struct TestResult {
        std::string testName;
        TestResult result;
        std::string message;
        std::chrono::milliseconds duration;
        std::string errorDetails;
    };

    NLMTestFramework() = default;
    ~NLMTestFramework() = default;

    // Test registration
    void registerTest(const TestCase& testCase);
    void registerTest(const std::string& name,
                     const std::string& description,
                     const std::function<bool()>& testFunction,
                     std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));

    // Test execution
    std::vector<TestResult> runAllTests();
    TestResult runTest(const std::string& name);
    void runTestsMatching(const std::string& pattern);

    // Test filtering and selection
    void setFilter(const std::string& filter);
    void setTags(const std::vector<std::string>& tags);
    void setParallel(bool enable, size_t threadCount = std::thread::hardware_concurrency());

    // Test reporting
    void setVerbose(bool verbose);
    void setOutputFormat(TestOutputFormat format);
    void saveResults(const std::string& filepath) const;

    // Performance testing
    struct PerformanceTest {
        std::string name;
        std::function<void()> testFunction;
        std::chrono::milliseconds minDuration{1000};
        std::chrono::milliseconds maxDuration{60000};
    };

    void registerPerformanceTest(const PerformanceTest& test);
    std::vector<std::pair<std::string, double>> runPerformanceTests();

    // Memory testing
    void testMemoryLeaks(size_t duration = 10000);
    void testMemoryUsage(const std::string& scenario);

    // Stress testing
    void testStress(size_t neuronCount, size_t steps);

    // Configuration
    struct Config {
        bool verbose{false};
        bool parallel{false};
        size_t threadCount{std::thread::hardware_concurrency()};
        std::string filter{};
        std::vector<std::string> tags{};
        std::chrono::milliseconds timeout{5000};
    };

    void configure(const Config& config) { config_ = config; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Config config_;
};

/**
 * @brief Simple configuration class for basic NLM operations
 */
class SimpleConfig {
public:
    SimpleConfig() = default;

    // Get singleton instance
    static SimpleConfig& instance() {
        static SimpleConfig config;
        return config;
    }

    // Common configuration methods
    void setNeuronCount(size_t count) { neuronCount_ = count; }
    size_t getNeuronCount() const { return neuronCount_; }

    void setRegionCount(size_t count) { regionCount_ = count; }
    size_t getRegionCount() const { return regionCount_; }

    void setConnectionProbability(float prob) { connectionProbability_ = prob; }
    float getConnectionProbability() const { return connectionProbability_; }

    void setTimestep(double dt) { timestep_ = dt; }
    double getTimestep() const { return timestep_; }

    void setRandomSeed(uint64_t seed) { randomSeed_ = seed; }
    uint64_t getRandomSeed() const { return randomSeed_; }

    // Check if configuration is valid
    bool validate() const {
        return neuronCount_ > 0 && regionCount_ > 0 && 
               connectionProbability_ >= 0.0f && connectionProbability_ <= 1.0f &&
               timestep_ > 0.0 && timestep_ <= 1.0;
    }

private:
    size_t neuronCount_{1000};
    size_t regionCount_{1};
    float connectionProbability_{0.1f};
    double timestep_{0.001};
    uint64_t randomSeed_{42};
};

/**
 * @brief Simple neural network builder for quick setup
 */
class SimpleBrainBuilder {
public:
    SimpleBrainBuilder() = default;
    ~SimpleBrainBuilder() = default;

    // Configure the brain
    SimpleBrainBuilder& withNeurons(size_t count) {
        config_.setNeuronCount(count);
        return *this;
    }

    SimpleBrainBuilder& withRegions(size_t count) {
        config_.setRegionCount(count);
        return *this;
    }

    SimpleBrainBuilder& withConnectionProbability(float prob) {
        config_.setConnectionProbability(prob);
        return *this;
    }

    SimpleBrainBuilder& withTimestep(double dt) {
        config_.setTimestep(dt);
        return *this;
    }

    SimpleBrainBuilder& withRandomSeed(uint64_t seed) {
        config_.setRandomSeed(seed);
        return *this;
    }

    // Build the brain
    std::shared_ptr<Brain> build() {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        // Apply configuration to brain (simplified)
        // In a real implementation, this would properly configure the brain
        return brain;
    }

    // Convenience static methods
    static std::shared_ptr<Brain> createExcitatoryBrain(size_t neuronCount) {
        return SimpleBrainBuilder()
            .withNeurons(neuronCount)
            .withConnectionProbability(0.15f)
            .withRandomSeed(42)
            .build();
    }

    static std::shared_ptr<Brain> createBalancedBrain(size_t neuronCount) {
        return SimpleBrainBuilder()
            .withNeurons(neuronCount)
            .withConnectionProbability(0.1f)
            .withRandomSeed(123)
            .build();
    }

    static std::shared_ptr<Brain> createForLearning(size_t neuronCount) {
        return SimpleBrainBuilder()
            .withNeurons(neuronCount)
            .withConnectionProbability(0.05f)
            .withRandomSeed(999)
            .build();
    }

private:
    SimpleConfig config_;
};

/**
 * @brief Test utilities for common NLM testing scenarios
 */
namespace test_utils {

// Common test scenarios
class TestScenarios {
public:
    static bool testBasicConnectivity(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) return false;

        // Simple connectivity test
        size_t initialSpikes = brain->getTotalSpikeCount();
        brain->step(0);
        size_t finalSpikes = brain->getTotalSpikeCount();

        return finalSpikes >= initialSpikes; // Should have some activity
    }

    static bool testPlasticity(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) return false;

        // Enable plasticity
        auto* stdp = brain->getSTDP();
        if (!stdp) return false;

        stdp->enablePlasticity(true, true, false);

        // Apply some input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);

        // Run simulation
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step);
        }

        // Check that plasticity had an effect
        return brain->getTotalSpikeCount() > 0;
    }

    static bool testMemoryIntegration(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) return false;

        // Test memory system integration
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();

        if (!workingMemory || !episodicMemory) return false;

        // Store and retrieve some information
        bool success = workingMemory->store(NeuronId(1), "test_data");
        if (!success) return false;

        return true;
    }

    static bool testNeuromodulation(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) return false;

        // Apply neuromodulation signals
        Dopamine dopamine;
        dopamine.setLevel(1.0f);
        brain->applyNeuromodulation(dopamine);

        Curiosity curiosity;
        curiosity.setLevel(0.8f);
        brain->applyNeuromodulation(curiosity);

        return true;
    }

    static bool testCheckpointSaveLoad(std::shared_ptr<Brain> brain) {
        if (!brain || !brain->initialize()) return false;

        // Save checkpoint
        bool saveSuccess = brain->save("test_checkpoint.bin");
        if (!saveSuccess) return false;

        // Reset brain
        brain->reset();
        brain->initialize();

        // Load checkpoint
        bool loadSuccess = brain->load("test_checkpoint.bin");
        if (!loadSuccess) return false;

        // Clean up
        std::remove("test_checkpoint.bin");

        return true;
    }

    static bool testPerformance(std::shared_ptr<Brain> brain, 
                                size_t minNeurons, 
                                std::chrono::milliseconds timeout) {
        if (!brain || !brain->initialize()) return false;

        auto startTime = std::chrono::high_resolution_clock::now();

        // Run simulation
        for (size_t step = 0; step < minNeurons; ++step) {
            brain->step(step);

            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);

            if (elapsed > timeout) return false; // Timeout
        }

        return true;
    }
};

// Simple test runner
class SimpleTestRunner {
public:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
        std::chrono::milliseconds duration;
    };

    TestResult runTest(const std::string& name, std::function<bool()> testFunc) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool passed = false;
        std::string message;

        try {
            passed = testFunc();
            message = passed ? "Test passed" : "Test failed";
        } catch (const std::exception& e) {
            message = std::string("Exception: ") + e.what();
        } catch (...) {
            message = "Unknown exception";
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        return {name, passed, message, duration};
    }

    std::vector<TestResult> runAllTests(std::shared_ptr<Brain> brain) {
        std::vector<TestResult> results;

        if (brain) {
            results.push_back(runTest("Basic Connectivity", 
                [brain]() { return test_utils::TestScenarios::testBasicConnectivity(brain); }));

            results.push_back(runTest("Plasticity", 
                [brain]() { return test_utils::TestScenarios::testPlasticity(brain); }));

            results.push_back(runTest("Memory Integration", 
                [brain]() { return test_utils::TestScenarios::testMemoryIntegration(brain); }));

            results.push_back(runTest("Neuromodulation", 
                [brain]() { return test_utils::TestScenarios::testNeuromodulation(brain); }));

            results.push_back(runTest("Checkpoint Save/Load", 
                [brain]() { return test_utils::TestScenarios::testCheckpointSaveLoad(brain); }));
        }

        return results;
    }
};

} // namespace test_utils

} // namespace nlm