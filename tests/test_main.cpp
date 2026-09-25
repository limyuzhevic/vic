// Comprehensive Test Runner for NLM Phase 6 Integrated Brain System
// Uses modern C++17 features for detailed testing, benchmarking, and error handling

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <memory>
#include <sstream>

// Forward declarations for all test modules
namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

namespace test_brain_memory {
    void runAll();
}

namespace test_phase6_integration {
    void runAll();
}

namespace test_benchmark {
    void runAll();
}

namespace test_error_handling {
    void runAll();
}

namespace test_documentation {
    void runAll();
}

// Modern test framework with detailed reporting
struct TestResult {
    std::string name;
    std::chrono::duration<double> duration;
    bool passed;
    std::string message;
    
    TestResult(const std::string& testName, bool testPassed, 
               const std::chrono::duration<double>& time, 
               const std::string& testMessage = "")
        : name(testName), duration(time), passed(testPassed), message(testMessage) {}
};

struct TestCategory {
    std::string name;
    std::string description;
    std::vector<TestResult> results;
    bool passedAll() const {
        return std::all_of(results.begin(), results.end(), 
                          [](const TestResult& r) { return r.passed; });
    }
    size_t totalCount() const { return results.size(); }
    size_t passedCount() const { 
        return std::count_if(results.begin(), results.end(),
                           [](const TestResult& r) { return r.passed; });
    }
    double getTotalTime() const {
        double total = 0.0;
        for (const auto& r : results) total += r.duration.count();
        return total;
    }
};

class ModernTestRunner {
private:
    std::vector<TestCategory> categories;
    std::vector<TestResult> individualTests;
    
