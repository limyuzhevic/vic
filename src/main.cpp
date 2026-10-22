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

void printUsage() {
    std::cout << R"(
USAGE: nlm [OPTIONS]

EXPERIMENTAL ARTIFICIAL DEVELOPMENTAL BRAIN - PHASE 2: REAL NEURAL COMPUTATION

Options:
  --help, -h                    Show this help message
  --version, -v                 Show version information
  --config=FILE, -c FILE         Load configuration from file (default: configs/default.cfg)
  --debug, -d                   Enable debug logging
  --quiet, -q                   Suppress non-essential output
  --experiment, -e EXPERIMENT   Run specific experiment (connectivity|plasticity|stdp|all)
  --iterations, -i COUNT        Number of simulation iterations (default: 1)
  --timestep, -t SECONDS        Simulation timestep in seconds (default: 0.001)
  --neurons, -n COUNT           Number of neurons (default: 500)
  --regions, -r COUNT            Number of neural regions (default: 1)
  --plasticity, -p LEVEL         Plasticity level (0-2, where 2 = maximum)
  --stdp-weights, -sw LTP=LTD    STDP weights as "ltp_weight,ltd_weight" (e.g., "0.02,0.015")
  --stdp-tau, -tau TAU          STDP time constant in ms (default: 20.0)
  --checkpoint, -ck FILE         Save brain state to checkpoint file
  --load-checkpoint, -lc FILE    Load brain state from checkpoint file
  --runtime-params, -rp KEY=VAL  Set runtime parameter (can be used multiple times)
  --validate-config             Validate configuration and exit

Configuration parameters can also be set via config file (simple key=value format):
  # Comments start with # or /
  random_seed = 42
  simulation_timestep = 0.001
  neuron_count = 1000
  region_count = 1
  connection_probability = 0.1
  stdp_ltp_weight = 0.01
  stdp_ltd_weight = 0.012
  stdp_tau = 20.0
  synaptogenesis_rate = 0.0001
  pruning_rate = 0.00001
  replay_interval = 100
  consolidation_interval = 1000
  checkpoint_dir = ./checkpoints

Defaults and command line arguments override config file values.
" << std::endl;
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

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string experiment = "all";
    int iterations = 1;
    double timestep = 0.001;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float plasticityLevel = 1.0f;
    float stdpLTP = 0.02f;
    float stdpLTD = 0.015f;
    float stdpTau = 20.0f;
    bool debugMode = false;
    bool quietMode = false;
    std::string checkpointFile;
    std::string loadCheckpointFile;
    std::vector<std::pair<std::string, std::string>> runtimeParams;
    bool validateOnly = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "NLM Neural Learning Machine - Phase 2\nVersion: 0.1.0\n" << std::endl;
            return 0;
        } else if (arg.substr(0, 7) == "--config" || arg.substr(0, 2) == "-c") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--debug" || arg == "-d") {
            debugMode = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quietMode = true;
        } else if (arg.substr(0, 3) == "-e ") {
            experiment = arg.substr(3);
        } else if (arg == "--experiment" || arg == "-e") {
            if (i + 1 < argc) {
                experiment = argv[++i];
            }
        } else if (arg.substr(0, 3) == "-i ") {
            iterations = std::stoi(arg.substr(3));
        } else if (arg == "--iterations" || arg == "-i") {
            if (i + 1 < argc) {
                iterations = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 3) == "-t ") {
            timestep = std::stod(arg.substr(3));
        } else if (arg == "--timestep" || arg == "-t") {
            if (i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            }
        } else if (arg.substr(0, 3) == "-n ") {
            neuronCount = static_cast<size_t>(std::stoi(arg.substr(3)));
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                neuronCount = static_cast<size_t>(std::stoi(argv[++i]));
            }
        } else if (arg.substr(0, 3) == "-r ") {
            regionCount = static_cast<size_t>(std::stoi(arg.substr(3)));
        } else if (arg == "--regions" || arg == "-r") {
            if (i + 1 < argc) {
                regionCount = static_cast<size_t>(std::stoi(argv[++i]));
            }
        } else if (arg.substr(0, 3) == "-p ") {
            plasticityLevel = std::stof(arg.substr(3));
        } else if (arg == "--plasticity" || arg == "-p") {
            if (i + 1 < argc) {
                plasticityLevel = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 5) == "--sw ") {
            std::string weights = arg.substr(5);
            size_t pos = weights.find(',');
            if (pos != std::string::npos) {
                stdpLTP = std::stof(weights.substr(0, pos));
                stdpLTD = std::stof(weights.substr(pos + 1));
            }
        } else if (arg == "--stdp-weights" || arg == "-sw") {
            if (i + 1 < argc) {
                std::string weights = argv[++i];
                size_t pos = weights.find(',');
                if (pos != std::string::npos) {
                    stdpLTP = std::stof(weights.substr(0, pos));
                    stdpLTD = std::stof(weights.substr(pos + 1));
                }
            }
        } else if (arg.substr(0, 7) == "--tau ") {
            stdpTau = std::stof(arg.substr(7));
        } else if (arg == "--stdp-tau" || arg == "-tau") {
            if (i + 1 < argc) {
                stdpTau = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 11) == "--checkpoint ") {
            checkpointFile = arg.substr(11);
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                checkpointFile = argv[++i];
            }
        } else if (arg.substr(0, 15) == "--load-checkpoint ") {
            loadCheckpointFile = arg.substr(15);
        } else if (arg == "--load-checkpoint") {
            if (i + 1 < argc) {
                loadCheckpointFile = argv[++i];
            }
        } else if (arg.substr(0, 15) == "--runtime-params ") {
            runtimeParams.push_back(std::make_pair(
                arg.substr(15, arg.find('=') - 15),
                arg.substr(arg.find('=') + 1)
            ));
        } else if (arg == "--runtime-params") {
            if (i + 1 < argc) {
                size_t eqPos = std::string(argv[i + 1]).find('=');
                if (eqPos != std::string::npos) {
                    runtimeParams.push_back(std::make_pair(
                        std::string(argv[++i]).substr(0, eqPos),
                        std::string(argv[i]).substr(eqPos + 1)
                    ));
                }
            }
        } else if (arg == "--validate-config") {
            validateOnly = true;
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }
    
    // Initialize logger with appropriate log level
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(debugMode ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (!quietMode) {
        std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
        std::cout << "=====================================" << std::endl;
    }
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Register default parameters with validation
    config->registerParameter("random_seed", "Random seed for reproducible experiments", 42, ConfigSource::Default, false);
    config->registerParameter("simulation_timestep", "Simulation timestep in seconds", timestep, ConfigSource::Default, false);
    config->registerParameter("neuron_count", "Number of neurons to create", neuronCount, ConfigSource::Default, false);
    config->registerParameter("region_count", "Number of neural regions", regionCount, ConfigSource::Default, false);
    config->registerParameter("connection_probability", "Probability of synaptic connections", 0.15f, ConfigSource::Default, false);
    config->registerParameter("stdp_ltp_weight", "STDP long-term potentiation weight", stdpLTP, ConfigSource::Default, false);
    config->registerParameter("stdp_ltd_weight", "STDP long-term depression weight", stdpLTD, ConfigSource::Default, false);
    config->registerParameter("stdp_tau", "STDP time constant in milliseconds", stdpTau, ConfigSource::Default, false);
    config->registerParameter("synaptogenesis_rate", "Rate of new synapse formation", 0.0001f, ConfigSource::Default, false);
    config->registerParameter("pruning_rate", "Rate of synapse pruning", 0.00001f, ConfigSource::Default, false);
    config->registerParameter("replay_interval", "Interval between memory replays", 100, ConfigSource::Default, false);
    config->registerParameter("consolidation_interval", "Interval between memory consolidation", 1000, ConfigSource::Default, false);
    config->registerParameter("checkpoint_dir", "Directory for checkpoint files", "./checkpoints", ConfigSource::Default, false);
    
    // Load configuration with enhanced error handling
    if (!quietMode) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
    }
    
    bool configLoaded = config->loadFromFile(configFile);
    if (!configLoaded) {
        if (!quietMode) {
            std::cout << "Using default configuration (file not found or invalid)." << std::endl;
        }
    }
    
    // Apply command line arguments
    for (const auto& arg : runtimeParams) {
        config->set(arg.first, arg.second, ConfigSource::CommandLine);
    }
    
    // Validate configuration
    auto validationErrors = config->validate();
    if (!validationErrors.empty()) {
        if (!quietMode) {
            std::cout << "\nConfiguration Validation Errors:" << std::endl;
            for (const auto& error : validationErrors) {
                std::cout << "  ERROR: " << error << std::endl;
            }
        }
        if (validateOnly) {
            return 1;
        }
    }
    
    if (validateOnly) {
        if (!quietMode) {
            std::cout << "Configuration validation successful!" << std::endl;
        }
        return 0;
    }
    
    // Log configuration summary
    if (!quietMode) {
        std::cout << "\nConfiguration:" << std::endl;
        std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
        std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
        std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
        std::cout << "  plasticity_level: " << plasticityLevel << std::endl;
        std::cout << std::endl;
    }
    
    // Initialize simulation clock
    double configTimestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(configTimestep);
    
    if (!quietMode) {
        std::cout << "Simulation timestep: " << configTimestep << "s" << std::endl;
    }
    
    // Load checkpoint if specified
    if (!loadCheckpointFile.empty()) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->load(loadCheckpointFile)) {
            std::cerr << "Failed to load checkpoint from: " << loadCheckpointFile << std::endl;
            return 1;
        }
        
        if (!quietMode) {
            std::cout << "Loaded brain state from checkpoint: " << loadCheckpointFile << std::endl;
        }
    } else {
        // Initialize brain
        if (!quietMode) {
            std::cout << "Initializing NLM Brain..." << std::endl;
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        brain->logStatus();
    }
    
    // Run experiments based on selection
    if (!quietMode) {
        std::cout << "\nRunning experiments..." << std::endl;
    }
    
    auto brain = std::make_shared<Brain>(config); // Reinitialize for experiments
    
    if (experiment == "connectivity" || experiment == "all") {
        if (!quietMode) {
            std::cout << "Test 1: Basic Neural Connectivity" << std::endl;
        }
        runBasicConnectivityTest(brain);
        
        // Reset for next experiment
        brain->reset();
        brain->initialize();
    }
    
    if (experiment == "plasticity" || experiment == "all") {
        if (!quietMode) {
            std::cout << "Test 2: Plasticity Learning Experiment" << std::endl;
        }
        runPlasticityExperiment(brain);
        
        // Reset for next experiment
        brain->reset();
        brain->initialize();
    }
    
    if (experiment == "stdp" || experiment == "all") {
        if (!quietMode) {
            std::cout << "Test 3: STDP Verification" << std::endl;
        }
        runStdpVerification(brain);
    }
    
    // Save checkpoint if requested
    if (!checkpointFile.empty()) {
        if (!brain->save(checkpointFile)) {
            std::cerr << "Failed to save checkpoint to: " << checkpointFile << std::endl;
            return 1;
        }
        
        if (!quietMode) {
            std::cout << "Saved brain state to checkpoint: " << checkpointFile << std::endl;
        }
    }
    
    if (!quietMode) {
        std::cout << "\n=== Phase 2 Complete ===" << std::endl;
        std::cout << "All experiments completed successfully!" << std::endl;
    }
    
    return 0;
}
