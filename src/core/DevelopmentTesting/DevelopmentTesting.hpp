// Development and testing utilities for NLM
// Provides comprehensive testing framework, debugging tools, and development support

#ifndef NLM_DEVELOPMENT_TESTING_HPP
#define NLM_DEVELOPMENT_TESTING_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <random>
#include "../core/Config/Config.hpp"
#include "../core/CommandSystem/CommandSystem.hpp"

namespace nlm {

// Test result structure
struct TestResult {
    std::string testName;
    std::string testSuite;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool passed;
    std::string message;
    std::string errorType;
    std::vector<std::string> stackTrace;
    std::unordered_map<std::string, std::string> metadata;
    
    TestResult(const std::string& name, const std::string& suite)
        : testName(name), testSuite(suite), passed(false) {}
    
    double getDuration() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return static_cast<double>(duration.count()) / 1000.0;
    }
    
    std::string getFormattedDuration() const {
        double seconds = getDuration();
        std::ostringstream oss;
        if (seconds >= 1.0) {
            oss << std::fixed << std::setprecision(2) << seconds << "s";
        } else {
            oss << std::fixed << std::setprecision(3) << seconds * 1000 << "ms";
        }
        return oss.str();
    }
};

// Test case interface
class ITestCase {
public:
    virtual ~ITestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getSuite() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TestResult run() = 0;
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
    virtual std::vector<std::string> getDependencies() const { return {}; }
    virtual double getTimeout() const { return 60.0; } // seconds
};

// Test suite interface
class ITestSuite {
public:
    virtual ~ITestSuite() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::unique_ptr<ITestCase>> getTestCases() const = 0;
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
};

// Test runner
class TestRunner {
public:
    TestRunner() = default;
    
    void runTest(std::unique_ptr<ITestCase> testCase) {
        if (!testCase) return;
        
        std::cout << "Running test: " << testCase->getName() << " (" << testCase->getSuite() << ")" << std::endl;
        
        // Setup
        if (!testCase->setup()) {
            std::cout << "  Setup failed" << std::endl;
            return;
        }
        
        // Run test
        TestResult result = testCase->run();
        
        // Teardown
        testCase->teardown();
        
        // Report result
        reportTestResult(result);
    }
    
    void runAllTests(const std::vector<std::unique_ptr<ITestSuite>>& suites) {
        std::cout << "=== NLM Test Suite ===" << std::endl;
        std::cout << "Running " << suites.size() << " test suites..." << std::endl << std::endl;
        
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& suite : suites) {
            std::cout << "Suite: " << suite->getName() << std::endl;
            std::cout << "Description: " << suite->getDescription() << std::endl;
            
            auto testCases = suite->getTestCases();
            std::cout << "Tests: " << testCases.size() << std::endl;
            
            for (const auto& testCase : testCases) {
                runTest(std::move(testCase));
                totalTests++;
            }
            
            std::cout << std::endl;
        }
        
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
        std::cout << "Total duration: " << totalDuration << " seconds" << std::endl;
    }
    
protected:
    virtual void reportTestResult(const TestResult& result) {
        std::string status = result.passed ? "PASS" : "FAIL";
        std::cout << "  " << status << ": " << result.testName << " (" << result.getFormattedDuration() << ")" << std::endl;
        
        if (!result.passed) {
            std::cout << "    Error: " << result.message << std::endl;
            if (!result.errorType.empty()) {
                std::cout << "    Type: " << result.errorType << std::endl;
            }
        }
    }
};

// Test data generator
class TestDataGenerator {
public:
    TestDataGenerator() {
        std::random_device rd;
        rng_.seed(rd());
    }
    
    // Generate random brain configuration
    std::shared_ptr<Config> generateBrainConfig(size_t neuronCount = 0, size_t regionCount = 0) {
        auto config = std::make_shared<Config>();
        
        if (neuronCount == 0) {
            neuronCount = std::uniform_int_distribution<size_t>(500, 1000)(rng_);
        }
        
        if (regionCount == 0) {
            regionCount = std::uniform_int_distribution<size_t>(1, 3)(rng_);
        }
        
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("simulation_timestep", 0.001);
        config->set("connection_probability", 0.1f + std::uniform_real_distribution<float>(0.0f, 0.2f)(rng_));
        
        return config;
    }
    
    // Generate random neural weights
    std::vector<float> generateRandomWeights(size_t count) {
        std::vector<float> weights;
        weights.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            weights.push_back(std::normal_distribution<float>(0.2f, 0.1f)(rng_));
        }
        
        return weights;
    }
    
    // Generate random spike times
    std::vector<double> generateSpikeTimes(size_t count, double minTime = 0.0, double maxTime = 100.0) {
        std::vector<double> spikeTimes;
        spikeTimes.reserve(count);
        
        std::uniform_real_distribution<double> dist(minTime, maxTime);
        
        for (size_t i = 0; i < count; ++i) {
            spikeTimes.push_back(dist(rng_));
        }
        
        std::sort(spikeTimes.begin(), spikeTimes.end());
        return spikeTimes;
    }
    
    // Generate random neuron connections
    std::vector<std::pair<size_t, size_t>> generateRandomConnections(size_t neuronCount, float connectionProbability) {
        std::vector<std::pair<size_t, size_t>> connections;
        
        for (size_t i = 0; i < neuronCount; ++i) {
            for (size_t j = 0; j < neuronCount; ++j) {
                if (i != j && std::bernoulli_distribution(connectionProbability)(rng_)) {
                    connections.push_back({i, j});
                }
            }
        }
        
        return connections;
    }
    
private:
    std::mt19937 rng_;
};

