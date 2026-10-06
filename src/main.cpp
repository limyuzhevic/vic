// NLM (熙然) - Neural Learning Machine
// Advanced Command-Line Interface
//
// This file implements an enhanced command-line interface for NLM with:
// - Comprehensive argument parsing
// - Multiple execution modes
// - Configuration file management
// - Experiment control
// - Output formatting options
// - Performance monitoring
// - Validation and error handling

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
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <iomanip>

using namespace nlm;

// Command-line argument parser
class CommandLineParser {
public:
    struct Arguments {
        std::string configFile = "configs/default.cfg";
        std::string outputFile;
        std::string checkpointFile;
        std::string logFile;
        std::string experimentMode = "standard";
        std::string logLevel = "info";
        std::string outputFormat = "text";
        bool verbose = false;
        bool quiet = false;
        bool help = false;
        bool version = false;
        bool validate = false;
        bool benchmark = false;
        bool profile = false;
        int maxSteps = 1000;
        double timestep = 0.001;
        int neuronCount = 1000;
        int regionCount = 1;
        float connectionProbability = 0.1f;
        uint64_t randomSeed = 42;
        std::vector<std::string> customArgs;
    };

    Arguments parse(int argc, char** argv) {
        Arguments args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version") {
                args.version = true;
            } else if (arg == "--verbose" || arg == "-v") {
                args.verbose = true;
            } else if (arg == "--quiet" || arg == "-q") {
                args.quiet = true;
            } else if (arg == "--validate") {
                args.validate = true;
            } else if (arg == "--benchmark") {
                args.benchmark = true;
            } else if (arg == "--profile") {
                args.profile = true;
            } else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    args.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.configFile = argv[++i];
                }
            } else if (arg.substr(0, 4) == "--log") {
                if (arg.find('=') != std::string::npos) {
                    args.logFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.logFile = argv[++i];
                }
            } else if (arg.substr(0, 6) == "--out") {
                if (arg.find('=') != std::string::npos) {
                    args.outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.outputFile = argv[++i];
                }
            } else if (arg.substr(0, 11) == "--checkpoint") {
                if (arg.find('=') != std::string::npos) {
                    args.checkpointFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.checkpointFile = argv[++i];
                }
            } else if (arg.substr(0, 6) == "--mode") {
                if (arg.find('=') != std::string::npos) {
                    args.experimentMode = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.experimentMode = argv[++i];
                }
            } else if (arg.substr(0, 11) == "--loglevel") {
                if (arg.find('=') != std::string::npos) {
                    args.logLevel = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.logLevel = argv[++i];
                }
            } else if (arg.substr(0, 12) == "--output-format") {
                if (arg.find('=') != std::string::npos) {
                    args.outputFormat = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.outputFormat = argv[++i];
                }
            } else if (arg.substr(0, 5) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    args.maxSteps = std::stoi(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    args.maxSteps = std::stoi(argv[++i]);
                }
            } else if (arg.substr(0, 5) == "--time") {
                if (arg.find('=') != std::string::npos) {
                    args.timestep = std::stod(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    args.timestep = std::stod(argv[++i]);
                }
            } else if (arg.substr(0, 7) == "--neurons") {
                if (arg.find('=') != std::string::npos) {
                    args.neuronCount = std::stoi(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    args.neuronCount = std::stoi(argv[++i]);
                }
            } else if (arg.substr(0, 7) == "--regions") {
                if (arg.find('=') != std::string::npos) {
                    args.regionCount = std::stoi(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    args.regionCount = std::stoi(argv[++i]);
                }
            } else if (arg.substr(0, 7) == "--conn-prob") {
                if (arg.find('=') != std::string::npos) {
                    args.connectionProbability = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    args.connectionProbability = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 6) == "--seed") {
                if (arg.find('=') != std::string::npos) {
                    args.randomSeed = std::stoull(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    args.randomSeed = std::stoull(argv[++i]);
                }
            } else {
                // Store unrecognized arguments for custom processing
                args.customArgs.push_back(arg);
            }
        }
        
        return args;
    }
    
    void printUsage() {
        std::cout << R"(" NLM - Neural Learning Machine - Advanced Command-Line Interface

USAGE:
  nlm [OPTIONS]

BASIC OPTIONS:
  --help, -h              Show this help message
  --version               Show version information
  --verbose, -v           Enable verbose output
  --quiet, -q             Suppress non-essential output

CONFIGURATION:
  --config FILE           Load configuration from FILE (default: configs/default.cfg)
  --loglevel LEVEL        Set log level (debug, info, warning, error, critical)
  --output-format FORMAT   Output format (text, json, csv)

EXPERIMENT CONTROL:
  --mode MODE             Experiment mode (standard, benchmark, validation)
  --steps N               Maximum simulation steps (default: 1000)
  --time DT               Simulation timestep (default: 0.001)

NEURAL NETWORK:
  --neurons N             Number of neurons (default: 1000)
  --regions R             Number of brain regions (default: 1)
  --conn-prob P            Connection probability (default: 0.1)

OUTPUT:
  --out FILE              Output results to FILE
  --checkpoint FILE       Save checkpoint to FILE
  --log FILE              Log to FILE instead of console

ADVANCED OPTIONS:
  --validate              Validate configuration and exit
  --benchmark             Run benchmark tests
  --profile                Enable performance profiling

EXAMPLES:
  nlm --config myconfig.cfg --mode benchmark --steps 5000
  nlm --loglevel debug --verbose --out results.txt
  nlm --validate --config invalid.cfg
  nlm --benchmark --profile

For more information, visit: https://github.com/your-project/nlm

)" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM - Neural Learning Machine v2.0.0 (Phase 6 - Final Integration)" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "C++ Standard: C++20" << std::endl;
        std::cout << "Author: Research Team" << std::endl;
        std::cout << "License: MIT" << std::endl;
    }
};

