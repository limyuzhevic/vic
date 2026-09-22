// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "main.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace nlm {

// Command line interface class for advanced configuration and control
class CommandLineInterface {
public:
    struct Options {
        bool help = false;
        bool version = false;
        std::string configFile = "";
        std::string outputFile = "";
        std::string checkpointFile = "";
        std::string saveFile = "";
        std::string loadFile = "";
        bool listConfig = false;
        bool saveConfig = false;
        bool batchMode = false;
        size_t batchCount = 1;
        size_t stepsPerBatch = 1000;
        bool enableLearning = true;
        bool enableDevelopment = true;
        bool enableNeuromodulation = true;
        bool enableMemory = true;
        bool enablePrediction = true;
        bool enableCognition = true;
        bool enableCuriosity = false;
        bool enableReplay = true;
        bool profiling = false;
        std::string experimentType = "demo";
        size_t neuronCount = 500;
        size_t regionCount = 1;
        double simulationTimestep = 0.001;
        std::string learningRate = "0.001";
        std::string explorationRate = "0.1";
        bool quiet = false;
        bool verbose = false;
    };

    struct Results {
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        double averageFiringRate = 0.0;
        double totalSimulationTime = 0.0;
        bool success = false;
        std::string message;
    };

    CommandLineInterface(int argc, char** argv) {
        parseArguments(argc, argv);
    }

    bool run() {
        if (options.help) {
            printHelp();
            return true;
        }
        
        if (options.version) {
            printVersion();
            return true;
        }
        
        if (options.listConfig) {
            listAvailableConfig();
            return true;
        }
        
        if (options.saveConfig) {
            saveDefaultConfig("configs/default.cfg");
            return true;
        }
        
        return runSimulation();
    }

private:
    Options options;

