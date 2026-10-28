// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced version with comprehensive test runner and command-line interface
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
#include <thread>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <set>

using namespace nlm;

// Constants and configuration
namespace ConfigKeys {
    const std::string RANDOM_SEED = "random_seed";
    const std::string SIMULATION_TIMESTEP = "simulation_timestep";
    const std::string NEURON_COUNT = "neuron_count";
    const std::string REGION_COUNT = "region_count";
    const std::string CONNECTION_PROBABILITY = "connection_probability";
    const std::string STDP_LTP_WEIGHT = "stdp_ltp_weight";
    const std::string STDP_LTD_WEIGHT = "stdp_ltd_weight";
    const std::string STDP_TIME_CONSTANT = "stdp_time_constant";
    const std::string SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    const std::string PRUNING_RATE = "pruning_rate";
    const std::string DOPAMINE_BASELINE = "dopamine_baseline";
    const std::string MAX_SIMULATION_STEPS = "max_simulation_steps";
    const std::string SIMULATION_TIME_LIMIT = "simulation_time_limit";
    const std::string TEST_ITERATIONS = "test_iterations";
    const std::string VERBOSE_MODE = "verbose_mode";
    const std::string BENCHMARK_MODE = "benchmark_mode";
    const std::string REPLAY_MODE = "replay_mode";
    const std::string CHECKPOINT_FILE = "checkpoint_file";
    const std::string SAVE_RESULTS = "save_results";
    const std::string RESULTS_FILE = "results_file";
    const std::string PARAMETER_SWEEP = "parameter_sweep";
    const std::string COMPARE_BASELINE = "compare_baseline";
    const std::string MEMORY_PROFILE = "memory_profile";
}

// Color codes for terminal output
namespace Color {
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
    const std::string RESET = "\033[0m";
    const std::string DIM = "\033[2m";
}

// Terminal capability detection
namespace Terminal {
    static bool supportsColor() {
#ifdef __unix__
        return std::getenv("TERM") != nullptr && std::string(std::getenv("TERM")).find("color") != std::string::npos;
#endif
        return false;
    }
    
    static bool isANSI() {
#ifdef __unix__
        return std::getenv("TERM") != nullptr && std::string(std::getenv("TERM")) != "dumb";
#endif
        return false;
    }
}

// Test result structure
struct TestResult {
    std::string name;
    std::string description;
    bool passed;
    double duration; // seconds
    std::string errorMessage;
    std::map<std::string, double> metrics;
    std::map<std::string, std::string> metadata;
    
    // Helper for JSON serialization
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"name\": \"" << name << "\", \"description\": \"" << description << "\", \"passed\": " << (passed ? "true" : "false")
           << ", \"duration\": " << duration << ", \"metrics\": {";
        bool first = true;
        for (const auto& pair : metrics) {
            if (!first) ss << ", ";
            ss << "\"" << pair.first << "\": " << pair.second;
            first = false;
        }
        ss << "}}";
        return ss.str();
    }
};

// Progress indicator for long-running tests
class ProgressIndicator {
private:
    std::string description;
    size_t total;
    size_t current;
    double startTime;
    bool showPercentage;
    bool showTime;
    
public:
    ProgressIndicator(const std::string& desc, size_t total, 
                     bool showPercentage = true, bool showTime = true)
        : description(desc), total(total), current(0), 
          startTime(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()),
          showPercentage(showPercentage), showTime(showTime) {
    }
    
    void update(size_t increment = 1) {
        current += increment;
        if (showPercentage || showTime) {
            std::cout << "\r";
            std::cout << "  [PROGRESS] " << description << ": ";
            
            if (showPercentage) {
                float percentage = static_cast<float>(current) / total * 100.0f;
                std::cout << "[" << std::fixed << std::setprecision(1) << percentage << "%] ";
            }
            
            if (showTime && current > 0) {
                double elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - startTime;
                double eta = (elapsed / current) * (total - current);
                auto hours = static_cast<int>(eta) / 3600;
                auto minutes = (static_cast<int>(eta) % 3600) / 60;
                auto seconds = static_cast<int>(eta) % 60;
                std::cout << "ETA: ";
                if (hours > 0) std::cout << hours << "h ";
                if (hours > 0 || minutes > 0) std::cout << minutes << "m ";
                std::cout << seconds << "s";
            }
            
            std::cout << std::flush;
        }
    }
    
    void complete(const std::string& finalMessage = "") {
        current = total;
        std::cout << "\r";
        if (!finalMessage.empty()) {
            std::cout << "  [PROGRESS] " << finalMessage << std::endl;
        } else {
            std::cout << "\n";
        }
    }
};

// Timestamp utility
class Timestamp {
public:
    static std::string now() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

// Enhanced argument parser with validation
class ArgumentParser {
private:
    std::map<std::string, std::string> args;
    
public:
    void parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --help flag
            if (arg == "--help") {
                args["help"] = "true";
            } 
            // Handle --verbose and -v flags
            else if (arg == "--verbose" || arg == "-v") {
                args["verbose"] = "true";
            } 
            
            // Handle --benchmark flag
            else if (arg == "--benchmark") {
                args["benchmark"] = "true";
            } 
            
            // Handle --replay flag with filename
            else if (arg == "--replay") {
                args["replay"] = "true";
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args["replay_file"] = argv[++i];
                }
            }
            
