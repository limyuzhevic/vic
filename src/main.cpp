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
#include "performance/CheckpointSystem.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <set>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>

using namespace nlm;

// Enhanced error handling with context
class NLMError {
public:
    enum class Type {
        ConfigurationError,
        InitializationError,
        PlasticityError,
        MemoryError,
        SimulationError,
        CheckpointError,
        ValidationError,
        SystemError
    };
    
    NLMError(Type type, const std::string& message, const std::string& context = "")
        : type(type), message(message), context(context) {}
    
    Type getType() const { return type; }
    const std::string& getMessage() const { return message; }
    const std::string& getContext() const { return context; }
    
    std::string what() const {
        std::ostringstream oss;
        oss << "[NLM " << static_cast<int>(type) << "]";
        if (!context.empty()) oss << " " << context;
        oss << ": " << message;
        return oss.str();
    }
    
private:
    Type type;
    std::string message;
    std::string context;
};

// Configuration validation
class ConfigValidator {
public:
    static bool validate(const Config& config, std::string& error) {
        error.clear();
        
        // Check required keys
        std::vector<std::string> requiredKeys = {"random_seed", "simulation_timestep", "neuron_count", "region_count"};
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                error = "Missing required configuration key: " + key;
                return false;
            }
        }
        
        // Validate value ranges
        auto seedOpt = config.get<int64_t>("random_seed");
        if (seedOpt && (*seedOpt < 0 || *seedOpt > 2147483647)) {
            error = "Invalid random_seed value: must be between 0 and 2147483647";
            return false;
        }
        
        auto timestepOpt = config.get<double>("simulation_timestep");
        if (timestepOpt && (*timestepOpt <= 0.0 || *timestepOpt > 1.0)) {
            error = "Invalid simulation_timestep: must be positive and <= 1.0";
            return false;
        }
        
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        if (neuronCountOpt && (*neuronCountOpt <= 0 || *neuronCountOpt > 1000000)) {
            error = "Invalid neuron_count: must be positive and <= 1000000";
            return false;
        }
        
        auto regionCountOpt = config.get<int64_t>("region_count");
        if (regionCountOpt && (*regionCountOpt <= 0 || *regionCountOpt > 1000)) {
            error = "Invalid region_count: must be positive and <= 1000";
            return false;
        }
        
        auto connectionProbOpt = config.get<float>("connection_probability");
        if (connectionProbOpt && (*connectionProbOpt < 0.0f || *connectionProbOpt > 1.0f)) {
            error = "Invalid connection_probability: must be between 0.0 and 1.0";
            return false;
        }
        
        return true;
    }
    
    static std::string generateSchema() {
        std::ostringstream oss;
        oss << "# NLM Configuration Schema" << std::endl;
        oss << "# Required keys:" << std::endl;
        oss << "#   random_seed: integer (0-2147483647)" << std::endl;
        oss << "#   simulation_timestep: double (0.0-1.0)" << std::endl;
        oss << "#   neuron_count: integer (1-1000000)" << std::endl;
        oss << "#   region_count: integer (1-1000)" << std::endl;
        oss << std::endl;
        oss << "# Optional keys:" << std::endl;
        oss << "#   connection_probability: float (0.0-1.0, default: 0.1)" << std::endl;
        oss << "#   plasticity_learning_rate: float (default: 0.01)" << std::endl;
        oss << "#   stdp_ltp_weight: float (default: 0.01)" << std::endl;
        oss << "#   stdp_ltd_weight: float (default: 0.012)" << std::endl;
        oss << "#   stdp_time_constant: float (default: 20.0)" << std::endl;
        return oss.str();
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct Metrics {
        double cpuTime = 0.0;
        double wallTime = 0.0;
        size_t memoryUsage = 0;
        size_t memoryPeak = 0;
        size_t memoryAllocated = 0;
        std::vector<double> timestepTimes;
        std::vector<size_t> spikeCounts;
        std::vector<size_t> neuronCounts;
    };
    
    static void startTiming() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    static double stopTiming() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0; // Convert to seconds
    }
    
    static void recordStepMetrics(size_t spikeCount, size_t neuronCount, double timestep) {
        if (!currentMetrics) return;
        currentMetrics->timestepTimes.push_back(timestep);
        currentMetrics->spikeCounts.push_back(spikeCount);
        currentMetrics->neuronCounts.push_back(neuronCount);
    }
    
    static Metrics getMetrics() {
        return currentMetrics ? *currentMetrics : Metrics();
    }
    
    static void reset() {
        currentMetrics = std::make_unique<Metrics>();
    }
    
    static void initialize() {
        currentMetrics = std::make_unique<Metrics>();
        startTiming();
    }
    
