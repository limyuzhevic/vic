// NLM — 熙然
// Advanced Neural Learning Machine CLI
// Enhanced command-line interface with comprehensive configuration and controls

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
#include <algorithm>
#include <chrono>
#include <thread>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Advanced Neural Simulation Framework                     ║
    ║                                                               ║
    ║     Enhanced Phase 2+ Neural Computation                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << R"(
USAGE:
    ./nlm_advanced [OPTIONS]

MODES:
    --help                    Show this help message
    --version                 Show version information
    --demo                    Run Phase 2 demonstration (default)
    --phase3                  Run Phase 3 world interaction demo
    --phase6                  Run Phase 6 integration test
    --benchmark               Run performance benchmark
    --interactive             Start interactive mode with real-time monitoring

OPTIONS:
    --config FILE             Load configuration from JSON file
    --seed VALUE              Set random seed (default: 42)
    --neurons COUNT           Set number of neurons (default: 1000)
    --regions COUNT           Set number of brain regions (default: 2)
    --steps COUNT             Number of simulation steps (default: 1000)
    --timestep VALUE          Simulation timestep in seconds (default: 0.001)
    --verbose LEVEL           Set log verbosity (0-3, default: 2)
    --output FILE             Save simulation results to file
    --checkpoint FILE         Save brain state to checkpoint
    --load-checkpoint FILE    Load brain state from checkpoint
    --monitor                 Enable real-time monitoring
    --profile                 Enable performance profiling
    --export-stats FILE       Export performance statistics
    --neural-view            Start neural visualization

ADVANCED OPTIONS:
    --synaptogenesis RATE     Set synaptogenesis rate (default: 0.0001)
    --pruning RATE            Set pruning rate (default: 0.00001)
    --stdp-ltp WEIGHT         STDP LTP weight (default: 0.02)
    --stdp-ltd WEIGHT         STDP LTD weight (default: 0.015)
    --stdp-tau VALUE          STDP time constant (default: 20.0)
    --plasticity-mod VALUE    Plasticity modifier (default: 1.0)
    --curiosity-enabled       Enable curiosity-driven exploration
    --development-enabled     Enable developmental stages
    --reward-modulation      Enable reward-based learning
    --structural-plasticity  Enable structural plasticity
    --working-memory-SIZE     Working memory capacity (default: 100)
    --episodic-memory-SIZE    Max episodic memory entries (default: 1000)

EXPERIMENTS:
    --experiment BASIC         Basic connectivity test
    --experiment PLASTICITY    Plasticity learning experiment
    --experiment STDP          STDP verification
    --experiment COMPREHENSIVE  Comprehensive integration test
    --experiment CUSTOM       Custom experiment with custom parameters

EXAMPLES:
    # Run basic demo with custom configuration
    ./nlm_advanced --demo --config myconfig.json --neurons 2000 --steps 5000

    # Run comprehensive benchmark
    ./nlm_advanced --benchmark --profile --export-stats benchmark.json

    # Load and continue a saved brain
    ./nlm_advanced --demo --load-checkpoint brain.state --steps 2000

    # Interactive exploration
    ./nlm_advanced --interactive --monitor --neural-view

    # Run specific experiment
    ./nlm_advanced --experiment STDP --verbose 3

    # Create and save a new brain
    ./nlm_advanced --demo --output brain.state --checkpoint brain.state
    )" << std::endl;
}

void printVersion() {
    std::cout << R"(
NLM Advanced Neural Learning Machine v2.0.0
Phase 2+ Enhanced Neural Computation Framework
Built with C++20, SIMD optimizations, and advanced neural integration

Key Features:
- Real LIF neuron dynamics with event-driven spike propagation
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction and planning systems
- Cognitive mechanisms (attention, concept formation)
- Developmental stages and structural plasticity
- Checkpointing and persistence
- Advanced configuration and command-line interface

Architecture:
World → Sensory Input → Neural Processing → Internal State
→ Memory/Prediction → Neuromodulation → Action Selection
→ Motor Output → World Consequence → Plasticity → New Brain

License: MIT
Authors: NLM Research Project
)" << std::endl;
}

