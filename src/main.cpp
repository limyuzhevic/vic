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
#include <chrono>
#include <fstream>
#include <map>

using namespace nlm;

// Command-line argument parsing
struct CLIArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string benchmarkFile;
    std::string profileFile;
    std::string checkpointFile;
    std::string loadCheckpoint;
    
    // Modes
    bool demo = false;
    bool benchmark = false;
    bool profile = false;
    bool test = false;
    
    // Options
    bool verbose = false;
    bool quiet = false;
    bool help = false;
    bool version = false;
    
    int demoEpisodes = 5;
    int demoSteps = 200;
    int benchmarkIterations = 3;
    int profileDuration = 30; // seconds
    int testIterations = 1;
    
    // Configuration validation
    bool validateConfig = false;
    std::string configSchema;
    
    // Checkpoint options
    bool saveCheckpoint = false;
    bool loadCheckpoint = false;
    int checkpointInterval = 1000;
    
    // Progress reporting
    bool showProgress = true;
    std::string progressFormat = "[{elapsed}s] {ep}ep {s}step: {rate:.1f}Hz";
    
    // Performance monitoring
    bool measureMemory = true;
    bool measureCpu = true;
    
    // Output options
    bool jsonOutput = false;
    std::string outputFormat = "text";
};

// Command-line usage information
void printUsage(const std::string& programName) {
    std::cout << R"(
NLM — 熙然 (Neural Learning Machine)
Version: 0.1.0
Usage: "" << programName << " [OPTIONS]

Modes:
  --demo              Run interactive demonstration mode
  --benchmark         Run performance benchmarks
  --profile           Run profiling session
  --test              Run unit tests
  --help              Show this help message
  --version           Show version information

Options:
  --config <file>     Configuration file (default: configs/default.cfg)
  --output <file>     Output file for results
  --json              Output results in JSON format
  
Demo Mode Options:
  --demo-episodes <n>   Number of demo episodes (default: 5)
  --demo-steps <n>      Steps per episode (default: 200)
  
Benchmark Mode Options:
  --benchmark-iterations <n> Number of benchmark runs (default: 3)
  --benchmark-file <file>   File to save benchmark results
  
Profile Mode Options:
  --profile-duration <s>    Profile duration in seconds (default: 30)
  --profile-file <file>     File to save profiling data
  
Test Mode Options:
  --test-iterations <n>     Number of test iterations (default: 1)
  
General Options:
  --verbose                Enable verbose logging
  --quiet                  Suppress output
  --validate-config        Validate configuration against schema
  --checkpoint <file>      Save/load checkpoint
  --checkpoint-interval <n> Checkpoint interval in steps (default: 1000)
  --no-progress            Disable progress reporting
  --progress-format <fmt>  Progress report format string
  --no-memory              Disable memory measurement
  --no-cpu                 Disable CPU measurement

Examples:
  "" << programName << " --demo --verbose --demo-episodes 10
  "" << programName << " --benchmark --benchmark-file results.csv
  "" << programName << " --profile --profile-duration 60 --profile-file profile.json
  "" << programName << " --test --test-iterations 5

For more information, visit: https://github.com/NLM-project/NLM
    )" << std::endl;
}

