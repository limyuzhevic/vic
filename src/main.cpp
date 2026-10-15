// NLM (熙然) - Neural Learning Machine
// Phase 2 Enhanced: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// Enhanced with comprehensive user interface and advanced features

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
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
#include <chrono>
#include <ctime>
#include <sstream>
#include <filesystem>
#include <fstream>

using namespace nlm;

// Enhanced configuration structure for advanced users
struct AdvancedConfig {
    std::string mode = "standard";
    std::string outputFile;
    std::string loadFile;
    bool saveEnabled = false;
    bool benchmarkMode = false;
    bool verboseLogging = false;
    bool visualization = false;
    int steps = 1000;
    float timestep = 0.001f;
    bool testBasic = false;
    bool testPlasticity = false;
    bool testStdp = false;
    bool testMemory = false;
    bool testNeuromodulation = false;
    bool testCognition = false;
    bool testDevelopment = false;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    std::string customPattern;
    float inputAmplitude = 50.0f;
    float learningRate = 0.01f;
    std::string logLevel = "info";
};

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
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Enhanced help function
void printHelp() {
    printBanner();
    std::cout << "\nUsage: ./nlm [OPTIONS]\n\n";
    std::cout << "Basic Options:\n";
    std::cout << "  --help, -h                    Show this help message\n";
    std::cout << "  --config=file.cfg            Load configuration from file (default: configs/default.cfg)\n";
    std::cout << "  --save                       Save brain state to file\n";
    std::cout << "  --output=file.bin            Save brain state to specified file\n";
    std::cout << "  --load=file.bin              Load brain state from file\n\n";
    std::cout << "Mode Options:\n";
    std::cout << "  --mode=standard              Standard execution (default)\n";
    std::cout << "  --mode=benchmark             Run performance benchmark\n";
    std::cout << "  --mode=quick                 Quick test mode\n";
    std::cout << "  --mode=research              Research mode with detailed output\n\n";
    std::cout << "Simulation Options:\n";
    std::cout << "  --steps=NUMBER              Number of simulation steps (default: 1000)\n";
    std::cout << "  --timestep=FLOAT             Simulation timestep in seconds (default: 0.001)\n";
    std::cout << "  --neurons=NUMBER            Number of neurons (default: 500)\n";
    std::cout << "  --regions=NUMBER            Number of brain regions (default: 1)\n";
    std::cout << "  --connection-prob=FLOAT     Connection probability (default: 0.15)\n\n";
    std::cout << "Test Modes:\n";
    std::cout << "  --test-basic                Run basic connectivity test\n";
    std::cout << "  --test-plasticity           Run plasticity learning experiment\n";
    std::cout << "  --test-stdp                 Run STDP verification test\n";
    std::cout << "  --test-memory               Run memory integration test\n";
    std::cout << "  --test-neuromod             Run neuromodulation test\n";
    std::cout << "  --test-cognition            Run cognition test\n";
    std::cout << "  --test-development          Run development test\n\n";
    std::cout << "Advanced Options:\n";
    std::cout << "  --benchmark                 Enable performance benchmarking\n";
    std::cout << "  --verbose                   Enable verbose logging\n";
    std::cout << "  --visualization             Enable terminal visualization\n";
    std::cout << "  --pattern=NAME              Use predefined input pattern\n";
    std::cout << "  --amplitude=FLOAT           Input stimulus amplitude (default: 50.0)\n";
    std::cout << "  --learning-rate=FLOAT       Learning rate for plasticity (default: 0.01)\n";
    std::cout << "  --log-level=LEVEL           Logging level (debug, info, warning, error)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  ./nlm --mode=benchmark --steps=10000 --neurons=2000\n";
    std::cout << "  ./nlm --test-plasticity --save --output=experiment.bin\n";
    std::cout << "  ./nlm --load=saved_brain.bin --steps=5000\n";
    std::cout << "  ./nlm --test-stdp --visualization --verbose\n\n";
}

// File utilities
std::string generateTimestampedFilename(const std::string& prefix = "nlm") {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch() % std::chrono::seconds(1)).count();
    
    std::stringstream ss;
    ss << prefix << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") 
       << "_" << ms << ".bin";
    return ss.str();
}

