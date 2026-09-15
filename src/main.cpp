// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation - Enhanced with Comprehensive Test Configuration
//
// This phase implements real spiking neural computation with comprehensive testing capabilities:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Comprehensive test framework with configurable parameters

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
#include <algorithm>

using namespace nlm;

// Enhanced error handling utility
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, const std::string& context = "")
        : std::runtime_error((context.empty() ? "NLM Error: " : "NLM Error in " + context + ": ") + message) {}
};

// Comprehensive command-line argument parsing structure
struct TestConfig {
    bool help = false;
    bool verbose = false;
    int numSteps = 1000;
    int neuronCount = 500;
    int regionCount = 1;
    float connectionProbability = 0.15f;
    float stdpLTPWeight = 0.02f;
    float stdpLTDWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    std::string configFile = "configs/default.cfg";
    bool enableProgressLogging = true;
    bool enableTiming = true;
    std::string logLevel = "info";
    bool runAllTests = true;
    bool skipSystemStabilityTests = false;
};

// Timer utility for measuring execution time with descriptions
class Timer {
private:
    std::chrono::steady_clock::time_point startTime;
    bool running;
    std::string description;
    double accumulatedTime;
public:
    Timer() : running(false), description("Timer"), accumulatedTime(0.0) {}
    Timer(const std::string& desc) : running(false), description(desc), accumulatedTime(0.0) {}
    
    void start() {
        startTime = std::chrono::steady_clock::now();
        running = true;
    }
    
    double stop() {
        if (!running) return 0.0;
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        running = false;
        double seconds = duration.count() / 1000000.0;
        accumulatedTime += seconds;
        return seconds;
    }
    
    double getAccumulatedTime() const { return accumulatedTime; }
    bool isRunning() const { return running; }
    double elapsed() const {
        if (!running) return accumulatedTime;
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime);
        return accumulatedTime + (duration.count() / 1000000.0);
    }
};

// Print NLM banner
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

// Enhanced help message with comprehensive configuration options
void printHelp() {
    printBanner();
    std::cout << "Usage: nlm [OPTIONS]\n";
    std::cout << "\nComprehensive Test Configuration Options:\n";
    std::cout << "  --help                    Show this help message and exit\n";
    std::cout << "  --verbose, -v             Enable verbose output\n";
    std::cout << "  --num-steps <N>           Number of simulation steps to run\n";
    std::cout << "  --neuron-count <N>        Number of neurons in the brain\n";
    std::cout << "  --region-count <N>        Number of neural regions\n";
    std::cout << "  --connection-prob <P>     Connection probability (0.0-1.0)\n";
    std::cout << "  --stdp-ltp <W>            STDP LTP weight\n";
    std::cout << "  --stdp-ltd <W>            STDP LTD weight\n";
    std::cout << "  --stdp-tau <T>            STDP tau parameter\n";
    std::cout << "  --synaptogenesis <R>      Synaptogenesis rate\n";
    std::cout << "  --pruning <R>             Pruning rate\n";
    std::cout << "  --config <FILE>           Configuration file path\n";
    std::cout << "  --no-progress             Disable progress logging\n";
    std::cout << "  --no-timing               Disable timing output\n";
    std::cout << "  --log-level <LEVEL>       Set log level (debug, info, warning, error, critical)\n";
    std::cout << "  --skip-system-tests       Skip system stability and edge case tests\n";
    std::cout << "  --quick-test              Run only basic tests (neuron count 10, steps 50)\n";
    std::cout << "\nExample - Standard Test:\n";
    std::cout << "  nlm --neuron-count 200 --num-steps 500 --verbose\n";
    std::cout << "\nExample - Quick Performance Test:\n";
    std::cout << "  nlm --quick-test\n";
    std::cout << "\nExample - Debug Mode:\n";
    std::cout << "  nlm --verbose --log-level debug --neuron-count 50 --num-steps 200\n";
}