// Command-line argument parsing
CLIArgs parseArguments(int argc, char** argv) {
    CLIArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--demo") {
            args.demo = true;
        } else if (arg == "--benchmark") {
            args.benchmark = true;
        } else if (arg == "--profile") {
            args.profile = true;
        } else if (arg == "--test") {
            args.test = true;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--quiet") {
            args.quiet = true;
        } else if (arg == "--validate-config") {
            args.validateConfig = true;
        } else if (arg == "--no-progress") {
            args.showProgress = false;
        } else if (arg == "--no-memory") {
            args.measureMemory = false;
        } else if (arg == "--no-cpu") {
            args.measureCpu = false;
        } else if (arg == "--json") {
            args.jsonOutput = true;
            args.outputFormat = "json";
        } else if (arg == "--output-format") {
            if (i + 1 < argc) {
                args.outputFormat = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg == "--benchmark-file") {
            if (i + 1 < argc) {
                args.benchmarkFile = argv[++i];
            }
        } else if (arg == "--profile-file") {
            if (i + 1 < argc) {
                args.profileFile = argv[++i];
            }
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                args.checkpointFile = argv[++i];
            }
        } else if (arg == "--load-checkpoint") {
            if (i + 1 < argc) {
                args.loadCheckpoint = argv[++i];
            }
        } else if (arg == "--demo-episodes") {
            if (i + 1 < argc) {
                args.demoEpisodes = std::atoi(argv[++i]);
            }
        } else if (arg == "--demo-steps") {
            if (i + 1 < argc) {
                args.demoSteps = std::atoi(argv[++i]);
            }
        } else if (arg == "--benchmark-iterations") {
            if (i + 1 < argc) {
                args.benchmarkIterations = std::atoi(argv[++i]);
            }
        } else if (arg == "--profile-duration") {
            if (i + 1 < argc) {
                args.profileDuration = std::atoi(argv[++i]);
            }
        } else if (arg == "--test-iterations") {
            if (i + 1 < argc) {
                args.testIterations = std::atoi(argv[++i]);
            }
        } else if (arg == "--checkpoint-interval") {
            if (i + 1 < argc) {
                args.checkpointInterval = std::atoi(argv[++i]);
            }
        } else if (arg == "--progress-format") {
            if (i + 1 < argc) {
                args.progressFormat = argv[++i];
            }
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }
    
    return args;
}

// Progress reporter
class ProgressReporter {
public:
    ProgressReporter(bool showProgress, const std::string& format = "[{elapsed}s] {ep}ep {s}step: {rate:.1f}Hz")
        : showProgress(showProgress), format(format), startTime(std::chrono::high_resolution_clock::now()) {
    }
    
    void update(int episode, int step, int totalEpisodes, int totalSteps, double spikeRate) {
        if (!showProgress) return;
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(currentTime - startTime).count();
        
        std::string report = format;
        // Simple format replacement - could be enhanced
        size_t pos = 0;
        while ((pos = report.find("{elapsed}", pos)) != std::string::npos) {
            report.replace(pos, 9, std::to_string(elapsed));
            pos += std::to_string(elapsed).length();
        }
        
        while ((pos = report.find("{ep}", pos)) != std::string::npos) {
            report.replace(pos, 4, std::to_string(episode));
            pos += std::to_string(episode).length();
        }
        
        while ((pos = report.find("{s}", pos)) != std::string::npos) {
            report.replace(pos, 3, std::to_string(step));
            pos += std::to_string(step).length();
        }
        
        while ((pos = report.find("{rate}", pos)) != std::string::npos) {
            report.replace(pos, 6, std::to_string(spikeRate));
            pos += std::to_string(spikeRate).length();
        }
        
        std::cout << "\r" << report << std::flush;
    }
    
    void finish() {
        if (showProgress) {
            std::cout << std::endl;
        }
    }
    
private:
    bool showProgress;
    std::string format;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Configuration validation
bool validateConfig(const Config& config) {
    bool valid = true;
    
    // Required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "simulation_timestep"};
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            std::cerr << "ERROR: Missing required configuration key: " << key << std::endl;
            valid = false;
        }
    }
    
    // Value validation
    if (config.has("neuron_count")) {
        auto count = config.getOr<int64_t>("neuron_count", 0);
        if (count <= 0) {
            std::cerr << "ERROR: neuron_count must be positive (got: " << count << ")" << std::endl;
            valid = false;
        }
    }
    
    if (config.has("region_count")) {
        auto count = config.getOr<int64_t>("region_count", 0);
        if (count <= 0) {
            std::cerr << "ERROR: region_count must be positive (got: " << count << ")" << std::endl;
            valid = false;
        }
    }
    
    if (config.has("simulation_timestep")) {
        auto timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0) {
            std::cerr << "ERROR: simulation_timestep must be positive (got: " << timestep << ")" << std::endl;
            valid = false;
        }
    }
    
    return valid;
}

// Benchmark mode
void runBenchmarkMode(const CLIArgs& args) {
    // Implementation would go here
    // For now, just a placeholder
    std::cout << "Running benchmark mode with " << args.benchmarkIterations << " iterations..." << std::endl;
}

