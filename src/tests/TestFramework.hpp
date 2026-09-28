// Test framework for NLM
// Provides unit testing capabilities for NLM components

#ifndef NLM_TEST_FRAMEWORK_HPP
#define NLM_TEST_FRAMEWORK_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
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
    std::vector<std::string> logMessages;
    
    TestResult(const std::string& name, const std::string& description)
        : testName(name), testDescription(description), passed(false), executionTime(0.0) {}
};

// Base test class
class Test {
public:
    virtual ~Test() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual void setup() {}
    virtual void teardown() {}
    virtual TestResult run() = 0;
    
    virtual bool setupRequired() const { return false; }
};

// Test case for specific assertions
class TestCase {
public:
    TestCase(std::shared_ptr<Test> test) : test_(test) {}
    
    TestCase& assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            fail("assertTrue failed: " + message);
        }
        return *this;
    }
    
    TestCase& assertFalse(bool condition, const std::string& message = "") {
        if (condition) {
            fail("assertFalse failed: " + message);
        }
        return *this;
    }
    
    TestCase& assertEqual(float a, float b, float tolerance = 1e-6f, const std::string& message = "") {
        if (std::abs(a - b) > tolerance) {
            fail("assertEqual failed: " + message + " (expected " + std::to_string(b) + ", got " + std::to_string(a) + ")");
        }
        return *this;
    }
    
    TestCase& assertNotEqual(float a, float b, float tolerance = 1e-6f, const std::string& message = "") {
        if (std::abs(a - b) <= tolerance) {
            fail("assertNotEqual failed: " + message + " (values too close)");
        }
        return *this;
    }
    
    TestCase& assertGreaterThan(float a, float b, const std::string& message = "") {
        if (a <= b) {
            fail("assertGreaterThan failed: " + message + " (expected > " + std::to_string(b) + ", got " + std::to_string(a) + ")");
        }
        return *this;
    }
    
    TestCase& assertLessThan(float a, float b, const std::string& message = "") {
        if (a >= b) {
            fail("assertLessThan failed: " + message + " (expected < " + std::to_string(b) + ", got " + std::to_string(a) + ")");
        }
        return *this;
    }
    
    TestCase& assertInRange(float value, float min, float max, const std::string& message = "") {
        if (value < min || value > max) {
            fail("assertInRange failed: " + message + " (expected in [" + std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(value) + ")");
        }
        return *this;
    }
    
    TestCase& assertNotNull(const void* ptr, const std::string& message = "") {
        if (ptr == nullptr) {
            fail("assertNotNull failed: " + message);
        }
        return *this;
    }
    
    TestCase& log(const std::string& message) {
        logMessages_.push_back(message);
        return *this;
    }
    
    // For backward compatibility with EXPECT_TRUE style
    void EXPECT_TRUE(bool condition, const std::string& message = "") {
        if (!condition) {
            fail("EXPECT_TRUE failed: " + message);
        }
    }
    
    void EXPECT_FALSE(bool condition, const std::string& message = "") {
        if (condition) {
            fail("EXPECT_FALSE failed: " + message);
        }
    }
    
    void EXPECT_EQ(float a, float b, float tolerance = 1e-6f, const std::string& message = "") {
        if (std::abs(a - b) > tolerance) {
            fail("EXPECT_EQ failed: " + message + " (expected " + std::to_string(b) + ", got " + std::to_string(a) + ")");
        }
    }
    
    void EXPECT_NE(float a, float b, float tolerance = 1e-6f, const std::string& message = "") {
        if (std::abs(a - b) <= tolerance) {
            fail("EXPECT_NE failed: " + message + " (values too close)");
        }
    }
    
    void EXPECT_GT(float a, float b, const std::string& message = "") {
        if (a <= b) {
            fail("EXPECT_GT failed: " + message + " (expected > " + std::to_string(b) + ", got " + std::to_string(a) + ")");
        }
    }
    
    void EXPECT_LT(float a, float b, const std::string& message = "") {
        if (a >= b) {
            fail("EXPECT_LT failed: " + message + " (expected < " + std::to_string(b) + ", got " + std::to_string(a) + ")");
        }
    }
    
    // Get test result
    TestResult getResult() {
        TestResult result(test_->getName(), test_->getDescription());
        result.passed = !failed_;
        result.errorMessage = errorMessage_;
        result.logMessages = logMessages_;
        return result;
    }
    
protected:
    void fail(const std::string& message) {
        failed_ = true;
        errorMessage_ = message;
    }
    
    std::shared_ptr<Test> test_;
    bool failed_ = false;
    std::string errorMessage_;
    std::vector<std::string> logMessages_;
};

