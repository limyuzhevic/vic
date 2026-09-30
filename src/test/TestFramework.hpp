// NLM Test Framework
// Comprehensive test organization for NLM brain simulation

#ifndef NLM_TEST_FRAMEWORK_H
#define NLM_TEST_FRAMEWORK_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <algorithm>

namespace nlm {
namespace test {

// Test result structure
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string message;
    double executionTime;
    size_t neuronCount;
    size_t synapseCount;
    size_t spikeCount;
    
    TestResult(const std::string& testName, const std::string& testCategory)
        : name(testName), category(testCategory), passed(false), executionTime(0.0),
          neuronCount(0), synapseCount(0), spikeCount(0) {}
};

// Base test case class
class TestCase {
public:
    virtual ~TestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getCategory() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual void setup() {}
    virtual void teardown() {}
    
    virtual void run() = 0;
    
    TestResult execute() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        setup();
        TestResult result(getName(), getCategory());
        
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

// Test suite for organizing related tests
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
    
    std::vector<TestResult> runAll() {
        std::vector<TestResult> results;
        runAll(results);
        return results;
    }
};

// Performance test mixin for measuring system performance
class PerformanceTestMixin {
public:
    virtual ~PerformanceTestMixin() = default;
    
    virtual double getAverageNeuronExcitability(const std::shared_ptr<Brain>& brain) const {
        double totalExcitability = 0.0;
        size_t count = 0;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& neuron : region->getAllNeurons()) {
                totalExcitability += neuron->getExcitability();
                count++;
            }
        }
        
        return count > 0 ? totalExcitability / count : 0.0;
    }
    
    virtual double getAverageSynapticStrength(const std::shared_ptr<Brain>& brain) const {
        double totalStrength = 0.0;
        size_t count = 0;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                totalStrength += synapse->getWeight();
                count++;
            }
        }
        
        return count > 0 ? totalStrength / count : 0.0;
    }
    
    virtual size_t getTotalSpikes(const std::shared_ptr<Brain>& brain) const {
        return brain->getTotalSpikeCount();
    }
    
    virtual size_t getActiveNeuronCount(const std::shared_ptr<Brain>& brain) const {
        return brain->getActiveNeuronCount();
    }
};

// Test result reporter for formatted output
class TestReporter {
public:
    static void printTestHeader(const std::vector<TestResult>& results) {
        std::cout << "" << std::endl;
        std::cout << "=" << std::string(70, '=') << std::endl;
        std::cout << "                        NLM TEST SUITE RESULTS" << std::endl;
        std::cout << "=" << std::string(70, '=') << std::endl;
        std::cout << "" << std::endl;
        
        // Category summary
        std::cout << std::left << std::setw(25) << "Category" 
                  << std::setw(15) << "Tests" 
                  << std::setw(10) << "Passed" 
                  << std::setw(10) << "Failed" 
                  << std::setw(15) << "Success Rate" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        std::map<std::string, std::pair<size_t, size_t>> categoryStats;
        for (const auto& result : results) {
            auto& stats = categoryStats[result.category];
            stats.first++;
            if (!result.passed) stats.second++;
        }
        
        for (const auto& pair : categoryStats) {
            double successRate = ((double)(pair.first - pair.second) / pair.first) * 100.0;
            std::cout << std::left << std::setw(25) << pair.first
                      << std::setw(15) << pair.first
                      << std::setw(10) << (pair.first - pair.second)
                      << std::setw(10) << pair.second
                      << std::setw(15) << std::fixed << std::setprecision(1) << successRate << "%" << std::endl;
        }
        
        std::cout << "" << std::endl;
    }
    