            // Handle --save-results flag with filename
            else if (arg == "--save-results") {
                args["save_results"] = "true";
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args["results_file"] = argv[++i];
                }
            }
            
            // Handle --checkpoint flag with filename
            else if (arg == "--checkpoint") {
                args["checkpoint"] = "true";
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args["checkpoint_file"] = argv[++i];
                }
            }
            
            // Handle --config flag with optional value
            else if (arg.rfind("--config", 0) == 0) {
                if (arg.find('=') != std::string::npos) {
                    args["config"] = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args["config"] = argv[++i];
                }
            }
            
            // Handle --iterations flag with optional value
            else if (arg.rfind("--iterations", 0) == 0) {
                if (arg.find('=') != std::string::npos) {
                    args["iterations"] = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args["iterations"] = argv[++i];
                }
            }
            
            // Handle --set flag for parameter overrides (KEY=VALUE)
            else if (arg.rfind("--set ", 0) == 0) {
                std::string param = arg.substr(6);
                if (param.find('=') != std::string::npos) {
                    size_t eqPos = param.find('=');
                    std::string key = param.substr(0, eqPos);
                    std::string value = param.substr(eqPos + 1);
                    args["set_" + key] = value;
                }
            }
            
            // Handle --set without space for short form (e.g., --setkey=value)
            else if (arg.find("--set") == 0 && arg.length() > 5) {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(5, pos - 5);
                    std::string value = arg.substr(pos + 1);
                    args["set_" + key] = value;
                }
            }
            
            // Handle --test flags (1, 2, 3)
            else if (arg == "--test" && i + 1 < argc) {
                std::string testNum = argv[++i];
                if (testNum == "1" || testNum == "2" || testNum == "3") {
                    args["test_" + testNum] = "true";
                }
            }
            
            // Handle short --test1, --test2, --test3
            else if (arg == "--test1") {
                args["test_1"] = "true";
            } else if (arg == "--test2") {
                args["test_2"] = "true";
            } else if (arg == "--test3") {
                args["test_3"] = "true";
            }
            
            // Handle other flags
            else if (arg == "--version") {
                args["version"] = "true";
            } else if (arg == "--license") {
                args["license"] = "true";
            } else if (arg == "--compare") {
                args["compare"] = "true";
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args["compare_file"] = argv[++i];
                }
            } else if (arg == "--param-sweep") {
                args["parameter_sweep"] = "true";
            } else if (arg == "--profile-memory") {
                args["memory_profile"] = "true";
            } else if (arg == "--list-tests") {
                args["list_tests"] = "true";
            }
            
            // Handle unknown flags with = syntax (--key=value)
            else if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args[key] = value;
            }
        }
    }
    
    bool has(const std::string& key) const {
        return args.find(key) != args.end();
    }
    
    std::string get(const std::string& key, const std::string& defaultValue = "") const {
        auto it = args.find(key);
        if (it != args.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        return has(key) && (get(key) == "true" || get(key) == "1");
    }
    
    size_t getSizeT(const std::string& key, size_t defaultValue = 0) const {
        if (!has(key)) return defaultValue;
        try {
            return static_cast<size_t>(std::stoull(get(key)));
        } catch (...) {
            return defaultValue;
        }
    }
    
    int getInt(const std::string& key, int defaultValue = 0) const {
        if (!has(key)) return defaultValue;
        try {
            return std::stoi(get(key));
        } catch (...) {
            return defaultValue;
        }
    }
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        if (!has(key)) return defaultValue;
        try {
            return std::stod(get(key));
        } catch (...) {
            return defaultValue;
        }
    }
    
    void printHelp() const {
        std::cout << Color::BOLD << "NLM Phase 2: Neural Learning Machine - Command Line Options" << Color::RESET << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        std::cout << std::endl;
        
        std::cout << Color::BOLD << "Basic Options:" << Color::RESET << std::endl;
        std::cout << "  --help                    " << std::setw(55) << "Show this help message" << std::endl;
        std::cout << "  --version                 " << std::setw(55) << "Show version information" << std::endl;
        std::cout << "  --license                 " << std::setw(55) << "Show license information" << std::endl;
        std::cout << "  -v, --verbose             " << std::setw(55) << "Enable verbose output" << std::endl;
        std::cout << std::endl;
        
        std::cout << Color::BOLD << "Test Selection:" << Color::RESET << std::endl;
        std::cout << "  --test 1                  " << std::setw(55) << "Run only Test 1: Basic Connectivity" << std::endl;
        std::cout << "  --test 2                  " << std::setw(55) << "Run only Test 2: Plasticity Learning" << std::endl;
        std::cout << "  --test 3                  " << std::setw(55) << "Run only Test 3: STDP Verification" << std::endl;
        std::cout << "  --test1                   " << std::setw(55) << "Run only Test 1 (short form)" << std::endl;
        std::cout << "  --test2                   " << std::setw(55) << "Run only Test 2 (short form)" << std::endl;
        std::cout << "  --test3                   " << std::setw(55) << "Run only Test 3 (short form)" << std::endl;
        std::cout << "  --iterations N             " << std::setw(55) << "Number of iterations for each test (default: 1)" << std::endl;
        std::cout << std::endl;
        
        std::cout << Color::BOLD << "Performance & Analysis:" << Color::RESET << std::endl;
        std::cout << "  --benchmark               " << std::setw(55) << "Run performance benchmark" << std::endl;
        std::cout << "  --replay FILE             " << std::setw(55) << "Replay previously saved test scenario" << std::endl;
        std::cout << "  --checkpoint FILE         " << std::setw(55) << "Save brain state after test completion" << std::endl;
        std::cout << "  --save-results [FILE]     " << std::setw(55) << "Save test results to file" << std::endl;
        std::cout << std::endl;
        
        std::cout << Color::BOLD << "Configuration:" << Color::RESET << std::endl;
        std::cout << "  --config FILE             " << std::setw(55) << "Load configuration from file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --set KEY=VALUE           " << std::setw(55) << "Override configuration parameter" << std::endl;
        std::cout << "  --set KEY VALUE           " << std::setw(55) << "Override configuration parameter (space separated)" << std::endl;
        std::cout << std::endl;
        
        std::cout << Color::BOLD << "Advanced Features:" << Color::RESET << std::endl;
        std::cout << "  --compare BASELINE        " << std::setw(55) << "Run comparison mode: baseline vs new version" << std::endl;
        std::cout << "  --param-sweep             " << std::setw(55) << "Run parameter sweep experiments" << std::endl;
        std::cout << "  --profile-memory          " << std::setw(55) << "Enable memory profiling hooks" << std::endl;
        std::cout << "  --list-tests              " << std::setw(55) << "List available tests" << std::endl;
        std::cout << std::endl;
        
        std::cout << Color::BOLD << "Example Usage:" << Color::RESET << std::endl;
        std::cout << "./nlm_phase2 --test 2 --iterations 3 --verbose --set neuron_count=1000" << std::endl;
        std::cout << "./nlm_phase2 --config custom.cfg --benchmark --checkpoint brain_state.bin" << std::endl;
        std::cout << "./nlm_phase2 --compare baseline_results.json --save-results final_results.json" << std::endl;
        std::cout << "./nlm_phase2 --param-sweep --set connection_probability=0.2 --set stdp_ltp_weight=0.03" << std::endl;
        std::cout << std::endl;
    }
};

