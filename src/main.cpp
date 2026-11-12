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

// Helper function declarations
void printBanner();
void printHelp();
void printVersion();
void runTestSuites(std::shared_ptr<Brain> brain, bool runConnectivityTest, bool runPlasticityTest, bool runStdpTest, bool verbose);
bool parseCommandLineArgs(int argc, char** argv, bool& verbose, bool& quiet, std::string& logLevel, std::string& configFile, 
                          bool& runConnectivity, bool& runPlasticity, bool& runStdp, bool& runNoTests, bool& showVersion, 
                          bool& showHelpAdvanced, bool& configOnly, bool& quickMode, int64_t& neuronCount, int64_t& regionCount,
                          size_t& steps, double& timestep, int64_t& seed, std::string& checkpointFile);

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

void printHelp() {
    printBanner();
    std::cout << R"(
    USAGE: nlm [OPTIONS]
    
    NLM (Neural Learning Machine) - Phase 2: Real Neural Computation
    
    This program demonstrates real spiking neural computation with:
      - Leaky Integrate-and-Fire (LIF) neuron dynamics
      - Event-driven spike propagation with synaptic delays
      - STDP and Hebbian plasticity rules
      - Structural plasticity (synaptogenesis/pruning)
    
    OPTIONS:
      -h, --help                    Show this help message
      -v, --verbose                 Enable verbose logging output
      -q, --quiet                   Suppress informational messages (errors only)
      -l, --log-level LEVEL          Set logging level (trace, debug, info, warn, error, fatal)
      -c, --config FILE             Load configuration from file
      --test-connectivity          Run basic connectivity test (default)
      --test-plasticity            Run plasticity learning experiment (default)
      --test-stdp                   Run STDP verification test (default)
      --no-tests                    Run no tests (only initialization)
      --config-only                 Load and display configuration, exit
      --version                     Display version information
      --help-advanced              Show advanced options
    
    CONFIG FILE OPTIONS:
      --config=file.cfg             Load specific configuration file
      -f file.cfg                   Alternative config file syntax
    
    ADVANCED OPTIONS:
      --neuron-count N              Number of neurons (default: 500)
      --region-count N              Number of brain regions (default: 1)
      --steps N                     Number of simulation steps (default: 1000)
      --timestep T                  Simulation timestep in seconds (default: 0.001)
      --seed SEED                   Random seed for reproducibility
      --checkpoint FILE             Save/load checkpoint file
      --quick-mode                  Use minimal configuration for fast testing
    
    EXAMPLES:
      nlm                                    # Run all tests with default configuration
      nlm --verbose --config myconfig.cfg   # Run with verbose output
      nlm --no-tests --help-advanced        # Show configuration options
      nlm --test-stdp --test-plasticity     # Run specific tests only
    )" << std::endl;
}

void printHelpAdvanced() {
    printBanner();
    std::cout << R"(
    ADVANCED USAGE - NLM Configuration and Operation
    
    NLM supports multiple configuration methods and advanced operation modes:
    
    === CONFIGURATION FILE FORMATS ===
    
    1. JSON Configuration (recommended):
    {
      "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
      },
      "plasticity": {
        "stdp": {
          "enable": true,
          "learning_rate": 0.001,
          "tau_plus": 20.0,
          "tau_minus": 20.0
        },
        "hebbian": {
          "enable": true,
          "learning_rate": 0.0005
        }
      },
      "neuromodulation": {
        "dopamine": {"scale": 1.0},
        "curiosity": {"enable": true},
        "novelty": {"enable": true}
      }
    }
    
    2. Simple key=value format (backward compatible):
    # NLM Configuration File
    # brain section
    brain.neuron_count = 1000
    brain.region_count = 2
    brain.connection_probability = 0.1
    
    # plasticity section
    plasticity.stdp.enable = true
    plasticity.stdp.learning_rate = 0.001
    
    === ADVANCED COMMAND LINE OPTIONS ===
    
    --neuron-count N              Set number of neurons (default: 500)
    --region-count N              Set number of brain regions (default: 1)
    --steps N                     Run N simulation steps (default: 1000)
    --timestep T                  Set simulation timestep in seconds (default: 0.001)
    --seed SEED                   Set random seed for reproducible results
    --checkpoint FILE             Save brain state to FILE
    --load-checkpoint FILE        Load brain state from FILE
    --quick-mode                  Use minimal configuration for fast testing
    --config-only                 Only load and display configuration
    --validate-config             Validate configuration before running
    
    === OPERATION MODES ===
    
    Full simulation (default):
      nlm --test-connectivity --test-plasticity --test-stdp
      nlm --verbose --config myconfig.json
    
    Quick test mode:
      nlm --quick-mode --steps 100 --neuron-count 100
    
    Configuration validation:
      nlm --validate-config --config myconfig.json
    
    Checkpoint operations:
      nlm --checkpoint brain.bin          # Save state at end
      nlm --load-checkpoint brain.bin     # Load state and continue
      nlm --checkpoint brain.bin --load-checkpoint brain.bin  # Round-trip
    
    === TESTING AND DEBUGGING ===
    
    Run specific test suites:
      nlm --test-connectivity     # Only run connectivity test
      nlm --test-plasticity       # Only run plasticity experiment
      nlm --test-stdp             # Only run STDP verification
      nlm --no-tests              # Run no tests (initialization only)
    
    Logging levels:
      nlm --log-level trace       # All logging (debug)
      nlm --log-level debug       # Debug and above
      nlm --log-level info        # Normal operation (default)
      nlm --log-level warn        # Warnings and errors
      nlm --log-level error       # Errors only
      nlm --log-level fatal       # Fatal errors only
    
    Quiet mode:
      nlm --quiet                # Suppress informational messages
    
    === EXAMPLES ===
    
    Example 1: Run a complete experiment with custom configuration
      nlm --verbose --config experiment.json --steps 5000
    
    Example 2: Quick performance test
      nlm --quick-mode --steps 200 --neuron-count 200 --region-count 1
    
    Example 3: Validate configuration before running
      nlm --validate-config --config mybrain.json
    
    Example 4: Run with checkpoint support
      nlm --checkpoint mybrain.bin --load-checkpoint previous.bin --steps 1000
    
    Example 5: Debug mode with full logging
      nlm --verbose --log-level debug --test-plasticity --test-stdp
    
    === CONFIGURATION VALIDATION ===
    
    The configuration validation checks include:
      
    - Parameter ranges (neuron_count > 0, timestep > 0, etc.)
    - Consistency checks (region_count ≤ neuron_count, etc.)
    - Required fields (brain.neuron_count, brain.connection_probability)
    - Data type validation (numbers must be numeric, booleans must be true/false)
    
    Configuration files can be created using the save functionality:
      nlm --save-config myconfig.json
    
    Configuration validation errors will be reported before brain initialization.
    " << std::endl;
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

