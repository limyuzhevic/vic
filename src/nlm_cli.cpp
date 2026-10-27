#!/usr/bin/env cpp
/**
 * NLM Command Line Interface
 * 
 * Provides comprehensive control over NLM brain simulation with support for:
 * - Configuration management
 * - Command execution
 * - Simulation control
 * - Memory management
 * - Debug and logging
 * - Integration with other tools
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/Types/Types.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"

namespace nlm {

    // Command line options structure
    struct CLIConfig {
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "";
        std::string checkpointFile = "";
        std::string logLevel = "info";
        std::string logFile = "";
        
        // Simulation parameters
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        size_t maxSteps = 10000;
        double timestep = 0.001;
        float connectionProbability = 0.1f;
        
        // Experiment parameters
        bool enableLearning = true;
        bool enableMemory = true;
        bool enableNeuromodulation = true;
        bool enableDevelopment = true;
        bool enableReplay = true;
        bool enableCheckpointing = true;
        
        // Behavior parameters
        float learningRate = 0.01f;
        float noveltyThreshold = 0.5f;
        float curiosityDrive = 1.0f;
        float dopamineLevel = 0.5f;
        
        // Control parameters
        size_t experimentalCycles = 1;
        size_t sleepInterval = 100;
        bool continuousMode = false;
        bool quietMode = false;
        
        std::map<std::string, std::string> customParams;
    };

    class NLMCLI {
    public:
        NLMCLI() {
            logger_ = std::make_shared<Logger>();
            consoleLogger_ = std::make_shared<ConsoleLogger>(parseLogLevel(logLevel_));
            logger_->addLogger(consoleLogger_);
            Logger::setGlobal(logger_);
        }

        bool parseArguments(int argc, char** argv) {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                if (arg == "--help" || arg == "-h") {
                    printHelp();
                    return false;
                }
                else if (arg == "--version") {
                    printVersion();
                    return false;
                }
                else if (arg.substr(0, 2) == "--") {
                    size_t pos = arg.find("=");
                    if (pos != std::string::npos) {
                        std::string key = arg.substr(2, pos - 2);
                        std::string value = arg.substr(pos + 1);
                        setParameter(key, value);
                    }
                }
                else if (arg[0] == '-') {
                    std::string key = arg.substr(1);
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        std::string value = argv[++i];
                        setParameter(key, value);
                    }
                }
                else if (arg == "configs" || arg == "experiment") {
                    runExperiments();
                    return false;
                }
            }
            return true;
        }

        bool run() {
            printBanner();
            
            // Load configuration
            loadConfiguration();
            
            // Create brain
            auto config = std::make_shared<Config>();
            config->loadFromFile(config_.configFile);
            
            for (const auto& [key, value] : config_.customParams) {
                config->set(key, value, ConfigSource::CommandLine);
            }
            
            // Set default values
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", config_.timestep, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(config_.neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(config_.regionCount), ConfigSource::Default);
            config->set("connection_probability", config_.connectionProbability, ConfigSource::Default);
            
            // Create and initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                logError("Failed to initialize brain!");
                return false;
            }
            
            brain->logStatus();
            
            // Run experiment
            runExperiment(brain);
            
            // Save final state if requested
            if (!config_.outputFile.empty()) {
                if (!brain->save(config_.outputFile)) {
                    logError("Failed to save brain state to: " + config_.outputFile);
                }
            }
            
            logInfo("NLM CLI completed successfully");
            return true;
        }

    private:
        void printBanner() {
            std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Command Line Interface                                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
            
            if (!config_.quietMode) {
                std::cout << "NLM v2.0 - Advanced Neural Simulation CLI\n" << std::endl;
            }
        }

        void printHelp() {
            std::cout << R"(
Usage: nlm [OPTIONS]

Options:
  -h, --help                    Show this help message
  --version                     Show version information
  --config=file                Load configuration from file
  --output=file                Save final brain state to file
  --checkpoint=file            Save checkpoint to file
  --log-level=level            Set log level (debug, info, warning, error)
  --log-file=file              Log output to file
  --neurons=N                 Set number of neurons (default: 1000)
  --regions=N                  Set number of regions (default: 1)
  --steps=N                    Set maximum simulation steps (default: 10000)
  --timestep=T                 Set simulation timestep (default: 0.001)
  --connection-probability=P   Set connection probability (default: 0.1)
  --no-learning                 Disable learning mechanisms
  --no-memory                   Disable memory systems
  --no-neuromodulation          Disable neuromodulation
  --no-development              Disable developmental system
  --no-replay                   Disable replay mechanism
  --no-checkpointing            Disable checkpointing
  --learning-rate=R             Set learning rate (default: 0.01)
  --novelty-threshold=T         Set novelty detection threshold (default: 0.5)
  --curiosity-drive=D           Set curiosity drive (default: 1.0)
  --dopamine-level=L            Set dopamine level (default: 0.5)
  --experimental-cycles=N      Set number of experimental cycles (default: 1)
  --sleep-interval=N           Set sleep interval for replay (default: 100)
  --continuous                  Run in continuous mode
  --quiet                       Quiet mode (minimal output)
  KEY=VALUE                    Set custom configuration parameter

Examples:
  nlm --neurons=2000 --steps=5000 --output=brain.bin
  nlm --config=myconfig.cfg --learning-rate=0.05 --curiosity-drive=2.0
  nlm --continuous --log-level=debug

For more information, visit: https://github.com/nlm-project/nlm
)" << std::endl;
        }

        void printVersion() {
            std::cout << "NLM v2.0 - Advanced Neural Learning Machine\n";
            std::cout << "Phase 6: Final Integration\n";
            std::cout << "Built with C++17\n";
            std::cout << "Python bindings: pynlm\n";
            std::cout << "License: MIT\n";
            std::cout << "Copyright: NLM Research Team\n" << std::endl;
        }

        void setParameter(const std::string& key, const std::string& value) {
            config_.customParams[key] = value;
            
            // Parse boolean parameters
            if (key == "no-learning") {
                config_.enableLearning = value == "true" || value == "1" || value == "yes";
            }
            else if (key == "no-memory") {
                config_.enableMemory = value == "true" || value == "1" || value == "yes";
            }
            else if (key == "no-neuromodulation") {
                config_.enableNeuromodulation = value == "true" || value == "1" || value == "yes";
            }
            else if (key == "no-development") {
                config_.enableDevelopment = value == "true" || value == "1" || value == "yes";
            }
            else if (key == "no-replay") {
                config_.enableReplay = value == "true" || value == "1" || value == "yes";
            }
            else if (key == "no-checkpointing") {
                config_.enableCheckpointing = value == "true" || value == "1" || value == "yes";
            }
            else if (key == "continuous") {
                config_.continuousMode = value == "true" || value == "1" || value == "yes";
            }
            else if (key == "quiet") {
                config_.quietMode = value == "true" || value == "1" || value == "yes";
            }
            // Parse numeric parameters
            else if (key == "neurons") {
                config_.neuronCount = std::stoull(value);
            }
            else if (key == "regions") {
                config_.regionCount = std::stoull(value);
            }
            else if (key == "steps") {
                config_.maxSteps = std::stoull(value);
            }
            else if (key == "timestep") {
                config_.timestep = std::stod(value);
            }
            else if (key == "connection-probability") {
                config_.connectionProbability = std::stof(value);
            }
            else if (key == "learning-rate") {
                config_.learningRate = std::stof(value);
            }
            else if (key == "novelty-threshold") {
                config_.noveltyThreshold = std::stof(value);
            }
            else if (key == "curiosity-drive") {
                config_.curiosityDrive = std::stof(value);
            }
            else if (key == "dopamine-level") {
                config_.dopamineLevel = std::stof(value);
            }
            else if (key == "experimental-cycles") {
                config_.experimentalCycles = std::stoull(value);
            }
            else if (key == "sleep-interval") {
                config_.sleepInterval = std::stoull(value);
            }
            // Parse file paths
            else if (key == "output") {
                config_.outputFile = value;
            }
            else if (key == "checkpoint") {
                config_.checkpointFile = value;
            }
            else if (key == "config") {
                config_.configFile = value;
            }
            else if (key == "log-file") {
                config_.logFile = value;
            }
            else if (key == "log-level") {
                config_.logLevel = value;
            }
        }

        LogLevel parseLogLevel(const std::string& level) {
            std::string levelStr = level;
            std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::tolower);
            
            if (levelStr == "debug") return LogLevel::Debug;
            if (levelStr == "info") return LogLevel::Info;
            if (levelStr == "warning") return LogLevel::Warning;
            if (levelStr == "error") return LogLevel::Error;
            return LogLevel::Info; // default
        }

        void loadConfiguration() {
            // Load default config file if it exists
            std::ifstream defaultConfig("configs/default.cfg");
            if (defaultConfig.good()) {
                logInfo("Using default configuration from configs/default.cfg");
            }
            
            // Log current configuration
            logInfo("Configuration loaded:")
                << "  Neurons: " << config_.neuronCount
                << "  Regions: " << config_.regionCount
                << "  Max Steps: " << config_.maxSteps
                << "  Timestep: " << config_.timestep
                << "  Connection Probability: " << config_.connectionProbability
                << "  Learning: " << (config_.enableLearning ? "enabled" : "disabled")
                << "  Memory: " << (config_.enableMemory ? "enabled" : "disabled")
                << "  Neuromodulation: " << (config_.enableNeuromodulation ? "enabled" : "disabled")
                << "  Development: " << (config_.enableDevelopment ? "enabled" : "disabled")
                << "  Replay: " << (config_.enableReplay ? "enabled" : "disabled")
                << "  Checkpointing: " << (config_.enableCheckpointing ? "enabled" : "disabled");
        }

        void runExperiment(std::shared_ptr<Brain> brain) {
            logInfo("Starting NLM experiment with " << config_.experimentalCycles << " cycles");
            
            for (size_t cycle = 0; cycle < config_.experimentalCycles; ++cycle) {
                logInfo("");
                logInfo("=== Experimental Cycle " << (cycle + 1) << "/" << config_.experimentalCycles << " ===");
                
                // Create sensory input
                auto vision = std::make_unique<Vision>(10, 10, 3);
                for (size_t i = 0; i < 100; ++i) {
                    float value = static_cast<float>(rand()) / RAND_MAX;
                    vision->setData(&value, 1);
                }
                
                // Inject sensory input
                brain->receiveSensoryInput(*vision);
                
                // Run simulation steps
                for (size_t step = 0; step < config_.maxSteps; ++step) {
                    brain->step(step, step * config_.timestep);
                    
                    // Apply neuromodulation
                    if (config_.enableNeuromodulation) {
                        auto dopamine = std::make_unique<Dopamine>();
                        dopamine->setLevel(config_.dopamineLevel);
                        brain->applyNeuromodulation(*dopamine);
                    }
                    
                    // Log progress
                    if (step % config_.sleepInterval == 0 && !config_.quietMode) {
                        logInfo("Step " << step << ": "
                            << "Firing: " << brain->getFiringNeuronCount()
                            << " | Spikes: " << brain->getTotalSpikeCount()
                            << " | E/I Ratio: " << brain->getExcitationInhibitionRatio());
                    }
                }
                
                // Save checkpoint if enabled
                if (config_.enableCheckpointing && !config_.checkpointFile.empty()) {
                    std::string checkpointFile = config_.checkpointFile;
                    if (config_.experimentalCycles > 1) {
                        checkpointFile = config_.checkpointFile + ".cycle" + std::to_string(cycle + 1);
                    }
                    brain->save(checkpointFile);
                    logInfo("Checkpoint saved to: " + checkpointFile);
                }
                
                // Sleep between cycles
                if (config_.experimentalCycles > 1 && cycle < config_.experimentalCycles - 1) {
                    logInfo("Sleeping before next cycle...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            }
        }

        void logInfo(const std::string& message) {
            if (!config_.quietMode) {
                NLM_LOG_INFO(message);
            }
        }

        void logError(const std::string& message) {
            NLM_LOG_ERROR(message);
        }

        void runExperiments() {
            logInfo("Running experimental configurations...");
            
            // Example experimental configurations
            std::vector<CLIConfig> experiments = {
                {"configs/experiment1.cfg", "brain1.bin", "checkpoint1.bin", "info", "", 
                 2000, 1, 5000, 0.001, 0.15f, true, true, true, true, true, true, 0.02f, 0.6f, 1.5f, 0.7f, 2, 100},
                {"configs/experiment2.cfg", "brain2.bin", "checkpoint2.bin", "debug", "", 
                 500, 2, 1000, 0.002, 0.05f, false, true, false, true, true, true, 0.005f, 0.3f, 0.8f, 0.3f, 1, 200}
            };
            
            for (size_t i = 0; i < experiments.size(); ++i) {
                logInfo("");
                logInfo("=== Running Experiment " << (i + 1) << "/" << experiments.size() << " ===");
                config_ = experiments[i];
                
                // Save experiment config
                auto config = std::make_shared<Config>();
                if (config_->loadFromFile(config_.configFile)) {
                    for (const auto& [key, value] : config_.customParams) {
                        config->set(key, value, ConfigSource::CommandLine);
                    }
                    
                    auto brain = std::make_shared<Brain>(config);
                    if (brain->initialize()) {
                        runExperiment(brain);
                        brain->save(config_.outputFile);
                        logInfo("Experiment " << (i + 1) << " completed. Results saved to: " << config_.outputFile);
                    }
                    else {
                        logError("Failed to initialize brain for experiment " << (i + 1));
                    }
                }
                else {
                    logError("Failed to load config file for experiment " << (i + 1));
                }
            }
        }

        std::shared_ptr<Logger> logger_;
        std::shared_ptr<ConsoleLogger> consoleLogger_;
        CLIConfig config_;
    };

} // namespace nlm

int main(int argc, char** argv) {
    nlm::NLMCLI cli;
    
    if (!cli.parseArguments(argc, argv)) {
        return 0;
    }
    
    return cli.run() ? 0 : 1;
}
