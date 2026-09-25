// Enhanced NLM Main - Command Line Interface
// 
// This is the improved main.cpp that provides enhanced command-line interface
// with support for testing, configuration management, and demonstration modes
// 
// Command-line options:
// --help: Display help information
// --version: Display version information
// --config <file>: Load configuration from file
// --test <category>: Run tests in specific category
// --benchmark <benchmark>: Run performance benchmarks
// --experiment <name>: Run specific experiments
// --visualize <mode>: Enable visualization mode
// --debug: Enable debug logging
// --verbose: Enable verbose output
// --quiet: Suppress non-error output
// --duration <seconds>: Run simulation for specified duration
// --steps <steps>: Run simulation for specified number of steps
// --neurons <count>: Set number of neurons
// --save <file>: Save configuration to file
// --load <file>: Load configuration from file
// --validate: Validate configuration
// --profiles: Show performance profiles
// --benchmark: Run performance benchmarks
// --compare: Compare with baseline results
// --export: Export results to file
// --import: Import results from file

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
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <thread>
#include <csignal>

using namespace nlm;

// Global variables for signal handling
volatile bool g_running = true;
void signalHandler(int signal) {
    g_running = false;
}

// Command line argument parser
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string testCategory = "";
        std::string benchmark = "";
        std::string experiment = "";
        std::string visualizationMode = "none";
        std::string logLevel = "info";
        bool debug = false;
        bool verbose = false;
        bool quiet = false;
        double duration = 0.0;
        SimulationStep steps = 0;
        int64_t neuronCount = 0;
        std::string saveFile = "";
        std::string loadFile = "";
        bool validate = false;
        bool profiles = false;
        bool compare = false;
        std::string exportFile = "";
        std::string importFile = "";
        std::string outputFormat = "console";
        bool help = false;
        bool version = false;
    };
    
    static Options parse(int argc, char** argv) {
        Options options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version" || arg == "-v") {
                options.version = true;
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg == "--test" && i + 1 < argc) {
                options.testCategory = argv[++i];
            } else if (arg == "--benchmark" && i + 1 < argc) {
                options.benchmark = argv[++i];
            } else if (arg == "--experiment" && i + 1 < argc) {
                options.experiment = argv[++i];
            } else if (arg == "--visualize" && i + 1 < argc) {
                options.visualizationMode = argv[++i];
            } else if (arg == "--debug") {
                options.debug = true;
                options.logLevel = "debug";
            } else if (arg == "--verbose") {
                options.verbose = true;
            } else if (arg == "--quiet") {
                options.quiet = true;
            } else if (arg == "--duration" && i + 1 < argc) {
                options.duration = std::stod(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                options.steps = static_cast<SimulationStep>(std::stoul(argv[++i]));
            } else if (arg == "--neurons" && i + 1 < argc) {
                options.neuronCount = std::stol(argv[++i]);
            } else if (arg == "--save" && i + 1 < argc) {
                options.saveFile = argv[++i];
            } else if (arg == "--load" && i + 1 < argc) {
                options.loadFile = argv[++i];
            } else if (arg == "--validate") {
                options.validate = true;
            } else if (arg == "--profiles") {
                options.profiles = true;
            } else if (arg == "--compare") {
                options.compare = true;
            } else if (arg == "--export" && i + 1 < argc) {
                options.exportFile = argv[++i];
            } else if (arg == "--import" && i + 1 < argc) {
                options.importFile = argv[++i];
            } else if (arg == "--format" && i + 1 < argc) {
                options.outputFormat = argv[++i];
            } else if (arg.substr(0, 2) == "--") {
                std::cout << "Unknown option: " << arg << std::endl;
            }
        }
        
        return options;
    }
    
    static void printHelp() {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: ./nlm [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --version, -v                 Show version information" << std::endl;
        std::cout << "  --config <file>               Load configuration from file" << std::endl;
        std::cout << "  --test <category>             Run tests in specific category" << std::endl;
        std::cout << "  --benchmark <benchmark>        Run performance benchmarks" << std::endl;
        std::cout << "  --experiment <name>           Run specific experiments" << std::endl;
        std::cout << "  --visualize <mode>            Enable visualization mode" << std::endl;
        std::cout << "  --debug                       Enable debug logging" << std::endl;
        std::cout << "  --verbose                     Enable verbose output" << std::endl;
        std::cout << "  --quiet                       Suppress non-error output" << std::endl;
        std::cout << "  --duration <seconds>          Run simulation for specified duration" << std::endl;
        std::cout << "  --steps <steps>               Run simulation for specified number of steps" << std::endl;
        std::cout << "  --neurons <count>             Set number of neurons" << std::endl;
        std::cout << "  --save <file>                 Save configuration to file" << std::endl;
        std::cout << "  --load <file>                 Load configuration from file" << std::endl;
        std::cout << "  --validate                    Validate configuration" << std::endl;
        std::cout << "  --profiles                    Show performance profiles" << std::endl;
        std::cout << "  --compare                     Compare with baseline results" << std::endl;
        std::cout << "  --export <file>               Export results to file" << std::endl;
        std::cout << "  --import <file>               Import results from file" << std::endl;
        std::cout << "  --format <format>             Output format (console, json, xml)" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm --config my_config.cfg --verbose" << std::endl;
        std::cout << "  ./nlm --test config --verbose" << std::endl;
        std::cout << "  ./nlm --benchmark neuron_performance --duration 10.0" << std::endl;
        std::cout << "  ./nlm --experiment demo --save experiment.cfg" << std::endl;
        std::cout << std::endl;
    }
};

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version 0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << std::endl;
}

void printSimulationBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Command Line Interface                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printTestResults(const std::string& category, bool success, 
                     const std::string& details = "") {
    if (success) {
        NLM_LOG_INFO("=== " + category + " Tests PASSED ===");
    } else {
        NLM_LOG_ERROR("=== " + category + " Tests FAILED ===");
        if (!details.empty()) {
            NLM_LOG_ERROR(details);
        }
    }
}

void assertEqual(int a, int b, const std::string& message = "") {
    if (a != b) {
        std::string msg = "Assertion failed: " + std::to_string(a) + " != " + std::to_string(b);
        if (!message.empty()) msg += " (" + message + ")";
        throw std::runtime_error(msg);
    }
}

void assertNotEqual(int a, int b, const std::string& message = "") {
    if (a == b) {
        std::string msg = "Assertion failed: " + std::to_string(a) + " == " + std::to_string(b);
        if (!message.empty()) msg += " (" + message + ")";
        throw std::runtime_error(msg);
    }
}

void assertLess(int a, int b, const std::string& message = "") {
    if (a >= b) {
        std::string msg = "Assertion failed: " + std::to_string(a) + " >= " + std::to_string(b);
        if (!message.empty()) msg += " (" + message + ")";
        throw std::runtime_error(msg);
    }
}

void assertGreater(int a, int b, const std::string& message = "") {
    if (a <= b) {
        std::string msg = "Assertion failed: " + std::to_string(a) + " <= " + std::to_string(b);
        if (!message.empty()) msg += " (" + message + ")";
        throw std::runtime_error(msg);
    }
}

void assertTrue(bool condition, const std::string& message = "") {
    if (!condition) {
        std::string msg = "Assertion failed: condition is false";
        if (!message.empty()) msg += " (" + message + ")";
        throw std::runtime_error(msg);
    }
}

void assertFalse(bool condition, const std::string& message = "") {
    if (condition) {
        std::string msg = "Assertion failed: condition is true";
        if (!message.empty()) msg += " (" + message + ")";
        throw std::runtime_error(msg);
    }
}

void assertEqual(double a, double b, double tolerance = 1e-6, const std::string& message = "") {
    if (std::abs(a - b) > tolerance) {
        std::string msg = "Assertion failed: " + std::to_string(a) + " != " + std::to_string(b);
        if (!message.empty()) msg += " (" + message + ")";
        throw std::runtime_error(msg);
    }
}

