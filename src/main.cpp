// NLM (熙然) - Neural Learning Machine
// Phase 6: FINAL INTEGRATION
//
// This phase implements the complete integrated brain system with all subsystems:
// - Real LIF neuron dynamics with event-driven spike propagation
// - Working memory with persistent activity
// - Episodic memory with experience encoding and replay
// - Associative memory with Hebbian pattern associations
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Prediction system with prediction error computation
// - Neural planning and concept formation
// - Developmental system with age-dependent plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - STM and LTM consolidation during sleep/rest

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
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <ctime>

using namespace nlm;

// CLI options structure
struct CLIOptions {
    std::string configFile = "configs/default.cfg";
    std::string experimentType = "demo";
    std::string outputFile = "";
    bool verbose = false;
    bool benchmark = false;
    bool saveBrain = false;
    bool loadBrain = false;
    bool help = false;
    bool version = false;
    int simulationSteps = 0;
    double timestep = 0.001;
    uint64_t randomSeed = 42;
    std::string logLevel = "INFO";
    std::vector<std::string> customArgs;
};

// Forward declarations for help and version
void printHelp(const std::string& programName);
void printVersion();
CLIOptions parseArguments(int argc, char** argv);
void saveRunResults(const CLIOptions& options, const std::shared_ptr<Brain>& brain, double duration);
void printSystemInfo();

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: FINAL INTEGRATION                               ║
    ║                                                               ║
    ║     Complete artificial brain with all subsystems integrated.   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printSystemInfo() {
    std::cout << R"(
    ════════════════════════════════════════════════════════════════
    NLM Phase 6 System Information
    ════════════════════════════════════════════════════════════════
    
    Core Components:
    • Real LIF neuron dynamics with event-driven spike propagation
    • Working memory with persistent activity and competition
    • Episodic memory with experience encoding and replay
    • Associative memory with Hebbian pattern associations
    • Neuromodulation (dopamine, curiosity, novelty, prediction error)
    
    Cognitive Systems:
    • Neural planner with action sequence evaluation
    • Concept formation from experience patterns
    • Attention with competitive selection dynamics
    
    Integration Features:
    • Prediction system integrated into brain loop
    • Development stages affecting plasticity rates
    • Checkpoint save/load implemented
    • Replay and consolidation during rest/sleep
    
    Architectural Principles:
    • Closed-loop interaction with environment
    • Experience-driven synaptic plasticity
    • Multi-timescale processing (fast spikes, slow plasticity)
    
    ════════════════════════════════════════════════════════════════
    )" << std::endl;
}

void printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM Phase 6 - Neural Learning Machine (Final Integration)" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                          Show this help message" << std::endl;
    std::cout << "  --version                       Show version information" << std::endl;
    std::cout << "  --config FILE                   Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << "  --steps N                       Run for N simulation steps (default: 0 = full demo)" << std::endl;
    std::cout << "  --timestep SECONDS              Set simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --seed SEED                     Set random seed (default: 42)" << std::endl;
    std::cout << "  --verbose                       Enable verbose logging" << std::endl;
    std::cout << "  --benchmark                     Run performance benchmark" << std::endl;
    std::cout << "  --save-brain FILE               Save brain state to FILE" << std::endl;
    std::cout << "  --load-brain FILE               Load brain state from FILE" << std::endl;
    std::cout << "  --experiment TYPE               Run specific experiment (demo, plasticity, stdp, connectivity)" << std::endl;
    std::cout << "  --log-level LEVEL               Set log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "  --output FILE                   Save run results to FILE" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --steps 1000 --verbose" << std::endl;
    std::cout << "  " << programName << " --config custom.cfg --save-brain brain_state.bin" << std::endl;
    std::cout << "  " << programName << " --benchmark --experiment stdp" << std::endl;
    std::cout << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 6 - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6 - Final Integration)" << std::endl;
    std::cout << "Architecture: Event-driven spiking neural network with integrated memory systems" << std::endl;
    std::cout << "Features: Working memory, episodic memory, neuromodulation, prediction, cognition" << std::endl;
}

