// NLM Test Utilities
// Common utilities for testing NLM components

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

namespace nlm {

// File utilities for testing
class TestFileUtils {
public:
    // Create a temporary directory for test files
    static std::string createTempDirectory(const std::string& prefix = "nlm_test_") {
        std::string tempDir = "/tmp/" + prefix + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        std::string mkdirCmd = "mkdir -p " + tempDir;
        system(mkdirCmd.c_str());
        return tempDir;
    }
    
    // Remove a directory and all its contents
    static bool removeDirectory(const std::string& path) {
        std::string rmCmd = "rm -rf " + path;
        return system(rmCmd.c_str()) == 0;
    }
    
    // Write content to a file
    static bool writeToFile(const std::string& filepath, const std::string& content) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return true;
    }
    
    // Read content from a file
    static std::string readFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    // Check if file exists
    static bool fileExists(const std::string& filepath) {
        std::ifstream file(filepath);
        return file.good();
    }
};

// Configuration utilities for testing
class ConfigTestUtils {
public:
    // Create a complete test configuration
    static std::shared_ptr<Config> createCompleteTestConfig() {
        auto config = std::make_shared<Config>();
        
        // Basic simulation settings
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default, "Random seed for reproducible tests");
        config->set("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds");
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default, "Number of neurons");
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default, "Number of brain regions");
        config->set("connection_probability", 0.1, ConfigSource::Default, "Connection probability for random connectivity");
        
        // Plasticity settings
        config->set("plasticity_learning_rate", 0.01, ConfigSource::Default, "Plasticity learning rate");
        config->set("stdp_ltp_weight", 0.01, ConfigSource::Default, "STDP LTP weight");
        config->set("stdp_ltd_weight", 0.012, ConfigSource::Default, "STDP LTD weight");
        config->set("stdp_time_constant", 20.0, ConfigSource::Default, "STDP time constant");
        
        // Development settings
        config->set("development_synaptogenesis_rate", 0.001, ConfigSource::Default, "Synaptogenesis rate");
        config->set("development_pruning_rate", 0.0001, ConfigSource::Default, "Pruning rate");
        
        // Neuromodulation settings
        config->set("dopamine_baseline", 0.1, ConfigSource::Default, "Dopamine baseline level");
        config->set("dopamine_excitability", 1.0, ConfigSource::Default, "Dopamine excitability factor");
        config->set("dopamine_plasticity", 0.5, ConfigSource::Default, "Dopamine plasticity factor");
        
        // Reward settings
        config->set("reward_discount_factor", 0.99, ConfigSource::Default, "Reward discount factor");
        
        // Environment settings
        config->set("environment_name", std::string("GridWorld"), ConfigSource::Default, "Environment name");
        config->set("environment_width", 10, ConfigSource::Default, "Environment width");
        config->set("environment_height", 10, ConfigSource::Default, "Environment height");
        
        // Logging settings
        config->set("log_level", std::string("INFO"), ConfigSource::Default, "Log level");
        config->set("log_to_file", false, ConfigSource::Default, "Log to file");
        config->set("log_filename", std::string("nlm_test.log"), ConfigSource::Default, "Log filename");
        
        // Simulation settings
        config->set("max_simulation_steps", 10000, ConfigSource::Default, "Maximum simulation steps");
        config->set("simulation_time_limit", 0.0, ConfigSource::Default, "Simulation time limit (0 = unlimited)");
        
        // Visualization settings
        config->set("visualization_enabled", false, ConfigSource::Default, "Enable visualization");
        config->set("visualization_update_rate", 30.0, ConfigSource::Default, "Visualization update rate (Hz)");
        
        return config;
    }
    
    // Load test configuration from file with fallback
    static std::shared_ptr<Config> loadTestConfig(const std::string& filepath = "configs/default.cfg") {
        auto config = std::make_shared<Config>();
        
        // Try to load from file
        if (config->loadFromFile(filepath)) {
            std::cout << "Loaded test configuration from: " << filepath << std::endl;
            return config;
        }
        
        // If file not found, use complete test configuration
        std::cout << "Configuration file not found, using default test configuration" << std::endl;
        return createCompleteTestConfig();
    }
    
    // Compare two configurations
    static bool configsEqual(const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2) {
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        if (keys1.size() != keys2.size()) {
            return false;
        }
        
        for (const auto& key : keys1) {
            if (!config2->has(key)) {
                return false;
            }
            
            auto value1 = config1->get<std::string>(key);
            auto value2 = config2->get<std::string>(key);
            
            if (value1.is_null() != value2.is_null()) {
                return false;
            }
            
            if (value1.has_value() && value2.has_value()) {
                if (value1.value() != value2.value()) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    // Get configuration diff
    static std::string getConfigDiff(const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2) {
        std::stringstream diff;
        
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        diff << "Configuration differences:" << std::endl;
        
        // Find keys only in config1
        for (const auto& key : keys1) {
            if (!config2->has(key)) {
                diff << "  + \"" << key << "\" present only in config1" << std::endl;
            }
        }
        
        // Find keys only in config2
        for (const auto& key : keys2) {
            if (!config1->has(key)) {
                diff << "  + \"" << key << "\" present only in config2" << std::endl;
            }
        }
        
        // Find different values
        for (const auto& key : keys1) {
            if (config2->has(key)) {
                auto value1 = config1->get<std::string>(key);
                auto value2 = config2->get<std::string>(key);
                
                if (value1.is_null() && value2.is_null()) {
                    continue;
                }
                
                if (value1.is_null() || value2.is_null()) {
                    diff << "  * \"" << key << "\" differs: config1=" << (value1.is_null() ? "null" : value1.value()) 
                         << ", config2=" << (value2.is_null() ? "null" : value2.value()) << std::endl;
                } else if (value1.value() != value2.value()) {
                    diff << "  * \"" << key << "\" differs: config1=\"" << value1.value() << "\", config2=\"" 
                         << value2.value() << "\"" << std::endl;
                }
            }
        }
        
        if (diff.str().empty()) {
            diff << "  No differences found" << std::endl;
        }
        
        return diff.str();
    }
};

// Performance measurement utilities
class PerformanceTestUtils {
public:
    // Measure and log performance of a function
    template<typename Func>
    static double measureAndLog(Func func, const std::string& description, 
                               int iterations = 1, bool logToConsole = true) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        double avgTime = duration.count() / 1000000.0 / iterations;
        
        if (logToConsole) {
            std::cout << "Performance: " << description << std::endl;
            std::cout << "  Iterations: " << iterations << std::endl;
            std::cout << "  Total time: " << duration.count() / 1000000.0 << "s" << std::endl;
            std::cout << "  Average time: " << avgTime << "s per iteration" << std::endl;
            std::cout << "  Operations per second: " << iterations / avgTime << std::endl;
        }
        
        return avgTime;
    }
    
    // Measure memory usage (approximate)
    static double measureMemoryUsage() {
        // This is a rough approximation - actual memory measurement would be platform-specific
        // For now, return a placeholder value
        return 0.0;
    }
    
    // Create performance benchmark results
    struct BenchmarkResult {
        std::string testName;
        std::string testDescription;
        int iterations;
        double avgTime;
        double minTime;
        double maxTime;
        double opsPerSecond;
        std::chrono::system_clock::time_point timestamp;
        
        BenchmarkResult(const std::string& name, const std::string& desc, int iters)
            : testName(name), testDescription(desc), iterations(iters),
              avgTime(0.0), minTime(0.0), maxTime(0.0), opsPerSecond(0.0) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    // Run benchmark suite
    template<typename Func>
    static BenchmarkResult runBenchmark(const std::string& testName, const std::string& description,
                                        Func func, int iterations = 10) {
        std::vector<double> times;
        times.reserve(iterations);
        
        for (int i = 0; i < iterations; ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();
            func();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            times.push_back(duration.count() / 1000000.0);
        }
        
        // Calculate statistics
        double sum = 0.0;
        double min = times[0];
        double max = times[0];
        
        for (double time : times) {
            sum += time;
            if (time < min) min = time;
            if (time > max) max = time;
        }
        
        double avg = sum / times.size();
        double opsPerSec = iterations / avg;
        
        return BenchmarkResult(testName, description, iterations) {
            avgTime = avg,
            minTime = min,
            maxTime = max,
            opsPerSecond = opsPerSec
        };
    }
};

// Validation utilities for testing
class ValidationUtils {
public:
    // Validate configuration values against expected ranges
    static bool validateConfigRanges(const std::shared_ptr<Config>& config) {
        bool valid = true;
        
        // Check random seed
        auto seed = config->getOr<int64_t>("random_seed", 0);
        if (seed < 0) {
            std::cerr << "Error: random_seed must be non-negative" << std::endl;
            valid = false;
        }
        
        // Check timestep
        auto timestep = config->getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0) {
            std::cerr << "Error: simulation_timestep must be positive" << std::endl;
            valid = false;
        } else if (timestep > 1.0) {
            std::cerr << "Warning: simulation_timestep is large (" << timestep << "), may cause numerical instability" << std::endl;
        }
        
        // Check neuron count
        auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) {
            std::cerr << "Error: neuron_count must be positive" << std::endl;
            valid = false;
        }
        
        // Check region count
        auto regionCount = config->getOr<int64_t>("region_count", 0);
        if (regionCount <= 0) {
            std::cerr << "Error: region_count must be positive" << std::endl;
            valid = false;
        }
        
        // Check connection probability
        auto connProb = config->getOr<double>("connection_probability", 0.0);
        if (connProb < 0.0 || connProb > 1.0) {
            std::cerr << "Error: connection_probability must be between 0 and 1" << std::endl;
            valid = false;
        }
        
        return valid;
    }
    
    // Validate consistency between configuration values
    static bool validateConfigConsistency(const std::shared_ptr<Config>& config) {
        bool valid = true;
        
        // Check that simulation steps is reasonable relative to timestep
        auto steps = config->getOr<size_t>("max_simulation_steps", 0);
        auto timestep = config->getOr<double>("simulation_timestep", 0.001);
        auto timeLimit = config->getOr<double>("simulation_time_limit", 0.0);
        
        if (steps > 0 && timestep > 0.0) {
            double estimatedTime = steps * timestep;
            if (timeLimit > 0.0 && estimatedTime > timeLimit) {
                std::cerr << "Error: Estimated simulation time (" << estimatedTime << "s) exceeds time limit (" << timeLimit << "s)" << std::endl;
                valid = false;
            }
        }
        
        return valid;
    }
    
    // Validate that all required keys are present and have valid values
    static std::vector<std::string> getMissingKeys(const std::shared_ptr<Config>& config, 
                                                  const std::vector<std::string>& requiredKeys) {
        std::vector<std::string> missing;
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                missing.push_back(key);
            }
        }
        
        return missing;
    }
    
    // Validate that all keys have non-null values
    static std::vector<std::string> getInvalidKeys(const std::shared_ptr<Config>& config) {
        std::vector<std::string> invalid;
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            if (config->get<std::string>(key).is_null()) {
                invalid.push_back(key);
            }
        }
        
        return invalid;
    }
};

