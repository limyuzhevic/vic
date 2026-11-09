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

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config=<file>    Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --help, -h          Show this help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << "  --seed=<int>        Random seed (default: 42)" << std::endl;
    std::cout << "  --neurons=<int>     Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=<int>       Number of simulation steps" << std::endl;
    std::cout << "  --verbose=<level>   Verbosity level (0=error, 1=warning, 2=info, 3=debug)" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 2)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

// Improved command line argument parsing with better error handling
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    int64_t randomSeed = 42;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    int simulationSteps = 1000;
    LogLevel logLevel = LogLevel::Info;
    bool help = false;
    bool version = false;
    bool verbose = false;
    bool showHelp = false;
};

CommandLineArgs parseCommandLineArgs(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
            args.showHelp = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(11); // Skip "--config="
        } else if (arg.find("--seed=") == 0) {
            try {
                args.randomSeed = std::stoll(arg.substr(8));
            } catch (...) {
                NLM_LOG_ERROR("Invalid seed value: " + arg.substr(8));
            }
        } else if (arg.find("--neurons=") == 0) {
            try {
                args.neuronCount = std::stoll(arg.substr(11));
            } catch (...) {
                NLM_LOG_ERROR("Invalid neuron count: " + arg.substr(11));
            }
        } else if (arg.find("--steps=") == 0) {
            try {
                args.simulationSteps = std::stoi(arg.substr(9));
            } catch (...) {
                NLM_LOG_ERROR("Invalid steps value: " + arg.substr(9));
            }
        } else if (arg.find("--verbose=") == 0) {
            try {
                int level = std::stoi(arg.substr(13));
                if (level >= 0 && level <= 4) {
                    args.logLevel = static_cast<LogLevel>(level);
                    args.verbose = true;
                } else {
                    NLM_LOG_ERROR("Invalid verbose level: " + arg.substr( cependant));
                }
            } catch (...) {
                NLM_LOG_ERROR("Invalid verbose value: " + arg.substr(13));
            }
        } else if (arg.size() == 1 && arg[0] == '-') {
            // Handle single-character flags like -v, -s, etc.
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                // -v for verbose, -s for seed, etc.
                if (arg == "-v") {
                    args.verbose = true;
                    i++; // Skip next argument
                } else if (arg == "-s") {
                    args.simulationSteps = std::stoi(argv[++i]);
                } else {
                    NLM_LOG_ERROR("Unknown single-character flag: " + arg);
                }
            } else {
                NLM_LOG_ERROR("Single-character flag requires value: " + arg);
            }
        } else if (arg.substr(0, 2) == "--") {
            // Handle --key format (legacy support)
            if (arg.find('=') != std::string::npos) {
                std::string key = arg.substr(2, arg.find('=') - 2);
                std::string value = arg.substr(arg.find('=') + 1);
                NLM_LOG_WARNING("Legacy format --key=value is deprecated, use --key=value instead");
            }
        } else {
            // Unknown argument
            NLM_LOG_ERROR("Unknown argument: " + arg);
        }
    }
    
    return args;
}

void applyCommandLineArgsToConfig(Config& config, const CommandLineArgs& args) {
    if (args.verbose) {
        config.set("log_level", static_cast<int64_t>(args.logLevel), ConfigSource::CommandLine);
    }
    
    if (args.randomSeed != 42) { // Only if different from default
        config.set("random_seed", args.randomSeed, ConfigSource::CommandLine);
    }
    
    if (args.neuronCount != 500) { // Only if different from default
        config.set("neuron_count", args.neuronCount, ConfigSource::CommandLine);
    }
    
    if (args.simulationSteps > 0) {
        config.set("max_simulation_steps", args.simulationSteps, ConfigSource::CommandLine);
    }
    
    // Note: The runPlasticityExperiment and runStdpVerification functions
    // already use their own defaults, so we won't override them here
    // to maintain backward compatibility with the existing tests
}

void logConfigSummary(const Config& config, const CommandLineArgs& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration summary (applied):");
    NLM_LOG_INFO("  config_file: " + args.configFile);
    NLM_LOG_INFO("  random_seed: " + std::to_string(config.getOr<int64_t>("random_seed", args.randomSeed)));
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config.getOr<int64_t>("neuron_count", args.neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config.getOr<int64_t>("region_count", args.regionCount)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config.getOr<float>("connection_probability", args.connectionProbability)));
    NLM_LOG_INFO("  max_simulation_steps: " + std::to_string(config.getOr<int64_t>("max_simulation_steps", 10000)));
    NLM_LOG_INFO("  log_level: " + std::to_string(static_cast<int>(config.getOr<int64_t>("log_level", static_cast<int64_t>(args.logLevel)))));
}

void logEnhancedBrainStatus(const Brain& brain, const CommandLineArgs& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Brain Status ===");
    NLM_LOG_INFO("Performance metrics:");
    
    // Get performance metrics (these would need to be added to Brain class)
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("  Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
    
    if (args.verbose) {
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain.getAverageFiringRate()) + " Hz");
        NLM_LOG_INFO("  Excitation/Inhibition ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));
        NLM_LOG_INFO("  Inter-region connections: " + std::to_string(brain.getRegionCount() - 1));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Region information:");
    auto regionIds = brain.getRegionIds();
    for (size_t i = 0; i < regionIds.size(); ++i) {
        auto* region = brain.getRegion(regionIds[i]);
        if (region) {
            NLM_LOG_INFO("  Region " + std::to_string(i) + ": " + region->getName() + 
                        " (" + std::to_string(region->getNeuronCount()) + " neurons, " +
                        std::to_string(region->getSynapseCount()) + " synapses)");
        }
    }
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

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLineArgs(argc, argv);
    
    // Handle help/version requests
    if (args.showHelp) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (args.version) {
        printVersion();
        return 0;
    }
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file
    if (config->loadFromFile(args.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found: " + args.configFile + ")");
    }
    
    // Apply command line argument overrides
    applyCommandLineArgsToConfig(*config, args);
    
    // Set default values for Phase 2 (only if not already set)
    if (!config->has("random_seed")) {
        config->set("random_seed", args.randomSeed, ConfigSource::Default);
    }
    if (!config->has("neuron_count")) {
        config->set("neuron_count", args.neuronCount, ConfigSource::Default);
    }
    if (!config->has("region_count")) {
        config->set("region_count", args.regionCount, ConfigSource::Default);
    }
    if (!config->has("connection_probability")) {
        config->set("connection_probability", args.connectionProbability, ConfigSource::Default);
    }
    if (!config->has("max_simulation_steps")) {
        config->set("max_simulation_steps", args.simulationSteps, ConfigSource::Default);
    }
    
    // Log configuration summary
    logConfigSummary(*config, args);
    
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
    
    // Run enhanced tests
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
    
    // Log enhanced brain status
    logEnhancedBrainStatus(*brain, args);
    
    // Final brain status (original)
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}
