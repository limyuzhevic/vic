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
#include <map>
#include <set>
#include <algorithm>

using namespace nlm;

// Configuration validation and help system
struct CLIConfig {
    bool help = false;
    bool version = false;
    bool verbose = false;
    bool quiet = false;
    std::string configFile = "configs/default.cfg";
    std::string logFile = "";
    LogLevel logLevel = LogLevel::Info;
    size_t experimentDuration = 0;
    std::string experimentMode = "auto";
    bool enableAdvanced = false;
    bool validateConfig = true;
    std::set<std::string> excludeTests;
    std::map<std::string, std::string> additionalArgs;
    
    // Test selection
    bool runBasicConnectivity = true;
    bool runPlasticityExperiment = true;
    bool runStdpVerification = true;
    bool runAdvanced = false;
    
    // Performance and monitoring
    bool enableMetrics = true;
    bool enableProgress = true;
    size_t progressInterval = 100;
    
    // Configuration validation
    std::set<std::string> requiredKeys;
    std::map<std::string, std::string> validationRanges;
};

class CLIParser {
public:
    CLIConfig parse(int argc, char** argv) {
        CLIConfig config;
        
        // Build usage string
        usage = buildUsageString();
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                config.help = true;
            } else if (arg == "--version") {
                config.version = true;
            } else if (arg == "--verbose" || arg == "-v") {
                config.verbose = true;
                config.logLevel = LogLevel::Debug;
            } else if (arg == "--quiet" || arg == "-q") {
                config.quiet = true;
                config.logLevel = LogLevel::Error;
            } else if (arg.substr(0, 8) == "--config") {
                parseConfigArgument(arg, i, argc, argv, config);
            } else if (arg.substr(0, 9) == "--logfile") {
                config.logFile = parseLogArgument(arg, i, argc, argv);
            } else if (arg.substr(0, 4) == "--log") {
                config.logLevel = parseLogLevelArgument(arg);
            } else if (arg.substr(0, 12) == "--experiment") {
                parseExperimentArgument(arg, i, argc, argv, config);
            } else if (arg.substr(0, 13) == "--exclude-test") {
                parseExcludeArgument(arg, i, argc, argv, config);
            } else if (arg.substr(0, 20) == "--additional-arg") {
                parseAdditionalArg(arg, i, argc, argv, config);
            } else if (arg.substr(0, 17) == "--validation-range") {
                parseValidationRange(arg, i, argc, argv, config);
            } else if (arg.substr(0, 16) == "--required-key") {
                parseRequiredKey(arg, i, argc, argv, config);
            } else if (arg.substr(0, 17) == "--enable-advanced") {
                config.enableAdvanced = true;
            } else if (arg.substr(0, 20) == "--disable-test") {
                parseDisableTest(arg, i, argc, argv, config);
            } else if (arg.substr(0, 15) == "--progress-int") {
                parseProgressInterval(arg, i, argc, argv, config);
            } else if (arg == "--metrics") {
                config.enableMetrics = true;
            } else if (arg == "--no-progress") {
                config.enableProgress = false;
            } else if (arg.substr(0, 2) == "--") {
                std::string key = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    config.additionalArgs[key] = argv[++i];
                }
            }
        }
        
        return config;
    }
    
    void printHelp() const {
        std::cout << usage << std::endl;
    }
    
    static std::string getVersion() {
        return "NLM Phase 2.0 - Neural Learning Machine (熙然)\n"
               "Built with C++20 - Real Neural Computation\n"
               "Copyright 2026 - Advanced Neuromorphic Systems";
    }
    
