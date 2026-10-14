// Main Enhanced Test Driver
// Enhanced test runner with modern testing framework integration

#include "nlm_testing_framework.hpp"
#include "nlm_test_config.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <map>

namespace {
    // Test result collector
    class TestResultCollector {
    private:
        std::vector<nlm::TestResult> results;
        std::map<std::string, int> categoryCounts;
        double totalExecutionTime;
        int totalTests;
        int passedTests;
        int failedTests;
        int skippedTests;
        
    public:
        TestResultCollector() : totalExecutionTime(0.0), totalTests(0), 
                               passedTests(0), failedTests(0), skippedTests(0) {}
        
        void addResult(const nlm::TestResult& result) {
            results.push_back(result);
            totalTests++;
            totalExecutionTime += result.executionTime;
            
            categoryCounts[result.category]++;
            
            if (result.status == "PASSED") {
                passedTests++;
            } else if (result.status == "FAILED") {
                failedTests++;
            } else if (result.status == "SKIPPED") {
                skippedTests++;
            }
        }
        
        void printSummary() const {
            std::cout << "\n=== Test Execution Summary ===" << std::endl;
            std::cout << "Total Tests: " << totalTests << std::endl;
            std::cout << "Passed: " << passedTests << std::endl;
            std::cout << "Failed: " << failedTests << std::endl;
            std::cout << "Skipped: " << skippedTests << std::endl;
            std::cout << "Total Execution Time: " << std::fixed << std::setprecision(3)
                      << totalExecutionTime << " seconds" << std::endl;
            
            if (totalTests > 0) {
                double passRate = static_cast<double>(passedTests) / totalTests * 100.0;
                std::cout << "Pass Rate: " << std::fixed << std::setprecision(2) << passRate << "%" << std::endl;
            }
            
            std::cout << "\nCategory Breakdown:" << std::endl;
            for (const auto& pair : categoryCounts) {
                std::cout << "  " << pair.first << ": " << pair.second << " tests" << std::endl;
            }
        }
        
        void exportResults(const std::string& filename) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot create results file: " << filename << std::endl;
                return;
            }
            
            file << "NLM Test Results" << std::endl;
            file << "===============" << std::endl;
            file << "Generated: " << __DATE__ << " " << __TIME__ << std::endl;
            file << std::endl;
            
            file << "Test Results:" << std::endl;
            file << "-------------" << std::endl;
            
            for (const auto& result : results) {
                file << "Test: " << result.testName << std::endl;
                file << "Category: " << result.category << std::endl;
                file << "Status: " << result.status << std::endl;
                file << "Execution Time: " << std::fixed << std::setprecision(6) 
                      << result.executionTime << " seconds" << std::endl;
                if (!result.message.empty()) {
                    file << "Message: " << result.message << std::endl;
                }
                file << "---" << std::endl;
            }
            
            file << "\nSummary:" << std::endl;
            file << "-------" << std::endl;
            file << "Total Tests: " << totalTests << std::endl;
            file << "Passed: " << passedTests << std::endl;
            file << "Failed: " << failedTests << std::endl;
            file << "Skipped: " << skippedTests << std::endl;
            
            file.close();
            
            std::cout << "Test results exported to: " << filename << std::endl;
        }
    };
    
    // Test timer utility
    class TestTimer {
    private:
        std::chrono::high_resolution_clock::time_point startTime;
        
    public:
        void start() {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        double stop() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            return duration.count() / 1e6; // Convert to seconds
        }
    };
    
    // Test case wrapper for timing and result collection
    bool runTimedTest(const std::string& testName, const std::string& category,
                      std::function<bool()> testFunc) {
        TestTimer timer;
        timer.start();
        
        bool result;
        std::string errorMessage;
        
        try {
            result = testFunc();
        } catch (const std::exception& e) {
            result = false;
            errorMessage = std::string("Exception: ") + e.what();
        } catch (...) {
            result = false;
            errorMessage = "Unknown exception";
        }
        
        double executionTime = timer.stop();
        
        nlm::TestResult testResult(testName, category);
        testResult.status = result ? "PASSED" : "FAILED";
        testResult.message = errorMessage;
        testResult.executionTime = executionTime;
        
        // Add to global result collector
        static TestResultCollector collector;
        collector.addResult(testResult);
        
        return result;
    }
}

// Test namespace for integration
namespace test_integration {
    // Integration tests will be declared here
    // They are implemented in the respective test files
    
    bool testBrainMemoryIntegration();
    bool testNeuromodulationIntegration();
    bool testCognitionIntegration();
    bool testAgentIntegration();
}

// Test runner main function
int main(int argc, char* argv[]) {
    std::cout << "=== Enhanced NLM Test Suite ===" << std::endl;
    std::cout << "Phase 3: Modern Neural Computation Testing Framework" << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments
    std::string categoryFilter;
    std::string outputFile;
    bool enableVerbose = false;
    bool enableBenchmark = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--category" && i + 1 < argc) {
            categoryFilter = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--verbose") {
            enableVerbose = true;
        } else if (arg == "--benchmark") {
            enableBenchmark = true;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_test [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --category TYPE    Filter by test category (unit, integration, performance, configuration, error, cli, system)" << std::endl;
            std::cout << "  --output FILE      Export results to file" << std::endl;
            std::cout << "  --verbose          Enable verbose output" << std::endl;
            std::cout << "  --benchmark        Enable performance benchmarking" << std::endl;
            std::cout << "  --help             Show this help message" << std::endl;
            return 0;
        }
    }
    
    TestRegistry& registry = TestRegistry::getInstance();
    
    // Determine which tests to run
    std::vector<std::string> testSuites;
    
    if (!categoryFilter.empty()) {
        std::cout << "Running tests for category: " << categoryFilter << std::endl;
        // In a real implementation, we would filter by category
        // For now, we'll run all tests
    } else {
        std::cout << "Running all tests..." << std::endl;
    }
    
    // Run all tests
    registry.runAll();
    
    // Export results if requested
    if (!outputFile.empty()) {
        static TestResultCollector collector;
        collector.exportResults(outputFile);
    }
    
    // Print final summary
    static TestResultCollector collector;
    collector.printSummary();
    
    std::cout << "\n=== Test Run Complete ===" << std::endl;
    return 0;
}