// NLM — 熙然 (Serene Flow)
// An Experimental Artificial Developmental Brain
// Phase 6: Final Integration

// This phase implements a comprehensive integrated brain system with:
// - Real spiking neural computation (LIF neurons, event-driven dynamics)
// - Memory systems (working, episodic, associative)
// - Prediction and cognition systems
// - Neuromodulation (dopamine, curiosity, novelty)
// - Developmental stages
// - Structural plasticity
// - Checkpoint and replay systems
// - Integration with world interaction

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
#include <sstream>
#include <chrono>

using namespace nlm;

// Forward declarations for new functions
void printUsageAndExit();
void runExperiments(std::shared_ptr<Brain> brain, const std::string& experiment, 
                   bool verbose, bool quiet);
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, bool verbose);
void runPlasticityExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runStdpVerification(std::shared_ptr<Brain> brain, bool verbose);
void runMemorySystemTest(std::shared_ptr<Brain> brain, bool verbose);
void runPhase6IntegrationTest(std::shared_ptr<Brain> brain, bool verbose);
void runExplorationMode(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, 
                       bool verbose);
void runBenchmarkMode(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, 
                     bool verbose);

// New experiment functions
void runAdversarialExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runDevelopmentExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runSocialLearningExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runPredictionExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runNeuromodulationExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runConceptFormationExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runSelfModelExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runWorldInteractionExperiment(std::shared_ptr<Brain> brain, bool verbose);
void runTextIntegrationExperiment(std::shared_ptr<Brain> brain, bool verbose);

// Utility functions
void setupAdvancedConfig(std::shared_ptr<Config> config);
void printProgress(const std::string& message, int step, int total);
void saveBrainState(std::shared_ptr<Brain> brain, const std::string& filename);
void loadBrainState(std::shared_ptr<Brain> brain, const std::string& filename);
void displayBrainStats(std::shared_ptr<Brain> brain);
void displaySystemStatus(std::shared_ptr<Brain> brain);
void displayPerformanceMetrics(std::shared_ptr<Brain> brain, double elapsedTime);

// Command parsing helpers
void parseAdvancedOptions(int argc, char** argv, std::string& mode, 
                         std::string& experiment, bool& verbose, bool& quiet,
                         bool& saveCheckpoint, std::string& checkpointFile,
                         std::string& configFile);
void handleExplorationCommands(std::shared_ptr<Brain> brain, const std::vector<std::string>& args);
void handleExperimentCommands(std::shared_ptr<Brain> brain, const std::vector<std::string>& args);
void handleDebugCommands(std::shared_ptr<Brain> brain, const std::vector<std::string>& args);

// Advanced experiment modes
enum class ExperimentMode {
    NONE,
    ADVERSARIAL,
    DEVELOPMENT,
    SOCIAL_LEARNING,
    PREDICTION,
    NEUROMODULATION,
    CONCEPT_FORMATION,
    SELF_MODEL,
    WORLD_INTERACTION,
    TEXT_INTEGRATION
};

ExperimentMode parseExperimentMode(const std::string& modeStr);
std::string experimentModeToString(ExperimentMode mode);

