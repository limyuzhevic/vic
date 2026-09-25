// Test parameters header
// Parameters for NLM test configuration
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {
namespace test {

// Test parameter definitions
struct TestParameter {
    std::string name;
    std::string type; // "int", "double", "string", "bool", "enum"
    std::string value;
    std::string description;
    std::vector<std::string> enumValues;
    bool required;
    double minValue;
    double maxValue;
    std::string defaultValue;
    
    TestParameter(const std::string& paramName, const std::string& paramType, 
                  const std::string& paramDescription = "",
                  bool paramRequired = false, double paramMin = 0.0, double paramMax = 0.0,
                  const std::string& paramDefault = "")
        : name(paramName), type(paramType), description(paramDescription),
          required(paramRequired), minValue(paramMin), maxValue(paramMax),
          defaultValue(paramDefault), value(paramDefault) {}
};

// Test settings configuration
class TestSettings {
public:
    struct PerformanceSettings {
        double minNeuronsPerSecond;
        double maxMemoryUsageMB;
        std::chrono::nanoseconds maxStepTime;
        bool enableMonitoring;
        bool enableBenchmarking;
        int benchmarkIterations;
    };
    
    struct StressSettings {
        int maxNetworkSize;
        double maxPlasticityRate;
        std::chrono::nanoseconds maxSimulationTime;
        bool enableConcurrentStress;
    };
    
    struct MemorySettings {
        size_t maxMemoryDeltaKB;
        bool detectLeaks;
        int allocationRounds;
        size_t sampleInterval;
    };
    
    struct ConcurrencySettings {
        int maxThreads;
        double threadContention;
        bool detectDeadlocks;
        bool testRaceConditions;
    };
    
    struct RegressionSettings {
        double regressionThreshold;
        bool compareWithBaseline;
        std::string baselineFile;
        bool generateRegressionReport;
    };
    
    struct OutputSettings {
        std::string reportFormat; // "text", "xml", "json", "csv", "html"
        std::string reportFile;
        bool verboseOutput;
        bool generateStatistics;
        bool saveIntermediateResults;
    };
    
    PerformanceSettings performance;
    StressSettings stress;
    MemorySettings memory;
    ConcurrencySettings concurrency;
    RegressionSettings regression;
    OutputSettings output;
    
    TestSettings() {
        // Default performance settings
        performance.minNeuronsPerSecond = 1000.0;
        performance.maxMemoryUsageMB = 1024.0;
        performance.maxStepTime = std::chrono::milliseconds(100);
        performance.enableMonitoring = true;
        performance.enableBenchmarking = true;
        performance.benchmarkIterations = 1000;
        
        // Default stress settings
        stress.maxNetworkSize = 5000;
        stress.maxPlasticityRate = 0.5;
        stress.maxSimulationTime = std::chrono::minutes(30);
        stress.enableConcurrentStress = true;
        
        // Default memory settings
        memory.maxMemoryDeltaKB = 1024; // 1MB
        memory.detectLeaks = true;
        memory.allocationRounds = 100;
        memory.sampleInterval = 10; // milliseconds
        
        // Default concurrency settings
        concurrency.maxThreads = 8;
        concurrency.threadContention = 0.5;
        concurrency.detectDeadlocks = true;
        concurrency.testRaceConditions = true;
        
        // Default regression settings
        regression.regressionThreshold = 1.2; // 20% degradation triggers regression
        regression.compareWithBaseline = true;
        regression.baselineFile = "baseline_results.json";
        regression.generateRegressionReport = true;
        
        // Default output settings
        output.reportFormat = "html";
        output.reportFile = "test_report.html";
        output.verboseOutput = true;
        output.generateStatistics = true;
        output.saveIntermediateResults = false;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "=== Test Settings ===" << std::endl;
        ss << std::endl;
        
        ss << "Performance Settings:" << std::endl;
        ss << "  Min Neurons Per Second: " << performance.minNeuronsPerSecond << std::endl;
        ss << "  Max Memory Usage: " << performance.maxMemoryUsageMB << "MB" << std::endl;
        ss << "  Max Step Time: " << performance.maxStepTime.count() << "ns" << std::endl;
        ss << "  Enable Monitoring: " << (performance.enableMonitoring ? "Yes" : "No") << std::endl;
        ss << "  Enable Benchmarking: " << (performance.enableBenchmarking ? "Yes" : "No") << std::endl;
        ss << "  Benchmark Iterations: " << performance.benchmarkIterations << std::endl;
        
        ss << std::endl;
        ss << "Stress Settings:" << std::endl;
        ss << "  Max Network Size: " << stress.maxNetworkSize << std::endl;
        ss << "  Max Plasticity Rate: " << stress.maxPlasticityRate << std::endl;
        ss << "  Max Simulation Time: " << stress.maxSimulationTime.count() << "ns" << std::endl;
        ss << "  Enable Concurrent Stress: " << (stress.enableConcurrentStress ? "Yes" : "No") << std::endl;
        
        ss << std::endl;
        ss << "Memory Settings:" << std::endl;
        ss << "  Max Memory Delta: " << memory.maxMemoryDeltaKB << "KB" << std::endl;
        ss << "  Detect Leaks: " << (memory.detectLeaks ? "Yes" : "No") << std::endl;
        ss << "  Allocation Rounds: " << memory.allocationRounds << std::endl;
        ss << "  Sample Interval: " << memory.sampleInterval << "ms" << std::endl;
        
        ss << std::endl;
        ss << "Concurrency Settings:" << std::endl;
        ss << "  Max Threads: " << concurrency.maxThreads << std::endl;
        ss << "  Thread Contention: " << concurrency.threadContention << std::endl;
        ss << "  Detect Deadlocks: " << (concurrency.detectDeadlocks ? "Yes" : "No") << std::endl;
        ss << "  Test Race Conditions: " << (concurrency.testRaceConditions ? "Yes" : "No") << std::endl;
        
        ss << std::endl;
        ss << "Regression Settings:" << std::endl;
        ss << "  Regression Threshold: " << regression.regressionThreshold << std::endl;
        ss << "  Compare With Baseline: " << (regression.compareWithBaseline ? "Yes" : "No") << std::endl;
        ss << "  Baseline File: " << regression.baselineFile << std::endl;
        ss << "  Generate Regression Report: " << (regression.generateRegressionReport ? "Yes" : "No") << std::endl;
        
        ss << std::endl;
        ss << "Output Settings:" << std::endl;
        ss << "  Report Format: " << output.reportFormat << std::endl;
        ss << "  Report File: " << output.reportFile << std::endl;
        ss << "  Verbose Output: " << (output.verboseOutput ? "Yes" : "No") << std::endl;
        ss << "  Generate Statistics: " << (output.generateStatistics ? "Yes" : "No") << std::endl;
        ss << "  Save Intermediate Results: " << (output.saveIntermediateResults ? "Yes" : "No") << std::endl;
        
        return ss.str();
    }
    
    bool validate() const {
        bool valid = true;
        
        if (performance.minNeuronsPerSecond <= 0.0) {
            std::cerr << "Error: minNeuronsPerSecond must be positive" << std::endl;
            valid = false;
        }
        
        if (performance.maxMemoryUsageMB <= 0.0) {
            std::cerr << "Error: maxMemoryUsageMB must be positive" << std::endl;
            valid = false;
        }
        
        if (performance.maxStepTime <= std::chrono::nanoseconds(0)) {
            std::cerr << "Error: maxStepTime must be positive" << std::endl;
            valid = false;
        }
        
        if (stress.maxNetworkSize <= 0) {
            std::cerr << "Error: maxNetworkSize must be positive" << std::endl;
            valid = false;
        }
        
        if (stress.maxPlasticityRate < 0.0 || stress.maxPlasticityRate > 1.0) {
            std::cerr << "Error: maxPlasticityRate must be between 0 and 1" << std::endl;
            valid = false;
        }
        
        if (memory.maxMemoryDeltaKB <= 0) {
            std::cerr << "Error: maxMemoryDeltaKB must be positive" << std::endl;
            valid = false;
        }
        
        if (concurrency.maxThreads <= 0) {
            std::cerr << "Error: maxThreads must be positive" << std::endl;
            valid = false;
        }
        
        if (regression.regressionThreshold <= 1.0) {
            std::cerr << "Error: regressionThreshold must be greater than 1.0" << std::endl;
            valid = false;
        }
        
        if (output.reportFormat != "text" && output.reportFormat != "xml" && 
            output.reportFormat != "json" && output.reportFormat != "csv" && 
            output.reportFormat != "html") {
            std::cerr << "Error: Invalid report format: " << output.reportFormat << std::endl;
            valid = false;
        }
        
        return valid;
    }
}; 

// Global test settings
extern TestSettings globalTestSettings;

// Load test settings from file
bool loadTestSettings(const std::string& filename);

// Save test settings to file
bool saveTestSettings(const std::string& filename);

}}} // namespace nlm::test