void runConfigTests(const CommandLineParser::Options& options) {
    NLM_LOG_INFO("=== Configuration Tests ===");
    
    // Test configuration loading
    auto config = std::make_shared<Config>();
    
    // Test basic configuration operations
    config->set("test_int", 42, ConfigSource::Runtime);
    config->set("test_string", std::string("hello"), ConfigSource::Runtime);
    config->set("test_double", 3.14, ConfigSource::Runtime);
    config->set("test_bool", true, ConfigSource::Runtime);
    
    // Test retrieval
    auto intVal = config->get<int>("test_int");
    auto stringVal = config->get<std::string>("test_string");
    auto doubleVal = config->get<double>("test_double");
    auto boolVal = config->get<bool>("test_bool");
    
    assertEqual(intVal.has_value() && *intVal == 42);
    assertEqual(stringVal.has_value() && *stringVal == "hello");
    assertEqual(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
    assertEqual(boolVal.has_value() && *boolVal == true);
    
    // Test getOr
    int defaultVal = config->getOr<int>("missing", 100);
    assertEqual(defaultVal, 100);
    
    // Test configuration file loading
    bool fileLoaded = config->loadFromFile(options.configFile);
    NLM_LOG_INFO("Configuration file loaded: " + std::string(fileLoaded ? "true" : "false"));
    
    if (fileLoaded) {
        NLM_LOG_INFO("Configuration keys: ");
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            NLM_LOG_INFO("  - " + key);
        }
    }
    
    // Test configuration saving
    bool saved = config->saveToFile("test_output.cfg");
    NLM_LOG_INFO("Configuration saved: " + std::string(saved ? "true" : "false"));
    
    printTestResults("Configuration", true, "All configuration tests passed");
}

void runNeuronTests(const CommandLineParser::Options& options) {
    NLM_LOG_INFO("=== Neuron Tests ===");
    
    // Test neuron creation
    Neuron neuron(NeuronId(1));
    assert(neuron.getId() == NeuronId(1));
    assert(neuron.getType() == NeuronType::Internal);
    
    // Test neuron type changes
    neuron.setType(NeuronType::Excitatory);
    assert(neuron.getType() == NeuronType::Excitatory);
    
    neuron.setType(NeuronType::Inhibitory);
    assert(neuron.getType() == NeuronType::Inhibitory);
    
    // Test membrane potential
    neuron.setMembranePotential(-70.0f);
    assert(neuron.getMembranePotential() == -70.0f);
    
    neuron.addToMembranePotential(10.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    // Test threshold
    neuron.setThreshold(-55.0f);
    assert(neuron.getThreshold() == -55.0f);
    
    // Test firing state
    assertFalse(neuron.isFiring());
    assertFalse(neuron.isRefractory());
    
    neuron.setFiringState(FiringState::Active);
    assert(neuron.getState().firingState == FiringState::Active);
    
    // Test refractory period
    neuron.setRefractoryPeriod(5);
    assert(neuron.getRefractoryPeriod() == 5);
    
    // Test spike history
    neuron.recordSpike(1.0f);
    const auto& spikeHistory = neuron.getSpikeHistory();
    assertEqual(spikeHistory.size(), 1);
    assertEqual(spikeHistory[0], 1.0f);
    
    neuron.clearSpikeHistory();
    assertEqual(neuron.getSpikeHistory().size(), 0);
    
    printTestResults("Neuron", true, "All neuron tests passed");
}

void runBrainTests(const CommandLineParser::Options& options) {
    NLM_LOG_INFO("=== Brain Tests ===");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1, ConfigSource::Default);
    
    // Create brain
    Brain brain(config);
    
    // Test initialization
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assertEqual(brain.getRegionCount(), 2);
    assertEqual(brain.getTotalNeuronCount(), 100);
    assertGreater(brain.getTotalSynapseCount(), 0);
    
    // Test brain step
    brain.step(0);
    assertEqual(brain.getTotalNeuronCount(), 100);
    
    // Test brain reset
    brain.reset();
    assertEqual(brain.getTotalNeuronCount(), 100);
    
    // Test re-initialization
    success = brain.initialize();
    assert(success);
    
    printTestResults("Brain", true, "All brain tests passed");
}

