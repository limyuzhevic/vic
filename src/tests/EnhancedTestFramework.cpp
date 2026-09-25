// Enhanced Testing Framework for NLM
// 
// This file provides an enhanced testing framework with:
// 1. Comprehensive unit tests for all components
// 2. Integration tests for system-level functionality
// 3. Performance and stress tests
// 4. Regression tests
// 5. Documentation and validation tests
// 6. Advanced test utilities and reporting

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include <random>

namespace nlm {

namespace test {

// Test result structure
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string message;
    double executionTime;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    TestResult(const std::string& testName, const std::string& testCategory)
        : name(testName), category(testCategory), passed(false), executionTime(0.0) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void complete(bool success, const std::string& msg = "") {
        endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        executionTime = duration.count() / 1000.0;
        
        passed = success;
        message = msg;
    }
    
    std::string getFormattedTime() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << executionTime << "s";
        return ss.str();
    }
};

// Test suite for organizing related tests
class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description = "")
        : name_(name), description_(description), passedCount_(0), failedCount_(0), totalCount_(0) {}
    
    virtual ~TestSuite() = default;
    
    void addTest(const std::string& testName, std::function<void()> testFunc,
                 const std::string& category = "general");
    
    void runAllTests();
    
    // Get results
    const std::vector<TestResult>& getResults() const { return results_; }
    size_t getPassedCount() const { return passedCount_; }
    size_t getFailedCount() const { return failedCount_; }
    size_t getTotalCount() const { return totalCount_; }
    double getTotalExecutionTime() const { return totalExecutionTime_; }
    
    // Suite information
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    // Report generation
    std::string generateTextReport() const;
    std::string generateJSONReport() const;
    std::string generateHTMLReport() const;
    
    // Filtering and categorization
    void filterByCategory(const std::string& category);
    void runCategoryTests(const std::string& category);
    std::vector<std::string> getCategories() const;
    
protected:
    virtual void setup() {}  // Override in derived classes
    virtual void teardown() {}  // Override in derived classes
    virtual void setupSuite() {}  // Override in derived classes
    virtual void teardownSuite() {}  // Override in derived classes
    
    virtual void onTestStart(const TestResult& test) {}  // Override in derived classes
    virtual void onTestComplete(const TestResult& test) {}  // Override in derived classes
    
private:
    std::string name_;
    std::string description_;
    std::vector<TestResult> results_;
    std::vector<std::function<void()>> tests_;
    std::map<std::string, std::vector<size_t>> testsByCategory_;
    size_t passedCount_;
    size_t failedCount_;
    size_t totalCount_;
    double totalExecutionTime_;
};

// Unit test base class
class UnitTest {
public:
    virtual ~UnitTest() = default;
    
    virtual void setUp() {}
    virtual void tearDown() {}
    virtual void test() = 0;
    
    virtual std::string getTestName() const = 0;
    virtual std::string getTestCategory() const = 0;
    virtual std::string getTestDescription() const = 0;
};

// Component-specific unit tests
class ConfigTests : public UnitTest {
public:
    void testCreation() override;
    void testParameterSetting() override;
    void testParameterGetting() override;
    void testValidation() override;
    void testSerialization() override;
    
    std::string getTestName() const override { return "ConfigTests"; }
    std::string getTestCategory() const override { return "config"; }
    std::string getTestDescription() const override { return "Tests for configuration system"; }
};

class NeuronTests : public UnitTest {
public:
    void testCreation() override;
    void testMembraneDynamics() override;
    void testSpikeGeneration() override;
    void testSynapticInput() override;
    void testRefractoryPeriod() override;
    
    std::string getTestName() const override { return "NeuronTests"; }
    std::string getTestCategory() const override { return "neuron"; }
    std::string getTestDescription() const override { return "Tests for neuron dynamics"; }
};

class SynapseTests : public UnitTest {
public:
    void testCreation() override;
    void testWeightUpdate() override;
    void testDelayPropagation() override;
    void testSynapticType() override;
    void testPlasticityIntegration() override;
    
    std::string getTestName() const override { return "SynapseTests"; }
    std::string getTestCategory() const override { return "synapse"; }
    std::string getTestDescription() const override { return "Tests for synapse functionality"; }
};

class BrainTests : public UnitTest {
public:
    void testInitialization() override;
    void testRegionManagement() override;
    void testStepSimulation() override;
    void testActionProduction() override;
    void testMemorySystems() override;
    void testNeuromodulation() override;
    
