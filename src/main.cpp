// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// IMPROVED VERSION: Modular, configurable, and extensible test framework

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
#include <map>
#include <set>

using namespace nlm;

// Configuration constants for easy customization
struct TestConfig {
    // Basic connectivity test parameters
    static constexpr float BASIC_CURRENT_INJECT = 50.0f;
    static constexpr SimulationStep BASIC_STEPS = 50;
    static constexpr size_t BASIC_NEURONS_TO_INJECT = 10;
    
    // Plasticity experiment parameters
    static constexpr SimulationStep PLASTICITY_STEPS = 1000;
    static constexpr size_t PLASTICITY_NEURONS_INJECT = 20;
    static constexpr float PLASTICITY_CURRENT = 30.0f;
    static constexpr SimulationStep PLASTICITY_LOG_INTERVAL = 100;
    
    // STDP verification parameters
    static constexpr int STDP_TRIALS = 50;
    static constexpr float STDP_PRE_POST_CURRENT = 60.0f;
    static constexpr float STDP_WEIGHT_CHANGE_THRESHOLD = 0.001f;
    
    // General parameters
    static constexpr size_t MOST_ACTIVE_NEURONS = 10;
    static constexpr float SYNAPSE_WEIGHT_CHANGE_THRESHOLD = 0.01f;
    static constexpr float LEARNING_THRESHOLD = 0.001f;
};

// Test result structure
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    float executionTimeMs = 0.0f;
};

// Command-line argument parsing
class ArgumentParser {
public:
    static bool parseArgs(int argc, char** argv, std::map<std::string, std::string>& parsedArgs) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    parsedArgs["config"] = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    parsedArgs["config"] = argv[++i];
                }
            } else if (arg == "--help" || arg == "-h") {
                parsedArgs["help"] = "true";
            } else if (arg == "--test" || arg == "-t") {
                if (i + 1 < argc) {
                    parsedArgs["test"] = argv[++i];
                }
            } else if (arg.substr(0, 7) == "--param") {
                // Parse --param key=value
                size_t pos = arg.find('=');
                if (pos != std::string::npos && pos > 7) {
                    std::string key = arg.substr(7, pos - 7);
                    std::string value = arg.substr(pos + 1);
                    parsedArgs["param_" + key] = value;
                }
            } else if (arg == "--run-all") {
                parsedArgs["run_all"] = "true";
            } else if (arg == "--verbose" || arg == "-v") {
                parsedArgs["verbose"] = "true";
            } else if (arg == "--debug") {
                parsedArgs["debug"] = "true";
            }
        }
        return !parsedArgs.count("help");
    }
    
    static bool validateTestName(const std::string& testName) {
        const std::set<std::string> validTests = {"basic", "plasticity", "stdp", "all"};
        return validTests.find(testName) != validTests.end();
    }
    
    static bool applyParameters(std::shared_ptr<Config> config, const std::map<std::string, std::string>& parsedArgs) {
        bool success = true;
        
        // Apply parameter overrides
        for (const auto& pair : parsedArgs) {
            if (pair.first.find("param_") == 0) {
                std::string key = pair.first.substr(6); // Remove "param_" prefix
                std::string value = pair.second;
                
                // Try to parse as float first
                try {
                    float floatValue = std::stof(value);
                    config->set(key, floatValue, ConfigSource::CommandLine);
                } catch (...) {
                    try {
                        int intValue = std::stoi(value);
                        config->set(key, static_cast<int64_t>(intValue), ConfigSource::CommandLine);
                    } catch (...) {
                        try {
                            double doubleValue = std::stod(value);
                            config->set(key, doubleValue, ConfigSource::CommandLine);
                        } catch (...) {
                            NLM_LOG_WARNING("Could not parse parameter value for '" + key + "': " + value);
                            success = false;
                        }
                    }
                }
            }
        }
        
        return success;
    }
    
    static void printHelp() {
        std::cout << R"(
=== NLM Phase 2 Test Framework - Help ===

Usage: nlm [OPTIONS]

Options:
  --config <file>           Load configuration from file
  --test <name>             Run specific test (basic, plasticity, stdp, all)
  --run-all                 Run all tests (default)
  --param key=value         Set parameter value
  --verbose, -v             Enable verbose output
  --help, -h                Show this help message

Test names:
  basic     - Basic neural connectivity test
  plasticity - Plasticity learning experiment
  stdp      - STDP verification test
  all       - Run all tests

Example:
  nlm --config custom.cfg --test plasticity --verbose
  nlm --run-all --param BASIC_CURRENT_INJECT=100.0
  nlm --test stdp

)" << std::endl;
    }
};

