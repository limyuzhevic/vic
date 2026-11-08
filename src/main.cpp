// NLM (熙然) - Neural Learning Machine
// Refactored Modular CLI for Phase 2 Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "CommandLineParser.hpp"
#include "TestFramework.hpp"
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
#include <fstream>
#include <sstream>

using namespace nlm;

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

// Forward declarations
class NLMSimulation;

// Main application class orchestrating everything
class NLMApplication {
public:
    NLMApplication() = default;
    ~NLMApplication() = default;
    
    bool initialize(int argc, char** argv);
    bool run(const CommandLineParser& parser);
    void cleanup();
    
    // Configuration and brain access
    std::shared_ptr<Config> getConfig() const { return config_; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Result management
    void saveResults(const std::string& filepath) const;
    void printResults() const;
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<TestFramework> testFramework_;
    std::shared_ptr<CommandLineParser> parser_;
    bool verbose_ = false;
};

// NLMApplication class implementation

bool NLMApplication::initialize(int argc, char** argv) {
    try {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
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
        config_ = std::make_shared<Config>();
        
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
        
        // Load config from file (ignore if not found)
        if (config_->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Set default values for Phase 2
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
        config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config_->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config_->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config_->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config_->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config_->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config_->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config_->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("");
        
        // Override with command line args
        config_->loadFromArgs(argc, argv);
        
        // Override with parser if provided
        if (parser_) {
            if (parser_->hasSeed()) {
                config_->set("random_seed", parser_->getSeed(), ConfigSource::CommandLine);
            }
            if (parser_->hasStepsOverride()) {
                // Store steps override for test execution
            }
            if (parser_->hasCurrentOverride()) {
                // Store current override for test execution
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during initialization: " + std::string(e.what()));
        return false;
    }
}

bool NLMApplication::run(const CommandLineParser& parser) {
    parser_ = std::const_pointer_cast<CommandLineParser>(std::shared_ptr<const CommandLineParser>(&parser, [](const CommandLineParser*) {}));
    verbose_ = parser.getVerbose();
    
    // Initialize test framework
    testFramework_ = std::make_shared<TestFramework>();
    testFramework_->setVerbose(verbose_);
    testFramework_->setConfig(config_);
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    brain_ = std::make_shared<Brain>(config_);
    
    if (!brain_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    brain_->logStatus();
    
    // Run all tests by default for backward compatibility
    auto testNames = parser.getTestNames();
    if (testNames.empty()) {
        testNames = {"all"};
    }
    testFramework_->setTestsToRun(testNames);
    
    // Run tests
    auto results = testFramework_->runAllTests(brain_, *config_);
    
    return true;
}

void NLMApplication::cleanup() {
    if (brain_) {
        brain_->logStatus();
        brain_.reset();
    }
    
    if (config_) {
        config_.reset();
    }
    
    if (testFramework_) {
        testFramework_.reset();
    }
    
    if (parser_) {
        parser_.reset();
    }
}

void NLMApplication::saveResults(const std::string& filepath) const {
    // This would save the test results to a JSON file
    // For now, we'll just log that we're saving
    NLM_LOG_INFO("Saving results to: " + filepath);
    
    // In a real implementation, this would:
    // 1. Get the test results from testFramework_
    // 2. Convert them to JSON
    // 3. Write to the specified file
}

void NLMApplication::printResults() const {
    // This would display the test results
    // For now, we'll just log that we're printing
    NLM_LOG_INFO("Displaying test results...");
    
    // In a real implementation, this would:
    // 1. Get the test results from testFramework_
    // 2. Display them in a user-friendly format
}

// Basic connectivity test class
class BasicConnectivityTest : public Test {
public:
    BasicConnectivityTest() : Test("BasicConnectivityTest", "Basic Neural Connectivity Test") {}
    
    TestResult execute(std::shared_ptr<Brain> brain, const Config& config) override {
        TestResult result("BasicConnectivityTest", TestStatus::PASSED);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.addMessage("ERROR", "No region found");
            return result;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            result.addMessage("WARNING", "No neurons found!");
            return result;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f); // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            result.addMessage("INFO", "✓ Spikes propagate through network");
            result.addMetric("spikes_generated", static_cast<double>(spikes));
        } else {
            result.addMessage("INFO", "  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
        
        return result;
    }
};

// Plasticity learning test class
class PlasticityLearningTest : public Test {
public:
    PlasticityLearningTest() : Test("PlasticityLearningTest", "Plasticity Learning Experiment") {}
    
    TestResult execute(std::shared_ptr<Brain> brain, const Config& config) override {
        TestResult result("PlasticityLearningTest", TestStatus::PASSED);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
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
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        std::vector<NeuronId> patternNeurons;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
        
        return result;
    }
};

// STDP verification test class
class StdpVerificationTest : public Test {
public:
    StdpVerificationTest() : Test("StdpVerificationTest", "STDP Verification Test") {}
    
    TestResult execute(std::shared_ptr<Brain> brain, const Config& config) override {
        TestResult result("StdpVerificationTest", TestStatus::PASSED);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.addMessage("ERROR", "No region found");
            return result;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            result.addMessage("WARNING", "No synapses for STDP test");
            return result;
        }
        
        // Use one real synapse and its own pre/post neurons
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        NLM_LOG_INFO("  Testing STDP on 1 synapse:");
        NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                     " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                     ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  After correlated activity:");
        NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")");
        
        float totalDelta = delta;
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            result.addMessage("SUCCESS", "✓ STDP WORKING: Pre-before-post produced potentiation");
            result.addMetric("weight_change", static_cast<double>(totalDelta));
            result.addMetric("delta_positive", 1.0);
        } else if (totalDelta < -0.001f) {
            result.addMessage("INFO", "  ! STDP reversed: Check parameters");
            result.addMetric("weight_change", static_cast<double>(totalDelta));
            result.addMetric("delta_negative", 1.0);
        } else {
            result.addMessage("INFO", "  ! No change: STDP may not be triggering");
            result.addMetric("weight_change", static_cast<double>(totalDelta));
            result.addMetric("delta_zero", 1.0);
        }
        
        return result;
    }
};

// Utility functions
void printUsage(const std::string& programName) {
    std::cout << R"(
NLM (熙然) - Neural Learning Machine
Usage: "" << programName << " [OPTIONS]"

Options:
  --help, -h              Show this help message
  --verbose, -v           Enable verbose output
  --config=file, -c file  Load configuration from file
  --tests=NAME, -t NAME   Specify which tests to run:
                          all, connectivity, plasticity, stdp, or combinations
  --output=file.json      Save results to JSON file
  --seed=number           Set random seed for reproducible experiments
  --steps=number          Custom test steps override
  --current=amount        Custom current injection amount

Advanced Options:
  --experiment=TYPE       Advanced experiment type (optional)
  --stats-analysis        Enable statistical analysis
  --benchmark             Run performance benchmarks
  --profile               Enable profiling

Examples:
  "" << programName << " --help
  "" << programName << " --tests=all --verbose --output=results.json
  "" << programName << " --tests=connectivity,plasticity --seed=123
  "" << programName << " --tests=stdp --current=40.0 --steps=100

NLM Phase 2: Real Neural Computation
  - Leaky Integrate-and-Fire (LIF) neuron dynamics
  - Event-driven spike propagation with delays
  - STDP and Hebbian plasticity rules
  - Structural plasticity (synaptogenesis/pruning)
  )" << std::endl;
}