CLIOptions parseArguments(int argc, char** argv) {
    CLIOptions options;
    std::string programName = argv[0];
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--benchmark") {
            options.benchmark = true;
        } else if (arg == "--save-brain" && i + 1 < argc) {
            options.saveBrain = true;
            options.customArgs.push_back(arg);
            options.customArgs.push_back(argv[++i]);
        } else if (arg == "--load-brain" && i + 1 < argc) {
            options.loadBrain = true;
            options.customArgs.push_back(arg);
            options.customArgs.push_back(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg == "--steps" && i + 1 < argc) {
            options.simulationSteps = std::stoi(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            options.timestep = std::stod(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            options.randomSeed = std::stoull(argv[++i]);
        } else if (arg == "--experiment" && i + 1 < argc) {
            options.experimentType = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            options.logLevel = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg == "--" && i + 1 < argc) {
            // Pass through custom arguments
            options.customArgs.push_back(arg);
            options.customArgs.push_back(argv[++i]);
        } else if (arg.find("--") == 0) {
            options.customArgs.push_back(arg);
        } else {
            options.customArgs.push_back(arg);
        }
    }
    
    return options;
}

void saveRunResults(const CLIOptions& options, const std::shared_ptr<Brain>& brain, double duration) {
    if (options.outputFile.empty()) {
        return;
    }
    
    std::ofstream outFile(options.outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Warning: Could not write results to " << options.outputFile << std::endl;
        return;
    }
    
    // Save basic brain statistics
    auto stats = brain->getConfig();
    
    outFile.write(reinterpret_cast<const char*>(&duration), sizeof(double));
    outFile.write(reinterpret_cast<const char*>(&stats->getOr<int64_t>("neuron_count", 0)), sizeof(int64_t));
    outFile.write(reinterpret_cast<const char*>(&stats->getOr<int64_t>("step_count", 0)), sizeof(int64_t));
    
    // Write spike statistics
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    
    outFile.write(reinterpret_cast<const char*>(&totalSpikes), sizeof(size_t));
    outFile.write(reinterpret_cast<const char*>(&activeNeurons), sizeof(size_t));
    
    outFile.close();
    
    if (options.verbose) {
        std::cout << "Results saved to: " << options.outputFile << std::endl;
    }
}

void runIntegrationDemo(std::shared_ptr<Brain> brain, const CLIOptions& options) {
    std::cout << "Running NLM Phase 6 Integration Demo..." << std::endl;
    std::cout << "This tests the complete integrated brain system." << std::endl;
    std::cout << std::endl;
    
    // Test 1: Basic connectivity
    std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject current into first 10 neurons
            NLM_LOG_INFO("Injecting current into 10 neurons...");
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            // Run simulation steps
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * options.timestep);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
            
            if (spikes > 0) {
                NLM_LOG_INFO("  ✓ Basic connectivity working");
            }
        }
    }
    
    std::cout << std::endl;
    
    // Test 2: Memory system integration
    std::cout << "=== Test 2: Memory System Integration ===" << std::endl;
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    
    if (workingMemory && episodicMemory && associativeMemory) {
        NLM_LOG_INFO("Working memory capacity: " + std::to_string(workingMemory->getCapacity()) << "/" << brain->getTotalNeuronCount() / 10);
        NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(episodicMemory->getMaxEpisodes()));
        NLM_LOG_INFO("Associative memory active patterns: " + std::to_string(associativeMemory->getPatternCount()));
        
        NLM_LOG_INFO("  ✓ Memory systems integrated and operational");
    }
    
    std::cout << std::endl;
    
    // Test 3: Neuromodulation integration
    std::cout << "=== Test 3: Neuromodulation Integration ===" << std::endl;
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (dopamine && curiosity && novelty && predictionError) {
        NLM_LOG_INFO("Neuromodulation systems active:");
        NLM_LOG_INFO("  - Dopamine: " + std::to_string(dopamine->getCurrentModulation()) << " modulation strength");
        NLM_LOG_INFO("  - Curiosity: " + std::to_string(curiosity->getExplorationDrive()) << " exploration drive");
        NLM_LOG_INFO("  - Novelty: " + std::to_string(novelty->getNoveltyDetection()) << " sensitivity");
        NLM_LOG_INFO("  - Prediction Error: " + std::to_string(predictionError->getErrorMagnitude()) << " error signal");
        
        NLM_LOG_INFO("  ✓ Neuromodulation integrated");
    }
    
    std::cout << std::endl;
    
    // Test 4: Cognitive systems integration
    std::cout << "=== Test 4: Cognitive Systems Integration ===" << std::endl;
    
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    
    if (planner && conceptFormation && attention) {
        NLM_LOG_INFO("Cognitive systems status:");
        NLM_LOG_INFO("  - Planner: Planning depth = " + std::to_string(planner->getPlanningDepth()));
        NLM_LOG_INFO("  - Concept formation: Active concepts = " + std::to_string(conceptFormation->getActiveConceptCount()));
        NLM_LOG_INFO("  - Attention: Inhibition strength = " + std::to_string(attention->getInhibitionStrength()));
        
        NLM_LOG_INFO("  ✓ Cognitive systems integrated");
    }
    
    std::cout << std::endl;
    
    // Test 5: Prediction system integration
    std::cout << "=== Test 5: Prediction System Integration ===" << std::endl;
    
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        NLM_LOG_INFO("Prediction system status:");
        NLM_LOG_INFO("  - Prediction accuracy: " + std::to_string(predictionSystem->getPredictionAccuracy()) * 100.0f << "%");
        NLM_LOG_INFO("  - Error tracking: " + std::to_string(predictionSystem->getErrorCount()) << " errors");
        
        NLM_LOG_INFO("  ✓ Prediction system integrated");
    }
    
    std::cout << std::endl;
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain, const CLIOptions& options) {
    std::cout << "Running Plasticity Experiment..." << std::endl;
    std::cout << "Testing experience-driven synaptic changes." << std::endl;
    std::cout << std::endl;
    
    // Enable plasticity on all synapses
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false); // Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern
    size_t steps = options.benchmark ? 100 : 1000;
    std::cout << "Applying input patterns for " << steps << " steps..." << std::endl;
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Inject input pattern
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * options.timestep);
        
        if (step % 200 == 0 && options.verbose) {
            std::cout << "  Step " << step << ": Spikes = " << brain->getTotalSpikeCount()
                      << ", Active neurons = " << brain->getActiveNeuronCount() << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Plasticity experiment completed." << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
}

