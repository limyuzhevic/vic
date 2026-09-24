// Performance Test Configuration System
// NLM Phase 5: Performance Regression Testing Configuration

#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

// Performance test scenarios
scale_t {
    Small = 1,     // Baseline: 100 neurons
    Medium = 10,    // Medium: 1000 neurons
    Large = 100,    // Large: 10,000 neurons
    ExtraLarge = 1000  // Extra large: 100,000+ neurons
};

// Test scenario configuration
struct TestScenario {
    std::string name;
    scale_t scale;
    size_t neuronCount;
    size_t synapticConnections;
    size_t simulationSteps;
    std::string description;
    
    TestScenario() : scale(scale_t::Small), neuronCount(100), synapticConnections(1000),
                     simulationSteps(1000), description("Small scale baseline") {}
};

// Performance regression thresholds
struct PerformanceThresholds {
    // Timing thresholds (seconds)
    double maxWallTimeSmall;      // Small scale maximum wall time
    double maxWallTimeMedium;     // Medium scale maximum wall time
    double maxWallTimeLarge;      // Large scale maximum wall time
    double maxWallTimeExtraLarge; // Extra large scale maximum wall time
    
    // Performance thresholds (operations per second)
    double minOpsPerSecondSmall;
    double minOpsPerSecondMedium;
    double minOpsPerSecondLarge;
    double minOpsPerSecondExtraLarge;
    
    // Memory thresholds (bytes)
    size_t maxMemoryUsageSmall;
    size_t maxMemoryUsageMedium;
    size_t maxMemoryUsageLarge;
    size_t maxMemoryUsageExtraLarge;
    
    // Scalability thresholds (improvement factors)
    double minScalabilityFactor;  // Minimum performance per neuron ratio
    double maxMemoryPerOperation; // Maximum memory usage per operation
    
    // Regression detection thresholds
    double regressionThreshold;   // Percentage threshold for regression detection
    int64_t baselineRetentionDays; // How long to retain baseline measurements
    
    PerformanceThresholds() : 
        maxWallTimeSmall(1.0), maxWallTimeMedium(10.0), maxWallTimeLarge(60.0), maxWallTimeExtraLarge(600.0),
        minOpsPerSecondSmall(1000.0), minOpsPerSecondMedium(10000.0), minOpsPerSecondLarge(100000.0), minOpsPerSecondExtraLarge(1000000.0),
        maxMemoryUsageSmall(10 * 1024 * 1024), maxMemoryUsageMedium(100 * 1024 * 1024),
        maxMemoryUsageLarge(1000 * 1024 * 1024), maxMemoryUsageExtraLarge(10000 * 1024 * 1024),
        minScalabilityFactor(0.9), maxMemoryPerOperation(1000.0), regressionThreshold(0.05), baselineRetentionDays(90) {}
};

// Performance test configuration
struct PerformanceTestConfig {
    std::string projectName;
    std::string version;
    std::string gitRepository;
    std::string resultsDirectory;
    std::string baselineDirectory;
    bool enableCIIntegration;
    bool enableAutoBaselineCreation;
    bool enableRegressionDetection;
    std::vector<TestScenario> testScenarios;
    PerformanceThresholds thresholds;
    
    PerformanceTestConfig() : 
        projectName("NLM"), version("5.0"), gitRepository(""),
        resultsDirectory("performance_results"), baselineDirectory("performance_baseline"),
        enableCIIntegration(true), enableAutoBaselineCreation(true), enableRegressionDetection(true) {
        
        // Add default test scenarios
        testScenarios.push_back(createScenario("small", scale_t::Small, 100, 1000, 1000, "Small neural population"));
        testScenarios.push_back(createScenario("medium", scale_t::Medium, 1000, 10000, 500, "Medium neural population"));
        testScenarios.push_back(createScenario("large", scale_t::Large, 10000, 100000, 100, "Large neural population"));
        testScenarios.push_back(createScenario("extra_large", scale_t::ExtraLarge, 100000, 1000000, 10, "Extra large neural population"));
    }
    
private:
    TestScenario createScenario(const std::string& name, scale_t scale, 
                               size_t neurons, size_t synapses, size_t steps,
                               const std::string& description) {
        TestScenario scenario;
        scenario.name = name;
        scenario.scale = scale;
        scenario.neuronCount = neurons;
        scenario.synapticConnections = synapses;
        scenario.simulationSteps = steps;
        scenario.description = description;
        return scenario;
    }
};

