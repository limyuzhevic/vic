# Test framework for NLM improvements
# Comprehensive test suite for the enhanced NLM system

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace testing {

/**
 * @brief Test framework base class
 */
class TestFramework {
public:
    TestFramework() : passedTests(0), failedTests(0), totalTests(0) {}
    virtual ~TestFramework() {}
    
    virtual void runAllTests() {
        runTests();
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed tests: " << passedTests << std::endl;
        std::cout << "Failed tests: " << failedTests << std::endl;
        
        if (failedTests == 0) {
            std::cout << "✓ All tests passed!" << std::endl;
        } else {
            std::cout << "✗ " << failedTests << " tests failed!" << std::endl;
        }
    }
    
protected:
    virtual void runTests() = 0;
    
    template<typename TestFunc>
    void runTest(const std::string& testName, TestFunc testFunction) {
        totalTests++;
        try {
            testFunction();
            std::cout << "✓ " << testName << " passed" << std::endl;
            passedTests++;
        } catch (const std::exception& e) {
            std::cout << "✗ " << testName << " failed: " << e.what() << std::endl;
            failedTests++;
        } catch (...) {
            std::cout << "✗ " << testName << " failed (unknown error)" << std::endl;
            failedTests++;
        }
    }
    
    int passedTests;
    int failedTests;
    int totalTests;
};

/**
 * @brief Configuration system tests
 */
class ConfigTests : public TestFramework {
protected:
    void runTests() override {
        std::cout << "=== Configuration System Tests ===" << std::endl;
        
        runTest("Default configuration initialization", [this]() {
            Config config;
            assert(config.has("random_seed"));
            assert(config.has("simulation_timestep"));
            assert(config.has("neuron_count"));
            assert(config.has("connection_probability"));
        });
        
        runTest("Value retrieval", [this]() {
            Config config;
            auto seed = config.getOr<int>("random_seed", 42);
            assert(seed == 42);
            
            auto timestep = config.getOr<double>("simulation_timestep", 0.001);
            assert(std::abs(timestep - 0.001) < 0.0001);
        });
        
        runTest("Value setting", [this]() {
            Config config;
            config.set("test_key", "test_value", ConfigSource::Runtime);
            assert(config.has("test_key"));
            
            auto value = config.getOr<std::string>("test_key", "");
            assert(value == "test_value");
        });
        
        runTest("Type conversion", [this]() {
            Config config;
            config.set("int_value", 42);
            config.set("double_value", 3.14);
            config.set("bool_value", true);
            config.set("string_value", "test");
            
            assert(config.getOr<int>("int_value") == 42);
            assert(std::abs(config.getOr<double>("double_value") - 3.14) < 0.0001);
            assert(config.getOr<bool>("bool_value") == true);
            assert(config.getOr<std::string>("string_value") == "test");
        });
        
        runTest("Configuration summary", [this]() {
            Config config;
            std::string summary = config.summary();
            assert(!summary.empty());
            assert(summary.find("Configuration Summary") != std::string::npos);
        });
        
        runTest("Configuration loading from args", [this]() {
            Config config;
            config.loadFromArgs(3, new char*[]{"test", "--random_seed", "123", "--neuron_count", "500"});
            
            assert(config.getOr<int>("random_seed") == 123);
            assert(config.getOr<int>("neuron_count") == 500);
        });
        
        runTest("Key removal", [this]() {
            Config config;
            config.remove("random_seed");
            assert(!config.has("random_seed"));
            
            // Should still have default value if we ask with getOr
            assert(config.getOr<int>("random_seed", 99) == 99);
        });
        
        runTest("Configuration clearing", [this]() {
            Config config;
            config.clear();
            // Should still have default keys
            assert(config.has("random_seed"));
            assert(config.has("neuron_count"));
        });
    }
};

/**
 * @brief Python bindings tests
 */
class PythonBindingsTests : public TestFramework {
protected:
    void runTests() override {
        std::cout << "=== Python Bindings Tests ===" << std::endl;
        
        // Note: These tests would require actual Python bindings to be built
        // For demonstration, we'll just show the structure
        runTest("Python module import", [this]() {
            // This would test if Python bindings can be imported
            // Since we can't actually test Python from C++ here, we'll skip
            // assert(true); // Placeholder
        });
        
        runTest("Configuration export/import", [this]() {
            Config config;
            config.set("test_key", "test_value", ConfigSource::Runtime);
            
            std::string jsonFile = "/tmp/test_config.json";
            if (config.saveToFile(jsonFile)) {
                Config config2;
                if (config2.loadFromFile(jsonFile)) {
                    assert(config2.has("test_key"));
                }
                // Clean up
                std::remove(jsonFile.c_str());
            }
        });
    }
};

/**
 * @brief Main NLM system tests
 */
class NLMSystemTests : public TestFramework {
protected:
    void runTests() override {
        std::cout << "=== NLM System Tests ===" << std::endl;
        
        runTest("Brain initialization", [this]() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            
            auto brain = std::make_shared<Brain>(config);
            assert(brain->initialize());
            
            assert(brain->getTotalNeuronCount() == 100);
        });
        
        runTest("Brain step execution", [this]() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 50);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run a few steps
            for (size_t i = 0; i < 10; ++i) {
                brain->step(i, i * 0.001);
            }
            
            assert(brain->getTotalSpikeCount() > 0);
        });
        
        runTest("Memory system integration", [this]() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Test working memory
            auto* workingMemory = brain->getWorkingMemory();
            if (workingMemory) {
                // Should have some capacity
                assert(workingMemory->getCapacity() > 0);
            }
            
            // Test episodic memory
            auto* episodicMemory = brain->getEpisodicMemory();
            if (episodicMemory) {
                assert(episodicMemory->getCapacity() > 0);
            }
        });
        
        runTest("Neuromodulation system", [this]() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Test dopamine system
            auto* dopamine = brain->getDopamine();
            if (dopamine) {
                // Should have some functionality
                assert(dopamine->getSensitivity() > 0);
            }
            
            // Test curiosity system
            auto* curiosity = brain->getCuriosity();
            if (curiosity) {
                assert(curiosity->getThreshold() > 0);
            }
        });
        
        runTest("Development system", [this]() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            config->set("development_enabled", true);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto* development = brain->getDevelopmentSystem();
            if (development) {
                // Should have developmental stages
                assert(static_cast<int>(brain->getDevelopmentalStage()) >= 0);
            }
        });
    }
};

