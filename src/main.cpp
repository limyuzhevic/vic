// NLM (熙然) - Neural Learning Machine
// Phase 2+: Enhanced Command-Line Interface and Configuration System
//
// This phase enhances the original with:
// - Flexible command-line argument parsing
// - Configurable experiment modes
// - Progress tracking and status reporting
// - Advanced configuration validation
// - Multiple execution modes (test, run, profile, visualize)

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
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>
#include <csignal>

using namespace nlm;

// Forward declarations
void setupLogging(const Config& config);
void printUsage(const std::string& programName);
void printHeader(const std::string& title);
void runExperimentMode(std::shared_ptr<Config> config);
void runTestMode(std::shared_ptr<Config> config);
void runProfileMode(std::shared_ptr<Config> config);
void runConfigDump(std::shared_ptr<Config> config);

// Global flag for graceful shutdown
volatile bool g_shutdownRequested = false;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully..." << std::endl;
    g_shutdownRequested = true;
}

void setupLogging(const Config& config) {
    auto logger = std::make_shared<Logger>();
    
    // Set log level from config
    LogLevel logLevel = LogLevel::Info;
    if (config.has("log_level")) {
        std::string level = config.getOr<std::string>("log_level", "INFO");
        if (level == "DEBUG") logLevel = LogLevel::Debug;
        else if (level == "WARN") logLevel = LogLevel::Warn;
        else if (level == "ERROR") logLevel = LogLevel::Error;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void printUsage(const std::string& programName) {
    printHeader("NLM Usage");
    std::cout << "\nUSAGE:" << std::endl;
    std::cout << "  " << programName << " [OPTIONS] [MODE]" << std::endl;
    std::cout << "\nMODES:" << std::endl;
    std::cout << "  test          Run unit tests and validation (default)" << std::endl;
    std::cout << "  run           Run neural simulation with current configuration" << std::endl;
    std::cout << "  profile       Run performance profiling" << std::endl;
    std::cout << "  experiment    Run Phase 6 integration experiment" << std::endl;
    std::cout << "  config        Dump and validate configuration" << std::endl;
    std::cout << "\nOPTIONS:" << std::endl;
    std::cout << "  --help, -h    Show this help message" << std::endl;
    std::cout << "  --config FILE  Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << "  --seed NUM     Set random seed for reproducibility" << std::endl;
    std::cout << "  --neurons NUM  Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps NUM    Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --region NUM   Number of regions (default: 1)" << std::endl;
    std::cout << "  --verbose      Enable verbose logging" << std::endl;
    std::cout << "  --quiet        Suppress output (only errors)" << std::endl;
    std::cout << "  --checkpoint   Enable checkpoint save/load testing" << std::endl;
    std::cout << "  --develop      Enable developmental plasticity" << std::endl;
    std::cout << "  --no-plastic   Disable synaptic plasticity" << std::endl;
    std::cout << "\nEXAMPLE USAGE:" << std::endl;
    std::cout << "  " << programName << " run --neurons 1000 --steps 5000 --seed 12345" << std::endl;
    std::cout << "  " << programName << " test --verbose" << std::endl;
    std::cout << "  " << programName << " profile --neurons 2000 --config myconfig.cfg" << std::endl;
    std::cout << "\nFor more information, visit: https://nlm.readthedocs.io" << std::endl;
}

void printHeader(const std::string& title) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced with Improved CLI & Configuration               ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    std::cout << "\n=== " << title << " ===\n" << std::endl;
}

void printProgress(const std::string& label, int current, int total, float progress) {
    int barWidth = 50;
    int pos = static_cast<int>(barWidth * progress);
    
    std::cout << "\r" << std::string(100, ' ') << "\r"; // Clear line
    std::cout << "[" << label << "] ";
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] ";
    std::cout << std::fixed << std::setprecision(1) << (progress * 100) << "% ";
    std::cout << "(" << current << "/" << total << ")";
    std::cout.flush();
}

