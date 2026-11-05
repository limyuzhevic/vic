// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced modular architecture with improved usability and extensibility

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
#include <unordered_map>
#include <functional>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace nlm;

// =============================================================================
// Command Line Interface
// =============================================================================

enum class ExecutionMode {
    Phase2BasicConnectivity,
    Phase2Plasticity,
    Phase2STDP,
    Phase2Complete,
    Phase6Integration,
    Benchmark,
    Help,
    Version
};

struct CommandLineArgs {
    ExecutionMode mode;
    std::string configFile;
    int64_t randomSeed;
    int64_t neuronCount;
    int64_t maxSteps;
    double timestep;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    bool verboseLogging;
    std::string checkpointPath;
    int logLevel;
    std::string outputFile;
    std::string metricsFile;
    bool quietMode;
    bool autoMode;
    int threadCount;
    bool dryRun;
};

class CommandLineParser {
private:
    static std::unordered_map<std::string, ExecutionMode> modeMap;
    
public:
    static CommandLineArgs parseArgs(int argc, char** argv) {
        CommandLineArgs args;
        
        // Set defaults
        args.mode = ExecutionMode::Phase2Complete;
        args.configFile = "configs/default.cfg";
        args.randomSeed = 42;
        args.neuronCount = 500;
        args.maxSteps = 2000;
        args.timestep = 0.001;
        args.enableCheckpointing = false;
        args.enableReplay = false;
        args.enableDevelopment = false;
        args.verboseLogging = false;
        args.checkpointPath = "/tmp/nlm_checkpoint.bin";
        args.logLevel = 3;
        args.outputFile = "";
        args.metricsFile = "";
        args.quietMode = false;
        args.autoMode = false;
        args.threadCount = std::thread::hardware_concurrency();
        args.dryRun = false;
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.mode = ExecutionMode::Help;
                return args;
            }
            else if (arg == "--version") {
                args.mode = ExecutionMode::Version;
                return args;
            }
            else if (arg == "--mode") {
                if (i + 1 < argc) args.mode = parseExecutionMode(argv[++i]);
            }
            else if (arg == "--config") {
                if (i + 1 < argc) args.configFile = argv[++i];
            }
            else if (arg == "--seed") {
                if (i + 1 < argc) args.randomSeed = std::stoll(argv[++i]);
            }
            else if (arg == "--neurons") {
                if (i + 1 < argc) args.neuronCount = std::stoll(argv[++i]);
            }
            else if (arg == "--steps") {
                if (i + 1 < argc) args.maxSteps = std::stoll(argv[++i]);
            }
            else if (arg == "--timestep") {
                if (i + 1 < argc) args.timestep = std::stod(argv[++i]);
            }
            else if (arg == "--checkpoint") {
                if (i + 1 < argc) args.checkpointPath = argv[++i];
            }
            else if (arg == "--checkpoint-enable") {
                args.enableCheckpointing = true;
            }
            else if (arg == "--replay-enable") {
                args.enableReplay = true;
            }
            else if (arg == "--development-enable") {
                args.enableDevelopment = true;
            }
            else if (arg == "--verbose") {
                args.verboseLogging = true;
                args.logLevel = 4;
            }
            else if (arg == "--quiet") {
                args.quietMode = true;
                args.logLevel = 1;
            }
            else if (arg == "--auto") {
                args.autoMode = true;
            }
            else if (arg == "--output") {
                if (i + 1 < argc) args.outputFile = argv[++i];
            }
            else if (arg == "--metrics") {
                if (i + 1 < argc) args.metricsFile = argv[++i];
            }
            else if (arg == "--dry-run") {
                args.dryRun = true;
            }
            else if (arg == "--threads") {
                if (i + 1 < argc) args.threadCount = std::stoi(argv[++i]);
            }
            else if (arg == "--benchmark") {
                args.mode = ExecutionMode::Benchmark;
            }
            else {
                if (arg == "--phase2-basic") args.mode = ExecutionMode::Phase2BasicConnectivity;
                else if (arg == "--phase2-plasticity") args.mode = ExecutionMode::Phase2Plasticity;
                else if (arg == "--phase2-stdp") args.mode = ExecutionMode::Phase2STDP;
                else if (arg == "--phase2-complete") args.mode = ExecutionMode::Phase2Complete;
                else if (arg == "--phase6") args.mode = ExecutionMode::Phase6Integration;
                else if (arg == "--dry-run") args.dryRun = true;
                else {
                    std::cerr << "Unknown argument: " << arg << std::endl;
                    printHelp();
                    exit(1);
                }
            }
        }
        
        return args;
    }
    
    static ExecutionMode parseExecutionMode(const std::string& modeStr) {
        static const std::unordered_map<std::string, ExecutionMode> modeMap = {
            {"phase2-basic", ExecutionMode::Phase2BasicConnectivity},
            {"phase2-plasticity", ExecutionMode::Phase2Plasticity},
            {"phase2-stdp", ExecutionMode::Phase2STDP},
            {"phase2-complete", ExecutionMode::Phase2Complete},
            {"phase6", ExecutionMode::Phase6Integration},
            {"benchmark", ExecutionMode::Benchmark},
            {"help", ExecutionMode::Help},
            {"version", ExecutionMode::Version}
        };
        
        auto it = modeMap.find(modeStr);
        return (it != modeMap.end()) ? it->second : ExecutionMode::Phase2Complete;
    }
    
    static void printHelp() {
        std::cout << R"(
NLM (熙然) - Neural Learning Machine
Usage: ./nlm [OPTIONS]

Execution Modes:
  --phase2-basic           Run basic connectivity test only
  --phase2-plasticity     Run plasticity learning experiment  
  --phase2-stdp           Run STDP verification test
  --phase2-complete        Run all Phase 2 tests (default)
  --phase6                Run Phase 6 integration test
  --benchmark             Run comprehensive benchmark tests

Configuration Options:
  --config <file>          Configuration file (default: configs/default.cfg)
  --seed <number>          Random seed (default: 42)
  --neurons <number>       Neuron count (default: 500)
  --steps <number>         Max simulation steps (default: 2000)
  --timestep <number>      Simulation timestep (default: 0.001)

Feature Flags:
  --checkpoint-enable      Enable checkpointing
  --replay-enable          Enable memory replay
  --development-enable     Enable developmental system
  --verbose               Enable verbose logging
  --quiet                 Suppress non-error output
  --auto                  Automatic mode with progress bars
  --dry-run                Validate configuration without running
  --threads <number>       Number of threads for parallel processing

Output Options:
  --output <file>          Save output to file
  --metrics <file>         Save performance metrics to file

Examples:
  ./nlm --phase2-complete --seed 123 --neurons 1000
  ./nlm --phase6 --config my_config.cfg --checkpoint-enable
  ./nlm --benchmark --auto --output benchmark_results.json
  ./nlm --dry-run --config test.cfg

For more information, visit: https://github.com/nlm-project/nlm

----- End of Help -----

;) << std::endl;
    }
    
    static void printVersion() {
        std::cout << "NLM Version 2.0 Enhanced\n";
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
        std::cout << "Architecture: Modular neural computation with advanced plasticity\n";
        std::cout << "Features: Real LIF neurons, STDP, Hebbian, structural plasticity\n";
        std::cout << "Integrations: Memory systems, neuromodulation, prediction, development\n";
    }
};