private:
    static inline std::unique_ptr<Metrics> currentMetrics;
    static inline std::chrono::high_resolution_clock::time_point startTime;
};

// Command-line option parser
class CommandLineParser {
public:
    struct Options {
        // Basic options
        std::string configFile = "configs/default.cfg";
        bool verbose = false;
        bool quiet = false;
        std::string logLevel = "INFO";
        
        // Power user options
        std::string saveBrain;      // Save brain state with custom name
        std::string loadBrain;      // Load brain state
        std::string brainName;      // Custom brain name for checkpointing
        
        // Test suite options
        std::set<std::string> testSuites;  // {"all", "connectivity", "plasticity", "stdp", "stress", "plasticity-only", "stdp-only", "development"}
        
        // Configuration validation
        bool validateConfig = false;
        
        // Parallel execution
        int parallelThreads = 1;
        bool enableParallel = false;
        std::string parallelMode;   // {"region", "neuron", "synapse"}
        
        // Advanced debugging
        bool enableTrace = false;
        bool enableDebug = false;
        bool enablePerformance = false;
        bool enableMemoryProfile = false;
        std::string debugOutput = ""; // File for debug output
        
        // Performance monitoring
        bool timingEnabled = true;
        bool memoryTrackingEnabled = false;
        bool metricsCollection = false;
        std::string metricsOutput = "";
        
        // Interactive mode
        bool interactiveMode = false;
        bool stepByStep = false;
        bool liveMonitoring = false;
        int maxSteps = 10000;
        
        // Stress testing
        bool stressTest = false;
        int stressTestMultiplier = 10;
        double stressTestDuration = 100.0;
        
        // New test modes
        bool plasticityOnly = false;
        bool stdpOnly = false;
        bool developmentStageTest = false;
        int developmentStage = 0;   // 0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult
        
        // Output options
        bool summaryOnly = false;
        bool jsonOutput = false;
        std::string outputFile = "";
        
        // Safety and validation
        bool validateInputs = true;
        bool autoSave = false;
        int autoSaveInterval = 1000;
    };
    
    static Options parse(int argc, char** argv) {
        Options opts;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                std::exit(0);
            }
            
            // Basic options
            else if (arg == "--verbose" || arg == "-v") {
                opts.verbose = true;
            }
            else if (arg == "--quiet" || arg == "-q") {
                opts.quiet = true;
            }
            else if (arg == "--json-output") {
                opts.jsonOutput = true;
            }
            else if (arg.find("--log-level=") == 0) {
                opts.logLevel = arg.substr(12);
            }
            
            // Power user options
            else if (arg.find("--save-brain=") == 0) {
                opts.saveBrain = arg.substr(13);
            }
            else if (arg.find("--load-brain=") == 0) {
                opts.loadBrain = arg.substr(12);
            }
            else if (arg.find("--brain-name=") == 0) {
                opts.brainName = arg.substr(12);
            }
            
            // Test suite options
            else if (arg.find("--test=") == 0) {
                std::string test = arg.substr(7);
                if (test == "all") {
                    opts.testSuites = {"connectivity", "plasticity", "stdp", "stress", "plasticity-only", "stdp-only", "development"};
                } else {
                    opts.testSuites.insert(test);
                }
            }
            else if (arg.find("--test-suite=") == 0) {
                opts.testSuites.insert(arg.substr(12));
            }
            
            // Configuration validation
            else if (arg == "--validate-config") {
                opts.validateConfig = true;
            }
            
            // Parallel execution
            else if (arg == "--parallel" || arg == "--enable-parallel") {
                opts.enableParallel = true;
            }
            else if (arg.find("--parallel-threads=") == 0) {
                opts.parallelThreads = std::stoi(arg.substr(19));
            }
            else if (arg.find("--parallel-mode=") == 0) {
                opts.parallelMode = arg.substr(16);
            }
            
            // Advanced debugging
            else if (arg == "--trace" || arg == "--enable-trace") {
                opts.enableTrace = true;
            }
            else if (arg == "--debug" || arg == "--enable-debug") {
                opts.enableDebug = true;
            }
            else if (arg == "--performance" || arg == "--enable-performance") {
                opts.enablePerformance = true;
            }
            else if (arg == "--memory-profile" || arg == "--enable-memory-profile") {
                opts.enableMemoryProfile = true;
            }
            else if (arg.find("--debug-output=") == 0) {
                opts.debugOutput = arg.substr(15);
            }
            
