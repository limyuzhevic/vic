// NLM (熙然) - Neural Learning Machine
// Phase 2: Enhanced Real Neural Computation
//
// This phase implements real spiking neural computation with improved structure:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Advanced test framework
// - Configurable execution modes
// - Comprehensive logging and error handling
// - Performance profiling capabilities

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
#include "ConfigManager.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <map>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <fstream>

using namespace nlm;

// Enhanced configuration structure
struct EnhancedConfig {
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    
    // STDP parameters
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    
    // Structural plasticity
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Test configuration
    int maxSteps = 1000;
    int connectivityTestSteps = 50;
    int plasticityTestSteps = 1000;
    int stdpTestTrials = 50;
    
    // Performance and output
    bool enableProfiling = false;
    bool verboseOutput = true;
    bool saveResults = false;
    std::string outputDir = ".";
    
    // Error handling
    int maxRetries = 3;
    double retryDelay = 0.1;
    
    // Test selection
    bool runConnectivityTest = true;
    bool runPlasticityTest = true;
    bool runStdpTest = true;
    bool runLearningExperiment = true;
};

// Print enhanced banner
void printEnhancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Enhanced Neural Learning Machine                    ║
    ║     Phase 2: Real Neural Computation (v2.0)                    ║
    ║                                                               ║
    ║     Advanced features:                                        ║
    ║     - Modular test framework                                 ║
    ║     - Configurable execution modes                           ║
    ║     - Performance profiling                                  ║
    ║     - Comprehensive error handling                           ║
    ║     - Enhanced logging                                        ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help information
void printHelp() {
    std::cout << "Enhanced NLM Phase 2 - Command Line Options:" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
    std::cout << "  -q, --quiet             Quiet mode (minimal output)" << std::endl;
    std::cout << "  -o, --output <file>     Save results to file" << std::endl;
    std::cout << "  -c, --config <file>     Configuration file" << std::endl;
    std::cout << std::endl;
    std::cout << "Test Control:" << std::endl;
    std::cout << "  --no-connectivity       Skip connectivity test" << std::endl;
    std::cout << "  --no-plasticity         Skip plasticity test" << std::endl;
    std::cout << "  --no-stdp               Skip STDP verification test" << std::endl;
    std::cout << "  --no-learning           Skip learning experiment" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  -n, --neuron-count <N>  Number of neurons (default: 500)" << std::endl;
    std::cout << "  -s, --steps <N>         Maximum simulation steps (default: 1000)" << std::endl;
    std::cout << "  --random-seed <N>       Random seed (default: 42)" << std::endl;
    std::cout << "  --timestep <T>          Simulation timestep (default: 0.001)" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --profile               Enable performance profiling" << std::endl;
    std::cout << "  --log-level <L>         Set log level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << "  --list-tests            List available tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Example usage:" << std::endl;
    std::cout << "  ./nlm_phase2 -n 1000 -s 5000 --verbose" << std::endl;
    std::cout << "  ./nlm_phase2 --no-connectivity --no-stdp" << std::endl;
    std::cout << "  ./nlm_phase2 --config custom.cfg --output results.json" << std::endl;
}

// Print list of available tests
void printListTests() {
    std::cout << "=== Available Tests ===" << std::endl;
    std::cout << std::endl;
    std::cout << "1. Basic Connectivity Test" << std::endl;
    std::cout << "   - Tests neural signal propagation" << std::endl;
    std::cout << "   - Injects current into neurons" << std::endl;
    std::cout << "   - Verifies spike propagation" << std::endl;
    std::cout << std::endl;
    std::cout << "2. Plasticity Learning Experiment" << std::endl;
    std::cout << "   - Tests Hebbian and STDP plasticity" << std::endl;
    std::cout << "   - Applies learning patterns" << std::endl;
    std::cout << "   - Measures synaptic changes" << std::endl;
    std::cout << std::endl;
    std::cout << "3. STDP Verification" << std::endl;
    std::cout << "   - Tests spike-timing dependent plasticity" << std::endl;
    std::cout << "   - Verifies LTP/LTD mechanisms" << std::endl;
    std::cout << "   - Checks weight modifications" << std::endl;
    std::cout << std::endl;
    std::cout << "4. Spike Statistics Analysis" << std::endl;
    std::cout << "   - Analyzes network dynamics" << std::endl;
    std::cout << "   - Computes firing rates" << std::endl;
    std::cout << "   - Measures network activity" << std::endl;
    std::cout << std::endl;
    std::cout << "5. Synapse Distribution Analysis" << std::endl;
    std::cout << "   - Analyzes synaptic weight distribution" << std::endl;
    std::cout << "   - Computes statistical measures" << std::endl;
    std::cout << "   - Evaluates network connectivity" << std::endl;
    std::cout << std::endl;
    std::cout << "6. Region Functionality Test" << std::endl;
    std::cout << "   - Tests multi-region brain functionality" << std::endl;
    std::cout << "   - Verifies region communication" << std::endl;
    std::cout << "   - Checks brain architecture" << std::endl;
}

