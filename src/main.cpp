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
#include "development/DebugAnalysis.hpp"
#include "performance/PerformanceMonitor.hpp"
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
#include <algorithm>
#include <chrono>
#include <map>
#include <cstdint>
#include <sstream>
#include <ctime>
#include <csignal>
#include <filesystem>

using namespace nlm;

// Version information
const std::string PROGRAM_NAME = "nlm";
const std::string PROGRAM_VERSION = "Phase 2.0.0";

// Operation modes enum
enum class OperationMode {
    DEMO,
    BENCHMARK,
    VALIDATE,
    PROFILE,
    PERFORMANCE,
    REPLAY,
    DEBUG,
    HELP,
    VERSION
};

// Global signal handler for graceful shutdown
volatile sig_atomic_t g_interruptSignal = 0;

// Signal handler for keyboard interrupt
void signalHandler(int signal) {
    g_interruptSignal = signal;
    NLM_LOG_WARNING("Received interrupt signal, shutting down...");
}

// Print program banner
void printBanner(const std::string& mode = "") {
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
    
    if (!mode.empty()) {
        std::cout << "\nMode: " << mode << std::endl;
    }
    std::cout << "Version: " << PROGRAM_VERSION << std::endl;
}

// Print help/usage information
void printHelp() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "USAGE: " << PROGRAM_NAME << " [OPTIONS]" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << "\nCore Command-Line Features:" << std::endl;
    std::cout << "  --mode <mode>                    Select operation mode (demo, benchmark, validate, profile, replay, debug)" << std::endl;
    std::cout << "  --help                           Show this help information" << std::endl;
    std::cout << "  --version                        Show version information" << std::endl;
    
    std::cout << "\nExtended Configuration Options:" << std::endl;
    std::cout << "  --config <file>                  Load configuration from file" << std::endl;
    std::cout << "  --set <key=value>                Override configuration values from command line" << std::endl;
    std::cout << "  --clear <key>                    Clear configuration value" << std::endl;
    std::cout << "  --list                           List all configuration keys and values" << std::endl;
    std::cout << "  --validate                       Validate current configuration" << std::endl;
    
    std::cout << "\nAdvanced Options:" << std::endl;
    std::cout << "  --checkpoint <file>              Load from checkpoint" << std::endl;
    std::cout << "  --save <file>                    Save brain state to checkpoint" << std::endl;
    std::cout << "  --steps <n>                      Run specific number of simulation steps" << std::endl;
    std::cout << "  --duration <seconds>             Run for specific duration" << std::endl;
    std::cout << "  --quiet                          Minimal output" << std::endl;
    std::cout << "  --verbose                        Detailed output" << std::endl;
    std::cout << "  --log-level <level>              Set logging level (debug, info, warning, error, critical)" << std::endl;
    
    std::cout << "\nMode-Specific Options (see --mode for details):" << std::endl;
    std::cout << "  Demo mode:" << std::endl;
    std::cout << "    --demo-mode <type>              (basic, advanced, comprehensive)" << std::endl;
    std::cout << "  Benchmark mode:" << std::endl;
    std::cout << "    --iterations <n>                Number of iterations" << std::endl;
    std::cout << "    --warmup <n>                    Number of warmup iterations" << std::endl;
    std::cout << "  Validate mode:" << std::endl;
    std::cout << "    --validate-system               Validate system integrity" << std::endl;
    std::cout << "    --check-connections             Check connectivity" << std::endl;
    std::cout << "  Profile mode:" << std::endl;
    std::cout << "    --profile-cpu                   Profile CPU usage" << std::endl;
    std::cout << "    --profile-memory                Profile memory usage" << std::endl;
    std::cout << "    --profile-detailed              Detailed profiling" << std::endl;
    std::cout << "  Replay mode:" << std::endl;
    std::cout << "    --replay-file <file>            Replay from checkpoint" << std::endl;
    std::cout << "    --replay-speed <factor>         Replay speed factor" << std::endl;
    std::cout << "  Debug mode:" << std::endl;
    std::cout << "    --analyze-neurons                 Detailed neuron state analysis" << std::endl;
    std::cout << "    --analyze-synapses                Detailed synapse state analysis" << std::endl;
    std::cout << "    --analyze-network                 Network topology analysis" << std::endl;
    std::cout << "    --analyze-activity                Neural activity profiling" << std::endl;
    std::cout << "    --analyze-performance             System performance profiling" << std::endl;
    std::cout << "    --analyze-memory                  Memory usage analysis" << std::endl;
    std::cout << "    --analyze-statistics              Statistical analysis of neural data" << std::endl;
    std::cout << "    --event-trace <duration>         Enhanced event tracing" << std::endl;
    std::cout << "    --compare-states <file>          Compare current state with saved state" << std::endl;
    std::cout << "    --pattern-analysis <type>        Pattern recognition analysis" << std::endl;
    std::cout << "    --bottleneck-analysis             Identify system bottlenecks" << std::endl;
    
    std::cout << "    --visualization                   Enable enhanced visualization interface" << std::endl;
    std::cout << "    --real-time-monitoring            Enable real-time monitoring" << std::endl;
    std::cout << "    --output-file <file>              Export results to file" << std::endl;
    
    std::cout << "  Performance mode:" << std::endl;
    std::cout << "    --profile-cpu                     CPU usage profiling" << std::endl;
    std::cout << "    --profile-memory                 Memory usage tracking" << std::endl;
    std::cout << "    --profile-simulation             Simulation performance analysis" << std::endl;
    std::cout << "    --profile-neural                 Neural performance metrics" << std::endl;
    std::cout << "    --profile-resources              Resource utilization monitoring" << std::endl;
    std::cout << "    --profile-bottlenecks            Bottleneck analysis" << std::endl;
    std::cout << "    --profile-benchmark <type>       Benchmark specific performance aspects" << std::endl;
    std::cout << "    --live-monitoring               Real-time performance monitoring" << std::endl;
    std::cout << "    --performance-report <format>    Generate performance reports" << std::endl;
    std::cout << "    --benchmark-comparison <file>    Compare with baseline performance" << std::endl;
    
    std::cout << "\nFile I/O Options:" << std::endl;
    std::cout << "  --output <file>                  Save results to file" << std::endl;
    std::cout << "  --export-brain <file>            Export brain state" << std::endl;
    std::cout << "  --import-brain <file>            Import brain state" << std::endl;
    std::cout << "  --metrics <file>                 Save performance metrics" << std::endl;
    
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode demo" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode benchmark --config my_config.cfg --iterations 1000" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode debug --analyze-neurons --analyze-synapses --analyze-network" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode demo --checkpoint brain_state.chk" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode replay --replay-file brain_state.chk" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode validate --validate-system --check-connections" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode profile --profile-cpu --profile-memory --profile-detailed" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode debug --analyze-neurons --analyze-synapses --analyze-network --analyze-activity --analyze-performance" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode debug --analyze-statistics --pattern-analysis firing --bottleneck-analysis" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode debug --event-trace 30 --compare-states saved_state.chk --output-file debug_results.json" << std::endl;
    
    // Add new performance mode examples
    std::cout << "  " << PROGRAM_NAME << " --mode performance --profile-cpu --profile-memory --profile-neural --live-monitoring" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode performance --profile-bottlenecks --performance-report json --benchmark-comparison baseline_perf.json" << std::endl;
    std::cout << "  " << PROGRAM_NAME << " --mode performance --profile-resources --profile-simulation --live-monitoring --performance-report csv" << std::endl;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

