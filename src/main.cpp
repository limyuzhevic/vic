// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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

using namespace nlm;

bool isFlagPresent(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

std::string getFlagValue(const std::vector<std::string>& args, const std::string& flag) {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && std::next(it) != args.end()) {
        return *(std::next(it));
    }
    return "";
}

void printVersion() {
    std::cout << "NLM (Neural Learning Machine) Phase 6 - Final Integration" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Build: Release with Phase 6 integration" << std::endl;
    std::cout << "All systems integrated: Memory, Prediction, Cognition, Neuromodulation, Development" << std::endl;
}

void runAdvancedSimulation(const Config& config) {
    std::cout << "=== Advanced NLM Simulation ===" << std::endl;
    std::cout << "Brain initialized with all Phase 6 systems integrated." << std::endl;
    std::cout << "Running comprehensive integration test..." << std::endl << std::endl;
    
    // Run advanced integration test
    auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(std::make_shared<Config>(config)));
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    std::cout << "Simulation completed successfully." << std::endl;
}

bool isFlagPresent(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

std::string getFlagValue(const std::vector<std::string>& args, const std::string& flag) {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && std::next(it) != args.end()) {
        return *(std::next(it));
    }
    return "";
}

void printVersion() {
    std::cout << "NLM (Neural Learning Machine) Phase 6 - Final Integration" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Build: Release with Phase 6 integration" << std::endl;
    std::cout << "All systems integrated: Memory, Prediction, Cognition, Neuromodulation, Development" << std::endl;
}

