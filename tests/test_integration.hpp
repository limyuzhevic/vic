// Integration test header file
#ifndef TEST_INTEGRATION_HPP
#define TEST_INTEGRATION_HPP

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class Neuron;
class WorkingMemory;
class EpisodicMemory;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionSystem;
class InternalSignals;
class SensoryInput;
class Action;
class NeuralRegion;
class Synapse;
class SpikeSystem;
class SensoryPercept;
class MotorCommand;
class RegionId;
class NeuronId;
class SynapseId;
class SynapseHandle;
class PopulationId;
class NeuronState;
class PlasticityFlags;
class DevelopmentalStage;

// Test result type
enum class TestResult {
    PASSED,
    FAILED,
    SKIPPED
};

// Test framework utilities
class TestFramework {
public:
    static void logTest(const std::string& testName, TestResult result, const std::string& message = "");
    static bool assert(bool condition, const std::string& message = "");
    static bool assertEqual(const std::string& actual, const std::string& expected, const std::string& message = "");
    static bool assertNotEqual(const std::string& actual, const std::string& expected, const std::string& message = "");
    static bool assertGreaterThan(double actual, double expected, const std::string& message = "");
    static bool assertLessThan(double actual, double expected, const std::string& message = "");
    static bool assertContains(const std::string& actual, const std::string& expected, const std::string& message = "");
    static bool assertNotContains(const std::string& actual, const std::string& expected, const std::string& message = "");
    static bool assertTrue(bool actual, const std::string& message = "");
    static bool assertFalse(bool actual, const std::string& message = "");
    static bool assertNotNull(const void* ptr, const std::string& message = "");
    static bool assertNull(const void* ptr, const std::string& message = "");
    
    static void cleanup();
};

// Test macros for easier test writing
#define TEST(name, condition) \
    do { \
        std::string testName = name; \
        bool testResult = (condition); \
        nlm::TestFramework::logTest(testName, testResult ? TestResult::PASSED : TestResult::FAILED, ""); \
        if (!testResult) { \
            std::cout << "❌ Test " << testName << " FAILED" << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << #condition << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT_EQUAL(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            std::cerr << "Assertion failed: " << #actual << " != " << #expected << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT_NOT_EQUAL(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            std::cerr << "Assertion failed: " << #actual << " == " << #expected << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT_GREATER_THAN(actual, expected) \
    do { \
        if ((actual) <= (expected)) { \
            std::cerr << "Assertion failed: " << #actual << " <= " << #expected << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT_LESS_THAN(actual, expected) \
    do { \
        if ((actual) >= (expected)) { \
            std::cerr << "Assertion failed: " << #actual << " >= " << #expected << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT_TRUE(actual) \
    do { \
        if (!(actual)) { \
            std::cerr << "Assertion failed: " << #actual << " is not true" << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT_FALSE(actual) \
    do { \
        if ((actual)) { \
            std::cerr << "Assertion failed: " << #actual << " is not false" << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == nullptr) { \
            std::cerr << "Assertion failed: " << #ptr << " is null" << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

#define ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != nullptr) { \
            std::cerr << "Assertion failed: " << #ptr << " is not null" << std::endl; \
            return TestResult::FAILED; \
        } \
    } while(0)

// Main integration test runner
class IntegrationTestRunner {
public:
    static void registerTest(const std::string& name, std::function<TestResult()> testFunc);
    static void runAllTests();
    static void printSummary();
    
private:
    static std::vector<std::pair<std::string, std::function<TestResult()>>> tests;
    static int totalTests;
    static int passedTests;
    static int failedTests;
};

} // namespace nlm

#endif // TEST_INTEGRATION_HPP