            // Performance monitoring
            else if (arg.find("--metrics-output=") == 0) {
                opts.metricsOutput = arg.substr(16);
            }
            
            // Interactive mode
            else if (arg == "--interactive" || arg == "--enable-interactive") {
                opts.interactiveMode = true;
            }
            else if (arg == "--step-by-step" || arg == "--enable-step-by-step") {
                opts.stepByStep = true;
                opts.interactiveMode = true;
            }
            else if (arg == "--live-monitoring" || arg == "--enable-live-monitoring") {
                opts.liveMonitoring = true;
                opts.interactiveMode = true;
            }
            else if (arg.find("--max-steps=") == 0) {
                opts.maxSteps = std::stoi(arg.substr(11));
            }
            
            // Stress testing
            else if (arg == "--stress-test" || arg == "--enable-stress-test") {
                opts.stressTest = true;
            }
            else if (arg.find("--stress-multiplier=") == 0) {
                opts.stressTestMultiplier = std::stoi(arg.substr(20));
            }
            
            // New test modes
            else if (arg == "--plasticity-only") {
                opts.plasticityOnly = true;
            }
            else if (arg == "--stdp-only") {
                opts.stdpOnly = true;
            }
            else if (arg == "--development-stage-test" || arg == "--enable-development-stage-test") {
                opts.developmentStageTest = true;
            }
            else if (arg.find("--development-stage=") == 0) {
                opts.developmentStage = std::stoi(arg.substr(20));
            }
            
            // Config file
            else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    opts.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    opts.configFile = argv[++i];
                }
            }
            
            // Output file
            else if (arg.find("--output=") == 0) {
                opts.outputFile = arg.substr(9);
            }
            else if (arg.find("--output-file=") == 0) {
                opts.outputFile = arg.substr(14);
            }
            
            // Auto save
            else if (arg == "--auto-save" || arg == "--enable-auto-save") {
                opts.autoSave = true;
            }
            else if (arg.find("--auto-save-interval=") == 0) {
                opts.autoSaveInterval = std::stoi(arg.substr(22));
            }
            
            else if (arg.find("--") == 0) {
                std::cerr << "Unknown option: " << arg << std::endl;
                printHelp();
                std::exit(1);
            }
        }
        
        return opts;
    }
    
    static void printHelp() {
        std::cout << "NLM - Neural Learning Machine (Phase 2)" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
        std::cout << "  -q, --quiet             Suppress output" << std::endl;
        std::cout << "  --config=file           Load configuration from file" << std::endl;
        std::cout << "  --json-output           Output results in JSON format" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Power User Options:" << std::endl;
        std::cout << "  --save-brain=name       Save brain state with custom name" << std::endl;
        std::cout << "  --load-brain=name       Load brain state" << std::endl;
        std::cout << "  --brain-name=name       Custom brain name for checkpointing" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Test Suite Options:" << std::endl;
        std::cout << "  --test=all              Run all test suites" << std::endl;
        std::cout << "  --test=connectivity     Run connectivity test only" << std::endl;
        std::cout << "  --test=plasticity       Run plasticity test only" << std::endl;
        std::cout << "  --test=stdp             Run STDP verification only" << std::endl;
        std::cout << "  --test=stress           Run stress test" << std::endl;
        std::cout << "  --test=plasticity-only  Run plasticity-only test" << std::endl;
        std::cout << "  --test=stdp-only        Run STDP-only test" << std::endl;
        std::cout << "  --test=development      Run development stage test" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Configuration Validation:" << std::endl;
        std::cout << "  --validate-config       Validate configuration values" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Parallel Execution:" << std::endl;
        std::cout << "  --parallel              Enable parallel execution" << std::endl;
        std::cout << "  --parallel-threads=N    Number of parallel threads" << std::endl;
        std::cout << "  --parallel-mode=MODE    Parallel execution mode (region|neuron|synapse)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Advanced Debugging:" << std::endl;
        std::cout << "  --trace                 Enable detailed trace output" << std::endl;
        std::cout << "  --debug                 Enable debug mode" << std::endl;
        std::cout << "  --performance           Enable performance monitoring" << std::endl;
        std::cout << "  --memory-profile        Enable memory profiling" << std::endl;
        std::cout << "  --debug-output=file     File for debug output" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Interactive Mode:" << std::endl;
        std::cout << "  --interactive           Enable interactive mode" << std::endl;
        std::cout << "  --step-by-step          Step-by-step execution" << std::endl;
        std::cout << "  --live-monitoring        Enable live monitoring interface" << std::endl;
        std::cout << "  --max-steps=N           Maximum number of steps (default: 10000)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Stress Testing:" << std::endl;
        std::cout << "  --stress-test           Enable stress testing mode" << std::endl;
        std::cout << "  --stress-multiplier=N   Stress test multiplier (default: 10)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "New Test Modes:" << std::endl;
        std::cout << "  --plasticity-only       Plasticity-only test mode" << std::endl;
        std::cout << "  --stdp-only             STDP-only test mode" << std::endl;
        std::cout << "  --development-stage-test Enable development stage testing" << std::endl;
        std::cout << "  --development-stage=N   Development stage (0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Output Options:" << std::endl;
        std::cout << "  --output=file           Output results to file" << std::endl;
        std::cout << "  --output-file=file      Alternative output file option" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Auto Save:" << std::endl;
        std::cout << "  --auto-save             Enable automatic brain checkpointing" << std::endl;
        std::cout << "  --auto-save-interval=N  Auto-save interval in steps (default: 1000)" << std::endl;
        std::cout << std::endl;
    }
};

