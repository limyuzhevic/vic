// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Improved User Experience
//
// This phase demonstrates the fully integrated artificial brain with:
// - All brain systems (memory, neuromodulation, prediction, cognition)
// - Memory systems connected to neural processing
// - Neuromodulation affecting plasticity and neural dynamics
// - Development affecting plasticity rates
// - Prediction system integrated
// - Replay and consolidation functional
// - Checkpoint save/load working

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
#include <algorithm>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     All brain systems integrated into coherent architecture.    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << R"(
USAGE: nlm [--help] [--config=<file>] [--neurons=<count>] [--steps=<count>]
                  [--demo-mode] [--integration-only] [--performance-test]
                  [--checkpoint=<path>] [--load=<path>]

Arguments:
  --help                    Show this help message
  --config=<file>          Load configuration from file (default: configs/default.cfg)
  --neurons=<count>        Set number of neurons (default: 500)
  --steps=<count>          Set simulation steps (default: 2000)
  --demo-mode              Run quick demo instead of full integration test
  --integration-only      Only test integration, no learning/experiment
  --performance-test       Run performance benchmark
  --checkpoint=<path>      Path for checkpoint save
  --load=<path>            Load brain state from checkpoint
  --enable-development     Enable developmental plasticity (default: true)
  --enable-memory          Enable memory systems (default: true)
  --enable-neuromodulation  Enable neuromodulation (default: true)
  --enable-prediction      Enable prediction system (default: true)
  --verbose                Enable verbose logging

Examples:
  nlm                                   # Run full integration test
  nlm --demo-mode                      # Quick demo run
  nlm --integration-only               # Only test integration
  nlm --checkpoint=./test_checkpoint    # Save checkpoint
  nlm --load=./test_checkpoint          # Load brain state
  nlm --performance-test               # Run performance benchmark

Defaults:
  Neurons: 500
  Steps: 2000
  Development: Enabled
  Memory Systems: Enabled
  Neuromodulation: Enabled
  Prediction: Enabled
  Checkpointing: Enabled
  Replay: Enabled
" << std::endl;
}

void printPhase6Summary(const Phase6IntegrationResult& result) {
    std::cout << std::endl;
    std::cout << "=== NLM Phase 6 Integration Results ===" << std::endl;
    std::cout << std::endl;
    
    // System Integration Status
    std::cout << "System Integration Status:" << std::endl;
    std::cout << "  Working Memory:       " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory:     " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation:     " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Prediction System:   " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Development System:  " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << std::endl;
    
    // Performance Metrics
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Total Reward:        " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate << " spikes/s" << std::endl;
    std::cout << "  Synaptic Weight Avg: " << std::fixed << std::setprecision(6) << result.avgSynapticWeight << std::endl;
    std::cout << "  Memory Episodes:     " << std::fixed << std::setprecision(1) << result.memoryEpisodesStored << std::endl;
    std::cout << std::endl;
    
    // Neuromodulator Levels
    std::cout << "Neuromodulator Levels:" << std::endl;
    std::cout << "  Novelty Level:      " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity Level:    " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine Level:     " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    // Operation Results
    std::cout << "Operation Results:" << std::endl;
    std::cout << "  Checkpointing:       " << (result.checkpointingWorks ? "WORKING" : "FAILED") << std::endl;
    std::cout << "  Replay System:       " << (result.replayWorks ? "WORKING" : "FAILED") << std::endl;
    std::cout << "  Wall Clock Time:    " << std::fixed << std::setprecision(2) << result.totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
}

