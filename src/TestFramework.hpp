// TestFramework.hpp
// Modular test framework for NLM

#ifndef NLM_TESTFRAMEWORK_HPP
#define NLM_TESTFRAMEWORK_HPP

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>

namespace nlm {

// Test result structure
struct TestResult {
    std::string name;              ///< Test name
    bool passed;                   ///< Whether test passed
    std::string message;           ///< Result message
    double duration;               ///< Test duration in seconds
    std::string errorDetails;      ///< Detailed error information if failed
    
    TestResult(const std::string& testName, bool pass, const std::string& msg = "")
        : name(testName), passed(pass), message(msg), duration(0.0), errorDetails("") {}
};

// Test category
enum class TestCategory {
    Unit,          ///< Unit tests (individual components)
    Integration,   ///< Integration tests (system components)
    Performance,   ///< Performance tests
    Stress,        ///< Stress/performance under load
    Functional,    ///< Functional/behavioral tests
    Regression,    ///< Regression tests
    Benchmark      ///< Benchmark tests
};

// Test case base class
class TestCase {
public:
    virtual ~TestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual TestCategory getCategory() const = 0;
    virtual bool run() = 0;
    virtual std::string getDescription() const = 0;
    
    TestResult runWithTiming() {
        auto start = std::chrono::high_resolution_clock::now();
        
        bool passed = false;
        std::string message;
        std::string errorDetails;
        
        try {
            passed = run();
            message = "Test passed";
        } catch (const std::exception& e) {
            message = "Test failed with exception: " + std::string(e.what());
            errorDetails = e.what();
        } catch (...) {
            message = "Test failed with unknown exception";
            errorDetails = "Unknown exception";
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        
        return TestResult(getName(), passed, message);
    }
};

// Test suite class
class TestSuite {
private:
    std::string name;
    std::vector<std::unique_ptr<TestCase>> testCases;
    
public:
    TestSuite(const std::string& suiteName) : name(suiteName) {}
    
    void addTest(std::unique_ptr<TestCase> test) {
        testCases.push_back(std::move(test));
    }
    
    std::string getName() const {
        return name;
    }
    
    TestCategory getCategory() const {
        return TestCategory::Unit; // Default
    }
    
    const std::vector<std::unique_ptr<TestCase>>& getTestCases() const {
        return testCases;
    }
    
    TestResult runTest(const TestCase& test) {
        return test.runWithTiming();
    }
    
    std::vector<TestResult> runAll() {
        std::vector<TestResult> results;
        
        for (const auto& test : testCases) {
            TestResult result = runTest(*test);
            results.push_back(result);
            
            // Output result
            std::cout << "Test: " << result.name << std::endl;
            std::cout << "  Status: " << (result.passed ? "PASS" : "FAIL") << std::endl;
            std::cout << "  Message: " << result.message << std::endl;
            std::cout << "  Duration: " << std::fixed << std::setprecision(3) << result.duration << "s" << std::endl;
            if (!result.errorDetails.empty()) {
                std::cout << "  Error: " << result.errorDetails << std::endl;
            }
            std::cout << std::endl;
        }
        
        return results;
    }
    
    size_t getTestCount() const {
        return testCases.size();
    }
    
    size_t getPassedCount() const {
        size_t count = 0;
        for (const auto& test : testCases) {
            TestResult result = runTest(*test);
            if (result.passed) count++;
        }
        return count;
    }
    
    size_t getFailedCount() const {
        return testCases.size() - getPassedCount();
    }
};

// Test macros for convenience
#define TEST_CASE(name, category) \
    class name##Test : public TestCase { \
    public: \
        std::string getName() const override { return #name; } \
        TestCategory getCategory() const override { return category; } \
        std::string getDescription() const override { return "Test case " #name; } \
        bool run() override

#define TEST_FIXTURE(name, category, setup, teardown) \
    class name##Fixture : public TestCase { \
    public: \
        std::string getName() const override { return #name; } \
        TestCategory getCategory() const override { return category; } \
        std::string getDescription() const override { return "Fixture test " #name; } \
        bool run() override \
        bool setup(); \
        bool teardown(); \
        static name##Fixture& getInstance()

// Test framework manager
class TestFramework {
private:
    std::vector<std::unique_ptr<TestSuite>> suites;
    bool verbose = false;
    bool quiet = false;
    std::string outputFile;
    
public:
    void addSuite(std::unique_ptr<TestSuite> suite) {
        suites.push_back(std::move(suite));
    }
    
    void setVerbose(bool verbose) {
        this->verbose = verbose;
    }
    
    void setQuiet(bool quiet) {
        this->quiet = quiet;
    }
    
    void setOutputFile(const std::string& filename) {
        outputFile = filename;
    }
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> allResults;
        
        for (const auto& suite : suites) {
            std::cout << "Running test suite: " << suite->getName() << std::endl;
            std::cout << std::string(50, '=') << std::endl;
            
            std::vector<TestResult> results = suite->runAll();
            allResults.insert(allResults.end(), results.begin(), results.end());
        }
        
        // Summary report
        printTestSummary(allResults);
        
        // Export results if requested
        if (!outputFile.empty()) {
            exportResults(allResults, outputFile);
        }
        
        return allResults;
    }
    
    void printTestSummary(const std::vector<TestResult>& results) {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        
        size_t totalTests = results.size();
        size_t passedTests = 0;
        size_t failedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
            totalDuration += result.duration;
        }
        
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed:      " << passedTests << std::endl;
        std::cout << "Failed:      " << failedTests << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
        std::cout << "Total time:  " << std::fixed << std::setprecision(3) << totalDuration << "s" << std::endl;
        
        if (failedTests > 0) {
            std::cout << std::endl;
            std::cout << "=== Failed Tests ===" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << ": " << result.message << std::endl;
                }
            }
        }
    }
    
    void exportResults(const std::vector<TestResult>& results, const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
            return;
        }
        
        outFile << "NLM Test Results Export" << std::endl;
        outFile << "=======================" << std::endl;
        outFile << "Generated: " << __DATE__ " " __TIME__ << std::endl;
        outFile << "Total tests: " << results.size() << std::endl;
        outFile << "Passed: " << std::count_if(results.begin(), results.end(),
            [](const TestResult& r) { return r.passed; }) << std::endl;
        outFile << "Failed: " << std::count_if(results.begin(), results.end(),
            [](const TestResult& r) { return !r.passed; }) << std::endl;
        outFile << std::endl;
        
        for (const auto& result : results) {
            outFile << "Test: " << result.name << std::endl;
            outFile << "  Category: " << static_cast<int>(result.getCategory()) << std::endl;
            outFile << "  Status: " << (result.passed ? "PASS" : "FAIL") << std::endl;
            outFile << "  Message: " << result.message << std::endl;
            outFile << "  Duration: " << result.duration << "s" << std::endl;
            if (!result.errorDetails.empty()) {
                outFile << "  Error: " << result.errorDetails << std::endl;
            }
            outFile << std::endl;
        }
        
        outFile.close();
        std::cout << "Test results exported to: " << filename << std::endl;
    }
};

} // namespace nlm

#endif // NLM_TESTFRAMEWORK_HPP