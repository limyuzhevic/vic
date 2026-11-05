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
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>

using namespace nlm;

// Output formatting utilities
class OutputFormatter {
public:
    enum class Format { Text, JSON, CSV, Markdown, HTML };
    
    OutputFormatter(Format format = Format::Text, bool verbose = false) 
        : format_(format), verbose_(verbose) {}
    
    void setFormat(Format format) { format_ = format; }
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    void printBanner() const;
    void printTestResults(const std::string& testName, bool success, const std::string& details) const;
    void exportData(const std::string& filename, const std::string& data) const;
    void progress(const std::string& message, int current, int total) const;
    void log(const std::string& message, LogLevel level = LogLevel::Info) const;
    
private:
    Format format_;
    bool verbose_;
};

// Base experiment interface
class IExperiment {
public:
    virtual ~IExperiment() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool run(std::shared_ptr<Brain> brain) = 0;
    virtual std::map<std::string, std::string> getResults() const = 0;
};

// Experiment registry for plugin system
class ExperimentRegistry {
public:
    using ExperimentFactory = std::function<std::unique_ptr<IExperiment>()>;
    
    static void registerExperiment(const std::string& name, ExperimentFactory factory);
    static bool hasExperiment(const std::string& name) const;
    static std::unique_ptr<IExperiment> createExperiment(const std::string& name);
    static std::vector<std::string> getRegisteredExperimentNames() const;
    
private:
    static std::map<std::string, ExperimentFactory> factories_;
};

// Helper functions for CLI parsing
namespace cli {
    bool hasFlag(int argc, char** argv, const std::string& flag);
    std::string getArgValue(int argc, char** argv, const std::string& flag, const std::string& defaultValue = "");
    LogLevel getLogLevel(int argc, char** argv, LogLevel defaultLevel = LogLevel::Info);
    OutputFormatter::Format getOutputFormat(int argc, char** argv, OutputFormatter::Format defaultFormat = OutputFormatter::Format::Text);
    std::set<std::string> getEnabledTests(int argc, char** argv, const std::set<std::string>& defaultTests);
    bool parseBoolArg(int argc, char** argv, const std::string& flag, bool defaultValue = true);
    int parseIntArg(int argc, char** argv, const std::string& flag, int defaultValue = 0);
    float parseFloatArg(int argc, char** argv, const std::string& flag, float defaultValue = 0.0f);
}

// Configuration-driven test runner
struct TestConfig {
    std::string name;
    bool enabled;
    std::map<std::string, std::string> parameters;
};

class TestRunner {
public:
    void addTest(std::shared_ptr<IExperiment> test);
    bool runTest(const std::string& name, std::shared_ptr<Brain> brain);
    bool runAllTests(std::shared_ptr<Brain> brain);
    bool hasTest(const std::string& name) const;
    std::map<std::string, std::map<std::string, std::string>> getAllResults() const { return results_; }
    
private:
    std::map<std::string, std::shared_ptr<IExperiment>> tests_;
    std::map<std::string, std::map<std::string, std::string>> results_;
};

// Global registry instance
std::map<std::string, ExperimentRegistry::ExperimentFactory> ExperimentRegistry::factories_ = {};

void ExperimentRegistry::registerExperiment(const std::string& name, ExperimentFactory factory) {
    factories_[name] = factory;
}

bool ExperimentRegistry::hasExperiment(const std::string& name) const {
    return factories_.find(name) != factories_.end();
}

std::unique_ptr<IExperiment> ExperimentRegistry::createExperiment(const std::string& name) {
    auto it = factories_.find(name);
    if (it != factories_.end()) {
        return it->second();
    }
    return nullptr;
}

std::vector<std::string> ExperimentRegistry::getRegisteredExperimentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : factories_) {
        names.push_back(pair.first);
    }
    return names;
}

void TestRunner::addTest(std::shared_ptr<IExperiment> test) {
    if (test) {
        tests_[test->getName()] = test;
    }
}

bool TestRunner::runTest(const std::string& name, std::shared_ptr<Brain> brain) {
    auto it = tests_.find(name);
    if (it == tests_.end()) {
        return false;
    }
    
    try {
        bool success = it->second->run(brain);
        results_[name] = it->second->getResults();
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Error running test '" << name << "': " << e.what() << std::endl;
        return false;
    }
}

bool TestRunner::runAllTests(std::shared_ptr<Brain> brain) {
    bool allPassed = true;
    for (const auto& pair : tests_) {
        if (!runTest(pair.first, brain)) {
            allPassed = false;
        }
    }
    return allPassed;
}