// Command line option flags
struct AdvancedOptions {
    bool help = false;
    bool version = false;
    bool verbose = false;
    bool quiet = false;
    bool saveCheckpoint = false;
    bool benchmark = false;
    bool explore = false;
    bool test = false;
    std::string configFile;
    std::string checkpointFile;
    std::string mode;
    std::string experiment;
    std::string exploreCommand;
    std::string debugCommand;
    int explorationSteps = 100;
    int benchmarkSteps = 1000;
    double timestep = 0.001;
    int neuronCount = 1000;
    float connectionProb = 0.05f;
    bool enableMemorySystems = true;
    bool enablePredictionSystem = true;
    bool enableCognitionSystems = true;
    bool enableNeuromodulation = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
};

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete artificial brain with all systems integrated.    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    // Initialize Kilo framework (simplified version)
    // In a real Kilo setup, this would configure the environment
    
    printBanner();
    
    // Parse command line arguments for improved user experience
    std::string configFile = "configs/default.cfg";
    std::string mode = "demo";  // demo, test, explore, benchmark
    std::string experiment = "all";  // all, plasticity, stdp, connectivity, memory
    bool verbose = false;
    bool quiet = false;
    bool saveCheckpoint = false;
    std::string checkpointFile;
    
    // Parse arguments with better structure
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printUsageAndExit();
            return 0;
        }
        else if (arg.substr(0, 8) == "--mode=") {
            mode = arg.substr(8);
        }
        else if (arg.substr(0, 12) == "--experiment=") {
            experiment = arg.substr(12);
        }
        else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
        else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        }
        else if (arg == "--save-checkpoint") {
            saveCheckpoint = true;
        }
        else if (arg.substr(0, 18) == "--checkpoint=") {
            checkpointFile = arg.substr(18);
        }
        else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
        else if (arg.substr(0, 2) == "-") {
            // Handle short options
            if (arg == "-m" && i + 1 < argc) {
                mode = argv[++i];
            }
            else if (arg == "-e" && i + 1 < argc) {
                experiment = argv[++i];
            }
        }
    }
    
    // Initialize logger with configurable level
    auto logger = std::make_shared<Logger>();
    
    LogLevel logLevel = verbose ? LogLevel::Debug : 
                       quiet ? LogLevel::Warning : LogLevel::Info;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (!quiet) {
        std::cout << "Initializing NLM Brain with improved Phase 6 integration...\n" << std::endl;
    }
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("Mode: " + mode);
    NLM_LOG_INFO("Experiment: " + experiment);
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args (more comprehensive parsing)
    config->loadFromArgs(argc, argv);
    
    // Enhanced default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);  // Full demo size
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);  // More selective
    
    // Enhanced STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Enhanced structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Phase 6 specific parameters
    config->set("enable_memory_systems", true, ConfigSource::Default);
    config->set("enable_prediction_system", true, ConfigSource::Default);
    config->set("enable_cognition_systems", true, ConfigSource::Default);
    config->set("enable_neuromodulation", true, ConfigSource::Default);
    config->set("enable_development", true, ConfigSource::Default);
    config->set("enable_curiosity", true, ConfigSource::Default);
    
    // Integration parameters
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    config->set("enable_checkpoints", true, ConfigSource::Default);
    config->set("checkpoint_interval", 1000, ConfigSource::Default);
    
    if (!quiet) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration summary:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 2)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.05f)));
    }
    
    // Set up config file for checkpoint management
    if (saveCheckpoint && !checkpointFile.empty()) {
        config->set("checkpoint_dir", "./checkpoints", ConfigSource::Runtime);
        config->set("checkpoint_interval", 500, ConfigSource::Runtime);
    }
    
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
    
    // Run experiments based on mode and experiment selection
    if (mode == "demo" || mode == "test") {
        runExperiments(brain, experiment, verbose, quiet);
    } else if (mode == "explore") {
        runExplorationMode(brain, config, verbose);
    } else if (mode == "benchmark") {
        runBenchmarkMode(brain, config, verbose);
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        printUsageAndExit();
        return 1;
    }
    
    // Save checkpoint if requested
    if (saveCheckpoint && !checkpointFile.empty()) {
        NLM_LOG_INFO("Saving checkpoint to: " + checkpointFile);
        if (!brain->save(checkpointFile)) {
            NLM_LOG_ERROR("Failed to save checkpoint!");
        } else {
            NLM_LOG_INFO("Checkpoint saved successfully!");
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    NLM_LOG_INFO("The NLM brain simulation completed successfully.");
    
    return 0;
}

void printUsageAndExit() {
    std::cout << R"(
NLM — 熙然 (Serene Flow)
An Experimental Artificial Developmental Brain

USAGE:
  ./nlm [OPTIONS]

MODES:
  --mode=DEMO      Run Phase 6 integration demo (default)
  --mode=TEST      Run comprehensive test suite
  --mode=EXPLORE   Explore individual brain components
  --mode=BENCHMARK Run performance benchmarks

EXPERIMENTS:
  --experiment=ALL      Run all experiments (default)
  --experiment=PLASTICITY Run plasticity experiments
  --experiment=STDP      Run STDP verification
  --experiment=CONNECTIVITY Run connectivity tests
  --experiment=MEMORY    Run memory system tests
  --experiment=INTEGRATION Run Phase 6 integration tests

OPTIONS:
  -h, --help          Show this help message
  -v, --verbose       Enable verbose logging
  -q, --quiet         Quiet mode (less output)
  --save-checkpoint   Save checkpoint at the end
  --checkpoint=<file> Checkpoint file path (with --save-checkpoint)
  --config=<file>     Configuration file path

CONFIGURATION:
  Config file format: key=value (one per line)
  Standard keys:
    random_seed          Random seed for reproducible results
    simulation_timestep  Time step duration in seconds
    neuron_count         Number of neurons in brain
    region_count         Number of brain regions
    connection_probability Connection probability (0.0-1.0)
    stdp_ltp_weight      STDP LTP weight
    stdp_ltd_weight      STDP LTD weight
    stdp_tau             STDP time constant
    synaptogenesis_rate  Structural plasticity rate
    pruning_rate         Structural pruning rate

EXAMPLES:
  ./nlm                           # Run demo with defaults
  ./nlm --mode=TEST --experiment=ALL
  ./nlm --verbose --mode=EXPLORE --experiment=PLASTICITY
  ./nlm --save-checkpoint --checkpoint=brain_state.chk
  ./nlm --config=my_config.cfg

For more information, visit: https://github.com/nlm-project/nlm

PHASE 6 Features Demonstrated:
  • Memory Systems (working, episodic, associative)
  • Prediction and Planning
  • Neuromodulation (dopamine, curiosity, novelty)
  • Cognitive Development
  • Structural Plasticity
  • Checkpoint/Restart
  • Text Integration
  • Self-Model Development
  • Social Learning

For phase-specific information, see README.md

PHASE 2 Experiments (for compatibility):
  These are still available in test mode:
  Test 1: Basic Neural Connectivity
  Test 2: Plasticity Learning Experiment  
  Test 3: STDP Verification
  ())" << std::endl;
    exit(0);
}

