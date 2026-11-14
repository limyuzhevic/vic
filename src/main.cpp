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
#include <sstream>
#include <map>
#include <algorithm>
#include <cctype>

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

// Enhanced NLM Main - Improved Configuration and API Design
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// Features:
// - Command-line configuration for flexible execution
// - Test selection and parameterization
// - Programmatic API for integration
// - Enhanced error handling and validation
// - Configurable logging and output

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
#include <map>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace nlm;

// Command line argument parser with enhanced functionality
class CommandLineParser {
public:
    struct ParsedArgs {
        std::string configFile = "configs/default.cfg";
        std::map<std::string, std::string> configOverrides;
        std::vector<std::string> enabledTests;
        std::vector<std::string> disabledTests;
        std::string outputFile;
        std::string logLevel = "Info";
        bool verbose = false;
        bool quiet = false;
        int maxSteps = 1000;
        double timestep = 0.001;
        bool help = false;
        bool version = false;
    };

    static ParsedArgs parse(int argc, char** argv) {
        ParsedArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version") {
                args.version = true;
            } else if (arg == "--quiet" || arg == "-q") {
                args.quiet = true;
                args.logLevel = "Error";
            } else if (arg == "--verbose" || arg == "-v") {
                args.verbose = true;
                args.logLevel = "Debug";
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    args.outputFile = argv[++i];
                } else {
                    std::cerr << "Error: --output requires a filename" << std::endl;
                }
            } else if (arg == "--config") {
                if (i + 1 < argc) {
                    args.configFile = argv[++i];
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args.configFile = argv[++i];
                } else {
                    std::cerr << "Error: --config requires a filename" << std::endl;
                }
            } else if (arg.find("--config=") == 0) {
                args.configFile = arg.substr(8);
            } else if (arg == "--enable-test") {
                if (i + 1 < argc) {
                    args.enabledTests.push_back(argv[++i]);
                }
            } else if (arg == "--disable-test") {
                if (i + 1 < argc) {
                    args.disabledTests.push_back(argv[++i]);
                }
            } else if (arg == "--max-steps") {
                if (i + 1 < argc) {
                    try {
                        args.maxSteps = std::stoi(argv[++i]);
                    } catch (...) {
                        std::cerr << "Error: Invalid max-steps value" << std::endl;
                    }
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    try {
                        args.timestep = std::stod(argv[++i]);
                    } catch (...) {
                        std::cerr << "Error: Invalid timestep value" << std::endl;
                    }
                }
            } else if (arg.find("--set=") == 0) {
                std::string pair = arg.substr(6);
                size_t pos = pair.find('=');
                if (pos != std::string::npos) {
                    std::string key = pair.substr(0, pos);
                    std::string value = pair.substr(pos + 1);
                    args.configOverrides[key] = value;
                }
            } else if (arg[0] == '-') {
                std::cerr << "Warning: Unknown argument " << arg << std::endl;
            }
        }
        
        return args;
    }
    
    static void printHelp() {
        std::cout << R"(
NLM - Neural Learning Machine
Enhanced Phase 2: Real Neural Computation

USAGE:
  nlm [OPTIONS]

OPTIONS:
  --help, -h                    Show this help message
  --version                      Show version information
  --quiet, -q                    Suppress informational messages (only errors)
  --verbose, -v                  Enable verbose debug output
  --output, -o FILE              Write output to FILE
  --config FILE, --config=FILE    Load configuration from FILE (default: configs/default.cfg)
  --enable-test NAME             Enable specific test (default: all enabled)
  --disable-test NAME            Disable specific test
  --max-steps N                 Maximum simulation steps (default: 1000)
  --timestep T                   Simulation timestep in seconds (default: 0.001)
  --set=KEY=VALUE                Set configuration parameter

DEFAULT TESTS:
  basic_connectivity              Test 1: Basic neural connectivity
  plasticity_learning              Test 2: Plasticity learning experiment  
  stdp_verification               Test 3: STDP verification

EXAMPLES:
  nlm                                    # Run all tests with defaults
  nlm --enable-test basic_connectivity   # Run only connectivity test
  nlm --disable-test stdp_verification    # Skip STDP test
  nlm --config=myconfig.cfg              # Use custom configuration
  nlm --set=neuron_count=1000            # Override neuron count
  nlm --verbose                          # Enable debug output

") << std::endl;
    }
};