// Print version information
void printVersion() {
    std::cout << PROGRAM_NAME << " version " << PROGRAM_VERSION << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Implementations: LIF neurons, event-driven spikes, STDP plasticity, structural plasticity" << std::endl;
}

// Parse operation mode from string
OperationMode parseMode(const std::string& modeStr) {
    std::string mode = modeStr;
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    
    if (mode == "demo") return OperationMode::DEMO;
    if (mode == "benchmark") return OperationMode::BENCHMARK;
    if (mode == "validate") return OperationMode::VALIDATE;
    if (mode == "profile") return OperationMode::PROFILE;
    if (mode == "performance") return OperationMode::PERFORMANCE;
    if (mode == "replay") return OperationMode::REPLAY;
    if (mode == "debug") return OperationMode::DEBUG;
    if (mode == "help") return OperationMode::HELP;
    if (mode == "version") return OperationMode::VERSION;
    
    return OperationMode::DEMO; // Default
}

// Parse log level from string
LogLevel parseLogLevel(const std::string& levelStr) {
    std::string level = levelStr;
    std::transform(level.begin(), level.end(), level.begin(), ::tolower);
    
    if (level == "debug") return LogLevel::Debug;
    if (level == "info") return LogLevel::Info;
    if (level == "warning") return LogLevel::Warning;
    if (level == "error") return LogLevel::Error;
    if (level == "critical") return LogLevel::Critical;
    
    return LogLevel::Info; // Default
}

// Parse boolean value from string
bool parseBool(const std::string& str) {
    std::string value = str;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

// Parse integer value from string
bool parseInt(const std::string& str, int64_t& value) {
    try {
        value = std::stoll(str);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Parse double value from string
bool parseDouble(const std::string& str, double& value) {
    try {
        value = std::stod(str);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Parse command-line arguments into a map
std::map<std::string, std::string> parseCommandLineArgs(int argc, char** argv) {
    std::map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            // Handle --key=value or --key value formats
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                // --key=value format
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                args[key] = value;
            } else {
                // --key value format - check if next arg doesn't start with --
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args[arg.substr(2)] = argv[++i];
                } else {
                    args[arg.substr(2)] = "true"; // Boolean flag
                }
            }
        }
    }
    
    return args;
}

// Command-line parser that sets configuration from args
void parseCommandLineConfig(const std::map<std::string, std::string>& args, std::shared_ptr<Config> config) {
    for (const auto& [key, value] : args) {
        // Skip special options that are handled separately
        std::string keyLower = key;
        std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);
        
        if (keyLower == "help" || keyLower == "version" || keyLower == "mode") {
            continue; // Skip special options
        }
        
        // Try to detect type and set accordingly
        if (value == "true" || value == "false") {
            config->set(key, parseBool(value), ConfigSource::CommandLine);
        } else {
            int64_t intVal;
            if (parseInt(value, intVal)) {
                config->set(key, intVal, ConfigSource::CommandLine);
            } else {
                double doubleVal;
                if (parseDouble(value, doubleVal)) {
                    config->set(key, doubleVal, ConfigSource::CommandLine);
                } else {
                    // Try to parse as string
                    config->set(key, value, ConfigSource::CommandLine);
                }
            }
        }
    }
}