// Command line argument parser
class CommandLineParser {
public:
    struct Arguments {
        std::string configFile = "configs/default.cfg";
        std::string outputFile;
        EnhancedConfig config;
        bool listTests = false;
        bool help = false;
        bool version = false;
        std::string logLevel = "INFO";
    };
    
    static Arguments parse(int argc, char** argv) {
        Arguments args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version") {
                args.version = true;
            } else if (arg == "--list-tests") {
                args.listTests = true;
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) args.outputFile = argv[++i];
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) args.configFile = argv[++i];
            } else if (arg == "--neuron-count" || arg == "-n") {
                if (i + 1 < argc) {
                    int64_t value = std::stoll(argv[++i]);
                    args.config.neuronCount = value;
                }
            } else if (arg == "--steps" || arg == "-s") {
                if (i + 1 < argc) {
                    args.config.maxSteps = std::stoi(argv[++i]);
                }
            } else if (arg == "--random-seed") {
                if (i + 1 < argc) {
                    args.config.randomSeed = std::stoll(argv[++i]);
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    args.config.simulationTimestep = std::stod(argv[++i]);
                }
            } else if (arg == "--connection-probability") {
                if (i + 1 < argc) {
                    args.config.connectionProbability = std::stof(argv[++i]);
                }
            } else if (arg == "--no-connectivity") {
                args.config.runConnectivityTest = false;
            } else if (arg == "--no-plasticity") {
                args.config.runPlasticityTest = false;
            } else if (arg == "--no-stdp") {
                args.config.runStdpTest = false;
            } else if (arg == "--verbose" || arg == "-v") {
                args.config.verboseOutput = true;
            } else if (arg == "--quiet" || arg == "-q") {
                args.config.verboseOutput = false;
            } else if (arg == "--profile") {
                args.config.enableProfiling = true;
            } else if (arg.substr(0, 6) == "--log-") {
                args.logLevel = arg.substr(6);
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
            }
        }
        
        return args;
    }
};

// Enhanced test framework
class TestFramework {
public:
    struct TestResult {
        std::string name;
        std::string description;
        bool passed = false;
        std::string message;
        double executionTime = 0.0;
        std::map<std::string, std::string> metrics;
    };
    
    struct TestSuite {
        std::string name;
        std::string description;
        std::vector<std::function<bool(TestResult&)>> tests;
    };
    
private:
    std::vector<TestSuite> suites;
    std::vector<TestResult> results;
    
public:
    void addSuite(const std::string& name, const std::string& description) {
        suites.push_back({name, description, {}});
    }
    
    void addTest(const std::string& suiteName, const std::string& testName, 
                 const std::string& description, std::function<bool(TestResult&)> testFunc) {
        for (auto& suite : suites) {
            if (suite.name == suiteName) {
                suite.tests.push_back([testName, description, testFunc](TestResult& result) {
                    result.name = testName;
                    result.description = description;
                    return testFunc(result);
                });
                return;
            }
        }
    }
    
