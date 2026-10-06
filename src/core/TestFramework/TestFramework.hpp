#ifndef NLM_TEST_FRAMEWORK_HPP
#define NLM_TEST_FRAMEWORK_HPP

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <functional>

namespace nlm {

class TestResult {
public:
    enum class Status { PASSED, FAILED, SKIPPED, ERROR };
    
    TestResult(const std::string& name, Status status, 
              const std::string& message = "", 
              double duration = 0.0)
        : name_(name), status_(status), message_(message), duration_(duration) {}
    
    const std::string& getName() const { return name_; }
    Status getStatus() const { return status_; }
    const std::string& getMessage() const { return message_; }
    double getDuration() const { return duration_; }
    
    std::string getStatusString() const {
        switch (status_) {
            case Status::PASSED: return "PASS";
            case Status::FAILED: return "FAIL";
            case Status::SKIPPED: return "SKIP";
            case Status::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

private:
    std::string name_;
    Status status_;
    std::string message_;
    double duration_;
};

class TestFramework {
public:
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    void addTest(const std::string& name, std::function<void()> testFunc) {
        tests_.push_back({name, testFunc});
    }
    
    void runTests() {
        std::cout << "=== NLM Test Framework ===" << std::endl;
        std::cout << "Running " << tests_.size() << " tests..." << std::endl << std::endl;
        
        int passed = 0, failed = 0, skipped = 0, errors = 0;
        
        for (auto& test : tests_) {
            TestResult result = runSingleTest(test.name_, test.testFunc);
            printResult(result);
            
            switch (result.getStatus()) {
                case TestResult::Status::PASSED: passed++; break;
                case TestResult::Status::FAILED: failed++; break;
                case TestResult::Status::SKIPPED: skipped++; break;
                case TestResult::Status::ERROR: errors++; break;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << tests_.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Skipped: " << skipped << std::endl;
        std::cout << "Errors: " << errors << std::endl;
        std::cout << std::endl;
        
        if (failed > 0 || errors > 0) {
            throw std::runtime_error("Test suite failed: " + std::to_string(failed) + " failed, " + std::to_string(errors) + " errors");
        }
    }
    
    TestResult runSingleTest(const std::string& name, std::function<void()> testFunc) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            testFunc();
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            return TestResult(name, TestResult::Status::PASSED, "", duration);
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            return TestResult(name, TestResult::Status::ERROR, 
                             std::string("Exception: ") + e.what(), duration);
        }
    }
    
    void printResult(const TestResult& result) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "[" << std::fixed << std::setprecision(3) << result.getDuration() << "s] "
                  << result.getStatusString() << " " << result.getName();
        
        if (!result.getMessage().empty()) {
            std::cout << " - " << result.getMessage();
        }
        std::cout << std::endl;
    }

private:
    TestFramework() = default;
    
    struct Test {
        std::string name;
        std::function<void()> testFunc;
    };
    
    std::vector<Test> tests_;
};

// Macro for test registration
#define REGISTER_TEST(testName) \
    class testName##_wrapper { \
    public: \
        testName##_wrapper() { \
            TestFramework::getInstance().addTest(#testName, testName##_func); \
        } \
        static void testName##_func(); \
    }; \
    testName##_wrapper testName##_instance; \
    void testName##_wrapper::testName##_func()

// Test assertions
#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            throw std::runtime_error("Assertion failed: " #expected " != " #actual " at " __FILE__ ":" #line); \
        } \
    } while(0)

#define ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            throw std::runtime_error("Assertion failed: " #expected " == " #actual " at " __FILE__ ":" #line); \
        } \
    } while(0)

#define ASSERT_GT(actual, value) \
    do { \
        if ((actual) <= (value)) { \
            throw std::runtime_error("Assertion failed: " #actual " > " #value " at " __FILE__ ":" #line); \
        } \
    } while(0)

#define ASSERT_LT(actual, value) \
    do { \
        if ((actual) >= (value)) { \
            throw std::runtime_error("Assertion failed: " #actual " < " #value " at " __FILE__ ":" #line); \
        } \
    } while(0)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition " at " __FILE__ ":" #line); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if ((condition)) { \
            throw std::runtime_error("Assertion failed: not " #condition " at " __FILE__ ":" #line); \
        } \
    } while(0)

#define ASSERT_THROW(expr, exceptionType) \
    do { \
        bool caught = false; \
        try { expr; } catch (const exceptionType&) { caught = true; } \
        catch (...) {} \
        if (!caught) { \
            throw std::runtime_error("Assertion failed: expected " #exceptionType " to be thrown at " __FILE__ ":" #line); \
        } \
    } while(0)

} // namespace nlm

#endif // NLM_TEST_FRAMEWORK_HPP
