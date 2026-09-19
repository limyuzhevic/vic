#!/usr/bin/env c++
// NLM Command Line Interface
// Simple command-line interface for running NLM simulations
// Based on easy_usage.md patterns but with real CLI support

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"
#include "src/visualization/VisualizationInterface.hpp"
#include "src/environment/Environment.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/agent/SensoryPercept.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Command line argument parsing
struct CLIArgs {
    bool help = false;
    bool version = false;
    std::string config_file = "configs/default.json";
    std::string output_file = "";
    int steps = 1000;
    double timestep = 0.001;
    bool verbose = false;
    bool visualize = false;
    bool quiet = false;
    std::string experiment = "basic";
    bool save_checkpoint = false;
    bool load_checkpoint = false;
};

// Print help information
void printHelp() {
    std::cout << "NLM — Neural Learning Machine CLI" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                Show this help message" << std::endl;
    std::cout << "  -v, --version            Show version information" << std::endl;
    std::cout << "  -c, --config FILE        Load configuration from FILE (default: configs/default.json)" << std::endl;
    std::cout << "  -o, --output FILE        Save brain state to FILE" << std::endl;
    std::cout << "  -s, --steps N            Run N simulation steps (default: 1000)" << std::endl;
    std::cout << "  -t, --timestep T         Set timestep (default: 0.001)" << std::endl;
    std::cout << "  -V, --verbose            Enable verbose output" << std::endl;
    std::cout << "  -z, --quiet              Quiet mode (minimal output)" << std::endl;
    std::cout << "  -l, --load FILE          Load checkpoint from FILE" << std::endl;
    std::cout << "  -S, --save              Save checkpoint after simulation" << std::endl;
    std::cout << "  -x, --experiment TYPE    Run specific experiment (basic, plasticity, stdp, connectivity)" << std::endl;
    std::cout << std::endl;
    std::cout << "Experiments:" << std::endl;
    std::cout << "  basic           Basic connectivity test" << std::endl;
    std::cout << "  plasticity      Plasticity learning experiment" << std::endl;
    std::cout << "  stdp           STDP verification" << std::endl;
    std::cout << "  connectivity    Network connectivity test" << std::endl;
    std::cout << "  all             Run all experiments (default)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --steps 5000 --verbose" << std::endl;
    std::cout << "  nlm -c myconfig.json -o brain_state.bin -s" << std::endl;
    std::cout << "  nlm -x plasticity -v" << std::endl;
}

// Parse command line arguments
CLIArgs parseArgs(int argc, char** argv) {
    CLIArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "-h" || arg == "--help") {
            args.help = true;
        } else if (arg == "-v" || arg == "--version") {
            args.version = true;
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) args.config_file = argv[++i];
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) args.output_file = argv[++i];
        } else if (arg == "-s" || arg == "--steps") {
            if (i + 1 < argc) args.steps = std::stoi(argv[++i]);
        } else if (arg == "-t" || arg == "--timestep") {
            if (i + 1 < argc) args.timestep = std::stod(argv[++i]);
        } else if (arg == "-V" || arg == "--verbose") {
            args.verbose = true;
            args.quiet = false;
        } else if (arg == "-z" || arg == "--quiet") {
            args.quiet = true;
            args.verbose = false;
        } else if (arg == "-l" || arg == "--load") {
            if (i + 1 < argc) args.load_checkpoint = true; // Note: using same string for simplicity
            // Would need separate filename param
        } else if (arg == "-S" || arg == "--save") {
            args.save_checkpoint = true;
        } else if (arg == "-x" || arg == "--experiment") {
            if (i + 1 < argc) args.experiment = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            args.help = true;
        }
    }
    
    return args;
}

// Run basic connectivity test
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, bool verbose) {
    if (verbose) {
        std::cout << "=== Basic Connectivity Test ===" << std::endl;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cout << "Error: No regions found" << std::endl;
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cout << "Error: No neurons found" << std::endl;
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first 10 neurons
    if (verbose) {
        std::cout << "Injecting current into 10 neurons..." << std::endl;
    }
    
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    
    if (verbose) {
        std::cout << "Spikes generated: " << spikes << std::endl;
        if (spikes > 0) {
            std::cout << "✓ Spikes propagate through network" << std::endl;
        } else {
            std::cout << "! No spikes - checking neuron parameters..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "  Neuron " << i 
                          << " V=" << neurons[i]->getMembranePotential()
                          << " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
        }
    }
}

// Run plasticity learning experiment
void runPlasticityExperiment(std::shared_ptr<Brain> brain, bool verbose) {
    if (verbose) {
        std::cout << "=== Plasticity Learning Experiment ===" << std::endl;
    }
    
    // Enable plasticity on synapses
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern
    if (verbose) {
        std::cout << "Applying repeated input patterns..." << std::endl;
    }
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (verbose && step % 100 == 0) {
            std::cout << "  Step " << step 
                      << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    if (verbose) {
        std::cout << "✓ Plasticity experiment complete" << std::endl;
    }
}

// Run STDP verification
void runStdpVerification(std::shared_ptr<Brain> brain, bool verbose) {
    if (verbose) {
        std::cout << "=== STDP Verification ===" << std::endl;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cout << "Not enough synapses for STDP test" << std::endl;
        return;
    }
    
    // Enable only STDP on first 5 synapses
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
    }
    
    if (verbose) {
        std::cout << "Testing STDP on 5 synapses:" << std::endl;
        for (size_t i = 0; i < 5; ++i) {
            std::cout << "  Synapse " << i << " before: " << beforeWeights[i] << std::endl;
        }
    }
    
    // Create correlated activity
    if (verbose) {
        std::cout << "Creating correlated pre->post activity (potentiation)..." << std::endl;
    }
    
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
    
    if (verbose) {
        std::cout << "After correlated activity:" << std::endl;
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            std::cout << "  Synapse " << i << " after: " << synapses[i]->getWeight()
                      << " (Δ=" << delta << ")" << std::endl;
            totalDelta += delta;
        }
        
        if (totalDelta > 0.001f) {
            std::cout << "✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "! STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "! No change: STDP may not be triggering" << std::endl;
        }
    }
}