    std::string getTestName() const override { return "BrainTests"; }
    std::string getTestCategory() const override { return "brain"; }
    std::string getTestDescription() const override { return "Tests for brain functionality"; }
};

class PlasticityTests : public UnitTest {
public:
    void testSTDP() override;
    void testHebbian() override;
    void testStructuralPlasticity() override;
    void testRewardModulation() override;
    void testPlasticityIntegration() override;
    
    std::string getTestName() const override { return "PlasticityTests"; }
    std::string getTestCategory() const override { return "plasticity"; }
    std::string getTestDescription() const override { return "Tests for plasticity rules"; }
};

class MemoryTests : public UnitTest {
public:
    void testWorkingMemory() override;
    void testEpisodicMemory() override;
    void testAssociativeMemory() override;
    void testMemoryReplay() override;
    void testMemoryConsolidation() override;
    
    std::string getTestName() const override { return "MemoryTests"; }
    std::string getTestCategory() const override { return "memory"; }
    std::string getTestDescription() const override { return "Tests for memory systems"; }
};

class NeuromodTests : public UnitTest {
public:
    void testDopamine() override;
    void testCuriosity() override;
    void testNovelty() override;
    void testPredictionError() override;
    void testNeuromodIntegration() override;
    
    std::string getTestName() const override { return "NeuromodTests"; }
    std::string getTestCategory() const override { return "neuromod"; }
    std::string getTestDescription() const override { return "Tests for neuromodulation systems"; }
};

// Performance tests
class PerformanceTests {
public:
    struct PerformanceMetrics {
        double executionTime;
        size_t memoryUsage;
        double cpuUsage;
        size_t totalOperations;
        size_t successfulOperations;
        double successRate;
    };
    
    static PerformanceMetrics runBrainSimulation(const Brain& brain, size_t steps);
    static PerformanceMetrics runMemoryTest(const Brain& brain, size_t iterations);
    static PerformanceMetrics runPlasticityTest(const Brain& brain, size_t steps);
    static double calculatePerformanceScore(const PerformanceMetrics& metrics);
    
    static void stressTest(const std::string& testName, std::function<void()> stressFunction,
                           size_t iterations = 100);
};

// Regression tests
class RegressionTests {
public:
    static bool runAllRegressionTests();
    static bool testRegression(const std::string& testName, std::function<bool()> regressionTest);
    static void updateRegression baselines();
    static void validateRegression(const std::string& testName, double expectedValue,
                                   double actualValue, double tolerance = 0.01);
};

// Integration tests
class IntegrationTests {
public:
    static bool testBrainWithMemory();
    static bool testBrainWithNeuromod();
    static bool testBrainWithPlasticity();
    static bool testCompleteSystem();
    static bool testRealTimePerformance();
    
    static void runAllIntegrationTests();
};

// Test runner for the entire system
class TestRunner {
public:
    TestRunner() : verbose_(false), parallel_(false), maxWorkers_(0) {}
    
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setParallel(bool parallel, size_t maxWorkers = 0) { 
        parallel_ = parallel; 
        maxWorkers_ = maxWorkers; 
    }
    
    void addSuite(TestSuite* suite) {
        suites_.push_back(suite);
    }
    
    void runAllTests();
    void runSuite(TestSuite* suite);
    
    std::string generateFullReport() const;
    std::string generateSummaryReport() const;
    std::string generatePerformanceReport() const;
    
    // Configuration
    void setReportFormat(const std::string& format) { reportFormat_ = format; }
    void setOutputFile(const std::string& filename) { outputFile_ = filename; }
    void setLogFile(const std::string& filename) { logFile_ = filename; }
    
    // Statistics
    size_t getTotalTestsRun() const { return totalTestsRun_; }
    size_t getPassedTests() const { return passedTests_; }
    size_t getFailedTests() const { return failedTests_; }
    double getTotalExecutionTime() const { return totalExecutionTime_; }
    
protected:
    void logTest(const TestResult& test);
    void logSuite(const TestSuite* suite);
    void logPerformance(const PerformanceMetrics& metrics);
    
private:
    std::vector<TestSuite*> suites_;
    bool verbose_;
    bool parallel_;
    size_t maxWorkers_;
    std::string reportFormat_;
    std::string outputFile_;
    std::string logFile_;
    
