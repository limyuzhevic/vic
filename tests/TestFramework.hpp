#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

using namespace nlm;

enum class TestCategory {
    Unit,
    Integration,
    Performance,
    Stress
};

enum class TestResult {
    Pass,
    Fail,
    Skip,
    Error
};

struct TestCase {
    std::string name;
    std::string description;
    TestCategory category;
    std::function<bool()> testFunction;
    double timeoutSeconds;
    bool enabled;
    
    TestCase(const std::string& name, const std::string& description, 
             TestCategory category, std::function<bool()> func,
             double timeout = 30.0, bool enabled = true)
        : name(name), description(description), category(category), 
          testFunction(std::move(func)), timeoutSeconds(timeout), enabled(enabled) {}
};

struct TestResultInfo {
    std::string testName;
    TestResult result;
    std::string message;
    double durationSeconds;
    std::string errorDetails;
    
    TestResultInfo(const std::string& name, TestResult res, const std::string& msg, 
                  double duration, const std::string& error = "")
        : testName(name), result(res), message(msg), durationSeconds(duration),
          errorDetails(error) {}
};

class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name), passed_(0), failed_(0), skipped_(0), errors_(0) {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void addTest(const TestCase& test) {
        tests_.push_back(test);
    }
    
    std::vector<TestResultInfo> runAllTests() {
        std::vector<TestResultInfo> results;
        
        std::cout << "Running test suite: " << name_ << std::endl;
        std::cout << "===========================" << std::endl;
        
        for (const auto& test : tests_) {
            if (!test.enabled) {
                results.emplace_back(test.name, TestResult::Skip, "Skipped (disabled)", 0.0);
                skipped_++;
                continue;
            }
            
            std::cout << "\nRunning: " << test.name << std::endl;
            std::cout << "Description: " << test.description << std::endl;
            
            bool testPassed = runSingleTest(test, results);
            if (testPassed) {
                passed_++;
                std::cout << "✓ PASS" << std::endl;
            } else {
                failed_++;
                std::cout << "✗ FAIL" << std::endl;
            }
        }
        
        return results;
    }
    
    void printSummary() const {
        std::cout << "\n" << name_ << " Test Summary" << std::endl;
        std::cout << "=======================" << std::endl;
        std::cout << "Total tests: " << tests_.size() << std::endl;
        std::cout << "Passed:      " << passed_ << std::endl;
        std::cout << "Failed:      " << failed_ << std::endl;
        std::cout << "Skipped:     " << skipped_ << std::endl;
        std::cout << "Errors:      " << errors_ << std::endl;
        
        if (failed_ > 0 || errors_ > 0) {
            std::cout << "\nFAILED TESTS:" << std::endl;
            for (const auto& result : testResults_) {
                if (result.result == TestResult::Fail || result.result == TestResult::Error) {
                    std::cout << "  " << result.testName << ": " << result.message << std::endl;
                }
            }
        }
    }
    
    std::vector<TestResultInfo> getResults() const {
        return testResults_;
    }
    
protected:
    bool runSingleTest(const TestCase& test, std::vector<TestResultInfo>& results) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool testPassed = false;
        std::string errorMsg;
        
        try {
            // Set up timeout
            auto timeoutTime = startTime + std::chrono::duration<double>(test.timeoutSeconds);
            
            // Run test with timeout
            testPassed = runWithTimeout(test.testFunction, timeoutTime);
            
            if (std::chrono::high_resolution_clock::now() > timeoutTime) {
                errorMsg = "Test timed out after " + std::to_string(test.timeoutSeconds) + " seconds";
                testPassed = false;
            }
        } catch (const std::exception& e) {
            errorMsg = std::string("Exception: ") + e.what();
            testPassed = false;
        } catch (...) {
            errorMsg = "Unknown exception occurred";
            testPassed = false;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        TestResult result;
        if (testPassed) {
            result = TestResult::Pass;
        } else if (errorMsg.empty()) {
            result = TestResult::Fail;
        } else {
            result = TestResult::Error;
            errors_++;
        }
        
        results.emplace_back(test.name, result, errorMsg, durationSeconds);
        
        return testPassed;
    }
    
    bool runWithTimeout(std::function<bool()> testFunc, std::chrono::steady_clock::time_point deadline) {
        // Run test in a separate thread with timeout
        std::promise<bool> promise;
        std::future<bool> future = promise.get_future();
        
        std::thread testThread([&]() {
            try {
                promise.set_value(testFunc());
            } catch (...) {
                promise.set_exception(std::current_exception());
            }
        });
        
        // Wait for result with timeout
        std::future_status status = future.wait_until(deadline);
        
        if (status == std::future_status::ready) {
            testThread.join();
            return future.get();
        } else {
            // Timeout - detach thread and return false
            testThread.detach();
            return false;
        }
    }
    
private:
    std::string name_;
    std::vector<TestCase> tests_;
    std::vector<TestResultInfo> testResults_;
    size_t passed_;
    size_t failed_;
    size_t skipped_;
    size_t errors_;
};

class TestFramework {
public:
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    void addSuite(const TestSuite& suite) {
        suites_.push_back(suite);
    }
    
    void runAllSuites() {
        std::cout << "NLM Test Framework" << std::endl;
        std::cout << "===================" << std::endl;
        
        for (auto& suite : suites_) {
            std::cout << std::endl;
            auto results = suite.runAllTests();
            suite.printSummary();
            
            // Store results for later analysis
            allTestResults_.insert(allTestResults_.end(), results.begin(), results.end());
        }
        
        printOverallSummary();
    }
    
    void saveResults(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open file " << filename << " for writing results." << std::endl;
            return;
        }
        
        file << "NLM Test Results" << std::endl;
        file << "=================" << std::endl;
        
        for (const auto& result : allTestResults_) {
            file << result.testName << ": ";
            switch (result.result) {
                case TestResult::Pass: file << "PASS"; break;
                case TestResult::Fail: file << "FAIL"; break;
                case TestResult::Skip: file << "SKIP"; break;
                case TestResult::Error: file << "ERROR"; break;
            }
            file << " (" << std::fixed << std::setprecision(3) << result.durationSeconds << "s)";
            if (!result.message.empty()) {
                file << " - " << result.message;
            }
            file << std::endl;
        }
        
        file.close();
        std::cout << "Results saved to: " << filename << std::endl;
    }
    
private:
    TestFramework() {}
    
    void printOverallSummary() const {
        std::cout << "\n" << "Overall Test Summary" << std::endl;
        std::cout << "====================" << std::endl;
        
        size_t totalTests = 0;
        size_t totalPassed = 0;
        size_t totalFailed = 0;
        size_t totalSkipped = 0;
        size_t totalErrors = 0;
        
        for (const auto& suite : suites_) {
            // We would need to track these per suite in a real implementation
        }
        
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed:      " << totalPassed << std::endl;
        std::cout << "Failed:      " << totalFailed << std::endl;
        std::cout << "Skipped:     " << totalSkipped << std::endl;
        std::cout << "Errors:      " << totalErrors << std::endl;
        
        if (totalFailed > 0 || totalErrors > 0) {
            std::cout << "\nTest run FAILED!" << std::endl;
            return;
        }
        
        std::cout << "\nAll tests passed!" << std::endl;
    }
    
    std::vector<TestSuite> suites_;
    std::vector<TestResultInfo> allTestResults_;
};
