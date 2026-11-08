// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// IMPROVED VERSION: Enhanced with better error handling, configuration loading,
// command-line argument parsing, logging, and code organization.

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
#include <cmath>

using namespace nlm;

// Enhanced logging helper functions
void logSection(const std::string& title) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== " + title + " ===");
    NLM_LOG_INFO("");
}

void logProgress(const std::string& message, bool verbose = false) {
    if (verbose || true) { // Can be configured
        NLM_LOG_INFO("  " + message);
    }
}

// Improved help and version functions
void printHelp() {
    std::cout << R"(
NLM (熙然) - Neural Learning Machine
Phase 2: Real Neural Computation

USAGE:
    nlm [OPTIONS]

OPTIONS:
    -h, --help              Show this help message
    -v, --version           Show version information
    -V, --verbose           Enable verbose output
    -q, --quiet             Suppress non-essential output
    
    --config <file>         Load configuration from file (default: configs/default.cfg)
    --mode <mode>           Demo mode: connectivity, plasticity, stdp, all
    --plasticity-steps <n>   Number of steps for plasticity experiment (default: 1000)
    --current-inject <val>  Current injection amount for experiments (default: 30.0)
    --sensory-neurons <n>    Number of sensory neurons for patterns (default: 20)
    --test-neuron-count <n>  Number of neurons in brain (default: 500)

EXAMPLES:
    nlm                          # Run all tests with defaults
    nlm --mode connectivity      # Run only connectivity test
    nlm --mode plasticity         # Run plasticity experiment
    nlm --mode stdp               # Run STDP verification
    nlm --config myconfig.cfg     # Use custom config file
    nlm --verbose --quiet         # Enable verbose output
    nlm --help                    # Show help message
    nlm --version                 # Show version information

For more information, visit: https://github.com/your-org/nlm
    )" << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 2: Real Neural Computation v0.1.0" << std::endl;
    std::cout << "Built with C++20, implementing LIF neuron dynamics, STDP, and structural plasticity." << std::endl;
    std::cout << "Experimental artificial developmental brain for research purposes." << std::endl;
}

// Configuration structure with all possible options
struct DemoOptions {
    std::string configFile = "configs/default.cfg";
    bool help = false;
    bool version = false;
    bool verbose = false;
    bool quiet = false;
    std::string mode = "connectivity"; // "connectivity", "plasticity", "stdp", "all"
    SimulationStep plasticitySteps = 1000;
    SimulationStep stdpTrials = 50;
    float currentInjection = 30.0f;
    size_t sensoryNeurons = 20;
    int64_t testNeuronCount = 500;
    bool saveCheckpoint = false;
    std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
};

DemoOptions parseArguments(int argc, char** argv) {
    DemoOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-v") {
            options.version = true;
        } else if (arg == "--verbose" || arg == "-V") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            } else {
                NLM_LOG_ERROR("Error: --config option requires a value");
                throw std::runtime_error("Invalid command line arguments");
            }
        } else if (arg.substr(0, 6) == "--mode") {
            if (arg.find('=') != std::string::npos) {
                options.mode = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.mode = argv[++i];
            } else {
                NLM_LOG_ERROR("Error: --mode option requires a value");
                throw std::runtime_error("Invalid command line arguments");
            }
        } else if (arg.substr(0, 17) == "--plasticity-steps") {
            if (arg.find('=') != std::string::npos) {
                options.plasticitySteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.plasticitySteps = std::stoull(argv[++i]);
            } else {
                NLM_LOG_ERROR("Error: --plasticity-steps option requires a value");
                throw std::runtime_error("Invalid command line arguments");
            }
        } else if (arg.substr(0, 13) == "--current-inject") {
            if (arg.find('=') != std::string::npos) {
                options.currentInjection = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.currentInjection = std::stof(argv[++i]);
            } else {
                NLM_LOG_ERROR("Error: --current-inject option requires a value");
                throw std::runtime_error("Invalid command line arguments");
            }
        } else if (arg.substr(0, 16) == "--sensory-neurons") {
            if (arg.find('=') != std::string::npos) {
                options.sensoryNeurons = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.sensoryNeurons = std::stoull(argv[++i]);
            } else {
                NLM_LOG_ERROR("Error: --sensory-neurons option requires a value");
                throw std::runtime_error("Invalid command line arguments");
            }
        } else if (arg.substr(0, 19) == "--test-neuron-count") {
            if (arg.find('=') != std::string::npos) {
                options.testNeuronCount = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.testNeuronCount = std::stoll(argv[++i]);
            } else {
                NLM_LOG_ERROR("Error: --test-neuron-count option requires a value");
                throw std::runtime_error("Invalid command line arguments");
            }
        } else if (arg == "--save-checkpoint") {
            options.saveCheckpoint = true;
        } else if (arg.substr(0, 18) == "--checkpoint-path") {
            if (arg.find('=') != std::string::npos) {
                options.checkpointPath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.checkpointPath = argv[++i];
            } else {
                NLM_LOG_ERROR("Error: --checkpoint-path option requires a value");
                throw std::runtime_error("Invalid command line arguments");
            }
        } else {
            NLM_LOG_ERROR("Unknown option: " + arg);
            throw std::runtime_error("Invalid command line arguments");
        }
    }
    
    return options;
}

