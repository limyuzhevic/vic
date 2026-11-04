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
#include <cstring>
#include <map>
#include <algorithm>

using namespace nlm;

// Forward declarations for demo functions
namespace demo {
    void printBanner();
    void runPhase2Demo(std::shared_ptr<Brain> brain, int steps);
    void runPhase3Demo(std::shared_ptr<Brain> brain, int steps);
    void runPhase4Demo(std::shared_ptr<Brain> brain, int steps);
    void runPhase5Demo(std::shared_ptr<Brain> brain, int steps);
    void runPhase6Demo(std::shared_ptr<Brain> brain, int steps);
    
    // Test functions from original main.cpp
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
}

// Command-line argument parser
struct NLMArgs {
    std::string phase = "2";
    std::string demoMode = "full";
    std::string configFile = "configs/default.cfg";
    int steps = 1000;
    int neurons = 1000;
    bool verbose = false;
    bool quiet = false;
    std::string checkpointFile = "";
    bool showHelp = false;
};

NLMArgs parseArguments(int argc, char** argv) {
    NLMArgs args;
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--phase" || arg == "-p") {
            if (i + 1 < argc) args.phase = argv[++i];
        } else if (arg == "--demo" || arg == "-d") {
            if (i + 1 < argc) args.demoMode = argv[++i];
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) args.configFile = argv[++i];
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) args.steps = std::stoi(argv[++i]);
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) args.neurons = std::stoi(argv[++i]);
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--checkpoint" || arg == "-ck") {
            if (i + 1 < argc) args.checkpointFile = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg.substr(0, 2) == "--") {
            // Try to parse as --key=value
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                if (key == "--phase") args.phase = value;
                else if (key == "--demo") args.demoMode = value;
                else if (key == "--config") args.configFile = value;
                else if (key == "--steps") args.steps = std::stoi(value);
                else if (key == "--neurons") args.neurons = std::stoi(value);
            }
        }
    }
    
    return args;
}

void printHelp() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 6.0.0 (Phase 6)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Phase Selection:" << std::endl;
    std::cout << "  -p, --phase PHASE          Phase to run (2, 3, 4, 5, or 6)" << std::endl;
    std::cout << "  -d, --demo MODE           Demo mode: 'full', 'quick', 'test', or 'analysis'" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  -c, --config FILE          Configuration file (JSON, YAML, or simple key=value)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Simulation Control:" << std::endl;
    std::cout << "  -s, --steps STEPS         Number of simulation steps to run" << std::endl;
    std::cout << "  -n, --neurons NEURONS     Number of neurons in the brain" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Output Options:" << std::endl;
    std::cout << "  -v, --verbose              Verbose output (debug level)" << std::endl;
    std::cout << "  -q, --quiet                Quiet output (error level only)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Checkpointing:" << std::endl;
    std::cout << "  -ck, --checkpoint FILE     Save/load brain state to file" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --phase 2                   Run Phase 2 demo" << std::endl;
    std::cout << "  nlm --phase 6 --steps 5000      Run Phase 6 with 5000 steps" << std::endl;
    std::cout << "  nlm --config my_config.json     Load custom JSON configuration" << std::endl;
    std::cout << "  nlm --checkpoint brain.bin       Save brain state to file" << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    NLMArgs args = parseArguments(argc, argv);
    
    // Show help and exit
    if (args.showHelp) {
        printHelp();
        return 0;
    }
    
    // Set logging level based on verbosity
    LogLevel logLevel = LogLevel::Info;
    if (args.quiet) {
        logLevel = LogLevel::Error;
    } else if (args.verbose) {
        logLevel = LogLevel::Debug;
    }
    
    Logger::getGlobal().setLevel(logLevel);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from file
    if (config->loadFromFile(args.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (failed to load: " + args.configFile + ")");
    }
    
    // Override configuration from command line arguments
    config->loadFromArgs(argc, argv);
    
    // Override with command line specific values
    config->set("neuron_count", args.neurons, ConfigSource::CommandLine);
    config->set("simulation_max_steps", args.steps, ConfigSource::CommandLine);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    printBanner();
    
    std::cout << "=== NLM Phase " << args.phase << " ===" << std::endl;
    std::cout << "Loading configuration..." << std::endl;
    
    // Create brain with configuration
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run selected phase demo
    if (args.phase == "2") {
        demo::runPhase2Demo(brain, args.steps);
    } else if (args.phase == "3") {
        demo::runPhase3Demo(brain, args.steps);
    } else if (args.phase == "4") {
        demo::runPhase4Demo(brain, args.steps);
    } else if (args.phase == "5") {
        demo::runPhase5Demo(brain, args.steps);
    } else if (args.phase == "6") {
        demo::runPhase6Demo(brain, args.steps);
    } else {
        NLM_LOG_ERROR("Invalid phase: " + args.phase);
        std::cout << "Valid phases: 2, 3, 4, 5, 6" << std::endl;
        return 1;
    }
    
    // Checkpoint if requested
    if (!args.checkpointFile.empty()) {
        if (brain->save(args.checkpointFile)) {
            NLM_LOG_INFO("Brain state saved to: " + args.checkpointFile);
        } else {
            NLM_LOG_ERROR("Failed to save brain state to: " + args.checkpointFile);
        }
    }
    
    NLM_LOG_INFO("=== NLM Demo Complete ===");
    
    return 0;
}

