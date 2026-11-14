#ifndef NLM_DEMO_FRAMEWORK_HPP
#define NLM_DEMO_FRAMEWORK_HPP

#include <string>
#include <memory>
#include <map>
#include <iostream>
#include <vector>
#include <chrono>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// CLI argument structure for all demos
struct CLIArgs {
    std::string phase;
    std::string demo;
    std::string configFile;
    int neurons = 500;
    int steps = 1000;
    double timestep = 0.001;
    long seed = 42;
    bool verbose = false;
    bool quiet = false;
    bool benchmark = false;
    bool performance = false;
    std::string checkpointDir = "checkpoints";
    int batches = 1;
    std::string outputFile;
    bool help = false;
    
    // Additional demo-specific parameters
    std::map<std::string, std::string> extraParams;
};

// Base demo class to reduce code duplication
class DemoBase {
public:
    virtual ~DemoBase() {}
    virtual void run(const CLIArgs& args) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void addConfigOptions(std::shared_ptr<Config> config) = 0;
    
    // Common helper methods
    void logInfo(const std::string& message) const {
        NLM_LOG_INFO(message);
    }
    
    void logError(const std::string& message) const {
        NLM_LOG_ERROR(message);
    }
    
    void logWarning(const std::string& message) const {
        NLM_LOG_WARNING(message);
    }
    
    void printProgress(int current, int total, const std::string& label = "Progress") {
        if (current % 100 == 0 || current == total) {
            std::cout << "\r" << label << ": " << current << "/" << total 
                      << " (" << (100.0 * current / total) << "%)";
            std::cout.flush();
        }
    }
    
    // Configuration loading helper
    bool loadConfig(std::shared_ptr<Config>& config, const CLIArgs& args) {
        if (!args.configFile.empty()) {
            if (config->loadFromFile(args.configFile)) {
                logInfo("Loaded configuration from: " + args.configFile);
                return true;
            } else {
                logWarning("Failed to load configuration from: " + args.configFile);
            }
        }
        return false;
    }
    
    // Brain initialization helper
    std::shared_ptr<Brain> createBrain(const CLIArgs& args, std::shared_ptr<Config> config) {
        // Override config with CLI arguments
        config->set("random_seed", args.seed, ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(args.neurons), ConfigSource::CommandLine);
        config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            logError("Failed to initialize brain!");
            return nullptr;
        }
        
        return brain;
    }
    
    // Benchmark timing helper
    struct BenchmarkResult {
        double initializationMs = 0.0;
        double simulationMs = 0.0;
        double stepsPerSecond = 0.0;
        size_t totalSpikes = 0;
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
    };
    
    BenchmarkResult benchmarkBrain(std::shared_ptr<Brain> brain, int steps = 100) {
        BenchmarkResult result;
        
        // Time initialization
        auto startTime = std::chrono::high_resolution_clock::now();
        if (brain && !brain->initialize()) {
            logError("Brain initialization failed during benchmark");
            return result;
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        result.initializationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        if (!brain) return result;
        
        result.totalNeurons = brain->getTotalNeuronCount();
        result.totalSynapses = brain->getTotalSynapseCount();
        
        // Time simulation steps
        startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        endTime = std::chrono::high_resolution_clock::now();
        result.simulationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        result.totalSpikes = brain->getTotalSpikeCount();
        
        if (result.simulationMs > 0) {
            result.stepsPerSecond = steps / (result.simulationMs / 1000.0);
        }
        
        return result;
    }
    
    // Output results to file helper
    bool saveResults(const std::string& filename, const std::string& data) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            logError("Failed to open output file: " + filename);
            return false;
        }
        file << data;
        file.close();
        return true;
    }
};

// Performance benchmarking utilities
class Benchmark {
public:
    static void runBenchmarks(const CLIArgs& args) {
        logInfo("=== NLM Performance Benchmark ===");
        
        // Test different brain sizes
        std::vector<int> neuronCounts = {100, 500, 1000, 2000};
        
        for (int neuronCount : neuronCounts) {
            logInfo("");
            logInfo("Benchmarking brain size: " + std::to_string(neuronCount) + " neurons");
            
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", static_cast<int64_t>(2));
            config->set("connection_probability", 0.05f);
            config->set("random_seed", static_cast<int64_t>(args.seed));
            config->set("simulation_timestep", args.timestep);
            
            // Time initialization
            auto startTime = std::chrono::high_resolution_clock::now();
            
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                logInfo("  Initialization: " + std::to_string(duration.count()) + "ms");
                logInfo("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
                
                // Time simulation steps
                startTime = std::chrono::high_resolution_clock::now();
                
                for (int step = 0; step < 100; ++step) {
                    brain->step(step, step * args.timestep);
                }
                
                endTime = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                
                double avgTimePerStep = static_cast<double>(duration.count()) / 100.0;
                double stepsPerSecond = 1000.0 / avgTimePerStep;
                
                logInfo("  Simulation (100 steps): " + std::to_string(duration.count()) + "ms");
                logInfo("  Steps per second: " + std::to_string(stepsPerSecond));
                logInfo("  Neurons per second: " + std::to_string(stepsPerSecond * neuronCount / 1000.0));
            }
        }
        
        logInfo("");
        logInfo("Benchmark complete.\n");
    }
};

// Demo factory for easy registration
class DemoFactory {
public:
    static std::map<std::string, std::unique_ptr<DemoBase>> demos;
    
    static void registerDemo(const std::string& name, std::unique_ptr<DemoBase> demo) {
        demos[name] = std::move(demo);
    }
    
    static bool runDemo(const std::string& name, const CLIArgs& args) {
        std::string demoKey = toLower(name);
        if (demos.find(demoKey) != demos.end()) {
            demos[demoKey]->run(args);
            return true;
        } else {
            logError("Unknown demo: " + name);
            return false;
        }
    }
    
    static void printAvailableDemos() {
        logInfo("Available demos:");
        for (const auto& pair : demos) {
            logInfo("  " + pair.first + " - " + pair.second->getDescription());
        }
    }
    
private:
    static std::string toLower(std::string str) {
        for (char& c : str) {
            c = std::tolower(static_cast<unsigned char>(c));
        }
        return str;
    }
};

} // namespace nlm

#endif // NLM_DEMO_FRAMEWORK_HPP
