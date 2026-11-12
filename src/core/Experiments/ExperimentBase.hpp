// Experiment Base Classes
// Provides common functionality for all NLM experiments

#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"

namespace nlm {

// Base class for all experiments
class ExperimentBase {
public:
    ExperimentBase() = default;
    virtual ~ExperimentBase() = default;
    
    // Disable copying
    ExperimentBase(const ExperimentBase&) = delete;
    ExperimentBase& operator=(const ExperimentBase&) = delete;
    
    // Run the experiment
    virtual int run(int argc, char** argv) = 0;
    
    // Get experiment name
    virtual std::string getName() const = 0;
    
    // Get experiment description
    virtual std::string getDescription() const = 0;
    
    // Setup common experiment infrastructure
    void setupCommonInfrastructure() {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        printHeader(getName());
    }
    
    // Load configuration from command line and file
    std::shared_ptr<Config> loadConfiguration(const std::string& defaultConfig = "configs/default.cfg") {
        auto config = std::make_shared<Config>();
        
        // Try to load from command line args
        config->loadFromArgs(1, nullptr);  // Will be overridden by actual args in run()
        
        // Try to load from file if provided via command line
        // This is implemented in each specific experiment
        
        return config;
    }
    
    // Create and initialize brain
    std::shared_ptr<Brain> createAndInitializeBrain(std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            throw InitializationError("Brain initialization failed");
        }
        
        return brain;
    }
    
    // Create and initialize agent
    std::shared_ptr<AgentBrain> createAndInitializeAgent(std::shared_ptr<Brain> brain, 
                                                          std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(*brain);
        agent->initialize(*world);
        return agent;
    }
    
    // Create and configure world
    std::shared_ptr<SimpleWorld> createWorld(double width = 20.0, double height = 20.0,
                                            int visionWidth = 16, int visionHeight = 16,
                                            uint64_t seed = 42) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        if (seed != 0) {
            world->setRandomSeed(seed);
        }
        return world;
    }
    
    // Common timing utility
    class Timer {
    public:
        Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            return duration.count() / 1000000.0;
        }
        
        void reset() {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
    private:
        std::chrono::high_resolution_clock::time_point startTime;
    };
    
protected:
    // Print formatted header
    void printHeader(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "  " << title << "\n";
        std::cout << std::string(60, '=') << "\n\n";
    }
    
    // Print formatted result
    void printResult(const std::string& name, float value, float max = 1.0f) {
        std::cout << "  " << std::left << std::setw(30) << name << ": ";
        std::cout << std::fixed << std::setprecision(3) << value;
        if (max > 0) {
            std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
        }
        std::cout << "\n";
    }
    
    // Print section header
    void printSection(const std::string& title) {
        std::cout << "\n--- " << title << " ---\n";
    }
    
    // Format a number with specified precision
    std::string formatNumber(float value, int precision = 3, bool fixed = false) {
        std::stringstream ss;
        if (fixed) {
            ss << std::fixed << std::setprecision(precision) << value;
        } else {
            ss << std::setprecision(precision) << value;
        }
        return ss.str();
};

// Experiment result base class
struct ExperimentResult {
    std::string experimentName;
    double executionTime;
    bool success;
    std::map<std::string, float> metrics;
    std::string outputFile;
    
    ExperimentResult(const std::string& name) 
        : experimentName(name), executionTime(0.0), success(false) {}
};

// Configuration utilities
class ConfigurationUtils {
public:
    // Apply default configuration for different experiment types
    static void applyDefaultsForPhase(std::shared_ptr<Config> config, int phase) {
        if (!config) return;
        
        switch (phase) {
            case 2:
                config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
                config->set("simulation_timestep", 0.001, ConfigSource::Default);
                config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
                config->set("connection_probability", 0.15f, ConfigSource::Default);
                config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
                config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
                config->set("stdp_tau", 20.0f, ConfigSource::Default);
                break;
                
            case 3:
                config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
                config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
                config->set("connection_probability", 0.1f, ConfigSource::Default);
                config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
                config->set("pruning_rate", 0.00002f, ConfigSource::Default);
                break;
                
            case 4:
                config->set("random_seed", static_cast<int64_t>(456), ConfigSource::Default);
                config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
                config->set("connection_probability", 0.08f, ConfigSource::Default);
                config->set("working_memory_capacity", 200, ConfigSource::Default);
                config->set("episodic_memory_max_episodes", 500, ConfigSource::Default);
                break;
                
            case 5:
                config->set("random_seed", static_cast<int64_t>(789), ConfigSource::Default);
                config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
                config->set("connection_probability", 0.05f, ConfigSource::Default);
                config->set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
                config->set("max_checkpoint_memory", 10000, ConfigSource::Default);
                break;
                
            case 6:
                config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
                config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
                config->set("connection_probability", 0.12f, ConfigSource::Default);
                config->set("enable_checkpointing", true, ConfigSource::Default);
                config->set("enable_replay", true, ConfigSource::Default);
                config->set("enable_development", true, ConfigSource::Default);
                config->set("working_memory_capacity", neuronCount / 10, ConfigSource::Default);
                config->set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
                break;
        }
    }
    