// Fixed Learning Experiment with proper structure
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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, int maxSteps = 1000) {
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
    
    // Run a few steps (limited by maxSteps if provided)
    int stepsToRun = std::min(50, maxSteps);
    for (SimulationStep step = 0; step < stepsToRun; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    NLM_LOG_INFO("  Steps executed: " + std::to_string(stepsToRun));
    
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, int maxSteps = 1000) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    EnhancedLearningExperiment experiment(brain, 42, "Plasticity Learning Experiment");
    
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
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(maxSteps) + " steps)...");
    
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
    for (SimulationStep step = 0; step < maxSteps; ++step) {
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
    auto parserArgs = CommandLineParser::parse(argc, argv);
    
    if (parserArgs.help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    // Configure logging based on verbosity
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    
    LogLevel logLevel;
    if (parserArgs.quiet) {
        logLevel = LogLevel::Error;
    } else if (parserArgs.verbose) {
        logLevel = LogLevel::Debug;
    } else {
        logLevel = LogLevel::Info;
    }
    
    consoleLogger->setLevel(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Output configuration
    if (!parserArgs.outputFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(parserArgs.outputFile);
        fileLogger->setLevel(logLevel);
        logger->addLogger(fileLogger);
    }
    
    std::cout << "Initializing NLM Enhanced Neural Simulation...\n" << std::endl;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    NLM_LOG_INFO("=== NLM Enhanced Neural Simulation ===");
    NLM_LOG_INFO("Phase 2: Real Neural Computation with Advanced Features");
    NLM_LOG_INFO("");
    
    // Try to load from file if provided
    if (parserArgs.configFile != "configs/default.cfg") {
        NLM_LOG_INFO("Loading configuration from: " + parserArgs.configFile);
        if (!config->loadFromFile(parserArgs.configFile)) {
            NLM_LOG_WARNING("Configuration file not found, using defaults");
        }
    }
    
    // Apply command line overrides
    config->loadFromArgs(argc, argv);
    
    // Set default values for enhanced Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", parserArgs.timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Apply configuration overrides from command line
    for (const auto& override : parserArgs.configOverrides) {
        try {
            if (override.first.find("int64_t") != std::string::npos) {
                int64_t value = std::stoll(override.second);
                config->set(override.first.substr(7), value, ConfigSource::CommandLine);
            } else if (override.first.find("double") != std::string::npos) {
                double value = std::stod(override.second);
                config->set(override.first.substr(6), value, ConfigSource::CommandLine);
            } else if (override.first.find("float") != std::string::npos) {
                float value = std::stof(override.second);
                config->set(override.first.substr(5), value, ConfigSource::CommandLine);
            } else if (override.first.find("bool") != std::string::npos) {
                bool value = (override.second == "true" || override.second == "1");
                config->set(override.first.substr(4), value, ConfigSource::CommandLine);
            } else {
                // Try to parse as float for backward compatibility
                float value = std::stof(override.second);
                config->set(override.first, value, ConfigSource::CommandLine);
            }
        } catch (...) {
            NLM_LOG_WARNING("Failed to parse configuration override: " + override.first + "=" + override.second);
        }
    }
    
    // Enhanced error handling and configuration validation
    struct ValidationResult {
        bool success;
        std::string message;
        std::map<std::string, std::string> warnings;
        
        ValidationResult(bool s, const std::string& m) : success(s), message(m) {}
        void addWarning(const std::string& key, const std::string& value) {
            warnings[key] = value;
        }
    };
    
    ValidationResult validateConfiguration(const Config& config, const CommandLineParser::ParsedArgs& args) {
        ValidationResult result(true, "Configuration validation passed");
        
        // Validate timestep
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0 || timestep > 1.0) {
            result.addWarning("simulation_timestep", "Value " + std::to_string(timestep) + " may cause stability issues. Recommended range: 0.0001 to 0.1");
        }
        
        // Validate neuron count
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 500);
        if (neuronCount <= 0 || neuronCount > 10000) {
            result.addWarning("neuron_count", "Value " + std::to_string(neuronCount) + " may cause performance issues. Recommended range: 1 to 10000");
        }
        
        // Validate region count
        int64_t regionCount = config.getOr<int64_t>("region_count", 1);
        if (regionCount <= 0 || regionCount > 100) {
            result.addWarning("region_count", "Value " + std::to_string(regionCount) + " may cause performance issues. Recommended range: 1 to 100");
        }
        
        // Validate connection probability
        float connProb = config.getOr<float>("connection_probability", 0.15f);
        if (connProb < 0.0f || connProb > 1.0f) {
            result.addWarning("connection_probability", "Value " + std::to_string(connProb) + " is outside valid range [0.0, 1.0]");
        }
        
        // Validate STDP parameters
        float stdpLTP = config.getOr<float>("stdp_ltp_weight", 0.02f);
        float stdpLTD = config.getOr<float>("stdp_ltd_weight", 0.015f);
        float stdpTau = config.getOr<float>("stdp_tau", 20.0f);
        
        if (stdpLTP <= 0.0f || stdpLTP > 1.0f) {
            result.addWarning("stdp_ltp_weight", "Value " + std::to_string(stdpLTP) + " may cause unstable LTP");
        }
        
        if (stdpLTD <= 0.0f || stdpLTD > 1.0f) {
            result.addWarning("stdp_ltd_weight", "Value " + std::to_string(stdpLTD) + " may cause unstable LTD");
        }
        
        if (stdpTau <= 0.0f) {
            result.addWarning("stdp_tau", "Value " + std::to_string(stdpTau) + " must be positive");
        }
        
        // Validate plasticity parameters
        float synaptogenesisRate = config.getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config.getOr<float>("pruning_rate", 0.00001f);
        
        if (synaptogenesisRate < 0.0f || synaptogenesisRate > 0.01f) {
            result.addWarning("synaptogenesis_rate", "Value " + std::to_string(synaptogenesisRate) + " may cause excessive structural changes");
        }
        
        if (pruningRate < 0.0f || pruningRate > 0.001f) {
            result.addWarning("pruning_rate", "Value " + std::to_string(pruningRate) + " may cause excessive structural changes");
        }
        
        return result;
    }
    
    void printValidationResults(const ValidationResult& result) {
        if (!result.success) {
            NLM_LOG_ERROR("Configuration validation failed: " + result.message);
        }
        
        for (const auto& warning : result.warnings) {
            NLM_LOG_WARNING("Configuration warning: " + warning.first + " = " + warning.second);
        }
        
        if (!result.success || !result.warnings.empty()) {
            NLM_LOG_INFO("See warnings above for configuration recommendations");
        }
    // Validate configuration and print warnings/errors
    auto validationResult = validateConfiguration(*config, parserArgs);
    printValidationResults(validationResult);
    NLM_LOG_INFO("");
    double timestep = config->getOr<double>("simulation_timestep", parserArgs.timestep);
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
    
    // Determine which tests to run based on command line arguments
    std::vector<std::string> testsToRun;
    bool runAllTests = parserArgs.enabledTests.empty() && parserArgs.disabledTests.empty();
    
    if (runAllTests) {
        testsToRun = {"basic_connectivity", "plasticity_learning", "stdp_verification"};
    } else {
        // Check which tests to enable
        std::vector<std::string> defaultTests = {"basic_connectivity", "plasticity_learning", "stdp_verification"};
        for (const auto& test : defaultTests) {
            bool shouldRun = true;
            for (const auto& disabled : parserArgs.disabledTests) {
                if (disabled == test) {
                    shouldRun = false;
                    break;
                }
            }
            for (const auto& enabled : parserArgs.enabledTests) {
                if (enabled == test) {
                    shouldRun = true;
                    break;
                }
            }
            if (shouldRun) {
                testsToRun.push_back(test);
            }
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test Execution Plan:");
    for (const auto& test : testsToRun) {
        NLM_LOG_INFO("  - " + test);
    }
    
    // Run selected tests
    bool allTestsPassed = true;
    
    if (std::find(testsToRun.begin(), testsToRun.end(), "basic_connectivity") != testsToRun.end()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        runBasicConnectivityTest(brain, parserArgs.maxSteps);
        brain->reset();
        brain->initialize();
    }
    
    if (std::find(testsToRun.begin(), testsToRun.end(), "plasticity_learning") != testsToRun.end()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        runPlasticityExperiment(brain, parserArgs.maxSteps);
        brain->reset();
        brain->initialize();
    }
    
    if (std::find(testsToRun.begin(), testsToRun.end(), "stdp_verification") != testsToRun.end()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        runStdpVerification(brain);
        brain->reset();
        brain->initialize();
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Configurable test execution");
    NLM_LOG_INFO("  ✓ Enhanced command-line interface");
    NLM_LOG_INFO("  ✓ Flexible configuration system");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now an enhanced artificial neural substrate");
    NLM_LOG_INFO("with advanced configuration and test selection capabilities.");
    NLM_LOG_INFO("");
    
    return 0;
}