// Test report generator
class TestReportGenerator {
public:
    std::string generateReport(const std::vector<TestResult>& results) {
        std::ostringstream oss;
        oss << "=== NLM Test Report ===" << std::endl;
        oss << "Generated: " << getCurrentTimestamp() << std::endl;
        oss << std::endl;
        
        if (results.empty()) {
            oss << "No test results available" << std::endl;
            return oss.str();
        }
        
        // Summary statistics
        oss << "=== Test Summary ===" << std::endl;
        size_t totalTests = results.size();
        size_t passedTests = 0;
        size_t failedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
            totalDuration += result.getDuration();
        }
        
        oss << "Total tests: " << totalTests << std::endl;
        oss << "Passed: " << passedTests << std::endl;
        oss << "Failed: " << failedTests << std::endl;
        oss << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
        oss << "Total duration: " << totalDuration << " seconds" << std::endl;
        oss << std::endl;
        
        // Detailed results
        oss << "=== Detailed Results ===" << std::endl;
        for (const auto& result : results) {
            oss << result.testName << ": " << (result.passed ? "PASS" : "FAIL") << " (" << result.getFormattedDuration() << ")" << std::endl;
            
            if (!result.passed) {
                oss << "  Error: " << result.message << std::endl;
                if (!result.errorType.empty()) {
                    oss << "  Type: " << result.errorType << std::endl;
                }
            }
        }
        
        return oss.str();
    }
    
    // Export test results to file
    bool exportResults(const std::vector<TestResult>& results, const std::string& filename) {
        try {
            if (!std::filesystem::exists(std::filesystem::path(filename).parent_path())) {
                std::filesystem::create_directories(std::filesystem::path(filename).parent_path());
            }
            
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "test_name,test_suite,passed,duration_seconds,message,error_type" << std::endl;
            
            for (const auto& result : results) {
                // Escape quotes in message
                std::string message = result.message;
                std::replace(message.begin(), message.end(), '"', '""');
                std::replace(message.begin(), message.end(), ',', " ");
                
                std::string errorType = result.errorType;
                std::replace(errorType.begin(), errorType.end(), '"', '""');
                std::replace(errorType.begin(), errorType.end(), ',', " ");
                
                file << result.testName << "," << result.testSuite << "," 
                     << (result.passed ? "true" : "false") << "," << result.getDuration() << "," 
                     << "\"" << message << "\",\"" << errorType << "\"" << std::endl;
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

// Development utilities
class DevelopmentUtilities {
public:
    // Create a test environment
    static std::unique_ptr<Config> createTestEnvironment(const std::string& name) {
        auto config = std::make_unique<Config>();
        
        config->set("test_mode", true);
        config->set("test_name", name);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.2f);
        
        // Test-specific settings
        config->set("enable_memory_monitoring", true);
        config->set("enable_performance_monitoring", true);
        config->set("enable_logging", true);
        config->set("log_level", "info");
        
        return config;
    }
    
    // Generate test configuration
    static std::string generateTestConfiguration(const std::string& basePath, const std::string& testName) {
        std::ostringstream oss;
        oss << "# Test Configuration for " << testName << std::endl;
        oss << "# Generated by NLM Development Utilities" << std::endl;
        oss << "# " << std::endl;
        oss << "neuron_count = 500" << std::endl;
        oss << "region_count = 1" << std::endl;
        oss << "simulation_timestep = 0.001" << std::endl;
        oss << "connection_probability = 0.1" << std::endl;
        oss << "test_mode = true" << std::endl;
        oss << "test_name = \"" << testName << "\"" << std::endl;
        oss << "random_seed = 42" << std::endl;
        oss << std::endl;
        
        std::string configPath = basePath + "/" + testName + ".cfg";
        std::ofstream file(configPath);
        if (file.is_open()) {
            file << oss.str();
            file.close();
            return configPath;
        }
        
        return "";
    }
    
    // Validate test configuration
    static bool validateTestConfiguration(const Config& config) {
        // Check required test configuration keys
        std::vector<std::string> requiredKeys = {
            "test_mode",
            "random_seed",
            "neuron_count",
            "region_count"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                return false;
            }
        }
        
        // Validate values
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (!neuronCount || *neuronCount <= 0) {
            return false;
        }
        
        auto regionCount = config.get<int64_t>("region_count");
        if (!regionCount || *regionCount <= 0) {
            return false;
        }
        
        auto connectionProbability = config.get<float>("connection_probability");
        if (!connectionProbability || *connectionProbability < 0.0f || *connectionProbability > 1.0f) {
            return false;
        }
        
        return true;
    }
};

// Development and testing system
class DevelopmentTestingSystem {
public:
    DevelopmentTestingSystem() = default;
    
    void registerTestSuite(std::unique_ptr<ITestSuite> suite) {
        if (suite) {
            testSuites_.push_back(std::move(suite));
        }
    }
    
    void runAllTests() {
        TestRunner runner;
        runner.runAllTests(testSuites_);
    }
    
    void saveTestReport(const std::string& filename) {
        TestReportGenerator reportGenerator;
        std::vector<TestResult> allResults;
        
        // Collect all test results (in a real implementation, this would be stored during test execution)
        // For now, return empty report
        reportGenerator.exportResults(allResults, filename);
    }
    
    TestDataGenerator& getDataGenerator() {
        return dataGenerator_;
    }
    
    DevelopmentUtilities& getUtilities() {
        return utilities_;
    }
    
private:
    std::vector<std::unique_ptr<ITestSuite>> testSuites_;
    TestDataGenerator dataGenerator_;
    DevelopmentUtilities utilities_;
};

} // namespace nlm

#endif // NLM_DEVELOPMENT_TESTING_HPP
