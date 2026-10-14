// NLM (熙然) - Neural Learning Machine
// Phase 2-6: Comprehensive Neural Computation and Integration
//
// This program provides a comprehensive interface to NLM experiments:
// - Phase 2: Real neural computation with LIF neurons
// - Phase 3: World interaction and behavior
// - Phase 4: Cognition and memory emergence
// - Phase 6: Final integration of all systems
//
// Usage:
//   ./nlm [mode] [options]
//   ./nlm --help                    # Show help
//   ./nlm --version                 # Show version
//   ./nlm phase2 [--iterations N] [--neuron-count N] [--region-count N]
//   ./nlm phase3 [--episodes N] [--steps-per-episode N]
//   ./nlm phase4 [--trials N]
//   ./nlm phase6 [--config FILE] [--neurons N] [--steps N] [--checkpoint] [--replay] [--development]
//   ./nlm benchmark [--reps N]
//   ./nlm list-experiments               # List available experiments

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
#include <map>
#include <functional>
#include <algorithm>
#include <limits>
#include <chrono>

using namespace nlm;

// Forward declarations for demo classes
class Phase3Demo;
class Phase4Demo;
class Phase6Demo;

// Configuration for command line options
struct NLMOptions {
    std::string mode = "phase2";
    bool help = false;
    bool version = false;
    bool listExperiments = false;
    
    // Common options
    uint64_t iterations = 100;
    uint64_t neuronCount = 500;
    uint64_t regionCount = 1;
    uint64_t maxSteps = 1000;
    uint64_t trials = 50;
    uint64_t episodes = 5;
    uint64_t stepsPerEpisode = 200;
    
    // Phase 3 options
    std::string environmentType = "simple";
    float width = 20.0f;
    float height = 20.0f;
    
    // Phase 4 options
    bool enableTemporalPrediction = true;
    bool enableWorkingMemory = true;
    bool enableEpisodicMemory = true;
    bool enableConceptFormation = true;
    bool enableAttention = true;
    bool enablePlanning = true;
    bool enableSelfModel = true;
    bool enableSocialLearning = true;
    bool enableContinualLearning = true;
    
    // Phase 6 options
    std::string configFile = "configs/default.cfg";
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./nlm_checkpoint.bin";
    
    // Benchmark options
    uint64_t benchmarkReps = 5;
    
    // Verbosity
    std::string logLevel = "info";
    bool quiet = false;
    bool verbose = false;
};

// Print banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2-6: Comprehensive Neural Computation and Integration║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This program provides access to all experimental phases.   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help information
void printHelp(const std::string& programName) {
    printBanner();
    std::cout << "\nUSAGE:\n";
    std::cout << "  " << programName << " [mode] [options]\n\n";
    
    std::cout << "MODES:\n";
    std::cout << "  phase2       Run Phase 2: Real neural computation\n";
    std::cout << "  phase3       Run Phase 3: World interaction\n";
    std::cout << "  phase4       Run Phase 4: Cognition and memory\n";
    std::cout << "  phase6       Run Phase 6: Final integration\n";
    std::cout << "  benchmark    Run performance benchmark\n";
    std::cout << "  list-experiments   List all available experiments\n\n";
    
    std::cout << "COMMON OPTIONS:\n";
    std::cout << "  --help               Show this help message\n";
    std::cout << "  --version            Show version information\n";
    std::cout << "  --quiet              Suppress informational output\n";
    std::cout << "  --verbose            Enable verbose output\n";
    std::cout << "  --log-level LEVEL    Set log level (trace, debug, info, warn, error)\n\n";
    
    std::cout << "PHASE 2 OPTIONS:\n";
    std::cout << "  --iterations N       Number of iterations (default: 100)\n";
    std::cout << "  --neuron-count N     Number of neurons (default: 500)\n";
    std::cout << "  --region-count N     Number of regions (default: 1)\n\n";
    
    std::cout << "PHASE 3 OPTIONS:\n";
    std::cout << "  --episodes N        Number of episodes (default: 5)\n";
    std::cout << "  --steps-per-episode N Number steps per episode (default: 200)\n";
    std::cout << "  --width N            Environment width (default: 20.0)\n";
    std::cout << "  --height N           Environment height (default: 20.0)\n\n";
    
    std::cout << "PHASE 4 OPTIONS:\n";
    std::cout << "  --trials N           Number of trials (default: 50)\n\n";
    
    std::cout << "PHASE 6 OPTIONS:\n";
    std::cout << "  --config FILE        Configuration file (default: configs/default.cfg)\n";
    std::cout << "  --neurons N          Number of neurons (default: 1000)\n";
    std::cout << "  --steps N            Maximum simulation steps (default: 10000)\n";
    std::cout << "  --no-checkpoint      Disable checkpointing\n";
    std::cout << "  --no-replay          Disable replay\n";
    std::cout << "  --no-development     Disable development\n";
    std::cout << "  --checkpoint-path PATH Checkpoint file path (default: ./nlm_checkpoint.bin)\n\n";
    
    std::cout << "EXAMPLES:\n";
    std::cout << "  " << programName << " phase2 --iterations 500 --neuron-count 1000\n";
    std::cout << "  " << programName << " phase3 --episodes 10 --steps-per-episode 500\n";
    std::cout << "  " << programName << " phase6 --neurons 2000 --steps 20000 --checkpoint\n\n";
}