// Test reporting utilities
class TestReportUtils {
public:
    // Generate a comprehensive test report
    static std::string generateTestReport(const std::vector<TestFramework::TestCase>& testCases) {
        std::stringstream report;
        
        report << "=== NLM Test Report ===" << std::endl << std::endl;
        
        // Summary section
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        int skippedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& testCase : testCases) {
            totalTests++;
            totalDuration += testCase.result.durationSeconds;
            
            switch (testCase.result.status) {
                case TestStatus::PASSED:
                    passedTests++;
                    break;
                case TestStatus::FAILED:
                    failedTests++;
                    break;
                case TestStatus::SKIPPED:
                    skippedTests++;
                    break;
                default:
                    // Don't count PENDING or RUNNING
                    break;
            }
        }
        
        report << "Summary:" << std::endl;
        report << "  Total tests: " << totalTests << std::endl;
        report << "  Passed: " << passedTests << std::endl;
        report << "  Failed: " << failedTests << std::endl;
        report << "  Skipped: " << skippedTests << std::endl;
        report << "  Total duration: " << std::fixed << std::setprecision(3) << totalDuration << "s" << std::endl;
        report << "  Success rate: " << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%" << std::endl << std::endl;
        
        // Detailed test results
        report << "Detailed Test Results:" << std::endl;
        report << std::string(80, '=') << std::endl;
        