void validateOptions(const DemoOptions& options) {
    if (options.testNeuronCount <= 0) {
        throw std::runtime_error("Error: --test-neuron-count must be positive");
    }
    if (options.plasticitySteps == 0) {
        throw std::runtime_error("Error: --plasticity-steps must be positive");
    }
    if (options.sensoryNeurons == 0) {
        throw std::runtime_error("Error: --sensory-neurons must be positive");
    }
}

// Enhanced Learning Experiment with better statistics and error handling
struct EnhancedLearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    double learningScore;
    
    EnhancedLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0), learningScore(0.0) {}
    
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
        
        computeLearningScore();
    }
    
    void computeLearningScore() {
        if (initialWeights.empty() || finalWeights.empty()) {
            learningScore = 0.0;
            return;
        }
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        double totalWeightChange = 0.0;
        size_t significantChanges = 0;
        
        for (size_t i = 0; i < minSize; ++i) {
            double delta = finalWeights[i] - initialWeights[i];
            totalWeightChange += std::abs(delta);
            if (std::abs(delta) > 0.01f) {
                significantChanges++;
            }
        }
        
        // Calculate learning score: normalized total change with bonus for significant changes
        double maxPossibleChange = minSize * 2.0; // Max change per synapse (1.0 to -1.0)
        learningScore = (totalWeightChange / maxPossibleChange) * 
                       (1.0 + (significantChanges * 0.1));
    }
    
    void printDetailedResults() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Enhanced Learning Experiment Results ===");
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
        NLM_LOG_INFO("Learning Analysis:");
        NLM_LOG_INFO("  Learning Score: " + std::to_string(learningScore));
        NLM_LOG_INFO("  Spike Activity:");
        NLM_LOG_INFO("    Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("    Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                               (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
            if (learningScore > 1.0) {
                NLM_LOG_INFO("  Learning quality: GOOD (selective pattern learning)");
            } else {
                NLM_LOG_INFO("  Learning quality: MODERATE (saturated changes)");
            }
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

// Improved test functions with better error handling and reporting
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, const DemoOptions& options) {
    logSection("Test 1: Basic Neural Connectivity");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("Error: No regions found for connectivity test");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    logProgress("Injecting current into " + std::to_string(std::min(size_t(10), neurons.size())) + " neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(options.currentInjection);  // Configurable injection
    }
    
    // Run a few steps
    logProgress("Running " + std::to_string(50) + " simulation steps...");
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, const DemoOptions& options) {
    logSection("Test 2: Plasticity Learning Experiment");
    
    EnhancedLearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    logProgress("Applying repeated input patterns (" + std::to_string(options.plasticitySteps) + " steps)...");
    
    // Improved pattern: stimulate controlled number of sensory neurons
    // This avoids the old problem where every neuron fired simultaneously
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < options.sensoryNeurons) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    for (SimulationStep step = 0; step < options.plasticitySteps; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, options.currentInjection);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            logProgress("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.printDetailedResults();
}

void runStdpVerification(std::shared_ptr<Brain> brain, const DemoOptions& options) {
    logSection("Test 3: STDP Verification");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("Error: No regions found for STDP test");
        return;
    }
    
    // Use one real synapse and its own pre/post neurons for more accurate testing
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    
    // Try to find a synapse with valid pre/post neurons
    Synapse* syn = nullptr;
    for (auto& s : synapses) {
        auto sourceNeurons = brain->getNeuronsByRegion();
        bool valid = false;
        for (auto& r : sourceNeurons) {
            for (auto* n : r->getAllNeurons()) {
                if (n->getId() == s->getSourceNeuron() || n->getId() == s->getDestinationNeuron()) {
                    valid = true;
                    break;
                }
            }
            if (valid) break;
        }
        if (valid) {
            syn = s.get();
            break;
        }
    }
    
    if (!syn) {
        NLM_LOG_INFO("  No valid synapse found for STDP test");
        return;
    }
    
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < options.stdpTrials; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), options.currentInjection);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), options.currentInjection);
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

