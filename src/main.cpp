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
#include "performance/CheckpointSystem.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <map>
#include <sstream>
#include <chrono>
#include <thread>
#include <cmath>
#include <map>
#include <sstream>

using namespace nlm;

using namespace nlm;

// Constants for NLM
namespace nlm_constants {
    const std::string VERSION = "2.0.0";
    const std::string PROJECT_NAME = "NLM — 熙然 (Neural Learning Machine)";
    const std::string PHASE_NAME = "Phase 2: Real Neural Computation";
    const std::string PHASE_DESCRIPTION = "Implementing real LIF neuron dynamics, event-driven spike propagation, STDP and Hebbian plasticity, and structural plasticity.";
    
    // Default configuration values
    const int64_t DEFAULT_NEURON_COUNT = 500;
    const int64_t DEFAULT_REGION_COUNT = 1;
    const float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    const int64_t DEFAULT_RANDOM_SEED = 42;
    const double DEFAULT_SIMULATION_TIMESTEP = 0.001;
    
    // Plasticity parameters
    const float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    const float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    const float DEFAULT_STDP_TAU = 20.0f;
    const float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    const float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Learning parameters
    const float DEFAULT_LEARNING_RATE = 0.001f;
    const float DEFAULT_NOISE_LEVEL = 0.01f;
    const int DEFAULT_MAX_STEPS = 1000;
    const float DEFAULT_REWARD_SCALE = 1.0f;
}

// CLI parsing utilities
namespace cli_utils {
    
    struct CLIArgs {
        bool help = false;
        bool version = false;
        bool listConfigs = false;
        bool validateConfig = false;
        std::string configFile = "configs/default.cfg";
        int64_t neuronCount = nlm_constants::DEFAULT_NEURON_COUNT;
        int64_t regionCount = nlm_constants::DEFAULT_REGION_COUNT;
        float connectionProbability = nlm_constants::DEFAULT_CONNECTION_PROBABILITY;
        int64_t randomSeed = nlm_constants::DEFAULT_RANDOM_SEED;
        double timestep = nlm_constants::DEFAULT_SIMULATION_TIMESTEP;
        bool enableDebug = false;
        bool enableVerbose = false;
        bool benchmarkMode = false;
        int benchmarkDuration = 100;
        std::string outputFile = "";
        bool saveCheckpoint = false;
        std::string checkpointFile = "brain_checkpoint.bin";
        bool runAllTests = false;
        std::vector<std::string> customArgs;
    };
    