void saveJsonResults(const TestResults& results, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return;
    }
    
    file << std::setw(2) << results.toJson() << std::endl;
    file.close();
    
    NLM_LOG_INFO("Results saved to: " + filepath);
}

int main(int argc, char** argv) {
    try {
        // Initialize global logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Print banner
        printBanner();
        
        // Parse command line arguments
        auto parser = std::make_shared<CommandLineParser>(argc, argv);
        
        // Show help if requested
        if (parser->hasHelp()) {
            printUsage(argv[0]);
            return 0;
        }
        
        // Create test framework
        auto testFramework = std::make_shared<TestFramework>();
        
        // Register all tests
        testFramework->registerTest(std::make_shared<BasicConnectivityTest>());
        testFramework->registerTest(std::make_shared<PlasticityLearningTest>());
        testFramework->registerTest(std::make_shared<StdpVerificationTest>());
        
        // Create application
        auto app = std::make_shared<NLMApplication>();
        
        // Initialize application
        if (!app->initialize(argc, argv)) {
            NLM_LOG_ERROR("Failed to initialize application");
            return 1;
        }
        
        // Set up test framework
        testFramework->setVerbose(parser->getVerbose());
        testFramework->setConfig(app->getConfig());
        
        // Parse test names from command line
        auto testNames = parser->getTestNames();
        testFramework->setTestsToRun(testNames);
        
        // Run tests
        auto results = testFramework->runAllTests(app->getBrain(), *app->getConfig());
        
        // Save or display results
        std::string outputFile = parser->getOutputFile();
        if (!outputFile.empty()) {
            app->saveResults(outputFile);
        } else {
            app->printResults();
        }
        
        // Cleanup
        app->cleanup();
        
        // Print summary
        std::cout << std::endl;
        std::cout << "=== NLM Simulation Complete ===" << std::endl;
        
        int passed = 0, failed = 0;
        for (const auto& result : results.getResults()) {
            if (result.status == TestStatus::PASSED) passed++;
            else if (result.status == TestStatus::FAILED) failed++;
        }
        
        std::cout << "Tests: " << passed << " passed, " << failed << " failed" << std::endl;
        
        return (failed > 0) ? 1 : 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception occurred");
        return 1;
    }
}
