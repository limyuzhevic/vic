// Test Framework Header
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>

// Forward declarations
namespace nlm {
    class Brain;
    class Config;
}

// Test Framework Classes

class TestConfig {
public:
    // Test selection
    bool runConnectivityTest = true;
    bool runPlasticityTest = true;
    bool runStdpTest = true;
    
    // Connectivity test parameters
    size_t connectivityNeuronCount = 10;
    float connectivityCurrent = 50.0f;
    size_t connectivitySteps = 50;
    
    // Plasticity test parameters
    size_t plasticitySteps = 1000;
    size_t plasticityNeuronCount = 20;
    float plasticityCurrent = 30.0f;
    float plasticityWeightThreshold = 0.01f;
    float plasticityLearningThreshold = 0.001f;
    
    // STDP test parameters
    size_t stdpTrials = 50;
    size_t stdpSynapseCount = 5;
    float stdpCurrent = 60.0f;
    float stdpDeltaThreshold = 0.001f;
    
    // Common parameters
    uint64_t seed = 42;
    double timestep = 0.001;
    
    // Test execution control
    bool resetBetweenTests = true;
    bool logDetailedResults = false;
    bool enableProgressReporting = true;
    
    // Validation
    bool validate() const {
        bool valid = true;
        if (connectivityNeuronCount == 0) {
            std::cerr << "Error: connectivityNeuronCount must be > 0" << std::endl;
            valid = false;
        }
        if (connectivityCurrent <= 0.0f) {
            std::cerr << "Error: connectivityCurrent must be > 0" << std::endl;
            valid = false;
        }
        if (plasticitySteps == 0) {
            std::cerr << "Error: plasticitySteps must be > 0" << std::endl;
            valid = false;
        }
        if (stdpTrials == 0) {
            std::cerr << "Error: stdpTrials must be > 0" << std::endl;
            valid = false;
        }
        return valid;
    }
};

class TestResult {
public:
    std::string testName;
    bool passed;
    std::string message;
    double executionTime;
    std::vector<std::string> details;
    
    TestResult(const std::string& name, bool success, const std::string& msg = "")
        : testName(name), passed(success), message(msg), executionTime(0.0) {}
    
    void addDetail(const std::string& detail) {
        details.push_back(detail);
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Test: " << testName << " - " << (passed ? "PASSED" : "FAILED");
        if (!message.empty()) {
            oss << " (" << message << ")";
        }
        if (executionTime > 0) {
            oss << " [" << std::fixed << std::setprecision(3) << executionTime << "s]";
        }
        return oss.str();
    }
};

class TestBase {
public:
    virtual ~TestBase() = default;
    
    virtual std::string getName() const = 0;
    virtual TestConfig getDefaultConfig() const = 0;
    virtual bool run(std::shared_ptr<nlm::Brain> brain, const TestConfig& config) = 0;
    
    TestResult execute(std::shared_ptr<nlm::Brain> brain, const TestConfig& config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::string testName = getName();
        
        try {
            bool result = run(brain, config);
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            TestResult testResult(testName, result);
            testResult.executionTime = duration;
            testResult.addDetail("Test completed successfully");
            if (config.logDetailedResults) {
                testResult.addDetail("Configuration used for test execution");
            }
            
            return testResult;
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            TestResult testResult(testName, false, std::string("Exception: ") + e.what());
            testResult.executionTime = duration;
            return testResult;
        }
    }
};

class TestRunner {
public:
    TestRunner(std::shared_ptr<nlm::Config> config)
        : config_(config) {}
    
    void addTest(std::shared_ptr<TestBase> test) {
        tests_.push_back(test);
    }
    
    std::vector<std::shared_ptr<TestBase>> getAvailableTests() const {
        return tests_;
    }
    
    std::vector<TestResult> runTests(const std::string& testNames, const TestConfig& testConfig) {
        std::vector<TestResult> results;
        
        // Parse test names (comma-separated)
        std::vector<std::string> selectedTests;
        size_t start = 0;
        size_t comma = testNames.find(',');
        
        if (comma == std::string::npos) {
            selectedTests.push_back(testNames);
        } else {
            while (comma != std::string::npos) {
                selectedTests.push_back(testNames.substr(start, comma - start));
                start = comma + 1;
                comma = testNames.find(',', start);
                if (comma == std::string::npos) {
                    selectedTests.push_back(testNames.substr(start));
                }
            }
        }
        
        for (const auto& testName : selectedTests) {
            auto test = findTest(testName);
            if (test) {
                if (config_->has("brain.reset_between_tests")) {
                    testConfig_.resetBetweenTests = 
                        config_->getOr<bool>("brain.reset_between_tests", true);
                }
                if (config_->has("brain.log_detailed_results")) {
                    testConfig_.logDetailedResults = 
                        config_->getOr<bool>("brain.log_detailed_results", false);
                }
                if (config_->has("brain.enable_progress_reporting")) {
                    testConfig_.enableProgressReporting = 
                        config_->getOr<bool>("brain.enable_progress_reporting", true);
                }
                
                results.push_back(runSingleTest(test, testConfig));
            } else {
                TestResult failedTest(testName, false, "Test not found");
                results.push_back(failedTest);
            }
        }
        
        return results;
    }
    
    TestResult runSingleTest(const std::shared_ptr<TestBase>& test, const TestConfig& testConfig) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain with test-specific configuration
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("random_seed", testConfig.seed, nlm::ConfigSource::Runtime);
        brainConfig->set("simulation_timestep", testConfig.timestep, nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            TestResult result(test->getName(), false, "Failed to initialize brain");
            result.executionTime = duration;
            return result;
        }
        
        // Run the test
        auto result = test->execute(brain, testConfig);
        
        // Reset brain if configured
        if (testConfig.resetBetweenTests) {
            brain->reset();
            brain->initialize();
        }
        
        return result;
    }
    
    std::shared_ptr<TestBase> findTest(const std::string& name) {
        for (const auto& test : tests_) {
            if (test->getName() == name) {
                return test;
            }
        }
        return nullptr;
    }
    
    std::string getHelpText() const {
        std::ostringstream oss;
        oss << "Available tests:" << std::endl;
        for (const auto& test : tests_) {
            oss << "  - " << test->getName() << std::endl;
        }
        return oss.str();
    }
    
private:
    std::vector<std::shared_ptr<TestBase>> tests_;
    std::shared_ptr<nlm::Config> config_;
    TestConfig testConfig_;
};