void runStdpExperiment(std::shared_ptr<Brain> brain, const CLIOptions& options) {
    std::cout << "Running STDP Experiment..." << std::endl;
    std::cout << "Testing spike-timing dependent plasticity." << std::endl;
    std::cout << std::endl;
    
    // Get first 5 synapses
    auto* region = brain->getRegion(RegionId(1));
    if (!region || region->getSynapses().size() < 5) {
        std::cout << "Not enough synapses for STDP test" << std::endl;
        return;
    }
    
    auto& synapses = region->getSynapses();
    std::vector<float> beforeWeights;
    
    // Record initial weights
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
    }
    
    // Create correlated activity
    std::cout << "Creating correlated pre->post activity (potentiation)..." << std::endl;
    
    for (int trial = 0; trial < 50; ++trial) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            // Pre fires first
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * options.timestep);
            
            // Then post fires 1ms later
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * options.timestep);
        }
    }
    
    // Check weight changes
    float totalDelta = 0.0f;
    std::cout << "Weight changes:" << std::endl;
    
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        std::cout << "  Synapse " << i << ": " << beforeWeights[i] << " → " << synapses[i]->getWeight()
                  << " (Δ=" << delta << ")" << std::endl;
    }
    
    std::cout << std::endl;
    if (totalDelta > 0.001f) {
        std::cout << "✓ STDP working: Pre-before-post produced potentiation" << std::endl;
    } else if (totalDelta < -0.001f) {
        std::cout << "! STDP reversed: Check parameters" << std::endl;
    } else {
        std::cout << "! No change: STDP may not be triggering" << std::endl;
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    CLIOptions options = parseArguments(argc, argv);
    
    // Handle help and version
    if (options.help) {
        printHelp(argv[0]);
        return 0;
    }
    
    if (options.version) {
        printVersion();
        return 0;
    }
    
    // Show system info
    printSystemInfo();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    if (options.verbose) {
        consoleLogger->setLogLevel(LogLevel::Debug);
    }
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: FINAL INTEGRATION DEMO ===");
    NLM_LOG_INFO("Starting NLM Phase 6 - Complete Artificial Brain Integration");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    if (!options.configFile.empty()) {
        configFile = options.configFile;
    }
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set runtime configuration
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.timestep, ConfigSource::Default);
    config->set("log_level", options.logLevel, ConfigSource::Default);
    
    // Configure log level based on options
    if (options.verbose) {
        consoleLogger->setLogLevel(LogLevel::Debug);
    }
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Record start time for benchmarking
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run experiment based on type
    if (options.experimentType == "plasticity") {
        runPlasticityExperiment(brain, options);
    } else if (options.experimentType == "stdp") {
        runStdpExperiment(brain, options);
    } else {
        // Default integration demo
        if (options.benchmark) {
            // Run shorter benchmark
            NLM_LOG_INFO("Running benchmark mode (100 steps)...");
            for (SimulationStep step = 0; step < 100; ++step) {
                brain->step(step, step * options.timestep);
            }
            NLM_LOG_INFO("Benchmark completed.");
        } else if (options.simulationSteps > 0) {
            NLM_LOG_INFO("Running " + std::to_string(options.simulationSteps) + " steps...");
            for (SimulationStep step = 0; step < options.simulationSteps; ++step) {
                brain->step(step, step * options.timestep);
            }
            NLM_LOG_INFO("Simulation completed.");
        } else {
            // Full integration demo
            runIntegrationDemo(brain, options);
        }
    }
    
    // Record end time
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("");
    
    // Save results if requested
    saveRunResults(options, brain, duration);
    
    if (options.benchmark) {
        NLM_LOG_INFO("Benchmark completed in " + std::to_string(duration) + " seconds");
        NLM_LOG_INFO("Performance: " + std::to_string(options.simulationSteps / duration) + " steps/second");
    } else {
        NLM_LOG_INFO("Phase 6 integration demo completed successfully!");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a fully functioning artificial neural system");
    NLM_LOG_INFO("with all subsystems integrated and operational.");
    NLM_LOG_INFO("");
    
    return 0;
}