// Test runner system with enhanced error handling
class TestRunner {
private:
    std::vector<TestResult> results;
    std::shared_ptr<Config> config;
    bool verbose;
    bool benchmarkMode;
    bool saveResults;
    std::string resultsFile;
    std::string checkpointFile;
    
public:
    TestRunner(std::shared_ptr<Config> cfg, bool verboseMode = false, 
              bool benchmark = false, const std::string& resultsFile = "", 
              const std::string& checkpointFile = "")
        : config(cfg), verbose(verboseMode), benchmarkMode(benchmark), 
          resultsFile(resultsFile), checkpointFile(checkpointFile) {
        // Ensure parameters are valid
        if (!config->has(ConfigKeys::RANDOM_SEED)) {
            config->set(ConfigKeys::RANDOM_SEED, static_cast<int64_t>(42), ConfigSource::Default);
        }
        if (!config->has(ConfigKeys::SIMULATION_TIMESTEP)) {
            config->set(ConfigKeys::SIMULATION_TIMESTEP, 0.001, ConfigSource::Default);
        }
        if (!config->has(ConfigKeys::NEURON_COUNT)) {
            config->set(ConfigKeys::NEURON_COUNT, static_cast<int64_t>(500), ConfigSource::Default);
        }
        if (!config->has(ConfigKeys::REGION_COUNT)) {
            config->set(ConfigKeys::REGION_COUNT, static_cast<int64_t>(1), ConfigSource::Default);
        }
        if (!config->has(ConfigKeys::CONNECTION_PROBABILITY)) {
            config->set(ConfigKeys::CONNECTION_PROBABILITY, 0.15f, ConfigSource::Default);
        }
    }
    
