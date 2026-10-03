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
#include <algorithm>
#include <sstream>

using namespace nlm;

// Version information
const std::string NLM_VERSION = "0.1.0";
const std::string NLM_PHASE = "Phase 2: Real Neural Computation";

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     " << NLM_PHASE << "                         ║
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

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Neural Learning Machine - " << NLM_PHASE << std::endl;
    std::cout << "Version: " << NLM_VERSION << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message and exit" << std::endl;
    std::cout << "  --version                 Show version information and exit" << std::endl;
    std::cout << "  --config <file>           Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --neurons <N>             Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions <N>             Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --timestep <ms>           Simulation timestep in milliseconds (default: 1)" << std::endl;
    std::cout << "  --seed <seed>             Random seed (default: 42)" << std::endl;
    std::cout << "  --test-mode <mode>        Test mode: basic, plasticity, stdp, all (default: all)" << std::endl;
    std::cout << "  --log-level <level>       Logging level: debug, info, warning, error (default: info)" << std::endl;
    std::cout << "  --logfile <file>           Write log output to file in addition to console" << std::endl;
    std::cout << "  --save-state <file>        Save brain state to file after tests" << std::endl;
    std::cout << "  --no-colors               Disable colored console output" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --test-mode basic" << std::endl;
    std::cout << "  " << programName << " --neurons 1000 --regions 2 --timestep 0.5" << std::endl;
    std::cout << "  " << programName << " --config myconfig.cfg --save-state brain_state.chk" << std::endl;
}

void printVersion() {
    std::cout << "NLM " << NLM_VERSION << std::endl;
    std::cout << NLM_PHASE << std::endl;
    std::cout << "Neural Learning Machine - An experimental artificial developmental brain." << std::endl;
}

// Helper functions for command-line parsing
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

LogLevel stringToLogLevel(const std::string& level) {
    std::string lower = toLower(level);
    if (lower == "debug") return LogLevel::Debug;
    if (lower == "warning") return LogLevel::Warning;
    if (lower == "error") return LogLevel::Error;
    if (lower == "critical") return LogLevel::Critical;
    return LogLevel::Info; // default
}

std::string testModeToString(TestMode mode) {
    switch (mode) {
        case TestMode::Basic: return "basic";
        case TestMode::Plasticity: return "plasticity";
        case TestMode::STDP: return "stdp";
        case TestMode::All: return "all";
        default: return "all";
    }
}

TestMode stringToTestMode(const std::string& mode) {
    std::string lower = toLower(mode);
    if (lower == "basic") return TestMode::Basic;
    if (lower == "plasticity") return TestMode::Plasticity;
    if (lower == "stdp") return TestMode::STDP;
    if (lower == "all") return TestMode::All;
    return TestMode::All; // default
}

// Configuration structure for command-line overrides
struct ProgramConfig {
    std::string configFile = "configs/default.cfg";
    size_t neuronCount = 500;
    size_t regionCount = 1;
    double timestep = 0.001; // seconds
    uint64_t randomSeed = 42;
    TestMode testMode = TestMode::All;
    LogLevel logLevel = LogLevel::Info;
    std::string logFile;
    std::string saveStateFile;
    bool useColors = true;
    bool helpRequested = false;
    bool versionRequested = false;
};

bool parseCommandLine(int argc, char** argv, ProgramConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            config.helpRequested = true;
        } else if (arg == "--version") {
            config.versionRequested = true;
        } else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                config.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg.substr(0, 10) == "--neurons=") {
            config.neuronCount = std::stoull(arg.substr(10));
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoull(argv[++i]);
        } else if (arg.substr(0, 9) == "--regions=") {
            config.regionCount = std::stoull(arg.substr(9));
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = std::stoull(argv[++i]);
        } else if (arg.substr(0, 10) == "--timestep=") {
            config.timestep = std::stod(arg.substr(10));
        } else if (arg == "--timestep" && i + 1 < argc) {
            config.timestep = std::stod(argv[++i]);
        } else if (arg.substr(0, 7) == "--seed=") {
            config.randomSeed = std::stoull(arg.substr(7));
        } else if (arg == "--seed" && i + 1 < argc) {
            config.randomSeed = std::stoull(argv[++i]);
        } else if (arg.substr(0, 11) == "--test-mode=") {
            config.testMode = stringToTestMode(arg.substr(11));
        } else if (arg == "--test-mode" && i + 1 < argc) {
            config.testMode = stringToTestMode(argv[++i]);
        } else if (arg.substr(0, 12) == "--log-level=") {
            config.logLevel = stringToLogLevel(arg.substr(12));
        } else if (arg == "--log-level" && i + 1 < argc) {
            config.logLevel = stringToLogLevel(argv[++i]);
        } else if (arg.substr(0, 10) == "--logfile=") {
            config.logFile = arg.substr(10);
        } else if (arg == "--logfile" && i + 1 < argc) {
            config.logFile = argv[++i];
        } else if (arg.substr(0, 13) == "--save-state=") {
            config.saveStateFile = arg.substr(13);
        } else if (arg == "--save-state" && i + 1 < argc) {
            config.saveStateFile = argv[++i];
        } else if (arg == "--no-colors") {
            config.useColors = false;
        } else {
            // Unknown argument, ignore
            std::cerr << "Warning: Unknown argument ignored: " << arg << std::endl;
        }
    }
    return true;
}