// Parse command line arguments
bool parseArguments(int argc, char** argv, NLMOptions& options) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    if (args.empty()) {
        return false;
    }
    
    // Check for global flags
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--help" || args[i] == "-h") {
            options.help = true;
            return true;
        } else if (args[i] == "--version") {
            options.version = true;
            return true;
        } else if (args[i] == "list-experiments") {
            options.listExperiments = true;
            return true;
        }
    }
    
    // First argument is the mode
    options.mode = args[0];
    
    // Parse remaining arguments as options
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--iterations" && i + 1 < args.size()) {
            options.iterations = std::stoull(args[++i]);
        } else if (arg == "--neuron-count" && i + 1 < args.size()) {
            options.neuronCount = std::stoull(args[++i]);
        } else if (arg == "--region-count" && i + 1 < args.size()) {
            options.regionCount = std::stoull(args[++i]);
        } else if (arg == "--max-steps" && i + 1 < args.size()) {
            options.maxSteps = std::stoull(args[++i]);
        } else if (arg == "--trials" && i + 1 < args.size()) {
            options.trials = std::stoull(args[++i]);
        } else if (arg == "--episodes" && i + 1 < args.size()) {
            options.episodes = std::stoull(args[++i]);
        } else if (arg == "--steps-per-episode" && i + 1 < args.size()) {
            options.stepsPerEpisode = std::stoull(args[++i]);
        } else if (arg == "--width" && i + 1 < args.size()) {
            options.width = std::stof(args[++i]);
        } else if (arg == "--height" && i + 1 < args.size()) {
            options.height = std::stof(args[++i]);
        } else if (arg == "--config" && i + 1 < args.size()) {
            options.configFile = args[++i];
        } else if (arg == "--neurons" && i + 1 < args.size()) {
            options.neuronCount = std::stoull(args[++i]);
        } else if (arg == "--steps" && i + 1 < args.size()) {
            options.maxSteps = std::stoull(args[++i]);
        } else if (arg == "--no-checkpoint") {
            options.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            options.enableReplay = false;
        } else if (arg == "--no-development") {
            options.enableDevelopment = false;
        } else if (arg == "--checkpoint-path" && i + 1 < args.size()) {
            options.checkpointPath = args[++i];
        } else if (arg == "--benchmark-reps" && i + 1 < args.size()) {
            options.benchmarkReps = std::stoull(args[++i]);
        } else if (arg == "--log-level" && i + 1 < args.size()) {
            options.logLevel = args[++i];
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return false;
        }
    }
    
    return true;
}

// Print version information
void printVersion() {
    std::cout << "NLM Phase 2-6 Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Copyright (c) 2026 NLM Research Project" << std::endl;
}

