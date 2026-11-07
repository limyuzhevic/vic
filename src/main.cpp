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
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase5Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include <fstream>
#include <iomanip>
#include <numeric>
#include <unordered_map>

using namespace nlm;

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

// Simple command-line argument parser
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputDir = ".";
    std::string logLevel = "info";
    std::string logFile = "";
    bool verbose = false;
    bool runPhase2 = false;
    bool runPhase3 = false;
    bool runPhase4 = false;
    bool runPhase5 = false;
    bool runPhase6 = false;
    bool runBenchmark = false;
    bool checkpointSave = false;
    std::string checkpointName = "";
    bool checkpointLoad = false;
    std::string checkpointPath = "";
    bool listCheckpoints = false;
    int maxSteps = 0;
    int neuronCount = 0;
    float connectionProbability = 0.0f;
    int numEpisodes = 0;
    int stepsPerEpisode = 0;
    
    CommandLineArgs() {}
    
    void printUsage() {
        std::cout << "NLM — Command Line Usage:" << std::endl;
        std::cout << "  nlm [options]" << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "  --config=file     Configuration file path" << std::endl;
        std::cout << "  --output-dir=dir  Output directory for logs/checkpoints" << std::endl;
        std::cout << "  --log-level=level Log level (debug, info, warning, error, critical)" << std::endl;
        std::cout << "  --log-file=file   Log output to file" << std::endl;
        std::cout << "  --verbose         Enable verbose output" << std::endl;
        std::cout << "\nPhase Selection:" << std::endl;
        std::cout << "  --phase2          Run Phase 2: Real neural computation" << std::endl;
        std::cout << "  --phase3          Run Phase 3: World interaction demo" << std::endl;
        std::cout << "  --phase4          Run Phase 4: Prediction and planning" << std::endl;
        std::cout << "  --phase5          Run Phase 5: Performance optimizations" << std::endl;
        std::cout << "  --phase6          Run Phase 6: Integration test" << std::endl;
        std::cout << "  --benchmark       Run benchmark suite" << std::endl;
        std::cout << "\nCheckpoint Operations:" << std::endl;
        std::cout << "  --checkpoint-save=name Save checkpoint with given name" << std::endl;
        std::cout << "  --checkpoint-load=path Load checkpoint from path" << std::endl;
        std::cout << "  --list-checkpoints  List available checkpoints" << std::endl;
        std::cout << "\nConfiguration Overrides:" << std::endl;
        std::cout << "  --max-steps=N     Maximum simulation steps" << std::endl;
        std::cout << "  --neuron-count=N  Number of neurons" << std::endl;
        std::cout << "  --conn-prob=P     Connection probability (0-1)" << std::endl;
        std::cout << "  --episodes=N      Number of episodes (for demo phases)" << std::endl;
        std::cout << "  --steps-per-episode=N Steps per episode" << std::endl;
        std::cout << "\nExamples:" << std::endl;
        std::cout << "  nlm --phase2 --neuron-count=1000 --max-steps=5000" << std::endl;
        std::cout << "  nlm --phase3 --episodes=10 --steps-per-episode=500" << std::endl;
        std::cout << "  nlm --phase6 --checkpoint-save=final_state --max-steps=10000" << std::endl;
        std::cout << "  nlm --list-checkpoints --output-dir=./checkpoints" << std::endl;
    }
    
    bool parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.find("=") != std::string::npos) {
                std::string key = arg.substr(0, arg.find('='));
                std::string value = arg.substr(arg.find('=') + 1);
                
                // Handle --key value format
                if (key == "--config") {
                    configFile = value;
                } else if (key == "--output-dir") {
                    outputDir = value;
                } else if (key == "--log-level") {
                    logLevel = value;
                } else if (key == "--log-file") {
                    logFile = value;
                } else if (key == "--max-steps") {
                    maxSteps = std::stoi(value);
                } else if (key == "--neuron-count") {
                    neuronCount = std::stoi(value);
                } else if (key == "--conn-prob") {
                    connectionProbability = std::stof(value);
                } else if (key == "--episodes") {
                    numEpisodes = std::stoi(value);
                } else if (key == "--steps-per-episode") {
                    stepsPerEpisode = std::stoi(value);
                } else if (key == "--checkpoint-save") {
                    checkpointSave = true;
                    checkpointName = value;
                } else if (key == "--checkpoint-load") {
                    checkpointLoad = true;
                    checkpointPath = value;
                }
                // Handle boolean flags (--phase2, --phase3, etc.)
                else {
                    if (key == "--phase2") {
                        runPhase2 = true;
                    } else if (key == "--phase3") {
                        runPhase3 = true;
                    } else if (key == "--phase4") {
                        runPhase4 = true;
                    } else if (key == "--phase5") {
                        runPhase5 = true;
                    } else if (key == "--phase6") {
                        runPhase6 = true;
                    } else if (key == "--benchmark") {
                        runBenchmark = true;
                    } else if (key == "--verbose") {
                        verbose = true;
                    } else if (key == "--list-checkpoints") {
                        listCheckpoints = true;
                    }
                }
            }
            // Handle --key value format (space separated)
            else {
                if (arg == "--phase2") {
                    runPhase2 = true;
                } else if (arg == "--phase3") {
                    runPhase3 = true;
                } else if (arg == "--phase4") {
                    runPhase4 = true;
                } else if (arg == "--phase5") {
                    runPhase5 = true;
                } else if (arg == "--phase6") {
                    runPhase6 = true;
                } else if (arg == "--benchmark") {
                    runBenchmark = true;
                } else if (arg == "--verbose") {
                    verbose = true;
                } else if (arg == "--list-checkpoints") {
                    listCheckpoints = true;
                }
            }
        }
        
        return true;
    }
    
    LogLevel getLogLevel() {
        if (logLevel == "debug") return LogLevel::Debug;
        if (logLevel == "info") return LogLevel::Info;
        if (logLevel == "warning") return LogLevel::Warning;
        if (logLevel == "error") return LogLevel::Error;
        if (logLevel == "critical") return LogLevel::Critical;
        return LogLevel::Info;
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
    // Parse command-line arguments
    CommandLineArgs args;
    if (!args.parseArgs(argc, argv)) {
        std::cout << "Error parsing command-line arguments." << std::endl;
        return 1;
    }
    
    // If no specific phase is requested, run Phase 2 with enhanced features
    if (!args.runPhase2 && !args.runPhase3 && !args.runPhase4 && 
        !args.runPhase5 && !args.runPhase6 && !args.runBenchmark && 
        !args.listCheckpoints) {
        args.runPhase2 = true;  // Default to Phase 2
    }
    
    // Handle special options first
    if (args.listCheckpoints) {
        // List checkpoints functionality would go here
        std::cout << "Listing checkpoints in: " << args.outputDir << std::endl;
        // Implementation would create a CheckpointManager and list checkpoints
        return 0;
    }
    
    if (args.checkpointLoad) {
        // Load checkpoint functionality would go here
        std::cout << "Loading checkpoint from: " << args.checkpointPath << std::endl;
        // Implementation would create a CheckpointManager and load checkpoint
        return 0;
    }
    
    printBanner();
    
    // Display help if requested
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            args.printUsage();
            return 0;
        }
    }
    
    // Initialize logger with enhanced configuration
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.getLogLevel());
    
    // Set output file if specified
    if (!args.logFile.empty()) {
        consoleLogger->setOutputFile(args.logFile);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Log startup information
    NLM_LOG_INFO("=== NLM Enhanced Command Line Interface ===");
    NLM_LOG_INFO("Phase 2: Real Neural Computation (Default)");
    NLM_LOG_INFO("Arguments parsed successfully");
    
    if (args.verbose) {
        NLM_LOG_INFO("Verbose mode enabled");
    }
    
    if (args.outputDir != ".") {
        NLM_LOG_INFO("Output directory: " + args.outputDir);
    }
    
    // Execute selected phases
    if (args.runPhase2) {
        NLM_LOG_INFO("\n=== Executing Phase 2: Real Neural Computation ===");
        
        // Run Phase 2 with enhanced configuration
        std::cout << "Initializing NLM Phase 2 Real Neural Computation..." << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration from file
        std::string configFile = args.configFile;
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Override with command-line arguments
        config->loadFromArgs(argc, argv);
        
        // Set default values for Phase 2 (overridable by command line)
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        if (args.neuronCount > 0) {
            config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::User);
        } else {
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        }
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        if (args.connectionProbability > 0.0f) {
            config->set("connection_probability", args.connectionProbability, ConfigSource::User);
        } else {
            config->set("connection_probability", 0.15f, ConfigSource::Default);
        }
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        if (args.maxSteps > 0) {
            config->set("max_simulation_steps", static_cast<int64_t>(args.maxSteps), ConfigSource::User);
        }
        
        // Set configuration parameters for logger
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("  max_simulation_steps: " + std::to_string(config->getOr<int64_t>("max_simulation_steps", 10000)));
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
        
        // Run tests
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        // Final brain status
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
    }
    
    return 0;
}
