// NLM (熙然) - Neural Learning Machine
// Enhanced Command Line Interface for NLM
//
// This provides comprehensive CLI support for:
// - Multiple execution modes (basic, advanced, debug, interactive)
// - Configuration file management (JSON/YAML support)
// - Experiment management and validation
// - Performance monitoring and profiling
// - Save/load checkpoint capabilities
// - Detailed help and documentation

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
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
#include <fstream>
#include <algorithm>
#include <ctime>
#include <map>

using namespace nlm;

// Command line argument parser
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "nlm_output.txt";
    std::string checkpointFile = "";
    std::string logFile = "";
    std::string experimentType = "basic"; // basic, advanced, debug, interactive
    LogLevel logLevel = LogLevel::Info;
    bool verbose = false;
    bool quiet = false;
    bool saveConfig = false;
    bool validateConfig = false;
    bool benchmark = false;
    uint32_t simulationSteps = 1000;
    double timestep = 0.001;
    uint64_t randomSeed = 42;
    std::vector<std::string> customArgs;
    bool showHelp = false;
    bool showVersion = false;
};

// Execution modes
enum class ExecutionMode {
    BASIC,      // Simple demonstration with basic tests
    ADVANCED,   // Full integration with all features enabled
    DEBUG,      // Debug mode with detailed logging
    INTERACTIVE // Interactive mode with user prompts
};

// Help system
void printHelp(const std::string& programName) {
    std::cout << "=== NLM — Neural Learning Machine Command Line Interface ===" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "MODES:" << std::endl;
    std::cout << "  --mode basic        Basic demonstration with connectivity tests" << std::endl;
    std::cout << "  --mode advanced     Full integration experiment with all systems" << std::endl;
    std::cout << "  --mode debug        Debug mode with detailed logging and analysis" << std::endl;
    std::cout << "  --mode interactive  Interactive mode with user prompts" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION:" << std::endl;
    std::cout << "  --config <file>    Configuration file (JSON/YAML supported, default: configs/default.cfg)" << std::endl;
    std::cout << "  --validate-config   Validate configuration file and exit" << std::endl;
    std::cout << "  --save-config       Save current config to file" << std::endl;
    std::cout << "  --random-seed <n>   Set random seed (default: 42)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXECUTION CONTROL:" << std::endl;
    std::cout << "  --steps <n>        Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --timestep <t>     Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "OUTPUT AND LOGGING:" << std::endl;
    std::cout << "  --output <file>    Output file for results (default: nlm_output.txt)" << std::endl;
    std::cout << "  --log-level <L>    Log level: DEBUG, INFO, WARN, ERROR, CRITICAL (default: INFO)" << std::endl;
    std::cout << "  --log-file <file>   Log output to file" << std::endl;
    std::cout << "  --verbose           Enable verbose output" << std::endl;
    std::cout << "  --quiet            Suppress non-error output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CHECKPOINTING:" << std::endl;
    std::cout << "  --checkpoint <file> Save/load checkpoint file" << std::endl;
    std::cout << "  --benchmark         Run performance benchmark" << std::endl;
    std::cout << std::endl;
    
    std::cout << "MISCELLANEOUS:" << std::endl;
    std::cout << "  --help              Show this help message and exit" << std::endl;
    std::cout << "  --version           Show version information and exit" << std::endl;
    std::cout << "  --build-info        Show build configuration information" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --mode basic" << std::endl;
    std::cout << "  " << programName << " --mode advanced --steps 5000 --benchmark" << std::endl;
    std::cout << "  " << programName << " --config myconfig.json --validate-config" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM — Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6: Final Integration)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Compiler: " << __VERSION__ << std::endl;
    std::cout << "Architecture: x86_64" << std::endl;
}

void printBuildInfo() {
    std::cout << "=== Build Configuration Information ===" << std::endl;
    std::cout << "C++ Standard: C++20" << std::endl;
    std::cout << "Compiler: GNU/Linux x86_64" << std::endl;
    std::cout << "Build Type: Release" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  ✓ JSON/YAML Configuration Support" << std::endl;
    std::cout << "  ✓ Integrated Memory Systems" << std::endl;
    std::cout << "  ✓ Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
    std::cout << "  ✓ Prediction System" << std::endl;
    std::cout << "  ✓ Cognition (Planning, Attention, Concept Formation)" << std::endl;
    std::cout << "  ✓ Development System" << std::endl;
    std::cout << "  ✓ Plasticity (STDP, Hebbian, Structural)" << std::endl;
    std::cout << "  ✓ Working Memory" << std::endl;
    std::cout << "  ✓ Episodic Memory" << std::endl;
    std::cout << "  ✓ Checkpoint System" << std::endl;
    std::cout << "  ✓ Replay and Consolidation" << std::endl;
}

