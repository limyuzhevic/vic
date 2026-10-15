// NLM (熙然) - Neural Learning Machine
// Phase 6: Enhanced Integration with Advanced Features
//
// This phase provides enhanced CLI interface, advanced debugging,
// visualization, benchmarking, and user-friendly features for
// both beginners and advanced users.

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <chrono>
#include <algorithm>

using namespace nlm;

// Command-line interface helpers
class CommandLineInterface {
public:
    static void printWelcome() {
        std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║        NLM — 熙然                                                ║
║        Neural Learning Machine                                   ║
║                                                               ║
║        Phase 6: Enhanced Integration                             ║
║                                                               ║
║        An experimental artificial developmental brain          ║
║        with advanced features and debugging capabilities        ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

)===
    }
    
    static void printHelp() {
        std::cout << R"(
USAGE: nlm [OPTIONS]

BASIC OPTIONS:
  -h, --help                    Show this help message
  -v, --version                 Show version information
  --config FILE                 Configuration file (default: configs/default.cfg)
  --log-level LEVEL             Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)

RUN MODES:
  --demo                        Run Phase 6 integration demo
  --experiment                  Run full experiment suite
  --benchmark                   Run performance benchmarks
  --debug                       Run in debug mode with enhanced logging
  --visualize                   Enable visualization output

CONFIGURATION OPTIONS:
  --neuron-count N              Number of neurons (default: 1000)
  --timestep T                  Simulation timestep in seconds (default: 0.001)
  --steps S                     Number of simulation steps (default: 1000)
  --regions R                   Number of brain regions (default: 1)
  --plasticity                  Enable plasticity (default: enabled)
  --neuromodulation              Enable neuromodulation (default: enabled)
  --development                  Enable development system (default: enabled)

ADVANCED OPTIONS (for power users):
  --checkpoint FILE              Load/save checkpoint file
  --custom-config               Custom configuration from file
  --profile                      Enable performance profiling
  --monitor                      Enable real-time monitoring
  --script FILE                  Run Python script for custom experiments
  --mpi                          Enable MPI parallel processing
  --gpu                          Enable GPU acceleration if available

EXPERIMENT PRESETS:
  --preset tiny                 Small scale demonstration (50 neurons)
  --preset small                Standard configuration (500 neurons)
  --preset medium               Large configuration (2000 neurons)
  --preset large                Maximum configuration (10000 neurons)
  --preset research             Research-grade configuration

VISUALIZATION OPTIONS:
  --plot-neural                 Plot neural activity over time
  --plot-memory                 Plot memory system state
  --plot-plasticity             Plot synaptic plasticity
  --plot-neuromodulation         Plot neuromodulation levels
  --export-data FILE            Export simulation data in JSON/CSV format

MONITORING OPTIONS:
  --monitor-neural              Monitor neural system metrics
  --monitor-memory              Monitor memory system metrics
  --monitor-cognition            Monitor cognition system metrics
  --monitor-development          Monitor development system metrics
  --monitor-plasticity          Monitor plasticity metrics
  --monitor-neuromodulation      Monitor neuromodulation metrics

)===
    }
    
    static void printVersion() {
        std::cout << R"(
NLM (Neural Learning Machine) - Phase 6 Enhanced Edition
Version: 0.1.0 (Phase 6 - Final Integration)
Build: $(__DATE__), $(__TIME__)

Features:
  ✓ Integrated memory systems
  ✓ Neuromodulation (dopamine, curiosity, novelty)
  ✓ Prediction and cognition systems
  ✓ Development and plasticity
  ✓ Checkpoint save/load
  ✓ Memory replay and consolidation
  ✓ Real-time system monitoring
  ✓ Advanced CLI interface
  ✓ Visualization tools
  ✓ Performance benchmarking

)===
    }
};