void runModeBasedTests(std::shared_ptr<Brain> brain, const DemoOptions& options) {
    switch (options.mode) {
        case "connectivity":
            runBasicConnectivityTest(brain, options);
            break;
        case "plasticity":
            runPlasticityExperiment(brain, options);
            break;
        case "stdp":
            runStdpVerification(brain, options);
            break;
        case "all":
            // Run all tests
            runBasicConnectivityTest(brain, options);
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain, options);
            brain->reset();
            brain->initialize();
            runStdpVerification(brain, options);
            break;
        default:
            NLM_LOG_ERROR("Unknown mode: " + options.mode);
            NLM_LOG_INFO("Valid modes: connectivity, plasticity, stdp, all");
            break;
    }
}

int main(int argc, char** argv) {
    try {
        printBanner();
        
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Parse command line arguments
        DemoOptions options;
        try {
            options = parseArguments(argc, argv);
        } catch (const std::runtime_error& e) {
            NLM_LOG_ERROR(std::string("Failed to parse arguments: ") + e.what());
            return 1;
        }
        
        // Handle help and version before continuing
        if (options.help) {
            printHelp();
            return 0;
        }
        
        if (options.version) {
            printVersion();
            return 0;
        }
        
        // Validate options
        try {
            validateOptions(options);
        } catch (const std::runtime_error& e) {
            NLM_LOG_ERROR(e.what());
            printHelp();
            return 1;
        }
        
        // Load configuration
        auto config = std::make_shared<Config>();
        
        const bool configLoaded = config->loadFromFile(options.configFile);
        
        if (configLoaded) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            if (options.verbose) {
                NLM_LOG_INFO("Config file not found or invalid: " + options.configFile);
                NLM_LOG_INFO("Using default configuration.");
            }
        }
        
        // Override with command line args
        if (!config->loadFromArgs(argc, argv)) {
            NLM_LOG_ERROR("Invalid command line arguments");
            return 1;
        }
        
        // Set default values for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", options.testNeuronCount, ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
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
        
// Enhanced configuration validation function
bool validateConfiguration(const nlm::Config& config) {
    bool isValid = true;
    
    // Validate required fields
    if (!config.has("random_seed")) {
        NLM_LOG_ERROR("Configuration validation failed: Missing required field 'random_seed'");
        isValid = false;
    } else {
        int64_t seed = config.getOr<int64_t>("random_seed", 0);
        if (seed < 0) {
            NLM_LOG_ERROR("Configuration validation failed: Invalid 'random_seed' value. Must be non-negative.");
            isValid = false;
        }
    }
    
    if (!config.has("simulation_timestep")) {
        NLM_LOG_ERROR("Configuration validation failed: Missing required field 'simulation_timestep'");
        isValid = false;
    } else {
        double timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            NLM_LOG_ERROR("Configuration validation failed: Invalid 'simulation_timestep' value. Must be > 0 and <= 1.0.");
            isValid = false;
        }
    }
    
    if (!config.has("neuron_count")) {
        NLM_LOG_ERROR("Configuration validation failed: Missing required field 'neuron_count'");
        isValid = false;
    } else {
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
        if (neuronCount < 100 || neuronCount > 10000) {
            NLM_LOG_ERROR("Configuration validation failed: Invalid 'neuron_count' value. Must be between 100 and 10000.");
            isValid = false;
        }
    }
    
    if (!config.has("region_count")) {
        NLM_LOG_ERROR("Configuration validation failed: Missing required field 'region_count'");
        isValid = false;
    } else {
        int64_t regionCount = config.getOr<int64_t>("region_count", 0);
        if (regionCount < 1 || regionCount > 10) {
            NLM_LOG_ERROR("Configuration validation failed: Invalid 'region_count' value. Must be between 1 and 10.");
            isValid = false;
        }
    }
    
    if (!config.has("connection_probability")) {
        NLM_LOG_ERROR("Configuration validation failed: Missing required field 'connection_probability'");
        isValid = false;
    } else {
        float connProb = config.getOr<float>("connection_probability", 0.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            NLM_LOG_ERROR("Configuration validation failed: Invalid 'connection_probability' value. Must be between 0.0 and 1.0.");
            isValid = false;
        }
    }
    
    // Validate optional fields if present
    if (config.has("stdp_ltp_weight")) {
        float ltpWeight = config.getOr<float>("stdp_ltp_weight", 0.0f);
        if (ltpWeight < 0.0f || ltpWeight > 10.0f) {
            NLM_LOG_ERROR("Configuration validation warning: Invalid 'stdp_ltp_weight' value. Should be between 0.0 and 10.0.");
            // This is a warning, not a failure
        }
    }
    
    if (config.has("stdp_ltd_weight")) {
        float ltdWeight = config.getOr<float>("stdp_ltd_weight", 0.0f);
        if (ltdWeight < 0.0f || ltdWeight > 10.0f) {
            NLM_LOG_ERROR("Configuration validation warning: Invalid 'stdp_ltd_weight' value. Should be between 0.0 and 10.0.");
            // This is a warning, not a failure
        }
    }
    
    if (config.has("stdp_time_constant")) {
        float timeConstant = config.getOr<float>("stdp_time_constant", 0.0f);
        if (timeConstant <= 0.0f) {
            NLM_LOG_ERROR("Configuration validation warning: Invalid 'stdp_time_constant' value. Should be positive.");
            // This is a warning, not a failure
        }
    }
    
    // Validate environment settings
    if (config.has("environment_width")) {
        int64_t width = config.getOr<int64_t>("environment_width", 0);
        if (width < 1 || width > 1000) {
            NLM_LOG_ERROR("Configuration validation warning: Invalid 'environment_width' value. Should be between 1 and 1000.");
        }
    }
    
    if (config.has("environment_height")) {
        int64_t height = config.getOr<int64_t>("environment_height", 0);
        if (height < 1 || height > 1000) {
            NLM_LOG_ERROR("Configuration validation warning: Invalid 'environment_height' value. Should be between 1 and 1000.");
        }
    }
    
    if (config.has("visualization_update_rate")) {
        float updateRate = config.getOr<float>("visualization_update_rate", 0.0f);
        if (updateRate <= 0.0f) {
            NLM_LOG_ERROR("Configuration validation warning: Invalid 'visualization_update_rate' value. Should be positive.");
        }
    }
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation passed.");
    } else {
        NLM_LOG_ERROR("Configuration validation failed. Please check the configuration file.");
    }
    
    return isValid;
}

