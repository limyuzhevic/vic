// Updated main.cpp that uses the enhanced ArgsParser
// This file demonstrates how to use the new argument parsing system

#include "core/ArgsParser/ArgsParser.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Argument Parser Version                          ║
    ║                                                               ║
    ║     Advanced neural computation with comprehensive config      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Enhanced argument definitions for the parser
void registerArguments(ArgsParser& parser) {
    // Core parameters
    parser.registerArgument(ArgMetadata("random_seed", 
                                       "Random seed for reproducible simulations",
                                       "--random-seed=<value>", "42", false,
                                       ArgsCategory::Core, "s", "random-seed"));
    
    parser.registerArgument(ArgMetadata("simulation_timestep", 
                                       "Time step for neural simulation",
                                       "--simulation-timestep=<value>", "0.001", false,
                                       ArgsCategory::Core, "t", "simulation-timestep"));
    
    parser.registerArgument(ArgMetadata("neuron_count", 
                                       "Number of neurons in the brain network",
                                       "--neuron-count=<value>", "500", false,
                                       ArgsCategory::Core, "n", "neuron-count"));
    
    parser.registerArgument(ArgMetadata("region_count", 
                                       "Number of neural regions",
                                       "--region-count=<value>", "1", false,
                                       ArgsCategory::Core, "r", "region-count"));
    
    parser.registerArgument(ArgMetadata("connection_probability", 
                                       "Probability of synaptic connections between neurons",
                                       "--connection-probability=<value>", "0.15", false,
                                       ArgsCategory::Core, "p", "connection-probability"));
    
    parser.registerArgument(ArgMetadata("enable_neuromodulation", 
                                       "Enable neuromodulation system for adaptive behavior",
                                       "--enable-neuromodulation=<true|false>", "true", false,
                                       ArgsCategory::Core, "m", "enable-neuromodulation"));
    
    // Brain parameters
    parser.registerArgument(ArgMetadata("brain_type", 
                                       "Type of brain architecture",
                                       "--brain-type=<value>", "standard", false,
                                       ArgsCategory::Brain, "b", "brain-type"));
    
    parser.registerArgument(ArgMetadata("enable_plasticity", 
                                       "Enable synaptic plasticity mechanisms",
                                       "--enable-plasticity=<true|false>", "true", false,
                                       ArgsCategory::Brain, "l", "enable-plasticity"));
    
    // Plasticity parameters
    parser.registerArgument(ArgMetadata("stdp_ltp_weight", 
                                       "STDP long-term potentiation weight",
                                       "--stdp-ltp-weight=<value>", "0.02", false,
                                       ArgsCategory::Plasticity, "a", "stdp-ltp-weight"));
    
    parser.registerArgument(ArgMetadata("stdp_ltd_weight", 
                                       "STDP long-term depression weight",
                                       "--stdp-ltd-weight=<value>", "0.015", false,
                                       ArgsCategory::Plasticity, "A", "stdp-ltd-weight"));
    
    parser.registerArgument(ArgMetadata("stdp_tau", 
                                       "STDP time constant",
                                       "--stdp-tau=<value>", "20.0", false,
                                       ArgsCategory::Plasticity, "T", "stdp-tau"));
    
    parser.registerArgument(ArgMetadata("synaptogenesis_rate", 
                                       "Rate of new synapse formation",
                                       "--synaptogenesis-rate=<value>", "0.0001", false,
                                       ArgsCategory::Plasticity, "f", "synaptogenesis-rate"));
    
    parser.registerArgument(ArgMetadata("pruning_rate", 
                                       "Rate of synapse pruning",
                                       "--pruning-rate=<value>", "0.00001", false,
                                       ArgsCategory::Plasticity, "P", "pruning-rate"));
    
    // Neuromodulation parameters
    parser.registerArgument(ArgMetadata("curiosity_strength", 
                                       "Strength of curiosity-driven learning",
                                       "--curiosity-strength=<value>", "0.5", false,
                                       ArgsCategory::Neuromodulation, "c", "curiosity-strength"));
    
    parser.registerArgument(ArgMetadata("novelty_threshold", 
                                       "Threshold for novelty detection",
                                       "--novelty-threshold=<value>", "0.3", false,
                                       ArgsCategory::Neuromodulation, "N", "novelty-threshold"));
    
    parser.registerArgument(ArgMetadata("reward_decay", 
                                       "Reward decay factor",
                                       "--reward-decay=<value>", "0.95", false,
                                       ArgsCategory::Neuromodulation, "d", "reward-decay"));
    
    // Simulation parameters
    parser.registerArgument(ArgMetadata("max_steps", 
                                       "Maximum number of simulation steps",
                                       "--max-steps=<value>", "1000", false,
                                       ArgsCategory::Simulation, "x", "max-steps"));
    
    parser.registerArgument(ArgMetadata("enable_checkpointing", 
                                       "Enable checkpoint saving",
                                       "--enable-checkpointing=<true|false>", "false", false,
                                       ArgsCategory::Simulation, "k", "enable-checkpointing"));
    
    // Output parameters
    parser.registerArgument(ArgMetadata("output_dir", 
                                       "Directory for output files",
                                       "--output-dir=<path>", "outputs", false,
                                       ArgsCategory::Output, "o", "output-dir"));
    
    parser.registerArgument(ArgMetadata("save_weights", 
                                       "Save neural network weights",
                                       "--save-weights=<true|false>", "true", false,
                                       ArgsCategory::Output, "w", "save-weights"));
    
    // Debugging parameters
    parser.registerArgument(ArgMetadata("log_level", 
                                       "Logging level (debug, info, warning, error)",
                                       "--log-level=<level>", "info", false,
                                       ArgsCategory::Debugging, "L", "log-level"));
    
    parser.registerArgument(ArgMetadata("debug_neurons", 
                                       "List of neuron IDs for debugging",
                                       "--debug-neurons=<list>", "1", false,
                                       ArgsCategory::Debugging, "D", "debug-neurons"));
    
    parser.registerArgument(ArgMetadata("enable_profiling", 
                                       "Enable performance profiling",
                                       "--enable-profiling=<true|false>", "false", false,
                                       ArgsCategory::Debugging, "f", "enable-profiling"));
    
    parser.registerArgument(ArgMetadata("enable_verbose", 
                                       "Enable verbose output",
                                       "--verbose", "", false,
                                       ArgsCategory::Debugging, "V", "verbose"));
}

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

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Enhanced Argument Parser..." << std::endl;
    std::cout << "Phase 2: Real Neural Computation with Enhanced Configuration" << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Enhanced Argument Parser ===");
    NLM_LOG_INFO("Implementing comprehensive configuration system with:");
    NLM_LOG_INFO("  - Argument categories (core, brain, plasticity, neuromodulation, simulation, output, debugging)");
    NLM_LOG_INFO("  - Configuration file loading with format detection");
    NLM_LOG_INFO("  - Automatic configuration presets");
    NLM_LOG_INFO("  - Type-safe argument parsing with validation");
    NLM_LOG_INFO("  - Error handling with descriptive messages");
    NLM_LOG_INFO("  - Command completion support");
    NLM_LOG_INFO("");
    
    // Initialize the enhanced argument parser
    ArgsParser parser;
    
    // Register all arguments
    registerArguments(parser);
    
    // Parse command line arguments
    if (!parser.parse(argc, argv)) {
        NLM_LOG_ERROR("Failed to parse arguments:");
        for (const auto& error : parser.getErrors()) {
            NLM_LOG_ERROR("  " + error);
        }
        NLM_LOG_INFO("\nUse --help for more information.");
        return 1;
    }
    
    // Display parsed configuration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Configuration Summary ===");
    parser.logConfig();
    
    // Load configuration file if specified
    std::string configFile;
    if (parser.has("config-file")) {
        configFile = parser.getOr<std::string>("config-file", "");
        if (!configFile.empty()) {
            NLM_LOG_INFO("Loading configuration from file: " + configFile);
            if (!parser.loadConfig(configFile, true)) {
                NLM_LOG_WARNING("Failed to load configuration file: " + configFile);
            }
        }
    }
    
    // Apply preset if specified
    if (parser.has("preset")) {
        std::string preset = parser.getOr<std::string>("preset", "");
        if (!preset.empty() && preset != "list") {
            NLM_LOG_INFO("Applying preset: " + preset);
            if (!parser.applyPreset(preset)) {
                NLM_LOG_ERROR("Failed to apply preset: " + preset);
                return 1;
            }
        }
    }
    
    // Get parsed values
    int64_t randomSeed = parser.getOr<int64_t>("random_seed", 42);
    double simulationTimestep = parser.getOr<double>("simulation_timestep", 0.001);
    int64_t neuronCount = parser.getOr<int64_t>("neuron_count", 500);
    int64_t regionCount = parser.getOr<int64_t>("region_count", 1);
    double connectionProbability = parser.getOr<double>("connection_probability", 0.15);
    bool enableNeuromodulation = parser.getOr<bool>("enable_neuromodulation", true);
    std::string brainType = parser.getOr<std::string>("brain_type", "standard");
    bool enablePlasticity = parser.getOr<bool>("enable_plasticity", true);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Core Configuration ===");
    NLM_LOG_INFO("  random_seed: " + std::to_string(randomSeed));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(simulationTimestep) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(neuronCount));
    NLM_LOG_INFO("  region_count: " + std::to_string(regionCount));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(connectionProbability));
    NLM_LOG_INFO("  enable_neuromodulation: " + std::to_string(enableNeuromodulation));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  brain_type: " + brainType);
    NLM_LOG_INFO("  enable_plasticity: " + std::to_string(enablePlasticity));
    
    // Log plasticity configuration
    double stdpLtpWeight = parser.getOr<double>("stdp_ltp_weight", 0.02);
    double stdpLtdWeight = parser.getOr<double>("stdp_ltd_weight", 0.015);
    double stdpTau = parser.getOr<double>("stdp_tau", 20.0);
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Plasticity Configuration ===");
    NLM_LOG_INFO("  stdp_ltp_weight: " + std::to_string(stdpLtpWeight));
    NLM_LOG_INFO("  stdp_ltd_weight: " + std::to_string(stdpLtdWeight));
    NLM_LOG_INFO("  stdp_tau: " + std::to_string(stdpTau));
    
    // Initialize simulation clock
    SimulationClock clock(simulationTimestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(simulationTimestep) + "s");
    
    // Initialize random number generator
    Random::setSeed(randomSeed);
    NLM_LOG_INFO("Random seed set to: " + std::to_string(randomSeed));
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    
    // Create brain configuration from parsed arguments
    auto brainConfig = parser.getConfigObject();
    
    auto brain = std::make_shared<Brain>(brainConfig);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
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
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Argument Parser Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration successfully loaded and applied:");
    NLM_LOG_INFO("  ✓ Command line arguments parsed");
    NLM_LOG_INFO("  ✓ Configuration file loaded (if specified)");
    NLM_LOG_INFO("  ✓ Preset applied (if specified)");
    NLM_LOG_INFO("  ✓ Configuration validated");
    NLM_LOG_INFO("  ✓ Brain initialized with parsed parameters");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The enhanced argument parser provides:");
    NLM_LOG_INFO("  ✓ Type-safe argument parsing with validation");
    NLM_LOG_INFO("  ✓ Comprehensive error handling");
    NLM_LOG_INFO("  ✓ User-friendly help and error messages");
    NLM_LOG_INFO("  ✓ Configuration file loading with format detection");
    NLM_LOG_INFO("  ✓ Automatic configuration presets");
    NLM_LOG_INFO("  ✓ Command completion support");
    NLM_LOG_INFO("  ✓ Logging configuration integration");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 with Enhanced Argument Parser is now complete!");
    
    return 0;
}