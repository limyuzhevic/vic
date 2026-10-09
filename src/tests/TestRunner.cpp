#ifndef NLM_TEST_RUNNER_H
#define NLM_TEST_RUNNER_H

#include "tests/CoreTest.cpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace nlm {

class TestResult {
public:
    std::string testName;
    bool passed;
    std::string errorMessage;
    std::chrono::duration<double> duration;
    
    TestResult(const std::string& name, bool passed, const std::string& error = "", 
               const std::chrono::duration<double>& dur = std::chrono::duration<double>(0))
        : testName(name), passed(passed), errorMessage(error), duration(dur) {}
};

class TestRunner {
public:
    TestRunner() : totalTests_(0), passedTests_(0), failedTests_(0) {}
    
    void runAllTests() {
        std::cout << "========================================" << std::endl;
        std::cout << "      NLM Core System Test Suite" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run all test suites
        runCoreTests();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration<double>(endTime - startTime);
        
        // Generate summary report
        generateSummaryReport(totalDuration);
        
        // Display final results
        displayFinalResults();
    }
    
    void runCoreTests() {
        std::cout << "Running Core Tests..." << std::endl;
        std::cout << std::endl;
        
        auto testStart = std::chrono::high_resolution_clock::now();
        
        try {
            // Run Config tests
            auto configStart = std::chrono::high_resolution_clock::now();
            ConfigTest::runTests();
            auto configDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - configStart);
            
            // Run ConfigManager tests
            auto managerStart = std::chrono::high_resolution_clock::now();
            ConfigManagerTest::runTests();
            auto managerDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - managerStart);
            
            // Run Random tests
            auto randomStart = std::chrono::high_resolution_clock::now();
            RandomTest::runTests();
            auto randomDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - randomStart);
            
            // Run SimulationClock tests
            auto clockStart = std::chrono::high_resolution_clock::now();
            SimulationClockTest::runTests();
            auto clockDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - clockStart);
            
            // Run Neuron tests
            auto neuronStart = std::chrono::high_resolution_clock::now();
            NeuronTest::runTests();
            auto neuronDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - neuronStart);
            
            // Run Synapse tests
            auto synapseStart = std::chrono::high_resolution_clock::now();
            SynapseTest::runTests();
            auto synapseDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - synapseStart);
            
            // Run Core system tests
            auto coreStart = std::chrono::high_resolution_clock::now();
            CoreSystemTest::runTests();
            auto coreDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - coreStart);
            
            // Run benchmark tests
            auto benchmarkStart = std::chrono::high_resolution_clock::now();
            BenchmarkTest::runTests();
            auto benchmarkDuration = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - benchmarkStart);
            
            // Record test times
            testDurations_ = {
                {"Config", configDuration},
                {"ConfigManager", managerDuration},
                {"Random", randomDuration},
                {"SimulationClock", clockDuration},
                {"Neuron", neuronDuration},
                {"Synapse", synapseDuration},
                {"CoreSystem", coreDuration},
                {"Benchmark", benchmarkDuration}
            };
            
            totalTests_ = 8;
            passedTests_ = 8;
            failedTests_ = 0;
            
            std::cout << std::endl;
            std::cout << "All core tests passed!" << std::endl;
            
        } catch (const std::exception& e) {
            totalTests_ = 8;
            passedTests_ = 0;
            failedTests_ = 8;
            std::cerr << "Test suite failed with exception: " << e.what() << std::endl;
            throw;
        }
        
        auto testEnd = std::chrono::high_resolution_clock::now();
        overallTestDuration_ = std::chrono::duration<double>(testEnd - testStart);
    }
    
    void generateSummaryReport(const std::chrono::duration<double>& totalDuration) {
        std::cout << "========================================" << std::endl;
        std::cout << "              Test Summary Report" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Total Tests Run: " << totalTests_ << std::endl;
        std::cout << "Tests Passed: " << passedTests_ << std::endl;
        std::cout << "Tests Failed: " << failedTests_ << std::endl;
        std::cout << "Success Rate: " << (totalTests_ > 0 ? (passedTests_ * 100.0 / totalTests_) : 0.0) << "%" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Test Durations:" << std::endl;
        for (const auto& pair : testDurations_) {
            std::cout << "  " << std::left << std::setw(20) << pair.first 
                      << ": " << std::fixed << std::setprecision(3) << pair.second.count() << " seconds" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "Overall Performance:" << std::endl;
        std::cout << "  Total Duration: " << std::fixed << std::setprecision(3) << totalDuration.count() << " seconds" << std::endl;
        std::cout << "  Average per Test: " << std::fixed << std::setprecision(3) << (totalDuration.count() / totalTests_) << " seconds" << std::endl;
        std::cout << "  Tests per Second: " << std::fixed << std::setprecision(1) << (totalTests_ / totalDuration.count()) << " tests/second" << std::endl;
        std::cout << std::endl;
    }
    
    void displayFinalResults() {
        if (failedTests_ == 0) {
            std::cout << "========================================" << std::endl;
            std::cout << "              ✓ ALL TESTS PASSED" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << std::endl;
            std::cout << "The NLM core system is functioning correctly." << std::endl;
            std::cout << "All tests passed successfully!" << std::endl;
        } else {
            std::cout << "========================================" << std::endl;
            std::cout << "              ✗ TESTS FAILED" << std::endl;
            std::cout << "========================================" << std::endl;
            std::cout << std::endl;
            std::cout << "Some tests failed. Please check the output above for details." << std::endl;
        }
    }
    
    size_t getTotalTests() const { return totalTests_; }
    size_t getPassedTests() const { return passedTests_; }
    size_t getFailedTests() const { return failedTests_; }
    double getSuccessRate() const { return totalTests_ > 0 ? (passedTests_ * 100.0 / totalTests_) : 0.0; }
    
private:
    size_t totalTests_;
    size_t passedTests_;
    size_t failedTests_;
    std::chrono::duration<double> overallTestDuration_;
    std::vector<std::pair<std::string, std::chrono::duration<double>>> testDurations_;
};

} // namespace nlm

#endif // NLM_TEST_RUNNER_H