// Enhanced Learning Experiment with progress tracking
struct EnhancedLearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    bool verbose;
    
    EnhancedLearningExperiment(std::shared_ptr<Brain> b, uint64_t s, bool v = false) 
        : brain(b), seed(s), verbose(v), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        if (verbose) {
            NLM_LOG_INFO("Initial state recorded:");
            NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
            if (!initialWeights.empty()) {
                float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float mean = sum / initialWeights.size();
                NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
            }
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        if (verbose) {
            NLM_LOG_INFO("Final state recorded:");
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            if (!finalWeights.empty()) {
                float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float mean = sum / finalWeights.size();
                NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
            }
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Enhanced Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, bool verbose = false) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 1: Basic Neural Connectivity ===");
    
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, bool verbose = false) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 2: Plasticity Learning Experiment ===");
    
    EnhancedLearningExperiment experiment(brain, 42, verbose);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    int totalSteps = 1000;
    for (SimulationStep step = 0; step < totalSteps && !g_shutdownRequested; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            if (verbose) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
            printProgress("Learning", step, totalSteps, step / static_cast<float>(totalSteps));
        }
    }
    
    if (g_shutdownRequested) {
        NLM_LOG_INFO("Experiment interrupted by user");
        return;
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain, bool verbose = false) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 3: STDP Verification ===");
    
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
        if (verbose) {
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    int totalTrials = 50;
    for (int trial = 0; trial < totalTrials && !g_shutdownRequested; ++trial) {
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
        
        if (trial % 10 == 0 && verbose) {
            printProgress("STDP", trial, totalTrials, trial / static_cast<float>(totalTrials));
        }
    }
    
    if (g_shutdownRequested) {
        NLM_LOG_INFO("STDP test interrupted by user");
        return;
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        if (verbose) {
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
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

void runTestMode(std::shared_ptr<Config> config) {
    printHeader("NLM Test Mode");
    
    int seed = config->getOr<int>("random_seed", 42);
    bool verbose = config->getOr<bool>("verbose", false);
    
    NLM_LOG_INFO("Running NLM unit tests with seed: " + std::to_string(seed));
    
    // Initialize random seed
    auto rng = std::make_shared<RandomGenerator>(seed);
    
    // Run Test 1: Basic connectivity
    {
        auto testConfig = std::make_shared<Config>(*config);
        testConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(testConfig);
        if (brain->initialize()) {
            runBasicConnectivityTest(brain, verbose);
        } else {
            NLM_LOG_ERROR("Failed to initialize brain for connectivity test");
        }
    }
    
    // Reset brain for plasticity experiment
    {
        auto testConfig = std::make_shared<Config>(*config);
        testConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
        testConfig->set("connection_probability", 0.15f, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(testConfig);
        if (brain->initialize()) {
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain, verbose);
        } else {
            NLM_LOG_ERROR("Failed to initialize brain for plasticity test");
        }
    }
    
    // Reset and run STDP verification
    {
        auto testConfig = std::make_shared<Config>(*config);
        testConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(testConfig);
        if (brain->initialize()) {
            brain->reset();
            brain->initialize();
            runStdpVerification(brain, verbose);
        } else {
            NLM_LOG_ERROR("Failed to initialize brain for STDP test");
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== All Tests Complete ===");
}

void runExperimentMode(std::shared_ptr<Config> config) {
    printHeader("NLM Phase 6 Integration Experiment");
    
    int neuronCount = config->getOr<int>("neuron_count", 100);
    int maxSteps = config->getOr<int>("experiment_steps", 5000);
    bool enableDevelopment = config->getOr<bool>("enable_development", true);
    bool enableCheckpointing = config->getOr<bool>("enable_checkpoint", false);
    std::string checkpointPath = config->getOr<std::string>("checkpoint_path", "/tmp/nlm_checkpoint.bin");
    
    NLM_LOG_INFO("Starting Phase 6 integration experiment");
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " +
                std::to_string(maxSteps) + " simulation steps");
    
    Phase6Config phase6Config;
    phase6Config.neuronCount = static_cast<uint32_t>(neuronCount);
    phase6Config.regionCount = 1;
    phase6Config.connectionProbability = 0.15f;
    phase6Config.maxSteps = static_cast<uint64_t>(maxSteps);
    phase6Config.enableDevelopment = enableDevelopment;
    phase6Config.enableCheckpointing = enableCheckpointing;
    phase6Config.checkpointPath = checkpointPath;
    
    Phase6IntegratedExperiment experiment;
    Phase6IntegrationResult result = experiment.run(phase6Config);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Experiment Results ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Wall time: " + std::to_string(result.totalWallClockTime) + "s");
    
    if (result.checkpointingWorks) {
        NLM_LOG_INFO("✓ Checkpoint save/load functionality working");
    } else if (enableCheckpointing) {
        NLM_LOG_WARN("Checkpoint functionality failed");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Integration Verification ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "YES" : "NO"));
    
    if (experiment.verifyIntegration()) {
        NLM_LOG_INFO("✓ Phase 6 integration verification passed");
    } else {
        NLM_LOG_ERROR("✗ Phase 6 integration verification failed");
    }
}

void runProfileMode(std::shared_ptr<Config> config) {
    printHeader("NLM Performance Profile");
    
    int neuronCount = config->getOr<int>("neuron_count", 100);
    int maxSteps = config->getOr<int>("profile_steps", 1000);
    
    NLM_LOG_INFO("Starting performance profiling");
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " +
                std::to_string(maxSteps) + " steps");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto profileConfig = std::make_shared<Config>(*config);
    profileConfig->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
    
    auto brain = std::make_shared<Brain>(profileConfig);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for profiling");
        return;
    }
    
    // Run profile steps
    int totalSteps = maxSteps;
    int stepsCompleted = 0;
    float totalFiringRate = 0.0f;
    
    for (int step = 0; step < totalSteps && !g_shutdownRequested; ++step) {
        // Inject some activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        
        brain->step(step, step * 0.001);
        
        totalFiringRate += brain->getAverageFiringRate();
        stepsCompleted++;
        
        // Progress reporting
        if (step % 100 == 0) {
            printProgress("Profile", step, totalSteps, step / static_cast<float>(totalSteps));
        }
    }
    
    if (g_shutdownRequested) {
        NLM_LOG_INFO("Profile interrupted by user");
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime);
    
    float avgFiringRate = totalFiringRate / stepsCompleted;
    size_t totalSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Profile Results ===");
    NLM_LOG_INFO("Duration: " + std::to_string(duration.count()) + "s");
    NLM_LOG_INFO("Steps per second: " + std::to_string(stepsCompleted / duration.count()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate));
    NLM_LOG_INFO("Total spikes: " + std::to_string(totalSpikes));
    NLM_LOG_INFO("");
    
    // Memory usage information
    NLM_LOG_INFO("=== System Resources ===");
    // Note: In a real implementation, you would get actual memory usage here
    NLM_LOG_INFO("Note: Full system resource monitoring requires platform-specific APIs");
    NLM_LOG_INFO("Consider using platform tools (top, htop, memory_profiler) for complete profiling");
}

void runConfigDump(std::shared_ptr<Config> config) {
    printHeader("NLM Configuration Dump");
    
    NLM_LOG_INFO("Current configuration:");
    NLM_LOG_INFO("===================");
    
    std::vector<std::string> keys = config->getKeys();
    for (const auto& key : keys) {
        auto value = config->get<std::string>(key);
        if (value) {
            NLM_LOG_INFO("  " + key + " = " + *value);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration summary:");
    NLM_LOG_INFO(config->summary());
}

int main(int argc, char** argv) {
    // Setup signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::string programName = "nlm";
    if (argc > 0) {
        programName = argv[0];
    }
    
    // Parse command line arguments
    std::string mode = "test"; // default mode
    std::string configFile = "configs/default.cfg";
    int seed = 42;
    int neuronCount = 500;
    int maxSteps = 1000;
    bool verbose = false;
    bool quiet = false;
    bool enableDevelopment = true;
    bool enableCheckpoint = false;
    bool enablePlasticity = true;
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(programName);
            return 0;
        }
        else if (arg == "test" || arg == "run" || arg == "profile" || arg == "experiment" || arg == "config") {
            mode = arg;
        }
        else if (arg.substr(0, 9) == "--config=") {
            configFile = arg.substr(9);
        }
        else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        }
        else if (arg.substr(0, 8) == "--seed=") {
            seed = std::stoi(arg.substr(8));
        }
        else if (arg == "--seed" && i + 1 < argc) {
            seed = std::stoi(argv[++i]);
        }
        else if (arg.substr(0, 9) == "--neurons=") {
            neuronCount = std::stoi(arg.substr(9));
        }
        else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::stoi(argv[++i]);
        }
        else if (arg.substr(0, 8) == "--steps=") {
            maxSteps = std::stoi(arg.substr(8));
        }
        else if (arg == "--steps" && i + 1 < argc) {
            maxSteps = std::stoi(argv[++i]);
        }
        else if (arg == "--verbose") {
            verbose = true;
        }
        else if (arg == "--quiet") {
            quiet = true;
        }
        else if (arg == "--develop") {
            enableDevelopment = true;
        }
        else if (arg == "--no-plastic") {
            enablePlasticity = false;
        }
        else if (arg == "--checkpoint") {
            enableCheckpoint = true;
        }
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load from file
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
    }
    
    // Override with command line arguments
    config->set("random_seed", seed, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    config->set("max_steps", static_cast<int64_t>(maxSteps), ConfigSource::CommandLine);
    config->set("verbose", verbose, ConfigSource::CommandLine);
    config->set("quiet", quiet, ConfigSource::CommandLine);
    config->set("enable_development", enableDevelopment, ConfigSource::CommandLine);
    config->set("enable_checkpoint", enableCheckpoint, ConfigSource::CommandLine);
    config->set("enable_plasticity", enablePlasticity, ConfigSource::CommandLine);
    
    // Setup logging
    setupLogging(*config);
    
    printHeader("NLM Main");
    
    // Route to appropriate mode
    if (mode == "test") {
        runTestMode(config);
    }
    else if (mode == "run") {
        runExperimentMode(config);
    }
    else if (mode == "profile") {
        runProfileMode(config);
    }
    else if (mode == "experiment") {
        runExperimentMode(config);
    }
    else if (mode == "config") {
        runConfigDump(config);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Complete ===");
    
    return 0;
}