    void printHeader(const std::string& title) const {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void printTestResult(const TestResult& result) const {
        std::cout << "    " << (result.passed ? "✓ PASS" : "✗ FAIL")
                  << " | " << std::left << std::setw(50) << result.name
                  << " | " << std::fixed << std::setprecision(4) 
                  << result.duration.count() << "s";
        if (!result.message.empty()) {
            std::cout << " | " << result.message;
        }
        std::cout << std::endl;
    }
    
    void printCategorySummary(const TestCategory& category) const {
        std::cout << std::endl;
        std::cout << "    Category: " << category.name << std::endl;
        std::cout << "    Description: " << category.description << std::endl;
        std::cout << "    Status: " << category.passedCount() << "/" 
                  << category.totalCount() << " tests passed"
                  << " (" << std::fixed << std::setprecision(1)
                  << (category.totalCount() > 0 ? 
                      (100.0 * category.passedCount() / category.totalCount()) : 0.0)
                  << "%)" << std::endl;
        std::cout << "    Total Time: " << std::fixed << std::setprecision(4)
                  << category.getTotalTime() << "s" << std::endl;
    }
    
    void printFinalSummary() const {
        printHeader("FINAL TEST SUMMARY");
        
        size_t totalTests = 0;
        size_t totalPassed = 0;
        double totalTime = 0.0;
        
        for (const auto& category : categories) {
            std::cout << std::endl;
            std::cout << "  Category: " << category.name << std::endl;
            std::cout << "    Tests: " << category.passedCount() << "/" 
                      << category.totalCount() << " passed" << std::endl;
            totalTests += category.totalCount();
            totalPassed += category.passedCount();
            totalTime += category.getTotalTime();
        }
        
        std::cout << std::endl;
        std::cout << "  Overall: " << totalPassed << "/" << totalTests 
                  << " tests passed" << std::endl;
        if (totalTests > 0) {
            std::cout << "  Success Rate: " << std::fixed << std::setprecision(1)
                      << (100.0 * totalPassed / totalTests) << "%" << std::endl;
        }
        std::cout << "  Total Time: " << std::fixed << std::setprecision(4)
                  << totalTime << "s" << std::endl;
        
        std::cout << std::endl;
        if (totalPassed == totalTests && totalTests > 0) {
            std::cout << "  ✓ ALL TESTS PASSED!" << std::endl;
        } else {
            std::cout << "  ✗ SOME TESTS FAILED!" << std::endl;
        }
    }
    
public:
    void addTestCategory(const std::string& name, const std::string& description) {
        categories.push_back({name, description, std::vector<TestResult>()});
    }
    
    void addTestResult(const std::string& categoryName, 
                      const std::string& testName, 
                      bool passed, 
                      const std::chrono::duration<double>& duration,
                      const std::string& message = "") {
        individualTests.emplace_back(testName, passed, duration, message);
        
        // Find or create category
        for (auto& cat : categories) {
            if (cat.name == categoryName) {
                cat.results.emplace_back(testName, passed, duration, message);
                return;
            }
        }
        
        // Category not found, create it
        categories.push_back({categoryName, "", std::vector<TestResult>()});
        categories.back().results.emplace_back(testName, passed, duration, message);
    }
    
    void runAllTests() {
        std::cout << "\n" << std::string(80, '═') << std::endl;
        std::cout << "  NLM PHASE 6 INTEGRATED BRAIN SYSTEM - COMPREHENSIVE TEST RUNNER" << std::endl;
        std::cout << "  Using Modern C++17 with Enhanced Reporting and Benchmarking" << std::endl;
        std::cout << std::string(80, '═') << std::endl;
        
        // Phase 2 Tests (Existing - Backward Compatibility)
        printHeader("PHASE 2 TESTS - Basic Neural Computation");
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            test_types::runAll();
        } catch (const std::exception& e) {
            std::cerr << "ERROR in test_types: " << e.what() << std::endl;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start);
        std::cout << "Phase 2 Tests completed in " << std::fixed 
                  << std::setprecision(4) << duration.count() << "s" << std::endl;
        
        // Phase 6 Integration Tests
        printHeader("PHASE 6 INTEGRATION TESTS");
        start = std::chrono::high_resolution_clock::now();
        
        try {
            test_phase6_integration::runAll();
        } catch (const std::exception& e) {
            std::cerr << "ERROR in test_phase6_integration: " << e.what() << std::endl;
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration<double>(end - start);
        std::cout << "Phase 6 Integration Tests completed in " << std::fixed 
                  << std::setprecision(4) << duration.count() << "s" << std::endl;
        
        // Enhanced Config Tests
        printHeader("ENHANCED CONFIG TESTS (Phase 6 Features)");
        start = std::chrono::high_resolution_clock::now();
        
        try {
            test_config::runAll();
        } catch (const std::exception& e) {
            std::cerr << "ERROR in test_config: " << e.what() << std::endl;
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration<double>(end - start);
        std::cout << "Enhanced Config Tests completed in " << std::fixed 
                  << std::setprecision(4) << duration.count() << "s" << std::endl;
        
        // Enhanced Brain Tests
        printHeader("ENHANCED BRAIN TESTS (Memory Management Fixes)");
        start = std::chrono::high_resolution_clock::now();
        
        try {
            test_brain::runAll();
            test_brain_memory::runAll();
        } catch (const std::exception& e) {
            std::cerr << "ERROR in brain tests: " << e.what() << std::endl;
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration<double>(end - start);
        std::cout << "Enhanced Brain Tests completed in " << std::fixed 
                  << std::setprecision(4) << duration.count() << "s" << std::endl;
        
        // Benchmark Tests
        printHeader("PERFORMANCE BENCHMARK TESTS");
        start = std::chrono::high_resolution_clock::now();
        
        try {
            test_benchmark::runAll();
        } catch (const std::exception& e) {
            std::cerr << "ERROR in benchmark tests: " << e.what() << std::endl;
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration<double>(end - start);
        std::cout << "Benchmark Tests completed in " << std::fixed 
                  << std::setprecision(4) << duration.count() << "s" << std::endl;
        
        // Error Handling Tests
        printHeader("ERROR HANDLING AND EDGE CASE TESTS");
        start = std::chrono::high_resolution_clock::now();
        
        try {
            test_error_handling::runAll();
        } catch (const std::exception& e) {
            std::cerr << "ERROR in error handling tests: " << e.what() << std::endl;
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration<double>(end - start);
        std::cout << "Error Handling Tests completed in " << std::fixed 
                  << std::setprecision(4) << duration.count() << "s" << std::endl;
        
        // Documentation and API Usage Tests
        printHeader("DOCUMENTATION AND API USAGE TESTS");
        start = std::chrono::high_resolution_clock::now();
        
        try {
            test_documentation::runAll();
        } catch (const std::exception& e) {
            std::cerr << "ERROR in documentation tests: " << e.what() << std::endl;
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration<double>(end - start);
        std::cout << "Documentation Tests completed in " << std::fixed 
                  << std::setprecision(4) << duration.count() << "s" << std::endl;
        
        // Print summary
        printFinalSummary();
    }
};

// Helper functions for timing tests
auto timeTest(const std::function<void()>& testFunc, const std::string& testName) -> TestResult {
    auto start = std::chrono::high_resolution_clock::now();
    bool passed = false;
    std::string message = "";
    
    try {
        testFunc();
        passed = true;
    } catch (const std::exception& e) {
        message = std::string("Exception: ") + e.what();
    } catch (...) {
        message = "Unknown exception";
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    
    std::cout << "    Running " << testName << "... ";
    
    return TestResult(testName, passed, duration, message);
}

int main() {
    std::cout << "\n" << std::string(80, '═') << std::endl;
    std::cout << "  INITIALIZING COMPREHENSIVE TEST RUNNER FOR NLM PHASE 6" << std::endl;
    std::cout << "  Modern C++17 Test Framework with Enhanced Features" << std::endl;
    std::cout << std::string(80, '═') << std::endl;
    
    ModernTestRunner runner;
    
    // Initialize all test categories
    runner.addTestCategory("Phase 2 Tests", 
        "Basic neural computation tests for backward compatibility");
    runner.addTestCategory("Phase 6 Integration", 
        "Integration tests for all Phase 6 systems");
    runner.addTestCategory("Enhanced Config", 
        "Enhanced configuration tests with JSON/YAML support");
    runner.addTestCategory("Enhanced Brain", 
        "Brain tests with memory management fixes and move semantics");
    runner.addTestCategory("Benchmarks", 
        "Performance benchmarks and scalability tests");
    runner.addTestCategory("Error Handling", 
        "Error handling and edge case tests");
    runner.addTestCategory("Documentation", 
        "API usage examples and documentation validation");
    
    // Run all tests
    runner.runAllTests();
    
    return 0;
}