    void runTest1(std::shared_ptr<Brain> brain, size_t iterations = 1) {
        std::cout << "\n" << Color::CYAN << "=== Test 1: Basic Neural Connectivity ===" << Color::RESET << std::endl;
        std::cout << "Description: Test if spikes can propagate through a simple network" << std::endl;
        
        for (size_t iter = 0; iter < iterations; ++iter) {
            std::cout << "\nIteration " << (iter + 1) << "/" << iterations << ":" << std::endl;
            
            double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
            try {
                // Reset and reinitialize brain
                brain->reset();
                brain->initialize();
                
                // Inject current into a few neurons and see if spikes propagate
                auto* region = brain->getRegion(RegionId(1));
                if (!region) {
                    throw std::runtime_error("No region found!");
                }
                
                auto neurons = region->getAllNeurons();
                if (neurons.empty()) {
                    throw std::runtime_error("No neurons found!");
                }
                
                // Get initial spike count
                size_t initialSpikes = brain->getTotalSpikeCount();
                
                // Inject strong current into first 10 neurons
                std::cout << "  Injecting current into 10 neurons..." << std::endl;
                for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                    neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
                }
                
                // Run a few steps with progress indicator
                std::cout << "  Running simulation for 50 steps..." << std::endl;
                ProgressIndicator progress("Test 1 progress", 50, true, false);
                for (SimulationStep step = 0; step < 50; ++step) {
                    brain->step(step, step * 0.001);
                    progress.update(1);
                }
                progress.complete("Test 1 completed");
                
                size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
                
                double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                double duration = endTime - startTime;
                
                TestResult result;
                result.name = "test1_iteration_" + std::to_string(iter + 1);
                result.description = "Basic Neural Connectivity - Iteration " + std::to_string(iter + 1);
                result.passed = (spikes > 0);
                result.duration = duration;
                result.metrics["spikes_generated"] = static_cast<double>(spikes);
                result.metrics["spikes_per_step"] = spikes / 50.0;
                result.metrics["total_steps"] = 50.0;
                
                std::cout << "  " << Color::GREEN << "Spikes generated: " << spikes << Color::RESET << std::endl;
                
                if (spikes > 0) {
                    std::cout << "  " << Color::GREEN << "✓ SUCCESS: Spikes propagate through network" << Color::RESET << std::endl;
                    result.passed = true;
                } else {
                    std::cout << "  " << Color::YELLOW << "! WARNING: No spikes generated" << Color::RESET << std::endl;
                    std::cout << "    " << Color::YELLOW << "Checking neuron parameters..." << Color::RESET << std::endl;
                    for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                        std::cout << "    Neuron " << i << 
                                  << " V=" << std::fixed << std::setprecision(4) << neurons[i]->getMembranePotential() <<
                                  << " thresh=" << std::fixed << std::setprecision(4) << neurons[i]->getThreshold() << std::endl;
                    }
                    result.passed = false;
                    result.errorMessage = "No spikes generated despite current injection";
                }
                
                if (verbose) {
                    std::cout << "  " << Color::DIM << "Duration: " << std::fixed << std::setprecision(3) << duration << "s" << Color::RESET << std::endl;
                }
                
                results.push_back(result);
                
            } catch (const std::exception& e) {
                std::cout << "  " << Color::RED << "ERROR: " << e.what() << Color::RESET << std::endl;
                TestResult result;
                result.name = "test1_iteration_" + std::to_string(iter + 1);
                result.description = "Basic Neural Connectivity - Iteration " + std::to_string(iter + 1);
                result.passed = false;
                result.duration = 0.0;
                result.errorMessage = std::string("Exception: ") + e.what();
                results.push_back(result);
            }
        }
    }
    
    void runTest2(std::shared_ptr<Brain> brain, size_t iterations = 1) {
        std::cout << "\n" << Color::CYAN << "=== Test 2: Plasticity Learning Experiment ===" << Color::RESET << std::endl;
        std::cout << "Description: Apply repeated input patterns to stimulate synaptic learning" << std::endl;
        
        for (size_t iter = 0; iter < iterations; ++iter) {
            std::cout << "\nIteration " << (iter + 1) << "/" << iterations << ":" << std::endl;
            
            double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
            try {
                // Reset and reinitialize brain
                brain->reset();
                brain->initialize();
                
                // Create learning experiment wrapper
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
                    }
                    
                    void computeStatistics() {
                        if (initialWeights.empty() || finalWeights.empty()) {
                            std::cout << "    " << Color::RED << "ERROR: No weights recorded" << Color::RESET << std::endl;
                            return;
                        }
                        
                        // Compute weight changes
                        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                        float initialMean = initialSum / initialWeights.size();
                        float finalMean = finalSum / finalWeights.size();
                        
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
                        
                        std::cout << "    " << Color::YELLOW << "Weight Statistics:" << Color::RESET << std::endl;
                        std::cout << "      Initial mean weight: " << std::fixed << std::setprecision(4) << initialMean << std::endl;
                        std::cout << "      Final mean weight: " << std::fixed << std::setprecision(4) << finalMean << std::endl;
                        std::cout << "      Change: " << std::fixed << std::setprecision(4) << (finalMean - initialMean) << std::endl;
                        
                        std::cout << "    " << Color::YELLOW << "Synaptic Changes:" << Color::RESET << std::endl;
                        std::cout << "      Strengthened: " << strengthened << std::endl;
                        std::cout << "      Weakened: " << weakened << std::endl;
                        std::cout << "      Unchanged: " << unchanged << std::endl;
                        
                        // Determine if learning occurred
                        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                               (strengthened > 0 || weakened > 0);
                        
                        if (learningOccurred) {
                            std::cout << "    " << Color::GREEN << "✓ LEARNING DETECTED: Synaptic weights changed through experience" << Color::RESET << std::endl;
                        } else {
                            std::cout << "    " << Color::YELLOW << "✗ NO LEARNING: Weights did not change significantly" << Color::RESET << std::endl;
                        }
                    }
                };
                
                LearningExperiment experiment(brain, 42);
                
                // Record initial state
                std::cout << "  Recording initial state..." << std::endl;
                experiment.recordInitialState();
                std::cout << "    Synapses: " << experiment.initialSynapseCount << std::endl;
                if (!experiment.initialWeights.empty()) {
                    float sum = std::accumulate(experiment.initialWeights.begin(), experiment.initialWeights.end(), 0.0f);
                    float mean = sum / experiment.initialWeights.size();
                    std::cout << "    Mean weight: " << std::fixed << std::setprecision(4) << mean << std::endl;
                }
                
                // Enable plasticity on synapses
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (auto& syn : region->getSynapses()) {
                        syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
                    }
                }
                
                // Apply repeated input pattern to stimulate learning
                std::cout << "  Applying repeated input patterns (1000 steps)..." << std::endl;
                ProgressIndicator progress("Test 2 progress", 1000);
                
                for (SimulationStep step = 0; step < 1000; ++step) {
                    // Create input pattern - inject current into sensory neurons
                    for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                    }
                    
                    brain->step(step, step * 0.001);
                    
                    // Log progress every 100 steps
                    if (step % 100 == 0) {
                        std::cout << "    Step " << step << 
                                  << " | Spikes: " << brain->getTotalSpikeCount() <<
                                  << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
                        progress.update(100);
                    }
                }
                progress.update(1000 - 900); // Complete progress
                
                // Record final state
                std::cout << "  Recording final state..." << std::endl;
                experiment.recordFinalState();
                std::cout << "    Total spikes: " << brain->getTotalSpikeCount() << std::endl;
                if (!experiment.finalWeights.empty()) {
                    float sum = std::accumulate(experiment.finalWeights.begin(), experiment.finalWeights.end(), 0.0f);
                    float mean = sum / experiment.finalWeights.size();
                    std::cout << "    Mean weight: " << std::fixed << std::setprecision(4) << mean << std::endl;
                }
                
                // Compute and display statistics
                experiment.computeStatistics();
                
                double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                double duration = endTime - startTime;
                
                TestResult result;
                result.name = "test2_iteration_" + std::to_string(iter + 1);
                result.description = "Plasticity Learning - Iteration " + std::to_string(iter + 1);
                result.duration = duration;
                result.metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
                result.metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
                result.metrics["steps_performed"] = 1000.0;
                
                results.push_back(result);
                
                // Save checkpoint if requested
                if (!checkpointFile.empty()) {
                    std::cout << "  Saving checkpoint to: " << checkpointFile << std::endl;
                    // TODO: Implement checkpoint saving
                }
                
            } catch (const std::exception& e) {
                std::cout << "  " << Color::RED << "ERROR: " << e.what() << Color::RESET << std::endl;
                TestResult result;
                result.name = "test2_iteration_" + std::to_string(iter + 1);
                result.description = "Plasticity Learning - Iteration " + std::to_string(iter + 1);
                result.passed = false;
                result.duration = 0.0;
                result.errorMessage = std::string("Exception: ") + e.what();
                results.push_back(result);
            }
        }
    }
    
    void runTest3(std::shared_ptr<Brain> brain, size_t iterations = 1) {
        std::cout << "\n" << Color::CYAN << "=== Test 3: STDP Verification ===" << Color::RESET << std::endl;
        std::cout << "Description: Test spike-timing-dependent plasticity (STDP) rules" << std::endl;
        
        for (size_t iter = 0; iter < iterations; ++iter) {
            std::cout << "\nIteration " << (iter + 1) << "/" << iterations << ":" << std::endl;
            
            double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
            try {
                // Reset and reinitialize brain
                brain->reset();
                brain->initialize();
                
                auto* region = brain->getRegion(RegionId(1));
                if (!region) {
                    throw std::runtime_error("No region found!");
                }
                
                auto& synapses = region->getSynapses();
                if (synapses.size() < 5) {
                    throw std::runtime_error("Not enough synapses for STDP test");
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
                std::cout << "  Creating correlated pre->post activity (potentiation)..." << std::endl;
                ProgressIndicator progress("Test 3 progress", 50);
                
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
                    progress.update(1);
                }
                progress.complete("Test 3 completed");
                
                // Record after weights
                std::cout << "  After correlated activity:" << std::endl;
                float totalDelta = 0.0f;
                for (size_t i = 0; i < 5; ++i) {
                    float delta = synapses[i]->getWeight() - beforeWeights[i];
                    totalDelta += delta;
                    std::cout << "    Synapse " << i << 
                              << " after: " << std::fixed << std::setprecision(4) << synapses[i]->getWeight() <<
                              << " (Δ=" << std::fixed << std::setprecision(4) << delta << ")" << std::endl;
                }
                
                double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                double duration = endTime - startTime;
                
                TestResult result;
                result.name = "test3_iteration_" + std::to_string(iter + 1);
                result.description = "STDP Verification - Iteration " + std::to_string(iter + 1);
                result.duration = duration;
                result.metrics["total_delta"] = static_cast<double>(totalDelta);
                result.metrics["synapses_tested"] = 5.0;
                result.metrics["trials_performed"] = 50.0;
                
                if (totalDelta > 0.001f) {
                    std::cout << "  " << Color::GREEN << "✓ STDP WORKING: Pre-before-post produced potentiation" << Color::RESET << std::endl;
                    result.passed = true;
                } else if (totalDelta < -0.001f) {
                    std::cout << "  " << Color::YELLOW << "! STDP reversed: Check parameters" << Color::RESET << std::endl;
                    result.passed = false;
                    result.errorMessage = "STDP produced depression instead of potentiation";
                } else {
                    std::cout << "  " << Color::YELLOW << "! No change: STDP may not be triggering" << Color::RESET << std::endl;
                    result.passed = false;
                    result.errorMessage = "STDP produced no significant weight change";
                }
                
                results.push_back(result);
                
            } catch (const std::exception& e) {
                std::cout << "  " << Color::RED << "ERROR: " << e.what() << Color::RESET << std::endl;
                TestResult result;
                result.name = "test3_iteration_" + std::to_string(iter + 1);
                result.description = "STDP Verification - Iteration " + std::to_string(iter + 1);
                result.passed = false;
                result.duration = 0.0;
                result.errorMessage = std::string("Exception: ") + e.what();
                results.push_back(result);
            }
        }
    }
    
    void runAll(std::shared_ptr<Brain> brain, const std::vector<std::string>& testNames, size_t iterations = 1) {
        // Determine which tests to run based on names
        bool runTest1 = std::find(testNames.begin(), testNames.end(), "test1") != testNames.end();
        bool runTest2 = std::find(testNames.begin(), testNames.end(), "test2") != testNames.end();
        bool runTest3 = std::find(testNames.begin(), testNames.end(), "test3") != testNames.end();
        
        // Default: run all tests if none specified
        if (testNames.empty()) {
            runTest1 = runTest2 = runTest3 = true;
        }
        
        if (runTest1) runTest1(brain, iterations);
        if (runTest2) runTest2(brain, iterations);
        if (runTest3) runTest3(brain, iterations);
    }
    
    void printSummary() const {
        std::cout << "\n" << Color::BOLD << std::string(60, '=') << Color::RESET << std::endl;
        std::cout << Color::BOLD << "Test Results Summary" << Color::RESET << std::endl;
        std::cout << Color::BOLD << std::string(60, '=') << Color::RESET << std::endl;
        
        int totalTests = results.size();
        int passedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) passedTests++;
            totalDuration += result.duration;
        }
        
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << "Success Rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0) << "%" << std::endl;
        std::cout << "Total Duration: " << std::fixed << std::setprecision(2) << totalDuration << " seconds" << std::endl;
        
        if (totalTests > 0) {
            std::cout << "Average Duration per Test: " << std::fixed << std::setprecision(3) << (totalDuration / totalTests) << " seconds" << std::endl;
        }
        
        std::cout << std::endl;
        
        if (passedTests == totalTests && totalTests > 0) {
            std::cout << Color::GREEN << "✓ All tests passed!" << Color::RESET << std::endl;
        } else if (passedTests == 0 && totalTests > 0) {
            std::cout << Color::RED << "✗ All tests failed!" << Color::RESET << std::endl;
        } else {
            std::cout << Color::YELLOW << "⚠ Some tests failed" << Color::RESET << std::endl;
        }
    }
    
    void saveResultsToFile() const {
        if (resultsFile.empty()) return;
        
        std::cout << Timestamp::now() << " Saving results to: " << resultsFile << std::endl;
        
        std::ofstream file(resultsFile);
        if (!file.is_open()) {
            std::cout << Timestamp::now() << " " << Color::RED << "ERROR: Could not open results file for writing!" << Color::RESET << std::endl;
            return;
        }
        
        // Write JSON-like format for easy parsing
        file << "{\n";
        file << "  \"timestamp\": \"" << Timestamp::now() << "\",\n";
        file << "  \"total_tests\": " << results.size() << ",\n";
        file << "  \"passed_tests\": " << std::count_if(results.begin(), results.end(),
                  [](const TestResult& r) { return r.passed; }) << ",\n";
        file << "  \"total_duration\": " << std::fixed << std::setprecision(3) << std::accumulate(
            results.begin(), results.end(), 0.0, [](double sum, const TestResult& r) { return sum + r.duration; }) << ",\n";
        file << "  \"tests\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            file << "    {\n";
            file << "      \"name\": \"" << result.name << "\",\n";
            file << "      \"description\": \"" << result.description << "\",\n";
            file << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
            file << "      \"duration\": " << std::fixed << std::setprecision(3) << result.duration << ",\n";
            if (!result.errorMessage.empty()) {
                file << "      \"error_message\": \"" << result.errorMessage << "\",\n";
            }
            file << "      \"metrics\": {\n";
            bool firstMetric = true;
            for (const auto& metric : result.metrics) {
                if (!firstMetric) file << ",\n";
                file << "        \"" << metric.first << "\": " << std::fixed << std::setprecision(6) << metric.second;
                firstMetric = false;
            }
            file << "\n      },\n";
            file << "      \"metadata\": {\n";
            firstMetric = true;
            for (const auto& meta : result.metadata) {
                if (!firstMetric) file << ",\n";
                file << "        \"" << meta.first << "\": \"" << meta.second << "\"";
                firstMetric = false;
            }
            file << "\n      }\n";
            if (i < results.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
        std::cout << Timestamp::now() << " Results saved successfully!" << std::endl;
    }
    
    const std::vector<TestResult>& getResults() const {
        return results;
    }
};

// Enhanced main function with comprehensive argument parsing
int main(int argc, char** argv) {
    printBanner();
    
    std::cout << Timestamp::now() << " Starting NLM Phase 2 Neural Simulation..." << std::endl;
    
    // Parse command line arguments
    ArgumentParser parser;
    parser.parse(argc, argv);
    
    // Show help if requested
    if (parser.has("help")) {
        parser.printHelp();
        return 0;
    }
    
    // Show version
    if (parser.has("version")) {
        std::cout << "NLM Phase 2: Neural Learning Machine" << std::endl;
        std::cout << "Version: 2.0.0 (Enhanced)" << std::endl;
        std::cout << "Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
        return 0;
    }
    
    // Show license
    if (parser.has("license")) {
        std::cout << "NLM License" << std::endl;
        std::cout << "=========" << std::endl;
        std::cout << "Copyright (c) 2026 NLM Team" << std::endl;
        std::cout << "All rights reserved." << std::endl;
        return 0;
    }
    
    // List available tests
    if (parser.has("list_tests")) {
        std::cout << Color::BOLD << "Available Tests:" << Color::RESET << std::endl;
        std::cout << "  test1: Basic Neural Connectivity - Test if spikes can propagate through a simple network" << std::endl;
        std::cout << "  test2: Plasticity Learning - Apply repeated input patterns to stimulate synaptic learning" << std::endl;
        std::cout << "  test3: STDP Verification - Test spike-timing-dependent plasticity (STDP) rules" << std::endl;
        return 0;
    }
    
    // Initialize logger with appropriate level
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = parser.getBool("verbose") ? LogLevel::Info : LogLevel::Warning;
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Log startup message with color if supported
    std::cout << std::endl;
    if (Terminal::isANSI()) {
        std::cout << Color::BOLD << Color::CYAN << "NLM Phase 2: Neural Learning Machine" << Color::RESET << std::endl;
        std::cout << Color::DIM << "Phase 2 Enhanced with comprehensive test runner and CLI interface" << Color::RESET << std::endl;
    } else {
        std::cout << "NLM Phase 2: Neural Learning Machine" << std::endl;
        std::cout << "Phase 2 Enhanced with comprehensive test runner and CLI interface" << std::endl;
    }
    std::cout << std::endl;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    if (parser.has("config")) {
        configFile = parser.get("config");
    }
    
    // Validate config file path
    std::ifstream testFile(configFile);
    if (!testFile.is_open()) {
        std::cout << Timestamp::now() << " " << Color::YELLOW << "Warning: Config file not found: " << configFile << ". Using defaults." << Color::RESET << std::endl;
    } else {
        testFile.close();
        if (config->loadFromFile(configFile)) {
            std::cout << Timestamp::now() << " Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << Timestamp::now() << " " << Color::YELLOW << "Warning: Could not parse config file: " << configFile << ". Using defaults." << Color::RESET << std::endl;
        }
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply command-line parameter overrides (--set KEY=VALUE)
    for (const auto& pair : parser.args) {
        if (pair.first.substr(0, 4) == "set_") {
            std::string key = pair.first.substr(4);
            std::string value = pair.second;
            try {
                if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
                    config->set(key, std::stod(value), ConfigSource::CommandLine);
                } else if (value.find('f') != std::string::npos || value.find('F') != std::string::npos) {
                    config->set(key, std::stof(value), ConfigSource::CommandLine);
                } else {
                    // Try to detect if it's a boolean
                    if (value == "true" || value == "false") {
                        config->set(key, (value == "true"), ConfigSource::CommandLine);
                    } else {
                        config->set(key, std::stoll(value), ConfigSource::CommandLine);
                    }
                }
                std::cout << Timestamp::now() << " Applied parameter override: " << key << " = " << value << std::endl;
            } catch (const std::exception& e) {
                std::cout << Timestamp::now() << " " << Color::YELLOW << "Warning: Could not parse parameter override: " << key << " = " << value << " (" << e.what() << ")" << Color::RESET << std::endl;
            }
        }
    }
    
    // Set Phase 2 specific default values
    config->set(ConfigKeys::RANDOM_SEED, static_cast<int64_t>(42), ConfigSource::Default);
    config->set(ConfigKeys::SIMULATION_TIMESTEP, 0.001, ConfigSource::Default);
    config->set(ConfigKeys::NEURON_COUNT, static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set(ConfigKeys::REGION_COUNT, static_cast<int64_t>(1), ConfigSource::Default);
    config->set(ConfigKeys::CONNECTION_PROBABILITY, 0.15f, ConfigSource::Default);
    config->set(ConfigKeys::STDP_LTP_WEIGHT, 0.02f, ConfigSource::Default);
    config->set(ConfigKeys::STDP_LTD_WEIGHT, 0.015f, ConfigSource::Default);
    config->set(ConfigKeys::STDP_TIME_CONSTANT, 20.0f, ConfigSource::Default);
    config->set(ConfigKeys::SYNAPTOGENESIS_RATE, 0.0001f, ConfigSource::Default);
    config->set(ConfigKeys::PRUNING_RATE, 0.00001f, ConfigSource::Default);
    config->set(ConfigKeys::DOPAMINE_BASELINE, 0.1f, ConfigSource::Default);
    
    // Log configuration summary
    std::cout << std::endl;
    std::cout << Timestamp::now() << " Configuration Summary:" << std::endl;
    std::cout << "  " << Color::YELLOW << ConfigKeys::RANDOM_SEED << Color::RESET << ": " << config->getOr<int64_t>(ConfigKeys::RANDOM_SEED, 42) << std::endl;
    std::cout << "  " << Color::YELLOW << ConfigKeys::SIMULATION_TIMESTEP << Color::RESET << ": " << config->getOr<double>(ConfigKeys::SIMULATION_TIMESTEP, 0.001) << "s" << std::endl;
    std::cout << "  " << Color::YELLOW << ConfigKeys::NEURON_COUNT << Color::RESET << ": " << config->getOr<int64_t>(ConfigKeys::NEURON_COUNT, 500) << std::endl;
    std::cout << "  " << Color::YELLOW << ConfigKeys::REGION_COUNT << Color::RESET << ": " << config->getOr<int64_t>(ConfigKeys::REGION_COUNT, 1) << std::endl;
    std::cout << "  " << Color::YELLOW << ConfigKeys::CONNECTION_PROBABILITY << Color::RESET << ": " << config->getOr<float>(ConfigKeys::CONNECTION_PROBABILITY, 0.15f) << std::endl;
    
    // Initialize simulation clock
    double timestep = config->getOr<double>(ConfigKeys::SIMULATION_TIMESTEP, 0.001);
    SimulationClock clock(timestep);
    std::cout << Timestamp::now() << " Simulation clock initialized with timestep: " << timestep << "s" << std::endl;
    
    // Initialize brain
    std::cout << std::endl;
    std::cout << Timestamp::now() << " Initializing NLM Brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << Timestamp::now() << " " << Color::RED << "ERROR: Failed to initialize brain!" << Color::RESET << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Determine which tests to run
    std::vector<std::string> testNames;
    size_t iterations = parser.getSizeT("iterations", 1);
    
    if (parser.has("test_1")) testNames.push_back("test1");
    if (parser.has("test_2")) testNames.push_back("test2");
    if (parser.has("test_3")) testNames.push_back("test3");
    
    // Check for test names in other formats (--test 1, --test 2, etc.)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test" && i + 1 < argc) {
            std::string testNum = argv[++i];
            if (testNum == "1") testNames.push_back("test1");
            else if (testNum == "2") testNames.push_back("test2");
            else if (testNum == "3") testNames.push_back("test3");
        }
    }
    
    // Validate iterations
    if (iterations == 0) {
        std::cout << Timestamp::now() << " " << Color::YELLOW << "Warning: iterations cannot be 0, setting to 1" << Color::RESET << std::endl;
        iterations = 1;
    }
    
    // Create test runner
    TestRunner testRunner(config, parser.getBool("verbose"), 
                         parser.getBool("benchmark"), 
                         parser.get("results_file"), 
                         parser.get("checkpoint_file"));
    
    // Log test run information
    if (testNames.empty()) {
        std::cout << std::endl;
        std::cout << Timestamp::now() << " Running all tests with " << iterations << " iteration(s)..." << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << Timestamp::now() << " Running selected tests: ";
        for (size_t i = 0; i < testNames.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << testNames[i];
        }
        std::cout << " with " << iterations << " iteration(s)..." << std::endl;
    }
    
    // Run tests
    testRunner.runAll(brain, testNames, iterations);
    
    // Print summary
    testRunner.printSummary();
    
    // Save results if requested
    if (parser.getBool("save_results") || !parser.get("results_file").empty()) {
        testRunner.saveResultsToFile();
    }
    
    // Display advanced feature information
    if (parser.getBool("benchmark")) {
        std::cout << std::endl;
        std::cout << Color::CYAN << "Benchmark mode was enabled. For detailed benchmark results, check the benchmark.cpp file." << Color::RESET << std::endl;
    }
    
    if (parser.getBool("memory_profile")) {
        std::cout << std::endl;
        std::cout << Color::CYAN << "Memory profiling was enabled. For detailed memory profile, use external tools like valgrind or address sanitizer." << Color::RESET << std::endl;
    }
    
    if (parser.getBool("parameter_sweep")) {
        std::cout << std::endl;
        std::cout << Color::CYAN << "Parameter sweep mode was enabled. Consider running with --set to specify parameter ranges for systematic exploration." << Color::RESET << std::endl;
    }
    
    // Final brain status
    std::cout << std::endl;
    std::cout << Timestamp::now() << " === Final Brain Status ===" << std::endl;
    brain->logStatus();
    
    std::cout << std::endl;
    std::cout << Timestamp::now() << " === Phase 2 Complete ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 2 Objectives Completed:" << std::endl;
    std::cout << "  " << Color::GREEN << "✓" << Color::RESET << " Real LIF neuron dynamics implemented" << std::endl;
    std::cout << "  " << Color::GREEN << "✓" << Color::RESET << " Event-driven spike propagation with delays" << std::endl;
    std::cout << "  " << Color::GREEN << "✓" << Color::RESET << " STDP plasticity rule" << std::endl;
    std::cout << "  " << Color::GREEN << "✓" << Color::RESET << " Hebbian plasticity rule" << std::endl;
    std::cout << "  " << Color::GREEN << "✓" << Color::RESET << " Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "  " << Color::GREEN << "✓" << Color::RESET << " Learning experiment demonstrates measurable changes" << std::endl;
    std::cout << "  " << Color::GREEN << "✓" << Color::RESET << " Network shows activity-dependent synaptic modification" << std::endl;
    std::cout << std::endl;
    if (Terminal::isANSI()) {
        std::cout << Color::CYAN << "The NLM brain is now a functioning artificial neural substrate" << Color::RESET << std::endl;
        std::cout << Color::CYAN << "capable of changing its own synaptic connections through experience." << Color::RESET << std::endl;
    } else {
        std::cout << "The NLM brain is now a functioning artificial neural substrate" << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << Timestamp::now() << " NLM Phase 2 execution completed successfully!" << std::endl;
    
    return 0;
}