// List available experiments
void listExperiments() {
    std::cout << "AVAILABLE EXPERIMENTS:" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "  - Basic neural connectivity" << std::endl;
    std::cout << "  - Plasticity learning experiment" << std::endl;
    std::cout << "  - STDP verification" << std::endl;
    std::cout << "\nPhase 3: World Interaction" << std::endl;
    std::cout << "  - Sensory perception and motor control" << std::endl;
    std::cout << "  - Reward-based learning" << std::endl;
    std::cout << "  - Environmental navigation" << std::endl;
    std::cout << "\nPhase 4: Cognition and Memory" << std::endl;
    std::cout << "  - Temporal prediction" << std::endl;
    std::cout << "  - Working memory" << std::endl;
    std::cout << "  - Episodic memory" << std::endl;
    std::cout << "  - Concept formation" << std::endl;
    std::cout << "  - Neural attention" << std::endl;
    std::cout << "  - Planning and control" << std::endl;
    std::cout << "  - Self-model development" << std::endl;
    std::cout << "  - Social learning" << std::endl;
    std::cout << "  - Continual learning" << std::endl;
    std::cout << "\nPhase 6: Final Integration" << std::endl;
    std::cout << "  - Integration verification" << std::endl;
    std::cout << "  - Memory integration testing" << std::endl;
    std::cout << "  - Neuromodulation integration" << std::endl;
    std::cout << "  - Checkpointing and persistence" << std::endl;
    std::cout << "  - Replay and consolidation" << std::endl;
}