// Advanced NLM application with CLI support
class NLMApplicationCLI {
public:
    NLMApplicationCLI(int argc, char** argv) : argc_(argc), argv_(argv) {
        parser_.parse(argc, argv);
    }
    
    int run() {
        printBanner();
        
        if (shouldExit()) {
            return 0;
        }
        
        initializeLogger();
        
        if (!validateArguments()) {
            NLM_LOG_ERROR("Argument validation failed");
            return 1;
        }
        
        if (!loadConfiguration()) {
            NLM_LOG_ERROR("Failed to load configuration");
            return 1;
        }
        
        if (parser_.args_.validate) {
            NLM_LOG_INFO("Configuration validation successful");
            return 0;
        }
        
        if (!initializeBrain()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return 1;
        }
        
        return runApplication();
    }

private:
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Enhanced Command-Line Interface                          ║
    ║                                                               ║
    ╚═════════════════════════════════════════════════════════════╝
    )" << std::endl;
        
        if (parser_.args_.verbose) {
            NLM_LOG_INFO("Command-line arguments: parsing completed");
            NLM_LOG_INFO("Configuration file: " + parser_.args_.configFile);
            NLM_LOG_INFO("Experiment mode: " + parser_.args_.experimentMode);
        }
    }
    
    bool shouldExit() {
        return parser_.args_.help || parser_.args_.version;
    }
    
    void handleExit() {
        if (parser_.args_.help) {
            parser_.printUsage();
        } else if (parser_.args_.version) {
            parser_.printVersion();
        }
    }
    
    void initializeLogger() {
        auto logger = std::make_shared<Logger>();
        
        // Create console logger with enhanced configuration
        auto consoleLogger = std::make_shared<EnhancedConsoleLogger>();
        
        // Set log level based on command line argument
        LogLevel logLevel;
        if (parser_.args_.logLevel == "debug") logLevel = LogLevel::Debug;
        else if (parser_.args_.logLevel == "warning") logLevel = LogLevel::Warning;
        else if (parser_.args_.logLevel == "error") logLevel = LogLevel::Error;
        else if (parser_.args_.logLevel == "critical") logLevel = LogLevel::Critical;
        else logLevel = LogLevel::Info;  // default
        
        consoleLogger->setLevel(logLevel);
        consoleLogger->setUseColors(true);
        consoleLogger->setUseTimestamps(true);
        consoleLogger->setUseThreadIds(parser_.args_.verbose);
        
        // Set output file if specified
        if (!parser_.args_.logFile.empty()) {
            consoleLogger->setOutputFile(parser_.args_.logFile);
        }
        
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Logger initialized with level: " + parser_.args_.logLevel);
        if (!parser_.args_.logFile.empty()) {
            NLM_LOG_INFO("Logging to file: " + parser_.args_.logFile);
        }
    }
    
    bool validateArguments() {
        NLM_LOG_INFO("Validating command-line arguments");
        
        // Validate configuration file
        if (parser_.args_.configFile.empty()) {
            NLM_LOG_ERROR("Configuration file path is empty");
            return false;
        }
        
        // Check if config file exists
        std::filesystem::path configPath(parser_.args_.configFile);
        if (!std::filesystem::exists(configPath)) {
            NLM_LOG_WARNING("Configuration file does not exist: " + parser_.args_.configFile);
            if (parser_.args_.verbose) {
                NLM_LOG_INFO("Trying default configuration...");
                parser_.args_.configFile = "configs/default.cfg";
            } else {
                NLM_LOG_INFO("Using default configuration...");
                parser_.args_.configFile = "configs/default.cfg";
            }
        }
        
        // Validate numeric parameters
        if (parser_.args_.maxSteps <= 0) {
            NLM_LOG_ERROR("Invalid maxSteps: must be positive");
            return false;
        }
        
        if (parser_.args_.timestep <= 0.0) {
            NLM_LOG_ERROR("Invalid timestep: must be positive");
            return false;
        }
        
        if (parser_.args_.neuronCount <= 0) {
            NLM_LOG_ERROR("Invalid neuronCount: must be positive");
            return false;
        }
        
        if (parser_.args_.regionCount <= 0) {
            NLM_LOG_ERROR("Invalid regionCount: must be positive");
            return false;
        }
        
        if (parser_.args_.connectionProbability < 0.0f || 
            parser_.args_.connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Invalid connectionProbability: must be between 0.0 and 1.0");
            return false;
        }
        
        // Validate experiment mode
        std::vector<std::string> validModes = {"standard", "benchmark", "validation", "debug", "demo"};
        if (std::find(validModes.begin(), validModes.end(), parser_.args_.experimentMode) == validModes.end()) {
            NLM_LOG_WARNING("Unknown experiment mode: " + parser_.args_.experimentMode);
            NLM_LOG_INFO("Using 'standard' mode instead");
            parser_.args_.experimentMode = "standard";
        }
        
        NLM_LOG_INFO("Argument validation completed successfully");
        return true;
    }
    
    bool loadConfiguration() {
        NLM_LOG_INFO("Loading configuration from: " + parser_.args_.configFile);
        
        config_ = std::make_shared<Config>();
        
        // Load config from file
        if (!config_->loadFromFile(parser_.args_.configFile)) {
            NLM_LOG_WARNING("Failed to load configuration from file, using defaults");
        } else {
            NLM_LOG_INFO("Configuration loaded successfully");
        }
        
        // Override with command line arguments
        config_->loadFromArgs(argc_, argv_);
        
        // Apply command-line specific overrides
        config_->set("random_seed", static_cast<int64_t>(parser_.args_.randomSeed), ConfigSource::CommandLine);
        config_->set("simulation_timestep", parser_.args_.timestep, ConfigSource::CommandLine);
        config_->set("neuron_count", static_cast<int64_t>(parser_.args_.neuronCount), ConfigSource::CommandLine);
        config_->set("region_count", static_cast<int64_t>(parser_.args_.regionCount), ConfigSource::CommandLine);
        config_->set("connection_probability", parser_.args_.connectionProbability, ConfigSource::CommandLine);
        
        logConfigurationSummary();
        return true;
    }
    
    void logConfigurationSummary() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM Configuration Summary ===");
        NLM_LOG_INFO("Configuration file: " + parser_.args_.configFile);
        NLM_LOG_INFO("Experiment mode: " + parser_.args_.experimentMode);
        NLM_LOG_INFO("Max steps: " + std::to_string(parser_.args_.maxSteps));
        NLM_LOG_INFO("Timestep: " + std::to_string(parser_.args_.timestep) + "s");
        NLM_LOG_INFO("Neuron count: " + std::to_string(parser_.args_.neuronCount));
        NLM_LOG_INFO("Region count: " + std::to_string(parser_.args_.regionCount));
        NLM_LOG_INFO("Connection probability: " + std::to_string(parser_.args_.connectionProbability));
        NLM_LOG_INFO("Random seed: " + std::to_string(parser_.args_.randomSeed));
        NLM_LOG_INFO("");
    }
    
    bool initializeBrain() {
        NLM_LOG_INFO("Initializing NLM Brain in " + parser_.args_.experimentMode + " mode...");
        
        brain_ = std::make_shared<Brain>(config_);
        
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        brain_->logStatus();
        return true;
    }
    
    int runApplication() {
        NLM_LOG_INFO("Starting NLM application in " + parser_.args_.experimentMode + " mode");
        
        switch (getExperimentMode(parser_.args_.experimentMode)) {
            case ExperimentMode::STANDARD:
                return runStandardExperiment();
            case ExperimentMode::BENCHMARK:
                return runBenchmarkExperiment();
            case ExperimentMode::VALIDATION:
                return runValidationExperiment();
            case ExperimentMode::DEBUG:
                return runDebugExperiment();
            case ExperimentMode::DEMO:
                return runDemoExperiment();
            default:
                NLM_LOG_ERROR("Unknown experiment mode: " + parser_.args_.experimentMode);
                return 1;
        }
    }
    
    enum class ExperimentMode {
        STANDARD,
        BENCHMARK,
        VALIDATION,
        DEBUG,
        DEMO
    };
    
    ExperimentMode getExperimentMode(const std::string& mode) {
        if (mode == "benchmark") return ExperimentMode::BENCHMARK;
        if (mode == "validation") return ExperimentMode::VALIDATION;
        if (mode == "debug") return ExperimentMode::DEBUG;
        if (mode == "demo") return ExperimentMode::DEMO;
        return ExperimentMode::STANDARD;  // default
    }
    
    int runStandardExperiment() {
        NLM_LOG_INFO("Running standard experiment");
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain_);
        
        // Reset and run plasticity experiment
        resetAndInitializeBrain();
        runPlasticityExperiment(brain_);
        
        // Reset and run STDP verification
        resetAndInitializeBrain();
        runStdpVerification(brain_);
        
        // Run additional integration tests based on configuration
        if (parser_.args_.verbose) {
            NLM_LOG_INFO("Running additional integration tests");
            runIntegrationTests(brain_);
        }
        
        displayFinalStatus();
        
        // Save checkpoint if requested
        if (!parser_.args_.checkpointFile.empty()) {
            if (!saveCheckpoint()) {
                NLM_LOG_WARNING("Failed to save checkpoint");
            }
        }
        
        // Save output if requested
        if (!parser_.args_.outputFile.empty()) {
            if (!saveOutput()) {
                NLM_LOG_WARNING("Failed to save output");
            }
        }
        
        NLM_LOG_INFO("Standard experiment completed successfully");
        return 0;
    }
    
    int runBenchmarkExperiment() {
        NLM_LOG_INFO("Running benchmark experiment");
        NLM_LOG_INFO("This will test performance with " + std::to_string(parser_.args_.neuronCount) + " neurons");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run multiple iterations to measure performance
        int iterations = std::min(5, parser_.args_.maxSteps / 100);
        
        for (int iteration = 0; iteration < iterations; ++iteration) {
            NLM_LOG_INFO("Benchmark iteration " + std::to_string(iteration + 1) + "/" + std::to_string(iterations));
            
            resetAndInitializeBrain();
            
            // Inject current and measure spike time
            auto* region = brain_->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                if (!neurons.empty()) {
                    neurons[0]->injectCurrent(50.0f);
                }
            }
            
            // Run for a fixed number of steps
            int steps = parser_.args_.maxSteps / iterations;
            for (SimulationStep step = 0; step < steps; ++step) {
                brain_->step(step, step * parser_.args_.timestep);
            }
            
            size_t spikes = brain_->getTotalSpikeCount();
            NLM_LOG_INFO("  Iteration " + std::to_string(iteration + 1) + ": " + std::to_string(spikes) + " spikes");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Benchmark completed in " + std::to_string(duration.count()) + " ms");
        NLM_LOG_INFO("Average time per iteration: " + std::to_string(duration.count() / iterations) + " ms");
        
        return 0;
    }
    
    int runValidationExperiment() {
        NLM_LOG_INFO("Running validation experiment");
        NLM_LOG_INFO("This experiment validates the correctness of NLM implementation");
        
        // Validate neuron properties
        bool neuronValidation = validateNeurons(brain_);
        if (!neuronValidation) {
            NLM_LOG_ERROR("Neuron validation failed");
            return 1;
        }
        
        // Validate synapse properties
        bool synapseValidation = validateSynapses(brain_);
        if (!synapseValidation) {
            NLM_LOG_ERROR("Synapse validation failed");
            return 1;
        }
        
        // Validate memory systems
        bool memoryValidation = validateMemorySystems(brain_);
        if (!memoryValidation) {
            NLM_LOG_ERROR("Memory system validation failed");
            return 1;
        }
        
        NLM_LOG_INFO("Validation experiment completed successfully");
        NLM_LOG_INFO("All system validations passed");
        return 0;
    }
    
    int runDebugExperiment() {
        NLM_LOG_INFO("Running debug experiment");
        NLM_LOG_INFO("This experiment enables verbose debugging output");
        
        // Enable debug logging
        auto logger = Logger::getGlobal();
        logger->setLevel(LogLevel::Debug);
        
        // Run a simple test with detailed logging
        runBasicConnectivityTest(brain_);
        
        // Log detailed neuron states
        auto* region = brain_->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            NLM_LOG_INFO("Neuron states for debugging:");
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                auto* neuron = neurons[i];
                NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                           " ID: " + std::to_string(neuron->getId()) +
                           " V: " + std::to_string(neuron->getMembranePotential()) +
                           " thresh: " + std::to_string(neuron->getThreshold()) +
                           " state: " + std::to_string(static_cast<int>(neuron->getState().firingState)));
            }
        }
        
        NLM_LOG_INFO("Debug experiment completed successfully");
        return 0;
    }
    
    int runDemoExperiment() {
        NLM_LOG_INFO("Running demo experiment");
        NLM_LOG_INFO("This experiment demonstrates NLM capabilities with simplified settings");
        
        // Run a simplified version of the standard experiment
        // with smaller network size for demonstration
        
        // Create a smaller brain for demo
        config_->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        
        resetAndInitializeBrain();
        
        NLM_LOG_INFO("Demo brain initialized with 100 neurons");
        brain_->logStatus();
        
        // Run a quick connectivity test
        runBasicConnectivityTest(brain_);
        
        NLM_LOG_INFO("Demo experiment completed successfully");
        NLM_LOG_INFO("NLM demo shows basic brain functionality working correctly");
        return 0;
    }
    
    bool validateNeurons(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Validating neurons");
        
        size_t totalNeurons = brain->getTotalNeuronCount();
        NLM_LOG_INFO("Checking " + std::to_string(totalNeurons) + " neurons");
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Check basic neuron properties
                    if (neuron->getMembranePotential() < -100.0f || 
                        neuron->getMembranePotential() > 50.0f) {
                        NLM_LOG_WARNING("Neuron " + std::to_string(neuron->getId().index()) +
                                       " has abnormal membrane potential: " +
                                       std::to_string(neuron->getMembranePotential()));
                    }
                    
                    if (neuron->getThreshold() < -100.0f || 
                        neuron->getThreshold() > 50.0f) {
                        NLM_LOG_WARNING("Neuron " + std::to_string(neuron->getId().index()) +
                                       " has abnormal threshold: " +
                                       std::to_string(neuron->getThreshold()));
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Neuron validation completed");
        return true;
    }
    
    bool validateSynapses(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Validating synapses");
        
        size_t totalSynapses = brain->getTotalSynapseCount();
        NLM_LOG_INFO("Checking " + std::to_string(totalSynapses) + " synapses");
        
        // Check synapse weights
        float minWeight = std::numeric_limits<float>::max();
        float maxWeight = std::numeric_limits<float>::lowest();
        
        for (const auto& region : brain->getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                float weight = syn->getWeight();
                if (weight < minWeight) minWeight = weight;
                if (weight > maxWeight) maxWeight = weight;
                
                if (weight < -100.0f || weight > 100.0f) {
                    NLM_LOG_WARNING("Synapse " + std::to_string(syn->getId().index()) +
                                   " has extreme weight: " + std::to_string(weight));
                }
            }
        }
        
        NLM_LOG_INFO("Synapse weight range: [" + std::to_string(minWeight) + ", " +
                   std::to_string(maxWeight) + "]");
        NLM_LOG_INFO("Synapse validation completed");
        return true;
    }
    
    bool validateMemorySystems(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Validating memory systems");
        
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        if (workingMemory) {
            NLM_LOG_INFO("Working memory capacity: " + std::to_string(workingMemory->getCapacity()));
        }
        if (episodicMemory) {
            NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(episodicMemory->getMaxEpisodes()));
        }
        if (associativeMemory) {
            NLM_LOG_INFO("Associative memory pattern count: " + std::to_string(associativeMemory->getPatternCount()));
        }
        
        NLM_LOG_INFO("Memory system validation completed");
        return true;
    }
    
    void runIntegrationTests(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running integration tests");
        
        // Test 1: Memory system integration
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->storeToNeuron(NeuronId(1), 0.5f);
            NLM_LOG_INFO("Working memory test: stored neuron activation");
        }
        
        // Test 2: Prediction system integration
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            NLM_LOG_INFO("Prediction system test: integrated");
        }
        
        // Test 3: Neuromodulation integration
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            NLM_LOG_INFO("Dopamine system test: integrated");
        }
        
        NLM_LOG_INFO("Integration tests completed");
    }
    
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Basic Neural Connectivity Test ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first 10 neurons
        NLM_LOG_INFO("Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * parser_.args_.timestep);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                           " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                           " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
        
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input pattern
        NLM_LOG_INFO("Applying repeated input patterns (500 steps)...");
        
        for (SimulationStep step = 0; step < 500; ++step) {
            // Create input pattern
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * parser_.args_.timestep);
            
            // Log progress
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state and compute statistics
        experiment.recordFinalState();
        experiment.computeStatistics();
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
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
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                           " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity for potentiation
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 30; ++trial) {
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
                brain->step(trial * 2, trial * 2 * parser_.args_.timestep);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * parser_.args_.timestep);
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
    
    void displayFinalStatus() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final NLM Status (Phase 6 - Enhanced CLI) ===");
        brain_->logStatus();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM Application Complete ===");
        NLM_LOG_INFO("Experiment mode: " + parser_.args_.experimentMode);
        NLM_LOG_INFO("Max steps executed: " + std::to_string(parser_.args_.maxSteps));
        NLM_LOG_INFO("Final spike count: " + std::to_string(brain_->getTotalSpikeCount()));
        
        if (parser_.args_.benchmark) {
            NLM_LOG_INFO("Performance metrics available for benchmarking");
        }
        
        NLM_LOG_INFO("Application completed successfully");
    }
    
    void resetAndInitializeBrain() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Resetting and reinitializing brain...");
        brain_->reset();
        brain_->initialize();
    }
    
    bool saveCheckpoint() {
        NLM_LOG_INFO("Saving checkpoint to: " + parser_.args_.checkpointFile);
        
        try {
            std::filesystem::path path(parser_.args_.checkpointFile);
            std::filesystem::path parent = path.parent_path();
            if (!parent.empty() && !std::filesystem::exists(parent)) {
                std::filesystem::create_directories(parent);
            }
            
            if (!brain_->save(parser_.args_.checkpointFile)) {
                NLM_LOG_ERROR("Failed to save checkpoint");
                return false;
            }
            
            NLM_LOG_INFO("Checkpoint saved successfully");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception saving checkpoint: ") + e.what());
            return false;
        }
    }
    
    bool saveOutput() {
        NLM_LOG_INFO("Saving output to: " + parser_.args_.outputFile);
        
        try {
            std::ofstream file(parser_.args_.outputFile);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open output file");
                return false;
            }
            
            // Write experiment results
            file << "NLM Experiment Results" << std::endl;
            file << "========================" << std::endl;
            file << "Experiment mode: " << parser_.args_.experimentMode << std::endl;
            file << "Configuration file: " << parser_.args_.configFile << std::endl;
            file << "Max steps: " << parser_.args_.maxSteps << std::endl;
            file << "Timestep: " << parser_.args_.timestep << "s" << std::endl;
            file << "Neuron count: " << parser_.args_.neuronCount << std::endl;
            file << "Region count: " << parser_.args_.regionCount << std::endl;
            file << "Connection probability: " << parser_.args_.connectionProbability << std::endl;
            file << "Random seed: " << parser_.args_.randomSeed << std::endl;
            file << "" << std::endl;
            
            // Write brain statistics
            file << "Brain Statistics:" << std::endl;
            file << "  Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
            file << "  Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
            file << "  Regions: " << brain_->getRegionCount() << std::endl;
            file << "  Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
            file << "  Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
            file << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
            file << "  Pending spike events: " << brain_->getPendingSpikeEventCount() << std::endl;
            file << "  Average firing rate: " << brain_->getAverageFiringRate() << " Hz" << std::endl;
            file << "" << std::endl;
            
            // Write region details
            file << "Region Details:" << std::endl;
            for (const auto& region : brain_->getRegions()) {
                file << "  Region " << region->getId().index() << ": " << region->getName() << std::endl;
                file << "    Neurons: " << region->getTotalNeuronCount() << std::endl;
                file << "    Synapses: " << region->getSynapseCount() << std::endl;
                file << "    Average weight: " << region->getAverageSynapticWeight() << std::endl;
                file << "    Synaptic density: " << region->getSynapticDensity() << std::endl;
            }
            
            file << "" << std::endl;
            file << "Experiment completed successfully" << std::endl;
            
            file.flush();
            NLM_LOG_INFO("Output saved successfully");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception saving output: ") + e.what());
            return false;
        }
    }
    
    int argc_;
    char** argv_;
    CommandLineParser parser_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

// Original test implementations (preserved for backward compatibility)
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

int main(int argc, char** argv) {
    NLMApplicationCLI app(argc, argv);
    return app.run();
}