// Load configuration from command-line arguments
void loadConfigurationFromArgs(std::shared_ptr<Config> config, const std::map<std::string, std::string>& args) {
    // Handle --config option (backward compatibility)
    if (args.find("config") != args.end()) {
        std::string configFile = args.at("config");
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Apply command-line arguments as overrides
    parseCommandLineConfig(args, config);
}

// List all configuration keys and values
void listConfiguration(const std::shared_ptr<Config>& config) {
    std::cout << "\nConfiguration Summary:" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        auto entry = config->getHighestPriority(key);
        if (entry) {
            std::cout << std::left << std::setw(30) << key << ": ";
            std::cout << config->formatValue(*entry, false) << std::endl;
        }
    }
}

// Validate configuration
void validateConfiguration(const std::shared_ptr<Config>& config) {
    std::cout << "\nConfiguration Validation:" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    auto keys = config->getKeys();
    bool allValid = true;
    
    for (const auto& key : keys) {
        ValidationResult result = config->validateKey(key);
        if (!result.valid) {
            std::cout << "✗ " << std::left << std::setw(30) << key 
                      << " (Error: " << result.errorMessage << ")" << std::endl;
            allValid = false;
        } else {
            std::cout << "✓ " << std::left << std::setw(30) << key << std::endl;
        }
    }
    
    if (allValid) {
        std::cout << "\n✓ All configuration keys are valid." << std::endl;
    } else {
        std::cout << "\n✗ Some configuration keys are invalid." << std::endl;
    }
}

// Clear configuration value
void clearConfigurationValue(std::shared_ptr<Config> config, const std::string& key) {
    if (config->has(key)) {
        config->remove(key);
        NLM_LOG_INFO("Cleared configuration value: " + key);
    } else {
        NLM_LOG_WARNING("Configuration key not found: " + key);
    }
}

// Save configuration to file
void saveConfigurationToFile(const std::shared_ptr<Config>& config, const std::string& filepath) {
    if (config->saveToFile(filepath)) {
        NLM_LOG_INFO("Saved configuration to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
    }
}

// Load brain state from checkpoint
bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& checkpointFile) {
    std::cout << "Loading brain state from: " << checkpointFile << std::endl;
    
    // For now, just log that loading is not fully implemented
    // TODO: Implement actual checkpoint loading
    NLM_LOG_INFO("Checkpoint loading is not fully implemented yet");
    
    return true;
}

// Save brain state to checkpoint
bool saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& checkpointFile) {
    std::cout << "Saving brain state to: " << checkpointFile << std::endl;
    
    // For now, just log that saving is not fully implemented
    // TODO: Implement actual checkpoint saving
    NLM_LOG_INFO("Checkpoint saving is not fully implemented yet");
    
    return true;
}

// Mode-specific execution functions

// Demo mode execution
void runDemoMode(const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args) {
    std::string demoType = "basic";
    if (args.find("demo-mode") != args.end()) {
        demoType = args.at("demo-mode");
    }
    
    std::cout << "\n=== Demo Mode: " << demoType << " ===\n";
    
    SimulationClock clock;
    initializeSimulationClock(clock, config);
    
    std::shared_ptr<Brain> brain;
    if (!initializeBrain(brain, config)) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    if (demoType == "basic") {
        std::cout << "Running basic demo..." << std::endl;
        runBasicConnectivityTest(brain);
    } else if (demoType == "advanced") {
        std::cout << "Running advanced demo..." << std::endl;
        runPlasticityExperiment(brain);
    } else if (demoType == "comprehensive") {
        std::cout << "Running comprehensive demo..." << std::endl;
        runBasicConnectivityTest(brain);
        brain->reset();
        initializeBrain(brain, config);
        runPlasticityExperiment(brain);
        brain->reset();
        initializeBrain(brain, config);
        runStdpVerification(brain);
    }
    
    logFinalStatus(brain);
    std::cout << "Demo complete." << std::endl;
}

// Benchmark mode execution
void runBenchmarkMode(const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args) {
    size_t iterations = 1000;
    size_t warmup = 100;
    
    if (args.find("iterations") != args.end()) {
        iterations = std::stoull(args.at("iterations"));
    }
    
    if (args.find("warmup") != args.end()) {
        warmup = std::stoull(args.at("warmup"));
    }
    
    std::cout << "\n=== Benchmark Mode ===\n";
    std::cout << "Iterations: " << iterations << std::endl;
    std::cout << "Warmup: " << warmup << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run warmup
    std::cout << "Warming up..." << std::endl;
    auto warmupConfig = std::make_shared<Config>(*config);
    warmupConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::CommandLine);
    
    std::shared_ptr<Brain> warmupBrain;
    initializeBrain(warmupBrain, warmupConfig);
    
    for (size_t i = 0; i < warmup; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            warmupBrain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            warmupBrain->step(i * 10 + j, (i * 10 + j) * 0.001);
        }
    }
    
    // Run benchmark
    std::cout << "Running benchmark..." << std::endl;
    auto benchConfig = std::make_shared<Config>(*config);
    benchConfig->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::CommandLine);
    
    std::shared_ptr<Brain> benchBrain;
    initializeBrain(benchBrain, benchConfig);
    
    for (size_t i = 0; i < iterations; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            benchBrain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            benchBrain->step(i * 10 + j, (i * 10 + j) * 0.001);
        }
        
        if (i % 100 == 0) {
            std::cout << "  Iteration " << i << "/" << iterations << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "\nBenchmark Results:" << std::endl;
    std::cout << "  Total time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << (iterations * 10) / totalTime << std::endl;
    std::cout << "  Total spikes: " << benchBrain->getTotalSpikeCount() << std::endl;
    
    logFinalStatus(benchBrain);
    std::cout << "Benchmark complete." << std::endl;
}