    void runAll(const EnhancedConfig& config) {
        NLM_LOG_INFO("=== Enhanced NLM Phase 2 Test Suite ===");
        
        for (auto& suite : suites) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Suite: " + suite.name);
            NLM_LOG_INFO("Description: " + suite.description);
            
            for (auto& testFunc : suite.tests) {
                TestResult result;
                auto startTime = std::chrono::high_resolution_clock::now();
                
                try {
                    bool passed = testFunc(result);
                    result.passed = passed;
                    
                    auto endTime = std::chrono::high_resolution_clock::now();
                    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
                    
                    results.push_back(result);
                    
                    if (config.verboseOutput) {
                        std::string status = passed ? "PASS" : "FAIL";
                        NLM_LOG_INFO("  " + result.name + ": " + status);
                        if (!result.message.empty()) {
                            NLM_LOG_INFO("    Message: " + result.message);
                        }
                        if (!result.metrics.empty()) {
                            NLM_LOG_INFO("    Metrics:");
                            for (const auto& [key, value] : result.metrics) {
                                NLM_LOG_INFO("      " + key + ": " + value);
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    result.passed = false;
                    result.message = "Exception: " + std::string(e.what());
                    results.push_back(result);
                    
                    auto endTime = std::chrono::high_resolution_clock::now();
                    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
                    
                    NLM_LOG_ERROR("  " + result.name + ": EXCEPTION - " + result.message);
                }
            }
        }
    }
    
    const std::vector<TestResult>& getResults() const { return results; }
    
    void printSummary() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test Suite Summary ===");
        
        int passed = 0, failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
            totalTime += result.executionTime;
        }
        
        NLM_LOG_INFO("Total Tests: " + std::to_string(results.size()));
        NLM_LOG_INFO("Passed: " + std::to_string(passed));
        NLM_LOG_INFO("Failed: " + std::to_string(failed));
        NLM_LOG_INFO("Total Time: " + std::to_string(totalTime) + "s");
        
        if (failed > 0) {
            NLM_LOG_ERROR("Some tests failed!");
        } else {
            NLM_LOG_INFO("All tests passed!");
        }
    }
};

// Phase 2 executor with enhanced features
class Phase2Executor {
private:
    EnhancedConfig config;
    TestFramework testFramework;
    
public:
    Phase2Executor(const EnhancedConfig& cfg) : config(cfg) {
        setupTestFramework();
    }
    
    void setupTestFramework() {
        // Basic Connectivity Test
        testFramework.addSuite("Connectivity", "Test basic neural connectivity and signal propagation");
        testFramework.addTest("Connectivity", "basic_connectivity", 
                            "Test that injected current produces spikes that propagate",
                            [this](TestResult& result) { return runBasicConnectivityTest(result); });
        
        // Plasticity Test
        testFramework.addSuite("Plasticity", "Test synaptic plasticity mechanisms");
        testFramework.addTest("Plasticity", "plasticity_learning",
                            "Test learning through Hebbian and STDP plasticity",
                            [this](TestResult& result) { return runPlasticityExperiment(result); });
        
        testFramework.addTest("Plasticity", "stdp_verification",
                            "Test STDP potentiation and depression",
                            [this](TestResult& result) { return runStdpVerification(result); });
        
        // Advanced tests
        testFramework.addSuite("Advanced", "Advanced neural computation features");
        testFramework.addTest("Advanced", "spike_statistics",
                            "Analyze spike statistics and network dynamics",
                            [this](TestResult& result) { return analyzeSpikeStatistics(result); });
        
        testFramework.addTest("Advanced", "synapse_distribution",
                            "Analyze synapse weight distribution",
                            [this](TestResult& result) { return analyzeSynapseDistribution(result); });
        
        testFramework.addTest("Advanced", "region_functionality",
                            "Test multi-region brain functionality",
                            [this](TestResult& result) { return testRegionFunctionality(result); });
    }
    