        for (const auto& testCase : testCases) {
            report << std::left << std::setw(40) << testCase.name;
            report << std::left << std::setw(15) << testCase.category;
            
            switch (testCase.result.status) {
                case TestStatus::PENDING:
                    report << "PENDING"; break;
                case TestStatus::RUNNING:
                    report << "RUNNING"; break;
                case TestStatus::PASSED:
                    report << "PASSED"; break;
                case TestStatus::FAILED:
                    report << "FAILED"; break;
                case TestStatus::SKIPPED:
                    report << "SKIPPED"; break;
            }
            
            report << std::fixed << std::setw(10) << testCase.result.durationSeconds << "s";
            report << " - " << testCase.result.message << std::endl;
        }
        
        return report.str();
    }
    
    // Save test report to file
    static bool saveTestReport(const std::string& filepath, const std::string& report) {
        return TestFileUtils::writeToFile(filepath, report);
    }
    
    // Generate JSON test report
    static std::string generateJSONTestReport(const std::vector<TestFramework::TestCase>& testCases) {
        std::stringstream json;
        
        json << "{" << std::endl;
        json << "  \"testReport\": {" << std::endl;
        
        // Summary
        json << "    \"summary\": {" << std::endl;
        int totalTests = 0, passedTests = 0, failedTests = 0, skippedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& testCase : testCases) {
            totalTests++;
            totalDuration += testCase.result.durationSeconds;
            
            switch (testCase.result.status) {
                case TestStatus::PASSED: passedTests++; break;
                case TestStatus::FAILED: failedTests++; break;
                case TestStatus::SKIPPED: skippedTests++; break;
                default: break;
            }
        }
        
        json << "      \"totalTests\": " << totalTests << "," << std::endl;
        json << "      \"passedTests\": " << passedTests << "," << std::endl;
        json << "      \"failedTests\": " << failedTests << "," << std::endl;
        json << "      \"skippedTests\": " << skippedTests << "," << std::endl;
        json << "      \"totalDuration\": " << std::fixed << std::setprecision(3) << totalDuration << "," << std::endl;
        json << "      \"successRate\": " << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << std::endl;
        json << "    }," << std::endl;
        
        // Detailed results
        json << "    \"tests\": [" << std::endl;
        
        for (size_t i = 0; i < testCases.size(); ++i) {
            const auto& testCase = testCases[i];
            
            json << "      {" << std::endl;
            json << "        \"name\": \"" << testCase.name << "\"," << std::endl;
            json << "        \"category\": \"" << testCase.category << "\"," << std::endl;
            
            std::string statusStr;
            switch (testCase.result.status) {
                case TestStatus::PENDING: statusStr = "PENDING"; break;
                case TestStatus::RUNNING: statusStr = "RUNNING"; break;
                case TestStatus::PASSED: statusStr = "PASSED"; break;
                case TestStatus::FAILED: statusStr = "FAILED"; break;
                case TestStatus::SKIPPED: statusStr = "SKIPPED"; break;
            }
            
            json << "        \"status\": \"" << statusStr << "\"," << std::endl;
            json << "        \"duration\": " << std::fixed << std::setprecision(6) << testCase.result.durationSeconds << "," << std::endl;
            json << "        \"message\": \"" << testCase.result.message << "\"" << std::endl;
            
            if (i < testCases.size() - 1) {
                json << "      }," << std::endl;
            } else {
                json << "      }" << std::endl;
            }
        }
        
        json << "    ]" << std::endl;
        json << "  }" << std::endl;
        json << "}" << std::endl;
        
        return json.str();
    }
};

} // namespace nlm