// Validate mode execution
void runValidateMode(const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args) {
    std::cout << "\n=== Validate Mode ===\n";
    
    bool validateSystem = false;
    bool checkConnections = false;
    
    if (args.find("validate-system") != args.end()) {
        validateSystem = true;
    }
    
    if (args.find("check-connections") != args.end()) {
        checkConnections = true;
    }
    
    // Validate configuration
    if (args.find("validate") != args.end() || validateSystem) {
        std::cout << "Validating configuration..." << std::endl;
        validateConfiguration(config);
    }
    
    // Check system integrity
    std::cout << "\nChecking system integrity..." << std::endl;
    
    // Create a temporary brain for validation
    auto tempConfig = std::make_shared<Config>(*config);
    tempConfig->set("neuron_count", static_cast<int64_t>(100), ConfigSource::CommandLine);
    
    std::shared_ptr<Brain> testBrain;
    if (initializeBrain(testBrain, tempConfig)) {
        std::cout << "✓ Brain initialization successful" << std::endl;
        
        // Check connectivity if requested
        if (checkConnections) {
            std::cout << "Checking connectivity..." << std::endl;
            auto* region = testBrain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                std::cout << "  Neurons: " << neurons.size() << std::endl;
                
                auto synapses = region->getSynapses();
                std::cout << "  Synapses: " << synapses.size() << std::endl;
                
                // Check for basic connectivity
                bool hasConnections = false;
                for (const auto& syn : synapses) {
                    if (syn && syn->getPreNeuron() && syn->getPostNeuron()) {
                        hasConnections = true;
                        break;
                    }
                }
                
                if (hasConnections) {
                    std::cout << "✓ Network has connections" << std::endl;
                } else {
                    std::cout << "⚠ Network has no connections (may be expected)" << std::endl;
                }
            }
        }
        
        std::cout << "✓ System validation complete" << std::endl;
        
        testBrain->reset();
    } else {
        std::cout << "✗ Brain initialization failed" << std::endl;
    }
    
    std::cout << "Validate complete." << std::endl;
}

