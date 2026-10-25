#include "TestFramework.h"
#include "Brain.hpp"
#include "../config/AdvancedConfig.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <random>

namespace nlm {

// TestRunner implementation
inline TestRunner::TestRunner() : verboseMode_(false), parallelMode_(false), maxRetries_(1),
    totalTests_(0), passedTests_(0), failedTests_(0) {
    setupSuites();
}

inline TestRunner::~TestRunner() {
    for (auto& suite : suites_) {
        suite->teardown();
    }
}

inline void TestRunner::registerSuite(std::unique_ptr<TestSuite> suite) {
    if (suite && suite->canRun()) {
        suites_.push_back(std::move(suite));
    }
}

inline bool TestRunner::runAllTests() {
    bool allPassed = true;
    
    // Setup all suites
    for (auto& suite : suites_) {
        if (!suite->setup()) {
            NLM_LOG_ERROR(std::string("Failed to setup suite: ") + suite->getName());
            allPassed = false;
        }
    }
    
    // Run each suite
    for (auto& suite : suites_) {
        NLM_LOG_INFO(std::string("Running test suite: ") + suite->getName());
        
        if (!runTestSuite(*suite)) {
            allPassed = false;
        }
    }
    
    // Teardown all suites
    for (auto& suite : suites_) {
        suite->teardown();
    }
    
    return allPassed;
}

inline bool TestRunner::runSuite(const std::string& suiteName) {
    for (auto& suite : suites_) {
        if (suite->getName() == suiteName) {
            return runTestSuite(*suite);
        }
    }
    
    NLM_LOG_ERROR(std::string("Test suite not found: ") + suiteName);
    return false;
}

inline bool TestRunner::runTest(const std::string& suiteName, const std::string& testName) {
    for (auto& suite : suites_) {
        if (suite->getName() == suiteName) {
            for (auto& test : suite->getTests()) {
                if (test->getName() == testName) {
                    TestResult result = executeTestWithRetry(*test, *suite);
                    allResults_.push_back(result);
                    updateStatistics(result);
                    
                    if (verboseMode_) {
                        NLM_LOG_INFO(result.toString());
                    }
                    
                    return result.passed;
                }
            }
            break;
        }
    }
    
    NLM_LOG_ERROR(std::string("Test not found: ") + testName + " in suite " + suiteName);
    return false;
}

inline std::vector<TestResult> TestRunner::getResultsForSuite(const std::string& suiteName) const {
    std::vector<TestResult> suiteResults;
    for (const auto& result : allResults_) {
        if (result.testSuite == suiteName) {
            suiteResults.push_back(result);
        }
    }
    return suiteResults;
}

inline std::vector<TestResult> TestRunner::getFailedTests() const {
    std::vector<TestResult> failed;
    for (const auto& result : allResults_) {
        if (!result.passed) {
            failed.push_back(result);
        }
    }
    return failed;
}

inline std::vector<TestResult> TestRunner::getPassedTests() const {
    std::vector<TestResult> passed;
    for (const auto& result : allResults_) {
        if (result.passed) {
            passed.push_back(result);
        }
    }
    return passed;
}

inline std::string TestRunner::generateReport() const {
    std::ostringstream oss;
    oss << "NLM Test Results" << std::endl;
    oss << "================" << std::endl << std::endl;
    
    oss << "Total Tests: " << totalTests_ << std::endl;
    oss << "Passed: " << passedTests_ << std::endl;
    oss << "Failed: " << failedTests_ << std::endl;
    oss << "Success Rate: " << (getSuccessRate() * 100) << "%" << std::endl;
    oss << std::endl;
    
    if (failedTests_ > 0) {
        oss << "Failed Tests:" << std::endl;
        for (const auto& result : getFailedTests()) {
            oss << "  - " << result.testSuite << "." << result.testName << ": "
                << result.message << std::endl;
        }
    }
    
    return oss.str();
}

inline std::string TestRunner::generateJSONReport() const {
    std::ostringstream oss;
    oss << "{\"tests\": [";
    
    for (size_t i = 0; i < allResults_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << allResults_[i].toJSON();
    }
    
    oss << "], \"summary\": {\"total\": " << totalTests_
        << ", \"passed\": " << passedTests_
        << ", \"failed\": " << failedTests_
        << ", \"successRate\": " << getSuccessRate() << "}}";
    
    return oss.str();
}

inline std::string TestRunner::generateJUnitReport() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    oss << "<testsuites tests=\"" << totalTests_ << "\" failures=\"" << failedTests_
        << "\" time=\"0.0\" timestamp=\"" << std::chrono::system_clock::now().time_since_epoch().count()
        << "\">" << std::endl;
    
    oss << "  <testsuite name=\"NLM Tests\" tests=\"" << totalTests_
        << "\" failures=\"" << failedTests_ << "\" time=\"0.0\">" << std::endl;
    
    for (const auto& result : allResults_) {
        oss << "    <testcase name=\"" << result.testSuite << "." << result.testName
            << "\" time=\"" << result.executionTime.count() << "\"";
        if (!result.passed) {
            oss << ">" << std::endl;
            oss << "      <failure message=\"" << result.message << "\" />" << std::endl;
        }
        oss << " />" << std::endl;
    }
    
