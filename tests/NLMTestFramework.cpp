#include "NLMTestFramework.hpp"
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace nlm {
namespace test {

void TestRunner::addTest(const std::string& name, const std::string& category,
                         std::function<void()> testFunction) {
    testCases.push_back({name, category, std::move(testFunction)});
}

void TestRunner::runAllTests() {
    clear();
    totalTests = testCases.size();
    
    std::cout << "Running NLM Test Suite..." << std::endl;
    std::cout << "Total tests: " << totalTests << std::endl;
    std::cout << std::endl;
    
    for (const auto& testCase : testCases) {
        std::cout << "Running " << testCase.category << ": " << testCase.name << "...";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            testCase.function();
            results.emplace_back(testCase.name, testCase.category, true);
            passedTests++;
            std::cout << " PASSED" << std::endl;
        } catch (const std::exception& e) {
            results.emplace_back(testCase.name, testCase.category, false, e.what());
            failedTests++;
            std::cout << " FAILED: " << e.what() << std::endl;
        } catch (...) {
            results.emplace_back(testCase.name, testCase.category, false, "Unknown exception");
            failedTests++;
            std::cout << " FAILED: Unknown exception" << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        results.back().duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    }
    
    std::cout << std::endl;
    std::cout << "Test suite completed: " << passedTests << " passed, " 
              << failedTests << " failed" << std::endl;
}

void TestRunner::runCategory(const std::string& category) {
    clear();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (const auto& testCase : testCases) {
        if (testCase.category == category) {
            std::cout << "Running " << category << ": " << testCase.name << "...";
            
            auto testStartTime = std::chrono::high_resolution_clock::now();
            
            try {
                testCase.function();
                results.emplace_back(testCase.name, testCase.category, true);
                passedTests++;
                std::cout << " PASSED" << std::endl;
            } catch (const std::exception& e) {
                results.emplace_back(testCase.name, testCase.category, false, e.what());
                failedTests++;
                std::cout << " FAILED: " << e.what() << std::endl;
            } catch (...) {
                results.emplace_back(testCase.name, testCase.category, false, "Unknown exception");
                failedTests++;
                std::cout << " FAILED: Unknown exception" << std::endl;
            }
            
            auto testEndTime = std::chrono::high_resolution_clock::now();
            results.back().duration = std::chrono::duration_cast<std::chrono::microseconds>(testEndTime - testStartTime);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << std::endl;
    std::cout << "Category " << category << " completed: " << passedTests << " passed, " 
              << failedTests << " failed (total time: " << totalDuration.count() << " us)" << std::endl;
}

std::string TestRunner::generateReport() const {
    std::ostringstream oss;
    oss << "NLM Test Suite Report" << std::endl;
    oss << "=" << std::string(50, '=') << std::endl;
    oss << "Total Tests: " << totalTests << std::endl;
    oss << "Passed: " << passedTests << std::endl;
    oss << "Failed: " << failedTests << std::endl;
    oss << std::endl;
    
    // Group tests by category
    std::map<std::string, std::vector<const TestResult*>> categorizedResults;
    for (const auto& result : results) {
        categorizedResults[result.category].push_back(&result);
    }
    
    for (const auto& categoryPair : categorizedResults) {
        oss << categoryPair.first << ": " << categoryPair.second.size() << " tests" << std::endl;
        for (const auto* result : categoryPair.second) {
            std::string status = result->passed ? "PASS" : "FAIL";
            oss << "  " << status << " " << std::left << std::setw(30) << result->name;
            if (!result->message.empty()) {
                oss << " - " << result->message << std::endl;
            } else {
                oss << std::endl;
            }
        }
        oss << std::endl;
    }
    
    // Summary by duration
    oss << "Slowest Tests:" << std::endl;
    std::vector<const TestResult*> slowTests;
    for (const auto& result : results) {
        slowTests.push_back(&result);
    }
    
    std::sort(slowTests.begin(), slowTests.end(),
              [](const TestResult* a, const TestResult* b) {
                  return a->duration.count() > b->duration.count();
              });
    
    for (size_t i = 0; i < std::min(size_t(5), slowTests.size()); ++i) {
        const auto* result = slowTests[i];
        oss << "  " << std::setw(30) << result->name << " (" 
            << result->duration.count() << " us)" << std::endl;
    }
    
    return oss.str();
}

std::string TestRunner::generateJSONReport() const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"test_suite\": {" << std::endl;
    oss << "    \"total_tests\": " << totalTests << "," << std::endl;
    oss << "    \"passed_tests\": " << passedTests << "," << std::endl;
    oss << "    \"failed_tests\": " << failedTests << "," << std::endl;
    oss << "    \"success_rate\": " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "" << std::endl;
    oss << "  }," << std::endl;
    oss << "  \"tests\": [" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        oss << "    {" << std::endl;
        oss << "      \"name\": \"" << result.name << "\"," << std::endl;
        oss << "      \"category\": \"" << result.category << "\"," << std::endl;
        oss << "      \"passed\": " << (result.passed ? "true" : "false") << "," << std::endl;
        oss << "      \"message\": \"" << result.message << "\"," << std::endl;
        oss << "      \"duration_us\": " << result.duration.count() << std::endl;
        if (i < results.size() - 1) {
            oss << "    }," << std::endl;
        } else {
            oss << "    }" << std::endl;
        }
    }
    
    oss << "  ]" << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

void assertTest(bool condition, const std::string& description,
                const std::string& file, int line) {
    if (!condition) {
        throw std::runtime_error("Assertion failed: " + description + " at " + file + ":" + std::to_string(line));
    }
}

void assertEqual(double actual, double expected,
                  const std::string& actualDesc, const std::string& expectedDesc,
                  const std::string& file, int line) {
    if (std::abs(actual - expected) > 1e-9) {
        throw std::runtime_error("Equal assertion failed: " + actualDesc + " (" + std::to_string(actual) +
                                ") != " + expectedDesc + " (" + std::to_string(expected) +
                                ") at " + file + ":" + std::to_string(line));
    }
}

void assertNotEqual(double actual, double expected,
                     const std::string& actualDesc, const std::string& expectedDesc,
                     const std::string& file, int line) {
    if (std::abs(actual - expected) <= 1e-9) {
        throw std::runtime_error("Not equal assertion failed: " + actualDesc + " (" + std::to_string(actual) +
                                ") == " + expectedDesc + " (" + std::to_string(expected) +
                                ") at " + file + ":" + std::to_string(line));
    }
}

void assertGreaterThan(double actual, double expected,
                        const std::string& actualDesc, const std::string& expectedDesc,
                        const std::string& file, int line) {
    if (actual <= expected) {
        throw std::runtime_error("Greater than assertion failed: " + actualDesc + " (" + std::to_string(actual) +
                                ") <= " + expectedDesc + " (" + std::to_string(expected) +
                                ") at " + file + ":" + std::to_string(line));
    }
}

void assertGreaterOrEqual(double actual, double expected,
                           const std::string& actualDesc, const std::string& expectedDesc,
                           const std::string& file, int line) {
    if (actual < expected) {
        throw std::runtime_error("Greater or equal assertion failed: " + actualDesc + " (" + std::to_string(actual) +
                                ") < " + expectedDesc + " (" + std::to_string(expected) +
                                ") at " + file + ":" + std::to_string(line));
    }
}

void assertLessThan(double actual, double expected,
                     const std::string& actualDesc, const std::string& expectedDesc,
                     const std::string& file, int line) {
    if (actual >= expected) {
        throw std::runtime_error("Less than assertion failed: " + actualDesc + " (" + std::to_string(actual) +
                                ") >= " + expectedDesc + " (" + std::to_string(expected) +
                                ") at " + file + ":" + std::to_string(line));
    }
}

void assertLessOrEqual(double actual, double expected,
                        const std::string& actualDesc, const std::string& expectedDesc,
                        const std::string& file, int line) {
    if (actual > expected) {
        throw std::runtime_error("Less or equal assertion failed: " + actualDesc + " (" + std::to_string(actual) +
                                ") > " + expectedDesc + " (" + std::to_string(expected) +
                                ") at " + file + ":" + std::to_string(line));
    }
}

void assertThrows(const std::function<void()>& expression, const std::string& exceptionType,
                  const std::string& file, int line) {
    try {
        expression();
        throw std::runtime_error("Expected exception " + exceptionType + " was not thrown at " + file + ":" + std::to_string(line));
    } catch (const std::exception& e) {
        // Exception thrown as expected
    } catch (...) {
        // Exception thrown, type doesn't matter for basic test
    }
}

void TestFixture::runTest(const std::string& testName, std::function<void()> testMethod) {
    setUp();
    try {
        testMethod();
        std::cout << "  PASS: " << testName << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAIL: " << testName << " - " << e.what() << std::endl;
        throw;
    } catch (...) {
        std::cout << "  FAIL: " << testName << " - Unknown exception" << std::endl;
        throw;
    }
    tearDown();
}

void IntegrationTestFixture::setUp() {
    // Initialize integration test environment
    // This could include setting up neural networks, loading models, etc.
}

void IntegrationTestFixture::tearDown() {
    // Clean up integration test environment
    // This could include freeing memory, closing connections, etc.
}

} // namespace test
} // namespace nlm