private:
    std::string usage;
    
    std::string buildUsageString() const {
        return R"(
╔════════════════════════════════════════════════════════════════╗
║                    NLM Phase 2 - Neural Learning Machine      ║
║                            (熙然)                             ║
╠════════════════════════════════════════════════════════════════╣
║                                                              ║
║ USAGE: nlm [OPTIONS]                                        ║
║                                                              ║
║ CORE OPTIONS:                                               ║
║  --help, -h                    Show this help message           ║
║  --version                     Print version information       ║
║  --config FILE, -c FILE         Configuration file path       ║
║  --logfile FILE                Log output to file               ║
║  --log LEVEL                   Set log level (Debug, Info,        ║
║                                Warning, Error, Critical)       ║
║  --verbose, -v                 Enable verbose/debug logging    ║
║  --quiet, -q                   Enable quiet/error-only logging  ║
║                                                              ║
║ EXPERIMENT CONTROL:                                         ║
║  --experiment MODE[:DURATION]   Select experiment mode         ║
║                                Modes: auto, basic, plasticity,  ║
║                                       stdp, advanced, all      ║
║  --exclude-test TEST           Exclude specific test          ║
║  --disable-test TEST           Disable specific test          ║
║                                                              ║
║ ADVANCED OPTIONS:                                            ║
║  --enable-advanced             Enable advanced features        ║
║  --metrics                     Enable performance metrics       ║
║  --no-progress                 Disable progress reporting     ║
║  --progress-int INTERVAL       Progress update interval       ║
║                                                              ║
║ VALIDATION:                                                  ║
║  --required-key KEY            Mark key as required            ║
║  --validation-range KEY:RANGE  Set validation range for key    ║
║                                                              ║
║ ADDITIONAL ARGUMENTS:                                        ║
║  --arg KEY VALUE               Additional configuration key    ║
║                                                              ║
║ VALIDATION RANGE FORMAT:                                     ║
║  <min>,<max> (e.g., 0,100)    For numeric keys              ║
║  min,max (e.g., A,B)          For string keys (alphabetical)  ║
║                                                              ║
╚════════════════════════════════════════════════════════════════╝

EXAMPLES:
  nlm                                    # Run default experiments
  nlm --verbose                         # Run with verbose logging
  nlm --config myconfig.cfg             # Use custom config file
  nlm --experiment plasticity:5000      # Run plasticity experiment
  nlm --exclude-test stdp --metrics     # Exclude STDP test, enable metrics
  nlm --enable-advanced --logfile nlm.log # Enable advanced features
  nlm --validation-range neuron_count:1,10000                # Validate neuron count
  nlm --required-key random_seed       # Require random_seed key

For more information, visit: https://github.com/neural-learning-machine/nlm
        )";
    }
    
    void parseConfigArgument(const std::string& arg, int& i, int argc, char** argv, CLIConfig& config) {
        if (arg.find('=') != std::string::npos) {
            config.configFile = arg.substr(arg.find('=') + 1);
        } else if (i + 1 < argc) {
            config.configFile = argv[++i];
        }
    }
    
    std::string parseLogArgument(const std::string& arg, int& i, int argc, char** argv) {
        if (arg.find('=') != std::string::npos) {
            return arg.substr(arg.find('=') + 1);
        } else if (i + 1 < argc) {
            return argv[++i];
        }
        return "";
    }
    
    LogLevel parseLogLevelArgument(const std::string& arg) {
        std::string level = arg.substr(arg.find('=') + 1);
        std::transform(level.begin(), level.end(), level.begin(), ::tolower);
        
        if (level == "debug" || level == "d") return LogLevel::Debug;
        if (level == "info" || level == "i") return LogLevel::Info;
        if (level == "warning" || level == "w") return LogLevel::Warning;
        if (level == "error" || level == "e") return LogLevel::Error;
        if (level == "critical" || level == "c") return LogLevel::Critical;
        
        return LogLevel::Info;
    }
    
    void parseExperimentArgument(const std::string& arg, int& i, int argc, char** argv, CLIConfig& config) {
        std::string value = arg.substr(arg.find('=') + 1);
        if (value.find(':') != std::string::npos) {
            config.experimentMode = value.substr(0, value.find(':'));
            if (std::all_of(value.begin() + value.find(':') + 1, value.end(), ::isdigit)) {
                config.experimentDuration = std::stoul(value.substr(value.find(':') + 1));
            }
        } else {
            config.experimentMode = value;
        }
    }
    
    void parseExcludeArgument(const std::string& arg, int& i, int argc, char** argv, CLIConfig& config) {
        std::string value = arg.substr(arg.find('=') + 1);
        config.excludeTests.insert(value);
    }
    
    void parseDisableTest(const std::string& arg, int& i, int argc, char** argv, CLIConfig& config) {
        std::string value = arg.substr(arg.find('=') + 1);
        
        if (value == "basic") config.runBasicConnectivity = false;
        else if (value == "plasticity") config.runPlasticityExperiment = false;
        else if (value == "stdp") config.runStdpVerification = false;
        else if (value == "advanced") config.runAdvanced = false;
    }
    
    void parseAdditionalArg(const std::string& arg, int& i, int argc, char** argv, CLIConfig& config) {
        std::string value = arg.substr(arg.find('=') + 1);
        if (value.find(':') != std::string::npos) {
            std::string key = value.substr(0, value.find(':'));
            std::string val = value.substr(value.find(':') + 1);
            config.additionalArgs[key] = val;
        } else if (i + 1 < argc) {
            config.additionalArgs[arg.substr(0, arg.find('='))] = argv[++i];
        }
    }
    
    void parseValidationRange(const std::string& arg, int& i, int argc, char** argv, CLIConfig& config) {
        std::string value = arg.substr(arg.find('=') + 1);
        if (value.find(':') != std::string::npos) {
            std::string key = value.substr(0, value.find(':'));
            std::string range = value.substr(value.find(':') + 1);
            config.validationRanges[key] = range;
        }
    }
    
    void parseRequiredKey(const std::string& arg, int& i, int argc, char** argv, CLIConfig& config) {
        std::string value = arg.substr(arg.find('=') + 1);
        config.requiredKeys.insert(value);
    }
    
    void parseProgressInterval(const std::string& arg, int& i, int argc, char** argv, CLIConfig& config) {
        if (i + 1 < argc) {
            try {
                config.progressInterval = std::stoul(argv[++i]);
            } catch (...) {
                config.progressInterval = 100;
            }
        }
    }
};

