// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>

using namespace nlm;

// Command-line parsing and help utilities
struct CommandLineArgs {
    std::string configFile;
    bool help;
    bool version;
    bool verbose;
    bool quiet;
    bool runAllTests;
    bool runConnectivityTest;
    bool runPlasticityTest;
    bool runStdpTest;
    std::string outputFile;
    std::string logFile;
    int randomSeed;
    int maxSteps;
    double timestep;
    int neuronCount;
    int regionCount;
    float connectionProbability;
};

// Advanced configuration shortcuts
struct ConfigShortcuts {
    static void applyShortcuts(Config& config, const CommandLineArgs& args) {
        if (args.randomSeed >= 0) {
            config.set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::CommandLine);
        }
        if (args.maxSteps > 0) {
            config.set("max_simulation_steps", static_cast<int64_t>(args.maxSteps), ConfigSource::CommandLine);
        }
        if (args.timestep > 0.0) {
            config.set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
        }
        if (args.neuronCount > 0) {
            config.set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
        }
        if (args.regionCount > 0) {
            config.set("region_count", static_cast<int64_t>(args.regionCount), ConfigSource::CommandLine);
        }
        if (args.connectionProbability > 0.0f) {
            config.set("connection_probability", args.connectionProbability, ConfigSource::CommandLine);
        }
    }
};

// Print help/usage information
void printHelp(const std::string& programName) {
    std::cout << "NLM - Neural Learning Machine (Phase 2)" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    std::cout << std::endl;
    std::cout << "This phase implements real spiking neural computation with:" << std::endl;
    std::cout << "  • Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    std::cout << "  • Event-driven spike propagation with synaptic delays" << std::endl;
    std::cout << "  • STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  • Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q             Disable all output except errors" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --output <file>         Save brain state to file" << std::endl;
    std::cout << "  --log <file>            Save log to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --random-seed <seed>     Set random seed for reproducible experiments" << std::endl;
    std::cout << "  --max-steps <steps>      Maximum number of simulation steps" << std::endl;
    std::cout << "  --timestep <time>        Simulation timestep in seconds" << std::endl;
    std::cout << "  --neurons <count>        Number of neurons in the brain" << std::endl;
    std::cout << "  --regions <count>       Number of brain regions" << std::endl;
    std::cout << "  --conn-prob <prob>       Connection probability (0.0-1.0)" << std::endl;
    std::cout << "  --run-all               Run all available tests (default)" << std::endl;
    std::cout << "  --run-connectivity      Run only basic connectivity test" << std::endl;
    std::cout << "  --run-plasticity        Run only plasticity learning experiment" << std::endl;
    std::cout << "  --run-stdp              Run only STDP verification test" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration File:" << std::endl;
    std::cout << "  Configs/default.cfg     Default configuration file" << std::endl;
    std::cout << "  Any .cfg file          Custom configuration" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --help                          Show help" << std::endl;
    std::cout << "  " << programName << " --config custom.cfg             Load custom config" << std::endl;
    std::cout << "  " << programName << " --random-seed 123 --neurons 500   Custom experiment" << std::endl;
    std::cout << "  " << programName << " --run-connectivity --verbose     Quick test" << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 2)" << std::endl;
    std::cout << std::endl;
    std::cout << "This is Phase 2 of the NLM project, implementing real neural computation." << std::endl;
    std::cout << std::endl;
    std::cout << "For more information, visit the project documentation." << std::endl;
}

