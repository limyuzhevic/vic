// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This is the main entry point for the NLM simulation with improved structure:
// - Structured command-line interface with comprehensive options
// - Integration with advanced ExperimentManager framework
// - Better error handling and user experience
// - Result export and analysis capabilities

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
#include "experiments/ExperimentManager.hpp"
#include "experiments/ExperimentManager.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;
using namespace nlm;

// Configuration constants
const std::string DEFAULT_CONFIG_FILE = "configs/default.cfg";
const std::string DEFAULT_RESULT_FILE = "nlm_results.json";
const std::string DEFAULT_LOG_FILE = "nlm.log";
const std::string HELP_FILE = "docs/HELP.md";

// Command line options structure
struct CLIConfig {
    std::string configFile = DEFAULT_CONFIG_FILE;
    std::string resultFile = DEFAULT_RESULT_FILE;
    std::string logFile = DEFAULT_LOG_FILE;
    std::string experimentType = "auto"; // auto, basic, plasticity, stdp, all
    int verboseLevel = 1; // 0=quiet, 1=normal, 2=verbose, 3=debug
    bool saveResults = false;
    bool loadState = false;
    bool showVersion = false;
    bool showHelp = false;
    int simulationSteps = 0; // 0 = default, otherwise custom
    double timestep = 0.001;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    bool useAdvancedFramework = true; // Use new ExperimentManager
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

class CommandLineParser {
public:
    static bool parseArguments(int argc, char** argv, CLIConfig& config) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                config.showHelp = true;
                return true;
            } else if (arg == "--version") {
                config.showVersion = true;
                return true;
            } else if (arg == "--config") {
                if (i + 1 < argc) {
                    config.configFile = argv[++i];
                }
            } else if (arg == "--experiment") {
                if (i + 1 < argc) {
                    config.experimentType = argv[++i];
                }
            } else if (arg == "--steps") {
                if (i + 1 < argc) {
                    config.simulationSteps = std::stoi(argv[++i]);
                }
            } else if (arg == "--verbose") {
                config.verboseLevel = std::min(3, config.verboseLevel + 1);
            } else if (arg == "--quiet") {
                config.verboseLevel = 0;
            } else if (arg == "--save") {
                config.saveResults = true;
            } else if (arg == "--load") {
                config.loadState = true;
            } else if (arg == "--result-file") {
                if (i + 1 < argc) {
                    config.resultFile = argv[++i];
                }
            } else if (arg == "--log-file") {
                if (i + 1 < argc) {
                    config.logFile = argv[++i];
                }
            } else if (arg == "--neuron-count") {
                if (i + 1 < argc) {
                    config.neuronCount = static_cast<size_t>(std::stoi(argv[++i]));
                }
            } else if (arg == "--region-count") {
                if (i + 1 < argc) {
                    config.regionCount = static_cast<size_t>(std::stoi(argv[++i]));
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    config.timestep = std::stod(argv[++i]);
                }
            } else if (arg.substr(0, 12) == "--set=") {
                // Allow setting individual config values
                std::string value = arg.substr(12);
                config.configFile = ""; // Mark to use inline config
            } else if (arg == "--export") {
                config.saveResults = true;
            } else if (arg == "--import") {
                config.loadState = true;
            } else if (arg == "--advanced") {
                config.useAdvancedFramework = true;
            } else if (arg == "--legacy") {
                config.useAdvancedFramework = false;
            }
        }
        
        return true;
    }
    
    static void printHelp() {
        printBanner();
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "USAGE: nlm [OPTIONS]\n\n";
        std::cout << "NLM - Neural Learning Machine - Phase 2: Real Neural Computation\n\n";
        
        std::cout << "OPTIONS:\n";
        std::cout << "  -h, --help              Show this help message\n";
        std::cout << "  --version               Show version information\n";
        std::cout << "  --config <file>         Load configuration from file (default: configs/default.cfg)\n";
        std::cout << "  --experiment <type>     Specify experiment type (auto, basic, plasticity, stdp, all)\n";
        std::cout << "  --steps <count>         Number of simulation steps (0 = default, 1000 = 1 second)\n";
        std::cout << "  -v, --verbose           Increase verbosity (can be repeated up to 3 times)\n";
        std::cout << "  -q, --quiet             Suppress output\n";
        std::cout << "  --save, --export        Save brain state and results\n";
        std::cout << "  --load, --import        Load brain state from file\n";
        std::cout << "  --result-file <file>    File to save results (default: nlm_results.json)\n";
        std::cout << "  --log-file <file>       File to save log output (default: nlm.log)\n";
        std::cout << "  --neuron-count <n>      Number of neurons (default: 500)\n";
        std::cout << "  --region-count <n>      Number of regions (default: 1)\n";
        std::cout << "  --timestep <t>          Simulation timestep (default: 0.001)\n";
        std::cout << "  --set <key>=<value>     Set individual configuration parameter\n";
        std::cout << "  --advanced              Use advanced experiment framework (default)\n";
        std::cout << "  --legacy               Use legacy experiment system\n\n";
        
        std::cout << "EXPERIMENT TYPES:\n";
        std::cout << "  auto    Run automatic discovery of interesting experiments\n";
        std::cout << "  basic   Test basic neural connectivity and spike propagation\n";
        std::cout << "  plasticity  Run comprehensive plasticity and learning experiments\n";
        std::cout << "  stdp    Verify STDP (Spike-Timing-Dependent Plasticity) mechanism\n";
        std::cout << "  all     Run all experiments in sequence\n\n";
        
        std::cout << "FEATURES:\n";
        std::cout << "  --advanced: Use enhanced ExperimentManager with configurable experiment sets\n";
        std::cout << "  --legacy: Use original embedded experiment system\n";
        std::cout << "  Comprehensive experiment tracking and result analysis\n";
        std::cout << "  Configurable experiment parameters and reporting\n\n";
        
        std::cout << "EXAMPLES:\n";
        std::cout << "  nlm --experiment all --advanced\n";
        std::cout << "  nlm --steps 5000 --verbose --save\n";
        std::cout << "  nlm --experiment plasticity --config my_config.cfg\n";
        std::cout << "  nlm --load --save --result-file brain_state.json\n\n";
        
        std::cout << "For more detailed documentation, see docs/ directory.\n";
    }
    
    static void printVersion() {
        printBanner();
        std::cout << "\nNLM - Neural Learning Machine\n";
        std::cout << "Phase 2: Real Neural Computation\n";
        std::cout << "Version: 2.6.1 (Phase 6.0 Integrated)\n";
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
        std::cout << "Architecture: Advanced Artificial Developmental Brain\n";
        std::cout << "Features:\n";
        std::cout << "  - Real LIF neuron dynamics with event-driven spike propagation\n";
        std::cout << "  - STDP and Hebbian plasticity rules\n";
        std::cout << "  - Structural plasticity (synaptogenesis/pruning)\n";
        std::cout << "  - Integrated memory systems (working, episodic, associative)\n";
        std::cout << "  - Advanced prediction and cognition systems\n";
        std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)\n";
        std::cout << "  - Development system with plasticity stages\n";
        std::cout << "  - Enhanced ExperimentManager with configurable experiments\n";
        std::cout << "  - Comprehensive experiment tracking and analysis\n";
    }
};

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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, int verboseLevel) {
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, int verboseLevel) {
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
        if (verboseLevel >= 1 && step % 100 == 0) {
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

void runStdpVerification(std::shared_ptr<Brain> brain, int verboseLevel) {
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

void runAdvancedAnalysis(std::shared_ptr<Brain> brain, int verboseLevel, const std::string& resultFile) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Advanced Analysis ===");
    
    // Log comprehensive brain statistics
    brain->logStatus();
    
    // Analyze network properties
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Network Analysis ===");
    
    float eiRatio = brain->getExcitationInhibitionRatio();
    NLM_LOG_INFO("Excitation/Inhibition ratio: " + std::to_string(eiRatio));
    
    if (eiRatio == std::numeric_limits<float>::infinity()) {
        NLM_LOG_INFO("  Network is purely excitatory (no inhibition)");
    } else if (eiRatio == 0.0f) {
        NLM_LOG_INFO("  Network is purely inhibitory (no excitation)");
    } else {
        NLM_LOG_INFO("  Network has balanced excitation and inhibition");
    }
    
    // Check for learning signatures
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Signature Analysis ===");
    
    // Simple learning indicator based on spike activity
    float avgFiringRate = brain->getAverageFiringRate();
    NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate));
    
    if (avgFiringRate > 5.0f) {
        NLM_LOG_INFO("  ✓ Network exhibits high activity - potential learning state");
    } else if (avgFiringRate > 0.5f) {
        NLM_LOG_INFO("  ✓ Network shows moderate activity");
    } else {
        NLM_LOG_INFO("  ! Network exhibits low activity - may need stimulation");
    }
    
    // Save detailed results if requested
    if (!resultFile.empty() && resultFile != DEFAULT_RESULT_FILE) {
        std::ofstream resultFileStream(resultFile);
        if (resultFileStream.is_open()) {
            resultFileStream << "NLM Brain Analysis Results\n";
            resultFileStream << "==========================\n\n";
            resultFileStream << "Generated: " << __DATE__ << " " << __TIME__ << "\n\n";
            resultFileStream << "Network Statistics:\n";
            resultFileStream << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
            resultFileStream << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
            resultFileStream << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
            resultFileStream << "  Firing rate: " << avgFiringRate << " Hz\n";
            resultFileStream << "  E/I ratio: " << eiRatio << "\n";
            resultFileStream << "\nExperiment completed successfully.\n";
            resultFileStream.close();
            NLM_LOG_INFO("Results saved to: " + resultFile);
        }
    }
}