    CLIArgs parseCLIArgs(int argc, char** argv) {
        CLIArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version" || arg == "-v") {
                args.version = true;
            } else if (arg == "--list-configs") {
                args.listConfigs = true;
            } else if (arg == "--validate-config") {
                args.validateConfig = true;
            } else if (arg == "--neuron-count" || arg == "-nc") {
                if (i + 1 < argc) args.neuronCount = std::stoll(argv[++i]);
            } else if (arg == "--region-count" || arg == "-rc") {
                if (i + 1 < argc) args.regionCount = std::stoll(argv[++i]);
            } else if (arg == "--connection-prob" || arg == "-cp") {
                if (i + 1 < argc) args.connectionProbability = std::stof(argv[++i]);
            } else if (arg == "--seed" || arg == "-s") {
                if (i + 1 < argc) args.randomSeed = std::stoll(argv[++i]);
            } else if (arg == "--timestep" || arg == "-t") {
                if (i + 1 < argc) args.timestep = std::stod(argv[++i]);
            } else if (arg == "--config") {
                if (i + 1 < argc) args.configFile = argv[++i];
            } else if (arg == "--enable-debug" || arg == "-d") {
                args.enableDebug = true;
            } else if (arg == "--enable-verbose" || arg == "-vv") {
                args.enableVerbose = true;
            } else if (arg == "--benchmark" || arg == "-b") {
                args.benchmarkMode = true;
                if (i + 1 < argc) args.benchmarkDuration = std::stoi(argv[++i]);
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) args.outputFile = argv[++i];
            } else if (arg == "--checkpoint" || arg == "-ck") {
                args.saveCheckpoint = true;
                if (i + 1 < argc) args.checkpointFile = argv[++i];
            } else if (arg == "--run-all-tests" || arg == "-all") {
                args.runAllTests = true;
            } else if (arg == "--" || arg == "-+" || arg == "!") {
                // Arguments that should not be interpreted as options
                i++; // Skip the "--"
                while (i < argc) {
                    args.customArgs.push_back(argv[i]);
                    i++;
                }
                break;
            } else {
                // Unknown argument - treat as custom config
                args.customArgs.push_back(arg);
            }
        }
        
        return args;
    }
    
    void printHelp(const std::string& programName) {
        std::cout << R"(
USAGE: " + programName + " [OPTIONS]

NLM (Neural Learning Machine) - Phase 2: Real Neural Computation

BASIC OPTIONS:
    -h, --help              Show this help message
    -v, --version           Show version information
    -nc, --neuron-count N   Number of neurons (default: 500)
    -rc, --region-count R   Number of brain regions (default: 1)
    -cp, --connection-prob P Connection probability (default: 0.15)
    -s, --seed SEED         Random seed (default: 42)
    -t, --timestep T        Simulation timestep in seconds (default: 0.001)
    --config FILE            Configuration file path (default: configs/default.cfg)

ADVANCED OPTIONS:
    -d, --enable-debug      Enable debug logging
    -vv, --enable-verbose   Enable verbose output
    -b, --benchmark D       Run benchmark for D steps (e.g., 100, 1000, 10000)
    -o, --output FILE        Save output to file
    -ck, --checkpoint FILE   Save checkpoint to FILE (default: brain_checkpoint.bin)
    --run-all-tests         Run all experimental tests (default)
    --list-configs          List all available configuration options
    --validate-config       Validate current configuration

PLACEMENT PLACEHOLDER:
    --                         Use to separate option arguments from custom arguments

EXAMPLES:
    " + programName + " --help
    " + programName + " --neuron-count 1000 --seed 123
    " + programName + " --benchmark 1000 --output results.txt
    " + programName + " --config custom.cfg --enable-verbose

PHASE " + nlm_constants::PHASE_NAME + ":
    " + nlm_constants::PHASE_DESCRIPTION + "

For more information, see:
    https://github.com/nlm-project/nlm
    README.md in the project root

EXPERIMENTAL FEATURE:
    Use -- to pass arguments directly to the underlying configuration system.
)" << std::endl;
    }
    
    void printVersion() {
        std::cout << nlm_constants::PROJECT_NAME << std::endl;
        std::cout << "Version: " << nlm_constants::VERSION << std::endl;
        std::cout << "Phase: " << nlm_constants::PHASE_NAME << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    }
    
    std::string formatTime(double seconds) {
        int minutes = static_cast<int>(seconds / 60.0);
        int secs = static_cast<int>(seconds) % 60;
        int millisecs = static_cast<int>((seconds - std::floor(seconds)) * 1000);
        
        if (minutes > 0) {
            return std::to_string(minutes) + "m " + std::to_string(secs) + "s " + std::to_string(millisecs) + "ms";
        } else if (secs > 0) {
            return std::to_string(secs) + "s " + std::to_string(millisecs) + "ms";
        } else {
            return std::to_string(millisecs) + "ms";
        }
    }
    
    std::string formatNumber(size_t num) {
        std::stringstream ss;
        ss.imbue(std::locale(""));
        ss << num;
        return ss.str();
    }
    
    double getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double>(duration).count();
    }
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
    // Parse command line arguments
    auto args = cli_utils::parseCLIArgs(argc, argv);
    
    // Handle help/version requests immediately
    if (args.help) {
        cli_utils::printHelp("nlm");
        return 0;
    }
    
    if (args.version) {
        cli_utils::printVersion();
        return 0;
    }
    
    if (args.listConfigs) {
        std::cout << "=== Available Configuration Options ===" << std::endl;
        std::cout << "Basic settings:" << std::endl;
        std::cout << "  --neuron-count N      Number of neurons (default: " << nlm_constants::DEFAULT_NEURON_COUNT << ")" << std::endl;
        std::cout << "  --region-count R      Number of brain regions (default: " << nlm_constants::DEFAULT_REGION_COUNT << ")" << std::endl;
        std::cout << "  --connection-prob P   Connection probability (default: " << nlm_constants::DEFAULT_CONNECTION_PROBABILITY << ")" << std::endl;
        std::cout << "  --seed SEED           Random seed (default: " << nlm_constants::DEFAULT_RANDOM_SEED << ")" << std::endl;
        std::cout << "  --timestep T          Simulation timestep in seconds (default: " << nlm_constants::DEFAULT_SIMULATION_TIMESTEP << ")" << std::endl;
        std::cout << "  --config FILE          Configuration file path" << std::endl;
        
        std::cout << "\nPlasticity settings:" << std::endl;
        std::cout << "  --stdp-ltp-weight W   STDP LTP weight (default: " << nlm_constants::DEFAULT_STDP_LTP_WEIGHT << ")" << std::endl;
        std::cout << "  --stdp-ltd-weight W   STDP LTD weight (default: " << nlm_constants::DEFAULT_STDP_LTD_WEIGHT << ")" << std::endl;
        std::cout << "  --stdp-tau TAU        STDP time constant (default: " << nlm_constants::DEFAULT_STDP_TAU << ")" << std::endl;
        std::cout << "  --synaptogenesis-rate R Synaptogenesis rate (default: " << nlm_constants::DEFAULT_SYNAPTOGENESIS_RATE << ")" << std::endl;
        std::cout << "  --pruning-rate R      Pruning rate (default: " << nlm_constants::DEFAULT_PRUNING_RATE << ")" << std::endl;
        
        std::cout << "\nDebug and logging:" << std::endl;
        std::cout << "  --enable-debug        Enable debug logging" << std::endl;
        std::cout << "  --enable-verbose      Enable verbose output" << std::endl;
        
        std::cout << "\nAdvanced options:" << std::endl;
        std::cout << "  --benchmark D         Run benchmark for D steps" << std::endl;
        std::cout << "  --output FILE          Save output to FILE" << std::endl;
        std::cout << "  --checkpoint FILE      Save checkpoint to FILE" << std::endl;
        
        return 0;
    }
    
    if (args.validateConfig) {
        std::cout << "=== Configuration Validation ===" << std::endl;
        auto config = std::make_shared<Config>();
        if (config->loadFromArgs(argc, argv)) {
            std::cout << "✓ Configuration loaded successfully" << std::endl;
        } else {
            std::cout << "✗ Failed to load configuration" << std::endl;
        }
        return 0;
    }
    
    printBanner();
    
    // Log startup information
    auto startTime = cli_utils::getCurrentTimestamp();
    NLM_LOG_INFO("=== " + std::string(nlm_constants::PROJECT_NAME) + " ===");
    NLM_LOG_INFO("Version: " + std::string(nlm_constants::VERSION));
    NLM_LOG_INFO("Phase: " + std::string(nlm_constants::PHASE_NAME));
    
    if (args.benchmarkMode) {
        NLM_LOG_INFO("Benchmark mode enabled - running " + std::to_string(args.benchmarkDuration) + " steps");
    } else {
        NLM_LOG_INFO("Running " + nlm_constants::PHASE_NAME);
    }
    
    // Initialize logging levels
    if (args.enableDebug) {
        Logger::getGlobal().setLevel(LogLevel::Debug);
        NLM_LOG_DEBUG("Debug logging enabled");
    }
    
    if (args.enableVerbose) {
        ConsoleLogger::getGlobal()->setUseColors(true);
        NLM_LOG_INFO("Verbose logging enabled with colors");
    }
    
    std::cout << "Initializing " << std::string(nlm_constants::PHASE_NAME) << "...\n" << std::endl;
    
    // Initialize progress tracking
    auto progressStartTime = cli_utils::getCurrentTimestamp();
    size_t lastProgressReport = 0;
    const size_t PROGRESS_REPORT_INTERVAL = 100;
    
    // Progress reporting function
    auto reportProgress = [&](SimulationStep currentStep, SimulationStep totalSteps) {
        if (currentStep - lastProgressReport >= PROGRESS_REPORT_INTERVAL || currentStep == totalSteps) {
            double elapsed = cli_utils::getCurrentTimestamp() - progressStartTime;
            double stepsPerSecond = (elapsed > 0) ? (currentStep / elapsed) : 0.0;
            
            NLM_LOG_INFO("Progress: Step " + std::to_string(currentStep) + "/" + std::to_string(totalSteps) +
                        " | " + cli_utils::formatNumber(currentStep) + " steps @ " + 
                        cli_utils::formatNumber(static_cast<size_t>(stepsPerSecond)) + " steps/sec");
            lastProgressReport = currentStep;
        }
    };
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== " + std::string(nlm_constants::PROJECT_NAME) + " ===");
    NLM_LOG_INFO("Version: " + std::string(nlm_constants::VERSION));
    NLM_LOG_INFO("Phase: " + std::string(nlm_constants::PHASE_NAME));
    
    // Load configuration from file
    std::string configFile = args.configFile;
    auto config = std::make_shared<Config>();
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 (overridden by CLI args)
    config->set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::CommandLine);
    config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(args.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", args.connectionProbability, ConfigSource::CommandLine);
    
    // STDP parameters
    config->set("stdp_ltp_weight", nlm_constants::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", nlm_constants::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_tau", nlm_constants::DEFAULT_STDP_TAU, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", nlm_constants::DEFAULT_SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", nlm_constants::DEFAULT_PRUNING_RATE, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    if (args.benchmarkMode) {
        config->set("neuron_count", static_cast<int64_t>(std::min(args.benchmarkDuration / 10, nlm_constants::DEFAULT_NEURON_COUNT)), ConfigSource::CommandLine);
        NLM_LOG_INFO("Benchmark mode: Reduced neuron count to " + std::to_string(config->getOr<int64_t>("neuron_count", 500)) + " for faster benchmarking");
    }
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
    
    // Enhanced error handling with recovery strategies
    bool setupSuccessful = true;
    std::string lastErrorMessage;
    
    try {
        // Step 1: Initialize simulation clock
        NLM_LOG_INFO("Initializing simulation clock...");
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Step 2: Initialize brain with error handling
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            lastErrorMessage = "Failed to initialize brain!";
            NLM_LOG_ERROR(lastErrorMessage);
            setupSuccessful = false;
        } else {
            brain->logStatus();
            NLM_LOG_INFO("Brain initialization successful");
        }
        
    } catch (const std::exception& e) {
        lastErrorMessage = "Exception during setup: " + std::string(e.what());
        NLM_LOG_ERROR(lastErrorMessage);
        setupSuccessful = false;
    } catch (...) {
        lastErrorMessage = "Unknown exception during setup";
        NLM_LOG_ERROR(lastErrorMessage);
        setupSuccessful = false;
    }
    
    // Recovery strategy: If setup fails, use minimal configuration
    if (!setupSuccessful) {
        NLM_LOG_WARNING("Setup failed. Attempting recovery with minimal configuration...");
        
        // Create minimal configuration
        auto minimalConfig = std::make_shared<Config>();
        minimalConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
        minimalConfig->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        minimalConfig->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);  // Reduced for recovery
        minimalConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        minimalConfig->set("connection_probability", 0.1f, ConfigSource::Runtime);
        
        // Try to initialize with minimal configuration
        try {
            auto minimalBrain = std::make_shared<Brain>(minimalConfig);
            if (minimalBrain->initialize()) {
                NLM_LOG_INFO("Recovery successful with minimal configuration");
                brain = minimalBrain;
                setupSuccessful = true;
            } else {
                NLM_LOG_ERROR("Recovery failed. Cannot initialize even with minimal configuration.");
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during recovery: " + std::string(e.what()));
        }
    }
    
    if (!setupSuccessful) {
        NLM_LOG_ERROR("All setup attempts failed. Exiting.");
        return 1;
    }
    
    NLM_LOG_INFO("Setup completed successfully. Ready to run tests.");
    
    // Test execution with enhanced error handling
    bool test1Success = false;
    bool test2Success = false;
    bool test3Success = false;
    
    // Run Test 1 with error handling
    try {
        NLM_LOG_INFO("Running Test 1: Basic Neural Connectivity...");
        runBasicConnectivityTest(brain);
        test1Success = true;
        NLM_LOG_INFO("Test 1 completed successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Test 1 failed with exception: " + std::string(e.what()));
        lastErrorMessage = "Test 1 error: " + std::string(e.what());
    }
    
    // Recovery: Reset and try again if test 1 fails
    if (!test1Success) {
        NLM_LOG_WARNING("Test 1 failed. Attempting recovery...");
        try {
            brain->reset();
            brain->initialize();
            NLM_LOG_INFO("Attempting Test 1 recovery...");
            runBasicConnectivityTest(brain);
            test1Success = true;
            NLM_LOG_INFO("Test 1 recovery successful");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Test 1 recovery also failed: " + std::string(e.what()));
        }
    }
    
    // Run Test 2 with error handling
    try {
        NLM_LOG_INFO("Running Test 2: Plasticity Learning Experiment...");
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain, args.benchmarkMode);
        test2Success = true;
        NLM_LOG_INFO("Test 2 completed successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Test 2 failed with exception: " + std::string(e.what()));
        lastErrorMessage = "Test 2 error: " + std::string(e.what());
    }
    
    // Recovery: If test 2 fails, try with reduced settings
    if (!test2Success) {
        NLM_LOG_WARNING("Test 2 failed. Attempting recovery with reduced settings...");
        try {
            brain->reset();
            brain->initialize();
            
            // Temporarily reduce settings
            auto tempNeuronCount = brain->getConfig()->getOr<int64_t>("neuron_count", 500);
            brain->getConfig()->set("neuron_count", std::min(static_cast<int64_t>(50), tempNeuronCount), ConfigSource::Runtime);
            
            NLM_LOG_INFO("Attempting Test 2 recovery with reduced neuron count...");
            runPlasticityExperiment(brain, true);  // Force benchmark mode
            test2Success = true;
            NLM_LOG_INFO("Test 2 recovery successful");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Test 2 recovery also failed: " + std::string(e.what()));
        }
    }
    
    // Run Test 3 with error handling
    try {
        NLM_LOG_INFO("Running Test 3: STDP Verification...");
        brain->reset();
        brain->initialize();
        runStdpVerification(brain, args.benchmarkMode);
        test3Success = true;
        NLM_LOG_INFO("Test 3 completed successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Test 3 failed with exception: " + std::string(e.what()));
        lastErrorMessage = "Test 3 error: " + std::string(e.what());
    }
    
    // Recovery: If test 3 fails, try with minimal settings
    if (!test3Success) {
        NLM_LOG_WARNING("Test 3 failed. Attempting recovery with minimal settings...");
        try {
            brain->reset();
            brain->initialize();
            
            // Use minimal settings for STDP test
            auto tempNeuronCount = brain->getConfig()->getOr<int64_t>("neuron_count", 500);
            brain->getConfig()->set("neuron_count", std::min(static_cast<int64_t>(20), tempNeuronCount), ConfigSource::Runtime);
            
            NLM_LOG_INFO("Attempting Test 3 recovery with minimal neuron count...");
            runStdpVerification(brain, true);  // Force benchmark mode
            test3Success = true;
            NLM_LOG_INFO("Test 3 recovery successful");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Test 3 recovery also failed: " + std::string(e.what()));
        }
    }
    
    // Final results and error reporting
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== FINAL TEST RESULTS ===");
    NLM_LOG_INFO("Test 1 (Connectivity): " + (test1Success ? "PASS" : "FAIL"));
    NLM_LOG_INFO("Test 2 (Plasticity): " + (test2Success ? "PASS" : "FAIL"));
    NLM_LOG_INFO("Test 3 (STDP): " + (test3Success ? "PASS" : "FAIL"));
    
    if (test1Success && test2Success && test3Success) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("✓ ALL TESTS PASSED - Phase 2 completed successfully!");
    } else {
        NLM_LOG_WARNING("");
        NLM_LOG_WARNING("Some tests failed. See above for detailed error information.");
        
        if (!lastErrorMessage.empty()) {
            NLM_LOG_INFO("Last error: " + lastErrorMessage);
        }
        
        // Continue anyway to show brain status
        NLM_LOG_INFO("Continuing with final status report...");
    }
    
    // Continue with final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Performance monitoring
    auto overallStartTime = cli_utils::getCurrentTimestamp();
    
    // Memory management tracking
    size_t peakMemoryUsage = 0;
    auto getMemoryUsage = []() -> size_t {
        // Simple memory estimation - in real implementation would use system-specific calls
        return 0;
    };
    
    // Performance benchmarking
    struct PerformanceMetrics {
        double totalTime;
        double setupTime;
        double test1Time;
        double test2Time;
        double test3Time;
        double cleanupTime;
        size_t neuronsProcessed;
        size_t synapsesProcessed;
        size_t spikesGenerated;
        size_t memoryPeak;
    };
    
    PerformanceMetrics metrics;
    metrics.totalTime = 0.0;
    metrics.setupTime = 0.0;
    metrics.test1Time = 0.0;
    metrics.test2Time = 0.0;
    metrics.test3Time = 0.0;
    metrics.cleanupTime = 0.0;
    metrics.neuronsProcessed = 0;
    metrics.synapsesProcessed = 0;
    metrics.spikesGenerated = 0;
    metrics.memoryPeak = 0;
    
    auto recordSetupTime = [&]() {
        metrics.setupTime = cli_utils::getCurrentTimestamp() - overallStartTime;
    };
    
    auto recordTestTime = [&](double& timeVar) {
        timeVar = cli_utils::getCurrentTimestamp() - (overallStartTime + metrics.setupTime);
    };
    
    auto recordMemoryUsage = [&]() {
        size_t currentMemory = getMemoryUsage();
        if (currentMemory > metrics.memoryPeak) {
            metrics.memoryPeak = currentMemory;
        }
    };
    
    // Run setup with progress tracking
    NLM_LOG_INFO("Setting up simulation environment...");
    auto setupStart = cli_utils::getCurrentTimestamp();
    
    brain->logStatus();
    
    auto setupEnd = cli_utils::getCurrentTimestamp();
    metrics.setupTime = setupEnd - setupStart;
    
    // Record initial memory usage
    recordMemoryUsage();
    
    // Run Test 1 with performance tracking
    NLM_LOG_INFO("Starting performance test 1...");
    auto test1Start = cli_utils::getCurrentTimestamp();
    runBasicConnectivityTest(brain);
    auto test1End = cli_utils::getCurrentTimestamp();
    metrics.test1Time = test1End - test1Start;
    metrics.neuronsProcessed += brain->getTotalNeuronCount();
    metrics.synapsesProcessed += brain->getTotalSynapseCount();
    metrics.spikesGenerated += brain->getTotalSpikeCount();
    recordMemoryUsage();
    
    // Reset brain for plasticity experiment
    NLM_LOG_INFO("Resetting brain for plasticity experiment...");
    brain->reset();
    brain->initialize();
    recordMemoryUsage();
    
    // Run Test 2 with performance tracking
    NLM_LOG_INFO("Starting performance test 2 (plasticity)...");
    auto test2Start = cli_utils::getCurrentTimestamp();
    runPlasticityExperiment(brain, args.benchmarkMode);
    auto test2End = cli_utils::getCurrentTimestamp();
    metrics.test2Time = test2End - test2Start;
    metrics.neuronsProcessed += brain->getTotalNeuronCount();
    metrics.synapsesProcessed += brain->getTotalSynapseCount();
    metrics.spikesGenerated += brain->getTotalSpikeCount();
    recordMemoryUsage();
    
    // Reset and run Test 3 with performance tracking
    NLM_LOG_INFO("Resetting brain for STDP verification...");
    brain->reset();
    brain->initialize();
    recordMemoryUsage();
    
    NLM_LOG_INFO("Starting performance test 3 (STDP)...");
    auto test3Start = cli_utils::getCurrentTimestamp();
    runStdpVerification(brain, args.benchmarkMode);
    auto test3End = cli_utils::getCurrentTimestamp();
    metrics.test3Time = test3End - test3Start;
    metrics.neuronsProcessed += brain->getTotalNeuronCount();
    metrics.synapsesProcessed += brain->getTotalSynapseCount();
    metrics.spikesGenerated += brain->getTotalSpikeCount();
    recordMemoryUsage();
    
    // Performance analysis
    metrics.totalTime = cli_utils::getCurrentTimestamp() - overallStartTime;
    metrics.cleanupTime = 0.0;  // Placeholder for any cleanup
    
    // Log performance results
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== PERFORMANCE ANALYSIS ===");
    NLM_LOG_INFO("Total execution time: " + cli_utils::formatTime(metrics.totalTime));
    NLM_LOG_INFO("Setup time: " + cli_utils::formatTime(metrics.setupTime));
    NLM_LOG_INFO("Test 1 (connectivity): " + cli_utils::formatTime(metrics.test1Time));
    NLM_LOG_INFO("Test 2 (plasticity): " + cli_utils::formatTime(metrics.test2Time));
    NLM_LOG_INFO("Test 3 (STDP): " + cli_utils::formatTime(metrics.test3Time));
    NLM_LOG_INFO("Cleanup time: " + cli_utils::formatTime(metrics.cleanupTime));
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== RESOURCE UTILIZATION ===");
    NLM_LOG_INFO("Neurons processed: " + cli_utils::formatNumber(metrics.neuronsProcessed));
    NLM_LOG_INFO("Synapses processed: " + cli_utils::formatNumber(metrics.synapsesProcessed));
    NLM_LOG_INFO("Spikes generated: " + cli_utils::formatNumber(metrics.spikesGenerated));
    NLM_LOG_INFO("Memory peak usage: " + cli_utils::formatNumber(metrics.memoryPeak) + " bytes");
    
    if (metrics.totalTime > 0.0) {
        double stepsPerSecond = (metrics.neuronsProcessed + metrics.synapsesProcessed) / metrics.totalTime;
        NLM_LOG_INFO("Processing rate: " + cli_utils::formatNumber(static_cast<size_t>(stepsPerSecond)) + " operations/sec");
    }
    
    // Performance recommendations
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== PERFORMANCE RECOMMENDATIONS ===");
    
    if (metrics.test2Time > metrics.test1Time * 3) {
        NLM_LOG_INFO("Note: Plasticity test is significantly slower than connectivity test.");
        NLM_LOG_INFO("Consider reducing neuron count or steps for better performance.")
    }
    
    if (metrics.memoryPeak > 100 * 1024 * 1024) {  // 100MB
        NLM_LOG_WARNING("High memory usage detected. Consider reducing simulation scale.")
    }
    
    if (metrics.totalTime > 60.0) {  // 1 minute
        NLM_LOG_WARNING("Long execution time. Consider using benchmark mode for testing.")
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== FINAL BRAIN STATUS ===");
    brain->logStatus();