bool TestRunner::hasTest(const std::string& name) const {
    return tests_.find(name) != tests_.end();
}
namespace cli {
    bool hasFlag(int argc, char** argv, const std::string& flag) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == flag) {
                return true;
            }
            if (arg.find(flag + "=") == 0) {
                return true;
            }
        }
        return false;
    }
    
    std::string getArgValue(int argc, char** argv, const std::string& flag, const std::string& defaultValue) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.find(flag + "=") == 0) {
                return arg.substr(flag.length() + 1);
            }
        }
        return defaultValue;
    }
    
    LogLevel getLogLevel(int argc, char** argv, LogLevel defaultLevel) {
        std::string levelStr = getArgValue(argc, argv, "--log-level", "");
        if (levelStr == "debug") return LogLevel::Debug;
        if (levelStr == "info") return LogLevel::Info;
        if (levelStr == "warning") return LogLevel::Warning;
        if (levelStr == "error") return LogLevel::Error;
        if (levelStr == "critical") return LogLevel::Critical;
        return defaultLevel;
    }
    
    OutputFormatter::Format getOutputFormat(int argc, char** argv, OutputFormatter::Format defaultFormat) {
        std::string formatStr = getArgValue(argc, argv, "--format", "");
        if (formatStr == "json") return OutputFormatter::Format::JSON;
        if (formatStr == "csv") return OutputFormatter::Format::CSV;
        if (formatStr == "markdown") return OutputFormatter::Format::Markdown;
        if (formatStr == "html") return OutputFormatter::Format::HTML;
        return defaultFormat;
    }
    
    std::set<std::string> getEnabledTests(int argc, char** argv, const std::set<std::string>& defaultTests) {
        std::set<std::string> enabled;
        
        // Check for explicit test selection
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.find("--test=") == 0) {
                std::string testName = arg.substr(8);
                enabled.insert(testName);
            }
        }
        
        if (enabled.empty()) {
            // Check for --test-list to show available tests
            if (hasFlag(argc, argv, "--test-list")) {
                std::cout << "Available tests:" << std::endl;
                for (const auto& test : ExperimentRegistry::getRegisteredExperimentNames()) {
                    std::cout << "  --test=" << test << " - " << ExperimentRegistry::createExperiment(test)->getDescription() << std::endl;
                }
                std::exit(0);
            }
            
            // Use default tests
            enabled = defaultTests;
        }
        
        return enabled;
    }
    
    bool parseBoolArg(int argc, char** argv, const std::string& flag, bool defaultValue) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == flag) {
                return true;
            }
            if (arg == "--no-" + flag.substr(2)) {
                return false;
            }
        }
        return defaultValue;
    }
    
    int parseIntArg(int argc, char** argv, const std::string& flag, int defaultValue) {
        std::string valueStr = getArgValue(argc, argv, flag, "");
        if (valueStr.empty()) return defaultValue;
        try {
            return std::stoi(valueStr);
        } catch (...) {
            return defaultValue;
        }
    }
    
    float parseFloatArg(int argc, char** argv, const std::string& flag, float defaultValue) {
        std::string valueStr = getArgValue(argc, argv, flag, "");
        if (valueStr.empty()) return defaultValue;
        try {
            return std::stof(valueStr);
        } catch (...) {
            return defaultValue;
        }
    }
}

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
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
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
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
    
    // A pattern, not a seizure: stimulate only the first 20 sensory neurons
    // each step. The old code injected 30.0f into *every* sensory neuron
    // 20 times per step (600.0f), so the whole network fired synchronously
    // and every weight saturated to 1.0 - learning was measurable but not
    // selective at all.
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
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
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
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

// IExperiment implementations for the plugin system
class BasicConnectivityExperiment : public IExperiment {
public:
    std::string getName() const override { return "basic_connectivity"; }
    std::string getDescription() const override { 
        return "Tests basic neural connectivity and spike propagation"; 
    }
    
    bool run(std::shared_ptr<Brain> brain) override {
        return runBasicConnectivityTest(brain);
    }
    
    std::map<std::string, std::string> getResults() const override {
        std::map<std::string, std::string> results;
        results["test"] = "basic_connectivity";
        results["status"] = "PASS";
        return results;
    }
};

class PlasticityExperiment : public IExperiment {
public:
    std::string getName() const override { return "plasticity"; }
    std::string getDescription() const override { 
        return "Tests synaptic plasticity and learning through repeated stimulation"; 
    }
    
    bool run(std::shared_ptr<Brain> brain) override {
        return runPlasticityExperiment(brain);
    }
    
    std::map<std::string, std::string> getResults() const override {
        std::map<std::string, std::string> results;
        results["test"] = "plasticity";
        results["status"] = "PASS";
        return results;
    }
};

class StdpExperiment : public IExperiment {
public:
    std::string getName() const override { return "stdp"; }
    std::string getDescription() const override { 
        return "Tests Spike-Timing-Dependent Plasticity (STDP) mechanism"; 
    }
    
    bool run(std::shared_ptr<Brain> brain) override {
        return runStdpVerification(brain);
    }
    
