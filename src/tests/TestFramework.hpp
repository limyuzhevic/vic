#pragma once

#include "TestConfiguration.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace nlm {

class TestFramework {
public:
    class TestCase {
    public:
        TestCase(const std::string& name, TestLevel level, TestCategory category);
        virtual ~TestCase() = default;
        
        const std::string& getName() const { return name; }
        TestLevel getLevel() const { return level; }
        TestCategory getCategory() const { return category; }
        
        virtual void setup();
        virtual void teardown();
        virtual void run() = 0;
        virtual bool isParameterized() const { return false; }
        
        void setFixture(std::shared_ptr<void> fixture) { this->fixture = fixture; }
        template<typename T>
        std::shared_ptr<T> getFixture() { return std::static_pointer_cast<T>(fixture); }
        
    protected:
        std::string name;
        TestLevel level;
        TestCategory category;
        std::shared_ptr<void> fixture;
    };
    
    class ParameterizedTestCase : public TestCase {
    public:
        using ParameterSet = std::vector<std::pair<std::string, std::string>>;
        
        ParameterizedTestCase(const std::string& name, TestLevel level, TestCategory category);
        void addParameter(const std::string& key, const std::string& value);
        const std::vector<ParameterSet>& getParameterSets() const { return parameterSets; }
        void run() override;
        bool isParameterized() const override { return true; }
        
    private:
        virtual void runParameterizedTest(const ParameterSet& params) = 0;
        std::vector<ParameterSet> parameterSets;
    };
    
    class PropertyBasedTestCase : public TestCase {
    public:
        PropertyBasedTestCase(const std::string& name, TestLevel level, TestCategory category);
        virtual ~PropertyBasedTestCase() = default;
        
        void setPropertyGenerator(std::function<std::string(int)> generator);
        void setPropertyValidator(std::function<bool(const std::string&)> validator);
        void setMaxTests(int maxTests) { maxTestsCount = maxTests; }
        
        void run() override;
        
    private:
        virtual void runPropertyTest(const std::string& input) = 0;
        std::function<std::string(int)> propertyGenerator;
        std::function<bool(const std::string&)> propertyValidator;
        int maxTestsCount;
    };
    
    class FuzzTestCase : public TestCase {
    public:
        FuzzTestCase(const std::string& name, TestLevel level, TestCategory category);
        void setInputGenerator(std::function<std::string(int)> generator);
        void setMaxIterations(int maxIter) { maxIterations = maxIter; }
        void setTimeout(double timeoutSeconds) { timeout = timeoutSeconds; }
        
        void run() override;
        
    private:
        virtual void runFuzzTest(const std::string& input) = 0;
        std::function<std::string(int)> inputGenerator;
        int maxIterations;
        double timeout;
    };
    
    class ModelBasedTestCase : public TestCase {
    public:
        struct State {
            std::map<std::string, std::string> values;
            std::string toString() const;
        };
        
        ModelBasedTestCase(const std::string& name, TestLevel level, TestCategory category);
        void setInitialState(const State& state) { initialState = state; }
        void setTransitionFunction(std::function<State(const State&, const std::string&)> func);
        void setPropertyFunction(std::function<bool(const State&)> func);
        
        void run() override;
        
    private:
        virtual void runStateTransitionTest(const State& fromState, const std::string& input, const State& toState) = 0;
        State initialState;
        std::function<State(const State&, const std::string&)> transitionFunc;
        std::function<bool(const State&)> propertyFunc;
    };
    
    TestFramework();
    ~TestFramework() = default;
    
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    void registerTest(std::unique_ptr<TestCase> testCase);
    void registerTestSuite(const std::string& suiteName, const std::vector<std::unique_ptr<TestCase>>& testCases);
    
    void setConfiguration(const TestConfiguration& config);
    const TestConfiguration& getConfiguration() const { return config; }
    
    void runAllTests();
    void runTestsByCategory(TestCategory category);
    void runTestsByLevel(TestLevel level);
    
    const std::vector<TestResult>& getTestResults() const { return testResults; }
    const std::vector<BenchmarkResult>& getBenchmarkResults() const { return benchmarkResults; }
    
    // Advanced assertion macros
    class Assertion {
    public:
        static void require(bool condition, const std::string& message = "Assertion failed");
        static void ensure(bool condition, const std::string& message = "Ensure failed");
        static void assertEqual(const std::string& expected, const std::string& actual, const std::string& message = "Values not equal");
        static void assertEqual(double expected, double actual, double tolerance = 1e-6, const std::string& message = "Values not equal within tolerance");
        static void assertNotEqual(const std::string& expected, const std::string& actual, const std::string& message = "Values unexpectedly equal");
        static void assertGreaterThan(double expected, double actual, const std::string& message = "Expected not greater than actual");
        static void assertLessThan(double expected, double actual, const std::string& message = "Expected not less than actual");
        static void assertInRange(double value, double min, double max, const std::string& message = "Value not in range");
        static void assertTrue(bool condition, const std::string& message = "Condition not true");
        static void assertFalse(bool condition, const std::string& message = "Condition not false");
        static void assertNull(const void* ptr, const std::string& message = "Pointer is not null");
        static void assertNotNull(const void* ptr, const std::string& message = "Pointer is null");
        static void assertEmpty(const std::string& str, const std::string& message = "String is not empty");
        static void assertNotEmpty(const std::string& str, const std::string& message = "String is empty");
        static void assertContains(const std::string& str, const std::string& substr, const std::string& message = "Substring not found");
        static void assertNotContains(const std::string& str, const std::string& substr, const std::string& message = "Unexpected substring found");
        static void assertThrows(std::function<void()> func, const std::string& message = "Expected exception not thrown");
        static void assertNoThrow(std::function<void()> func, const std::string& message = "Unexpected exception thrown");
        
        // Performance assertions
        static void assertPerformance(const std::string& benchmarkName, double targetTime, double tolerance = 0.1, const std::string& message = "Performance target not met");
        static void assertMemoryUsage(const std::string& benchmarkName, double targetMemoryMB, double tolerance = 0.2, const std::string& message = "Memory target not met");
        
        // Property-based assertion helpers
        static bool propertyMatch(const std::string& input, std::function<bool(const std::string&)> validator);
        static double calculateProperty(const std::string& input, std::function<double(const std::string&)> calculator);
    };
    
    // Test case factory methods
    template<typename T, typename... Args>
    static std::unique_ptr<T> createTest(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    // Fixture management
    template<typename T>
    static std::shared_ptr<T> createFixture() {
        return std::make_shared<T>();
    }
    
    // Output formatting
    static std::string formatTestResult(const TestResult& result);
    static std::string formatTestSummary(const std::vector<TestResult>& results);
    static std::string formatBenchmarkResult(const BenchmarkResult& result);
    
private:
    TestConfiguration config;
    std::vector<std::unique_ptr<TestCase>> testCases;
    std::vector<TestResult> testResults;
    std::vector<BenchmarkResult> benchmarkResults;
    std::mutex resultsMutex;
    
    void runTestCase(TestCase& testCase);
    void executeBenchmark(const std::string& name, std::function<void()> benchmarkFunc);
};

} // namespace nlm