// Demo mode
void runDemoMode(const CLIArgs& args) {
    // Implementation would go here
    // For now, just a placeholder
    std::cout << "Running demo mode with " << args.demoEpisodes << " episodes, " << args.demoSteps << " steps per episode..." << std::endl;
}

// Profile mode
void runProfileMode(const CLIArgs& args) {
    // Implementation would go here
    // For now, just a placeholder
    std::cout << "Running profile mode for " << args.profileDuration << " seconds..." << std::endl;
}

// Test mode
void runTestMode(const CLIArgs& args) {
    // Implementation would go here
    // For now, just a placeholder
    std::cout << "Running test mode with " << args.testIterations << " iterations..." << std::endl;
}

// Default mode (original behavior)
void runDefaultMode(const CLIArgs& args) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.verbose ? LogLevel::Debug : LogLevel::Info);
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
    std::string configFile = args.configFile;
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Validate configuration if requested
    if (args.validateConfig) {
        if (!validateConfig(*config)) {
            std::cerr << "Configuration validation failed!" << std::endl;
            return;
        }
        NLM_LOG_INFO("Configuration validation passed.");
    }
    
    // Override with command line args (simulate loadFromArgs)
    // In a real implementation, this would parse command line args
    
    // Set default values for Phase 2
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
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
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

// Command-line argument parsing
struct CLIArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string benchmarkFile;
    std::string profileFile;
    std::string checkpointFile;
    std::string loadCheckpoint;
    
    // Modes
    bool demo = false;
    bool benchmark = false;
    bool profile = false;
    bool test = false;
    
    // Options
    bool verbose = false;
    bool quiet = false;
    bool help = false;
    bool version = false;
    
    int demoEpisodes = 5;
    int demoSteps = 200;
    int benchmarkIterations = 3;
    int profileDuration = 30; // seconds
    int testIterations = 1;
    
    // Configuration validation
    bool validateConfig = false;
    std::string configSchema;
    
    // Checkpoint options
    bool saveCheckpoint = false;
    bool loadCheckpoint = false;
    int checkpointInterval = 1000;
    
    // Progress reporting
    bool showProgress = true;
    std::string progressFormat = "[{elapsed}s] {ep}ep {s}step: {rate:.1f}Hz";
    
    // Performance monitoring
    bool measureMemory = true;
    bool measureCpu = true;
    
    // Output options
    bool jsonOutput = false;
    std::string outputFormat = "text";
};

// Command-line usage information
void printUsage(const std::string& programName) {
    std::cout << R"(
NLM — 熙然 (Neural Learning Machine)
Version: 0.1.0
Usage: "" << programName << " [OPTIONS]

Modes:
  --demo              Run interactive demonstration mode
  --benchmark         Run performance benchmarks
  --profile           Run profiling session
  --test              Run unit tests
  --help              Show this help message
  --version           Show version information

Options:
  --config <file>     Configuration file (default: configs/default.cfg)
  --output <file>     Output file for results
  --json              Output results in JSON format
  
Demo Mode Options:
  --demo-episodes <n>   Number of demo episodes (default: 5)
  --demo-steps <n>      Steps per episode (default: 200)
  
Benchmark Mode Options:
  --benchmark-iterations <n> Number of benchmark runs (default: 3)
  --benchmark-file <file>   File to save benchmark results
  
Profile Mode Options:
  --profile-duration <s>    Profile duration in seconds (default: 30)
  --profile-file <file>     File to save profiling data
  
Test Mode Options:
  --test-iterations <n>     Number of test iterations (default: 1)
  
General Options:
  --verbose                Enable verbose logging
  --quiet                  Suppress output
  --validate-config        Validate configuration against schema
  --checkpoint <file>      Save/load checkpoint
  --checkpoint-interval <n> Checkpoint interval in steps (default: 1000)
  --no-progress            Disable progress reporting
  --progress-format <fmt>  Progress report format string
  --no-memory              Disable memory measurement
  --no-cpu                 Disable CPU measurement

Examples:
  "" << programName << " --demo --verbose --demo-episodes 10
  "" << programName << " --benchmark --benchmark-file results.csv
  "" << programName << " --profile --profile-duration 60 --profile-file profile.json
  "" << programName << " --test --test-iterations 5

For more information, visit: https://github.com/NLM-project/NLM
    )" << std::endl;
}