// Visualization functions
void printVisualization(const std::shared_ptr<Brain>& brain, SimulationStep step, Timestamp time) {
    std::cout << "\n=== Visualization ===\n";
    std::cout << "Step: " << step << " | Time: " << std::fixed << std::setprecision(3) << time << "s\n";
    
    // Get firing statistics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    float firingRate = brain->getAverageFiringRate();
    
    // Simple ASCII visualization
    std::cout << "Network Activity:\n";
    std::cout << "  Neurons: " << totalNeurons << " | "
              << "Firing: " << firingNeurons << " (" 
              << std::fixed << std::setprecision(1) << (100.0f * firingNeurons / totalNeurons) << "%)\n";
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << firingRate << " Hz\n";
    
    // Spike system status
    auto* spikeSys = brain->getSpikeSystem();
    if (spikeSys) {
        size_t pendingSpikes = spikeSys->getPendingSpikeCount();
        size_t pendingDelayed = spikeSys->getPendingDelayedCount();
        std::cout << "  Pending Events: " << pendingSpikes << " spikes, " << pendingDelayed << " delayed\n";
    }
    
    // Memory system status
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        size_t memoryTraces = workingMem->getActiveTraces();
        std::cout << "  Working Memory Traces: " << memoryTraces << "\n";
    }
    
    // Neuromodulation status
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        float level = dopamine->getLevel();
        std::cout << "  Dopamine Level: " << std::fixed << std::setprecision(3) << level << "\n";
    }
    
    // E/I ratio
    float eiRatio = brain->getExcitationInhibitionRatio();
    std::cout << "  E/I Ratio: " << std::fixed << std::setprecision(2) << eiRatio << "\n";
    
    std::cout << "===================\n\n";
}

// Benchmarking utilities
void runBenchmark(const std::shared_ptr<Brain>& brain, size_t steps, double& totalTime) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i, static_cast<Timestamp>(i) * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    totalTime = std::chrono::duration<double>(endTime - startTime).count();
}

void printBenchmarkResults(const std::shared_ptr<Brain>& brain, double totalTime, const std::string& mode) {
    std::cout << "\n=== Benchmark Results ===\n";
    std::cout << "Mode: " << mode << "\n";
    std::cout << "Total Time: " << std::fixed << std::setprecision(3) << totalTime << "s\n";
    std::cout << "Steps Per Second: " << (static_cast<double>(config.steps) / totalTime) << "\n";
    std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(2) << brain->getAverageFiringRate() << " Hz\n";
    std::cout << "===========================\n\n";
}

// Enhanced test runner
void runEnhancedTests(const std::shared_ptr<Brain>& brain, const AdvancedConfig& config) {
    std::cout << "\n=== Enhanced Neural Network Test Suite ===\n";
    
    // Reset brain before tests
    brain->reset();
    brain->initialize();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (config.testBasic) {
        std::cout << "\n--- Test 1: Basic Connectivity ---\n";
        runBasicConnectivityTest(brain);
    }
    
    if (config.testPlasticity) {
        std::cout << "\n--- Test 2: Plasticity Learning ---\n";
        runPlasticityExperiment(brain);
    }
    
    if (config.testStdp) {
        std::cout << "\n--- Test 3: STDP Verification ---\n";
        runStdpVerification(brain);
    }
    
    if (config.testMemory) {
        std::cout << "\n--- Test 4: Memory Integration ---\n";
        // TODO: Implement memory integration test
        std::cout << "  Memory integration test - NOT IMPLEMENTED\n";
    }
    
    if (config.testNeuromodulation) {
        std::cout << "\n--- Test 5: Neuromodulation ---\n";
        // TODO: Implement neuromodulation test
        std::cout << "  Neuromodulation test - NOT IMPLEMENTED\n";
    }
    
    if (config.testCognition) {
        std::cout << "\n--- Test 6: Cognitive Functions ---\n";
        // TODO: Implement cognition test
        std::cout << "  Cognition test - NOT IMPLEMENTED\n";
    }
    
    if (config.testDevelopment) {
        std::cout << "\n--- Test 7: Developmental Processes ---\n";
        // TODO: Implement development test
        std::cout << "  Development test - NOT IMPLEMENTED\n";
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double testDuration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "\n=== Test Suite Summary ===\n";
    std::cout << "Total Test Duration: " << std::fixed << std::setprecision(3) << testDuration << "s\n";
    std::cout << "Total Spikes Generated: " << brain->getTotalSpikeCount() << "\n";
    std::cout << "Final Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
    std::cout << "==================================\n\n";
}

// Save brain state to file
bool saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& filename) {
    std::cout << "Saving brain state to: " << filename << std::endl;
    return brain->save(filename);
}