    void parseArguments(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--list-config") {
                options.listConfig = true;
            } else if (arg == "--save-config") {
                options.saveConfig = true;
            } else if (arg == "--batch") {
                options.batchMode = true;
            } else if (arg == "--enable-learning") {
                options.enableLearning = true;
            } else if (arg == "--disable-learning") {
                options.enableLearning = false;
            } else if (arg == "--enable-development") {
                options.enableDevelopment = true;
            } else if (arg == "--disable-development") {
                options.enableDevelopment = false;
            } else if (arg == "--enable-neuromodulation") {
                options.enableNeuromodulation = true;
            } else if (arg == "--disable-neuromodulation") {
                options.enableNeuromodulation = false;
            } else if (arg == "--enable-curiosity") {
                options.enableCuriosity = true;
            } else if (arg == "--enable-memory") {
                options.enableMemory = true;
            } else if (arg == "--enable-prediction") {
                options.enablePrediction = true;
            } else if (arg == "--enable-cognition") {
                options.enableCognition = true;
            } else if (arg == "--enable-replay") {
                options.enableReplay = true;
            } else if (arg == "--disable-replay") {
                options.enableReplay = false;
            } else if (arg == "--profiling") {
                options.profiling = true;
            } else if (arg == "--quiet") {
                options.quiet = true;
            } else if (arg == "--verbose") {
                options.verbose = true;
            } else if (arg == "--neuron-count" && i + 1 < argc) {
                options.neuronCount = std::stoul(argv[++i]);
            } else if (arg == "--region-count" && i + 1 < argc) {
                options.regionCount = std::stoul(argv[++i]);
            } else if (arg == "--timestep" && i + 1 < argc) {
                options.simulationTimestep = std::stod(argv[++i]);
            } else if (arg == "--batch-count" && i + 1 < argc) {
                options.batchCount = std::stoul(argv[++i]);
            } else if (arg == "--steps-per-batch" && i + 1 < argc) {
                options.stepsPerBatch = std::stoul(argv[++i]);
            } else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
            } else if (arg == "--checkpoint" && i + 1 < argc) {
                options.checkpointFile = argv[++i];
            } else if (arg == "--save" && i + 1 < argc) {
                options.saveFile = argv[++i];
            } else if (arg == "--load" && i + 1 < argc) {
                options.loadFile = argv[++i];
            } else if (arg == "--experiment" && i + 1 < argc) {
                options.experimentType = argv[++i];
            } else if (arg == "--learning-rate" && i + 1 < argc) {
                options.learningRate = argv[++i];
            } else if (arg == "--exploration-rate" && i + 1 < argc) {
                options.explorationRate = argv[++i];
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg.substr(0, 2) == "--") {
                // Handle --key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    setOption(key, value);
                }
            } else {
                if (!options.quiet) {
                    std::cout << "Unknown option: " << arg << std::endl;
                }
            }
        }
    }

    void setOption(const std::string& key, const std::string& value) {
        // Map of option names to member pointers
        static const std::map<std::string, std::function<void(const std::string&)>> setters = {
            {"neuron_count", [this](const std::string& v) { options.neuronCount = std::stoul(v); }},
            {"region_count", [this](const std::string& v) { options.regionCount = std::stoul(v); }},
            {"simulation_timestep", [this](const std::string& v) { options.simulationTimestep = std::stod(v); }},
            {"batch_count", [this](const std::string& v) { options.batchCount = std::stoul(v); }},
            {"steps_per_batch", [this](const std::string& v) { options.stepsPerBatch = std::stoul(v); }},
            {"learning_rate", [this](const std::string& v) { options.learningRate = v; }},
            {"exploration_rate", [this](const std::string& v) { options.explorationRate = v; }},
        };
        
        auto it = setters.find(key);
        if (it != setters.end()) {
            it->second(value);
        }
    }

    bool runSimulation() {
        // Initialize config
        auto config = std::make_shared<Config>();
        
        // Load config from file if specified
        if (!options.configFile.empty()) {
            if (!options.quiet) {
                std::cout << "Loading configuration from: " << options.configFile << std::endl;
            }
            config->loadFromFile(options.configFile);
        }
        
        // Apply command line options
        config->set("brain.neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
        config->set("brain.region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
        config->set("simulation_timestep", options.simulationTimestep, ConfigSource::CommandLine);
        
        // Set experiment-specific configuration
        config->set("experiment.type", options.experimentType, ConfigSource::CommandLine);
        config->set("experiment.learning_rate", std::stod(options.learningRate), ConfigSource::CommandLine);
        config->set("experiment.exploration_rate", std::stod(options.explorationRate), ConfigSource::CommandLine);
        
        // Enable/disable subsystems
        config->set("subsystems.learning.enabled", options.enableLearning, ConfigSource::CommandLine);
        config->set("subsystems.development.enabled", options.enableDevelopment, ConfigSource::CommandLine);
        config->set("subsystems.neuromodulation.enabled", options.enableNeuromodulation, ConfigSource::CommandLine);
        config->set("subsystems.memory.enabled", options.enableMemory, ConfigSource::CommandLine);
        config->set("subsystems.prediction.enabled", options.enablePrediction, ConfigSource::CommandLine);
        config->set("subsystems.cognition.enabled", options.enableCognition, ConfigSource::CommandLine);
        config->set("subsystems.curiosity.enabled", options.enableCuriosity, ConfigSource::CommandLine);
        config->set("subsystems.replay.enabled", options.enableReplay, ConfigSource::CommandLine);
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(options.quiet ? LogLevel::Error : (options.verbose ? LogLevel::Debug : LogLevel::Info));
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        if (!options.quiet) {
            printBanner();
            std::cout << "Starting NLM simulation with command line configuration..." << std::endl;
            std::cout << "===============================================" << std::endl;
        }
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        if (!options.quiet) {
            brain->logStatus();
        }
        
        // Run simulation
        Results results;
        if (options.batchMode) {
            results = runBatchSimulation(brain);
        } else {
            results = runSingleEpisode(brain);
        }
        
        // Save checkpoint if requested
        if (!options.checkpointFile.empty() && options.saveFile.empty()) {
            brain->save(options.checkpointFile);
            if (!options.quiet) {
                std::cout << "Checkpoint saved to: " << options.checkpointFile << std::endl;
            }
        }
        
        // Save final state if requested
        if (!options.saveFile.empty()) {
            brain->save(options.saveFile);
            if (!options.quiet) {
                std::cout << "Final state saved to: " << options.saveFile << std::endl;
            }
        }
        
        // Load state if requested
        if (!options.loadFile.empty()) {
            if (brain->load(options.loadFile)) {
                if (!options.quiet) {
                    std::cout << "State loaded from: " << options.loadFile << std::endl;
                }
                // Re-initialize after loading (if needed)
                brain->reset();
                brain->initialize();
            }
        }
        
        // Save results if requested
        if (!options.outputFile.empty()) {
            saveResults(options.outputFile, results);
            if (!options.quiet) {
                std::cout << "Results saved to: " << options.outputFile << std::endl;
            }
        }
        
        if (!options.quiet) {
            printResults(results);
            std::cout << "===============================================" << std::endl;
            std::cout << "Simulation completed successfully!" << std::endl;
        }
        
        return true;
    }

    Results runSingleEpisode(std::shared_ptr<Brain> brain) {
        Results results;
        
        // Run the episode
        for (SimulationStep step = 0; step < options.stepsPerBatch; ++step) {
            brain->step(step);
            results.totalSteps++;
            results.totalSpikes = brain->getTotalSpikeCount();
            
            // Log progress periodically
            if (options.stepsPerBatch > 100 && step % 100 == 0) {
                if (!options.quiet) {
                    double progress = (double)step / options.stepsPerBatch * 100.0;
                    std::cout << "Progress: " << progress << "% (Step: " << step 
                              << ", Spikes: " << results.totalSpikes << ")" << std::endl;
                }
            }
            
            // Handle checkpointing
            if (!options.checkpointFile.empty() && step % 100 == 0) {
                brain->save(options.checkpointFile + ".step" + std::to_string(step));
            }
        }
        
        results.totalSimulationTime = options.stepsPerBatch * options.simulationTimestep;
        results.averageFiringRate = static_cast<double>(results.totalSpikes) / options.stepsPerBatch;
        results.success = true;
        results.message = "Single episode simulation completed successfully";
        
        return results;
    }

    Results runBatchSimulation(std::shared_ptr<Brain> brain) {
        Results results;
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        
        std::cout << "Running batch simulation with " << options.batchCount << " episodes..." << std::endl;
        
        for (size_t episode = 0; episode < options.batchCount; ++episode) {
            std::cout << "\n--- Episode " << (episode + 1) << "/" << options.batchCount << " ---" << std::endl;
            
            // Reset and reinitialize brain for each episode
            brain->reset();
            brain->initialize();
            
            // Run this episode
            for (SimulationStep step = 0; step < options.stepsPerBatch; ++step) {
                brain->step(step);
                totalSteps++;
                totalSpikes = brain->getTotalSpikeCount();
                
                // Log progress periodically
                if (options.stepsPerBatch > 100 && step % 100 == 0) {
                    std::cout << "  Step " << step << "/" << options.stepsPerBatch 
                              << " (Total Spikes: " << totalSpikes << ")" << std::endl;
                }
            }
            
            // Save checkpoint after each episode
            if (!options.checkpointFile.empty()) {
                std::string checkpointFile = options.checkpointFile + ".episode" + std::to_string(episode + 1);
                brain->save(checkpointFile);
            }
        }
        
        results.totalSteps = totalSteps;
        results.totalSpikes = totalSpikes;
        results.totalSimulationTime = totalSteps * options.simulationTimestep;
        results.averageFiringRate = static_cast<double>(totalSpikes) / totalSteps;
        results.success = true;
        results.message = std::to_string(options.batchCount) + " episode batch simulation completed successfully";
        
        return results;
    }

    void saveResults(const std::string& filename, const Results& results) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to save results to: " << filename << std::endl;
            return;
        }
        
        file << "NLM Simulation Results" << std::endl;
        file << "=======================" << std::endl << std::endl;
        
        file << "Total Steps: " << results.totalSteps << std::endl;
        file << "Total Spikes: " << results.totalSpikes << std::endl;
        file << "Average Firing Rate: " << results.averageFiringRate << " Hz" << std::endl;
        file << "Total Simulation Time: " << results.totalSimulationTime << " seconds" << std::endl;
        file << "Status: " << (results.success ? "SUCCESS" : "FAILED") << std::endl;
        file << "Message: " << results.message << std::endl;
        
        file.close();
    }

    void printResults(const Results& results) {
        std::cout << "\nSimulation Results:" << std::endl;
        std::cout << "-------------------" << std::endl;
        std::cout << "Total Steps: " << results.totalSteps << std::endl;
        std::cout << "Total Spikes: " << results.totalSpikes << std::endl;
        std::cout << "Average Firing Rate: " << results.averageFiringRate << " Hz" << std::endl;
        std::cout << "Total Simulation Time: " << results.totalSimulationTime << " seconds" << std::endl;
        std::cout << "Status: " << (results.success ? "SUCCESS" : "FAILED") << std::endl;
    }

    void printHelp() {
        std::cout << "NLM Command Line Interface" << std::endl;
        std::cout << "===========================" << std::endl << std::endl;
        
        std::cout << "Usage: nlm [options]" << std::endl << std::endl;
        
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  -h, --help                    Show this help message" << std::endl;
        std::cout << "  --version                     Show version information" << std::endl;
        std::cout << "  --list-config                 List available configuration options" << std::endl;
        std::cout << "  --save-config                 Save default configuration to file" << std::endl;
        std::cout << "  --batch                       Enable batch simulation mode" << std::endl;
        std::cout << "  --enable-learning             Enable learning subsystem" << std::endl;
        std::cout << "  --disable-learning            Disable learning subsystem" << std::endl;
        std::cout << "  --enable-development          Enable development subsystem" << std::endl;
        std::cout << "  --disable-development         Disable development subsystem" << std::endl;
        std::cout << "  --enable-neuromodulation      Enable neuromodulation subsystem" << std::endl;
        std::cout << "  --disable-neuromodulation     Disable neuromodulation subsystem" << std::endl;
        std::cout << "  --enable-curiosity           Enable curiosity subsystem" << std::endl;
        std::cout << "  --enable-memory              Enable memory subsystem" << std::endl;
        std::cout << "  --enable-prediction          Enable prediction subsystem" << std::endl;
        std::cout << "  --enable-cognition           Enable cognition subsystem" << std::endl;
        std::cout << "  --enable-replay              Enable replay subsystem" << std::endl;
        std::cout << "  --disable-replay             Disable replay subsystem" << std::endl;
        std::cout << "  --profiling                  Enable profiling mode" << std::endl;
        std::cout << "  --quiet                      Quiet mode (minimal output)" << std::endl;
        std::cout << "  --verbose                    Verbose output mode" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Simulation Options:" << std::endl;
        std::cout << "  --neuron-count N              Number of neurons (default: 500)" << std::endl;
        std::cout << "  --region-count R              Number of regions (default: 1)" << std::endl;
        std::cout << "  --timestep T                  Simulation timestep in seconds (default: 0.001)" << std::endl;
        std::cout << "  --batch-count N               Number of episodes for batch simulation (default: 1)" << std::endl;
        std::cout << "  --steps-per-batch N           Steps per episode (default: 1000)" << std::endl;
        std::cout << "  --experiment TYPE             Experiment type (demo, learning, development, etc.)" << std::endl;
        std::cout << "  --learning-rate RATE          Learning rate (default: 0.001)" << std::endl;
        std::cout << "  --exploration-rate RATE       Exploration rate (default: 0.1)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "File Options:" << std::endl;
        std::cout << "  --checkpoint FILE             Save checkpoints to file" << std::endl;
        std::cout << "  --save FILE                  Save final brain state" << std::endl;
        std::cout << "  --load FILE                  Load brain state from file" << std::endl;
        std::cout << "  --output FILE                Save simulation results" << std::endl;
        std::cout << "  --config FILE                Load configuration from file" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm                                    Run simple demo" << std::endl;
        std::cout << "  nlm --batch --batch-count 10          Run 10 episodes in batch" << std::endl;
        std::cout << "  nlm --neuron-count 1000 --timestep 0.0001  Large network simulation" << std::endl;
        std::cout << "  nlm --experiment learning --enable-replay    Learning experiment with replay" << std::endl;
        std::cout << "  nlm --save brain.bin --load brain.bin    Save and load brain state" << std::endl;
        std::cout << std::endl;
    }

    void printVersion() {
        std::cout << "NLM (Neural Learning Machine) v0.1.0" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << "Build with C++20 support for advanced neural dynamics" << std::endl;
        std::cout << "Python bindings for advanced scripting and batch processing" << std::endl;
    }

    void listAvailableConfig() {
        std::cout << "Available Configuration Options:" << std::endl;
        std::cout << "===============================" << std::endl << std::endl;
        
        std::cout << "Brain Configuration:" << std::endl;
        std::cout << "  brain.neuron_count           Number of neurons (int64_t)" << std::endl;
        std::cout << "  brain.region_count           Number of neural regions (int64_t)" << std::endl;
        std::cout << "  brain.connection_probability Probability of connection between neurons (float)" << std::endl;
        std::cout << "  brain.synapse_density        Synapse connectivity density (float)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Simulation Configuration:" << std::endl;
        std::cout << "  simulation_timestep          Simulation timestep (double)" << std::endl;
        std::cout << "  random_seed                  Random seed for reproducible results (int64_t)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Plasticity Configuration:" << std::endl;
        std::cout << "  plasticity.stdp.enable      Enable STDP plasticity (bool)" << std::endl;
        std::cout << "  plasticity.stdp.learning_rate STDP learning rate (float)" << std::endl;
        std::cout << "  plasticity.hebbian.enable   Enable Hebbian plasticity (bool)" << std::endl;
        std::cout << "  plasticity.structural.enable Enable structural plasticity (bool)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Neuromodulation Configuration:" << std::endl;
        std::cout << "  neuromod.dopamine.scale      Dopamine modulation scale (float)" << std::endl;
        std::cout << "  neuromod.curiosity.enable   Enable curiosity-driven exploration (bool)" << std::endl;
        std::cout << "  neuromod.novelty.enable     Enable novelty detection (bool)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Memory Configuration:" << std::endl;
        std::cout << "  memory.working_memory.size   Working memory capacity (int64_t)" << std::endl;
        std::cout << "  memory.episodic.enabled     Enable episodic memory (bool)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Development Configuration:" << std::endl;
        std::cout << "  development.plasticity.rate Plasticity rate modifier (float)" << std::endl;
        std::cout << "  development.critical_period  Critical period duration (double)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Subsystems Configuration:" << std::endl;
        for (const auto& key : {"learning", "development", "neuromodulation", "memory", "prediction", "cognition", "curiosity", "replay"}) {
            std::cout << "  subsystems." << key << ".enabled           Enable " << key << " subsystem (bool)" << std::endl;
        }
        std::cout << std::endl;
    }

    void saveDefaultConfig(const std::string& filename) {
        std::cout << "Saving default configuration to: " << filename << std::endl;
        
        // Create default config file
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to save configuration to: " << filename << std::endl;
            return;
        }
        
        file << "# NLM Default Configuration" << std::endl;
        file << "# Generated by NLM command line interface" << std::endl;
        file << "# Phase 2: Real Neural Computation" << std::endl;
        file << std::endl;
        
        // Basic brain configuration
        file << "# Brain Configuration" << std::endl;
        file << "brain.neuron_count = 500" << std::endl;
        file << "brain.region_count = 1" << std::endl;
        file << "brain.connection_probability = 0.15" << std::endl;
        file << "brain.synapse_density = 0.1" << std::endl;
        file << std::endl;
        
        // Simulation configuration
        file << "# Simulation Configuration" << std::endl;
        file << "simulation_timestep = 0.001" << std::endl;
        file << "random_seed = 42" << std::endl;
        file << std::endl;
        
        // Plasticity configuration
        file << "# Plasticity Configuration" << std::endl;
        file << "plasticity.stdp.enable = true" << std::endl;
        file << "plasticity.stdp.learning_rate = 0.001" << std::endl;
        file << "plasticity.stdp.tau_plus = 20.0" << std::endl;
        file << "plasticity.stdp.tau_minus = 20.0" << std::endl;
        file << "plasticity.hebbian.enable = true" << std::endl;
        file << "plasticity.structural.enable = true" << std::endl;
        file << "plasticity.synaptogenesis_rate = 0.0001" << std::endl;
        file << "plasticity.pruning_rate = 0.00001" << std::endl;
        file << std::endl;
        
        // Neuromodulation configuration
        file << "# Neuromodulation Configuration" << std::endl;
        file << "neuromod.dopamine.scale = 1.0" << std::endl;
        file << "neuromod.curiosity.enable = true" << std::endl;
        file << "neuromod.novelty.enable = true" << std::endl;
        file << std::endl;
        
        // Memory configuration
        file << "# Memory Configuration" << std::endl;
        file << "memory.working_memory.size = 1000" << std::endl;
        file << "memory.episodic.enabled = true" << std::endl;
        file << std::endl;
        
        // Development configuration
        file << "# Development Configuration" << std::endl;
        file << "development.plasticity.rate = 1.0" << std::endl;
        file << "development.critical_period = 300.0" << std::endl;
        file << std::endl;
        
        // Subsystem configuration
        file << "# Subsystem Configuration" << std::endl;
        for (const auto& key : {"learning", "development", "neuromodulation", "memory", "prediction", "cognition", "curiosity", "replay"}) {
            file << "subsystems." << key << ".enabled = true" << std::endl;
        }
        file << std::endl;
        
        // Experiment configuration
        file << "# Experiment Configuration" << std::endl;
        file << "experiment.type = demo" << std::endl;
        file << "experiment.learning_rate = 0.001" << std::endl;
        file << "experiment.exploration_rate = 0.1" << std::endl;
        file << std::endl;
        
        file << "# End of Configuration" << std::endl;
        
        file.close();
        std::cout << "Configuration saved successfully to: " << filename << std::endl;
    }

    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics            ║
    ║     - Event-driven spike propagation with delays              ║
    ║     - STDP and Hebbian plasticity rules                        ║
    ║     - Structural plasticity                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
};

// Create a header file for the main.cpp
void createMainHeader() {
    std::ofstream file("src/main.h");
    if (!file.is_open()) {
        return;
    }
    
    file << "#pragma once" << std::endl;
    file << std::endl;
    file << "namespace nlm {" << std::endl;
    file << std::endl;
    file << "// Forward declarations for command line interface" << std::endl;
    file << "class CommandLineInterface;" << std::endl;
    file << std::endl;
    file << "// Banner printing function" << std::endl;
    file << "void printBanner();" << std::endl;
    file << std::endl;
    file << "} // namespace nlm" << std::endl;
    
    file.close();
}

int main(int argc, char** argv) {
    // Create the header file for main.cpp (should only be done once)
    createMainHeader();
    
    // Run the command line interface
    CommandLineInterface cli(argc, argv);
    return cli.run() ? 0 : 1;
}

} // namespace nlm