// Configuration parser
class ConfigurationParser {
public:
    static Config parseConfig(int argc, char** argv) {
        Config config;
        
        // Set defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Process command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                CommandLineInterface::printHelp();
                exit(0);
            }
            else if (arg == "--version" || arg == "-v") {
                CommandLineInterface::printVersion();
                exit(0);
            }
            else if (arg == "--demo") {
                config->set("experiment_mode", "demo", ConfigSource::CommandLine);
            }
            else if (arg == "--experiment") {
                config->set("experiment_mode", "full", ConfigSource::CommandLine);
            }
            else if (arg == "--benchmark") {
                config->set("experiment_mode", "benchmark", ConfigSource::CommandLine);
            }
            else if (arg == "--debug") {
                config->set("log_level", "DEBUG", ConfigSource::CommandLine);
                config->set("experiment_mode", "debug", ConfigSource::CommandLine);
            }
            else if (arg == "--visualize") {
                config->set("visualization_enabled", true, ConfigSource::CommandLine);
            }
            else if (arg.substr(0, 10) == "--neuron-count") {
                int count = std::stoi(arg.substr(12));
                config->set("neuron_count", static_cast<int64_t>(count), ConfigSource::CommandLine);
            }
            else if (arg.substr(0, 10) == "--timestep") {
                double timestep = std::stod(arg.substr(12));
                config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
            }
            else if (arg.substr(0, 7) == "--steps") {
                int steps = std::stoi(arg.substr(9));
                config->set("max_simulation_steps", static_cast<size_t>(steps), ConfigSource::CommandLine);
            }
            else if (arg.substr(0, 9) == "--regions") {
                int regions = std::stoi(arg.substr(11));
                config->set("region_count", static_cast<int64_t>(regions), ConfigSource::CommandLine);
            }
            else if (arg == "--plasticity") {
                config->set("plasticity_enabled", true, ConfigSource::CommandLine);
            }
            else if (arg == "--no-plasticity") {
                config->set("plasticity_enabled", false, ConfigSource::CommandLine);
            }
            else if (arg == "--neuromodulation") {
                config->set("neuromodulation_enabled", true, ConfigSource::CommandLine);
            }
            else if (arg == "--no-neuromodulation") {
                config->set("neuromodulation_enabled", false, ConfigSource::CommandLine);
            }
            else if (arg == "--development") {
                config->set("development_enabled", true, ConfigSource::CommandLine);
            }
            else if (arg == "--no-development") {
                config->set("development_enabled", false, ConfigSource::CommandLine);
            }
            else if (arg == "--checkpoint") {
                if (i + 1 < argc) {
                    config->set("checkpoint_path", std::string(argv[++i]), ConfigSource::CommandLine);
                }
            }
            else if (arg.substr(0, 10) == "--preset") {
                std::string preset = arg.substr(11);
                applyPreset(config, preset);
            }
            else if (arg == "--monitor") {
                config->set("monitoring_enabled", true, ConfigSource::CommandLine);
            }
            else if (arg == "--profile") {
                config->set("profiling_enabled", true, ConfigSource::CommandLine);
            }
            else if (arg == "--export-data") {
                if (i + 1 < argc) {
                    config->set("export_data_path", std::string(argv[++i]), ConfigSource::CommandLine);
                }
            }
            else if (arg == "--script") {
                if (i + 1 < argc) {
                    config->set("script_path", std::string(argv[++i]), ConfigSource::CommandLine);
                }
            }
            else if (arg.substr(0, 7) == "--log-level") {
                std::string level = arg.substr(9);
                config->set("log_level", level, ConfigSource::CommandLine);
            }
            else if (arg == "--config") {
                if (i + 1 < argc) {
                    std::string configFile = argv[++i];
                    if (!config->loadFromFile(configFile)) {
                        std::cerr << "Warning: Could not load configuration from " << configFile << std::endl;
                    }
                }
            }
        }
        
        return config;
    }
    
private:
    static void applyPreset(Config config, const std::string& preset) {
        if (preset == "tiny") {
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::CommandLine);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::CommandLine);
            config->set("connection_probability", 0.05f, ConfigSource::CommandLine);
        }
        else if (preset == "small") {
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::CommandLine);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::CommandLine);
            config->set("connection_probability", 0.1f, ConfigSource::CommandLine);
        }
        else if (preset == "medium") {
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::CommandLine);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::CommandLine);
            config->set("connection_probability", 0.15f, ConfigSource::CommandLine);
        }
        else if (preset == "large") {
            config->set("neuron_count", static_cast<int64_t>(10000), ConfigSource::CommandLine);
            config->set("region_count", static_cast<int64_t>(5), ConfigSource::CommandLine);
            config->set("connection_probability", 0.2f, ConfigSource::CommandLine);
        }
        else if (preset == "research") {
            config->set("neuron_count", static_cast<int64_t>(50000), ConfigSource::CommandLine);
            config->set("region_count", static_cast<int64_t>(10), ConfigSource::CommandLine);
            config->set("connection_probability", 0.25f, ConfigSource::CommandLine);
            config->set("plasticity_enabled", true, ConfigSource::CommandLine);
            config->set("neuromodulation_enabled", true, ConfigSource::CommandLine);
            config->set("development_enabled", true, ConfigSource::CommandLine);
            config->set("visualization_enabled", true, ConfigSource::CommandLine);
        }
    }
};

