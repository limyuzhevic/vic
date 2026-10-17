// TestFramework.hpp - Simplified test framework for NLM testing
#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// Simple test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string error;
    double executionTime;
    
    TestResult(const std::string& testName, bool success, const std::string& err = "", double time = 0.0)
        : name(testName), passed(success), error(err), executionTime(time) {}
};

// Simple test runner
class TestRunner {
private:
    std::vector<TestResult> results;
    bool verbose;
    
public:
    TestRunner(bool verboseOutput = true) : verbose(verboseOutput) {}
    
    template<typename Func>
    void addTest(const std::string& name, Func testFunc) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = false;
        std::string error = "";
        
        try {
            testFunc();
            passed = true;
        } catch (const std::exception& e) {
            error = std::string("Exception: ") + e.what();
        } catch (...) {
            error = "Unknown error";
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        results.emplace_back(name, passed, error, executionTime);
        
        if (verbose) {
            std::cout << (passed ? "  PASSED" : "  FAILED") << " (" << executionTime << "s)" << std::endl;
        }
    }
    
    void runAll(const std::string& testName) {
        std::cout << "Running " << testName << "..." << std::endl;
        
        for (const auto& result : results) {
            if (verbose) {
                std::cout << "  " << result.name;
                if (!result.error.empty()) {
                    std::cout << " - " << result.error;
                }
                std::cout << std::endl;
            }
        }
        
        if (verbose) {
            std::cout << std::endl;
        }
        
        // Summary
        int passed = 0, failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
            totalTime += result.executionTime;
        }
        
        std::cout << testName << " Summary:" << std::endl;
        std::cout << "  Passed: " << passed << "/" << (passed + failed) << std::endl;
        std::cout << "  Failed: " << failed << std::endl;
        std::cout << "  Total time: " << totalTime << "s" << std::endl;
        
        if (failed > 0) {
            std::cout << std::endl << "Failed tests:" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << ": " << result.error << std::endl;
                }
            }
        }
    }
    
    void clear() {
        results.clear();
    }
};

// Simple test assertion
#define ASSERT_TRUE(condition, msg) assert((condition) && (msg))
#define ASSERT_FALSE(condition, msg) assert(!(condition) && (msg))
#define ASSERT_EQ(expected, actual, msg) assert((expected) == (actual) && (msg))
#define ASSERT_FLOAT_EQ(expected, actual, tolerance, msg) assert(std::abs((expected) - (actual)) <= (tolerance) && (msg))

} // namespace nlm