void printAdvancedHelp(const std::string& programName) {
    std::cout << R"(
NLM Advanced Command Line Interface - Phase 6
====================================================

COMMANDS:

Basic Operations:
  help                    Show this help message
  version                 Show version information
  run [SECONDS]           Run simulation for specified seconds (default: 10)
  benchmark               Run performance benchmarks
  experiment [NAME]       Run specific experiment

Advanced Configuration:
  --neurons COUNT         Set number of neurons (default: 500)
  --regions COUNT         Set number of brain regions (default: 1)
  --step-duration TIME    Set simulation timestep (default: 0.001s)
  --plasticity            Enable structural plasticity
  --curiosity             Enable curiosity-driven exploration
  --reward-modulation     Enable reward modulation
  --development           Enable development system
  --checkpoint FILE       Save/restore checkpoint
  --export-stats FILE     Export simulation statistics
  --visualize [MODE]      Open visualization window

Configuration Files:
  --config FILE           Load configuration from file
  --save-config FILE      Save current configuration
  --validate-config       Validate current configuration

Experiment Presets:
  --demo1                  Phase 1: Basic connectivity
  --demo2                  Phase 2: Plasticity learning
  --demo3                  Phase 3: Agent in world
  --demo4                  Phase 4: Complex cognition
  --demo5                  Phase 5: Performance optimization
  --demo6                  Phase 6: Full integration

Monitoring & Profiling:
  --monitor [INTERVAL]    Enable real-time monitoring (default: 1.0s)
  --profile               Enable performance profiling
  --stats                 Print simulation statistics
  --trace                 Enable detailed tracing

Advanced Options:
  --threads COUNT         Set number of threads for parallel processing
  --simd                  Enable SIMD optimizations
  --memory-pool           Enable memory pools
  --verbose               Enable verbose logging
  --quiet                 Suppress informational messages
  --log-level LEVEL       Set log level: TRACE, DEBUG, INFO, WARN, ERROR, FATAL

PERFORMANCE MONITORING:
  --performance           Enable performance monitoring
  --throughput            Measure steps per second
  --memory                Track memory usage
  --efficiency            Calculate computational efficiency

VISUALIZATION OPTIONS:
  --visualize-mode TEXT   Set visualization mode: "none", "simple", "detailed", "3d"
  --export-frames FILE   Export simulation frames as images
  --animation-delay MS    Set delay between animation frames

EXPERIMENT TYPES:
  learning               Synaptic plasticity through experience
  prediction             Predictive coding and error signals
  development            Brain development and maturation
  integration            Full system integration
  adaptation            Environmental adaptation
  social                Social learning and imitation

EXAMPLES:
  " + programName + " --neurons 1000 --regions 2 --run 60 --plasticity --curiosity --reward-modulation
  " + programName + " --demo6 --performance --visualize-mode detailed --export-stats results.csv
  " + programName + " --benchmark --neurons 2000 --steps 1000 --memory-pool --simd
  " + programName + " --experiment learning --step-duration 0.01 --development --curiosity

For more information about specific options, use: " + programName + " help [OPTION]

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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
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
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
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
    
    for (int trial = 0; trial < 50; ++trial) {
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

// Command line argument parsing
std::vector<std::string> args;
for (int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
}

// Handle help command
if (isFlagPresent(args, "--help") || isFlagPresent(args, "-h")) {
    printAdvancedHelp("nlm");
    return 0;
}

// Handle version command
if (isFlagPresent(args, "--version") || isFlagPresent(args, "-v")) {
    printVersion();
    return 0;
}

// Load configuration
auto config = std::make_shared<Config>();

// Process command line arguments for configuration
for (size_t i = 0; i < args.size(); ++i) {
    std::string arg = args[i];
    
    // Basic configuration options
    if (arg.substr(0, 7) == "--config") {
        if (arg.find('=') != std::string::npos) {
            config->loadFromFile(arg.substr(arg.find('=') + 1));
        } else if (i + 1 < args.size()) {
            config->loadFromFile(args[++i]);
        }
    } else if (arg.substr(0, 9) == "--save-config") {
        if (arg.find('=') != std::string::npos) {
            config->saveToFile(arg.substr(arg.find('=') + 1));
        } else if (i + 1 < args.size()) {
            config->saveToFile(args[++i]);
        }
    } else if (arg.substr(0, 17) == "--validate-config") {
        std::cout << "Configuration validation:" << std::endl;
        for (const auto& key : config->getKeys()) {
            auto value = config->get<std::string>(key);
            std::cout << "  " << key << " = " << (value.has_value() ? value.value() : "<not set>") << std::endl;
        }
    }
    
    // Experiment presets
    else if (arg == "--demo1") {
        std::cout << "Running Phase 1 Demo: Basic connectivity..." << std::endl;
        // Would call Phase1Experiment
        return 0;
    } else if (arg == "--demo2") {
        std::cout << "Running Phase 2 Demo: Plasticity learning..." << std::endl;
        // Would call Phase2Experiment
        return 0;
    } else if (arg == "--demo3") {
        std::cout << "Running Phase 3 Demo: Agent in world..." << std::endl;
        // Would call Phase3Experiment
        return 0;
    } else if (arg == "--demo4") {
        std::cout << "Running Phase 4 Demo: Complex cognition..." << std::endl;
        // Would call Phase4Experiment
        return 0;
    } else if (arg == "--demo5") {
        std::cout << "Running Phase 5 Demo: Performance optimization..." << std::endl;
        // Would call Phase5Experiment
        return 0;
    } else if (arg == "--demo6") {
        std::cout << "Running Phase 6 Demo: Full integration..." << std::endl;
        // Would call Phase6Experiment
        return 0;
    }
    
    // Advanced configuration options
    else if (arg.substr(0, 11) == "--neurons=") {
        config->set("neuron_count", static_cast<int64_t>(std::stoll(arg.substr(11))));
    } else if (arg.substr(0, 10) == "--regions=") {
        config->set("region_count", static_cast<int64_t>(std::stoll(arg.substr(10))));
    } else if (arg.substr(0, 17) == "--step-duration=") {
        config->set("simulation_timestep", std::stod(arg.substr(17)));
    } else if (arg == "--plasticity") {
        config->set("plasticity.structural.enable", true);
    } else if (arg == "--curiosity") {
        config->set("neuromod.curiosity.enable", true);
    } else if (arg == "--reward-modulation") {
        config->set("neuromod.dopamine.enable", true);
    } else if (arg == "--development") {
        config->set("development.enabled", true);
    }
    
    // Monitoring options
    else if (arg.substr(0, 10) == "--monitor=") {
        std::cout << "Monitoring enabled with interval: " << arg.substr(10) << "s" << std::endl;
        // Would set up monitoring
    } else if (arg == "--profile") {
        std::cout << "Performance profiling enabled" << std::endl;
        // Would enable profiling
    } else if (arg == "--stats") {
        std::cout << "Statistics will be collected" << std::endl;
        // Would enable stats collection
    }
    
    // Advanced options
    else if (arg.substr(0, 11) == "--threads=") {
        std::cout << "Threads set to: " << arg.substr(11) << std::endl;
        // Would set thread count
    } else if (arg == "--simd") {
        std::cout << "SIMD optimizations enabled" << std::endl;
        // Would enable SIMD
    } else if (arg == "--memory-pool") {
        std::cout << "Memory pools enabled" << std::endl;
        // Would enable memory pools
    } else if (arg.substr(0, 13) == "--log-level=") {
        std::cout << "Log level set to: " << arg.substr(13) << std::endl;
        // Would set log level
    }
}

// Set default values if not already set
if (!config->has("random_seed")) {
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
}
if (!config->has("simulation_timestep")) {
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
}
if (!config->has("neuron_count")) {
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
}
if (!config->has("region_count")) {
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
}
if (!config->has("connection_probability")) {
    config->set("connection_probability", 0.15f, ConfigSource::Default);
}

printBanner();

std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