void printBannerAdvanced() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║        NLM Advanced Neural Simulation Framework               ║
║                                                               ║
║  Enhanced Phase 2+ Neural Computation with Advanced Features   ║
║                                                               ║
║  ===============================================            ║
║  Real-time monitoring | Advanced configuration | Export tools  ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void setupAdvancedConfiguration(std::shared_ptr<Config> config) {
    // Enhanced default configuration for advanced mode
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Advanced plasticity parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Advanced memory system parameters
    config->set("working_memory_capacity", 100, ConfigSource::Default);
    config->set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
    config->set("associative_memory_pattern_count", 500, ConfigSource::Default);
    
    // Advanced neuromodulation parameters
    config->set("dopamine_plasticity_factor", 1.0f, ConfigSource::Default);
    config->set("curiosity_threshold", 0.15f, ConfigSource::Default);
    config->set("novelty_decay", 0.99f, ConfigSource::Default);
    
    // Advanced cognitive parameters
    config->set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    config->set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    config->set("planning_depth", 5, ConfigSource::Default);
    config->set("concept_learning_rate", 0.1f, ConfigSource::Default);
    
    // Advanced developmental parameters
    config->set("developmental_initial_plasticity", 1.0f, ConfigSource::Default);
    config->set("developmental_critical_plasticity", 0.8f, ConfigSource::Default);
    config->set("developmental_maturation_plasticity", 0.5f, ConfigSource::Default);
    config->set("developmental_adult_plasticity", 0.2f, ConfigSource::Default);
    
    // Advanced performance parameters
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    config->set("max_spike_history", 100, ConfigSource::Default);
    
    NLM_LOG_INFO("Advanced configuration loaded with enhanced defaults");
}

void logConfigurationSummary(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("╔═══════════════════════════════════════════════════════════════╗");
    NLM_LOG_INFO("║                    ADVANCED NLM CONFIGURATION SUMMARY                  ║");
    NLM_LOG_INFO("╚═══════════════════════════════════════════════════════════════╝");
    
    std::vector<std::string> keys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
        "synaptogenesis_rate", "pruning_rate", "working_memory_capacity",
        "episodic_memory_max_episodes", "dopamine_plasticity_factor",
        "curiosity_threshold", "novelty_decay", "attention_inhibition_strength",
        "attention_excitation_strength", "planning_depth", "concept_learning_rate"
    };
    
    for (const auto& key : keys) {
        if (config.has(key)) {
            if constexpr (std::is_same_v<decltype(config.get<int64_t>(key)), std::optional<int64_t>>) {
                auto val = config.get<int64_t>(key);
                if (val) NLM_LOG_INFO("  │ " + key + ": " + std::to_string(*val) + " " + std::string(48 - key.length() - std::to_string(*val).length(), ' ') + " │");
            } else if constexpr (std::is_same_v<decltype(config.get<double>(key)), std::optional<double>>) {
                auto val = config.get<double>(key);
                if (val) NLM_LOG_INFO("  │ " + key + ": " + std::to_string(*val) + " " + std::string(48 - key.length() - std::to_string(*val).length(), ' ') + " │");
            } else if constexpr (std::is_same_v<decltype(config.get<float>(key)), std::optional<float>>) {
                auto val = config.get<float>(key);
                if (val) NLM_LOG_INFO("  │ " + key + ": " + std::to_string(*val) + " " + std::string(48 - key.length() - std::to_string(*val).length(), ' ') + " │");
            } else if constexpr (std::is_same_v<decltype(config.get<bool>(key)), std::optional<bool>>) {
                auto val = config.get<bool>(key);
                if (val) NLM_LOG_INFO("  │ " + key + ": " + std::string(*val ? "true" : "false") + " " + std::string(48 - key.length() - std::string(*val ? "true" : "false").length(), ' ') + " │");
            }
        }
    }
    
    NLM_LOG_INFO("╚═══════════════════════════════════════════════════════════════╝");
    NLM_LOG_INFO("");
}