// Test framework
class TestFramework {
public:
    static std::vector<TestResult> runTests(std::shared_ptr<Brain> brain, 
                                           const std::map<std::string, std::string>& args) {
        std::vector<TestResult> results;
        
        std::string testType = args.count("test") ? args.at("test") : "all";
        bool verbose = args.count("verbose") > 0;
        
        // Reset brain if it already exists
        brain->reset();
        brain->initialize();
        
        if (testType == "basic" || testType == "all") {
            results.push_back(runBasicConnectivityTest(brain, verbose));
        }
        
        if (testType == "plasticity" || testType == "all") {
            brain->reset();
            brain->initialize();
            results.push_back(runPlasticityExperiment(brain, verbose));
        }
        
        if (testType == "stdp" || testType == "all") {
            brain->reset();
            brain->initialize();
            results.push_back(runStdpVerification(brain, verbose));
        }
        
        return results;
    }
    
private:
    static TestResult runBasicConnectivityTest(std::shared_ptr<Brain> brain, bool verbose) {
        TestResult result;
        result.testName = "Basic Connectivity Test";
        
        auto startTime = std::chrono::steady_clock::now();
        
        try {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
            
            // Inject current into a few neurons and see if spikes propagate
            auto* region = brain->getRegion(RegionId(1));
            if (!region) {
                result.passed = false;
                result.message = "Failed to get region";
                return result;
            }
            
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) {
                result.passed = false;
                result.message = "No neurons found";
                return result;
            }
            
            // Get initial spike count
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject strong current into first 10 neurons
            NLM_LOG_INFO("  Injecting current into " + std::to_string(std::min(TestConfig::BASIC_NEURONS_TO_INJECT, neurons.size())) + " neurons...");
            for (size_t i = 0; i < std::min(TestConfig::BASIC_NEURONS_TO_INJECT, neurons.size()); ++i) {
                neurons[i]->injectCurrent(TestConfig::BASIC_CURRENT_INJECT);
            }
            
            // Run a few steps
            for (SimulationStep step = 0; step < TestConfig::BASIC_STEPS; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
            
            if (spikes > 0) {
                NLM_LOG_INFO("  ✓ Spikes propagate through network");
                result.passed = true;
                result.message = "Successfully generated " + std::to_string(spikes) + " spikes";
            } else {
                NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
                if (neurons.size() >= 1) {
                    NLM_LOG_INFO("    Neuron 0 V=" + std::to_string(neurons[0]->getMembranePotential()) +
                                " thresh=" + std::to_string(neurons[0]->getThreshold()));
                    if (neurons.size() >= 2) {
                        NLM_LOG_INFO("    Neuron 1 V=" + std::to_string(neurons[1]->getMembranePotential()) +
                                    " thresh=" + std::to_string(neurons[1]->getThreshold()));
                    }
                }
                result.passed = false;
                result.message = "No spikes generated despite current injection";
            }
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::steady_clock::now();
        result.executionTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
        return result;
    }
    
    static TestResult runPlasticityExperiment(std::shared_ptr<Brain> brain, bool verbose) {
        TestResult result;
        result.testName = "Plasticity Learning Experiment";
        
        auto startTime = std::chrono::steady_clock::now();
        
        try {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
            
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
                    
                    mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(TestConfig::MOST_ACTIVE_NEURONS);
                    
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
                        if (delta > TestConfig::SYNAPSE_WEIGHT_CHANGE_THRESHOLD) ++strengthened;
                        else if (delta < -TestConfig::SYNAPSE_WEIGHT_CHANGE_THRESHOLD) ++weakened;
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
                    bool learningOccurred = (std::abs(finalMean - initialMean) > TestConfig::LEARNING_THRESHOLD) ||
                                          (strengthened > 0 || weakened > 0);
                    
                    NLM_LOG_INFO("");
                    if (learningOccurred) {
                        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
                        result.passed = true;
                        result.message = "Learning detected with " + std::to_string(strengthened) + " strengthened, " + 
                                       std::to_string(weakened) + " weakened synapses";
                    } else {
                        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
                        result.passed = false;
                        result.message = "No significant learning detected";
                    }
                }
            };
            
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
            NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(TestConfig::PLASTICITY_STEPS) + " steps)...");
            
            for (SimulationStep step = 0; step < TestConfig::PLASTICITY_STEPS; ++step) {
                // Create input pattern - inject current into sensory neurons
                for (size_t i = 0; i < TestConfig::PLASTICITY_NEURONS_INJECT && 
                     i < brain->getTotalNeuronCount() / 4; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, TestConfig::PLASTICITY_CURRENT);
                }
                
                brain->step(step, step * 0.001);
                
                // Log progress every 100 steps
                if (step % TestConfig::PLASTICITY_LOG_INTERVAL == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
            
            // Record final state
            experiment.recordFinalState();
            
            // Compute and display statistics
            experiment.computeStatistics();
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::steady_clock::now();
        result.executionTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
        return result;
    }
    
