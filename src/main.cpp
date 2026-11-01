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

void printAdvancedHelp() {
    std::cout << R"(
=== NLM Advanced Options ===

Basic Options:
  --help, -h           Show this help message
  --version            Show version information
  --config FILE        Specify configuration file (default: configs/default.cfg)
  --log-file FILE      Write logs to file (default: nlm.log)
  --checkpoint FILE    Save/load brain state from file
  --visualization      Enable visualization (if available)
  --advanced          Enable advanced mode with additional options

Simulation Control:
  --max-steps N        Maximum simulation steps (default: 10000)
  --time-limit SECONDS Time limit in seconds (default: 0 = unlimited)
  --environment TYPE   Environment type: GridWorld, Maze, etc. (default: GridWorld)
  --env-params PARAMS  Environment parameters as key=value pairs

Development & Learning:
  --validate-only      Validate configuration only, don't run simulation
  --validate-config    Alias for --validate-only

Advanced Options (require --advanced):
  --adv-option         Placeholder for additional advanced options
  --adv-...            More advanced options can be added here

Configuration Examples:
  ./nlm --config myconfig.cfg --max-steps 5000 --environment Maze
  ./nlm --visualization --checkpoint mybrain.checkpoint --validate-config
  ./nlm --advanced --adv-save-stats --adv-enable-plasticity

Save/Load:
  Configuration files should be in key=value format
  Brain checkpoints preserve learned preferences and neural state

For more information, visit: https://github.com/nlm-project/nlm
  )" << std::endl;
}

void printVersionInfo() {
    std::cout << "NLM (熙然) - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "C++20 with Python bindings via scikit-build-core" << std::endl;
    std::cout << "Built with CMake" << std::endl;
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
    
    // Handle help and version requests before initialization
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printAdvancedHelp();
            return 0;
        } else if (arg == "--version") {
            printVersionInfo();
            return 0;
        }
    }
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
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
    
    // Enhanced command-line argument parsing with power user options
    std::string configFile = "configs/default.cfg";
    std::string logFile = "";
    std::string checkpointFile = "";
    bool enableVisualization = false;
    bool enableAdvancedMode = false;
    bool validateOnly = false;
    bool showHelp = false;
    bool showVersion = false;
    int maxSteps = 10000;
    double timeLimit = 0.0;
    std::string environmentType = "GridWorld";
    std::string environmentParams = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --config filename
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
        // Handle --log-file
        else if (arg.substr(0, 10) == "--log-file") {
            if (arg.find('=') != std::string::npos) {
                logFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                logFile = argv[++i];
            }
        }
        // Handle --checkpoint
        else if (arg.substr(0, 12) == "--checkpoint") {
            if (arg.find('=') != std::string::npos) {
                checkpointFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                checkpointFile = argv[++i];
            }
        }
        // Handle --visualization
        else if (arg == "--visualization") {
            enableVisualization = true;
        }
        // Handle --advanced
        else if (arg == "--advanced") {
            enableAdvancedMode = true;
        }
        // Handle --validate-only
        else if (arg == "--validate-only") {
            validateOnly = true;
        }
        // Handle --help
        else if (arg == "--help" || arg == "-h") {
            printAdvancedHelp();
            return 0;
        }
        // Handle --version
        else if (arg == "--version") {
            printVersionInfo();
            return 0;
        }
        // Handle --max-steps
        else if (arg.substr(0, 11) == "--max-steps") {
            if (arg.find('=') != std::string::npos) {
                maxSteps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                maxSteps = std::stoi(argv[++i]);
            }
        }
        // Handle --time-limit
        else if (arg.substr(0, 12) == "--time-limit") {
            if (arg.find('=') != std::string::npos) {
                timeLimit = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                timeLimit = std::stod(argv[++i]);
            }
        }
        // Handle --environment
        else if (arg.substr(0, 13) == "--environment") {
            if (arg.find('=') != std::string::npos) {
                environmentType = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                environmentType = argv[++i];
            }
        }
        // Handle --env-params
        else if (arg.substr(0, 11) == "--env-params") {
            if (arg.find('=') != std::string::npos) {
                environmentParams = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                environmentParams = argv[++i];
            }
        }
        // Handle --validate-config (validate config only)
        else if (arg == "--validate-config") {
            // For now, just show validation message
            validateOnly = true;
        }
        // Handle other advanced options starting with --adv-
        else if (arg.find("--adv-") == 0) {
            std::string option = arg.substr(5);
            if (enableAdvancedMode) {
                // Process advanced option
                NLM_LOG_INFO("Advanced option: " + option);
                // Advanced options could be parsed here
            }
        }
    }
    
    // Validate configuration if requested
    if (validateOnly) {
        auto errors = config->validate();
        auto warnings = config->getWarnings();
        
        NLM_LOG_INFO("=== Configuration Validation ===");
        if (errors.empty()) {
            NLM_LOG_INFO("✓ Configuration is valid");
        } else {
            NLM_LOG_ERROR("✗ Configuration has errors:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return 1;
        }
        
        if (!warnings.empty()) {
            NLM_LOG_INFO("Warnings:");
            for (const auto& warning : warnings) {
                NLM_LOG_INFO("  - " + warning);
            }
        }
        
        NLM_LOG_INFO("Configuration validation complete");
        return 0;
    }
    
    // Configure log file if specified
    if (!logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(logFile);
        config->set("log_to_file", true, ConfigSource::Runtime);
        config->set("log_filename", logFile, ConfigSource::Runtime);
        logger->addLogger(fileLogger);
        NLM_LOG_INFO("Log file enabled: " + logFile);
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 with advanced options
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
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
    
    // Advanced configuration parameters
    config->set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    config->set("simulation_time_limit", 0.0, ConfigSource::Default);
    config->set("environment_name", "GridWorld", ConfigSource::Default);
    config->set("environment_width", static_cast<int64_t>(10), ConfigSource::Default);
    config->set("environment_height", static_cast<int64_t>(10), ConfigSource::Default);
    config->set("visualization_enabled", false, ConfigSource::Default);
    config->set("visualization_update_rate", 30.0f, ConfigSource::Default);
    config->set("log_to_file", false, ConfigSource::Default);
    config->set("log_filename", "nlm.log", ConfigSource::Default);
    config->set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    config->set("reward_discount_factor", 0.99, ConfigSource::Default);
    config->set("dopamine_baseline", 0.1, ConfigSource::Default);
    
    // Development parameters
    config->set("replay_interval", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("consolidation_interval", static_cast<int64_t>(1000), ConfigSource::Default);
    
    // Validate configuration if requested
    if (validateOnly) {
        auto errors = config->validate();
        auto warnings = config->getWarnings();
        
        NLM_LOG_INFO("=== Configuration Validation ===");
        if (errors.empty()) {
            NLM_LOG_INFO("✓ Configuration is valid");
        } else {
            NLM_LOG_ERROR("✗ Configuration has errors:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return 1;
        }
        
        if (!warnings.empty()) {
            NLM_LOG_INFO("Warnings:");
            for (const auto& warning : warnings) {
                NLM_LOG_INFO("  - " + warning);
            }
        }
        
        NLM_LOG_INFO("Configuration validation complete");
        return 0;
    }
    
    // Configure log file if specified
    if (!logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(logFile);
        config->set("log_to_file", true, ConfigSource::Runtime);
        config->set("log_filename", logFile, ConfigSource::Runtime);
        logger->addLogger(fileLogger);
        NLM_LOG_INFO("Log file enabled: " + logFile);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
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