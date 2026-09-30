// NLM (熙然) - Neural Learning Machine
// Main entry point with flexible test framework

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
#include <fstream>
#include <sstream>

using namespace nlm;

// Forward declarations for test classes
class TestRunner;
class TestSuite;
class TestCase;

// Test framework classes
class TestResult {
public:
    std::string name;
    bool passed;
    std::string message;
    double executionTime;
    
    TestResult(const std::string& testName) : name(testName), passed(false), executionTime(0.0) {}
};

class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual void setup() {}
    virtual void teardown() {}
    virtual void run() = 0;
    
    TestResult execute() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        setup();
        TestResult result(getName());
        try {
            run();
            result.passed = true;
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = "Exception: " + std::string(e.what());
        }
        
        teardown();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return result;
    }
};

class TestSuite {
public:
    virtual ~TestSuite() = default;
    virtual std::string getName() const = 0;
    virtual std::vector<std::shared_ptr<TestCase>> getTestCases() const = 0;
    
    void runAll(std::vector<TestResult>& results) {
        for (const auto& testCase : getTestCases()) {
            results.push_back(testCase->execute());
        }
    }
};

class ConnectivityTest : public TestCase {
public:
    std::shared_ptr<Brain> brain;
    
    std::string getName() const override { return "Basic Neural Connectivity Test"; }
    
    void setup() override {
        // Create and initialize brain
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    void run() override {
        runBasicConnectivityTest(brain);
    }
    
    void teardown() override {
        brain->reset();
    }
};

class PlasticityTest : public TestCase {
public:
    std::shared_ptr<Brain> brain;
    
    std::string getName() const override { return "Plasticity Learning Experiment"; }
    
    void setup() override {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    void run() override {
        runPlasticityExperiment(brain);
    }
    
    void teardown() override {
        brain->reset();
    }
};

class StdpTest : public TestCase {
public:
    std::shared_ptr<Brain> brain;
    
    std::string getName() const override { return "STDP Verification Test"; }
    
    void setup() override {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    void run() override {
        runStdpVerification(brain);
    }
    
    void teardown() override {
        brain->reset();
    }
};

class IntegrationTestSuite : public TestSuite {
public:
    std::string getName() const override { return "Phase 2 Integration Tests"; }
    
