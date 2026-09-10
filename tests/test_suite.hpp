// NLM Test Suite
// Comprehensive testing framework for the Neural Learning Machine

#ifndef NLM_TEST_SUITE_HPP
#define NLM_TEST_SUITE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <memory>
#include <functional>
#include <limits>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <stdexcept>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <cstring>
#include <cstdint>

// Simple test framework for NLM
namespace test {
    
    class TestResult {
    public:
        std::string name;
        bool passed;
        std::string message;
        
        TestResult(const std::string& testName, bool isPassed, const std::string& msg = "")
            : name(testName), passed(isPassed), message(msg) {}
    };
    
    class TestSuite {
    public:
        std::string name;
        std::vector<TestResult> results;
        
        TestSuite(const std::string& suiteName) : name(suiteName) {}
        
        void addResult(const TestResult& result) {
            results.push_back(result);
        }
        
        size_t passedCount() const {
            return std::count_if(results.begin(), results.end(),
                [](const TestResult& r) { return r.passed; });
        }
        
        size_t totalCount() const {
            return results.size();
        }
        
        double passRate() const {
            return totalCount() > 0 ? static_cast<double>(passedCount()) / totalCount() : 0.0;
        }
        
        void printResults(std::ostream& out) const {
            out << "Test Suite: " << name << std::endl;
            out << "Passed: " << passedCount() << "/" << totalCount()
                << " (" << (passRate() * 100.0) << "%)" << std::endl;
                
            for (const auto& result : results) {
                out << "  [" << (result.passed ? "PASS" : "FAIL") << "] " << result.name;
                if (!result.message.empty()) {
                    out << " - " << result.message;
                }
                out << std::endl;
            }
        }
    };
    
    class TestRunner {
    public:
        std::vector<TestSuite> suites;
        
        void addSuite(const TestSuite& suite) {
            suites.push_back(suite);
        }
        
        void runAll() {
            std::cout << "Running NLM Tests..." << std::endl << std::endl;
            
            size_t totalPassed = 0;
            size_t totalTests = 0;
            
            for (auto& suite : suites) {
                suite.printResults(std::cout);
                std::cout << std::endl;
                
                totalPassed += suite.passedCount();
                totalTests += suite.totalCount();
            }
            
            std::cout << "=== Summary ===" << std::endl;
            std::cout << "Total Tests: " << totalTests << std::endl;
            std::cout << "Passed: " << totalPassed << std::endl;
            std::cout << "Failed: " << (totalTests - totalPassed) << std::endl;
            std::cout << "Success Rate: " << (totalTests > 0 ? 
                (static_cast<double>(totalPassed) / totalTests) * 100.0 : 0.0) << "%" << std::endl;
            
            if (totalPassed == totalTests) {
                std::cout << std::endl << "All tests passed! ✓" << std::endl;
            } else {
                std::cout << std::endl << "Some tests failed. ✗" << std::endl;
            }
        }
    };
} // namespace test

#endif // NLM_TEST_SUITE_HPP