void runMainSimulation(const CommandLineParser::Options& options) {
    NLM_LOG_INFO("=== NLM Phase 2 Main Simulation ===");
    
    // Print banner
    std::cout << std::endl;
    printSimulationBanner();
    std::cout << std::endl;
    
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
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = options.configFile;
    bool configLoaded = config->loadFromFile(configFile);
    
    if (configLoaded) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(0, nullptr);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
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
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Determine simulation duration
    SimulationStep duration = options.steps;
    double timeDuration = options.duration;
    
    if (duration == 0 && timeDuration > 0) {
        duration = static_cast<SimulationStep>(timeDuration / timestep);
    } else if (duration == 0) {
        duration = 100; // Default duration
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Running simulation for " + std::to_string(duration) + " steps...");
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (SimulationStep step = 0; step < duration && g_running; ++step) {
        brain->step(step);
        
        // Log progress
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Time: " + std::to_string(clock.getCurrentTime()) + "s" +
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record end time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Simulation completed in " + std::to_string(durationMs) + "ms");
    NLM_LOG_INFO("Total steps: " + std::to_string(duration));
    NLM_LOG_INFO("Steps per second: " + std::to_string(duration * 1000.0 / durationMs));
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Save configuration if requested
    if (!options.saveFile.empty()) {
        NLM_LOG_INFO("Saving configuration to: " + options.saveFile);
        config->saveToFile(options.saveFile);
    }
    
    // Export results if requested
    if (!options.exportFile.empty()) {
        NLM_LOG_INFO("Exporting results to: " + options.exportFile);
        // TODO: Implement result export functionality
        NLM_LOG_INFO("Export functionality to be implemented");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
}

void runPhase6Demo(const CommandLineParser::Options& options) {
    NLM_LOG_INFO("=== NLM Phase 6 Demo (Integration Test) ===");
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    std::cout << "=== Phase 6 Demo Complete ===" << std::endl;
}

void printPerformanceProfiles(const CommandLineParser::Options& options) {
    NLM_LOG_INFO("=== Performance Profiles ===");
    
    // Collect performance metrics
    NLM_LOG_INFO("Performance metrics will be collected and displayed here.");
    NLM_LOG_INFO("This feature provides detailed performance analysis of neural computation.");
    
    // Placeholder for performance profiling
    std::cout << "Performance profiling is currently implemented in the test framework.";
    std::cout << std::endl;
    std::cout << "Run './nlm_test_suite --benchmark neuron_performance' for detailed benchmarks.";
    std::cout << std::endl;
}

void compareResults(const CommandLineParser::Options& options) {
    NLM_LOG_INFO("=== Results Comparison ===");
    
    // Compare with baseline results
    NLM_LOG_INFO("Comparing with baseline results...");
    
    // Placeholder for comparison functionality
    std::cout << "Results comparison is currently implemented in the test framework.";
    std::cout << std::endl;
    std::cout << "Run './nlm_test_suite --compare baseline.json' for detailed comparison.";
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    // Set signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Parse command line arguments
    CommandLineParser::Options options = CommandLineParser::parse(argc, argv);
    
    // Handle help and version requests
    if (options.help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    if (options.version) {
        printVersion();
        return 0;
    }
    
    // Set up logging based on options
    auto logger = std::make_shared<Logger>();
    
    LogLevel logLevel;
    if (options.logLevel == "debug") {
        logLevel = LogLevel::Debug;
    } else if (options.logLevel == "warning") {
        logLevel = LogLevel::Warning;
    } else if (options.logLevel == "error") {
        logLevel = LogLevel::Error;
    } else if (options.logLevel == "critical") {
        logLevel = LogLevel::Critical;
    } else {
        logLevel = LogLevel::Info;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    consoleLogger->setUseColors(!options.quiet);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Override log level from options
    if (options.debug) {
        logger->setLevel(LogLevel::Debug);
    } else if (options.verbose) {
        logger->setLevel(LogLevel::Info);
    }
    
    try {
        // Run requested functionality based on command line options
        if (!options.testCategory.empty()) {
            NLM_LOG_INFO("Running tests in category: " + options.testCategory);
            
            if (options.testCategory == "config") {
                runConfigTests(options);
            } else if (options.testCategory == "neuron") {
                runNeuronTests(options);
            } else if (options.testCategory == "brain") {
                runBrainTests(options);
            } else if (options.testCategory == "all") {
                runConfigTests(options);
                runNeuronTests(options);
                runBrainTests(options);
            } else {
                NLM_LOG_WARNING("Unknown test category: " + options.testCategory);
                NLM_LOG_WARNING("Available categories: config, neuron, brain, all");
            }
        } else if (!options.benchmark.empty()) {
            NLM_LOG_INFO("Running benchmark: " + options.benchmark);
            printPerformanceProfiles(options);
        } else if (!options.experiment.empty()) {
            NLM_LOG_INFO("Running experiment: " + options.experiment);
            
            if (options.experiment == "demo") {
                runPhase6Demo(options);
            } else {
                NLM_LOG_WARNING("Unknown experiment: " + options.experiment);
                NLM_LOG_WARNING("Available experiments: demo");
            }
        } else {
            // Default: run main simulation
            runMainSimulation(options);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error occurred");
        return 1;
    }
    
    // Compare results if requested
    if (options.compare) {
        compareResults(options);
    }
    
    return 0;
}