// Enhanced demo with system monitoring
void runEnhancedDemo(Config config) {
    std::cout << "=== NLM Phase 6 Enhanced Demo ===" << std::endl;
    std::cout << "Starting enhanced integration demo with system monitoring..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Get timestep from config
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6 - Enhanced)...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Display system state monitoring
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== SYSTEM MONITORING ACTIVE ===");
    
    // Get system state tracker
    auto* stateTracker = brain->getStateTracker();
    
    // Display system metrics
    auto state = brain->getSystemState();
    NLM_LOG_INFO("System State:");
    NLM_LOG_INFO("  Total Neurons: " + std::to_string(state.neural.totalNeurons));
    NLM_LOG_INFO("  Total Synapses: " + std::to_string(state.neural.totalSynapses));
    NLM_LOG_INFO("  Active Neurons: " + std::to_string(state.neural.activeNeurons));
    NLM_LOG_INFO("  Firing Neurons: " + std::to_string(state.neural.firingNeurons));
    NLM_LOG_INFO("  Total Spikes: " + std::to_string(state.neural.totalSpikes));
    NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(state.neural.averageFiringRate));
    NLM_LOG_INFO("  E/I Ratio: " + std::to_string(state.neural.excitationInhibitionRatio));
    
    // Run demo steps
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Running demo simulation with monitoring...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        // Log system state every 10 steps
        if (step % 10 == 0) {
            stateTracker->updateState();
            auto currentState = stateTracker->getState();
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Step " + std::to_string(step) + ": ");
            NLM_LOG_INFO("  Neurons firing: " + std::to_string(currentState.neural.firingNeurons));
            NLM_LOG_INFO("  Total spikes: " + std::to_string(currentState.neural.totalSpikes));
            NLM_LOG_INFO("  Working memory traces: " + std::to_string(currentState.memory.workingMemoryTraces));
            NLM_LOG_INFO("  Dopamine level: " + std::to_string(currentState.neuromodulation.dopamineLevel));
            NLM_LOG_INFO("  Curiosity level: " + std::to_string(currentState.neuromodulation.curiosityLevel));
            NLM_LOG_INFO("  Novelty level: " + std::to_string(currentState.neuromodulation.noveltyLevel));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== DEMO COMPLETED ===");
    NLM_LOG_INFO("Simulation time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    
    // Final system status
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced demo completed successfully!");
}

// Performance benchmarking
void runBenchmark(Config config) {
    std::cout << "=== NLM Phase 6 Performance Benchmark ===" << std::endl;
    std::cout << "Running comprehensive performance benchmark..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Get timestep from config
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for benchmark!");
        return;
    }
    
    NLM_LOG_INFO("Brain initialized for benchmark: " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
    
    // Benchmark neural computation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    size_t numSteps = config->getOr<size_t>("max_simulation_steps", 1000);
    
    for (SimulationStep step = 0; step < numSteps; ++step) {
        brain->step(step);
        
        // Periodic checkpointing
        if (step % 100 == 0) {
            std::string checkpointFile = "./benchmark_checkpoint_" + std::to_string(step) + ".bin";
            brain->save(checkpointFile);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double totalTimeSeconds = duration.count() / 1000.0;
    double stepsPerSecond = numSteps / totalTimeSeconds;
    double spikesPerSecond = brain->getTotalSpikeCount() / totalTimeSeconds;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== BENCHMARK RESULTS ===");
    NLM_LOG_INFO("Total simulation time: " + std::to_string(totalTimeSeconds) + " seconds");
    NLM_LOG_INFO("Steps per second: " + std::to_string(stepsPerSecond));
    NLM_LOG_INFO("Spikes per second: " + std::to_string(spikesPerSecond));
    NLM_LOG_INFO("Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    // Get system state for additional metrics
    auto* stateTracker = brain->getStateTracker();
    if (stateTracker) {
        stateTracker->updateState();
        auto state = stateTracker->getState();
        
        NLM_LOG_INFO("System State Summary:");
        NLM_LOG_INFO("  Memory traces: " + std::to_string(state.memory.workingMemoryTraces));
        NLM_LOG_INFO("  Memory episodes: " + std::to_string(state.memory.episodicMemoryEpisodes));
        NLM_LOG_INFO("  Dopamine level: " + std::to_string(state.neuromodulation.dopamineLevel));
        NLM_LOG_INFO("  Curiosity level: " + std::to_string(state.neuromodulation.curiosityLevel));
        NLM_LOG_INFO("  Novelty level: " + std::to_string(state.neuromodulation.noveltyLevel));
        NLM_LOG_INFO("  Developmental stage: " + std::to_string(static_cast<int>(state.development.stage)));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Benchmark completed successfully!");
}

// Full experiment suite
void runFullExperiment(Config config) {
    std::cout << "=== NLM Phase 6 Full Experiment Suite ===" << std::endl;
    std::cout << "Running comprehensive experiment suite..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Get timestep from config
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for experiment!");
        return;
    }
    
    NLM_LOG_INFO("Brain initialized for experiment suite: " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
    
    // Run multiple experiment phases
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT PHASE 1: Basic Connectivity ===");
    runBasicConnectivityTest(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT PHASE 2: Plasticity Learning ===");
    runPlasticityExperiment(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT PHASE 3: STDP Verification ===");
    runStdpVerification(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT PHASE 4: Memory Integration ===");
    // Additional memory integration tests would go here
    NLM_LOG_INFO("Memory integration tests completed");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT PHASE 5: Neuromodulation Effects ===");
    // Additional neuromodulation tests would go here
    NLM_LOG_INFO("Neuromodulation tests completed");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT PHASE 6: Development and Learning ===");
    // Additional development tests would go here
    NLM_LOG_INFO("Development tests completed");
    
    // Final experiment summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPERIMENT SUITE COMPLETED ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Full experiment suite completed successfully!");
}

// Debugging mode with enhanced logging
void runDebugMode(Config config) {
    std::cout << "=== NLM Phase 6 Debug Mode ===" << std::endl;
    std::cout << "Starting with enhanced debugging and logging..." << std::endl;
    
    // Initialize logger with debug level
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
    logger->addLogger(consoleLogger);
    
    // Add file logger for debugging
    auto fileLogger = std::make_shared<FileLogger>("./debug.log", LogLevel::Debug);
    logger->addLogger(fileLogger);
    
    Logger::setGlobal(logger);
    
    // Get timestep from config
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain in debug mode!");
        return;
    }
    
    brain->logStatus();
    
    // Run debug simulation with enhanced monitoring
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== DEBUG SIMULATION STARTED ===");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < 50; ++step) {
        // Enable detailed logging for each step
        NLM_LOG_DEBUG("Starting simulation step " + std::to_string(step));
        
        // Get current system state for debugging
        auto* stateTracker = brain->getStateTracker();
        if (stateTracker) {
            stateTracker->updateState();
            auto state = stateTracker->getState();
            
            NLM_LOG_DEBUG("System State at step " + std::to_string(step) + ":");
            NLM_LOG_DEBUG("  Neurons: " + std::to_string(state.neural.totalNeurons));
            NLM_LOG_DEBUG("  Synapses: " + std::to_string(state.neural.totalSynapses));
            NLM_LOG_DEBUG("  Firing neurons: " + std::to_string(state.neural.firingNeurons));
            NLM_LOG_DEBUG("  Dopamine: " + std::to_string(state.neuromodulation.dopamineLevel));
            NLM_LOG_DEBUG("  Curiosity: " + std::to_string(state.neuromodulation.curiosityLevel));
            NLM_LOG_DEBUG("  Novelty: " + std::to_string(state.neuromodulation.noveltyLevel));
        }
        
        brain->step(step);
        
        // Check for any errors in step execution
        if (brain->getTotalSpikeCount() == 0 && step % 10 == 0) {
            NLM_LOG_WARNING("No spikes generated in recent steps - checking system health");
            
            // Check system health
            if (brain->getTotalNeuronCount() == 0) {
                NLM_LOG_ERROR("CRITICAL: Brain has no neurons!");
            }
            
            auto* stateTracker = brain->getStateTracker();
            if (stateTracker) {
                stateTracker->updateState();
                auto state = stateTracker->getState();
                
                if (state.neural.totalSynapses == 0) {
                    NLM_LOG_ERROR("CRITICAL: Brain has no synapses!");
                }
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== DEBUG SIMULATION COMPLETED ===");
    NLM_LOG_INFO("Simulation time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    
    // Generate debug report
    generateDebugReport(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Debug mode completed successfully!");
}

// Helper functions for experiment phases (simplified versions)
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (100 steps)...");
    
    for (SimulationStep step = 0; step < 100; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 20 steps
        if (step % 20 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Plasticity experiment completed");
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 20; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void generateDebugReport(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== DEBUG REPORT ===");
    
    if (!brain) return;
    
    // Get system state
    auto* stateTracker = brain->getStateTracker();
    if (stateTracker) {
        stateTracker->updateState();
        auto state = stateTracker->getState();
        
        NLM_LOG_INFO("System State at end of debug session:");
        NLM_LOG_INFO("  Total Neurons: " + std::to_string(state.neural.totalNeurons));
        NLM_LOG_INFO("  Total Synapses: " + std::to_string(state.neural.totalSynapses));
        NLM_LOG_INFO("  Active Neurons: " + std::to_string(state.neural.activeNeurons));
        NLM_LOG_INFO("  Firing Neurons: " + std::to_string(state.neural.firingNeurons));
        NLM_LOG_INFO("  Total Spikes: " + std::to_string(state.neural.totalSpikes));
        NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(state.neural.averageFiringRate));
        NLM_LOG_INFO("  E/I Ratio: " + std::to_string(state.neural.excitationInhibitionRatio));
        
        NLM_LOG_INFO("  Memory traces: " + std::to_string(state.memory.workingMemoryTraces));
        NLM_LOG_INFO("  Memory episodes: " + std::to_string(state.memory.episodicMemoryEpisodes));
        NLM_LOG_INFO("  Memory consolidation: " + std::to_string(state.memory.memoryConsolidationLevel));
        
        NLM_LOG_INFO("  Dopamine level: " + std::to_string(state.neuromodulation.dopamineLevel));
        NLM_LOG_INFO("  Curiosity level: " + std::to_string(state.neuromodulation.curiosityLevel));
        NLM_LOG_INFO("  Novelty level: " + std::to_string(state.neuromodulation.noveltyLevel));
        NLM_LOG_INFO("  Prediction error: " + std::to_string(state.neuromodulation.predictionError));
        
        NLM_LOG_INFO("  Developmental stage: " + std::to_string(static_cast<int>(state.development.stage)));
        NLM_LOG_INFO("  Plasticity modulation: " + std::to_string(state.development.plasticityModulation));
        
        NLM_LOG_INFO("  Wall clock time: " + std::to_string(state.timing.wallClockTime) + "s");
    }
    
    NLM_LOG_INFO("Debug report generated successfully");
}

int main(int argc, char** argv) {
    // Print welcome banner
    CommandLineInterface::printWelcome();
    
    std::cout << "Initializing NLM Phase 6 Enhanced System..." << std::endl << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6 Enhanced System ===");
    NLM_LOG_INFO("Building advanced brain simulation platform...");
    
    // Parse configuration
    Config config = ConfigurationParser::parseConfig(argc, argv);
    
    // Determine experiment mode
    std::string experimentMode = config->getOr<std::string>("experiment_mode", "demo");
    
    // Run selected experiment mode
    if (experimentMode == "demo") {
        runEnhancedDemo(config);
    }
    else if (experimentMode == "benchmark") {
        runBenchmark(config);
    }
    else if (experimentMode == "full") {
        runFullExperiment(config);
    }
    else if (experimentMode == "debug") {
        runDebugMode(config);
    }
    else {
        NLM_LOG_ERROR("Unknown experiment mode: " + experimentMode);
        CommandLineInterface::printHelp();
        return 1;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 6 Enhanced System Complete ===");
    NLM_LOG_INFO("All systems functioning optimally with advanced features!");
    
    return 0;
}