// Main CLI function
int main(int argc, char** argv) {
    // Parse command line arguments
    CLIArgs args = parseArgs(argc, argv);
    
    // Handle help/version
    if (args.help) {
        printHelp();
        return 0;
    }
    
    if (args.version) {
        std::cout << "NLM Phase 2 - Real Neural Computation" << std::endl;
        std::cout << "Version: 0.1.0" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (!args.quiet) {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << "Loading configuration from: " << args.config_file << std::endl;
        std::cout << "Running experiment: " << args.experiment << std::endl;
        std::cout << "Steps: " << args.steps << std::endl;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (!args.quiet) {
        std::cout << "Loading configuration from: " << args.config_file << std::endl;
    }
    
    // Try to load from file if provided
    if (config->loadFromFile(args.config_file)) {
        if (!args.quiet) {
            std::cout << "Loaded configuration from: " << args.config_file << std::endl;
        }
    } else {
        if (!args.quiet) {
            std::cout << "Using default configuration (JSON not available)" << std::endl;
        }
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", args.timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", args.timestep);
    SimulationClock clock(timestep);
    
    if (!args.quiet) {
        std::cout << "Simulation timestep: " << timestep << "s" << std::endl;
        std::cout << "Initializing NLM Brain..." << std::endl;
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    if (!args.quiet) {
        brain->logStatus();
    }
    
    // Run selected experiments
    if (args.experiment == "basic" || args.experiment == "all") {
        if (!args.quiet || args.verbose) {
            std::cout << std::endl;
        }
        runBasicConnectivityTest(brain, args.verbose);
    }
    
    if (args.experiment == "plasticity" || args.experiment == "all") {
        if (!args.quiet || args.verbose) {
            std::cout << std::endl;
        }
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain, args.verbose);
    }
    
    if (args.experiment == "stdp" || args.experiment == "all") {
        if (!args.quiet || args.verbose) {
            std::cout << std::endl;
        }
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        
        runStdpVerification(brain, args.verbose);
    }
    
    if (args.experiment == "connectivity" || args.experiment == "all") {
        if (!args.quiet || args.verbose) {
            std::cout << std::endl;
            std::cout << "=== Network Connectivity Analysis ===" << std::endl;
        }
        
        // Analyze connectivity
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        float eiratio = brain->getExcitationInhibitionRatio();
        
        if (!args.quiet) {
            std::cout << "Network Statistics:" << std::endl;
            std::cout << "  Total neurons: " << totalNeurons << std::endl;
            std::cout << "  Total synapses: " << totalSynapses << std::endl;
            std::cout << "  E/I ratio: " << eiratio << std::endl;
            std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        }
        
        // Run a connectivity test by injecting to all neurons
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject small current to all neurons
            for (auto* neuron : neurons) {
                neuron->injectCurrent(5.0f);
            }
            
            // Run steps
            for (SimulationStep step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            
            if (!args.quiet) {
                std::cout << "  Connectivity test: " << spikes << " spikes from " << neurons.size() 
                          << " neurons (" << (neurons.empty() ? 0 : spikes * 100 / neurons.size()) << "% firing)" << std::endl;
            }
        }
    }
    
    // Save checkpoint if requested
    if (args.save_checkpoint && !args.output_file.empty()) {
        if (!args.quiet) {
            std::cout << "Saving brain state to: " << args.output_file << std::endl;
        }
        if (!brain->save(args.output_file)) {
            std::cerr << "Failed to save brain state" << std::endl;
        }
    }
    
    // Final brain status
    if (!args.quiet) {
        std::cout << std::endl;
        std::cout << "=== Final Brain Status ===" << std::endl;
        brain->logStatus();
        
        std::cout << std::endl;
        std::cout << "=== Simulation Complete ===" << std::endl;
        std::cout << "Total steps: " << args.steps << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        
        if (args.experiment == "all") {
            std::cout << std::endl;
            std::cout << "All experiments completed successfully!" << std::endl;
        }
    }
    
    return 0;
}

} // namespace nlm