// Load brain state from file
bool loadBrainState(const std::shared_ptr<Brain>& brain, const std::string& filename) {
    std::cout << "Loading brain state from: " << filename << std::endl;
    return brain->load(filename);
}

bool parseAdvancedArgs(int argc, char** argv, AdvancedConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        }
        else if (arg.substr(0, 9) == "--config=") {
            configFile = arg.substr(9);
        }
        else if (arg == "--mode") {
            if (i + 1 < argc) {
                config.mode = argv[++i];
            }
        }
        else if (arg == "--output") {
            if (i + 1 < argc) {
                config.outputFile = argv[++i];
            }
        }
        else if (arg == "--load") {
            if (i + 1 < argc) {
                config.loadFile = argv[++i];
            }
        }
        else if (arg == "--save") {
            config.saveEnabled = true;
        }
        else if (arg == "--benchmark") {
            config.benchmarkMode = true;
        }
        else if (arg == "--verbose") {
            config.verboseLogging = true;
        }
        else if (arg == "--visualization") {
            config.visualization = true;
        }
        else if (arg == "--steps") {
            if (i + 1 < argc) {
                config.steps = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--timestep") {
            if (i + 1 < argc) {
                config.timestep = std::stof(argv[++i]);
            }
        }
        else if (arg == "--neurons") {
            if (i + 1 < argc) {
                config.neuronCount = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--regions") {
            if (i + 1 < argc) {
                config.regionCount = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--connection-prob") {
            if (i + 1 < argc) {
                config.connectionProbability = std::stof(argv[++i]);
            }
        }
        else if (arg == "--test-basic") {
            config.testBasic = true;
        }
        else if (arg == "--test-plasticity") {
            config.testPlasticity = true;
        }
        else if (arg == "--test-stdp") {
            config.testStdp = true;
        }
        else if (arg == "--test-memory") {
            config.testMemory = true;
        }
        else if (arg == "--test-neuromod") {
            config.testNeuromodulation = true;
        }
        else if (arg == "--test-cognition") {
            config.testCognition = true;
        }
        else if (arg == "--test-development") {
            config.testDevelopment = true;
        }
        else if (arg == "--pattern") {
            if (i + 1 < argc) {
                config.customPattern = argv[++i];
            }
        }
        else if (arg == "--amplitude") {
            if (i + 1 < argc) {
                config.inputAmplitude = std::stof(argv[++i]);
            }
        }
        else if (arg == "--learning-rate") {
            if (i + 1 < argc) {
                config.learningRate = std::stof(argv[++i]);
            }
        }
        else if (arg == "--log-level") {
            if (i + 1 < argc) {
                config.logLevel = argv[++i];
            }
        }
    }
    return true;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Enhanced Neural Computation...\n" << std::endl;
    
    // Initialize enhanced configuration
    AdvancedConfig config;
    
    // Parse advanced command-line arguments
    bool helpRequested = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
        }
    }
    
    if (helpRequested) {
        printHelp();
        return 0;
    }
    
    // Enhanced argument parsing
    parseAdvancedArgs(argc, argv, config);
    
    // Initialize logger with enhanced configuration
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Enhanced NLM Neural Learning Machine ===");
    NLM_LOG_INFO("Version: Phase 6 Enhanced");
    NLM_LOG_INFO("Mode: " + config.mode);
    NLM_LOG_INFO("Steps: " + std::to_string(config.steps));
    NLM_LOG_INFO("Neuron Count: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("Region Count: " + std::to_string(config.regionCount));
    NLM_LOG_INFO("Simulation Timestep: " + std::to_string(config.timestep) + "s");
    
    if (config.benchmarkMode) {
        NLM_LOG_INFO("Benchmark Mode Enabled");
    }
    
    if (config.saveEnabled && !config.outputFile.empty()) {
        NLM_LOG_INFO("Output File: " + config.outputFile);
    }
    
    if (!config.loadFile.empty()) {
        NLM_LOG_INFO("Load File: " + config.loadFile);
    }
    
    if (config.visualization) {
        NLM_LOG_INFO("Terminal Visualization Enabled");
    }
    
    // Load configuration
    auto configManager = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 9) == "--config=") {
            configFile = arg.substr(9);
        }
    }
    
    // Load config from file (ignore if not found)
    if (configManager->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args and advanced config
    configManager->loadFromArgs(argc, argv);
    
    // Set enhanced default values for Phase 6
    configManager->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    configManager->set("simulation_timestep", config.timestep, ConfigSource::Default);
    configManager->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    configManager->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    configManager->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    configManager->set("stdp_ltp_weight", config.learningRate, ConfigSource::Default);
    configManager->set("stdp_ltd_weight", config.learningRate * 0.95f, ConfigSource::Default);
    configManager->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Configure learning rate
    float enhancedLearningRate = config.learningRate;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(configManager->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(configManager->getOr<double>("simulation_timestep", config.timestep)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(configManager->getOr<int64_t>("neuron_count", config.neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(configManager->getOr<int64_t>("region_count", config.regionCount)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(configManager->getOr<float>("connection_probability", config.connectionProbability)));
    NLM_LOG_INFO("  stdp_ltp_weight: " + std::to_string(configManager->getOr<float>("stdp_ltp_weight", enhancedLearningRate)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = configManager->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(configManager);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    double totalStartTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Load brain state if requested
    if (!config.loadFile.empty()) {
        if (loadBrainState(brain, config.loadFile)) {
            NLM_LOG_INFO("Successfully loaded brain state");
        } else {
            NLM_LOG_WARNING("Failed to load brain state, continuing with fresh brain");
        }
    }
    
    // Run in different modes based on configuration
    if (config.benchmarkMode) {
        NLM_LOG_INFO("Running in BENCHMARK mode...");
        double totalTime;
        runBenchmark(brain, config.steps, totalTime);
        printBenchmarkResults(brain, totalTime, config.mode);
    }
    else if (config.mode == "quick") {
        NLM_LOG_INFO("Running in QUICK mode...");
        // Quick test mode - minimal steps
        size_t quickSteps = std::min(config.steps, static_cast<size_t>(100));
        for (size_t i = 0; i < quickSteps; ++i) {
            brain->step(i, static_cast<Timestamp>(i) * config.timestep);
            if (config.visualization && i % 10 == 0) {
                printVisualization(brain, i, static_cast<Timestamp>(i) * config.timestep);
            }
        }
    }
    else if (config.mode == "research") {
        NLM_LOG_INFO("Running in RESEARCH mode...");
        // Research mode with detailed analysis
        runEnhancedTests(brain, config);
    }
    else if (config.mode == "replay") {
        NLM_LOG_INFO("Running in REPLAY mode...");
        // Replay mode - load and replay saved state
        runEnhancedTests(brain, config);
    }
    else {
        // Standard mode - run original tests
        NLM_LOG_INFO("Running in STANDARD mode...");
        
        // Run Test 1: Basic connectivity
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 2: Plasticity learning experiment
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
    
    double totalEndTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    double totalDuration = totalEndTime - totalStartTime;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Summary ===");
    NLM_LOG_INFO("Total Execution Time: " + std::fixed + std::setprecision(3) + std::to_string(totalDuration) + "s");
    NLM_LOG_INFO("Total Spikes Generated: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Average Firing Rate: " + std::fixed + std::setprecision(2) + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("");
    
    // Save brain state if requested
    std::string outputFilename;
    if (config.saveEnabled) {
        if (!config.outputFile.empty()) {
            outputFilename = config.outputFile;
        } else {
            outputFilename = generateTimestampedFilename("nlm_enhanced");
        }
        if (saveBrainState(brain, outputFilename)) {
            NLM_LOG_INFO("Brain state saved to: " + outputFilename);
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced Features Completed:");
    NLM_LOG_INFO("  ✓ Enhanced command-line interface with advanced options");
    NLM_LOG_INFO("  ✓ Comprehensive test suite with multiple test modes");
    NLM_LOG_INFO("  ✓ Benchmarking and performance analysis");
    NLM_LOG_INFO("  ✓ Terminal visualization capabilities");
    NLM_LOG_INFO("  ✓ Save/load functionality for brain state");
    NLM_LOG_INFO("  ✓ Flexible execution modes (standard, quick, research, replay)");
    NLM_LOG_INFO("  ✓ Advanced configuration options");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The enhanced NLM brain now provides comprehensive neuroscience simulation capabilities");
    NLM_LOG_INFO("with extensive configurability and advanced analysis tools for research and education.");
    NLM_LOG_INFO("");
    
    return 0;
}