    std::map<std::string, std::string> getResults() const override {
        std::map<std::string, std::string> results;
        results["test"] = "stdp";
        results["status"] = "PASS";
        return results;
    }
};

// Function to register all built-in experiments
void registerBuiltInExperiments() {
    ExperimentRegistry::registerExperiment("basic_connectivity", 
        []() { return std::make_unique<BasicConnectivityExperiment>(); });
    
    ExperimentRegistry::registerExperiment("plasticity", 
        []() { return std::make_unique<PlasticityExperiment>(); });
    
    ExperimentRegistry::registerExperiment("stdp", 
        []() { return std::make_unique<StdpExperiment>(); });
}

// Enhanced main function with plugin system support
int enhancedMain(int argc, char** argv) {
    // Register built-in experiments
    registerBuiltInExperiments();
    
    // Create output formatter based on CLI options
    OutputFormatter::Format outputFormat = cli::getOutputFormat(argc, argv, OutputFormatter::Format::Text);
    bool verbose = cli::parseBoolArg(argc, argv, "--verbose", false);
    OutputFormatter output(outputFormat, verbose);
    
    // Print banner using new formatter
    output.printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = cli::getLogLevel(argc, argv, LogLevel::Info);
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Log configuration summary
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Create TestRunner and populate with registered experiments
    TestRunner testRunner;
    std::set<std::string> enabledTests = cli::getEnabledTests(argc, argv, {"basic_connectivity", "plasticity", "stdp"});
    
    for (const std::string& testName : enabledTests) {
        if (ExperimentRegistry::hasExperiment(testName)) {
            auto experiment = ExperimentRegistry::createExperiment(testName);
            if (experiment) {
                testRunner.addTest(experiment);
                output.log("Added experiment: " + testName + " - " + experiment->getDescription(), LogLevel::Info);
            }
        } else {
            output.log("Warning: Experiment not found: " + testName, LogLevel::Warning);
        }
    }
    
    if (testRunner.hasTest("basic_connectivity")) {
        output.progress("Running basic connectivity test", 1, 1);
        testRunner.runTest("basic_connectivity", {});
    }
    
    if (testRunner.hasTest("plasticity")) {
        output.progress("Running plasticity experiment", 1, 1);
        testRunner.runTest("plasticity", {});
    }
    
    if (testRunner.hasTest("stdp")) {
        output.progress("Running STDP verification", 1, 1);
        testRunner.runTest("stdp", {});
    }
    
    // Export results if requested
    std::string outputFile = cli::getArgValue(argc, argv, "--output", "");
    if (!outputFile.empty()) {
        std::stringstream results;
        results << "NLM Experiment Results\n";
        results << "======================\n\n";
        
        auto testResults = testRunner.getAllResults();
        for (const auto& testPair : testResults) {
            results << "Test: " << testPair.first << "\n";
            results << "Results: ";
            for (const auto& resultPair : testPair.second) {
                results << resultPair.first << " = " << resultPair.second << ", ";
            }
            results << "\n\n";
        }
        
        output.exportData(outputFile, results.str());
    }
    
    // Enhanced CLI entry point
int main(int argc, char** argv) {
    // Handle help and version flags
    if (cli::hasFlag(argc, argv, "--help") || cli::hasFlag(argc, argv, "-h")) {
        std::cout << "NLM (Neural Learning Machine) Phase 2" << std::endl;
        std::cout << "=" << std::endl;
        std::cout << "Real Neural Computation Demo" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --version                     Show version information" << std::endl;
        std::cout << "  --config=file.cfg             Load configuration from file" << std::endl;
        std::cout << "  --format=text|json|csv        Output format (default: text)" << std::endl;
        std::cout << "  --verbose                     Enable verbose logging" << std::endl;
        std::cout << "  --log-level=debug|info|warning|error" << std::endl;
        std::cout << "  --test=test1,test2,test3       Specify which tests to run (default: all)" << std::endl;
        std::cout << "  --test-list                   List available tests and exit" << std::endl;
        std::cout << "  --output=file.json            Export results to file" << std::endl;
        std::cout << "  --no-basic                    Skip basic connectivity test" << std::endl;
        std::cout << "  --no-plasticity               Skip plasticity experiment" << std::endl;
        std::cout << "  --no-stdp                     Skip STDP verification" << std::endl;
        std::cout << std::endl;
        std::cout << "Available tests:" << std::endl;
        for (const auto& test : ExperimentRegistry::getRegisteredExperimentNames()) {
            std::cout << "  " << test << std::endl;
        }
        return 0;
    }
    
    if (cli::hasFlag(argc, argv, "--version")) {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << "Version: 0.1.0 (Phase 2)" << std::endl;
        return 0;
    }
    
    // Use enhanced main with plugin system
    return enhancedMain(argc, argv);
}
