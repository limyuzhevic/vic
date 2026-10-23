#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <cassert>

namespace nlm {

// Test result structure
struct TestResult {
    std::string testName;
    std::string testDescription;
    bool passed;
    std::string errorMessage;
    double executionTime;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::map<std::string, std::string> metadata;
    
    TestResult(const std::string& name, const std::string& description)
        : testName(name), testDescription(description), passed(false),
          executionTime(0.0), startTime(std::chrono::system_clock::now()) {}
    
    void complete(bool success, const std::string& error = "") {
        endTime = std::chrono::system_clock::now();
        passed = success;
        errorMessage = error;
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        executionTime = duration;
    }
};

// Test case base class
class ITest {
public:
    virtual ~ITest() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool run() = 0;
    virtual std::string getCategory() const = 0;
    
    // Optional setup and teardown
    virtual void setup() {}
    virtual void teardown() {}
    
    // Time the test execution
    TestResult runWithTiming() {
        TestResult result(getName(), getDescription());
        auto start = std::chrono::system_clock::now();
        
        try {
            bool success = run();
            result.complete(success);
        } catch (const std::exception& e) {
            result.complete(false, std::string("Exception: ") + e.what());
        } catch (...) {
            result.complete(false, "Unknown exception");
        }
        
        auto end = std::chrono::system_clock::now();
        result.endTime = end;
        auto duration = std::chrono::duration<double>(end - start).count();
        result.executionTime = duration;
        
        return result;
    }
};

// Test suite that manages multiple tests
class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description);
    ~TestSuite();
    
    // Add a test
    void addTest(std::unique_ptr<ITest> test);
    
    // Run all tests in the suite
    std::vector<TestResult> runAllTests();
    
    // Run tests by category
    std::vector<TestResult> runTestsInCategory(const std::string& category);
    
    // Get test count
    size_t getTestCount() const;
    size_t getPassedCount() const;
    size_t getFailedCount() const;
    
    // Get test names
    std::vector<std::string> getTestNames() const;
    
    // Get suite info
    std::string getName() const;
    std::string getDescription() const;
    
    // Generate report
    std::string generateReport(const std::vector<TestResult>& results) const;
    std::string generateJSONReport(const std::vector<TestResult>& results) const;
    
    // Clear all tests
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Test framework for specific components
class BrainTests {
public:
    static std::unique_ptr<TestSuite> createBrainTests();
    static std::unique_ptr<TestSuite> createNeuronTests();
    static std::unique_ptr<TestSuite> createSynapseTests();
    static std::unique_ptr<TestSuite> createPlasticityTests();
    static std::unique_ptr<TestSuite> createDevelopmentTests();
};

class MemoryTests {
public:
    static std::unique_ptr<TestSuite> createMemoryTests();
};

class NeuromodulationTests {
public:
    static std::unique_ptr<TestSuite> createNeuromodulationTests();
};

class PredictionTests {
public:
    static std::unique_ptr<TestSuite> createPredictionTests();
};

class CognitionTests {
public:
    static std::unique_ptr<TestSuite> createCognitionTests();
};

class IntegrationTests {
public:
    static std::unique_ptr<TestSuite> createIntegrationTests();
};

class PerformanceTests {
public:
    static std::unique_ptr<TestSuite> createPerformanceTests();
};

// Test case implementations
class NeuronTest : public ITest {
public:
    std::string getName() const override { return "Neuron LIF Dynamics"; }
    std::string getDescription() const override { return "Tests Leaky Integrate-and-Fire neuron dynamics"; }
    std::string getCategory() const override { return "Neuron"; }
    bool run() override;
};

class SynapseTest : public ITest {
public:
    std::string getName() const override { return "Synapse Transmission"; }
    std::string getDescription() const override { return "Tests synaptic transmission and plasticity"; }
    std::string getCategory() const override { return "Synapse"; }
    bool run() override;
};

class STDPTest : public ITest {
public:
    std::string getName() const override { return "STDP Learning Rule"; }
    std::string getDescription() const override { return "Tests spike-timing dependent plasticity"; }
    std::string getCategory() const override { return "Plasticity"; }
    bool run() override;
};

class WorkingMemoryTest : public ITest {
public:
    std::string getName() const override { return "Working Memory"; }
    std::string getDescription() const override { return "Tests working memory functionality"; }
    std::string getCategory() const override { return "Memory"; }
    bool run() override;
};

class DopamineTest : public ITest {
public:
    std::string getName() const override { return "Dopamine Neuromodulation"; }
    std::string getDescription() const override { return "Tests dopamine-based neuromodulation"; }
    std::string getCategory() const override { return "Neuromodulation"; }
    bool run() override;
};

class PredictionSystemTest : public ITest {
public:
    std::string getName() const override { return "Prediction System"; }
    std::string getDescription() const override { return "Tests prediction error and learning"; }
    std::string getCategory() const override { return "Prediction"; }
    bool run() override;
};

class ConceptFormationTest : public ITest {
public:
    std::string getName() const override { return "Concept Formation"; }
    std::string getDescription() const override { return "Tests concept formation from experience"; }
    std::string getCategory() const override { return "Cognition"; }
    bool run() override;
};

class DevelopmentalStagesTest : public ITest {
public:
    std::string getName() const override { return "Developmental Stages"; }
    std::string getDescription() const override { return "Tests developmental stage modulation of plasticity"; }
    std::string getCategory() const override { return "Development"; }
    bool run() override;
};

class IntegrationTest : public ITest {
public:
    std::string getName() const override { return "Full Integration"; }
    std::string getDescription() const override { return "Tests integration of all brain systems"; }
    std::string getCategory() const override { return "Integration"; }
    bool run() override;
};

class PerformanceTest : public ITest {
public:
    std::string getName() const override { return "Performance Benchmark"; }
    std::string getDescription() const override { return "Benchmarks simulation performance"; }
    std::string getCategory() const override { return "Performance"; }
    bool run() override;
};

// Test assertion helpers
#define ASSERT_TRUE(condition, message) assertCondition(condition, message, __FILE__, __LINE__)
#define ASSERT_FALSE(condition, message) assertCondition(!condition, message, __FILE__, __LINE__)
#define ASSERT_EQ(expected, actual, message) assertEquality(expected, actual, message, __FILE__, __LINE__)
#define ASSERT_NE(expected, actual, message) assertCondition(expected != actual, message, __FILE__, __LINE__)
#define ASSERT_GREATER(actual, expected, message) assertCondition(actual > expected, message, __FILE__, __LINE__)
#define ASSERT_LESS(actual, expected, message) assertCondition(actual < expected, message, __FILE__, __LINE__)
#define ASSERT_WITHIN(actual, expected, tolerance, message) assertCondition(std::abs(actual - expected) <= tolerance, message, __FILE__, __LINE__)

// Utility functions
void assertCondition(bool condition, const std::string& message, const std::string& file, int line);
template<typename T> void assertEquality(const T& expected, const T& actual, const std::string& message, const std::string& file, int line);

// Test framework main function
void runAllTests(std::ostream& output = std::cout);

} // namespace nlm