// Configuration file parser
class PerformanceConfigParser {
private:
    PerformanceTestConfig config;
    
public:
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Configuration file not found: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            parseLine(line);
        }
        
        file.close();
        return true;
    }
    
    bool loadFromDirectory(const std::string& directory) {
        std::string configFile = directory + "/performance_config.json";
        if (fs::exists(configFile)) {
            return loadFromFile(configFile);
        }
        
        // If no JSON config, check for legacy format
        std::string legacyConfig = directory + "/perf_config.txt";
        if (fs::exists(legacyConfig)) {
            return loadFromFile(legacyConfig);
        }
        
        std::cerr << "Warning: No configuration file found in directory: " << directory << std::endl;
        return false;
    }
    
    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot write to configuration file: " << filename << std::endl;
            return;
        }
        
        file << "# NLM Performance Test Configuration" << std::endl;
        file << "# Generated automatically" << std::endl;
        file << std::endl;
        file << "project.name = " << config.projectName << std::endl;
        file << "project.version = " << config.version << std::endl;
        file << "results.directory = " << config.resultsDirectory << std::endl;
        file << "baseline.directory = " << config.baselineDirectory << std::endl;
        file << "ci.integration = " << (config.enableCIIntegration ? "true" : "false") << std::endl;
        file << "auto.baseline = " << (config.enableAutoBaselineCreation ? "true" : "false") << std::endl;
        file << "regression.detection = " << (config.enableRegressionDetection ? "true" : "false") << std::endl;
        file << std::endl;
        
        // Save test scenarios
        file << "# Test scenarios" << std::endl;
        for (const auto& scenario : config.testScenarios) {
            file << "[scenario." << scenario.name << "]" << std::endl;
            file << "scale = " << static_cast<int>(scenario.scale) << std::endl;
            file << "neuron_count = " << scenario.neuronCount << std::endl;
            file << "synaptic_connections = " << scenario.synapticConnections << std::endl;
            file << "simulation_steps = " << scenario.simulationSteps << std::endl;
            file << "description = " << scenario.description << std::endl;
            file << std::endl;
        }
        
        // Save performance thresholds
        file << "# Performance thresholds" << std::endl;
        file << "thresholds.max_wall_time_small = " << config.thresholds.maxWallTimeSmall << std::endl;
        file << "thresholds.min_ops_per_second_small = " << config.thresholds.minOpsPerSecondSmall << std::endl;
        file << "thresholds.max_memory_small = " << config.thresholds.maxMemoryUsageSmall << std::endl;
        file << "thresholds.regression_threshold = " << config.thresholds.regressionThreshold << std::endl;
        
        file.close();
    }
    
    const PerformanceTestConfig& getConfig() const {
        return config;
    }
    
    PerformanceTestConfig& getConfig() {
        return config;
    }
    
private:
    void parseLine(const std::string& line) {
        // Simple key = value parsing
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos || line.find('=') == 0) {
            return;
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Parse based on key
        parseKeyValue(key, value);
    }
    
    void parseKeyValue(const std::string& key, const std::string& value) {
        if (key == "project.name") {
            config.projectName = value;
        } else if (key == "project.version") {
            config.version = value;
        } else if (key == "results.directory") {
            config.resultsDirectory = value;
        } else if (key == "baseline.directory") {
            config.baselineDirectory = value;
        } else if (key == "ci.integration") {
            config.enableCIIntegration = (value == "true" || value == "true");
        } else if (key == "auto.baseline") {
            config.enableAutoBaselineCreation = (value == "true" || value == "true");
        } else if (key == "regression.detection") {
            config.enableRegressionDetection = (value == "true" || value == "true");
        }
        // Other parsing can be added as needed
    }
};