// Demo namespace implementations
namespace demo {
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
    
    void runPhase2Demo(std::shared_ptr<Brain> brain, int steps) {
        std::cout << "=== Running Phase 2 Demo ===" << std::endl;
        std::cout << "Real neural computation with LIF neurons" << std::endl;
        std::cout << "Running " << steps << " simulation steps..." << std::endl;
        
        // Reset brain for clean demo
        brain->reset();
        brain->initialize();
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain);
        
        // Reset and run plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        // Additional simulation steps if requested
        if (steps > 1000) {
            NLM_LOG_INFO("Running additional simulation steps...");
            for (SimulationStep step = 1000; step < steps; ++step) {
                brain->step(step, step * 0.001);
                if (step % 1000 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
        }
        
        std::cout << "=== Phase 2 Demo Complete ===" << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    }
    
    void runPhase3Demo(std::shared_ptr<Brain> brain, int steps) {
        std::cout << "=== Running Phase 3 Demo ===" << std::endl;
        std::cout << "World interaction and sensory processing" << std::endl;
        
        // Phase 3 demo would include world interaction
        // For now, we'll just show a message
        std::cout << "Phase 3 demo functionality will be added in future updates" << std::endl;
    }
    
    void runPhase4Demo(std::shared_ptr<Brain> brain, int steps) {
        std::cout << "=== Running Phase 4 Demo ===" << std::endl;
        std::cout << "Prediction and working memory" << std::endl;
        
        // Phase 4 demo would include prediction systems
        // For now, we'll just show a message
        std::cout << "Phase 4 demo functionality will be added in future updates" << std::endl;
    }
    
    void runPhase5Demo(std::shared_ptr<Brain> brain, int steps) {
        std::cout << "=== Running Phase 5 Demo ===" << std::endl;
        std::cout << "Performance optimizations and checkpointing" << std::endl;
        
        // Phase 5 demo would include performance optimizations
        // For now, we'll just show a message
        std::cout << "Phase 5 demo functionality will be added in future updates" << std::endl;
    }
    
    void runPhase6Demo(std::shared_ptr<Brain> brain, int steps) {
        std::cout << "=== Running Phase 6 Demo ===" << std::endl;
        std::cout << "Full integration of all brain systems" << std::endl;
        
        // Phase 6 demo would include full integration
        // For now, we'll just show a message
        std::cout << "Phase 6 demo functionality will be added in future updates" << std::endl;
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
        
        // A pattern, not a seizure: stimulate only the first 20 sensory neurons
        // each step. The old code injected 30.0f into *every* sensory neuron
        // 20 times per step (600.0f), so the whole network fired synchronously
        // and every weight saturated to 1.0 - learning was measurable but not
        // selective at all.
        std::vector<NeuronId> patternNeurons;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 30.0f);
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
        
        // Use one real synapse and its own pre/post neurons. The old test
        // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
        // usually unrelated to that pair, so its result said nothing about STDP.
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            NLM_LOG_INFO("  No synapses for STDP test");
            return;
        }
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        NLM_LOG_INFO("  Testing STDP on 1 synapse:");
        NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                     " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                     ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  After correlated activity:");
        NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")");
        
        float totalDelta = delta;
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
}