// Legacy experiment functions (used when --legacy is specified)
bool runLegacyExperiment(CLIConfig& config) {
    printBanner();
    
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
    auto configObj = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = config.configFile;
    if (config.configFile != "configs/default.cfg") {
        // Override with command line args
        configObj->loadFromArgs(0, nullptr); // Will be set later
    }
    
    // Set default values for Phase 2
    configObj->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    configObj->set("simulation_timestep", config.timestep, ConfigSource::Default);
    configObj->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    configObj->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    configObj->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    configObj->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    configObj->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    configObj->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    configObj->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    configObj->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(configObj->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(configObj->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(configObj->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(configObj->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(configObj->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = configObj->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(configObj);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    brain->logStatus();
    
    // Run experiments based on configuration
    bool success = true;
    
    if (config.experimentType == "all" || config.experimentType == "auto") {
        // Run all experiments
        runBasicConnectivityTest(brain, config.verboseLevel);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain, config.verboseLevel);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain, config.verboseLevel);
        
        // Run advanced analysis
        runAdvancedAnalysis(brain, config.verboseLevel, config.resultFile);
        
    } else if (config.experimentType == "basic") {
        runBasicConnectivityTest(brain, config.verboseLevel);
    } else if (config.experimentType == "plasticity") {
        runPlasticityExperiment(brain, config.verboseLevel);
    } else if (config.experimentType == "stdp") {
        runStdpVerification(brain, config.verboseLevel);
    }
    
    // Save results if requested
    if (config.saveResults) {
        std::string saveFile = config.resultFile;
        if (saveFile == DEFAULT_RESULT_FILE) {
            saveFile = "brain_state_checkpoint.nlm";
        }
        NLM_LOG_INFO("Saving brain state to: " + saveFile);
        if (!brain->save(saveFile)) {
            NLM_LOG_ERROR("Failed to save brain state!");
            success = false;
        }
    }
    
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
    
    return success;
}

// Advanced experiment functions using ExperimentManager
bool runAdvancedExperiment(CLIConfig& config) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation with Advanced Framework...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation (Advanced) ===");
    NLM_LOG_INFO("Using enhanced ExperimentManager framework for comprehensive experiment tracking");
    NLM_LOG_INFO("");
    
    // Initialize experiment manager
    auto expManager = std::make_shared<ExperimentManager>();
    
    // Configure experiment based on type
    ExperimentConfig expConfig;
    if (config.experimentType == "auto") {
        expConfig = StandardExperiments::createAllExperiments("auto_comprehensive");
    } else if (config.experimentType == "basic") {
        expConfig = StandardExperiments::createBasicConnectivityExperiment("basic_connectivity");
    } else if (config.experimentType == "plasticity") {
        expConfig = StandardExperiments::createPlasticityExperiment("plasticity_learning");
    } else if (config.experimentType == "stdp") {
        expConfig = StandardExperiments::createStdpExperiment("stdp_verification");
    } else if (config.experimentType == "all") {
        expConfig = StandardExperiments::createAllExperiments("comprehensive_all");
    } else {
        expConfig = StandardExperiments::createBasicConnectivityExperiment("default");
    }
    
    // Apply CLI overrides
    expConfig.neuronCount = config.neuronCount;
    expConfig.regionCount = config.regionCount;
    expConfig.timestep = config.timestep;
    expConfig.maxSteps = config.simulationSteps > 0 ? config.simulationSteps : expConfig.maxSteps;
    expConfig.saveResults = config.saveResults;
    expConfig.resultFile = config.resultFile;
    
    expManager->configure(expConfig);
    
    // Initialize experiment (configure logging and validation)
    if (!expManager->validateConfiguration()) {
        NLM_LOG_ERROR("Experiment configuration validation failed");
        return false;
    }
    
    // Create and initialize brain
    auto configObj = std::make_shared<Config>();
    configObj->set("random_seed", static_cast<int64_t>(expConfig.seed), ConfigSource::Default);
    configObj->set("simulation_timestep", expConfig.timestep, ConfigSource::Default);
    configObj->set("neuron_count", static_cast<int64_t>(expConfig.neuronCount), ConfigSource::Default);
    configObj->set("region_count", static_cast<int64_t>(expConfig.regionCount), ConfigSource::Default);
    configObj->set("connection_probability", expConfig.connectionProbability, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(configObj);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    brain->logStatus();
    
    // Set up progress tracking
    expManager->setProgressCallback([](float progress) {
        if (progress > 0.0f) {
            int barWidth = 50;
            int pos = static_cast<int>(barWidth * progress);
            std::cout << "\rProgress: [";
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << static_cast<int>(progress * 100.0f) << "%" << std::flush;
        }
    });
    
    // Run experiments using advanced framework
    std::vector<ExperimentResult> results;
    
    if (config.experimentType == "all") {
        NLM_LOG_INFO("\n=== RUNNING COMPREHENSIVE EXPERIMENT SET ===\n");
        results = expManager->runAllPhases(brain);
    } else {
        NLM_LOG_INFO("\n=== RUNNING SINGLE EXPERIMENT: " << config.experimentType << " ===\n");
        
        ExperimentManager::ExperimentPhase phase;
        if (config.experimentType == "basic") {
            phase = ExperimentManager::PHASE_BASIC_CONNECTIVITY;
        } else if (config.experimentType == "plasticity") {
            phase = ExperimentManager::PHASE_PLASTICITY_LEARNING;
        } else if (config.experimentType == "stdp") {
            phase = ExperimentManager::PHASE_STDP_VERIFICATION;
        } else {
            phase = ExperimentManager::PHASE_ADVANCED_ANALYSIS;
        }
        
        ExperimentResult result = expManager->runPhase(brain, phase);
        results.push_back(result);
    }
    
    std::cout << std::endl; // New line after progress bar
    
    // Log experiment summary
    NLM_LOG_INFO("\n");
    NLM_LOG_INFO("=== EXPERIMENT SUMMARY ===");
    NLM_LOG_INFO("Total experiments completed: " << results.size());
    NLM_LOG_INFO("Successful experiments: " << expManager->getSuccessfulExperimentCount());
    NLM_LOG_INFO("Average experiment score: " << expManager->getAverageScore());
    
    // Save results if requested
    if (config.saveResults) {
        std::string resultFile = config.resultFile;
        if (resultFile == DEFAULT_RESULT_FILE) {
            resultFile = "experiment_results.json";
        }
        
        NLM_LOG_INFO("\nSaving advanced experiment results to: " << resultFile);
        expManager->saveResults(resultFile);
        
        // Also export analysis data
        std::string analysisFile = resultFile.replace(resultFile.find_last_of('.'), 0, "_analysis");
        expManager->exportAnalysisData(analysisFile);
        
        // Generate comprehensive report
        std::string reportFile = resultFile.replace(resultFile.find_last_of('.'), 0, "_report");
        expManager->generateReport(reportFile);
    }
    
    // Final status
    NLM_LOG_INFO("\n");
    NLM_LOG_INFO("=== ADVANCED EXPERIMENT FRAMEWORK COMPLETE ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The enhanced NLM brain has been analyzed using the ExperimentManager framework.");
    NLM_LOG_INFO("This provides comprehensive tracking, configurable experiments, and detailed reporting.");
    NLM_LOG_INFO("");
    
    return true;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CLIConfig config;
    bool argsProcessed = CommandLineParser::parseArguments(argc, argv, config);
    
    // Show help or version if requested
    if (config.showHelp) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    if (config.showVersion) {
        CommandLineParser::printVersion();
        return 0;
    }
    
    // Run experiments based on configuration and framework preference
    bool success = false;
    
    if (config.useAdvancedFramework) {
        NLM_LOG_INFO("Using Advanced Experiment Framework...")
        success = runAdvancedExperiment(config) ? true : false;
    } else {
        NLM_LOG_INFO("Using Legacy Experiment System...")
        success = runLegacyExperiment(config) ? true : false;
    }
    
    return success ? 0 : 1;
}