// Enhanced configuration loading with validation
bool loadAndValidateConfiguration(nlm::Config& config, const std::string& configFile) {
    NLM_LOG_INFO("Loading configuration from: " + configFile);
    
    // Try to load from file
    if (!config.loadFromFile(configFile)) {
        NLM_LOG_ERROR("Failed to load configuration from: " + configFile);
        NLM_LOG_INFO("Using default configuration.");
        return false;
    }
    
    // Validate the loaded configuration
    if (!validateConfiguration(config)) {
        NLM_LOG_ERROR("Configuration validation failed. Using default configuration.");
        return false;
    }
    
    NLM_LOG_INFO("Configuration loaded and validated successfully.");
    return true;
}

// Helper function to display configuration summary
void displayConfigurationSummary(const nlm::Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Configuration Summary ===");
    NLM_LOG_INFO("");
    
    // Display key configuration values
    NLM_LOG_INFO("Brain Configuration:");
    NLM_LOG_INFO("  Neuron count: " + std::to_string(config.getOr<int64_t>("neuron_count", 0)));
    NLM_LOG_INFO("  Region count: " + std::to_string(config.getOr<int64_t>("region_count", 0)));
    NLM_LOG_INFO("  Connection probability: " + std::to_string(config.getOr<float>("connection_probability", 0.0f)));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Plasticity Configuration:");
    NLM_LOG_INFO("  STDP LTP weight: " + std::to_string(config.getOr<float>("stdp_ltp_weight", 0.0f)));
    NLM_LOG_INFO("  STDP LTD weight: " + std::to_string(config.getOr<float>("stdp_ltd_weight", 0.0f)));
    NLM_LOG_INFO("  STDP time constant: " + std::to_string(config.getOr<float>("stdp_time_constant", 0.0f)));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Development Configuration:");
    NLM_LOG_INFO("  Synaptogenesis rate: " + std::to_string(config.getOr<float>("development_synaptogenesis_rate", 0.0f)));
    NLM_LOG_INFO("  Pruning rate: " + std::to_string(config.getOr<float>("development_pruning_rate", 0.0f)));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Neuromodulation Configuration:");
    NLM_LOG_INFO("  Dopamine baseline: " + std::to_string(config.getOr<float>("dopamine_baseline", 0.0f)));
    NLM_LOG_INFO("  Curiosity baseline: " + std::to_string(config.getOr<float>("curiosity_baseline", 0.0f)));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Environment Configuration:");
    NLM_LOG_INFO("  Environment: " + config.getOr<std::string>("environment_name", "Unknown"));
    NLM_LOG_INFO("  World size: " + std::to_string(config.getOr<int64_t>("environment_width", 0)) + " x " + 
                std::to_string(config.getOr<int64_t>("environment_height", 0)));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Simulation Configuration:");
    NLM_LOG_INFO("  Timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.0)) + "s");
    NLM_LOG_INFO("  Max steps: " + std::to_string(config.getOr<int64_t>("max_simulation_steps", 0)));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== End Configuration Summary ===");
}
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error occurred");
        return 1;
    }
}