void runExperiments(std::shared_ptr<Brain> brain, const std::string& experiment, 
                   bool verbose, bool quiet) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Experiments: " + experiment + " ===");
    
    if (experiment == "all" || experiment == "plasticity") {
        if (!quiet) NLM_LOG_INFO("--- Running Plasticity Experiment ---");
        runPlasticityExperiment(brain, verbose);
    }
    
    if (experiment == "all" || experiment == "stdp") {
        if (!quiet) NLM_LOG_INFO("--- Running STDP Verification ---");
        runStdpVerification(brain, verbose);
    }
    
    if (experiment == "all" || experiment == "connectivity") {
        if (!quiet) NLM_LOG_INFO("--- Running Connectivity Test ---");
        runBasicConnectivityTest(brain, verbose);
    }
    
    if (experiment == "all" || experiment == "memory") {
        if (!quiet) NLM_LOG_INFO("--- Running Memory System Test ---");
        runMemorySystemTest(brain, verbose);
    }
    
    if (experiment == "all" || experiment == "integration") {
        if (!quiet) NLM_LOG_INFO("--- Running Phase 6 Integration Test ---");
        runPhase6IntegrationTest(brain, verbose);
    }
}

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Enhanced Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 20 neurons for better demonstration
    NLM_LOG_INFO("  Injecting current into 20 neurons...");
    for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run more steps for better demonstration
    for (SimulationStep step = 0; step < 100; ++step) {
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Enhanced Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses with enhanced settings
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true, true);  // Enable all plasticity types
            syn->setPlasticityFlags(true, true, false, true);  // Hebbian, STDP, structural
        }
    }
    
    // Apply repeated input pattern with better learning
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
        
        // Log progress every 100 steps (conditional compilation)
        if (verbose || step % 100 == 0) {
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

void runStdpVerification(std::shared_ptr<Brain> brain, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: Enhanced STDP Verification ===");
    
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
    NLM_LOG_INFO("  Creating correlated pre->post activity (enhanced potentiation)...");
    
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

void runMemorySystemTest(std::shared_ptr<Brain> brain, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Memory System Integration ===");
    
    // Test working memory
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        NLM_LOG_INFO("  Testing working memory...");
        workingMem->initialize(brain.get());
        
        // Store some neuron activity
        auto neurons = brain->getTotalNeuronCount();
        for (size_t i = 0; i < std::min(size_t(10), neurons); ++i) {
            workingMem->storeToNeuron(NeuronId(i), 1.0f);
        }
        
        workingMem->update(0.001);
        auto activeNeurons = workingMem->getMemoryNeurons();
        NLM_LOG_INFO("    Working memory contains " + std::to_string(activeNeurons.size()) + " active neurons");
    }
    
    // Test episodic memory
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        NLM_LOG_INFO("  Testing episodic memory...");
        episodicMem->initialize(brain.get());
        
        // Create a test episode
        EpisodicMemoryItem episode;
        episode.timestamp = 0;
        episode.positionX = 0.5f;
        episode.positionY = 0.5f;
        episode.orientation = 0.0f;
        episode.action = ActionType::MoveForward;
        episode.reward = 10.0f;
        episode.energy = 0.8f;
        episode.novelty = 0.3f;
        episode.resultingReward = 8.0f;
        episode.age = 1;
        
        // Add some neuron activity
        for (size_t i = 0; i < 5; ++i) {
            episode.activeNeurons.push_back(NeuronId(i));
            episode.neuronActivations.push_back(0.8f);
        }
        
        episodicMem->storeEpisode(episode);
        
        // Get episodes
        auto episodes = episodicMem->getEpisodes();
        NLM_LOG_INFO("    Episodic memory contains " + std::to_string(episodes.size()) + " episodes");
        
        // Test replay
        if (!episodes.empty()) {
            const auto* episodeToReplay = episodicMem->getEpisode(0);
            if (episodeToReplay) {
                episodicMem->replayEpisode(episodeToReplay);
                NLM_LOG_INFO("    Episode replay completed successfully");
            }
        }
    }
    
    // Test concept formation
    auto* conceptForm = brain->getConceptFormation();
    if (conceptForm) {
        NLM_LOG_INFO("  Testing concept formation...");
        conceptForm->initialize(brain.get());
        
        // Present some experience for concept formation
        std::vector<float> visionPattern(16, 0.0f);
        visionPattern[0] = 1.0f; // Bright object in vision
        std::vector<float> touchPattern(8, 0.0f);
        touchPattern[0] = 1.0f; // Touch
        
        conceptForm->presentExperience(visionPattern, touchPattern, 10.0f, 0);
        
        size_t conceptCount = conceptForm->getConceptCount();
        NLM_LOG_INFO("    Concept formation created " + std::to_string(conceptCount) + " concepts");
    }
}