class NLMConfigManager {
private:
    std::shared_ptr<Config> config;
    std::string configSource;
    bool configLoaded;
    
public:
    NLMConfigManager() : config(std::make_shared<Config>()), configSource("default"), configLoaded(false) {}
    
    bool loadConfiguration(const CommandLineArgs& args) {
        bool success = true;
        
        if (args.configFile != "configs/default.cfg") {
            if (config->loadFromFile(args.configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
                configSource = args.configFile;
                configLoaded = true;
            } else {
                NLM_LOG_WARNING("Could not load configuration from: " + args.configFile);
                success = false;
            }
        }
        
        // Override with command line arguments
        if (args.randomSeed != 42) {
            config->set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::CommandLine);
        }
        if (args.neuronCount != 500) {
            config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
        }
        if (args.maxSteps != 2000) {
            config->set("max_steps", static_cast<int64_t>(args.maxSteps), ConfigSource::CommandLine);
        }
        if (args.timestep != 0.001) {
            config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
        }
        
        // Apply defaults
        config->set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::Default);
        config->set("simulation_timestep", args.timestep, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        return success;
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
    bool isConfigLoaded() const { return configLoaded; }
};

class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::unordered_map<std::string, double> metrics;
    
public:
    void start() { startTime = std::chrono::high_resolution_clock::now(); }
    
