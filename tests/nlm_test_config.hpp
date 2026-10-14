// Main Enhanced Test Configuration
// Configuration for enhanced testing framework

#ifndef NLM_TEST_CONFIG_H
#define NLM_TEST_CONFIG_H

#include <string>
#include <vector>
#include <map>

namespace nlm {
    // Test configuration structure
    struct TestConfig {
        std::string name;
        std::string category;
        std::string description;
        bool enabled;
        int timeout; // in seconds
        std::map<std::string, std::string> parameters;
        
        TestConfig(const std::string& n, const std::string& c, const std::string& d = "")
            : name(n), category(c), description(d), enabled(true), timeout(30) {}
    };
    
    // Enhanced test result structure
    struct TestResult {
        std::string testName;
        std::string category;
        std::string status; // PASSED, FAILED, SKIPPED, ERROR
        std::string message;
        double executionTime; // in seconds
        std::map<std::string, std::string> metadata;
        
        TestResult(const std::string& name, const std::string& cat)
            : testName(name), category(cat), status("UNKNOWN"), 
              executionTime(0.0) {}
    };
    
    // Test suite configuration
    struct TestSuiteConfig {
        std::string suiteName;
        std::string category;
        std::string description;
        std::vector<TestConfig> testCases;
        int parallelThreads;
        bool requireGPU;
        double memoryLimit; // in GB
        
        TestSuiteConfig(const std::string& name, const std::string& cat)
            : suiteName(name), category(cat), parallelThreads(1), requireGPU(false), memoryLimit(2.0) {}
    };
    
    // Performance benchmark configuration
    struct BenchmarkConfig {
        std::string benchmarkName;
        double targetExecutionTime; // in seconds
        double memoryLimit; // in MB
        int iterationCount;
        std::string performanceMetric;
        double acceptableVariance; // percentage
        
        BenchmarkConfig(const std::string& name)
            : benchmarkName(name), targetExecutionTime(1.0), memoryLimit(1024.0),
              iterationCount(1000), acceptableVariance(10.0) {}
    };
    
    // Test environment configuration
    struct TestEnvironment {
        std::string name;
        std::string version;
        std::map<std::string, std::string> settings;
        std::vector<std::string> enabledComponents;
        std::vector<std::string> disabledComponents;
        
        TestEnvironment(const std::string& n, const std::string& v) : name(n), version(v) {}
        
        bool isComponentEnabled(const std::string& component) const {
            return std::find(enabledComponents.begin(), enabledComponents.end(), component) != enabledComponents.end();
        }
        
        bool isComponentDisabled(const std::string& component) const {
            return std::find(disabledComponents.begin(), disabledComponents.end(), component) != disabledComponents.end();
        }
    };
}

#endif // NLM_TEST_CONFIG_H