// Performance logging helper
void logPerformance(const std::string& operation, double time, size_t memory = 0) {
    NLM_LOG_INFO("PERF: " + operation + " took " + std::to_string(time) + "s");
    if (memory > 0) {
        NLM_LOG_INFO("PERF: Memory used: " + std::to_string(memory) + " bytes");
    }
}

// Configuration validation entry point
bool validateConfiguration(const Config& config) {
    std::string error;
    if (!ConfigValidator::validate(config, error)) {
        NLM_LOG_ERROR("Configuration validation failed: " + error);
        NLM_LOG_INFO("Configuration schema:");
        NLM_LOG_INFO(ConfigValidator::generateSchema());
        return false;
    }
    NLM_LOG_INFO("Configuration validation passed");
    return true;
}

// Forward declarations of the test functions
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

// Test mode runner
class TestModeRunner {
public:
    static void runTestSuite(std::shared_ptr<Brain> brain, const CommandLineParser::Options& opts) {
        if (opts.stressTest) {
            runStressTest(brain, opts);
            return;
        }
        
        if (opts.plasticityOnly) {
            runPlasticityOnlyTest(brain, opts);
            return;
        }
        
        if (opts.stdpOnly) {
            runSTDPOnlyTest(brain, opts);
            return;
        }
        
        if (opts.developmentStageTest) {
            runDevelopmentStageTest(brain, opts);
            return;
        }
        
        // Default test suite
        runStandardTestSuite(brain, opts);
    }
    
private:
    static void runStandardTestSuite(std::shared_ptr<Brain> brain, const CommandLineParser::Options& opts) {
        NLM_LOG_INFO("=== Running Standard Test Suite ===");
        
        // Test 1: Basic connectivity
        if (opts.testSuites.count("connectivity") > 0 || opts.testSuites.empty()) {
            NLM_LOG_INFO("--- Test 1: Basic Neural Connectivity ---");
            runBasicConnectivityTest(brain);
        }
        
        // Reset brain between tests
        brain->reset();
        brain->initialize();
        
        // Test 2: Plasticity learning experiment
        if (opts.testSuites.count("plasticity") > 0 || opts.testSuites.empty()) {
            NLM_LOG_INFO("--- Test 2: Plasticity Learning Experiment ---");
            runPlasticityExperiment(brain);
        }
        
        // Reset brain between tests
        brain->reset();
        brain->initialize();
        
        // Test 3: STDP verification
        if (opts.testSuites.count("stdp") > 0 || opts.testSuites.empty()) {
            NLM_LOG_INFO("--- Test 3: STDP Verification ---");
            runStdpVerification(brain);
        }
    }
    