    // Load configuration from file
    static bool loadConfigFile(std::shared_ptr<Config> config, const std::string& filename) {
        if (!config) return false;
        return config->loadFromFile(filename);
    }
    
    // Parse command line arguments for configuration
    static void parseCommandLineArgs(int argc, char** argv, std::shared_ptr<Config> config) {
        if (!config || argc <= 1) return;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config") {
                std::string configFile = "configs/default.cfg";
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
                loadConfigFile(config, configFile);
            } else if (arg.substr(0, 9) == "--neuron-count") {
                int64_t count = 1000;
                if (arg.find('=') != std::string::npos) {
                    count = std::stoi(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    count = std::stoi(argv[++i]);
                }
                config->set("neuron_count", count, ConfigSource::CommandLine);
            } else if (arg.substr(0, 10) == "--seed") {
                uint64_t seed = 42;
                if (arg.find('=') != std::string::npos) {
                    seed = std::stoull(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    seed = std::stoull(argv[++i]);
                }
                config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
            } else if (arg.substr(0, 6) == "--help") {
                printUsage();
                exit(0);
            }
        }
    }
    
    // Print usage information
    static void printUsage() {
        std::cout << "NLM Experiment Usage:\n";
        std::cout << "  --help                Show this help message\n";
        std::cout << "  --config=<file>       Load configuration from file\n";
        std::cout << "  --neuron-count=<N>    Set number of neurons (default: 1000)\n";
        std::cout << "  --seed=<S>            Set random seed (default: 42)\n";
        std::cout << "\nExperiment-specific options may be available for each phase.\n";
    }
};

// Experiment result utilities
class ExperimentResultUtils {
public:
    // Save experiment results to file
    static bool saveResults(const ExperimentResult& result) {
        if (result.outputFile.empty()) return false;
        
        std::ofstream file(result.outputFile);
        if (!file.is_open()) return false;
        
        file << "Experiment: " << result.experimentName << "\n";
        file << "Execution Time: " << result.executionTime << " seconds\n";
        file << "Success: " << (result.success ? "Yes" : "No") << "\n\n";
        
        file << "Metrics:\n";
        for (const auto& metric : result.metrics) {
            file << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        file.close();
        return true;
    }
    
    // Print experiment summary
    static void printSummary(const ExperimentResult& result) {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "  EXPERIMENT SUMMARY\n";
        std::cout << std::string(60, '=') << "\n";
        std::cout << "  Experiment: " << result.experimentName << "\n";
        std::cout << "  Execution Time: " << formatNumber(result.executionTime, 3) << " seconds\n";
        std::cout << "  Success: " << (result.success ? "Yes" : "No") << "\n";
        
        if (!result.metrics.empty()) {
            std::cout << "\n  Key Metrics:\n";
            for (const auto& metric : result.metrics) {
                printResult(metric.first, metric.second);
            }
        }
        
        std::cout << "\n" << std::string(60, '=') << "\n";
    }
    
    // Create default metrics for experiment
    static std::map<std::string, float> createDefaultMetrics() {
        return std::map<std::string, float>{
            {"execution_time", 0.0f},
            {"neuron_count", 0.0f},
            {"firing_rate", 0.0f},
            {"plasticity_strength", 0.0f},
            {"learning_occurred", 0.0f}
        };
    }
};

// Common test utilities
class TestUtils {
public:
    // Generate a deterministic pattern of values
    static std::vector<float> generatePattern(int size, float amplitude = 1.0f, float frequency = 0.1f) {
        std::vector<float> pattern(size);
        for (int i = 0; i < size; ++i) {
            pattern[i] = amplitude * std::sin(i * frequency * 3.14159 * 2 / size);
        }
        return pattern;
    }
    
    // Calculate statistics for a vector
    static std::map<std::string, float> calculateStatistics(const std::vector<float>& values) {
        std::map<std::string, float> stats;
        if (values.empty()) {
            return stats;
        }
        
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        float mean = sum / values.size();
        
        float min = *std::min_element(values.begin(), values.end());
        float max = *std::max_element(values.begin(), values.end());
        
        float variance = 0.0f;
        for (float val : values) {
            variance += (val - mean) * (val - mean);
        }
        variance /= values.size();
        float stddev = std::sqrt(variance);
        
        stats["mean"] = mean;
        stats["min"] = min;
        stats["max"] = max;
        stats["stddev"] = stddev;
        stats["range"] = max - min;
        stats["size"] = values.size();
        
        return stats;
    }
    
    // Format a memory size in human-readable format
    static std::string formatMemorySize(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double size = bytes;
        
        while (size >= 1024.0 && unitIndex < 4) {
            size /= 1024.0;
            unitIndex++;
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        return ss.str();
    }
};

// Exception hierarchy for experiments
class ExperimentException : public std::runtime_error {
public:
    explicit ExperimentException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationException : public ExperimentException {
public:
    explicit ConfigurationException(const std::string& message) : ExperimentException("Configuration Error: " + message) {}
};

class ExecutionException : public ExperimentException {
public:
    explicit ExecutionException(const std::string& message) : ExperimentException("Execution Error: " + message) {}
};

class ValidationException : public ExperimentException {
public:
    explicit ValidationException(const std::string& message) : ExperimentException("Validation Error: " + message) {}
};
}