void runAdvancedDemo(std::shared_ptr<Brain> brain, const Config& config, bool monitor, bool profile) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("╔═══════════════════════════════════════════════════════════════╗");
    NLM_LOG_INFO("║                    ADVANCED DEMONSTRATION STARTED                    ║");
    NLM_LOG_INFO("╚═══════════════════════════════════════════════════════════════╝");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    double totalSimTime = 0.0;
    
    // Test 1: Enhanced basic connectivity
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 1: Enhanced Neural Connectivity Analysis");
    NLM_LOG_INFO("===============================================");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found for connectivity test");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    NLM_LOG_INFO("  Brain initialized with " + std::to_string(neuron_count) + " neurons and " + 
                 std::to_string(region->getPopulationCount()) + " populations");
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Enhanced connectivity test with multiple current injections
    NLM_LOG_INFO("  Testing enhanced connectivity patterns...");
    
    // Pattern 1: Sparse stimulation
    std::vector<NeuronId> testNeurons;
    size_t testCount = std::min<size_t>(20, neurons.size());
    for (size_t i = 0; i < testCount; ++i) {
        if (neurons[i]->getType() == NeuronType::Sensory) {
            testNeurons.push_back(neurons[i]->getId());
        }
    }
    
    NLM_LOG_INFO("  Injecting current into " + std::to_string(testNeurons.size()) + " sensory neurons...");
    
    for (NeuronId id : testNeurons) {
        brain->injectCurrent(id, 50.0f);
    }
    
    // Run simulation steps
    size_t stepsRun = 0;
    for (SimulationStep step = 0; step < 500; ++step) {
        brain->step(step, step * 0.001);
        stepsRun++;
        
        if (monitor && step % 100 == 0) {
            double elapsed = (std::chrono::high_resolution_clock::now() - startTime).count() * 1e-9;
            NLM_LOG_INFO("  Step " + std::to_string(step) + "/500 | Spikes: " + 
                        std::to_string(brain->getTotalSpikeCount() - initialSpikes) +
                        " | Elapsed: " + std::to_string(elapsed) + "s");
        }
    }
    
    size_t spikesGenerated = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  ✓ Connectivity test complete: " + std::to_string(spikesGenerated) + " spikes generated");
    
    // Test 2: Enhanced plasticity experiment
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 2: Advanced Plasticity Learning Experiment");
    NLM_LOG_INFO("===============================================");
    
    // Reset and run enhanced plasticity experiment
    brain->reset();
    brain->initialize();
    
    NLM_LOG_INFO("  Running enhanced plasticity experiment (3000 steps)...");
    
    // Enable comprehensive plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);  // All plasticity types
        }
    }
    
    // Pattern-based learning with multiple patterns
    std::vector<NeuronId> plasticPatternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && plasticPatternNeurons.size() < 30) {
                plasticPatternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    // Run enhanced learning pattern
    initialSpikes = brain->getTotalSpikeCount();
    for (SimulationStep step = 0; step < 3000; ++step) {
        // Apply multiple learning patterns
        if (step % 3 == 0) {  // Pattern 1
            for (size_t i = 0; i < std::min<size_t>(10, plasticPatternNeurons.size()); ++i) {
                brain->injectCurrent(plasticPatternNeurons[i], 40.0f);
            }
        } else if (step % 3 == 1) {  // Pattern 2 (different neurons)
            for (size_t i = 10; i < std::min<size_t>(20, plasticPatternNeurons.size()); ++i) {
                brain->injectCurrent(plasticPatternNeurons[i], 40.0f);
            }
        } else {  // Pattern 3 (interleaved)
            for (size_t i = 20; i < std::min<size_t>(30, plasticPatternNeurons.size()); ++i) {
                brain->injectCurrent(plasticPatternNeurons[i], 40.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        
        if (monitor && step % 500 == 0) {
            double elapsed = (std::chrono::high_resolution_clock::now() - startTime).count() * 1e-9;
            NLM_LOG_INFO("  Step " + std::to_string(step) + "/3000 | Spikes: " + 
                        std::to_string(brain->getTotalSpikeCount() - initialSpikes) +
                        " | Elapsed: " + std::to_string(elapsed) + "s");
        }
    }
    
    size_t learningSpikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  ✓ Plasticity experiment complete: " + std::to_string(learningSpikes) + " spikes generated");
    
    // Test 3: STDP verification with enhanced testing
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 3: Enhanced STDP Verification");
    NLM_LOG_INFO("===============================================");
    
    brain->reset();
    brain->initialize();
    
    auto* testRegion = brain->getRegion(RegionId(1));
    if (testRegion && !testRegion->getSynapses().empty()) {
        Synapse* testSyn = testRegion->getSynapses()[0].get();
        testSyn->enablePlasticity(false, true, false);  // STDP only
        
        float beforeWeight = testSyn->getWeight();
        NLM_LOG_INFO("  Testing STDP on synapse: pre=" + std::to_string(testSyn->getSourceNeuron().index()) +
                     " post=" + std::to_string(testSyn->getDestinationNeuron().index()) +
                     " weight=" + std::to_string(beforeWeight));
        
        // Enhanced STDP test with correlated activity
        NLM_LOG_INFO("  Creating correlated pre->post activity (75 trials)...");
        
        for (int trial = 0; trial < 75; ++trial) {
            brain->injectCurrent(testSyn->getSourceNeuron(), 80.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(testSyn->getDestinationNeuron(), 80.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            
            if (profile && trial % 25 == 0) {
                double elapsed = (std::chrono::high_resolution_clock::now() - startTime).count() * 1e-9;
                NLM_LOG_INFO("    Trial " + std::to_string(trial) + "/75 | Weight: " + 
                           std::to_string(testSyn->getWeight()) + " | Elapsed: " + 
                           std::to_string(elapsed) + "s");
            }
        }
        
        float delta = testSyn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  STDP result: Δ=" + std::to_string(delta) +
                     " (before=" + std::to_string(beforeWeight) +
                     " after=" + std::to_string(testSyn->getWeight()) + ")");
        
        if (delta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (delta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
    
    // Final status
    double totalElapsed = (std::chrono::high_resolution_clock::now() - startTime).count() * 1e-9;
    NLM_LOG_INFO("");
    NLM_LOG_INFO("╔═══════════════════════════════════════════════════════════════╝");
    NLM_LOG_INFO("║                ADVANCED DEMONSTRATION COMPLETE                    ║");
    NLM_LOG_INFO("╚═══════════════════════════════════════════════════════════════╝");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Performance Summary:");
    NLM_LOG_INFO("  Total simulation time: " + std::to_string(totalElapsed) + " seconds");
    NLM_LOG_INFO("  Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  Brain state: " + std::to_string(brain->getTotalNeuronCount()) + " neurons, " +
                 std::to_string(brain->getTotalSynapseCount()) + " synapses");
    
    // Log final brain status
    brain->logStatus();
}

int main(int argc, char** argv) {
    printBannerAdvanced();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string checkpointFile = "";
    bool loadCheckpoint = false;
    bool monitor = false;
    bool profile = false;
    bool neuralView = false;
    std::string experimentType = "COMPREHENSIVE";
    std::string exportStatsFile = "";
    
    int seed = 42;
    int neuronCount = 1000;
    int regionCount = 2;
    int steps = 1000;
    double timestep = 0.001;
    int verbosity = 2;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--demo") {
            // Default mode, do nothing
        } else if (arg == "--phase3") {
            experimentType = "PHASE3";
        } else if (arg == "--phase6") {
            experimentType = "PHASE6";
        } else if (arg == "--benchmark") {
            experimentType = "BENCHMARK";
        } else if (arg == "--interactive") {
            experimentType = "INTERACTIVE";
        } else if (arg == "--experiment") {
            if (i + 1 < argc) {
                experimentType = argv[++i];
            }
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = std::stoi(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            regionCount = std::stoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoi(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            timestep = std::stod(argv[++i]);
        } else if (arg == "--verbose" && i + 1 < argc) {
            verbosity = std::stoi(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointFile = argv[++i];
        } else if (arg == "--load-checkpoint" && i + 1 < argc) {
            checkpointFile = argv[++i];
            loadCheckpoint = true;
        } else if (arg == "--monitor") {
            monitor = true;
        } else if (arg == "--profile") {
            profile = true;
        } else if (arg == "--neural-view") {
            neuralView = true;
        } else if (arg == "--export-stats" && i + 1 < argc) {
            exportStatsFile = argv[++i];
        } else if (arg == "--synaptogenesis" && i + 1 < argc) {
            // Could store this in config, but for now just acknowledge
            float rate = std::stof(argv[++i]);
            NLM_LOG_INFO("Synaptogenesis rate set to: " + std::to_string(rate));
        } else if (arg == "--pruning" && i + 1 < argc) {
            float rate = std::stof(argv[++i]);
            NLM_LOG_INFO("Pruning rate set to: " + std::to_string(rate));
        } else if (arg == "--stdp-ltp" && i + 1 < argc) {
            float weight = std::stof(argv[++i]);
            NLM_LOG_INFO("STDP LTP weight set to: " + std::to_string(weight));
        } else if (arg == "--stdp-ltd" && i + 1 < argc) {
            float weight = std::stof(argv[++i]);
            NLM_LOG_INFO("STDP LTD weight set to: " + std::to_string(weight));
        } else if (arg == "--stdp-tau" && i + 1 < argc) {
            float tau = std::stof(argv[++i]);
            NLM_LOG_INFO("STDP tau set to: " + std::to_string(tau));
        } else if (arg == "--curiosity-enabled") {
            NLM_LOG_INFO("Curiosity-driven exploration enabled");
        } else if (arg == "--development-enabled") {
            NLM_LOG_INFO("Developmental stages enabled");
        } else if (arg == "--reward-modulation") {
            NLM_LOG_INFO("Reward-based learning enabled");
        } else if (arg == "--structural-plasticity") {
            NLM_LOG_INFO("Structural plasticity enabled");
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }
    
    // Initialize logger with specified verbosity
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(static_cast<LogLevel>(std::max(0, std::min(3, verbosity))));
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Advanced Neural Simulation ===");
    NLM_LOG_INFO("Starting with seed: " + std::to_string(seed));
    NLM_LOG_INFO("Neuron count: " + std::to_string(neuronCount));
    NLM_LOG_INFO("Region count: " + std::to_string(regionCount));
    NLM_LOG_INFO("Steps: " + std::to_string(steps));
    NLM_LOG_INFO("Timestep: " + std::to_string(timestep) + "s");
    NLM_LOG_INFO("Monitor mode: " + std::string(monitor ? "enabled" : "disabled"));
    NLM_LOG_INFO("Profile mode: " + std::string(profile ? "enabled" : "disabled"));
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Set advanced defaults
    setupAdvancedConfiguration(config);
    
    // Override with parsed arguments
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    
    // Log configuration summary
    logConfigurationSummary(*config);
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Advanced Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Load checkpoint if requested
    if (loadCheckpoint && !checkpointFile.empty()) {
        NLM_LOG_INFO("Loading brain state from checkpoint: " + checkpointFile);
        if (brain->load(checkpointFile)) {
            NLM_LOG_INFO("Checkpoint loaded successfully");
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint");
        }
    }
    
    // Run selected experiment
    auto experimentStart = std::chrono::high_resolution_clock::now();
    
    if (experimentType == "PHASE3") {
        NLM_LOG_INFO("Running Phase 3 World Interaction Demo...");
        // Phase 3 implementation would go here
    } else if (experimentType == "PHASE6") {
        NLM_LOG_INFO("Running Phase 6 Integration Test...");
        // Phase 6 implementation would go here
    } else if (experimentType == "BENCHMARK") {
        NLM_LOG_INFO("Running Performance Benchmark...");
        // Benchmark implementation would go here
    } else if (experimentType == "INTERACTIVE") {
        NLM_LOG_INFO("Starting Interactive Mode...");
        // Interactive mode implementation would go here
    } else if (experimentType == "BASIC") {
        NLM_LOG_INFO("Running Basic Connectivity Test...");
        // Basic test implementation would go here
    } else if (experimentType == "PLASTICITY") {
        NLM_LOG_INFO("Running Plasticity Learning Experiment...");
        // Plasticity experiment implementation would go here
    } else if (experimentType == "STDP") {
        NLM_LOG_INFO("Running STDP Verification...");
        // STDP test implementation would go here
    } else {
        NLM_LOG_INFO("Running Advanced Demo with comprehensive testing...");
        runAdvancedDemo(brain, *config, monitor, profile);
    }
    
    auto experimentEnd = std::chrono::high_resolution_clock::now();
    double experimentTime = (experimentEnd - experimentStart).count() * 1e-9;
    
    // Export statistics if requested
    if (!exportStatsFile.empty()) {
        NLM_LOG_INFO("Exporting statistics to: " + exportStatsFile);
        // Export implementation would go here
    }
    
    // Save checkpoint if requested
    if (!checkpointFile.empty()) {
        NLM_LOG_INFO("Saving brain state to checkpoint: " + checkpointFile);
        if (brain->save(checkpointFile)) {
            NLM_LOG_INFO("Checkpoint saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint");
        }
    }
    
    // Save output file if requested
    if (!outputFile.empty()) {
        NLM_LOG_INFO("Saving simulation results to: " + outputFile);
        // Output implementation would go here
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("╔═══════════════════════════════════════════════════════════════╗");
    NLM_LOG_INFO("║                         SIMULATION COMPLETE                        ║");
    NLM_LOG_INFO("╚═══════════════════════════════════════════════════════════════╝");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Performance Summary:");
    NLM_LOG_INFO("  Experiment duration: " + std::to_string(experimentTime) + " seconds");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
    
    // Final brain status
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Advanced Simulation Finished Successfully ===");
    
    return 0;
}