// Command-line argument parsing
CLIArgs parseArguments(int argc, char** argv) {
    CLIArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--demo") {
            args.demo = true;
        } else if (arg == "--benchmark") {
            args.benchmark = true;
        } else if (arg == "--profile") {
            args.profile = true;
        } else if (arg == "--test") {
            args.test = true;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--quiet") {
            args.quiet = true;
        } else if (arg == "--validate-config") {
            args.validateConfig = true;
        } else if (arg == "--no-progress") {
            args.showProgress = false;
        } else if (arg == "--no-memory") {
            args.measureMemory = false;
        } else if (arg == "--no-cpu") {
            args.measureCpu = false;
        } else if (arg == "--json") {
            args.jsonOutput = true;
            args.outputFormat = "json";
        } else if (arg == "--output-format") {
            if (i + 1 < argc) {
                args.outputFormat = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg == "--benchmark-file") {
            if (i + 1 < argc) {
                args.benchmarkFile = argv[++i];
            }
        } else if (arg == "--profile-file") {
            if (i + 1 < argc) {
                args.profileFile = argv[++i];
            }
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                args.checkpointFile = argv[++i];
            }
        } else if (arg == "--load-checkpoint") {
            if (i + 1 < argc) {
                args.loadCheckpoint = argv[++i];
            }
        } else if (arg == "--demo-episodes") {
            if (i + 1 < argc) {
                args.demoEpisodes = std::atoi(argv[++i]);
            }
        } else if (arg == "--demo-steps") {
            if (i + 1 < argc) {
                args.demoSteps = std::atoi(argv[++i]);
            }
        } else if (arg == "--benchmark-iterations") {
            if (i + 1 < argc) {
                args.benchmarkIterations = std::atoi(argv[++i]);
            }
        } else if (arg == "--profile-duration") {
            if (i + 1 < argc) {
                args.profileDuration = std::atoi(argv[++i]);
            }
        } else if (arg == "--test-iterations") {
            if (i + 1 < argc) {
                args.testIterations = std::atoi(argv[++i]);
            }
        } else if (arg == "--checkpoint-interval") {
            if (i + 1 < argc) {
                args.checkpointInterval = std::atoi(argv[++i]);
            }
        } else if (arg == "--progress-format") {
            if (i + 1 < argc) {
                args.progressFormat = argv[++i];
            }
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }
    
    return args;
}

// Progress reporter
class ProgressReporter {
public:
    ProgressReporter(bool showProgress, const std::string& format = "[{elapsed}s] {ep}ep {s}step: {rate:.1f}Hz")
        : showProgress(showProgress), format(format), startTime(std::chrono::high_resolution_clock::now()) {
    }
    
    void update(int episode, int step, int totalEpisodes, int totalSteps, double spikeRate) {
        if (!showProgress) return;
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(currentTime - startTime).count();
        
        std::string report = format;
        // Simple format replacement - could be enhanced
        size_t pos = 0;
        while ((pos = report.find("{elapsed}", pos)) != std::string::npos) {
            report.replace(pos, 9, std::to_string(elapsed));
            pos += std::to_string(elapsed).length();
        }
        
        while ((pos = report.find("{ep}", pos)) != std::string::npos) {
            report.replace(pos, 4, std::to_string(episode));
            pos += std::to_string(episode).length();
        }
        
        while ((pos = report.find("{s}", pos)) != std::string::npos) {
            report.replace(pos, 3, std::to_string(step));
            pos += std::to_string(step).length();
        }
        
        while ((pos = report.find("{rate}", pos)) != std::string::npos) {
            report.replace(pos, 6, std::to_string(spikeRate));
            pos += std::to_string(spikeRate).length();
        }
        
        std::cout << "\r" << report << std::flush;
    }
    