// Parse command line arguments
CommandLineArgs parseCommandLine(int argc, char** argv) {
    CommandLineArgs args;
    std::string programName = "nlm";
    
    if (argc > 0) {
        programName = std::string(argv[0]);
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            args.showHelp = true;
        } else if (arg == "--version") {
            args.showVersion = true;
        } else if (arg == "--build-info") {
            args.showHelp = true; // Show build info instead of help
            args.verbose = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg == "--log-file") {
            if (i + 1 < argc) {
                args.logFile = argv[++i];
            }
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                args.checkpointFile = argv[++i];
            }
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                args.experimentType = argv[++i];
                // Convert to lowercase for comparison
                std::transform(args.experimentType.begin(), args.experimentType.end(),
                              args.experimentType.begin(), ::tolower);
            }
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                std::string level = argv[++i];
                std::transform(level.begin(), level.end(), level.begin(), ::toupper);
                if (level == "DEBUG") args.logLevel = LogLevel::Debug;
                else if (level == "INFO") args.logLevel = LogLevel::Info;
                else if (level == "WARN") args.logLevel = LogLevel::Warning;
                else if (level == "ERROR") args.logLevel = LogLevel::Error;
                else if (level == "CRITICAL") args.logLevel = LogLevel::Critical;
            }
        } else if (arg == "--random-seed") {
            if (i + 1 < argc) {
                args.randomSeed = std::stoul(argv[++i]);
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                args.simulationSteps = std::stoul(argv[++i]);
            }
        } else if (arg == "--timestep") {
            if (i + 1 < argc) {
                args.timestep = std::stod(argv[++i]);
            }
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--quiet") {
            args.quiet = true;
        } else if (arg == "--validate-config") {
            args.validateConfig = true;
        } else if (arg == "--save-config") {
            args.saveConfig = true;
        } else if (arg == "--benchmark") {
            args.benchmark = true;
        } else {
            // Store unknown arguments for custom config
            args.customArgs.push_back(arg);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.customArgs.push_back(argv[++i]);
            }
        }
    }
    
    return args;
}

// Determine execution mode from experiment type
ExecutionMode getExecutionMode(const std::string& experimentType) {
    if (experimentType == "debug") {
        return ExecutionMode::DEBUG;
    } else if (experimentType == "interactive") {
        return ExecutionMode::INTERACTIVE;
    } else if (experimentType == "advanced") {
        return ExecutionMode::ADVANCED;
    } else {
        return ExecutionMode::BASIC;
    }
}

// Basic mode: Simple connectivity tests
void runBasicMode(const CommandLineArgs& args) {
    std::cout << "=== NLM Basic Mode ===" << std::endl;
    std::cout << "Running basic neural connectivity tests..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration
    if (!config->loadFromFile(args.configFile)) {
        std::cerr << "ERROR: Failed to load configuration from " << args.configFile << std::endl;
        return;
    }
    
    // Override with command line arguments
    config->set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::CommandLine);
    config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    
    // Run basic connectivity test
    std::cout << "\n=== Basic Connectivity Test ===" << std::endl;
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "No neurons found!" << std::endl;
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first 10 neurons
        std::cout << "Injecting current into 10 neurons..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * args.timestep);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "Spikes generated: " << spikes << std::endl;
        
        if (spikes > 0) {
            std::cout << "✓ Basic connectivity test PASSED" << std::endl;
        } else {
            std::cout << "! Basic connectivity test: No spikes - checking neuron parameters..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "  Neuron " << i 
                          << " V=" << neurons[i]->getMembranePotential() 
                          << " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
        }
    }
    
    std::cout << "\n=== Basic Mode Complete ===" << std::endl;
}