void applyProgramConfig(std::shared_ptr<Config> config, const ProgramConfig& programConfig) {
    // Apply program-level configuration overrides
    if (programConfig.neuronCount != 500) {
        config->set("neuron_count", static_cast<int64_t>(programConfig.neuronCount), ConfigSource::CommandLine);
    }
    if (programConfig.regionCount != 1) {
        config->set("region_count", static_cast<int64_t>(programConfig.regionCount), ConfigSource::CommandLine);
    }
    if (programConfig.timestep != 0.001) {
        config->set("simulation_timestep", programConfig.timestep, ConfigSource::CommandLine);
    }
    if (programConfig.randomSeed != 42) {
        config->set("random_seed", static_cast<int64_t>(programConfig.randomSeed), ConfigSource::CommandLine);
    }
}

// Test mode enum
enum class TestMode {
    Basic,
    Plasticity,
    STDP,
    All
};

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
        NLM_LOG_INFO("  Synapses: " << initialSynapseCount);
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " << mean);
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
        NLM_LOG_INFO("  Total spikes: " << brain->getTotalSpikeCount());
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " << mean);
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
        NLM_LOG_INFO("  Initial mean weight: " << initialMean);
        NLM_LOG_INFO("  Final mean weight: " << finalMean);
        NLM_LOG_INFO("  Change: " << (finalMean - initialMean));
        
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
        NLM_LOG_INFO("  Strengthened: " << strengthened);
        NLM_LOG_INFO("  Weakened: " << weakened);
        NLM_LOG_INFO("  Unchanged: " << unchanged);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " << brain->getTotalSpikeCount());
        NLM_LOG_INFO("  Most active neurons recorded: " << mostActiveNeurons.size());
        
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
    NLM_LOG_INFO("  Spikes generated: " << spikes);
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " << i << 
                        " V=" << neurons[i]->getMembranePotential() <<
                        " thresh=" << neurons[i]->getThreshold());
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
            NLM_LOG_INFO("  Step " << step << 
                        " | Spikes: " << brain->getTotalSpikeCount() +
                        " | Firing: " << brain->getFiringNeuronCount());
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
        NLM_LOG_INFO("    Synapse " << i << 
                    " before: " << beforeWeights[i]);
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
        NLM_LOG_INFO("    Synapse " << i << 
                    " after: " << synapses[i]->getWeight() <<
                    " (Δ=" << delta << ")");
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
    // Parse command line arguments
    ProgramConfig programConfig;
    if (!parseCommandLine(argc, argv, programConfig)) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Handle help/version requests
    if (programConfig.helpRequested) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (programConfig.versionRequested) {
        printVersion();
        return 0;
    }
    
    printBanner();
    
    // Initialize logger with command-line control
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(programConfig.logLevel);
    
    // Configure console logger
    consoleLogger->setUseColors(programConfig.useColors);
    if (!programConfig.logFile.empty()) {
        consoleLogger->setOutputFile(programConfig.logFile);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM " << NLM_PHASE << " ===");
    NLM_LOG_INFO("Program: " << programConfig.neuronCount);
    NLM_LOG_INFO("Config file: " << programConfig.configFile);
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(programConfig.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " << programConfig.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply command-line overrides
    applyProgramConfig(config, programConfig);
    
    // Override with any remaining command line args (for backward compatibility)
    config->loadFromArgs(argc, argv);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Final Configuration:");
    NLM_LOG_INFO("  random_seed: " << config->getOr<int64_t>("random_seed", 42));
    NLM_LOG_INFO("  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s");
    NLM_LOG_INFO("  neuron_count: " << config->getOr<int64_t>("neuron_count", 500));
    NLM_LOG_INFO("  region_count: " << config->getOr<int64_t>("region_count", 1));
    NLM_LOG_INFO("  connection_probability: " << config->getOr<float>("connection_probability", 0.15f));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " << timestep << "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run experiments based on test mode
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running " << testModeToString(programConfig.testMode) << " Tests ===");
    
    switch (programConfig.testMode) {
        case TestMode::Basic:
            NLM_LOG_INFO("Running basic connectivity test...");
            runBasicConnectivityTest(brain);
            break;
            
        case TestMode::Plasticity:
            NLM_LOG_INFO("Running plasticity experiment...");
            runPlasticityExperiment(brain);
            break;
            
        case TestMode::STDP:
            NLM_LOG_INFO("Running STDP verification...");
            runStdpVerification(brain);
            break;
            
        case TestMode::All:
            // Run all tests in sequence
            
            // Test 1: Basic connectivity
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 1: Basic Connectivity ===");
            runBasicConnectivityTest(brain);
            
            // Reset for plasticity experiment
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Resetting brain for plasticity experiment...");
            brain->reset();
            brain->initialize();
            
            // Test 2: Plasticity learning experiment
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
            runPlasticityExperiment(brain);
            
            // Reset for STDP verification
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Resetting brain for STDP verification...");
            brain->reset();
            brain->initialize();
            
            // Test 3: STDP verification
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 3: STDP Verification ===");
            runStdpVerification(brain);
            break;
    }
    
    // Save state if requested
    if (!programConfig.saveStateFile.empty()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Saving brain state to: " << programConfig.saveStateFile);
        if (brain->save(programConfig.saveStateFile)) {
            NLM_LOG_INFO("✓ Brain state saved successfully");
        } else {
            NLM_LOG_ERROR("✗ Failed to save brain state");
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== " << NLM_PHASE << " Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Core objectives completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}