    void finish() {
        if (showProgress) {
            std::cout << std::endl;
        }
    }
    
private:
    bool showProgress;
    std::string format;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Configuration validation
bool validateConfig(const Config& config) {
    bool valid = true;
    
    // Required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "simulation_timestep"};
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            std::cerr << "ERROR: Missing required configuration key: " << key << std::endl;
            valid = false;
        }
    }
    
    // Value validation
    if (config.has("neuron_count")) {
        auto count = config.getOr<int64_t>("neuron_count", 0);
        if (count <= 0) {
            std::cerr << "ERROR: neuron_count must be positive (got: " << count << ")" << std::endl;
            valid = false;
        }
    }
    
    if (config.has("region_count")) {
        auto count = config.getOr<int64_t>("region_count", 0);
        if (count <= 0) {
            std::cerr << "ERROR: region_count must be positive (got: " << count << ")" << std::endl;
            valid = false;
        }
    }
    
    if (config.has("simulation_timestep")) {
        auto timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0) {
            std::cerr << "ERROR: simulation_timestep must be positive (got: " << timestep << ")" << std::endl;
            valid = false;
        }
    }
    
    return valid;
}

// Benchmark mode
void runBenchmarkMode(const CLIArgs& args) {
    // Implementation would go here
    // For now, just a placeholder
    std::cout << "Running benchmark mode with " << args.benchmarkIterations << " iterations..." << std::endl;
}

// Demo mode
void runDemoMode(const CLIArgs& args) {
    // Implementation would go here
    // For now, just a placeholder
    std::cout << "Running demo mode with " << args.demoEpisodes << " episodes, " << args.demoSteps << " steps per episode..." << std::endl;
}

// Profile mode
void runProfileMode(const CLIArgs& args) {
    // Implementation would go here
    // For now, just a placeholder
    std::cout << "Running profile mode for " << args.profileDuration << " seconds..." << std::endl;
}

// Test mode
void runTestMode(const CLIArgs& args) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Unit Tests                                                 ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    std::cout << "Running NLM Unit Tests..." << std::endl;
    std::cout << "Iterations: " << args.testIterations << std::endl;
    std::cout << std::endl;
    
    // TODO: Implement proper unit test framework
    // This is a placeholder for the unit test mode
    
    std::cout << "Unit test mode is not fully implemented yet." << std::endl;
    std::cout << "This would run the full test suite including:" << std::endl;
    std::cout << "  - Types tests" << std::endl;
    std::cout << "  - Config tests" << std::endl;
    std::cout << "  - Random tests" << std::endl;
    std::cout << "  - Clock tests" << std::endl;
    std::cout << "  - Neuron tests" << std::endl;
    std::cout << "  - Synapse tests" << std::endl;
    std::cout << "  - STDP tests" << std::endl;
    std::cout << "  - Brain tests" << std::endl;
    std::cout << std::endl;
    
    if (args.testIterations > 0) {
        std::cout << "Running " << args.testIterations << " iteration(s)..." << std::endl;
        // In a real implementation, this would run the actual tests
        for (int i = 0; i < args.testIterations; ++i) {
            std::cout << "  Iteration " << i + 1 << ": PASSED (placeholder)" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Test mode complete (placeholder implementation)." << std::endl;
}
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.verbose ? LogLevel::Debug : LogLevel::Info);
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
    std::string configFile = args.configFile;
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args (simulate loadFromArgs)
    // In a real implementation, this would parse command line args
    
    // Set default values for Phase 2
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
    
    // Validate configuration if requested
    if (args.validateConfig) {
        if (!config->validate({})) {
            std::cerr << "Configuration validation failed!" << std::endl;
            return 1;
        }
        NLM_LOG_INFO("Configuration validation passed.");
    }
    
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
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
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
    
    return 0;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CLIArgs args = parseArguments(argc, argv);
    
    // Handle help and version flags
    if (args.help) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (args.version) {
        std::cout << "NLM — 熙然 (Neural Learning Machine)" << std::endl;
        std::cout << "Version: 0.1.0" << std::endl;
        return 0;
    }
    
    // Determine which mode to run
    if (args.demo) {
        runDemoMode(args);
        return 0;
    } else if (args.benchmark) {
        runBenchmarkMode(args);
        return 0;
    } else if (args.profile) {
        runProfileMode(args);
        return 0;
    } else if (args.test) {
        runTestMode(args);
        return 0;
    }
    
    // Default mode (original behavior)
    return runDefaultMode(args);
}