    static void runStressTest(std::shared_ptr<Brain> brain, const CommandLineParser::Options& opts) {
        NLM_LOG_INFO("=== Running Stress Test ===");
        NLM_LOG_INFO("Stress test multiplier: " + std::to_string(opts.stressTestMultiplier));
        
        // Scale up the brain for stress testing
        auto config = brain->getConfig();
        config->set("neuron_count", static_cast<int64_t>(config->getOr<int64_t>("neuron_count", 1000) * opts.stressTestMultiplier), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(config->getOr<int64_t>("region_count", 1) * opts.stressTestMultiplier), ConfigSource::Runtime);
        
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("Running stress test for " + std::to_string(opts.stressTestDuration) + " seconds...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        SimulationStep stepsToRun = static_cast<SimulationStep>(opts.stressTestDuration / config->getOr<double>("simulation_timestep", 0.001));
        
        for (SimulationStep step = 0; step < stepsToRun; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Stress test step " + std::to_string(step) + "/" + std::to_string(stepsToRun) +
                           " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Neurons active: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        NLM_LOG_INFO("=== Stress Test Complete ===");
        NLM_LOG_INFO("Duration: " + std::to_string(duration.count() / 1000000.0) + " seconds");
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Final neurons active: " + std::to_string(brain->getActiveNeuronCount()));
    }
    
    static void runPlasticityOnlyTest(std::shared_ptr<Brain> brain, const CommandLineParser::Options& opts) {
        NLM_LOG_INFO("=== Plasticity-Only Test Mode ===");
        
        // Enable all plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
        
        LearningExperiment experiment(brain, 42);
        experiment.recordInitialState();
        
        NLM_LOG_INFO("Applying intensive plasticity learning (10,000 steps)...");
        
        for (SimulationStep step = 0; step < 10000; ++step) {
            for (size_t i = 0; i < 50 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f + (i % 5) * 10.0f);
            }
            
            brain->step(step, step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
            
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Plasticity test step " + std::to_string(step) +
                           " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
    }
    
    static void runSTDPOnlyTest(std::shared_ptr<Brain> brain, const CommandLineParser::Options& opts) {
        NLM_LOG_INFO("=== STDP-Only Test Mode ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found for STDP test");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("Testing STDP on 5 synapses in detail mode...");
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]) +
                        " type: " + std::to_string(static_cast<int>(synapses[i]->getType())));
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Creating correlated pre->post activity for STDP potentiation...");
        
        for (int trial = 0; trial < 100; ++trial) {
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                preNeuron->injectCurrent(80.0f);
                brain->step(trial * 2, trial * 2 * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                
                postNeuron->injectCurrent(80.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
            }
        }
        
        NLM_LOG_INFO("After correlated activity (STDP results):");
        float totalDelta = 0.0f;
        size_t strengthened = 0, weakened = 0, unchanged = 0;
        
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            
            if (std::abs(delta) < 0.001f) unchanged++;
            else if (delta > 0.001f) strengthened++;
            else if (delta < -0.001f) weakened++;
            
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " Δ=" + std::to_string(delta) +
                        " (before=" + std::to_string(beforeWeights[i]) +
                        ", after=" + std::to_string(synapses[i]->getWeight()) + ")");
        }
        
        NLM_LOG_INFO("STDP Analysis:");
        NLM_LOG_INFO("  Total Δ: " + std::to_string(totalDelta));
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Strong potentiation detected");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Possible LTD dominance");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering as expected");
        }
    }
    
    static void runDevelopmentStageTest(std::shared_ptr<Brain> brain, const CommandLineParser::Options& opts) {
        NLM_LOG_INFO("=== Development Stage Test Mode ===");
        
        DevelopmentalStage stage;
        switch (opts.developmentStage) {
            case 0: stage = DevelopmentalStage::Initial; break;
            case 1: stage = DevelopmentalStage::CriticalPeriod; break;
            case 2: stage = DevelopmentalStage::Maturation; break;
            case 3: stage = DevelopmentalStage::Adult; break;
            default: stage = DevelopmentalStage::Initial;
        }
        
        brain->setDevelopmentalStage(stage);
        NLM_LOG_INFO("Development stage set to: " + std::to_string(static_cast<int>(stage)));
        
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("Running plasticity experiments across developmental stages...");
        
        for (int testStep = 0; testStep < 1000; ++testStep) {
            brain->updatePlasticity();
            
            if (testStep % 100 == 0) {
                brain->develop();
                NLM_LOG_INFO("Development update at step " + std::to_string(testStep) +
                           " (stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())) + ")");
            }
            
            if (testStep % 200 == 0) {
                auto* sp = brain->getStructuralPlasticity();
                if (sp) {
                    NLM_LOG_INFO("Synaptogenesis rate: " + std::to_string(sp->getSynaptogenesisRate()) +
                               ", Pruning rate: " + std::to_string(sp->getPruningRate()));
                }
            }
        }
        
        NLM_LOG_INFO("=== Development Stage Test Complete ===");
        NLM_LOG_INFO("Final developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    }
};

// Default LearningExperiment struct (copied from original main.cpp)
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
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
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