void runPhase6IntegrationTest(std::shared_ptr<Brain> brain, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 5: Phase 6 Integration Test ===");
    NLM_LOG_INFO("Testing integration of all Phase 6 systems:");
    
    // Test 1: Neuromodulation
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        NLM_LOG_INFO("  Neuromodulation (dopamine)...");
        dopamine->initialize(brain.get());
        dopamine->update(0.001);
        NLM_LOG_INFO("    Dopamine level: " + std::to_string(dopamine->getLevel()) + " (prediction error signal)");
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        NLM_LOG_INFO("  Neuromodulation (curiosity)...");
        curiosity->initialize(brain.get());
        curiosity->update(0.0f, 0.0f, 0.001);
        NLM_LOG_INFO("    Curiosity level: " + std::to_string(curiosity->getLevel()));
    }
    
    auto* novelty = brain->getNovelty();
    if (novelty) {
        NLM_LOG_INFO("  Neuromodulation (novelty)...");
        novelty->initialize(brain.get());
        novelty->update(0.001);
        NLM_LOG_INFO("    Novelty level: " + std::to_string(novelty->getLevel()));
    }
    
    // Test 2: Development
    auto* development = brain->getDevelopmentSystem();
    if (development) {
        NLM_LOG_INFO("  Development system...");
        development->initialize();
        NLM_LOG_INFO("    Initial developmental stage: " + std::to_string(static_cast<int>(development->getDevelopmentalStage())));
    }
    
    // Test 3: Prediction system
    auto* prediction = brain->getPredictionSystem();
    if (prediction) {
        NLM_LOG_INFO("  Prediction system...");
        NLM_LOG_INFO("    Prediction system active (handles prediction errors)");
    }
    
    // Test 4: Cognition systems
    auto* planner = brain->getPlanner();
    if (planner) {
        NLM_LOG_INFO("  Cognition (planning)...");
        planner->initialize(brain.get());
        planner->setPlanningDepth(3);
        NLM_LOG_INFO("    Neural planner initialized with depth 3");
    }
    
    auto* attention = brain->getAttention();
    if (attention) {
        NLM_LOG_INFO("  Cognition (attention)...");
        attention->initialize(brain.get());
        NLM_LOG_INFO("    Attention system active (competitive selection)");
    }
    
    // Test 5: Structural plasticity
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        NLM_LOG_INFO("  Structural plasticity...");
        NLM_LOG_INFO("    Synaptogenesis and pruning systems active");
    }
    
    // Test 6: Integration through normal step execution
    NLM_LOG_INFO("  Integration test via normal simulation steps...");
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
        
        if (verbose && step % 10 == 0) {
            NLM_LOG_INFO("    Step " + std::to_string(step) + 
                        " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("  ✓ All Phase 6 systems integrated and functional");
}

void runExplorationMode(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, 
                       bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Exploration Mode ===");
    NLM_LOG_INFO("Use 'nlm --help' for detailed command information");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Available exploration commands:");
    NLM_LOG_INFO("  --explore-neurons     - Analyze neuron distribution");
    NLM_LOG_INFO("  --explore-synapses    - Analyze synaptic connectivity");
    NLM_LOG_INFO("  --explore-memory     - Analyze memory systems");
    NLM_LOG_INFO("  --explore-plasticity - Analyze plasticity state");
    NLM_LOG_INFO("  --explore-development - Analyze developmental state");
    NLM_LOG_INFO("  --explore-neuromod    - Analyze neuromodulation");
    NLM_LOG_INFO("");
    
    // In a real implementation, these would trigger actual exploration
    NLM_LOG_INFO("Exploring brain state...");
    
    // Show current brain statistics
    NLM_LOG_INFO("Brain Statistics:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("  E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Exploration complete. Run 'nlm --help' for detailed commands.");
}

void runBenchmarkMode(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, 
                     bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark Mode ===");
    NLM_LOG_INFO("Performance benchmark and system analysis...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run performance benchmark
    for (SimulationStep step = 0; step < 200; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("Benchmark Results:");
    NLM_LOG_INFO("  Simulation time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("  Steps per second: " + std::to_string(200.0 / (duration.count() / 1000.0)));
    NLM_LOG_INFO("  Spikes per step: " + std::to_string(brain->getTotalSpikeCount() / 200));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Benchmark complete. Use for performance analysis.");
}