// Improved test suite runner with better control
void runTestSuites(std::shared_ptr<Brain> brain, bool runConnectivityTest, bool runPlasticityTest, bool runStdpTest, bool verbose) {
    if (runConnectivityTest) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        runBasicConnectivityTest(brain);
    }
    
    if (runPlasticityTest) {
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        runPlasticityExperiment(brain);
    }
    
    if (runStdpTest) {
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        runStdpVerification(brain);
    }
}

bool parseCommandLineArgs(int argc, char** argv, bool& verbose, bool& quiet, std::string& logLevel, std::string& configFile, 
                          bool& runConnectivity, bool& runPlasticity, bool& runStdp, bool& runNoTests, bool& showVersion, 
                          bool& showHelpAdvanced, bool& configOnly, bool& quickMode, int64_t& neuronCount, int64_t& regionCount,
                          size_t& steps, double& timestep, int64_t& seed, std::string& checkpointFile) {
    // Set defaults
    verbose = false;
    quiet = false;
    logLevel = "info";
    configFile = "configs/default.cfg";
    runConnectivity = true;
    runPlasticity = true;
    runStdp = true;
    runNoTests = false;
    showVersion = false;
    showHelpAdvanced = false;
    configOnly = false;
    quickMode = false;
    neuronCount = 500;
    regionCount = 1;
    steps = 1000;
    timestep = 0.001;
    seed = 42;
    checkpointFile = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help
        if (arg == "-h" || arg == "--help") {
            return false;  // Help will be printed and program exits
        }
        
        // Handle version
        if (arg == "--version") {
            showVersion = true;
            continue;
        }
        
        // Handle verbose/quiet
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
            continue;
        }
        if (arg == "-q" || arg == "--quiet") {
            quiet = true;
            continue;
        }
        
        // Handle log level
        if (arg == "-l" || arg == "--log-level") {
            if (i + 1 < argc) {
                logLevel = argv[++i];
            }
            continue;
        }
        
        // Handle config file
        if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
            continue;
        }
        
        // Handle short config
        if (arg == "-f") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
            continue;
        }
        
        // Handle test selection
        if (arg == "--test-connectivity") {
            runConnectivity = true;
            continue;
        }
        if (arg == "--test-plasticity") {
            runPlasticity = true;
            continue;
        }
        if (arg == "--test-stdp") {
            runStdp = true;
            continue;
        }
        if (arg == "--no-tests") {
            runNoTests = true;
            runConnectivity = false;
            runPlasticity = false;
            runStdp = false;
            continue;
        }
        if (arg == "--config-only") {
            configOnly = true;
            continue;
        }
        if (arg == "--help-advanced") {
            showHelpAdvanced = true;
            continue;
        }
        if (arg == "--quick-mode") {
            quickMode = true;
            continue;
        }
        
        // Handle advanced options with equals sign or space
        if (arg.find("--neuron-count") == 0) {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoll(argv[++i]);
            }
            continue;
        }
        
        if (arg.find("--region-count") == 0) {
            if (arg.find('=') != std::string::npos) {
                regionCount = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                regionCount = std::stoll(argv[++i]);
            }
            continue;
        }
        
        if (arg.find("--steps") == 0) {
            if (arg.find('=') != std::string::npos) {
                steps = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
            } else if (i + 1 < argc) {
                steps = static_cast<size_t>(std::stoull(argv[++i]));
            }
            continue;
        }
        
        if (arg.find("--timestep") == 0) {
            if (arg.find('=') != std::string::npos) {
                timestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            }
            continue;
        }
        
        if (arg.find("--seed") == 0) {
            if (arg.find('=') != std::string::npos) {
                seed = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                seed = std::stoll(argv[++i]);
            }
            continue;
        }
        
        if (arg.find("--checkpoint") == 0) {
            if (arg.find('=') != std::string::npos) {
                checkpointFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                checkpointFile = argv[++i];
            }
            continue;
        }
    }
    
    return true;
}

