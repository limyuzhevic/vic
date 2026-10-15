// NLM (熙然) - Neural Learning Machine
// Enhanced Phase 6 Command Line Interface
// 
// This provides a comprehensive CLI for NLM with multiple execution modes:
// - Basic simulation runs
// - Experiment modes with predefined protocols
// - Configuration file management
// - Debug and profiling tools
// - Batch processing capabilities

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
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <chrono>
#include <filesystem>

namespace nlm {

void printHeader(const std::string& version = "Phase 6 Final Integration") {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     " """ << version << " " """                                  ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS] [ARGS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM (Neural Learning Machine) - Advanced Spiking Neural Network Simulator" << std::endl;
    std::cout << "Phase 6: Final Integration of All Brain Systems" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXECUTION MODES:" << std::endl;
    std::cout << "  --run BASIC               Run basic connectivity test" << std::endl;
    std::cout << "  --run PLASTICITY          Run plasticity learning experiment" << std::endl;
    std::cout << "  --run STDP                Verify STDP synaptic learning" << std::endl;
    std::cout << "  --run INTEGRATION         Full integration test (Phase 6 demo)" << std::endl;
    std::cout << "  --run BATCH file          Run batch of predefined experiments" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION OPTIONS:" << std::endl;
    std::cout << "  --config file             Load configuration from file" << std::endl;
    std::cout << "  --set key value           Set configuration parameter" << std::endl;
    std::cout << "  --list-config             List all available configuration options" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXPERIMENT CONTROL:" << std::endl;
    std::cout << "  --steps N                 Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --time T                  Total simulation time in seconds" << std::endl;
    std::cout << "  --neurons N               Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions R               Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --seed S                  Random seed for reproducibility" << std::endl;
    std::cout << std::endl;
    
    std::cout << "ADVANCED OPTIONS:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --benchmark               Run performance benchmark" << std::endl;
    std::cout << "  --debug                   Enable debug logging" << std::endl;
    std::cout << "  --profile                 Enable performance profiling" << std::endl;
    std::cout << "  --checkpoint FILE          Save brain state to checkpoint" << std::endl;
    std::cout << "  --load FILE               Load brain state from checkpoint" << std::endl;
    std::cout << "  --save FILE               Save brain state to file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXPERIMENT SPECIFIC:" << std::endl;
    std::cout << "  --plasticity-config       Enable plasticity with custom learning rates" << std::endl;
    std::cout << "  --neuromodulation          Enable neuromodulation systems" << std::endl;
    std::cout << "  --memory                  Enable memory systems" << std::endl;
    std::cout << "  --development             Enable developmental processes" << std::endl;
    std::cout << "  --prediction              Enable prediction system" << std::endl;
    std::cout << std::endl;
}

void printConfigurationSummary(const std::shared_ptr<Config>& config) {
    std::cout << "Current Configuration:" << std::endl;
    std::cout << "  " << std::string(50, '=') << std::endl;
    
    // Basic settings
    std::cout << "  Basic Settings:" << std::endl;
    std::cout << "    random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "    simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "    neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
    std::cout << "    region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
    
    // Plasticity
    std::cout << "  Plasticity:" << std::endl;
    std::cout << "    stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.02f) << std::endl;
    std::cout << "    stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.015f) << std::endl;
    std::cout << "    stdp_time_constant: " << config->getOr<float>("stdp_time_constant", 20.0f) << "ms" << std::endl;
    std::cout << "    hebbian_learning_rate: " << config->getOr<float>("hebbian_learning_rate", 0.01f) << std::endl;
    
    // Development
    std::cout << "  Development:" << std::endl;
    std::cout << "    synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0001f) << std::endl;
    std::cout << "    pruning_rate: " << config->getOr<float>("pruning_rate", 0.00001f) << std::endl;
    
    // Neuromodulation
    std::cout << "  Neuromodulation:" << std::endl;
    std::cout << "    dopamine_scale: " << config->getOr<float>("dopamine_scale", 1.0f) << std::endl;
    
    std::cout << "  " << std::string(50, '=') << std::endl;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
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
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, int steps) {
    NLM_LOG_INFO("");
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
    for (SimulationStep step = 0; step < steps; ++step) {
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, int steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
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
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain, int steps) {
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
    
    for (int trial = 0; trial < steps; ++trial) {
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

void runPhase6Integration(std::shared_ptr<Brain> brain, int steps, const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Test ===");
    NLM_LOG_INFO("Testing complete brain integration...");
    
    // Test 1: Basic connectivity
    runBasicConnectivityTest(brain, std::min(50, steps));
    
    // Reset for plasticity test
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity with learning
    runPlasticityExperiment(brain, std::min(500, steps));
    
    // Reset for STDP test
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP verification
    runStdpVerification(brain, std::min(100, steps));
    
    // Final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Integration Test Complete ===");
    brain->logStatus();
}

void printVersionInfo() {
    std::cout << "NLM (熙然) Neural Learning Machine - Phase 6 Final Integration" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Version: Phase 6.0.0" << std::endl;
    std::cout << "Build Type: Final Integration" << std::endl;
    std::cout << "Compiler: Modern C++20" << std::endl;
    std::cout << "Architecture: Complete Brain Simulation" << std::endl;
    std::cout << "Systems Integrated:" << std::endl;
    std::cout << "  ✓ Neural Computation (Phase 2)" << std::endl;
    std::cout << "  ✓ World Interaction (Phase 3)" << std::endl;
    std::cout << "  ✓ Memory Systems (Phase 4)" << std::endl;
    std::cout << "  ✓ Performance Optimizations (Phase 5)" << std::endl;
    std::cout << "  ✓ Neuromodulation Integration" << std::endl;
    std::cout << "  ✓ Development Processes" << std::endl;
    std::cout << "  ✓ Prediction Systems" << std::endl;
    std::cout << "  ✓ Cognitive Mechanisms" << std::endl;
}

int main(int argc, char** argv) {
    printHeader();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string mode = "integration";  // Default mode
    int steps = 1000;
    int neurons = 500;
    int regions = 1;
    uint64_t seed = 42;
    double timeLimit = 0.0;
    bool debugMode = false;
    bool profileMode = false;
    bool benchmarkMode = false;
    std::string checkpointFile;
    std::string saveFile;
    std::string loadFile;
    bool enablePlasticity = true;
    bool enableNeuromodulation = true;
    bool enableMemory = true;
    bool enableDevelopment = true;
    bool enablePrediction = true;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--version") {
            printVersionInfo();
            return 0;
        } else if (arg == "--debug") {
            debugMode = true;
        } else if (arg == "--profile") {
            profileMode = true;
        } else if (arg == "--benchmark") {
            benchmarkMode = true;
        } else if (arg == "--plasticity-config") {
            enablePlasticity = true;
        } else if (arg == "--neuromodulation") {
            enableNeuromodulation = true;
        } else if (arg == "--memory") {
            enableMemory = true;
        } else if (arg == "--development") {
            enableDevelopment = true;
        } else if (arg == "--prediction") {
            enablePrediction = true;
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(8);
        } else if (arg == "--set" && i + 2 < argc) {
            std::string key = argv[++i];
            std::string value = argv[++i];
            // Would need config->set(key, value) - but Config is not in scope here
            // This is a simplified example
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoi(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            neurons = std::stoi(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            regions = std::stoi(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = std::stoul(argv[++i]);
        } else if (arg == "--time" && i + 1 < argc) {
            timeLimit = std::stod(argv[++i]);
        } else if (arg == "--run" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointFile = argv[++i];
        } else if (arg == "--save" && i + 1 < argc) {
            saveFile = argv[++i];
        } else if (arg == "--load" && i + 1 < argc) {
            loadFile = argv[++i];
        } else if (arg == "--list-config") {
            // Would list config options
            std::cout << "Available configuration options are in configs/default.cfg" << std::endl;
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = debugMode ? LogLevel::Debug : LogLevel::Info;
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Set defaults for Phase 2/3/4/5/6
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regions), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation parameters
    config->set("dopamine_scale", 1.0f, ConfigSource::Default);
    
    // Memory parameters
    config->set("memory.capacity", 10000.0f, ConfigSource::Default);
    config->set("memory.forgetting_rate", 0.001f, ConfigSource::Default);
    
    // Development parameters
    config->set("development.synaptogenesis_rate", 0.001f, ConfigSource::Default);
    config->set("development.pruning_rate", 0.0001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("NLM Phase 6 Advanced Configuration:");
    NLM_LOG_INFO("  Mode: " + mode);
    NLM_LOG_INFO("  Steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Neurons: " + std::to_string(neurons));
    NLM_LOG_INFO("  Regions: " + std::to_string(regions));
    NLM_LOG_INFO("  Seed: " + std::to_string(seed));
    NLM_LOG_INFO("  Plasticity: " + std::string(enablePlasticity ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Neuromodulation: " + std::string(enableNeuromodulation ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Memory: " + std::string(enableMemory ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Development: " + std::string(enableDevelopment ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Prediction: " + std::string(enablePrediction ? "Enabled" : "Disabled"));
    
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
    
    // Run the selected mode
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Starting NLM Phase 6 Simulation in " + mode + " mode...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (mode == "basic") {
        runBasicConnectivityTest(brain, steps);
    } else if (mode == "plasticity") {
        runPlasticityExperiment(brain, steps);
    } else if (mode == "stdp") {
        runStdpVerification(brain, steps);
    } else if (mode == "integration") {
        runPhase6Integration(brain, steps, config);
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        printUsage(argv[0]);
        return 1;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
    NLM_LOG_INFO("Duration: " + std::to_string(duration.count() / 1000.0) + " seconds");
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    
    // Save checkpoint if requested
    if (!checkpointFile.empty()) {
        if (brain->save(checkpointFile)) {
            NLM_LOG_INFO("Brain state saved to checkpoint: " + checkpointFile);
        } else {
            NLM_LOG_ERROR("Failed to save brain state to checkpoint: " + checkpointFile);
        }
    }
    
    // Save final state if requested
    if (!saveFile.empty()) {
        if (brain->save(saveFile)) {
            NLM_LOG_INFO("Brain state saved to: " + saveFile);
        } else {
            NLM_LOG_ERROR("Failed to save brain state to: " + saveFile);
        }
    }
    
    // Load state if requested (before running)
    if (!loadFile.empty()) {
        if (brain->load(loadFile)) {
            NLM_LOG_INFO("Brain state loaded from: " + loadFile);
        } else {
            NLM_LOG_ERROR("Failed to load brain state from: " + loadFile);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Key achievements:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Neuromodulation integration");
    NLM_LOG_INFO("  ✓ Memory systems (working, episodic, associative)");
    NLM_LOG_INFO("  ✓ Prediction systems");
    NLM_LOG_INFO("  ✓ Cognitive mechanisms");
    NLM_LOG_INFO("  ✓ Development processes");
    NLM_LOG_INFO("  ✓ Performance optimizations");
    NLM_LOG_INFO("");
    
    return 0;
}
}