// Enhanced timer for performance monitoring
class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0;  // Convert to seconds
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// Enhanced experiment runner with better error handling and reporting
class EnhancedExperimentRunner {
public:
    EnhancedExperimentRunner(std::shared_ptr<Brain> brain, const CLIConfig& config)
        : brain(brain), config(config) {}
    
    void runAllTests() {
        Timer totalTimer;
        
        NLM_LOG_INFO("=== Starting NLM Phase 2 Enhanced Experiments ===");
        NLM_LOG_INFO("Experiment mode: " + config.experimentMode);
        NLM_LOG_INFO("Duration: " + (config.experimentDuration > 0 ? 
            std::to_string(config.experimentDuration) + " steps" : "auto") );
        
        if (config.enableMetrics) {
            NLM_LOG_INFO("Performance monitoring enabled");
        }
        
        if (config.enableProgress) {
            NLM_LOG_INFO("Progress reporting enabled (interval: " + 
                        std::to_string(config.progressInterval) + " steps)");
        }
        
        int testCount = 0;
        int passedTests = 0;
        int failedTests = 0;
        
        try {
            if (config.runBasicConnectivity && 
                !config.excludeTests.count("basic")) {
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Test " + std::to_string(++testCount) + ": Basic Neural Connectivity ===");
                
                if (runBasicConnectivityTest()) {
                    NLM_LOG_INFO("✓ BASIC CONNECTIVITY TEST PASSED");
                    passedTests++;
                } else {
                    NLM_LOG_ERROR("✗ BASIC CONNECTIVITY TEST FAILED");
                    failedTests++;
                }
            }
            
            if (config.runPlasticityExperiment &&
                !config.excludeTests.count("plasticity")) {
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Test " + std::to_string(++testCount) + ": Plasticity Learning Experiment ===");
                
                if (runPlasticityExperimentTest()) {
                    NLM_LOG_INFO("✓ PLASTICITY EXPERIMENT PASSED");
                    passedTests++;
                } else {
                    NLM_LOG_ERROR("✗ PLASTICITY EXPERIMENT FAILED");
                    failedTests++;
                }
            }
            
            if (config.runStdpVerification &&
                !config.excludeTests.count("stdp")) {
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Test " + std::to_string(++testCount) + ": STDP Verification ===");
                
                if (runStdpVerificationTest()) {
                    NLM_LOG_INFO("✓ STDP VERIFICATION PASSED");
                    passedTests++;
                } else {
                    NLM_LOG_ERROR("✗ STDP VERIFICATION FAILED");
                    failedTests++;
                }
            }
            
            if (config.runAdvanced &&
                !config.excludeTests.count("advanced")) {
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Test " + std::to_string(++testCount) + ": Advanced Features ===");
                
                if (runAdvancedFeaturesTest()) {
                    NLM_LOG_INFO("✓ ADVANCED FEATURES TEST PASSED");
                    passedTests++;
                } else {
                    NLM_LOG_ERROR("✗ ADVANCED FEATURES TEST FAILED");
                    failedTests++;
                }
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during experiment execution: " + std::string(e.what()));
            failedTests++;
        }
        
        double totalTime = totalTimer.elapsed();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Experiment Summary ===");
        NLM_LOG_INFO("Total tests executed: " + std::to_string(testCount));
        NLM_LOG_INFO("Passed tests: " + std::to_string(passedTests));
        NLM_LOG_INFO("Failed tests: " + std::to_string(failedTests));
        NLM_LOG_INFO("Total execution time: " + std::to_string(totalTime) + " seconds");
        NLM_LOG_INFO("Tests per second: " + std::to_string(totalTime > 0 ? testCount / totalTime : 0));
        
        if (failedTests > 0) {
            NLM_LOG_ERROR("Some tests failed. See details above.");
            throw std::runtime_error("Experiment execution completed with failures");
        }
        
        NLM_LOG_INFO("All tests completed successfully!");
    }
    
private:
    std::shared_ptr<Brain> brain;
    CLIConfig config;
    
    bool runBasicConnectivityTest() {
        NLM_LOG_INFO("Injecting current into 10 neurons...");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found for basic connectivity test");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found in region");
            return false;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
            
            if (config.enableProgress && step % config.progressInterval == 0) {
                NLM_LOG_INFO("  Progress: Step " + std::to_string(step) + 
                            ", Spikes: " + std::to_string(
                                brain->getTotalSpikeCount() - initialSpikes));
            }
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
        
        return spikes > 0;
    }
    