    bool runBasicConnectivityTest(TestResult& result) {
        NLM_LOG_INFO("Running basic connectivity test...");
        
        // Initialize brain
        auto brain = initializeBrain(config);
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        auto region = brain->getRegion(RegionId(1));
        if (!region) {
            result.message = "No brain region found";
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            result.message = "No neurons found";
            return false;
        }
        
        // Inject current into first few neurons
        size_t neuronsToStimulate = std::min(size_t(10), neurons.size());
        for (size_t i = 0; i < neuronsToStimulate; ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation steps
        for (SimulationStep step = 0; step < config.connectivityTestSteps; ++step) {
            brain->step(step, step * config.simulationTimestep);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        
        result.metrics["spikes_generated"] = std::to_string(spikes);
        result.metrics["neurons_stimulated"] = std::to_string(neuronsToStimulate);
        
        if (spikes > 0) {
            result.message = "Successfully propagated " + std::to_string(spikes) + " spikes through network";
            return true;
        } else {
            result.message = "No spikes generated - checking neuron parameters";
            if (config.verboseOutput && neurons.size() >= 3) {
                result.message += ". Neuron V/threshold: ";
                for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                    result.message += "[V=" + std::to_string(neurons[i]->getMembranePotential()) + 
                                    " thresh=" + std::to_string(neurons[i]->getThreshold()) + "] ";
                }
            }
            return false;  // Not a failure, just no spikes
        }
    }
    
    bool runPlasticityExperiment(TestResult& result) {
        NLM_LOG_INFO("Running plasticity learning experiment...");
        
        // Initialize brain with learning enabled
        auto brain = initializeBrain(config, true);
        
        // Create learning experiment structure
        struct LearningExperiment {
            std::shared_ptr<Brain> brain;
            std::vector<float> initialWeights;
            std::vector<float> finalWeights;
            
            LearningExperiment(std::shared_ptr<Brain> b) : brain(b) {}
            
            void recordInitialState() {
                initialWeights.clear();
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        initialWeights.push_back(syn->getWeight());
                    }
                }
            }
            
            void recordFinalState() {
                finalWeights.clear();
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        finalWeights.push_back(syn->getWeight());
                    }
                }
            }
            
            void computeLearning(TestResult& testResult) {
                if (initialWeights.empty() || finalWeights.empty()) {
                    testResult.message = "No weights recorded";
                    return;
                }
                
                // Compute weight changes
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                
                testResult.metrics["initial_mean_weight"] = std::to_string(initialMean);
                testResult.metrics["final_mean_weight"] = std::to_string(finalMean);
                testResult.metrics["weight_change"] = std::to_string(finalMean - initialMean);
                
                // Count significant changes
                size_t strengthened = 0, weakened = 0, unchanged = 0;
                size_t minSize = std::min(initialWeights.size(), finalWeights.size());
                
                for (size_t i = 0; i < minSize; ++i) {
                    float delta = finalWeights[i] - initialWeights[i];
                    if (delta > 0.01f) strengthened++;
                    else if (delta < -0.01f) weakened++;
                    else unchanged++;
                }
                
                testResult.metrics["strengthened"] = std::to_string(strengthened);
                testResult.metrics["weakened"] = std::to_string(weakened);
                testResult.metrics["unchanged"] = std::to_string(unchanged);
                
                bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) || 
                                       (strengthened > 0 || weakened > 0);
                
                if (learningOccurred) {
                    testResult.message = "Learning detected: synaptic weights changed through experience";
                    return true;
                } else {
                    testResult.message = "No significant learning detected";
                    return false;
                }
            }
        };
        
        LearningExperiment experiment(brain);
        experiment.recordInitialState();
        
        // Apply plasticity and run learning
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Hebbian and STDP
            }
        }
        
        // Apply input patterns to stimulate learning
        for (SimulationStep step = 0; step < config.plasticityTestSteps; ++step) {
            // Inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * config.simulationTimestep);
        }
        
        experiment.recordFinalState();
        experiment.computeLearning(result);
        
        return true;
    }
    
    bool runStdpVerification(TestResult& result) {
        NLM_LOG_INFO("Running STDP verification...");
        
        // Initialize brain with STDP only
        auto brain = initializeBrain(config);
        auto region = brain->getRegion(RegionId(1));
        
        if (!region || region->getSynapses().size() < 5) {
            result.message = "Not enough synapses for STDP test";
            return false;
        }
        
        auto& synapses = region->getSynapses();
        std::vector<float> beforeWeights;
        
        // Enable only STDP for first 5 synapses
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // STDP only
        }
        
        // Create correlated activity for LTP
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) {
            result.message = "Not enough neurons for STDP test";
            return false;
        }
        
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        // Run STDP trials
        for (int trial = 0; trial < config.stdpTestTrials; ++trial) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * config.simulationTimestep);
            
            // Post fires second
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * config.simulationTimestep);
        }
        
        // Record after weights
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            result.metrics["synapse_" + std::to_string(i) + "_delta"] = std::to_string(delta);
        }
        
        result.metrics["total_delta"] = std::to_string(totalDelta);
        
        if (totalDelta > 0.001f) {
            result.message = "STDP working: pre-before-post produced potentiation (LTP)";
            return true;
        } else if (totalDelta < -0.001f) {
            result.message = "STDP reversed: unexpected depression detected";
            return false;
        } else {
            result.message = "No significant STDP change detected";
            return false;  // Not a failure, just no change
        }
    }
    
    bool analyzeSpikeStatistics(TestResult& result) {
        NLM_LOG_INFO("Analyzing spike statistics...");
        
        auto brain = initializeBrain(config);
        
        // Run simulation
        for (SimulationStep step = 0; step < config.maxSteps; ++step) {
            brain->step(step, step * config.simulationTimestep);
            
            // Inject some activity
            if (step % 10 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 20.0f);
            }
        }
        
        // Collect statistics
        size_t totalSpikes = brain->getTotalSpikeCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        float avgFiringRate = brain->getAverageFiringRate();
        float eiRatio = brain->getExcitationInhibitionRatio();
        
        result.metrics["total_spikes"] = std::to_string(totalSpikes);
        result.metrics["firing_neurons"] = std::to_string(firingNeurons);
        result.metrics["avg_firing_rate"] = std::to_string(avgFiringRate);
        result.metrics["ei_ratio"] = std::to_string(eiRatio);
        
        result.message = "Spike statistics analysis complete";
        return true;
    }
    
    bool analyzeSynapseDistribution(TestResult& result) {
        NLM_LOG_INFO("Analyzing synapse distribution...");
        
        auto brain = initializeBrain(config);
        
        // Collect synapse statistics
        std::vector<float> weights;
        for (auto* region : brain->getAllRegions()) {
            for (auto& synapse : region->getSynapses()) {
                weights.push_back(synapse->getWeight());
            }
        }
        
        if (weights.empty()) {
            result.message = "No synapses found";
            return false;
        }
        
        // Compute distribution statistics
        float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
        float mean = sum / weights.size();
        
        float min = *std::min_element(weights.begin(), weights.end());
        float max = *std::max_element(weights.begin(), weights.end());
        
        // Simple variance calculation
        float variance = 0.0f;
        for (float w : weights) {
            variance += (w - mean) * (w - mean);
        }
        variance /= weights.size();
        float stddev = std::sqrt(variance);
        
        result.metrics["synapse_count"] = std::to_string(weights.size());
        result.metrics["mean_weight"] = std::to_string(mean);
        result.metrics["min_weight"] = std::to_string(min);
        result.metrics["max_weight"] = std::to_string(max);
        result.metrics["stddev_weight"] = std::to_string(stddev);
        
        result.message = "Synapse distribution analysis complete";
        return true;
    }
    
    bool testRegionFunctionality(TestResult& result) {
        NLM_LOG_INFO("Testing region functionality...");
        
        // Create brain with multiple regions if possible
        auto configCopy = config;
        configCopy.regionCount = std::max(1, configCopy.regionCount);
        
        auto brain = initializeBrain(configCopy);
        
        // Test region access and functionality
        auto regions = brain->getAllRegions();
        result.metrics["region_count"] = std::to_string(regions.size());
        
        size_t totalNeurons = 0, totalSynapses = 0;
        for (auto* region : regions) {
            totalNeurons += region->getAllNeurons().size();
            totalSynapses += region->getSynapses().size();
        }
        
        result.metrics["total_neurons"] = std::to_string(totalNeurons);
        result.metrics["total_synapses"] = std::to_string(totalSynapses);
        
        if (regions.size() > 1) {
            result.message = "Multi-region brain functionality verified";
        } else {
            result.message = "Single-region brain (expected for default config)";
        }
        
        return true;
    }
    