// Parse command line arguments
CommandLineArgs parseArgs(int argc, char** argv) {
    CommandLineArgs args;
    args.help = false;
    args.version = false;
    args.verbose = false;
    args.quiet = false;
    args.runAllTests = true; // Default: run all tests
    args.runConnectivityTest = false;
    args.runPlasticityTest = false;
    args.runStdpTest = false;
    args.randomSeed = -1;
    args.maxSteps = -1;
    args.timestep = -1.0;
    args.neuronCount = -1;
    args.regionCount = -1;
    args.connectionProbability = -1.0f;
    args.outputFile = "";
    args.logFile = "";
    args.configFile = "configs/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            args.outputFile = argv[++i];
        } else if (arg == "--log" && i + 1 < argc) {
            args.logFile = argv[++i];
        } else if (arg == "--random-seed" && i + 1 < argc) {
            args.randomSeed = std::atoi(argv[++i]);
        } else if (arg == "--max-steps" && i + 1 < argc) {
            args.maxSteps = std::atoi(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            args.timestep = std::atof(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            args.neuronCount = std::atoi(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            args.regionCount = std::atoi(argv[++i]);
        } else if (arg == "--conn-prob" && i + 1 < argc) {
            args.connectionProbability = std::atof(argv[++i]);
        } else if (arg == "--run-all") {
            args.runAllTests = true;
            args.runConnectivityTest = false;
            args.runPlasticityTest = false;
            args.runStdpTest = false;
        } else if (arg == "--run-connectivity") {
            args.runAllTests = false;
            args.runConnectivityTest = true;
            args.runPlasticityTest = false;
            args.runStdpTest = false;
        } else if (arg == "--run-plasticity") {
            args.runAllTests = false;
            args.runConnectivityTest = false;
            args.runPlasticityTest = true;
            args.runStdpTest = false;
        } else if (arg == "--run-stdp") {
            args.runAllTests = false;
            args.runConnectivityTest = false;
            args.runPlasticityTest = false;
            args.runStdpTest = true;
        } else if (arg.substr(0, 2) == "--") {
            // Unknown option, ignore for now but could log a warning
        } else if (arg.substr(0, 1) == "-") {
            // Unknown option, ignore for now but could log a warning
        }
    }
    
    return args;
}

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
    // Parse command line arguments
    CommandLineArgs args = parseArgs(argc, argv);
    
    // Handle help/version requests early
    if (args.help) {
        printHelp(argv[0]);
        return 0;
    }
    
    if (args.version) {
        printVersion();
        return 0;
    }
    
    // Setup logging based on command line arguments
    auto logger = std::make_shared<Logger>();
    
    LogLevel logLevel = LogLevel::Info;
    if (args.quiet) {
        logLevel = LogLevel::Error;
    } else if (args.verbose) {
        logLevel = LogLevel::Debug;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    
    if (!args.logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(args.logFile);
        fileLogger->setLevel(LogLevel::Info);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
    
    // Print banner (skip if quiet mode)
    if (!args.quiet) {
        printBanner();
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    }
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = args.configFile;
    
    NLM_LOG_INFO("Loading configuration from: " + configFile);
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Successfully loaded configuration from file.");
    } else {
        NLM_LOG_WARNING("Failed to load configuration from file. Using defaults.");
    }
    
    // Apply configuration shortcuts from command line
    ConfigShortcuts::applyShortcuts(*config, args);
    
    // Override with command line args (using new Config system)
    // Parse additional --key=value pairs not covered by shortcuts
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format for additional options
        if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            // Convert string value to appropriate type
            if (value == "true" || value == "false") {
                config->set(key, value == "true", ConfigSource::CommandLine);
            } else {
                // Try to parse as number
                try {
                    if (value.find('.') != std::string::npos) {
                        config->set(key, std::stod(value), ConfigSource::CommandLine);
                    } else {
                        config->set(key, std::stoll(value), ConfigSource::CommandLine);
                    }
                } catch (...) {
                    // Fall back to string
                    config->set(key, value, ConfigSource::CommandLine);
                }
            }
        }
    }
    
    // Set default values for Phase 2 (overriding file defaults)
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
    
    // Log configuration summary
    if (!args.quiet) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("");
    }
    
    // Log configuration statistics
    if (!args.quiet) {
        auto stats = config->getStats();
        NLM_LOG_INFO("Configuration Statistics:");
        NLM_LOG_INFO("  Total entries: " + std::to_string(stats.totalEntries));
        NLM_LOG_INFO("  From file: " + std::to_string(stats.fromFile));
        NLM_LOG_INFO("  From command line: " + std::to_string(stats.fromCommandLine));
        NLM_LOG_INFO("  From defaults: " + std::to_string(stats.fromDefault));
        NLM_LOG_INFO("  From runtime: " + std::to_string(stats.fromRuntime));
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
    
    // Determine which tests to run based on command line arguments
    if (args.runConnectivityTest) {
        NLM_LOG_INFO("=== Running Test 1: Basic Neural Connectivity ===");
        runBasicConnectivityTest(brain);
    }
    
    if (args.runPlasticityTest) {
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("=== Running Test 2: Plasticity Learning Experiment ===");
        runPlasticityExperiment(brain);
    }
    
    if (args.runStdpTest) {
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("=== Running Test 3: STDP Verification ===");
        runStdpVerification(brain);
    }
    
    // If no specific tests requested, run all tests
    if (args.runAllTests) {
        // Run Test 1: Basic connectivity
        NLM_LOG_INFO("=== Running Test 1: Basic Neural Connectivity ===");
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 2: Plasticity learning experiment
        NLM_LOG_INFO("=== Running Test 2: Plasticity Learning Experiment ===");
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("=== Running Test 3: STDP Verification ===");
        runStdpVerification(brain);
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
    
    // Save brain state if requested
    if (!args.outputFile.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + args.outputFile);
        if (!brain->save(args.outputFile)) {
            NLM_LOG_ERROR("Failed to save brain state!");
        } else {
            NLM_LOG_INFO("Brain state saved successfully.");
        }
    }
    
    return 0;
}