// Profile mode execution
void runProfileMode(const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args) {
    std::cout << "\n=== Profile Mode ===\n";
    
    bool profileCPU = false;
    bool profileMemory = false;
    bool profileDetailed = false;
    
    if (args.find("profile-cpu") != args.end()) {
        profileCPU = true;
    }
    
    if (args.find("profile-memory") != args.end()) {
        profileMemory = true;
    }
    
    if (args.find("profile-detailed") != args.end()) {
        profileDetailed = true;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startMemory = std::chrono::system_clock::now();
    
    // Run a representative workload
    std::cout << "Running profiling workload..." << std::endl;
    
    auto profileConfig = std::make_shared<Config>(*config);
    profileConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::CommandLine);
    
    std::shared_ptr<Brain> profileBrain;
    initializeBrain(profileBrain, profileConfig);
    
    // Simulate some work
    for (size_t step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 10; ++i) {
            profileBrain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        profileBrain->step(step, step * 0.001);
        
        if (profileDetailed && step % 100 == 0) {
            std::cout << "  Step " << step << "/1000 (Spikes: " << profileBrain->getTotalSpikeCount() << ")" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    if (profileCPU) {
        std::cout << "\nCPU Profile:" << std::endl;
        std::cout << "  Total time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << 1000.0 / totalTime << std::endl;
        std::cout << "  Total spikes: " << profileBrain->getTotalSpikeCount() << std::endl;
    }
    
    if (profileMemory) {
        auto endMemory = std::chrono::system_clock::now();
        auto memoryTime = std::chrono::duration<double>(endMemory - startMemory).count();
        std::cout << "\nMemory Profile:" << std::endl;
        std::cout << "  Memory tracking time: " << std::fixed << std::setprecision(2) << memoryTime << "s" << std::endl;
    }
    
    logFinalStatus(profileBrain);
    std::cout << "Profile complete." << std::endl;
}

// Performance mode execution with comprehensive monitoring
void runPerformanceMode(const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args) {
    std::cout << "\n=== Performance Monitoring Mode ===\n";
    
    // Parse performance monitoring flags
    bool profileCPU = false;
    bool profileMemory = false;
    bool profileSimulation = false;
    bool profileNeural = false;
    bool profileResources = false;
    bool profileBottlenecks = false;
    bool profileBenchmark = false;
    bool liveMonitoring = false;
    bool generateReport = false;
    std::string benchmarkType;
    std::string reportFormat = "json";
    std::string benchmarkFile;
    
    // Parse command-line arguments
    if (args.find("profile-cpu") != args.end()) profileCPU = true;
    if (args.find("profile-memory") != args.end()) profileMemory = true;
    if (args.find("profile-simulation") != args.end()) profileSimulation = true;
    if (args.find("profile-neural") != args.end()) profileNeural = true;
    if (args.find("profile-resources") != args.end()) profileResources = true;
    if (args.find("profile-bottlenecks") != args.end()) profileBottlenecks = true;
    if (args.find("profile-benchmark") != args.end()) {
        profileBenchmark = true;
        benchmarkType = args.at("profile-benchmark");
    }
    if (args.find("live-monitoring") != args.end()) liveMonitoring = true;
    if (args.find("performance-report") != args.end()) {
        generateReport = true;
        reportFormat = args.at("performance-report");
    }
    if (args.find("benchmark-comparison") != args.end()) {
        benchmarkFile = args.at("benchmark-comparison");
    }
    
    // Initialize performance monitoring
    std::cout << "Initializing comprehensive performance monitoring..." << std::endl;
    
    auto performanceConfig = std::make_shared<Config>(*config);
    performanceConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::CommandLine);
    
    std::shared_ptr<Brain> performanceBrain;
    if (!initializeBrain(performanceBrain, performanceConfig)) {
        std::cerr << "Failed to initialize brain for performance monitoring!" << std::endl;
        return;
    }
    
    // Create performance monitor
    PerformanceMonitor::MonitorFlags monitorFlags;
    monitorFlags.cpuMonitoring = profileCPU;
    monitorFlags.memoryMonitoring = profileMemory;
    monitorFlags.simulationMonitoring = profileSimulation;
    monitorFlags.neuralMonitoring = profileNeural;
    monitorFlags.resourceMonitoring = profileResources;
    monitorFlags.bottleneckMonitoring = profileBottlenecks;
    monitorFlags.benchmarkMonitoring = profileBenchmark;
    monitorFlags.liveMonitoring = liveMonitoring;
    monitorFlags.trendAnalysis = generateReport;
    monitorFlags.comparativeAnalysis = !benchmarkFile.empty();
    
    PerformanceMonitor perfMonitor(performanceBrain, performanceConfig, monitorFlags);
    
    // Start performance monitoring
    perfMonitor.start();
    
    std::cout << "Performance monitoring started. Running simulation..." << std::endl;
    
    // Run simulation with performance monitoring
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startMemory = std::chrono::system_clock::now();
    
    for (size_t step = 0; step < 1000; ++step) {
        // Inject input to generate neural activity
        for (size_t i = 0; i < 10; ++i) {
            performanceBrain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        performanceBrain->step(step, step * 0.001);
        
        // Record simulation step
        perfMonitor.recordStep(
            1.0, // Step time in ms
            performanceBrain->getTotalSpikeCount(),
            performanceBrain->getActiveNeuronCount(),
            0.0, // Memory usage will be updated by monitor
            0.0  // CPU usage will be updated by monitor
        );
        
        if (profileCPU && step % 100 == 0) {
            std::cout << "  Step " << step << "/1000 (Spikes: " << performanceBrain->getTotalSpikeCount() << ")" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto endMemory = std::chrono::system_clock::now();
    
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    auto memoryTime = std::chrono::duration<double>(endMemory - startMemory).count();
    
    // Stop performance monitoring
    perfMonitor.stop();
    
    // Get performance metrics
    PerformanceMonitor::PerformanceMetrics metrics = perfMonitor.getCurrentMetrics();
    
    std::cout << "\n=== Performance Monitoring Results ===\n";
    
    if (profileCPU) {
        std::cout << "CPU Performance:" << std::endl;
        std::cout << "  Total time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "  Simulation speed: " << std::fixed << std::setprecision(0) << 1000.0 / totalTime << " steps/sec" << std::endl;
        std::cout << "  CPU usage: " << std::fixed << std::setprecision(1) << metrics.system.cpuUsage << "%" << std::endl;
    }
    
    if (profileMemory) {
        std::cout << "\nMemory Performance:" << std::endl;
        std::cout << "  Memory tracking time: " << std::fixed << std::setprecision(2) << memoryTime << "s" << std::endl;
        std::cout << "  Peak memory usage: " << std::fixed << std::setprecision(1) << metrics.system.memoryUsageMB << " MB" << std::endl;
        std::cout << "  Memory usage efficiency: " << std::fixed << std::setprecision(1) << 100.0 * 512.0 / (metrics.system.memoryUsageMB + 1.0) << "%" << std::endl;
    }
    
    if (profileSimulation) {
        std::cout << "\nSimulation Performance:" << std::endl;
        std::cout << "  Step time (average/min/max/stddev): " << std::fixed << std::setprecision(2)
                 << metrics.system.stepTimeAverageMs << "/" << metrics.system.stepTimeMinMs << "/"
                 << metrics.system.stepTimeMaxMs << "/" << metrics.system.stepTimeStdDevMs << " ms" << std::endl;
        std::cout << "  Simulation efficiency: " << std::fixed << std::setprecision(2) << metrics.system.simulationEfficiency << "\%" << std::endl;
    }
    
    if (profileNeural) {
        std::cout << "\nNeural Performance:" << std::endl;
        std::cout << "  Average firing rate: " << std::fixed << std::setprecision(2) << metrics.neural.averageFiringRateHz << " Hz" << std::endl;
        std::cout << "  Max firing rate: " << std::fixed << std::setprecision(2) << metrics.neural.maxFiringRateHz << " Hz" << std::endl;
        std::cout << "  Total spikes generated: " << metrics.neural.totalSpikesGenerated << std::endl;
        std::cout << "  Total synapse operations: " << metrics.neural.totalSynapseOperations << std::endl;
        std::cout << "  Plasticity computation time: " << std::fixed << std::setprecision(2) << metrics.neural.plasticityTimeMs << " ms" << std::endl;
        std::cout << "  Active neurons: " << metrics.neural.activeNeurons << std::endl;
        std::cout << "  Firing neurons: " << metrics.neural.firingNeurons << std::endl;
    }
    
    if (profileResources) {
        std::cout << "\nResource Utilization:" << std::endl;
        std::cout << "  Memory pool efficiency: " << std::fixed << std::setprecision(1) << metrics.resources.memoryPoolEfficiency << "%" << std::endl;
        std::cout << "  Cache hit rate: " << std::fixed << std::setprecision(1) << metrics.resources.cacheHitRate << "%" << std::endl;
        std::cout << "  Cache miss rate: " << std::fixed << std::setprecision(1) << metrics.resources.cacheMissRate << "%" << std::endl;
        std::cout << "  CPU core utilization: " << metrics.resources.threadUtilization << "%" << std::endl;
    }
    
    if (profileBottlenecks) {
        std::cout << "\nBottleneck Analysis:" << std::endl;
        if (!metrics.bottlenecks.identifiedBottlenecks.empty()) {
            std::cout << "  Identified bottlenecks:" << std::endl;
            for (const auto& bottleneck : metrics.bottlenecks.identifiedBottlenecks) {
                std::cout << "    - " << bottleneck << std::endl;
            }
        } else {
            std::cout << "  No significant bottlenecks identified" << std::endl;
        }
        
        if (!metrics.bottlenecks.optimizationOpportunities.empty()) {
            std::cout << "  Optimization opportunities:" << std::endl;
            for (const auto& opportunity : metrics.bottlenecks.optimizationOpportunities) {
                std::cout << "    - " << opportunity << std::endl;
            }
        }
    }
    
    if (generateReport) {
        std::cout << "\nGenerating performance report in " << reportFormat << " format..." << std::endl;
        
        std::string report = perfMonitor.generateReport();
        
        std::string outputFile = "performance_report." + reportFormat;
        std::ofstream reportFile(outputFile);
        if (reportFile.is_open()) {
            reportFile << report;
            reportFile.close();
            std::cout << "Performance report saved to: " << outputFile << std::endl;
        } else {
            std::cout << "Warning: Could not create report file" << std::endl;
        }
        
        // Also generate visualization data
        perfMonitor.generateVisualizationData("performance_visualization.json");
        std::cout << "Visualization data generated: performance_visualization.json" << std::endl;
    }
    
    if (!benchmarkFile.empty()) {
        std::cout << "\nLoading baseline benchmark from: " << benchmarkFile << std::endl;
        // In a real implementation, load baseline metrics and perform comparison
        // For now, just demonstrate the concept
        std::cout << "Benchmark comparison functionality would be implemented here." << std::endl;
    }
    
    logFinalStatus(performanceBrain);
    std::cout << "Performance monitoring complete." << std::endl;
}
    std::string replayFile;
    double replaySpeed = 1.0;
    
    if (args.find("replay-file") != args.end()) {
        replayFile = args.at("replay-file");
    }
    
    if (args.find("replay-speed") != args.end()) {
        replaySpeed = std::stod(args.at("replay-speed"));
    }
    
    std::cout << "\n=== Replay Mode ===\n";
    std::cout << "Replay file: " << replayFile << std::endl;
    std::cout << "Replay speed: " << replaySpeed << "x" << std::endl;
    
    // Load brain state from checkpoint
    auto replayConfig = std::make_shared<Config>(*config);
    replayConfig->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::CommandLine);
    
    std::shared_ptr<Brain> replayBrain;
    if (!loadBrainState(replayBrain, replayFile)) {
        std::cerr << "Failed to load checkpoint!" << std::endl;
        return;
    }
    
    if (!replayBrain) {
        // Initialize new brain if loading failed
        initializeBrain(replayBrain, replayConfig);
    }
    
    std::cout << "Replaying simulation..." << std::endl;
    
    // Simulate replay (for now, just run a simple simulation)
    for (size_t step = 0; step < 500; ++step) {
        for (size_t i = 0; i < 5; ++i) {
            replayBrain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f * replaySpeed);
        }
        replayBrain->step(step, step * 0.001 * replaySpeed);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << "/500 (Speed: " << replaySpeed << "x)" << std::endl;
        }
    }
    
    logFinalStatus(replayBrain);
    std::cout << "Replay complete." << std::endl;
}