    oss << "  </testsuite>" << std::endl;
    oss << "</testsuites>";
    
    return oss.str();
}

inline bool TestRunner::exportResults(const std::string& filepath, const std::string& format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string("Failed to open results file: ") + filepath);
        return false;
    }
    
    std::string content;
    if (format == "json") {
        content = generateJSONReport();
    } else if (format == "junit") {
        content = generateJUnitReport();
    } else {
        content = generateReport();
    }
    
    file << content;
    file.close();
    
    NLM_LOG_INFO(std::string("Test results exported to: ") + filepath + " (" + format + ")");
    return true;
}

inline void TestRunner::updateStatistics(const TestResult& result) {
    totalTests_++;
    if (result.passed) {
        passedTests_++;
    } else {
        failedTests_++;
    }
}

// Test utilities implementation
inline std::vector<float> TestUtils::generateRandomPattern(size_t size, float min, float max) {
    std::vector<float> pattern(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    
    for (size_t i = 0; i < size; ++i) {
        pattern[i] = dist(gen);
    }
    
    return pattern;
}

inline std::vector<float> TestUtils::generateNoisyPattern(const std::vector<float>& pattern, float noiseLevel) {
    std::vector<float> noisyPattern = pattern;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-noiseLevel, noiseLevel);
    
    for (size_t i = 0; i < noisyPattern.size(); ++i) {
        noisyPattern[i] += dist(gen);
        noisyPattern[i] = std::max(0.0f, std::min(1.0f, noisyPattern[i]));
    }
    
    return noisyPattern;
}

inline double TestUtils::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

inline double TestUtils::calculateStandardDeviation(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    double mean = calculateMean(values);
    double sum = 0.0;
    
    for (double value : values) {
        sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum / (values.size() - 1));
}

inline double TestUtils::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double meanX = calculateMean(x);
    double meanY = calculateMean(y);
    double covariance = 0.0;
    double stdDevX = calculateStandardDeviation(x);
    double stdDevY = calculateStandardDeviation(y);
    
    if (stdDevX == 0.0 || stdDevY == 0.0) return 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        covariance += (x[i] - meanX) * (y[i] - meanY);
    }
    
    return covariance / ((x.size() - 1) * stdDevX * stdDevY);
}

inline std::shared_ptr<Brain> TestUtils::createTestBrain(size_t neuronCount, size_t regionCount) {
    auto config = std::make_shared<Config>();
    config->set<size_t>("neuron_count", neuronCount);
    config->set<size_t>("region_count", regionCount);
    config->set<float>("simulation_timestep", 0.001f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to create test brain");
        return nullptr;
    }
    
    return brain;
}

inline std::shared_ptr<Config> TestUtils::createTestConfig() {
    auto config = std::make_shared<Config>();
    config->set<size_t>("neuron_count", 100);
    config->set<size_t>("region_count", 1);
    config->set<float>("simulation_timestep", 0.001f);
    config->set<float>("connection_probability", 0.1f);
    return config;
}

inline std::shared_ptr<AdvancedConfig> TestUtils::createAdvancedTestConfig() {
    return std::make_shared<AdvancedConfig>(AdvancedConfig::getDefaultConfig());
}

inline bool TestUtils::vectorsAreEqual(const std::vector<float>& a, const std::vector<float>& b,
                                     float tolerance) {
    if (a.size() != b.size()) return false;
    
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > tolerance) return false;
    }
    
    return true;
}

inline bool TestUtils::matricesAreEqual(const std::vector<std::vector<float>>& a,
                                      const std::vector<std::vector<float>>& b,
                                      float tolerance) {
    if (a.size() != b.size()) return false;
    
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].size() != b[i].size()) return false;
        if (!vectorsAreEqual(a[i], b[i], tolerance)) return false;
    }
    
    return true;
}

inline double TestUtils::measureExecutionTime(const std::function<void()>& func) {
    auto startTime = std::chrono::high_resolution_clock::now();
    func();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    return std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
}

inline double TestUtils::measureMemoryUsage(const std::function<void()>& func) {
    // Simplified memory measurement
    return 0.0;
}

inline void TestUtils::logTestStart(const std::string& testName, const std::string& suiteName) {
    NLM_LOG_INFO(std::string("Starting test: ") + testName + " (" + suiteName + ")");
}

inline void TestUtils::logTestResult(const std::string& testName, bool passed, const std::string& message) {
    if (passed) {
        NLM_LOG_INFO(std::string("Test passed: ") + testName + " - " + message);
    } else {
        NLM_LOG_ERROR(std::string("Test failed: ") + testName + " - " + message);
    }
}

inline void TestUtils::logTestEnd(const std::string& testName, const std::chrono::duration<double>& duration) {
    NLM_LOG_INFO(std::string("Test completed: ") + testName + " (" + 
        std::to_string(duration.count()) + "s)");
}

} // namespace nlm