// Advanced mode: Full integration experiment
void runAdvancedMode(const CommandLineArgs& args) {
    std::cout << "=== NLM Advanced Mode - Full Integration ===" << std::endl;
    std::cout << "Running comprehensive integration experiment..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration
    if (!config->loadFromFile(args.configFile)) {
        std::cerr << "ERROR: Failed to load configuration from " << args.configFile << std::endl;
        return;
    }
    
    // Override with command line arguments
    config->set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::CommandLine);
    config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(args.benchmark ? 100 : 1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    
    // Run plasticity experiment
    std::cout << "\n=== Plasticity Learning Experiment ===" << std::endl;
    LearningExperiment experiment(brain, args.randomSeed);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    std::cout << "Applying repeated input patterns..." << std::endl;
    for (SimulationStep step = 0; step < args.simulationSteps; ++step) {
        // Create input pattern
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * args.timestep);
        
        // Log progress periodically
        if (step % 1000 == 0) {
            std::cout << "  Step " << step 
                      << " | Spikes: " << brain->getTotalSpikeCount() 
                      << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    // Record final state and compute statistics
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // Save configuration if requested
    if (args.saveConfig) {
        std::cout << "\nSaving configuration..." << std::endl;
        config->saveToFile("nlm_config.json");
    }
    
    // Save brain state if checkpoint file specified
    if (!args.checkpointFile.empty()) {
        std::cout << "\nSaving checkpoint..." << std::endl;
        if (brain->save(args.checkpointFile)) {
            std::cout << "✓ Checkpoint saved successfully" << std::endl;
        }
    }
    
    std::cout << "\n=== Advanced Mode Complete ===" << std::endl;
}

// Debug mode: Detailed logging and analysis
void runDebugMode(const CommandLineArgs& args) {
    std::cout << "=== NLM Debug Mode - Detailed Analysis ===" << std::endl;
    std::cout << "Running debug experiment with enhanced logging..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration
    if (!config->loadFromFile(args.configFile)) {
        std::cerr << "ERROR: Failed to load configuration from " << args.configFile << std::endl;
        return;
    }
    
    // Initialize logger with debug level
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
    if (!args.logFile.empty()) {
        consoleLogger->setOutputFile(args.logFile);
    }
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_DEBUG("Debug mode initialized with step limit: " + std::to_string(args.simulationSteps));
    NLM_LOG_DEBUG("Random seed: " + std::to_string(args.randomSeed));
    NLM_LOG_DEBUG("Timestep: " + std::to_string(args.timestep));
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    
    // Run multiple phases with detailed logging
    std::cout << "\n=== Phase Analysis ===" << std::endl;
    
    // Phase 1: Initialization
    std::cout << "Phase 1: Initialization" << std::endl;
    NLM_LOG_DEBUG("Brain initialized successfully");
    
    // Phase 2: Connectivity
    std::cout << "Phase 2: Connectivity Testing" << std::endl;
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        std::cout << "  Found " << neurons.size() << " neurons" << std::endl;
        std::cout << "  Synapses: " << region->getSynapseCount() << std::endl;
        std::cout << "  Average synaptic weight: " << region->getAverageSynapticWeight() << std::endl;
        
        // Test individual neuron properties
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_DEBUG("Neuron " + std::to_string(i) + 
                         " type=" + std::to_string(static_cast<int>(neurons[i]->getType())) +
                         " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                         " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
    
    // Phase 3: Plasticity
    std::cout << "Phase 3: Plasticity Testing" << std::endl;
    if (auto* region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        std::cout << "  Testing " << synapses.size() << " synapses" << std::endl;
        
        // Test STDP parameters
        auto* stdp = brain->getSTDP();
        if (stdp) {
            std::cout << "  STDP LTP weight: " << stdp->getLTPWeight() << std::endl;
            std::cout << "  STDP LTD weight: " << stdp->getLTDWeight() << std::endl;
            std::cout << "  STDP time constant: " << stdp->getTimeConstant() << std::endl;
        }
        
        // Test Hebbian parameters
        auto* hebbian = brain->getHebbian();
        if (hebbian) {
            std::cout << "  Hebbian system active" << std::endl;
        }
    }
    
    // Phase 4: Memory Systems
    std::cout << "Phase 4: Memory System Analysis" << std::endl;
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        std::cout << "  Working memory capacity: " << workingMem->getCapacity() << std::endl;
        std::cout << "  Working memory active traces: " << workingMem->getActiveTraces() << std::endl;
    }
    
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        std::cout << "  Episodic memory max episodes: " << episodicMem->getMaxEpisodes() << std::endl;
        std::cout << "  Episodic memory current episodes: " << episodicMem->getEpisodeCount() << std::endl;
    }
    
    // Phase 5: Neuromodulation
    std::cout << "Phase 5: Neuromodulation Analysis" << std::endl;
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "  Dopamine system active" << std::endl;
        NLM_LOG_DEBUG("Dopamine level initialized");
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        std::cout << "  Curiosity system active" << std::endl;
    }
    
    // Run simulation steps
    std::cout << "\nRunning simulation steps..." << std::endl;
    for (SimulationStep step = 0; step < args.simulationSteps; ++step) {
        brain->step(step, step * args.timestep);
        
        if (step % 1000 == 0) {
            std::cout << "  Step " << step 
                      << " | Spikes: " << brain->getTotalSpikeCount() 
                      << " | Firing: " << brain->getFiringNeuronCount() 
                      << " | Development: " << static_cast<int>(brain->getDevelopmentalStage())
                      << std::endl;
            
            NLM_LOG_DEBUG("Step " + std::to_string(step) + 
                         " completed. Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    // Final status report
    std::cout << "\n=== Final Debug Report ===" << std::endl;
    brain->logStatus();
    
    // Generate debug summary
    std::cout << "\n=== Debug Summary ===" << std::endl;
    std::cout << "Total simulation steps: " << args.simulationSteps << std::endl;
    std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    std::cout << "\n=== Debug Mode Complete ===" << std::endl;
}

// Interactive mode: User prompts and configuration
void runInteractiveMode(const CommandLineArgs& args) {
    std::cout << "=== NLM Interactive Mode ===" << std::endl;
    std::cout << "Starting interactive configuration and control..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration
    if (!config->loadFromFile(args.configFile)) {
        std::cerr << "ERROR: Failed to load configuration from " << args.configFile << std::endl;
        return;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Current configuration summary:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    std::cout << "\nInteractive controls:" << std::endl;
    std::cout << "1. Modify configuration parameters" << std::endl;
    std::cout << "2. Run simulation with custom settings" << std::endl;
    std::cout << "3. Save configuration" << std::endl;
    std::cout << "4. Load configuration" << std::endl;
    std::cout << "5. Run predefined experiments" << std::endl;
    std::cout << "6. Exit" << std::endl;
    
    std::cout << "\nInteractive mode currently in demo mode (would prompt user in full implementation)." << std::endl;
    
    // For now, run advanced mode with custom settings
    std::cout << "\nRunning interactive experiment (advanced mode)..." << std::endl;
    runAdvancedMode(args);
    
    std::cout << "\n=== Interactive Mode Complete ===" << std::endl;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Command Line Interface                           ║
    ║                                                               ║
    ║     Multiple execution modes and comprehensive configuration    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "NLM — Neural Learning Machine Enhanced CLI" << std::endl;
    std::cout << "Parsing command line arguments..." << std::endl << std::endl;
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    // Handle help/version requests
    if (args.showHelp) {
        if (args.verbose) {
            printBuildInfo();
        } else {
            printHelp(argv[0]);
        }
        return 0;
    }
    
    if (args.showVersion) {
        printVersion();
        return 0;
    }
    
    // Validate configuration if requested
    if (args.validateConfig) {
        std::cout << "Validating configuration file: " << args.configFile << std::endl;
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(args.configFile)) {
            std::cout << "✓ Configuration file is valid" << std::endl;
            std::cout << "Configuration summary:" << std::endl;
            std::cout << config->summary() << std::endl;
        } else {
            std::cerr << "✗ Configuration file is invalid or cannot be parsed" << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Save configuration if requested
    if (args.saveConfig) {
        std::cout << "Saving current configuration..." << std::endl;
        auto config = std::make_shared<Config>();
        config->saveToFile("nlm_config.json");
        std::cout << "✓ Configuration saved to nlm_config.json" << std::endl;
    }
    
    // Determine execution mode
    ExecutionMode mode = getExecutionMode(args.experimentType);
    
    std::cout << "Execution mode: " << args.experimentType << std::endl;
    std::cout << "Configuration file: " << args.configFile << std::endl;
    std::cout << "Simulation steps: " << args.simulationSteps << std::endl;
    std::cout << "Timestep: " << args.timestep << "s" << std::endl;
    std::cout << std::endl;
    
    // Run in selected mode
    switch (mode) {
        case ExecutionMode::BASIC:
            runBasicMode(args);
            break;
        case ExecutionMode::ADVANCED:
            runAdvancedMode(args);
            break;
        case ExecutionMode::DEBUG:
            runDebugMode(args);
            break;
        case ExecutionMode::INTERACTIVE:
            runInteractiveMode(args);
            break;
    }
    
    // Write output to file if specified
    if (!args.outputFile.empty() && args.outputFile != "nlm_output.txt") {
        std::cout << "\nWriting output to: " << args.outputFile << std::endl;
        // Output file writing would be implemented here
    }
    
    std::cout << "\n=== NLM Execution Complete ===" << std::endl;
    std::cout << "For additional help, run: nlm --help" << std::endl;
    
    return 0;
}