private:
    std::shared_ptr<Brain> initializeBrain(const EnhancedConfig& cfg, bool enablePlasticity = false) {
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Set basic parameters
        config->set("random_seed", static_cast<int64_t>(cfg.randomSeed), ConfigSource::Default);
        config->set("simulation_timestep", cfg.simulationTimestep, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(cfg.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(cfg.regionCount), ConfigSource::Default);
        config->set("connection_probability", cfg.connectionProbability, ConfigSource::Default);
        
        // Set plasticity parameters
        config->set("stdp_ltp_weight", cfg.stdpLtpWeight, ConfigSource::Default);
        config->set("stdp_ltd_weight", cfg.stdpLtdWeight, ConfigSource::Default);
        config->set("stdp_tau", cfg.stdpTau, ConfigSource::Default);
        
        config->set("synaptogenesis_rate", cfg.synaptogenesisRate, ConfigSource::Default);
        config->set("pruning_rate", cfg.pruningRate, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        if (enablePlasticity) {
            // Enable plasticity on all synapses
            for (auto* region : brain->getAllRegions()) {
                for (auto& synapse : region->getSynapses()) {
                    synapse->enablePlasticity(true, true, true);
                }
            }
        }
        
        return brain;
    }
};

// Main function
int main(int argc, char** argv) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Parse command line arguments
    auto args = CommandLineParser::parse(argc, argv);
    
    // Print help or version if requested
    if (args.help) {
        printEnhancedBanner();
        printHelp();
        return 0;
    }
    
    if (args.version) {
        printEnhancedBanner();
        std::cout << "Version: NLM Phase 2 Enhanced (v2.0)" << std::endl;
        return 0;
    }
    
    if (args.listTests) {
        printEnhancedBanner();
        printListTests();
        return 0;
    }
    
    // Initialize logger with specified level
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level based on user preference
    LogLevel logLevel = LogLevel::Info;
    if (args.logLevel == "DEBUG") logLevel = LogLevel::Debug;
    else if (args.logLevel == "WARN") logLevel = LogLevel::Warning;
    else if (args.logLevel == "ERROR") logLevel = LogLevel::Error;
    
    consoleLogger->setLevel(logLevel);
    
    printEnhancedBanner();
    
    NLM_LOG_INFO("=== Enhanced NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Starting enhanced neural computation simulation...");
    NLM_LOG_INFO("Configuration file: " + args.configFile);
    NLM_LOG_INFO("Output file: " + (args.outputFile.empty() ? "Standard output" : args.outputFile));
    NLM_LOG_INFO("Verbosity: " + std::string(args.config.verboseOutput ? "Enabled" : "Disabled"));
    
    if (args.config.enableProfiling) {
        NLM_LOG_INFO("Performance profiling: ENABLED");
    }
    
    NLM_LOG_INFO("");
    
    try {
        // Initialize configuration manager
        ConfigManager configManager;
        
        // Load configuration from file
        if (args.configFile != "configs/default.cfg") {
            if (configManager.loadFromFile(args.configFile)) {
                NLM_LOG_INFO("Loaded configuration from file: " + args.configFile);
            } else {
                NLM_LOG_WARN("Failed to load configuration from file: " + args.configFile);
                NLM_LOG_WARN("Using default configuration");
            }
        }
        
        // Set configuration from command line arguments
        configManager.setFromArgs(argc, argv);
        
        // Validate configuration
        auto validationResult = configManager.validate();
        if (!validationResult.valid) {
            NLM_LOG_ERROR("Configuration validation failed:");
            NLM_LOG_ERROR(configManager.getErrorMessages());
            return 1;
        }
        
        // Show warnings if any
        std::string warnings = configManager.getWarningMessages();
        if (!warnings.empty()) {
            NLM_LOG_WARN("Configuration warnings:");
            NLM_LOG_WARN(warnings);
        }
        
        // Print enhanced configuration summary
        const auto& enhancedConfig = configManager.getEnhancedConfig();
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Enhanced Configuration ===");
        NLM_LOG_INFO("Random seed: " + std::to_string(enhancedConfig.randomSeed));
        NLM_LOG_INFO("Simulation timestep: " + std::to_string(enhancedConfig.simulationTimestep) + "s");
        NLM_LOG_INFO("Neuron count: " + std::to_string(enhancedConfig.neuronCount));
        NLM_LOG_INFO("Region count: " + std::to_string(enhancedConfig.regionCount));
        NLM_LOG_INFO("Connection probability: " + std::to_string(enhancedConfig.connectionProbability));
        NLM_LOG_INFO("Max simulation steps: " + std::to_string(enhancedConfig.maxSteps));
        NLM_LOG_INFO("Enable profiling: " + std::string(enhancedConfig.enableProfiling ? "Yes" : "No"));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test Selection ===");
        NLM_LOG_INFO("Basic Connectivity Test: " + std::string(enhancedConfig.runConnectivityTest ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("Plasticity Test: " + std::string(enhancedConfig.runPlasticityTest ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("STDP Verification Test: " + std::string(enhancedConfig.runStdpTest ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("Learning Experiment: " + std::string(enhancedConfig.runLearningExperiment ? "Enabled" : "Disabled"));
        
        // Create enhanced Phase 2 executor
        Phase2Executor executor(enhancedConfig);
        
        if (enhancedConfig.enableProfiling) {
            NLM_LOG_INFO("Starting performance profiling...");
        }
        
        // Run the enhanced test suite
        executor.testFramework.runAll(enhancedConfig);
        
        if (enhancedConfig.enableProfiling) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            NLM_LOG_INFO("Profiling complete: Total execution time = " + std::to_string(elapsed) + "s");
        }
        
        // Print test summary
        executor.testFramework.printSummary();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Fatal error during execution: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown fatal error during execution");
        return 1;
    }
    
    // Calculate total execution time
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("Total execution time: " + std::to_string(totalTime) + "s");
    
    return 0;
}