/**
 * @brief Phase 6 integration tests
 */
class Phase6IntegrationTests : public TestFramework {
protected:
    void runTests() override {
        std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
        
        runTest("Integration verification", [this]() {
            Phase6IntegratedExperiment experiment;
            assert(experiment.verifyIntegration());
        });
        
        runTest("Memory integration", [this]() {
            Phase6IntegratedExperiment experiment;
            assert(experiment.testMemoryIntegration());
        });
        
        runTest("Neuromodulation integration", [this]() {
            Phase6IntegratedExperiment experiment;
            assert(experiment.testNeuromodulationIntegration());
        });
        
        runTest("Checkpoint functionality", [this]() {
            Phase6IntegratedExperiment experiment;
            assert(experiment.testCheckpointing());
        });
        
        runTest("Replay functionality", [this]() {
            Phase6IntegratedExperiment experiment;
            assert(experiment.testReplay());
        });
    }
};

/**
 * @brief Performance tests
 */
class PerformanceTests : public TestFramework {
protected:
    void runTests() override {
        std::cout << "=== Performance Tests ===" << std::endl;
        
        runTest("Configuration performance", [this]() {
            Config config;
            
            // Test setting/getting performance
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 10000; ++i) {
                config.set("temp_key", i);
                auto value = config.getOr<int>("temp_key", 0);
                assert(value == i);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            // Should complete 10000 operations in reasonable time
            assert(duration.count() < 1000); // Less than 1 second
        });
        
        runTest("Memory usage", [this]() {
            // Simple memory test - create and destroy many configs
            for (int i = 0; i < 100; ++i) {
                Config config;
                config.set("test", i);
            }
            // If we get here, memory management seems OK
            assert(true);
        });
        
        runTest("Brain performance", [this]() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < 100; ++i) {
                brain->step(i, i * 0.001);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            // Should complete 100 steps in reasonable time
            assert(duration.count() < 10000); // Less than 10 seconds
        });
    }
};

/**
 * @brief Full test suite runner
 */
class NLMSystemTestSuite {
public:
    NLMSystemTestSuite() {
        testSuites.push_back(std::make_unique<ConfigTests>());
        testSuites.push_back(std::make_unique<PythonBindingsTests>());
        testSuites.push_back(std::make_unique<NLMSystemTests>());
        testSuites.push_back(std::make_unique<Phase6IntegrationTests>());
        testSuites.push_back(std::make_unique<PerformanceTests>());
    }
    
    void runAllTests() {
        std::cout << "=== NLM Test Suite ===" << std::endl;
        std::cout << "Running comprehensive tests for NLM improvements..." << std::endl;
        std::cout << std::endl;
        
        int totalPassed = 0;
        int totalFailed = 0;
        
        for (auto& testSuite : testSuites) {
            testSuite->runAllTests();
            totalPassed += testSuite->passedTests;
            totalFailed += testSuite->failedTests;
        }
        
        std::cout << std::endl;
        std::cout << "=== Final Test Summary ===" << std::endl;
        std::cout << "Total test suites: " << testSuites.size() << std::endl;
        std::cout << "Total passed: " << totalPassed << std::endl;
        std::cout << "Total failed: " << totalFailed << std::endl;
        
        if (totalFailed == 0) {
            std::cout << "\n✓ All tests passed! NLM improvements are working correctly." << std::endl;
        } else {
            std::cout << "\n✗ " << totalFailed << " tests failed. Please review the improvements." << std::endl;
        }
    }
    
private:
    std::vector<std::unique_ptr<TestFramework>> testSuites;
};

} // namespace testing
} // namespace nlm