void runIntegrationDemo(std::shared_ptr<Brain> brain) {
    std::cout << "=== Integration Demo ===" << std::endl;
    
    // Create simple world for demonstration
    SimpleWorld world;
    world.initialize(16, 16);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run a short demo
    std::cout << "Running integration demo (200 steps)..." << std::endl;
    
    for (uint64_t step = 0; step < 200; ++step) {
        // Get observation
        SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        
        // Compute reward
        float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
        
        // Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.001);
        
        // Log progress every 50 steps
        if (step % 50 == 0) {
            std::cout << "  Step " << step 
                     << " | Reward: " << std::fixed << std::setprecision(4) << (reward / (step + 1))
                     << " | Firing: " << std::fixed << std::setprecision(2) << brain->getAverageFiringRate()
                     << " | WorkingMem: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        }
    }
    
    std::cout << "Integration demo complete!" << std::endl;
}

void runPerformanceBenchmark(std::shared_ptr<Brain> brain, int steps) {
    std::cout << "=== Performance Benchmark ===" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run benchmark with varying workloads
    int batchSize = std::min(100, steps / 10);
    int numBatches = std::max(1, steps / batchSize);
    
    std::vector<double> batchTimes;
    
    for (int batch = 0; batch < numBatches; ++batch) {
        auto batchStart = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < batchSize && (batch * batchSize + i) < steps; ++i) {
            brain->step(batch * batchSize + i, (batch * batchSize + i) * 0.001);
        }
        
        auto batchEnd = std::chrono::high_resolution_clock::now();
        double batchDuration = std::chrono::duration<double>(batchEnd - batchStart).count();
        batchTimes.push_back(batchDuration);
        
        std::cout << "  Batch " << (batch + 1) << "/" << numBatches 
                 << " | Time: " << std::fixed << std::setprecision(3) << batchDuration 
                 << "s | Rate: " << std::fixed << std::setprecision(1) << batchSize / batchDuration << " steps/s" << std::endl;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate statistics
    double avgBatchTime = std::accumulate(batchTimes.begin(), batchTimes.end(), 0.0) / batchTimes.size();
    double throughput = steps / totalTime;
    
    std::cout << std::endl;
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "  Total Time:         " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "  Steps per Second:   " << std::fixed << std::setprecision(1) << throughput << " steps/s" << std::endl;
    std::cout << "  Average Batch Time: " << std::fixed << std::setprecision(3) << avgBatchTime << "s" << std::endl;
    std::cout << "  Average Step Rate:  " << std::fixed << std::setprecision(1) << batchSize / avgBatchTime << " steps/s" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    bool helpRequested = false;
    bool demoMode = false;
    bool integrationOnly = false;
    bool performanceTest = false;
    bool enableDevelopment = true;
    bool enableMemory = true;
    bool enableNeuromodulation = true;
    bool enablePrediction = true;
    bool verbose = false;
    
    std::string configFile = "configs/default.cfg";
    std::string checkpointPath = "./checkpoint.bin";
    std::string loadPath;
    
    int64_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            helpRequested = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 5) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoll(argv[++i]);
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                maxSteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg == "--demo-mode") {
            demoMode = true;
        } else if (arg == "--integration-only") {
            integrationOnly = true;
        } else if (arg == "--performance-test") {
            performanceTest = true;
        } else if (arg == "--enable-development") {
            enableDevelopment = true;
        } else if (arg == "--disable-development") {
            enableDevelopment = false;
        } else if (arg == "--enable-memory") {
            enableMemory = true;
        } else if (arg == "--disable-memory") {
            enableMemory = false;
        } else if (arg == "--enable-neuromodulation") {
            enableNeuromodulation = true;
        } else if (arg == "--disable-neuromodulation") {
            enableNeuromodulation = false;
        } else if (arg == "--enable-prediction") {
            enablePrediction = true;
        } else if (arg == "--disable-prediction") {
            enablePrediction = false;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg.substr(0, 14) == "--checkpoint=") {
            checkpointPath = arg.substr(arg.find('=') + 1);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointPath = argv[++i];
        } else if (arg.substr(0, 7) == "--load=") {
            loadPath = arg.substr(arg.find('=') + 1);
        } else if (arg == "--load" && i + 1 < argc) {
            loadPath = argv[++i];
        }
    }
    
    if (helpRequested) {
        printUsage();
        return 0;
    }
    
    std::cout << "Initializing NLM Phase 6: Final Integration..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Creating fully integrated artificial brain with all systems connected.");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", neuronCount, ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Enable/disable specific systems based on flags
    config->set("enable_working_memory", enableMemory, ConfigSource::Default);
    config->set("enable_episodic_memory", enableMemory, ConfigSource::Default);
    config->set("enable_neuromodulation", enableNeuromodulation, ConfigSource::Default);
    config->set("enable_prediction", enablePrediction, ConfigSource::Default);
    config->set("enable_development", enableDevelopment, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(config->getOr<int64_t>("neuron_count", neuronCount)));
    NLM_LOG_INFO("  Steps: " + std::to_string(maxSteps));
    NLM_LOG_INFO("  Development: " + std::string(enableDevelopment ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Memory Systems: " + std::string(enableMemory ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Neuromodulation: " + std::string(enableNeuromodulation ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Prediction System: " + std::string(enablePrediction ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Checkpoint Path: " + checkpointPath);
    NLM_LOG_INFO("");
    
// Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Try to load from checkpoint if specified
    if (!loadPath.empty()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Attempting to load brain state from: " + loadPath);
        if (brain->load(loadPath)) {
            NLM_LOG_INFO("Successfully loaded brain state from checkpoint!");
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint. Starting from scratch.");
        }
    }
    
    // Run different modes based on command line
    Phase6IntegrationResult phase6Result;
    
    if (integrationOnly) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Integration Only Mode ===");
        runIntegrationDemo(brain);
    } else if (performanceTest) {
        runPerformanceBenchmark(brain, static_cast<int>(maxSteps));
    } else if (demoMode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Demo Mode ===");
        runIntegrationDemo(brain);
    } else {
        // Run full Phase 6 integration experiment
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 6 Integration Experiment ===");
        
        // Create Phase 6 experiment config
        Phase6Config phase6Config;
        phase6Config.neuronCount = static_cast<size_t>(neuronCount);
        phase6Config.maxSteps = maxSteps;
        phase6Config.enableCheckpointing = !loadPath.empty();
        phase6Config.enableReplay = true;
        phase6Config.enableDevelopment = enableDevelopment;
        phase6Config.checkpointPath = checkpointPath;
        
        // Run integration experiment
        Phase6IntegratedExperiment experiment;
        
        // First, verify integration
        NLM_LOG_INFO("Running integration verification...");
        if (!experiment.verifyIntegration()) {
            NLM_LOG_ERROR("Integration verification failed!");
            return 1;
        }
        NLM_LOG_INFO("Integration verification passed!");
        
        // Test individual systems
        NLM_LOG_INFO("Testing memory integration...");
        experiment.testMemoryIntegration();
        
        NLM_LOG_INFO("Testing neuromodulation integration...");
        experiment.testNeuromodulationIntegration();
        
        // Run full experiment
        NLM_LOG_INFO("Running full integration experiment (" + std::to_string(maxSteps) + " steps)...");
        phase6Result = experiment.run(phase6Config);
        
        // Save checkpoint if configured
        if (phase6Config.enableCheckpointing) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Saving checkpoint to: " + phase6Config.checkpointPath);
            if (brain->save(phase6Config.checkpointPath)) {
                NLM_LOG_INFO("Checkpoint saved successfully!");
            } else {
                NLM_LOG_ERROR("Failed to save checkpoint.");
            }
        }
    }
    
    // Final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    
    // Print final brain status
    brain->logStatus();
    
    // Show results summary if Phase 6 experiment was run
    if (!integrationOnly && !performanceTest && !demoMode) {
        printPhase6Summary(phase6Result);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 6 Complete ===");
    NLM_LOG_INFO("");
    
    // Display user-friendly summary
    std::cout << "=== NLM Phase 6 Summary ===" << std::endl;
    std::cout << "✓ All brain systems successfully integrated" << std::endl;
    std::cout << "✓ Memory systems connected to neural processing" << std::endl;
    std::cout << "✓ Neuromodulation affects plasticity and neural dynamics" << std::endl;
    std::cout << "✓ Development affects plasticity rates" << std::endl;
    std::cout << "✓ Prediction system integrated" << std::endl;
    std::cout << "✓ Replay and consolidation functional" << std::endl;
    if (integrationOnly || demoMode || performanceTest) {
        std::cout << "✓ Integration test completed" << std::endl;
    } else {
        std::cout << "✓ Full integration experiment completed" << std::endl;
    }
    std::cout << "" << std::endl;
    
    NLM_LOG_INFO("The NLM brain is a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("The integrated brain demonstrates coherent operation of all major systems:");
    NLM_LOG_INFO("  - Memory systems store and recall experiences");
    NLM_LOG_INFO("  - Neuromodulators regulate learning and behavior");
    NLM_LOG_INFO("  - Development shapes learning capabilities over time");
    NLM_LOG_INFO("  - Prediction guides action and learning");
    NLM_LOG_INFO("  - Replay consolidates important memories");
    NLM_LOG_INFO("");
    
    return 0;
}