// Test suite for running multiple tests
class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description) 
        : name_(name), description_(description) {}
    
    void addTest(std::shared_ptr<Test> test) {
        tests_.push_back(test);
    }
    
    std::vector<TestResult> runAll() {
        std::vector<TestResult> results;
        
        std::cout << "Running test suite: " << name_ << "\n";
        std::cout << "Description: " << description_ << "\n";
        std::cout << "Tests: " << tests_.size() << "\n\n";
        
        for (size_t i = 0; i < tests_.size(); ++i) {
            std::cout << "Test " << (i + 1) << "/" << tests_.size() << ": " 
                      << tests_[i]->getName() << "\n";
            
            if (tests_[i]->setupRequired()) {
                tests_[i]->setup();
            }
            
            TestResult result = tests_[i]->run();
            result.executionTime = 0.0; // TODO: measure actual execution time
            
            results.push_back(result);
            
            // Print result
            std::cout << "  Result: " << (result.passed ? "PASSED" : "FAILED") << "\n";
            if (!result.passed) {
                std::cout << "  Error: " << result.errorMessage << "\n";
            }
            
            if (tests_[i]->setupRequired()) {
                tests_[i]->teardown();
            }
            
            std::cout << "\n";
        }
        
        return results;
    }
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    size_t getTestCount() const { return tests_.size(); }
    
private:
    std::string name_;
    std::string description_;
    std::vector<std::shared_ptr<Test>> tests_;
};

// Test utilities
class TestUtils {
public:
    // Generate random seed based on current time
    static uint64_t generateRandomSeed() {
        return static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
    }
    
    // Format test results
    static std::string formatResults(const std::vector<TestResult>& results) {
        std::stringstream ss;
        
        int passed = 0, failed = 0;
        for (const auto& result : results) {
            if (result.passed) passed++; else failed++;
        }
        
        ss << "Test Results: " << passed << " passed, " << failed << " failed\n";
        ss << "\n";
        
        for (const auto& result : results) {
            ss << result.testName << ": " << (result.passed ? "PASS" : "FAIL");
            if (!result.errorMessage.empty()) {
                ss << " - " << result.errorMessage;
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
    // Save test results to file
    static bool saveResults(const std::string& filepath, const std::vector<TestResult>& results) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << formatResults(results);
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
};

// Macro-style test utilities for easy use
#define NLM_TEST_SUITE(name, description) \
    class name : public Test { \
    public: \
        std::string getName() const override { return #name; } \
        std::string getDescription() const override { return description; } \
    }; \
    \
    class name ## _TestCase : public TestCase { \
    public: \
        name ## _TestCase(std::shared_ptr<name> test) : TestCase(test) {} \
    }; \
    \
    \
    // Helper function to register test suite
    static TestSuite registerTestSuite(const std::string& name, const std::string& description) { \
        return TestSuite(name, description); \
    }

#define NLM_TEST_CASE(suite, testName) \
    class testName : public Test { \
    public: \
        std::string getName() const override { return testName; } \
        std::string getDescription() const override { return "Test case " #testName; } \
        TestResult run() override { \
            TestCase testCase(std::make_shared<testName>(this)); \
            testName instance; \
            instance.setup(); \
            TestResult result = testName::runTest(&testCase); \
            instance.teardown(); \
            return result; \
        } \
    }; \
    \
    TestResult runTest(TestCase* testCase) \

#define NLM_ASSERT_TRUE(condition) testCase->assertTrue(condition, "Assertion failed") \
    .log("assertTrue: " #condition)
#define NLM_ASSERT_FALSE(condition) testCase->assertFalse(condition, "Assertion failed") \
    .log("assertFalse: " #condition)
#define NLM_ASSERT_EQ(a, b, tolerance) testCase->assertEqual(a, b, tolerance, "Equality assertion failed") \
    .log(std::string("assertEq: ") + #a + " == " + #b)
#define NLM_ASSERT_NE(a, b, tolerance) testCase->assertNotEqual(a, b, tolerance, "Inequality assertion failed") \
    .log(std::string("assertNe: ") + #a + " != " + #b)
#define NLM_ASSERT_GT(a, b) testCase->assertGreaterThan(a, b, "Greater than assertion failed") \
    .log(std::string("assertGt: ") + #a + " > " + #b)
#define NLM_ASSERT_LT(a, b) testCase->assertLessThan(a, b, "Less than assertion failed") \
    .log(std::string("assertLt: ") + #a + " < " + #b)
#define NLM_LOG(message) testCase->log(message)

// Test factory for creating and managing tests
class TestFactory {
public:
    static std::vector<std::shared_ptr<TestSuite>> getAllTestSuites();
    static std::shared_ptr<TestSuite> getTestSuite(const std::string& name);
    
private:
    static std::vector<std::shared_ptr<TestSuite>> registerTestSuites();
};

} // namespace nlm

#endif // NLM_TEST_FRAMEWORK_HPP