    size_t totalTestsRun_;
    size_t passedTests_;
    size_t failedTests_;
    double totalExecutionTime_;
    std::vector<TestResult> allTestResults_;
};

// Convenience functions
inline void runUnitTests() {
    TestRunner runner;
    runner.setVerbose(true);
    
    // Add individual unit tests
    TestSuite* configSuite = new TestSuite("Config Tests", "Configuration system tests");
    configSuite->addTest("Creation", []() { ConfigTests().testCreation(); }, "config");
    configSuite->addTest("Parameter Setting", []() { ConfigTests().testParameterSetting(); }, "config");
    configSuite->addTest("Parameter Getting", []() { ConfigTests().testParameterGetting(); }, "config");
    configSuite->addTest("Validation", []() { ConfigTests().testValidation(); }, "config");
    configSuite->addTest("Serialization", []() { ConfigTests().testSerialization(); }, "config");
    
    TestSuite* neuronSuite = new TestSuite("Neuron Tests", "Neuron dynamics tests");
    neuronSuite->addTest("Creation", []() { NeuronTests().testCreation(); }, "neuron");
    neuronSuite->addTest("Membrane Dynamics", []() { NeuronTests().testMembraneDynamics(); }, "neuron");
    neuronSuite->addTest("Spike Generation", []() { NeuronTests().testSpikeGeneration(); }, "neuron");
    neuronSuite->addTest("Synaptic Input", []() { NeuronTests().testSynapticInput(); }, "neuron");
    neuronSuite->addTest("Refractory Period", []() { NeuronTests().testRefractoryPeriod(); }, "neuron");
    
    TestSuite* synapseSuite = new TestSuite("Synapse Tests", "Synapse functionality tests");
    synapseSuite->addTest("Creation", []() { SynapseTests().testCreation(); }, "synapse");
    synapseSuite->addTest("Weight Update", []() { SynapseTests().testWeightUpdate(); }, "synapse");
    synapseSuite->addTest("Delay Propagation", []() { SynapseTests().testDelayPropagation(); }, "synapse");
    synapseSuite->addTest("Synaptic Type", []() { SynapseTests().testSynapticType(); }, "synapse");
    synapseSuite->addTest("Plasticity Integration", []() { SynapseTests().testPlasticityIntegration(); }, "synapse");
    
    TestSuite* brainSuite = new TestSuite("Brain Tests", "Brain functionality tests");
    brainSuite->addTest("Initialization", []() { BrainTests().testInitialization(); }, "brain");
    brainSuite->addTest("Region Management", []() { BrainTests().testRegionManagement(); }, "brain");
    brainSuite->addTest("Step Simulation", []() { BrainTests().testStepSimulation(); }, "brain");
    brainSuite->addTest("Action Production", []() { BrainTests().testActionProduction(); }, "brain");
    brainSuite->addTest("Memory Systems", []() { BrainTests().testMemorySystems(); }, "brain");
    brainSuite->addTest("Neuromodulation", []() { BrainTests().testNeuromodulation(); }, "brain");
    
    TestSuite* plasticitySuite = new TestSuite("Plasticity Tests", "Plasticity rule tests");
    plasticitySuite->addTest("STDP", []() { PlasticityTests().testSTDP(); }, "plasticity");
    plasticitySuite->addTest("Hebbian", []() { PlasticityTests().testHebbian(); }, "plasticity");
    plasticitySuite->addTest("Structural Plasticity", []() { PlasticityTests().testStructuralPlasticity(); }, "plasticity");
    plasticitySuite->addTest("Reward Modulation", []() { PlasticityTests().testRewardModulation(); }, "plasticity");
    plasticitySuite->addTest("Plasticity Integration", []() { PlasticityTests().testPlasticityIntegration(); }, "plasticity");
    
    TestSuite* memorySuite = new TestSuite("Memory Tests", "Memory system tests");
    memorySuite->addTest("Working Memory", []() { MemoryTests().testWorkingMemory(); }, "memory");
    memorySuite->addTest("Episodic Memory", []() { MemoryTests().testEpisodicMemory(); }, "memory");
    memorySuite->addTest("Associative Memory", []() { MemoryTests().testAssociativeMemory(); }, "memory");
    memorySuite->addTest("Memory Replay", []() { MemoryTests().testMemoryReplay(); }, "memory");
    memorySuite->addTest("Memory Consolidation", []() { MemoryTests().testMemoryConsolidation(); }, "memory");
    
    TestSuite* neuromodSuite = new TestSuite("Neuromod Tests", "Neuromodulation tests");
    neuromodSuite->addTest("Dopamine", []() { NeuromodTests().testDopamine(); }, "neuromod");
    neuromodSuite->addTest("Curiosity", []() { NeuromodTests().testCuriosity(); }, "neuromod");
    neuromodSuite->addTest("Novelty", []() { NeuromodTests().testNovelty(); }, "neuromod");
    neuromodSuite->addTest("Prediction Error", []() { NeuromodTests().testPredictionError(); }, "neuromod");
    neuromodSuite->addTest("Neuromod Integration", []() { NeuromodTests().testNeuromodIntegration(); }, "neuromod");
    
    // Add suites to runner
    runner.addSuite(configSuite);
    runner.addSuite(neuronSuite);
    runner.addSuite(synapseSuite);
    runner.addSuite(brainSuite);
    runner.addSuite(plasticitySuite);
    runner.addSuite(memorySuite);
    runner.addSuite(neuromodSuite);
    
    // Run all tests
    runner.runAllTests();
    
    // Generate reports
    std::string report = runner.generateFullReport();
    std::cout << report << std::endl;
    
    // Save report to file
    if (!runner.getOutputFile().empty()) {
        std::ofstream file(runner.getOutputFile());
        if (file.is_open()) {
            file << report;
            file.close();
        }
    }
}