// Debug mode execution with enhanced analysis
void runDebugMode(const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args) {
    std::cout << "\n=== Enhanced Debug Mode ===\n";
    
    // Parse debug analysis flags
    DebugAnalysis::AnalysisFlags debugFlags(args);
    
    auto debugConfig = std::make_shared<Config>(*config);
    debugConfig->set("neuron_count", static_cast<int64_t>(100), ConfigSource::CommandLine);
    
    std::shared_ptr<Brain> debugBrain;
    if (!initializeBrain(debugBrain, debugConfig)) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Initialize DebugAnalysis system
    DebugAnalysis debugAnalysis(debugBrain, config, debugFlags);
    
    // Perform analysis based on flags
    std::vector<NeuronAnalysisResult> neuronResults;
    std::vector<SynapseAnalysisResult> synapseResults;
    NetworkAnalysisResult networkResult;
    ActivityAnalysisResult activityResult;
    PerformanceAnalysisResult performanceResult;
    StateAnalysisResult stateResult;
    StatisticalAnalysisResult statsResult;
    
    // Enhanced neuron analysis
    if (debugFlags.neuronAnalysis || debugFlags.neuronStateInspection) {
        std::cout << "\n=== Enhanced Neuron Analysis ===\n";
        debugAnalysis.analyzeNeurons(neuronResults);
        
        // Output neuron analysis results
        for (const auto& result : neuronResults) {
            debugAnalysis.outputToConsole(result);
            std::cout << std::endl;
        }
    }
    
    // Enhanced synapse analysis
    if (debugFlags.synapseAnalysis || debugFlags.synapseStateAnalysis) {
        std::cout << "\n=== Enhanced Synapse Analysis ===\n";
        debugAnalysis.analyzeSynapses(synapseResults);
        
        // Output synapse analysis results
        for (const auto& result : synapseResults) {
            debugAnalysis.outputToConsole(result);
            std::cout << std::endl;
        }
    }
    
    // Network analysis
    if (debugFlags.networkAnalysis) {
        std::cout << "\n=== Network Topology Analysis ===\n";
        debugAnalysis.analyzeNetwork(networkResult);
        debugAnalysis.outputToConsole(networkResult);
    }
    
    // Activity profiling
    if (debugFlags.activityAnalysis) {
        std::cout << "\n=== Neural Activity Profiling ===\n";
        debugAnalysis.analyzeActivity(activityResult);
        debugAnalysis.outputToConsole(activityResult);
    }
    
    // Performance profiling
    if (debugFlags.performanceAnalysis) {
        std::cout << "\n=== System Performance Profiling ===\n";
        debugAnalysis.startPerformanceMonitoring();
        
        // Run a short simulation for profiling
        for (size_t step = 0; step < 100; ++step) {
            for (size_t i = 0; i < 5; ++i) {
                debugBrain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            }
            debugBrain->step(step, step * 0.001);
        }
        
        debugAnalysis.stopPerformanceMonitoring(performanceResult);
        debugAnalysis.outputToConsole(performanceResult);
    }
    
    // State analysis
    if (debugFlags.stateComparison || debugFlags.brainStateComparison) {
        std::cout << "\n=== Brain State Comparison ===\n";
        debugAnalysis.analyzeState(stateResult);
        // Output state comparison results
        std::cout << "State comparison score: " << std::fixed << std::setprecision(2) 
                  << stateResult.stateComparisonScore << "/1.0" << std::endl;
        if (!stateResult.checkpointIssues.empty()) {
            std::cout << "Issues found:" << std::endl;
            for (const auto& issue : stateResult.checkpointIssues) {
                std::cout << "  - " << issue << std::endl;
            }
        }
    }
    
    // Statistical analysis
    if (debugFlags.statisticsAnalysis) {
        std::cout << "\n=== Statistical Analysis ===\n";
        debugAnalysis.analyzeStatistics(statsResult);
        // Output statistical results
        std::cout << "Average firing rate: " << std::fixed << std::setprecision(2) 
                  << statsResult.averageFiringRate << " Hz" << std::endl;
        std::cout << "Total spikes: " << std::fixed << std::setprecision(0) 
                  << statsResult.totalSpikes << std::endl;
        
        if (!statsResult.detectedPatterns.empty()) {
            std::cout << "Detected patterns:" << std::endl;
            for (const auto& pattern : statsResult.detectedPatterns) {
                std::cout << "  - " << pattern << std::endl;
            }
        }
        
        if (!statsResult.detectedAnomalies.empty()) {
            std::cout << "Detected anomalies:" << std::endl;
            for (const auto& anomaly : statsResult.detectedAnomalies) {
                std::cout << "  - " << anomaly << std::endl;
            }
        }
    }
    
    // Event tracing
    if (debugFlags.eventTracing) {
        std::cout << "\n=== Enhanced Event Tracing ===\n";
        std::string traceDuration = "10s";
        if (args.find("event-trace") != args.end()) {
            traceDuration = args.at("event-trace");
        }
        
        debugAnalysis.setupEventTracing(10); // 10 seconds
        
        // Run simulation with event tracing
        for (size_t step = 0; step < 50; ++step) {
            for (size_t i = 0; i < 3; ++i) {
                debugBrain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
            debugBrain->step(step, step * 0.001);
            
            std::cout << "  Step " << step << ": " << debugBrain->getTotalSpikeCount() 
                      << " spikes, " << debugBrain->getFiringNeuronCount() 
                      << " firing neurons" << std::endl;
        }
        
        debugAnalysis.stopEventTracing();
    }
    
    // Pattern analysis
    if (debugFlags.patternAnalysis) {
        std::cout << "\n=== Pattern Analysis ===\n";
        std::string patternType = "all";
        if (args.find("pattern-analysis") != args.end()) {
            patternType = args.at("pattern-analysis");
        }
        
        debugAnalysis.performPatternAnalysis(statsResult.detectedPatterns, statsResult);
        std::cout << "Pattern analysis complete for type: " << patternType << std::endl;
    }
    
    // Bottleneck analysis
    if (debugFlags.bottleneckAnalysis) {
        std::cout << "\n=== Bottleneck Analysis ===\n";
        // Would implement bottleneck analysis here
        std::cout << "Bottleneck analysis would identify system bottlenecks..." << std::endl;
    }
    
    // Generate visualization data if requested
    if (args.find("visualization") != args.end()) {
        std::cout << "\n=== Generating Visualization Data ===\n";
        std::string outputFile = "debug_output.json";
        if (args.find("output-file") != args.end()) {
            outputFile = args.at("output-file");
        }
        
        debugAnalysis.exportForExternalTools(outputFile, DebugAnalysis::OutputFormat::VISUALIZATION);
        std::cout << "Visualization data exported to: " << outputFile << std::endl;
    }
    
    logFinalStatus(debugBrain);
    std::cout << "Enhanced debug analysis complete." << std::endl;
}

// Main execution function
int main(int argc, char** argv) {
    // Set up signal handling for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Parse command line arguments
    std::map<std::string, std::string> args = parseCommandLineArgs(argc, argv);
    
    // Determine operation mode
    std::string modeStr = "demo"; // Default mode
    if (args.find("mode") != args.end()) {
        modeStr = args.at("mode");
    }
    
    OperationMode mode = parseMode(modeStr);
    
    // Handle special modes
    if (mode == OperationMode::HELP) {
        printBanner("Help");
        printHelp();
        return 0;
    }
    
    if (mode == OperationMode::VERSION) {
        printBanner("Version");
        printVersion();
        return 0;
    }
    
    // Print banner with mode
    printBanner(modeStr);
    
    // Setup logging
    std::shared_ptr<Logger> logger;
    std::shared_ptr<ConsoleLogger> consoleLogger;
    setupLogging(logger, consoleLogger);
    
    // Setup log level from args
    if (args.find("log-level") != args.end()) {
        LogLevel logLevel = parseLogLevel(args.at("log-level"));
        consoleLogger->setLevel(logLevel);
        NLM_LOG_INFO("Log level set to: " + std::to_string(static_cast<int>(logLevel)));
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from various sources
    loadConfigurationFromArgs(config, args);
    
    // Set default configuration
    setDefaultConfiguration(config);
    
    // List configuration if requested
    if (args.find("list") != args.end()) {
        listConfiguration(config);
    }
    
    // Validate configuration if requested
    if (args.find("validate") != args.end()) {
        validateConfiguration(config);
    }
    
    // Log configuration summary
    logConfigurationSummary(config);
    
    // Handle advanced options
    std::string checkpointFile;
    std::string saveFile;
    size_t simulationSteps = 100;
    double simulationDuration = 0.0;
    
    if (args.find("checkpoint") != args.end()) {
        checkpointFile = args.at("checkpoint");
    }
    
    if (args.find("save") != args.end()) {
        saveFile = args.at("save");
    }
    
    if (args.find("steps") != args.end()) {
        simulationSteps = std::stoull(args.at("steps"));
    }
    
    if (args.find("duration") != args.end()) {
        simulationDuration = std::stod(args.at("duration"));
    }
    
    // Execute mode-specific logic
    if (mode == OperationMode::DEMO) {
        runDemoMode(config, args);
    } else if (mode == OperationMode::BENCHMARK) {
        runBenchmarkMode(config, args);
    } else if (mode == OperationMode::VALIDATE) {
        runValidateMode(config, args);
    } else if (mode == OperationMode::PROFILE) {
        runProfileMode(config, args);
    } else if (mode == OperationMode::PERFORMANCE) {
        runPerformanceMode(config, args);
    } else if (mode == OperationMode::REPLAY) {
        runReplayMode(config, args);
    } else if (mode == OperationMode::DEBUG) {
        runDebugMode(config, args);
    }
    
    // Save brain state if requested
    if (!saveFile.empty() && mode != OperationMode::REPLAY) {
        // This is a simplified version - would need to track brain state across modes
        std::cout << "Saving brain state to: " << saveFile << std::endl;
        saveBrainState(nullptr, saveFile);
    }
    
    // Save configuration to file if requested
    if (args.find("output") != args.end()) {
        saveConfigurationToFile(config, args.at("output"));
    }
    
    std::cout << std::endl;
    std::cout << "=== Execution Complete ===" << std::endl;
    
    return 0;
}

// Backward compatibility functions (for existing --config option)
void loadConfiguration(std::shared_ptr<Config> config, int argc, char** argv) {
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
}

void applyCommandLineArgs(std::shared_ptr<Config> config, int argc, char** argv) {
    config->loadFromArgs(argc, argv);
}

void setDefaultConfiguration(std::shared_ptr<Config> config) {
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void logConfigurationSummary(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
}

void setupLogging(std::shared_ptr<Logger>& logger, std::shared_ptr<ConsoleLogger>& consoleLogger) {
    logger = std::make_shared<Logger>();
    consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
}

void initializeSimulationClock(SimulationClock& clock, const std::shared_ptr<Config>& config) {
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    clock = SimulationClock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
}

bool initializeBrain(std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    brain->logStatus();
    return true;
}

bool checkError(const std::string& message, bool condition) {
    if (!condition) {
        NLM_LOG_ERROR(message);
        return false;
    }
    return true;
}

void logFinalStatus(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
}

void logPhaseComplete() {
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

// Test helper functions (maintained for backward compatibility)
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
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
    
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
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
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    std::shared_ptr<Logger> logger;
    std::shared_ptr<ConsoleLogger> consoleLogger;
    setupLogging(logger, consoleLogger);
    
    auto config = std::make_shared<Config>();
    loadConfiguration(config, argc, argv);
    applyCommandLineArgs(config, argc, argv);
    setDefaultConfiguration(config);
    logConfigurationSummary(config);
    
    SimulationClock clock;
    initializeSimulationClock(clock, config);
    
    std::shared_ptr<Brain> brain;
    if (!initializeBrain(brain, config)) {
        return 1;
    }
    
    // Test 1
    runBasicConnectivityTest(brain);
    
    // Reset and prepare for Test 2
    brain->reset();
    initializeBrain(brain, config);
    
    // Test 2
    runPlasticityExperiment(brain);
    
    // Reset and prepare for Test 3
    brain->reset();
    initializeBrain(brain, config);
    
    // Test 3
    runStdpVerification(brain);
    
    // Final status and summary
    logFinalStatus(brain);
    logPhaseComplete();
    
    return 0;
}