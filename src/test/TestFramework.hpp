#include "TestRunner.hpp"

namespace nlm::test {

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    double durationSeconds;
    std::string message;
    size_t stepCount;
    float finalEir;
    size_t finalSpikes;
};

// Test configuration
struct TestConfig {
    std::string name;
    std::string description;
    uint64_t seed;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    double timestep;
    std::map<std::string, std::string> additionalConfig;
};

// Abstract base test class
class TestBase {
public:
    virtual ~TestBase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TestConfig getDefaultConfig() const = 0;
    
    virtual TestResult run(std::shared_ptr<Brain> brain, const TestConfig& config) = 0;
    virtual bool setup(std::shared_ptr<Brain> brain, const TestConfig& config) = 0;
    virtual void cleanup(std::shared_ptr<Brain> brain) = 0;
    
    virtual bool supportsParallelExecution() const { return false; }
    virtual size_t getExpectedStepCount() const { return 0; }
};

// Test runner
class TestRunner {
public:
    TestRunner() : logLevel_(LogLevel::Info) {}
    
    void setLogLevel(LogLevel level) { logLevel_ = level; }
    LogLevel getLogLevel() const { return logLevel_; }
    
    void addTest(std::unique_ptr<TestBase> test) {
        tests_.push_back(std::move(test));
    }
    
    std::vector<TestResult> runAllTests(const TestConfig& baseConfig) {
        std::vector<TestResult> results;
        
        NLM_LOG_INFO("=== NLM Phase 6 Test Suite ===");
        NLM_LOG_INFO("Running " + std::to_string(tests_.size()) + " test(s)...");
        
        for (auto& test : tests_) {
            NLM_LOG_INFO("\n--- Running Test: " + test->getName() + " ---");
            NLM_LOG_INFO(test->getDescription());
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            TestConfig config = test->getDefaultConfig();
            // Merge with base config
            config.seed = baseConfig.seed;
            config.neuronCount = baseConfig.neuronCount;
            config.regionCount = baseConfig.regionCount;
            config.connectionProbability = baseConfig.connectionProbability;
            config.timestep = baseConfig.timestep;
            
            for (const auto& pair : baseConfig.additionalConfig) {
                config.additionalConfig[pair.first] = pair.second;
            }
            
            // Create fresh brain for each test
            auto brain = createTestBrain(config);
            
            TestResult result;
            try {
                if (test->setup(brain, config)) {
                    result = test->run(brain, config);
                } else {
                    result.name = test->getName();
                    result.passed = false;
                    result.message = "Test setup failed";
                }
            } catch (const std::exception& e) {
                result.name = test->getName();
                result.passed = false;
                result.message = "Exception: " + std::string(e.what());
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.durationSeconds = 
                std::chrono::duration<double>(endTime - startTime).count();
            
            test->cleanup(brain);
            
            results.push_back(result);
            
            // Log test result
            if (result.passed) {
                NLM_LOG_INFO("✓ PASS: " + result.name + " (" + 
                           std::to_string(result.durationSeconds) + "s)");
                if (!result.message.empty()) {
                    NLM_LOG_INFO("  Message: " + result.message);
                }
            } else {
                NLM_LOG_ERROR("✗ FAIL: " + result.name + " - " + result.message);
            }
        }
        
        return results;
    }
    
    void printSummary(const std::vector<TestResult>& results) {
        NLM_LOG_INFO("\n=== Test Summary ===");
        
        size_t passed = 0;
        size_t failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
            totalTime += result.durationSeconds;
        }
        
        NLM_LOG_INFO("Total tests: " + std::to_string(results.size()));
        NLM_LOG_INFO("Passed: " + std::to_string(passed));
        NLM_LOG_INFO("Failed: " + std::to_string(failed));
        NLM_LOG_INFO("Total time: " + std::to_string(totalTime) + "s");
        
        if (failed > 0) {
            NLM_LOG_ERROR("Tests failed! Cannot proceed with Phase 6 validation.");
        } else {
            NLM_LOG_INFO("All tests passed. Phase 6 integration complete.");
        }
    }
    
    std::shared_ptr<Brain> createTestBrain(const TestConfig& config) {
        auto configPtr = std::make_shared<Config>();
        
        // Set basic configuration
        configPtr->set("random_seed", static_cast<int64_t>(config.seed), ConfigSource::Default);
        configPtr->set("simulation_timestep", config.timestep, ConfigSource::Default);
        configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        configPtr->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
        configPtr->set("connection_probability", config.connectionProbability, ConfigSource::Default);
        
        // Set STDP parameters for consistent testing
        configPtr->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        configPtr->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        configPtr->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Set structural plasticity parameters
        configPtr->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        configPtr->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Add additional config
        for (const auto& pair : config.additionalConfig) {
            if (pair.first == "stdp_ltp_weight") {
                configPtr->set(pair.first, std::stof(pair.second), ConfigSource::Default);
            } else if (pair.first == "stdp_ltd_weight") {
                configPtr->set(pair.first, std::stof(pair.second), ConfigSource::Default);
            } else if (pair.first == "stdp_tau") {
                configPtr->set(pair.first, std::stof(pair.second), ConfigSource::Default);
            } else {
                configPtr->set(pair.first, pair.second, ConfigSource::Default);
            }
        }
        
        auto brain = std::make_shared<Brain>(configPtr);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for testing");
        }
        
        return brain;
    }

private:
    std::vector<std::unique_ptr<TestBase>> tests_;
    LogLevel logLevel_;
};

} // namespace nlm::test