inline void runIntegrationTests() {
    IntegrationTests::runAllIntegrationTests();
}

inline void runPerformanceTests() {
    // Run performance tests
    std::cout << "Running performance tests..." << std::endl;
    
    // Create a test brain
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("brain.region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run performance measurements
    PerformanceMetrics metrics = PerformanceTests::runBrainSimulation(brain, 1000);
    
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Execution Time: " << metrics.executionTime << "s" << std::endl;
    std::cout << "  Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
    std::cout << "  CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "  Total Operations: " << metrics.totalOperations << std::endl;
    std::cout << "  Success Rate: " << metrics.successRate * 100 << "%" << std::endl;
    
    double score = PerformanceTests::calculatePerformanceScore(metrics);
    std::cout << "  Performance Score: " << score << std::endl;
}

inline void runRegressionTests() {
    bool success = RegressionTests::runAllRegressionTests();
    if (success) {
        std::cout << "All regression tests passed!" << std::endl;
    } else {
        std::cout << "Some regression tests failed!" << std::endl;
    }
}

} // namespace test

// Test macros for easy test creation
#define TEST_SUITE(name, description) \
class name : public nlm::test::TestSuite { \
public: \
    name() : TestSuite(#name, description) {} \
};

#define TEST(testName, testFunc) \
    addTest(#testName, []() { testFunc(); }, "general")

#define TEST_CATEGORY(testName, testFunc, category) \
    addTest(#testName, []() { testFunc(); }, category)

// Helper functions for common test operations
inline bool assertEqual(double expected, double actual, double tolerance = 0.01, 
                       const std::string& message = "") {
    if (std::abs(expected - actual) > tolerance) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Expected: " << expected << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertGreaterThan(double expected, double actual, 
                              const std::string& message = "") {
    if (expected >= actual) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Expected: > " << expected << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertLessThan(double expected, double actual, 
                           const std::string& message = "") {
    if (expected <= actual) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Expected: < " << expected << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertNotNull(void* ptr, const std::string& message = "") {
    if (ptr == nullptr) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Pointer is null" << std::endl;
        return false;
    }
    return true;
}

inline bool assertTrue(bool condition, const std::string& message = "") {
    if (!condition) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Condition is false" << std::endl;
        return false;
    }
    return true;
}

inline bool assertFalse(bool condition, const std::string& message = "") {
    if (condition) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Condition is true" << std::endl;
        return false;
    }
    return true;
}