    std::vector<std::shared_ptr<TestCase>> getTestCases() const override {
        std::vector<std::shared_ptr<TestCase>> tests;
        tests.push_back(std::make_shared<ConnectivityTest>());
        tests.push_back(std::make_shared<PlasticityTest>());
        tests.push_back(std::make_shared<StdpTest>());
        return tests;
    }
};

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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        nlm::Logger::getGlobal().info("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    nlm::Logger::getGlobal().info("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    nlm::Logger::getGlobal().info("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        nlm::Logger::getGlobal().info("  ✓ Spikes propagate through network");
    } else {
        nlm::Logger::getGlobal().info("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            nlm::Logger::getGlobal().info("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<nlm::NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        if (auto* region = brain->getRegion(nlm::RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        nlm::Logger::getGlobal().info("Initial state recorded:");
        nlm::Logger::getGlobal().info("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            nlm::Logger::getGlobal().info("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        if (auto* region = brain->getRegion(nlm::RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        nlm::Logger::getGlobal().info("Final state recorded:");
        nlm::Logger::getGlobal().info("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            nlm::Logger::getGlobal().info("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("=== Learning Experiment Results ===");
        nlm::Logger::getGlobal().info("")
; 
        if (initialWeights.empty() || finalWeights.empty()) {
            nlm::Logger::getGlobal().info("ERROR: No weights recorded");
            return;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        nlm::Logger::getGlobal().info("Weight Statistics:");
        nlm::Logger::getGlobal().info("  Initial mean weight: " + std::to_string(initialMean));
        nlm::Logger::getGlobal().info("  Final mean weight: " + std::to_string(finalMean));
        nlm::Logger::getGlobal().info("  Change: " + std::to_string(finalMean - initialMean));
        
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
        
        nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("Synaptic Changes:");
        nlm::Logger::getGlobal().info("  Strengthened: " + std::to_string(strengthened));
        nlm::Logger::getGlobal().info("  Weakened: " + std::to_string(weakened));
        nlm::Logger::getGlobal().info("  Unchanged: " + std::to_string(unchanged));
        
        nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("Spike Activity:");
        nlm::Logger::getGlobal().info("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        nlm::Logger::getGlobal().info("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        nlm::Logger::getGlobal().info("")
; 
        if (learningOccurred) {
            nlm::Logger::getGlobal().info("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            nlm::Logger::getGlobal().info("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(nlm::RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("Applying repeated input patterns (1000 steps)...");
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            nlm::Logger::getGlobal().info("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        nlm::Logger::getGlobal().info("  Not enough synapses for STDP test");
        return;
    }
    
    nlm::Logger::getGlobal().info("  Testing STDP on 5 synapses:");
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        nlm::Logger::getGlobal().info("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
    }
    
    nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        nlm::Neuron* preNeuron = nullptr;
        nlm::Neuron* postNeuron = nullptr;
        
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
    
    nlm::Logger::getGlobal().info("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        nlm::Logger::getGlobal().info("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
    }
    
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    nlm::Logger::getGlobal().info("")
; 
    if (totalDelta > 0.001f) {
        nlm::Logger::getGlobal().info("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        nlm::Logger::getGlobal().info("  ! STDP reversed: Check parameters");
    } else {
        nlm::Logger::getGlobal().info("  ! No change: STDP may not be triggering");
    }
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h               Show this help message" << std::endl;
    std::cout << "  --config=file.cfg        Load configuration from file" << std::endl;
    std::cout << "  --run-all                Run all Phase 2 tests" << std::endl;
    std::cout << "  --run-test=name          Run specific test (connectivity, plasticity, stdp)" << std::endl;
    std::cout << "  --help-tests             List available tests" << std::endl;
    std::cout << "  --version, -v            Show version information" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --run-all" << std::endl;
    std::cout << "  " << programName << " --run-test=plasticity" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Build with modern C++20 with LIF neurons, STDP, and plasticity" << std::endl;
}

void printTestHelp() {
    std::cout << "Available Tests:" << std::endl;
    std::cout << "  connectivity - Basic neural connectivity test" << std::endl;
    std::cout << "  plasticity  - Plasticity learning experiment" << std::endl;
    std::cout << "  stdp        - STDP verification test" << std::endl;
    std::cout << "  all         - Run all tests" << std::endl;
}

bool parseCommandLineArgs(int argc, char** argv, nlm::Config& config, std::string& testFilter) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return false;
        } 
        else if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;
        }
        
        else if (arg == "--help-tests") {
            printTestHelp();
            return false;
        }
        
        else if (arg == "--run-all") {
            testFilter = "all";
        }
        
        else if (arg.substr(0, 13) == "--run-test=") {
            testFilter = arg.substr(13);
        }
        
        else if (arg.substr(0, 11) == "--config=") {
            if (config.loadFromFile(arg.substr(11))) {
                nlm::Logger::getGlobal().info("Loaded configuration from: " + arg.substr(11));
            } else {
                nlm::Logger::getGlobal().info("Using default configuration.");
            }
        }
        
        else if (arg == "--config" && i + 1 < argc) {
            std::string configFile = argv[++i];
            if (config.loadFromFile(configFile)) {
                nlm::Logger::getGlobal().info("Loaded configuration from: " + configFile);
            } else {
                nlm::Logger::getGlobal().info("Using default configuration.");
            }
        }
        
        else if (config.loadFromArgs(argc, argv)) {
            continue;
        }
    }
    
    return true;
}

void printTestResults(const std::vector<TestResult>& results) {
    std::cout << "" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "           TEST RESULTS SUMMARY" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
    
    size_t passed = 0;
    size_t failed = 0;
    double totalTime = 0.0;
    
    for (const auto& result : results) {
        std::cout << std::left << std::setw(40) << result.name;
        if (result.passed) {
            std::cout << "✓ PASSED";
            passed++;
        } else {
            std::cout << "✗ FAILED";
            failed++;
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
        }
        std::cout << " (" << std::fixed << std::setprecision(3) << result.executionTime << "s)" << std::endl;
        
        totalTime += result.executionTime;
    }
    
    std::cout << "" << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "Total time: " << std::fixed << std::setprecision(3) << totalTime << " seconds" << std::endl;
    
    if (failed > 0) {
        std::cout << "" << std::endl;
        std::cout << "WARNING: Some tests failed!" << std::endl;
    } else {
        std::cout << "" << std::endl;
        std::cout << "SUCCESS: All tests passed!" << std::endl;
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    nlm::Logger::getGlobal().info("=== NLM Phase 2: Real Neural Computation ===");
    nlm::Logger::getGlobal().info("Implementing:");
    nlm::Logger::getGlobal().info("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    nlm::Logger::getGlobal().info("  - Event-driven spike propagation with delays");
    nlm::Logger::getGlobal().info("  - STDP and Hebbian plasticity rules");
    nlm::Logger::getGlobal().info("  - Structural plasticity (synaptogenesis/pruning)");
    nlm::Logger::getGlobal().info("")
; 
    nlm::Config config;
    std::string testFilter = "all";
    
    if (!parseCommandLineArgs(argc, argv, config, testFilter)) {
        return 0;
    }
    
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    nlm::SimulationClock clock(timestep);
    nlm::Logger::getGlobal().info("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    nlm::Logger::getGlobal().info("")
; nlm::Logger::getGlobal().info("Running tests with filter: '" + testFilter + "'");
    
    IntegrationTestSuite testSuite;
    std::vector<TestResult> results;
    
    auto tests = testSuite.getTestCases();
    
    if (testFilter == "all") {
        testSuite.runAll(results);
    } else {
        bool testFound = false;
        for (const auto& test : tests) {
            if (test->getName() == testFilter) {
                results.push_back(test->execute());
                testFound = true;
                break;
            }
        }
        if (!testFound) {
            nlm::Logger::getGlobal().error("Test not found: '" + testFilter + "'");
            printTestHelp();
            return 1;
        }
    }
    
    printTestResults(results);
    
    return 0;
}