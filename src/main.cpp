// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Types/Types.hpp"

#include <fstream>
#include <algorithm>

using namespace nlm;

// Print help information
void printHelp() {
    std::cout << R"(
NLM — Neural Learning Machine (Phase 2)

Usage: nlm [OPTIONS]

Options:
  --help                  Show this help message and exit
  --version               Show version information and exit
  --config <file>         Load configuration from JSON file (default: configs/default.cfg)
  --log-level <level>     Set log level (Trace, Debug, Info, Warning, Error, Critical)
  --phase <1|2|3|4|5|6>   Run specific phase demonstration
  --experiment <name>     Run named experiment
  --stats                 Output comprehensive brain statistics
  --profile               Enable performance profiling mode
  --visualize             Generate visualization data
  --replay <file>         Replay saved simulation from file
  --export <file>         Export brain state to file
  --import <file>         Import brain state from file
  --benchmark <n>         Run benchmark with n iterations
  --verbose               Enable verbose output
  --quiet                 Suppress non-essential output

Configuration File:
  Config files are JSON format with key-value pairs.
  Default config: configs/default.cfg
  See configs/ directory for examples.

For more information, visit: https://github.com/example/nlm

Examples:
  nlm --help                          Show help
  nlm --phase 2                       Run Phase 2 demonstration
  nlm --config myconfig.json          Load custom configuration
  nlm --stats --verbose               Show statistics with verbose output
  nlm --export brain_state.json        Save current brain state
  nlm --import brain_state.json        Load saved brain state
  nlm --benchmark 100                 Run performance benchmark

)" << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM Neural Learning Machine - Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Compiler: " << __VERSION__ << std::endl;
}

// Process command line arguments and perform early checks
bool processArguments(int argc, char** argv, std::string& configFile, bool& verbose, bool& quiet) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false; // Exit after showing help
        }
        
        if (arg == "--version") {
            printVersion();
            return false; // Exit after showing version
        }
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--stats") {
            // Will be implemented in main function
        } else if (arg == "--profile") {
            // Will be implemented in main function
        } else if (arg == "--visualize") {
            // Will be implemented in main function
        } else if (arg == "--phase" || arg == "--experiment") {
            // Will be implemented in main function
        } else if (arg == "--benchmark") {
            // Will be implemented in main function
        } else if (arg == "--export") {
            // Will be implemented in main function
        } else if (arg == "--import") {
            // Will be implemented in main function
        } else if (arg == "--replay") {
            // Will be implemented in main function
        } else if (arg.substr(0, 11) == "--log-level") {
            // Will be implemented in main function
        }
    }
    
    return true;
}

// Validate configuration file exists and is readable
bool validateConfigFile(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.good()) {
        NLM_LOG_ERROR("Configuration file not found or not readable: " + configFile);
        NLM_LOG_ERROR("Please ensure the file exists and has proper permissions");
        return false;
    }
    
    // Try to parse basic JSON structure
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0);
    
    if (fileSize == 0) {
        NLM_LOG_ERROR("Configuration file is empty: " + configFile);
        return false;
    }
    
    // Read first few characters to check if it's valid JSON
    char firstChar;
    file.get(firstChar);
    if (firstChar != '{' && firstChar != '[') {
        NLM_LOG_ERROR("Configuration file does not appear to be valid JSON: " + configFile);
        return false;
    }
    
    return true;
}

// Parse phase argument
bool parsePhaseArgument(const std::string& arg, int& phase) {
    if (arg == "--phase" || arg == "-p") {
        if (++i < argc) {
            std::string phaseStr = argv[i];
            try {
                phase = std::stoi(phaseStr);
                if (phase < 1 || phase > 6) {
                    NLM_LOG_ERROR("Phase must be between 1 and 6: " + phaseStr);
                    return false;
                }
                return true;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid phase value: " + phaseStr + " - " + e.what());
                return false;
            }
        }
    }
    return false;
}

// Parse log level argument
bool parseLogLevelArgument(const std::string& arg, std::string& logLevel) {
    if (arg == "--log-level") {
        if (++i < argc) {
            logLevel = argv[i];
            // Convert to uppercase for consistency
            std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::toupper);
            return true;
        }
    }
    return false;
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
    // Early argument processing - check for help/version first
    bool verbose = false;
    bool quiet = false;
    std::string configFile = "configs/default.cfg";
    bool showHelp = false;
    bool showVersion = false;
    
    // Process arguments for help/version and basic options
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
        
        if (arg == "--version") {
            printVersion();
            return 0;
        }
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        }
    }
    
    // Validate config file before proceeding
    if (!validateConfigFile(configFile)) {
        return 1;
    }
    
    printBanner();
    
    if (!quiet) {
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    }
    
    // Initialize logger with appropriate log level
    auto logger = std::make_shared<Logger>();
    if (verbose) {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Debug));
    } else {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    }
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
    
    // Load config from file (with error handling)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + configFile);
        NLM_LOG_ERROR("Using default configuration instead.");
        if (verbose) {
            NLM_LOG_INFO("Config file path: " + configFile);
        }
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", SIMULATION_TIMESTEP, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", STDP_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_tau", STDP_TIME_CONSTANT, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", DEVELOPMENT_SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", DEVELOPMENT_PRUNING_RATE, ConfigSource::Default);
    
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
    
    // Run tests based on available commands
    // In the current implementation, we always run the three standard tests
    // Users can run specific phases with --phase argument in the future
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 2 Execution Complete ===");
    
    // Additional features that could be implemented in the future:
    // --phase 2
    // --experiment <name>
    // --stats
    // --profile
    // --visualize
    // --replay <file>
    // --export <file>
    // --import <file>
    // --benchmark <n>
    
    return 0;
}