    static void printTestDetails(const std::vector<TestResult>& results, bool verbose = false) {
        std::cout << std::left << std::setw(50) << "Test Name"
                  << std::setw(15) << "Category"
                  << std::setw(12) << "Status"
                  << std::setw(12) << "Time (s)"
                  << std::setw(12) << "Neurons"
                  << std::setw(12) << "Synapses"
                  << std::setw(12) << "Spikes" << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(50) << result.name
                      << std::setw(15) << result.category
                      << std::setw(12) << (result.passed ? "PASS" : "FAIL")
                      << std::setw(12) << std::fixed << std::setprecision(4) << result.executionTime
                      << std::setw(12) << result.neuronCount
                      << std::setw(12) << result.synapseCount
                      << std::setw(12) << result.spikeCount << std::endl;
            
            if (!result.passed && verbose && !result.message.empty()) {
                std::cout << std::left << std::setw(62) << "  Reason: " << result.message << std::endl;
            }
        }
    }
    
    static void printTestSummary(const std::vector<TestResult>& results) {
        size_t totalTests = results.size();
        size_t passedTests = 0;
        size_t failedTests = 0;
        double totalExecutionTime = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
            totalExecutionTime += result.executionTime;
        }
        
        std::cout << "" << std::endl;
        std::cout << "=" << std::string(70, '=') << std::endl;
        std::cout << "                              FINAL SUMMARY" << std::endl;
        std::cout << "=" << std::string(70, '=') << std::endl;
        std::cout << "" << std::endl;
        
        std::cout << std::left << std::setw(30) << "Total Tests:" << totalTests << std::endl;
        std::cout << std::left << std::setw(30) << "Passed Tests:" << passedTests << std::endl;
        std::cout << std::left << std::setw(30) << "Failed Tests:" << failedTests << std::endl;
        std::cout << std::left << std::setw(30) << "Success Rate:" 
                  << std::fixed << std::setprecision(2) 
                  << ((double)passedTests / totalTests * 100.0) << "%" << std::endl;
        std::cout << std::left << std::setw(30) << "Total Execution Time:" 
                  << std::fixed << std::setprecision(4) << totalExecutionTime << " seconds" << std::endl;
        
        if (failedTests > 0) {
            std::cout << "" << std::endl;
            std::cout << "WARNING: " << failedTests << " test(s) failed!" << std::endl;
            std::cout << "Please review the test details above for more information." << std::endl;
        } else {
            std::cout << "" << std::endl;
            std::cout << "SUCCESS: All tests passed!" << std::endl;
        }
    }
    
    static std::string generateTestReport(const std::vector<TestResult>& results) {
        std::stringstream ss;
        ss << "NLM Test Suite Report" << std::endl;
        ss << "=====================" << std::endl << std::endl;
        
        ss << "Test Results Summary:" << std::endl;
        for (const auto& result : results) {
            ss << "  " << result.name << ": " 
               << (result.passed ? "PASS" : "FAIL") 
               << " (" << std::fixed << std::setprecision(4) << result.executionTime << "s)" << std::endl;
        }
        
        return ss.str();
    }
};

// Test utilities for common testing patterns
class TestUtilities {
public:
    template<typename T>
    static void assertEqual(const T& actual, const T& expected, const std::string& testName) {
        if (actual != expected) {
            throw std::runtime_error("Test \"" + testName + "\": expected " + 
                                    std::to_string(expected) + ", got " + std::to_string(actual));
        }
    }
    
    template<typename T>
    static void assertGreaterThan(const T& actual, const T& expected, const std::string& testName) {
        if (actual <= expected) {
            throw std::runtime_error("Test \"" + testName + "\": expected > " + 
                                    std::to_string(expected) + ", got " + std::to_string(actual));
        }
    }
    
    template<typename T>
    static void assertLessThan(const T& actual, const T& expected, const std::string& testName) {
        if (actual >= expected) {
            throw std::runtime_error("Test \"" + testName + "\": expected < " + 
                                    std::to_string(expected) + ", got " + std::to_string(actual));
        }
    }
    
    static void assertTrue(bool condition, const std::string& testName) {
        if (!condition) {
            throw std::runtime_error("Test \"" + testName + "\": assertion failed");
        }
    }
    
    static void assertFalse(bool condition, const std::string& testName) {
        if (condition) {
            throw std::runtime_error("Test \"" + testName + "\": assertion failed");
        }
    }
    
    static bool isApproximatelyEqual(double a, double b, double epsilon = 1e-6) {
        return std::abs(a - b) <= epsilon;
    }
    
    template<typename T>
    static void assertApproximatelyEqual(const T& actual, const T& expected, 
                                         double epsilon, const std::string& testName) {
        if (!isApproximatelyEqual(actual, expected, epsilon)) {
            throw std::runtime_error("Test \"" + testName + "\": expected approximately " + 
                                    std::to_string(expected) + ", got " + std::to_string(actual));
        }
    }
};

} // namespace test
} // namespace nlm

#endif // NLM_TEST_FRAMEWORK_H