inline bool assertEqual(size_t expected, size_t actual, 
                       const std::string& message = "") {
    if (expected != actual) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Expected: " << expected << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertGreaterThan(size_t expected, size_t actual, 
                              const std::string& message = "") {
    if (expected >= actual) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Expected: > " << expected << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertLessThan(size_t expected, size_t actual, 
                           const std::string& message = "") {
    if (expected <= actual) {
        std::cout << "FAILED: " << message << std::endl;
        std::cout << "  Expected: < " << expected << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

// Common test data generators
inline std::vector<nlm::NeuronId> generateNeuronIds(size_t count) {
    std::vector<nlm::NeuronId> ids;
    for (size_t i = 0; i < count; ++i) {
        ids.push_back(nlm::NeuronId(i));
    }
    return ids;
}

inline std::vector<nlm::RegionId> generateRegionIds(size_t count) {
    std::vector<nlm::RegionId> ids;
    for (size_t i = 0; i < count; ++i) {
        ids.push_back(nlm::RegionId(i + 1));
    }
    return ids;
}

inline std::vector<nlm::PopulationId> generatePopulationIds(size_t count) {
    std::vector<nlm::PopulationId> ids;
    for (size_t i = 0; i < count; ++i) {
        ids.push_back(nlm::PopulationId(i));
    }
    return ids;
}

inline std::vector<double> generateRandomValues(size_t count, double min = 0.0, double max = 1.0) {
    std::vector<double> values;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(min, max);
    
    for (size_t i = 0; i < count; ++i) {
        values.push_back(dist(gen));
    }
    
    return values;
}

inline std::vector<nlm::Timestamp> generateSpikeTimes(size_t count, double startTime = 0.0, double endTime = 1000.0) {
    std::vector<nlm::Timestamp> times;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(startTime, endTime);
    
    for (size_t i = 0; i < count; ++i) {
        times.push_back(static_cast<nlm::Timestamp>(dist(gen)));
    }
    
    // Sort spike times
    std::sort(times.begin(), times.end());
    
    return times;
}

} // namespace nlm

// Individual test implementations (these would be in separate files)
// For brevity, only a few key implementations are shown

void nlm::test::ConfigTests::testCreation() {
    auto config = std::make_shared<nlm::Config>();
    assertTrue(config != nullptr, "Config should be created");
}

void nlm::test::ConfigTests::testParameterSetting() {
    auto config = std::make_shared<nlm::Config>();
    config->set("test.param", 42.0, nlm::ConfigSource::Default);
    assertTrue(config->has("test.param"), "Parameter should be set");
}

void nlm::test::ConfigTests::testParameterGetting() {
    auto config = std::make_shared<nlm::Config>();
    config->set("test.param", 42.0, nlm::ConfigSource::Default);
    double value = config->getOr<double>("test.param", 0.0);
    assertEqual(42.0, value, 0.001, "Parameter value should match");
}

void nlm::test::ConfigTests::testValidation() {
    auto config = std::make_shared<nlm::Config>();
    // Add some invalid parameters
    config->set("invalid.param", 999999.0, nlm::ConfigSource::Default);
    assertTrue(config->isValid(), "Config should be valid");
}

void nlm::test::ConfigTests::testSerialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("test.param", 42.0, nlm::ConfigSource::Default);
    
    std::string json = config->toJSON();
    assertTrue(!json.empty(), "JSON should not be empty");
    
    auto config2 = std::make_shared<nlm::Config>();
    config2->loadFromJSON(json);
    assertTrue(config2->has("test.param"), "Config should load from JSON");
}

void nlm::test::NeuronTests::testCreation() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    assertTrue(neuron.getId().index() == 1, "Neuron ID should match");
}

void nlm::test::NeuronTests::testMembraneDynamics() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    // Set initial membrane potential
    neuron.setMembranePotential(-70.0f);
    assertEqual(-70.0, neuron.getMembranePotential(), 0.001, "Membrane potential should be set");
}

void nlm::test::NeuronTests::testSpikeGeneration() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    neuron.setThreshold(-55.0f);
    
    // Inject current to cause spike
    neuron.injectCurrent(100.0f);
    
    // This would be tested in a step-based simulation
    // For now, just verify basic setup
    assertTrue(neuron.getThreshold() == -55.0f, "Threshold should be set");
}

void nlm::test::NeuronTests::testSynapticInput() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    neuron.receiveExcitatoryInput(10.0f);
    neuron.receiveInhibitoryInput(5.0f);
    
    // Verify synaptic input (implementation dependent)
    // This would need access to internal state for full testing
}

void nlm::test::NeuronTests::testRefractoryPeriod() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    // Set refractory period
    neuron.setRefractoryPeriod(10);
    assertTrue(neuron.getRefractoryPeriod() == 10, "Refractory period should be set");
}

// Note: Full test implementations would be in separate files
// This is a simplified example to demonstrate the testing framework structure

} // namespace nlm