    void record(const std::string& name, double value) { metrics[name] = value; }
    
    void printReport() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "\n=== Performance Report ===" << std::endl;
        std::cout << "Total execution time: " << duration << "s" << std::endl;
        for (const auto& pair : metrics) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    }
};

class TestFramework {
private:
    std::vector<std::pair<std::string, bool>> testResults;
    bool verboseMode;
    
public:
    TestFramework(bool verbose = false) : verboseMode(verbose) {}
    
    bool runTest(const std::string& name, std::function<bool()> testFunc) {
        bool passed = testFunc();
        testResults.push_back({name, passed});
        if (verboseMode || !passed) {
            std::cout << "Test " << name << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
        }
        return passed;
    }
    
    void printResults() const {
        std::cout << "\n=== Test Results ===" << std::endl;
        int passed = 0;
        for (const auto& test : testResults) {
            std::cout << test.first << ": " << (test.second ? "PASSED" : "FAILED") << std::endl;
            if (test.second) passed++;
        }
        std::cout << "Passed: " << passed << "/" << testResults.size() << std::endl;
    }
};

// Phase 2 Test Functions
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
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
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    std::vector<float> initialWeights;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    float initialMeanWeight = 0.0f;
    if (!initialWeights.empty()) {
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        initialMeanWeight = sum / initialWeights.size();
    }
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    std::vector<float> finalWeights;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    float finalMeanWeight = 0.0f;
    if (!finalWeights.empty()) {
        float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        finalMeanWeight = sum / finalWeights.size();
    }
    
    float weightChange = finalMeanWeight - initialMeanWeight;
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMeanWeight));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMeanWeight));
    NLM_LOG_INFO("  Weight change: " + std::to_string(weightChange));
    
    if (std::abs(weightChange) > 0.001f) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 简直 Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);
    float beforeWeight = syn->getWeight();
    
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, (trial * 2) * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, ((trial * 2) + 1) * 0.001);
    }
    
    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                " (Δ=" + std::to_string(delta) + ")");
    
    if (delta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (delta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    CommandLineArgs args = CommandLineParser::parseArgs(argc, argv);
    
    // Handle special modes
    if (args.mode == ExecutionMode::Help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    if (args.mode == ExecutionMode::Version) {
        CommandLineParser::printVersion();
        return 0;
    }
    
    // Initialize configuration manager
    NLMConfigManager configManager;
    if (!configManager.loadConfiguration(args)) {
        NLM_LOG_WARNING("Using default configuration");
    }
    
    auto config = configManager.getConfig();
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", args.timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Log configuration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Performance monitoring
    PerformanceMonitor perfMonitor;
    perfMonitor.start();
    
    // Run tests based on mode
    TestFramework testFramework(args.verboseLogging);
    bool allPassed = true;
    
    switch (args.mode) {
        case ExecutionMode::Phase2BasicConnectivity:
            runBasicConnectivityTest(brain);
            break;
        case ExecutionMode::Phase2Plasticity:
            runPlasticityExperiment(brain);
            break;
        case ExecutionMode::Phase2STDP:
            runStdpVerification(brain);
            break;
        case ExecutionMode::Phase2Complete:
            runBasicConnectivityTest(brain);
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
            break;
        default:
            NLM_LOG_ERROR("Unsupported execution mode");
            return 1;
    }
    
    // Log final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    
    testFramework.printResults();
    perfMonitor.printReport();
    
    return 0;
}