// Main function with improved CLI support
int main(int argc, char** argv) {
    // Default values for test selection
    bool verbose = false;
    bool quiet = false;
    std::string logLevel = "info";
    std::string configFile = "configs/default.cfg";
    bool runConnectivity = true;      // Default: run connectivity test
    bool runPlasticity = true;        // Default: run plasticity test
    bool runStdp = true;              // Default: run STDP test
    bool runNoTests = false;
    bool showVersion = false;
    bool showHelpAdvanced = false;
    bool configOnly = false;
    bool quickMode = false;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    size_t steps = 1000;
    double timestep = 0.001;
    int64_t seed = 42;
    std::string checkpointFile = "";
    
    // Parse command line arguments
    if (!parseCommandLineArgs(argc, argv, verbose, quiet, logLevel, configFile,
                              runConnectivity, runPlasticity, runStdp, runNoTests, showVersion,
                              showHelpAdvanced, configOnly, quickMode, neuronCount, regionCount,
                              steps, timestep, seed, checkpointFile)) {
        return 1;
    }
    
    // Handle help/version commands first
    if (showVersion) {
        printVersion();
        return 0;
    }
    
    if (showHelpAdvanced) {
        printHelpAdvanced();
        return 0;
    }
    
    // Initialize logger with appropriate log level
    auto logger = std::make_shared<Logger>();
    LogLevel loggerLevel;
    
    if (logLevel == "trace") loggerLevel = LogLevel::Trace;
    else if (logLevel == "debug") loggerLevel = LogLevel::Debug;
    else if (logLevel == "info") loggerLevel = LogLevel::Info;
    else if (logLevel == "warn") loggerLevel = LogLevel::Warn;
    else if (logLevel == "error") loggerLevel = LogLevel::Error;
    else if (logLevel == "fatal") loggerLevel = LogLevel::Fatal;
    else loggerLevel = LogLevel::Info; // Default
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(loggerLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Suppress informational messages if quiet mode
    if (quiet) {
        consoleLogger->setLevel(LogLevel::Error);
    }
    
    // Show help if requested
    if (argc > 1 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        printHelp();
        return 0;
    }
    
    printBanner();
    
    if (!quiet) {
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    }
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (!config->loadFromFile(configFile)) {
        NLM_LOG_WARN("Config file not found: " + configFile);
        NLM_LOG_INFO("Using default configuration.");
    } else {
        if (!quiet) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        }
    }
    
    // Override with command line args (if verbose mode)
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 (override with command line if provided)
    config->set("random_seed", seed, ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("neuron_count", neuronCount, ConfigSource::Default);
    config->set("region_count", regionCount, ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", quickMode ? 0.01f : 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", quickMode ? 0.005f : 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    if (!quiet) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", seed)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", timestep)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", neuronCount)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", regionCount)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Validate configuration if requested
    if (configOnly) {
        std::string validationError;
        if (!ConfigValidator::validate(*config, validationError)) {
            NLM_LOG_ERROR("Configuration validation failed: " + validationError);
            return 1;
        }
        NLM_LOG_INFO("Configuration validation passed.");
        NLM_LOG_INFO(config->summary());
    } else {
        // Validate configuration before running if validation is enabled
        std::string validationError;
        bool validationPassed = ConfigValidator::validate(*config, validationError);
        
        if (!validationPassed) {
            NLM_LOG_WARN("Configuration validation warnings: " + validationError);
            NLM_LOG_INFO("Continuing with warnings (use --validate-strict to fail on warnings)...");
        } else {
            NLM_LOG_INFO("Configuration validation passed.");
        }
    }
    
    // Parse command line arguments for advanced options
    int64_t stepsOverride = steps;
    double timestepOverride = timestep;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle advanced options with equals sign or space
        if (arg.find("--steps") == 0) {
            if (arg.find('=') != std::string::npos) {
                stepsOverride = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
            } else if (i + 1 < argc) {
                stepsOverride = static_cast<size_t>(std::stoull(argv[++i]));
            }
        }
        
        if (arg.find("--timestep") == 0) {
            if (arg.find('=') != std::string::npos) {
                timestepOverride = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                timestepOverride = std::stod(argv[++i]);
            }
        }
    }
    
    // Apply overrides
    if (stepsOverride != steps) {
        config->set("brain.neuron_count", static_cast<int64_t>(stepsOverride), ConfigSource::CommandLine);
    }
    if (timestepOverride != timestep) {
        config->set("simulation_timestep", timestepOverride, ConfigSource::CommandLine);
    }
    
    return 0;
}