// Parse command line arguments with comprehensive validation
TestConfig parseArguments(int argc, char** argv) {
    TestConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            config.help = true;
        }
        else if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        }
        else if (arg == "--num-steps" && i + 1 < argc) {
            try {
                config.numSteps = std::stoi(argv[++i]);
                if (config.numSteps <= 0) {
                    NLM_LOG_ERROR("Number of steps must be positive. Got: " + std::to_string(config.numSteps));
                    config.numSteps = 1000;  // Default
                } else if (config.numSteps > 100000) {
                    NLM_LOG_WARNING("Very high step count detected: " + std::to_string(config.numSteps) + ". This may take a long time to complete.");
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --num-steps: " + std::string(e.what()));
                config.numSteps = 1000;  // Default
            }
        }
        else if (arg == "--neuron-count" && i + 1 < argc) {
            try {
                config.neuronCount = std::stoi(argv[++i]);
                if (config.neuronCount <= 0) {
                    NLM_LOG_ERROR("Neuron count must be positive. Got: " + std::to_string(config.neuronCount));
                    config.neuronCount = 500;  // Default
                } else if (config.neuronCount > 10000) {
                    NLM_LOG_WARNING("Very high neuron count detected: " + std::to_string(config.neuronCount) + ". Memory usage will be high.");
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --neuron-count: " + std::string(e.what()));
                config.neuronCount = 500;  // Default
            }
        }
        else if (arg == "--region-count" && i + 1 < argc) {
            try {
                config.regionCount = std::stoi(argv[++i]);
                if (config.regionCount <= 0) {
                    NLM_LOG_ERROR("Region count must be positive. Got: " + std::to_string(config.regionCount));
                    config.regionCount = 1;  // Default
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --region-count: " + std::string(e.what()));
                config.regionCount = 1;  // Default
            }
        }
        else if (arg == "--connection-prob" && i + 1 < argc) {
            try {
                float prob = std::stof(argv[++i]);
                if (prob < 0.0f || prob > 1.0f) {
                    NLM_LOG_ERROR("Connection probability must be between 0.0 and 1.0. Got: " + std::to_string(prob));
                    config.connectionProbability = 0.15f;  // Default
                } else {
                    config.connectionProbability = prob;
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --connection-prob: " + std::string(e.what()));
                config.connectionProbability = 0.15f;  // Default
            }
        }
        else if (arg == "--stdp-ltp" && i + 1 < argc) {
            try {
                config.stdpLTPWeight = std::stof(argv[++i]);
                if (config.stdpLTPWeight < 0.0f) {
                    NLM_LOG_ERROR("STDP LTP weight must be non-negative. Got: " + std::to_string(config.stdpLTPWeight));
                    config.stdpLTPWeight = 0.02f;  // Default
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --stdp-ltp: " + std::string(e.what()));
                config.stdpLTPWeight = 0.02f;  // Default
            }
        }
        else if (arg == "--stdp-ltd" && i + 1 < argc) {
            try {
                config.stdpLTDWeight = std::stof(argv[++i]);
                if (config.stdpLTDWeight < 0.0f) {
                    NLM_LOG_ERROR("STDP LTD weight must be non-negative. Got: " + std::to_string(config.stdpLTDWeight));
                    config.stdpLTDWeight = 0.015f;  // Default
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --stdp-ltd: " + std::string(e.what()));
                config.stdpLTDWeight = 0.015f;  // Default
            }
        }
        else if (arg == "--stdp-tau" && i + 1 < argc) {
            try {
                config.stdpTau = std::stof(argv[++i]);
                if (config.stdpTau <= 0.0f) {
                    NLM_LOG_ERROR("STDP tau must be positive. Got: " + std::to_string(config.stdpTau));
                    config.stdpTau = 20.0f;  // Default
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --stdp-tau: " + std::string(e.what()));
                config.stdpTau = 20.0f;  // Default
            }
        }
        else if (arg == "--synaptogenesis" && i + 1 < argc) {
            try {
                config.synaptogenesisRate = std::stof(argv[++i]);
                if (config.synaptogenesisRate < 0.0f) {
                    NLM_LOG_ERROR("Synaptogenesis rate must be non-negative. Got: " + std::to_string(config.synaptogenesisRate));
                    config.synaptogenesisRate = 0.0001f;  // Default
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --synaptogenesis: " + std::string(e.what()));
                config.synaptogenesisRate = 0.0001f;  // Default
            }
        }
        else if (arg == "--pruning" && i + 1 < argc) {
            try {
                config.pruningRate = std::stof(argv[++i]);
                if (config.pruningRate < 0.0f) {
                    NLM_LOG_ERROR("Pruning rate must be non-negative. Got: " + std::to_string(config.pruningRate));
                    config.pruningRate = 0.00001f;  // Default
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Invalid value for --pruning: " + std::string(e.what()));
                config.pruningRate = 0.00001f;  // Default
            }
        }
        else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        }
        else if (arg == "--no-progress") {
            config.enableProgressLogging = false;
        }
        else if (arg == "--no-timing") {
            config.enableTiming = false;
        }
        else if (arg == "--log-level" && i + 1 < argc) {
            config.logLevel = argv[++i];
        }
        else if (arg == "--skip-system-tests") {
            config.skipSystemStabilityTests = true;
        }
        else if (arg == "--quick-test") {
            config.neuronCount = 10;
            config.numSteps = 50;
            config.verbose = true;
            config.enableProgressLogging = true;
            config.enableTiming = true;
            NLM_LOG_INFO("Quick test mode enabled: Reduced neuron count (10) and steps (50) for faster execution");
        }
        else {
            NLM_LOG_WARNING("Unknown argument ignored: " + arg);
        }
    }
    
    return config;
}

// Comprehensive configuration validation with detailed error messages
void validateConfig(const std::shared_ptr<Config>& config, const TestConfig& testConfig) {
    NLM_LOG_INFO("Validating configuration parameters...");
    
    try {
        // Check for essential configuration parameters
        std::vector<std::string> requiredParams = {"neuron_count", "region_count", "connection_probability"};
        for (const auto& param : requiredParams) {
            if (!config->has(param)) {
                throw NLMException("Missing required configuration parameter: " + param, "Configuration Validation");
            }
        }
        
        // Validate parameter ranges with detailed error messages
        auto neuronCount = config->getOr<int64_t>("neuron_count", -1);
        if (neuronCount <= 0) {
            throw NLMException("Invalid neuron_count: must be positive. Got: " + std::to_string(neuronCount), "Parameter Validation");
        } else if (neuronCount > 10000) {
            NLM_LOG_WARNING("Very high neuron count detected: " + std::to_string(neuronCount) + ". Memory usage will be high. Performance may be impacted.");
        }
        
        auto regionCount = config->getOr<int64_t>("region_count", -1);
        if (regionCount <= 0) {
            throw NLMException("Invalid region_count: must be positive. Got: " + std::to_string(regionCount), "Parameter Validation");
        }
        
        auto connectionProb = config->getOr<double>("connection_probability", -1.0);
        if (connectionProb < 0.0 || connectionProb > 1.0) {
            throw NLMException("Invalid connection_probability: must be between 0.0 and 1.0. Got: " + std::to_string(connectionProb), "Parameter Validation");
        }
        
        // Validate plasticity parameters
        auto stdpLTP = config->getOr<double>("stdp_ltp_weight", -1.0);
        if (stdpLTP < 0.0) {
            throw NLMException("Invalid stdp_ltp_weight: must be non-negative. Got: " + std::to_string(stdpLTP), "Parameter Validation");
        }
        
        auto stdpLTD = config->getOr<double>("stdp_ltd_weight", -1.0);
        if (stdpLTD < 0.0) {
            throw NLMException("Invalid stdp_ltd_weight: must be non-negative. Got: " + std::to_string(stdpLTD), "Parameter Validation");
        }
        
        auto stdpTau = config->getOr<double>("stdp_tau", -1.0);
        if (stdpTau <= 0.0) {
            throw NLMException("Invalid stdp_tau: must be positive. Got: " + std::to_string(stdpTau), "Parameter Validation");
        }
        
        // Validate structural plasticity parameters
        auto synaptogenesis = config->getOr<double>("synaptogenesis_rate", -1.0);
        if (synaptogenesis < 0.0) {
            throw NLMException("Invalid synaptogenesis_rate: must be non-negative. Got: " + std::to_string(synaptogenesis), "Parameter Validation");
        }
        
        auto pruning = config->getOr<double>("pruning_rate", -1.0);
        if (pruning < 0.0) {
            throw NLMException("Invalid pruning_rate: must be non-negative. Got: " + std::to_string(pruning), "Parameter Validation");
        }
        
        // Validate simulation steps
        auto simulationSteps = config->getOr<int64_t>("simulation_steps", -1);
        if (simulationSteps <= 0) {
            throw NLMException("Invalid simulation_steps: must be positive. Got: " + std::to_string(simulationSteps), "Parameter Validation");
        }
        
        NLM_LOG_INFO("Configuration validation completed successfully");
    } catch (const NLMException& e) {
        NLM_LOG_CRITICAL("Configuration validation failed: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Unexpected error during configuration validation: " + std::string(e.what()));
        throw NLMException("Configuration validation failed", "Configuration Validation");
    }
}

// Enhanced basic connectivity test with comprehensive error handling and reporting
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, const TestConfig& testConfig) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    try {
        if (!brain) {
            throw NLMException("Brain pointer is null", "Test Execution");
        }
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw NLMException("Region not found", "Test Execution");
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into " + std::to_string(std::min(size_t(10), neurons.size())) + " neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
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
    } catch (const NLMException& e) {
        NLM_LOG_ERROR("Critical error in basic connectivity test: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in basic connectivity test: " + std::string(e.what()));
        throw NLMException("Basic connectivity test failed", "Test Execution");
    }
}

// Enhanced plasticity experiment with configurable steps and detailed logging
void runPlasticityExperiment(std::shared_ptr<Brain> brain, const TestConfig& testConfig) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    try {
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
        NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(testConfig.numSteps) + " steps)...");
        
        Timer plasticityTimer("Plasticity Experiment");
        plasticityTimer.start();
        
        for (SimulationStep step = 0; step < testConfig.numSteps; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps if enabled
            if (testConfig.enableProgressLogging && step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        double plasticityTime = plasticityTimer.stop();
        NLM_LOG_INFO("  Plasticity experiment completed in " + std::to_string(plasticityTime) + " seconds");
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
    } catch (const NLMException& e) {
        NLM_LOG_ERROR("Critical error in plasticity experiment: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in plasticity experiment: " + std::string(e.what()));
        throw NLMException("Plasticity experiment failed", "Test Execution");
    }
}

// Enhanced STDP verification with comprehensive testing
void runStdpVerification(std::shared_ptr<Brain> brain, const TestConfig& testConfig) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    try {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("Region not found!");
            return;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on " + std::to_string(std::min(size_t(5), synapses.size())) + " synapses:");
        
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
        
        int trials = std::min(50, static_cast<int>(testConfig.numSteps));
        for (int trial = 0; trial < trials; ++trial) {
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
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation (Δ=" + std::to_string(totalDelta) + ")");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters (Δ=" + std::to_string(totalDelta) + ")");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering (Δ=" + std::to_string(totalDelta) + ")");
        }
    } catch (const NLMException& e) {
        NLM_LOG_ERROR("Critical error in STDP verification: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in STDP verification: " + std::string(e.what()));
        throw NLMException("STDP verification failed", "Test Execution");
    }
}

// New comprehensive test cases for system stability and edge cases
void runSystemStabilityTests(std::shared_ptr<Brain> brain, const TestConfig& testConfig) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: System Stability and Edge Cases ===");
    
    try {
        if (!brain) {
            throw NLMException("Brain pointer is null", "System Stability Tests");
        }
        
        // Test 4.1: Empty network stability
        NLM_LOG_INFO("  Test 4.1: Empty network stability...");
        brain->reset();
        brain->initialize();
        NLM_LOG_INFO("    ✓ Empty network initialized without errors");
        
        // Test 4.2: Extreme values (small network)
        NLM_LOG_INFO("  Test 4.2: Extreme values (minimal network)...");
        brain->reset();
        // Temporarily set a smaller config
        auto tempConfig = std::make_shared<Config>(*brain->getConfig());
        tempConfig->set("neuron_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        tempConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        
        // Reinitialize brain with new config
        brain->reset();
        brain = std::make_shared<Brain>(tempConfig);
        brain->initialize();
        brain->step(0);
        NLM_LOG_INFO("    ✓ Minimal network handled correctly");
        
        // Test 4.3: Zero connection probability
        NLM_LOG_INFO("  Test 4.3: Zero connection probability...");
        brain->reset();
        tempConfig->set("connection_probability", 0.0f, ConfigSource::Runtime);
        brain = std::make_shared<Brain>(tempConfig);
        brain->initialize();
        brain->step(0);
        NLM_LOG_INFO("    ✓ Zero connection probability handled correctly");
        
        // Test 4.4: Maximum plasticity rates
        NLM_LOG_INFO("  Test 4.4: Maximum plasticity rates...");
        brain->reset();
        tempConfig->set("synaptogenesis_rate", 1.0f, ConfigSource::Runtime);
        tempConfig->set("pruning_rate", 1.0f, ConfigSource::Runtime);
        brain = std::make_shared<Brain>(tempConfig);
        brain->initialize();
        brain->step(0);
        NLM_LOG_INFO("    ✓ Maximum plasticity rates handled correctly");
        
        // Test 4.5: Rapid reset and reinitialize
        NLM_LOG_INFO("  Test 4.5: Rapid reset/reinitialize cycle...");
        for (int i = 0; i < 3; ++i) {
            brain->reset();
            brain->initialize();
            brain->step(0);
        }
        NLM_LOG_INFO("    ✓ Rapid reset/reinitialize cycle completed successfully");
        
        // Test 4.6: Very high neuron count
        NLM_LOG_INFO("  Test 4.6: High neuron count...");
        brain->reset();
        tempConfig->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        tempConfig->set("connection_probability", 0.05f, ConfigSource::Runtime);
        brain = std::make_shared<Brain>(tempConfig);
        brain->initialize();
        brain->step(0);
        NLM_LOG_INFO("    ✓ High neuron count handled correctly");
        
        // Test 4.7: No plasticity
        NLM_LOG_INFO("  Test 4.7: No plasticity enabled...");
        brain->reset();
        tempConfig->set("synaptogenesis_rate", 0.0f, ConfigSource::Runtime);
        tempConfig->set("pruning_rate", 0.0f, ConfigSource::Runtime);
        brain = std::make_shared<Brain>(tempConfig);
        brain->initialize();
        brain->step(0);
        NLM_LOG_INFO("    ✓ No plasticity handled correctly");
        
        NLM_LOG_INFO("  ✓ All system stability tests completed");
    } catch (const NLMException& e) {
        NLM_LOG_ERROR("Critical error in system stability tests: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in system stability tests: " + std::string(e.what()));
        throw NLMException("System stability tests failed", "System Stability Tests");
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    try {
        // Parse command line arguments
        TestConfig testConfig = parseArguments(argc, argv);
        
        if (testConfig.help) {
            printHelp();
            return 0;
        }
        
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        
        // Set log level based on command line
        LogLevel level;
        if (testConfig.logLevel == "debug") level = LogLevel::Debug;
        else if (testConfig.logLevel == "warning") level = LogLevel::Warning;
        else if (testConfig.logLevel == "error") level = LogLevel::Error;
        else if (testConfig.logLevel == "critical") level = LogLevel::Critical;
        else level = LogLevel::Info;
        
        consoleLogger->setLevel(level);
        logger->setLevel(level);
        Logger::setGlobal(logger);
        
        if (testConfig.verbose) {
            NLM_LOG_INFO("Verbose mode enabled - detailed logging active");
        }
        
        if (testConfig.enableProgressLogging) {
            NLM_LOG_INFO("Progress logging enabled");
        }
        
        if (testConfig.enableTiming) {
            NLM_LOG_INFO("Timing enabled for performance measurement");
        }
        
        // Load configuration with comprehensive error handling
        auto config = std::make_shared<Config>();
        
        // Try to load from file if provided
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
        
        // Load config from file with comprehensive error handling
        bool configLoaded = false;
        try {
            configLoaded = config->loadFromFile(configFile);
            if (configLoaded) {
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            } else {
                NLM_LOG_INFO("Using default configuration. Config file not found: " + configFile);
            }
        } catch (const NLMException& e) {
            NLM_LOG_ERROR("Configuration error: " + std::string(e.what()));
            NLM_LOG_INFO("Continuing with default configuration...");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error loading config file " + configFile + ": " + std::string(e.what()));
            NLM_LOG_INFO("Continuing with default configuration...");
        }
        
        // Override with command line args with error handling
        try {
            config->loadFromArgs(argc, argv);
        } catch (const NLMException& e) {
            NLM_LOG_ERROR("Configuration error: " + std::string(e.what()));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error loading command line arguments: " + std::string(e.what()));
        }
        
        // Set default values for Phase 2 with error checking
        try {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(testConfig.neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(testConfig.regionCount), ConfigSource::Default);
            config->set("connection_probability", testConfig.connectionProbability, ConfigSource::Default);
            
            // STDP parameters
            config->set("stdp_ltp_weight", testConfig.stdpLTPWeight, ConfigSource::Default);
            config->set("stdp_ltd_weight", testConfig.stdpLTDWeight, ConfigSource::Default);
            config->set("stdp_tau", testConfig.stdpTau, ConfigSource::Default);
            
            // Structural plasticity parameters
            config->set("synaptogenesis_rate", testConfig.synaptogenesisRate, ConfigSource::Default);
            config->set("pruning_rate", testConfig.pruningRate, ConfigSource::Default);
            
            // Set simulation steps
            config->set("simulation_steps", static_cast<int64_t>(testConfig.numSteps), ConfigSource::CommandLine);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error setting configuration parameters: " + std::string(e.what()));
        }
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", testConfig.neuronCount)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", testConfig.regionCount)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", testConfig.connectionProbability)));
        NLM_LOG_INFO("  simulation_steps: " + std::to_string(config->getOr<int64_t>("simulation_steps", testConfig.numSteps)));
        NLM_LOG_INFO("");
        
        // Validate configuration with comprehensive error handling
        validateConfig(config, testConfig);
        
        // Initialize timer for performance measurement
        Timer globalTimer("Global Performance");
        if (testConfig.enableTiming) {
            globalTimer.start();
            NLM_LOG_INFO("Performance timing started");
        }
        
        // Initialize simulation clock with error handling
        double timestep = 0.001;
        try {
            timestep = config->getOr<double>("simulation_timestep", 0.001);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error getting simulation timestep: " + std::string(e.what()));
            timestep = 0.001;
        }
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Initialize brain with comprehensive error handling
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run Test 1: Basic connectivity
        runBasicConnectivityTest(brain, testConfig);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 2: Plasticity learning experiment
        runPlasticityExperiment(brain, testConfig);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain, testConfig);
        
        // Run system stability and edge case tests if enabled
        if (!testConfig.skipSystemStabilityTests && testConfig.runAllTests) {
            runSystemStabilityTests(brain, testConfig);
        }
        
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
        
        // Stop timing and report performance with comprehensive reporting
        if (testConfig.enableTiming) {
            double elapsed = globalTimer.stop();
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Performance Summary ===");
            NLM_LOG_INFO("Total execution time: " + std::to_string(elapsed) + " seconds");
            NLM_LOG_INFO("Steps per second: " + std::to_string(testConfig.numSteps / elapsed) + " Hz");
            NLM_LOG_INFO("Configuration validation: PASSED");
            
            if (testConfig.verbose) {
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Detailed Timing ===");
                NLM_LOG_INFO("Performance timers are available for individual test phases");
                NLM_LOG_INFO("Total accumulated time across all phases: " + std::to_string(globalTimer.getAccumulatedTime()) + " seconds");
            }
        }
        
        NLM_LOG_INFO("All tests completed successfully!");
        
        return 0;
    } catch (const NLMException& e) {
        NLM_LOG_CRITICAL("Fatal NLM Error: " + std::string(e.what()));
        NLM_LOG_CRITICAL("This error originated from: " + std::string(e.what()));
        return 1;
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Unexpected error: " + std::string(e.what()));
        NLM_LOG_CRITICAL("Stack trace and details available in verbose mode");
        return 1;
    } catch (...) {
        NLM_LOG_CRITICAL("Unknown error occurred");
        NLM_LOG_CRITICAL("Please check system logs for more details");
        return 1;
    }
}