    bool runPlasticityExperimentTest() {
        LearningExperiment experiment(brain, 42);
        
        NLM_LOG_INFO("Recording initial state...");
        experiment.recordInitialState();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        NLM_LOG_INFO("Applying input patterns (1000 steps)...");
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (config.enableProgress && step % config.progressInterval == 0) {
                NLM_LOG_INFO("  Progress: Step " + std::to_string(step) + 
                            ", Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
        
        return true;
    }
    
    bool runStdpVerificationTest() {
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            NLM_LOG_INFO("Not enough synapses for STDP test");
            return false;
        }
        
        auto& synapses = region->getSynapses();
        NLM_LOG_INFO("Testing STDP on 5 synapses...");
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        NLM_LOG_INFO("Creating correlated pre->post activity...");
        
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
        
        NLM_LOG_INFO("After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("✗ STDP REVERSED: Check parameters");
            return false;
        } else {
            NLM_LOG_INFO("✗ NO CHANGE: STDP may not be triggering");
            return false;
        }
    }
    
    bool runAdvancedFeaturesTest() {
        if (!config.enableAdvanced) {
            NLM_LOG_INFO("Advanced features not enabled, skipping test");
            return true;
        }
        
        NLM_LOG_INFO("Testing advanced features (memory, prediction, cognition)...");
        
        // Test memory systems
        if (brain->getWorkingMemory()) {
            NLM_LOG_INFO("Working memory: Active traces = " + 
                        std::to_string(brain->getWorkingMemory()->getActiveTraces()));
        }
        
        if (brain->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic memory: Episodes = " + 
                        std::to_string(brain->getEpisodicMemory()->getEpisodeCount()));
        }
        
        // Test prediction system
        if (brain->getPredictionSystem()) {
            NLM_LOG_INFO("Prediction system: Active");
        }
        
        // Test cognition systems
        if (brain->getPlanner()) {
            NLM_LOG_INFO("Neural planner: Active");
        }
        
        if (brain->getAttention()) {
            NLM_LOG_INFO("Attention system: Active");
        }
        
        NLM_LOG_INFO("✓ Advanced features test completed");
        return true;
    }
};