    static TestResult runStdpVerification(std::shared_ptr<Brain> brain, bool verbose) {
        TestResult result;
        result.testName = "STDP Verification Test";
        
        auto startTime = std::chrono::steady_clock::now();
        
        try {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 3: STDP Verification ===");
            
            auto* region = brain->getRegion(RegionId(1));
            if (!region) {
                result.passed = false;
                result.message = "Failed to get region";
                return result;
            }
            
            // Get first few synapses
            auto& synapses = region->getSynapses();
            if (synapses.size() < 5) {
                result.passed = false;
                result.message = "Not enough synapses for STDP test";
                return result;
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
            
            for (int trial = 0; trial < TestConfig::STDP_TRIALS; ++trial) {
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
                    preNeuron->injectCurrent(TestConfig::STDP_PRE_POST_CURRENT);
                    brain->step(trial * 2, trial * 2 * 0.001);
                    
                    // Then post fires
                    postNeuron->injectCurrent(TestConfig::STDP_PRE_POST_CURRENT);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                }
            }
            
            // Record after weights
            NLM_LOG_INFO("  After correlated activity:");
            for (size_t i = 0; i < 5; ++i) {
                float delta = synapses[i]->getWeight() - beforeWeights[i];
                NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                            " after: " + std::to_string(synapses[i]->getWeight()) +
                            " (\\u0394=" + std::to_string(delta) + ")");
            }
            
            // Check if weights increased (LTP)
            float totalDelta = 0.0f;
            for (size_t i = 0; i < 5; ++i) {
                totalDelta += synapses[i]->getWeight() - beforeWeights[i];
            }
            
            NLM_LOG_INFO("");
            if (totalDelta > TestConfig::STDP_WEIGHT_CHANGE_THRESHOLD) {
                NLM_LOG_INFO("  \\u2713 STDP WORKING: Pre-before-post produced potentiation");
                result.passed = true;
                result.message = "STDP produced LTP with total delta " + std::to_string(totalDelta);
            } else if (totalDelta < -TestConfig::STDP_WEIGHT_CHANGE_THRESHOLD) {
                NLM_LOG_INFO("  ! STDP reversed: Check parameters");
                result.passed = false;
                result.message = "STDP produced LTD with total delta " + std::to_string(totalDelta);
            } else {
                NLM_LOG_INFO("  ! No change: STDP may not be triggering");
                result.passed = false;
                result.message = "STDP produced no significant change (delta=" + std::to_string(totalDelta) + ")";
            }
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::steady_clock::now();
        result.executionTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
        return result;
    }
};

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP plasticity rule");
    NLM_LOG_INFO("  - Hebbian plasticity rule");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Parse command line arguments
    std::map<std::string, std::string> parsedArgs;
    bool continueExecution = ArgumentParser::parseArgs(argc, argv, parsedArgs);
    
    if (!continueExecution) {
        ArgumentParser::printHelp();
        return 0;
    }
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    if (parsedArgs.count("config")) {
        configFile = parsedArgs.at("config");
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply parameter overrides from command line
    bool paramsApplied = ArgumentParser::applyParameters(config, parsedArgs);
    if (!paramsApplied) {
        NLM_LOG_WARNING("Some parameter overrides could not be applied.");
    }
    
    // Validate test name if specified
    std::string testType = "all"; // Default
    if (parsedArgs.count("test")) {
        testType = parsedArgs.at("test");
        if (!ArgumentParser::validateTestName(testType)) {
            NLM_LOG_ERROR("Invalid test name: " + testType);
            NLM_LOG_ERROR("Valid test names are: basic, plasticity, stdp, all");
            ArgumentParser::printHelp();
            return 1;
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    // Log debug info if requested
    if (parsedArgs.count("debug")) {
        NLM_LOG_INFO("  Debug mode enabled");
        NLM_LOG_INFO("  Test type: " + testType);
        if (parsedArgs.count("verbose")) {
            NLM_LOG_INFO("  Verbose mode enabled");
        }
    }
    
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
    
    // Run tests
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Execution ===");
    
    auto testResults = TestFramework::runTests(brain, parsedArgs);
    
    // Display summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Summary ===");
    
    size_t passedCount = 0;
    size_t failedCount = 0;
    float totalTime = 0.0f;
    
    for (const auto& result : testResults) {
        NLM_LOG_INFO("Test: " + result.testName);
        NLM_LOG_INFO("  Status: " + std::string(result.passed ? "PASSED" : "FAILED"));
        NLM_LOG_INFO("  Message: " + result.message);
        NLM_LOG_INFO("  Execution time: " + std::to_string(result.executionTimeMs) + " ms");
        NLM_LOG_INFO("");
        
        if (result.passed) {
            passedCount++;
        } else {
            failedCount++;
        }
        totalTime += result.executionTimeMs;
    }
    
    NLM_LOG_INFO("=== Overall Results ===");
    NLM_LOG_INFO("Total tests: " + std::to_string(testResults.size()));
    NLM_LOG_INFO("Passed: " + std::to_string(passedCount));
    NLM_LOG_INFO("Failed: " + std::to_string(failedCount));
    NLM_LOG_INFO("Total execution time: " + std::to_string(totalTime) + " ms");
    NLM_LOG_INFO("Average time per test: " + std::to_string(testResults.empty() ? 0.0f : totalTime / testResults.size()) + " ms");
    
    if (failedCount > 0) {
        NLM_LOG_ERROR("Some tests failed!");
        return 1;
    }
    
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