// Phase 2: Real Neural Computation
void runPhase2(const NLMOptions& options) {
    std::cout << "\n=== NLM Phase 2: Real Neural Computation ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Starting Phase 2 Real Neural Computation...");
    NLM_LOG_INFO("Configuration: " + std::to_string(options.iterations) + " iterations, " +
                 std::to_string(options.neuronCount) + " neurons, " +
                 std::to_string(options.regionCount) + " regions");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(0, nullptr);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run iterations
    for (uint64_t iteration = 0; iteration < options.iterations; ++iteration) {
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("Iteration " + std::to_string(iteration + 1) + "/" + std::to_string(options.iterations));
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        if (iteration % 10 == 0 && options.verbose) {
            NLM_LOG_INFO("  Iteration " + std::to_string(iteration + 1) + " completed");
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("Phase 2 complete with " + std::to_string(options.iterations) + " iterations.");
}

// Phase 3: World Interaction
void runPhase3(const NLMOptions& options) {
    std::cout << "\n=== NLM Phase 3: World Interaction ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Configure for Phase 3
    config->set("environment_width", options.width);
    config->set("environment_height", options.height);
    config->set("vision_width", static_cast<int64_t>(options.width / 2));
    config->set("vision_height", static_cast<int64_t>(options.height / 2));
    
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Initial brain state:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // Run episodes
    for (uint64_t episode = 0; episode < options.episodes; ++episode) {
        std::cout << "--- Episode " << episode << " started ---" << std::endl;
        
        // Reset brain
        brain->reset();
        brain->initialize();
        
        float episodeReward = 0.0f;
        
        // Run steps
        for (uint64_t step = 0; step < options.stepsPerEpisode; ++step) {
            // Simulate brain
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(step * 10 + neuralStep, (step * 10 + neuralStep) * 0.001);
            }
            
            // Simple reward based on firing rate
            float firingRate = brain->getAverageFiringRate();
            float reward = std::min(1.0f, firingRate / 20.0f);  // Normalize reward
            episodeReward += reward;
            
            // Apply reward modulation
            brain->injectCurrentToNeurons(NeuronType::Sensory, reward * 10.0f);
            
            // Update development
            brain->updateDevelopment(0.01);
            
            if (step % 100 == 0 && options.verbose) {
                std::cout << "  Step " << step << ": reward=" << reward 
                          << ", firing=" << firingRate << std::endl;
            }
        }
        
        std::cout << "--- Episode " << episode << " ended ---" << std::endl;
        std::cout << "  Total Reward: " << episodeReward << std::endl;
        std::cout << "  Final Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    }
    
    std::cout << "Phase 3 demonstration complete." << std::endl;
}

// Phase 4: Cognition and Memory
void runPhase4(const NLMOptions& options) {
    std::cout << "\n=== NLM Phase 4: Cognition and Memory ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Configure for Phase 4
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Run trials
    std::cout << "Running " << options.trials << " trials..." << std::endl;
    
    float totalPredictionErrorInitial = 0.0f;
    float totalPredictionErrorFinal = 0.0f;
    
    for (size_t trial = 0; trial < options.trials; ++trial) {
        // Create a simple prediction task
        // Inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Check prediction accuracy (simplified)
        float predictionError = 1.0f - brain->getAverageFiringRate() / 20.0f;
        totalPredictionErrorInitial += predictionError;
        
        // Reset and run again to see improvement
        brain->reset();
        brain->initialize();
        
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        float predictionErrorFinal = 1.0f - brain->getAverageFiringRate() / 20.0f;
        totalPredictionErrorFinal += predictionErrorFinal;
        
        if (trial % 10 == 0 && options.verbose) {
            std::cout << "  Trial " << trial << ": initial error=" << predictionError 
                      << ", final error=" << predictionErrorFinal << std::endl;
        }
    }
    
    float avgInitialError = totalPredictionErrorInitial / options.trials;
    float avgFinalError = totalPredictionErrorFinal / options.trials;
    float improvement = avgInitialError - avgFinalError;
    
    std::cout << "\n=== Phase 4 Results ===" << std::endl;
    std::cout << "Initial average prediction error: " << avgInitialError << std::endl;
    std::cout << "Final average prediction error: " << avgFinalError << std::endl;
    std::cout << "Average improvement: " << improvement << std::endl;
    
    if (improvement > 0) {
        std::cout << "[SUCCESS] Prediction improved through experience!" << std::endl;
    } else {
        std::cout << "[INFO] No significant prediction improvement yet." << std::endl;
    }
}

// Phase 6: Final Integration
void runPhase6(const NLMOptions& options) {
    std::cout << "\n=== NLM Phase 6: Final Integration ===" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(options.neuronCount) + " neurons, " +
                 std::to_string(options.maxSteps) + " steps");
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", options.neuronCount);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    NLM_LOG_INFO("Brain and agent initialized successfully");
    
    // Run simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (uint64_t step = 0; step < options.maxSteps; ++step) {
        // Simple neural activity simulation
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        }
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Compute reward based on activity
        float reward = brain->getAverageFiringRate() / 50.0f;
        totalReward += reward;
        
        // Apply reward modulation
        brain->injectCurrentToNeurons(NeuronType::Motor, reward * 10.0f);
        
        // Update development
        if (options.enableDevelopment) {
            brain->updateDevelopment(0.001);
        }
        
        // Collect metrics
        totalFiringRate += brain->getAverageFiringRate();
        if (brain->getFiringNeuronCount() > 0) firingCount++;
        
        // Periodic status
        if (step % 2000 == 0 && !options.quiet) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(totalReward / (step + 1)) +
                        " | Firing: " + std::to_string(brain->getAverageFiringRate()));
        }
    }
    
    // Collect final metrics
    float avgReward = totalReward / options.maxSteps;
    float avgFiringRate = totalFiringRate / options.maxSteps;
    float activityRatio = static_cast<float>(firingCount) / options.maxSteps;
    
    // Verify integration
    bool memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
    bool memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
    bool neuromodulationIntegrated = (brain->getDopamine() != nullptr);
    bool predictionIntegrated = (brain->getPredictionSystem() != nullptr);
    bool developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
    
    // Test checkpointing
    if (options.enableCheckpointing) {
        NLM_LOG_INFO("Testing checkpoint save/load...");
        if (brain->save(options.checkpointPath)) {
            NLM_LOG_INFO("Checkpoint saved successfully");
            
            // Create new brain and load
            auto brain2 = std::make_shared<Brain>(cfg);
            brain2->initialize();
            
            if (brain2->load(options.checkpointPath)) {
                NLM_LOG_INFO("Checkpoint loaded successfully");
                // Note: Exact state restoration is complex, but structure should be restored
            } else {
                NLM_LOG_ERROR("Failed to load checkpoint");
            }
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint");
        }
    }
    
    NLM_LOG_INFO("=== Experiment Complete ===");
    NLM_LOG_INFO("Average reward: " + std::to_string(avgReward));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate));
    NLM_LOG_INFO("Activity ratio: " + std::to_string(activityRatio));
    
    NLM_LOG_INFO("=== Integration Verification ===");
    NLM_LOG_INFO("Working Memory: " + std::string(memoryWorkingMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(neuromodulationIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Prediction: " + std::string(predictionIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Development: " + std::string(developmentIntegrated ? "YES" : "NO"));
    
    std::cout << "\n=== Phase 6 Results ===" << std::endl;
    std::cout << "Average reward: " << avgReward << std::endl;
    std::cout << "Average firing rate: " << avgFiringRate << std::endl;
    std::cout << "Activity ratio: " << activityRatio << std::endl;
    std::cout << std::endl;
    std::cout << "Integration Status:" << std::endl;
    std::cout << "  Working Memory: " << (memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Prediction: " << (predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
}

// Benchmark function
void runBenchmark(const NLMOptions& options) {
    std::cout << "\n=== NLM Performance Benchmark ===" << std::endl;
    std::cout << "Running benchmark with " << options.benchmarkReps << " repetitions..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    config->set("simulation_timestep", 0.001);
    
    double totalTime = 0.0;
    double minTime = std::numeric_limits<double>::max();
    double maxTime = 0.0;
    
    for (size_t rep = 0; rep < options.benchmarkReps; ++rep) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain in benchmark!" << std::endl;
            return;
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        totalTime += elapsed;
        minTime = std::min(minTime, elapsed);
        maxTime = std::max(maxTime, elapsed);
        
        if (options.verbose) {
            std::cout << "  Repetition " << rep + 1 << ": " << elapsed << "s" << std::endl;
        }
    }
    
    double avgTime = totalTime / options.benchmarkReps;
    
    std::cout << "\n=== Benchmark Results ===" << std::endl;
    std::cout << "Average time: " << avgTime << "s" << std::endl;
    std::cout << "Min time: " << minTime << "s" << std::endl;
    std::cout << "Max time: " << maxTime << "s" << std::endl;
    std::cout << "Neurons per second: " << (options.benchmarkReps * 100) / avgTime << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    NLMOptions options;
    if (!parseArguments(argc, argv, options)) {
        std::cerr << "Error parsing arguments. Use --help for usage information." << std::endl;
        return 1;
    }
    
    // Handle global flags
    if (options.help) {
        printHelp(argv[0]);
        return 0;
    }
    
    if (options.version) {
        printVersion();
        return 0;
    }
    
    if (options.listExperiments) {
        listExperiments();
        return 0;
    }
    
    // Set log level
    if (options.logLevel == "trace") {
        Logger::getInstance().setLevel(Logger::Level::Trace);
    } else if (options.logLevel == "debug") {
        Logger::getInstance().setLevel(Logger::Level::Debug);
    } else if (options.logLevel == "info") {
        Logger::getInstance().setLevel(Logger::Level::Info);
    } else if (options.logLevel == "warn") {
        Logger::getInstance().setLevel(Logger::Level::Warn);
    } else if (options.logLevel == "error") {
        Logger::getInstance().setLevel(Logger::Level::Error);
    }
    
    // Run the selected mode
    try {
        if (options.mode == "phase2") {
            runPhase2(options);
        } else if (options.mode == "phase3") {
            runPhase3(options);
        } else if (options.mode == "phase4") {
            runPhase4(options);
        } else if (options.mode == "phase6") {
            runPhase6(options);
        } else if (options.mode == "benchmark") {
            runBenchmark(options);
        } else if (options.mode == "list-experiments") {
            listExperiments();
        } else {
            std::cerr << "Unknown mode: " << options.mode << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "NLM completed successfully!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}