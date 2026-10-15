// Advanced Test Framework Implementation
// Comprehensive testing infrastructure for NLM

#include "AdvancedTestFramework.hpp"
#include "core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>

namespace nlm {
namespace tests {

struct AdvancedTestFramework::Impl {
    // Test registry
    struct TestEntry {
        std::string name;
        std::string suite;
        std::function<bool()> testFunction;
        TestConfig config;
        bool registered;
        
        TestEntry(const std::string& n, const std::string& s, 
                  std::function<bool()> func, const TestConfig& c)
            : name(n), suite(s), testFunction(std::move(func)), config(c), registered(true) {}
    };
    
    std::vector<TestEntry> testRegistry;
    std::mutex registryMutex;
    
    // Test results
    std::vector<TestResult> testResults;
    std::mutex resultsMutex;
    
    // Framework state
    std::atomic<bool> running{false};
    std::atomic<bool> paused{false};
    std::condition_variable pauseCV;
    std::mutex pauseMutex;
    bool stopRequested{false};
    
    // Statistics
    size_t totalTests{0};
    size_t passedTests{0};
    size_t failedTests{0};
    std::chrono::milliseconds totalExecutionTime{0};
    
    // Performance monitoring
    bool performanceMonitoringEnabled{false};
    std::mutex metricsMutex;
    std::map<std::string, std::chrono::system_clock::time_point> metricStarts;
    std::map<std::string, std::vector<std::chrono::milliseconds>> metricValues;
    
    // Logger
    Logger* logger;
    
    Impl() {
        logger = Logger::getInstance();
    }
};

AdvancedTestFramework::AdvancedTestFramework() : pImpl(std::make_unique<Impl>()) {}
AdvancedTestFramework::~AdvancedTestFramework() {
    stop();
}

void AdvancedTestFramework::registerTest(const std::string& suite, const std::string& name,
                                       std::function<bool()> testFunc, const TestConfig& config) {
    if (!validateTestSetup(config)) {
        pImpl->logger->error("Invalid test configuration for: " + name);
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    // Check if test already exists
    for (const auto& entry : pImpl->testRegistry) {
        if (entry.suite == suite && entry.name == name) {
            pImpl->logger->warning("Test already registered: " + suite + "." + name);
            return;
        }
    }
    
    pImpl->testRegistry.emplace_back(name, suite, std::move(testFunc), config);
    pImpl->logger->info("Test registered: " + suite + "." + name);
}

void AdvancedTestFramework::unregisterTest(const std::string& suite, const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    auto it = std::remove_if(pImpl->testRegistry.begin(), pImpl->testRegistry.end(),
                            [&suite, &name](const TestEntry& entry) {
                                return entry.suite == suite && entry.name == name;
                            });
    
    if (it != pImpl->testRegistry.end()) {
        pImpl->testRegistry.erase(it, pImpl->testRegistry.end());
        pImpl->logger->info("Test unregistered: " + suite + "." + name);
    }
}

TestResult AdvancedTestFramework::executeTest(const std::string& suite, const std::string& name) {
    TestResult result(name, suite);
    result.maxExecutionTime = std::chrono::milliseconds(5000); // Default timeout
    
    // Find test
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    auto it = std::find_if(pImpl->testRegistry.begin(), pImpl->testRegistry.end(),
                          [&suite, &name](const TestEntry& entry) {
                              return entry.suite == suite && entry.name == name;
                          });
    
    if (it == pImpl->testRegistry.end()) {
        result.errorMessage = "Test not found: " + suite + "." + name;
        return result;
    }
    
    const TestEntry& testEntry = *it;
    
    // Check if test is enabled
    if (!testEntry.config.enabled) {
        result.errorMessage = "Test disabled: " + suite + "." + name;
        return result;
    }
    
    pImpl->logger->info("Executing test: " + suite + "." + name);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Execute test with timeout
    std::future<bool> testFuture;
    if (testEntry.config.parallelExecution) {
        // Execute in thread pool (simplified implementation)
        testFuture = std::async(std::launch::async, testEntry.testFunction);
        
        // Wait with timeout
        auto status = testFuture.wait_for(testEntry.config.timeout);
        if (status == std::future_status::timeout) {
            result.timeoutOccurred = true;
            result.errorMessage = "Test timed out after " + 
                std::to_string(std::chrono::duration_cast<std::chrono::seconds>(testEntry.config.timeout).count()) + " seconds";
            
            // Note: In real implementation, we would need to cancel the running test
            return result;
        }
        
        result.passed = testFuture.get();
    } else {
        // Execute synchronously
        try {
            result.passed = testEntry.testFunction();
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = "Exception in test: " + std::string(e.what());
        } catch (...) {
            result.passed = false;
            result.errorMessage = "Unknown exception in test";
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Update statistics
    {
        std::lock_guard<std::mutex> resultsLock(pImpl->resultsMutex);
        pImpl->testResults.push_back(result);
        pImpl->totalTests++;
        if (result.passed) {
            pImpl->passedTests++;
        } else {
            pImpl->failedTests++;
        }
        pImpl->totalExecutionTime += result.executionTime;
    }
    
    if (result.passed) {
        pImpl->logger->info("Test passed: " + suite + "." + name + 
                           " (" + std::to_string(result.executionTime.count()) + "ms)");
    } else {
        pImpl->logger->error("Test failed: " + suite + "." + name + 
                            " - " + result.errorMessage);
    }
    
    return result;
}

bool AdvancedTestFramework::runTestSuite(const std::string& suite, bool parallel) {
    if (!validateSuite(suite)) {
        pImpl->logger->error("Invalid test suite: " + suite);
        return false;
    }
    
    std::vector<std::string> testNames = getTestNamesInSuite(suite);
    bool allPassed = true;
    
    pImpl->logger->info("Running test suite: " + suite + " (" + std::to_string(testNames.size()) + " tests)");
    
    for (const auto& testName : testNames) {
        if (pImpl->paused) {
            std::unique_lock<std::mutex> lock(pImpl->pauseMutex);
            pImpl->pauseCV.wait(lock, [this]() { return !pImpl->paused || !pImpl->stopRequested; });
            if (pImpl->stopRequested) break;
        }
        
        TestResult result = executeTest(suite, testName);
        if (!result.passed) {
            allPassed = false;
        }
    }
    
    return allPassed;
}

bool AdvancedTestFramework::runAllTests(bool parallel, int maxThreads) {
    pImpl->running = true;
    pImpl->stopRequested = false;
    
    std::vector<std::string> suites = getTestSuites();
    bool allPassed = true;
    
    pImpl->logger->info("Running all tests across " + std::to_string(suites.size()) + " suites");
    
    for (const auto& suite : suites) {
        if (pImpl->stopRequested) break;
        
        if (!runTestSuite(suite, parallel)) {
            allPassed = false;
        }
    }
    
    pImpl->running = false;
    
    return allPassed;
}

std::vector<std::string> AdvancedTestFramework::getTestSuites() const {
    std::set<std::string> suites;
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    for (const auto& entry : pImpl->testRegistry) {
        suites.insert(entry.suite);
    }
    
    return std::vector<std::string>(suites.begin(), suites.end());
}

std::vector<std::string> AdvancedTestFramework::getTestNamesInSuite(const std::string& suite) const {
    std::vector<std::string> testNames;
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    for (const auto& entry : pImpl->testRegistry) {
        if (entry.suite == suite) {
            testNames.push_back(entry.name);
        }
    }
    
    return testNames;
}

std::vector<TestConfig> AdvancedTestFramework::getTestConfigs(const std::string& suite) const {
    std::vector<TestConfig> configs;
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    for (const auto& entry : pImpl->testRegistry) {
        if (entry.suite == suite) {
            configs.push_back(entry.config);
        }
    }
    
    return configs;
}

bool AdvancedTestFramework::validateTestSetup(const TestConfig& config) const {
    return !config.name.empty() && config.name.find('.') == std::string::npos;
}

bool AdvancedTestFramework::validateSuite(const std::string& suite) const {
    std::lock_guard<std::mutex> lock(pImpl->registryMutex);
    
    return std::any_of(pImpl->testRegistry.begin(), pImpl->testRegistry.end(),
                      [&suite](const TestEntry& entry) { return entry.suite == suite; });
}

std::vector<TestResult> AdvancedTestFramework::getAllResults() const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    return pImpl->testResults;
}

std::vector<TestResult> AdvancedTestFramework::getFailedResults() const {
    std::vector<TestResult> failed;
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    for (const auto& result : pImpl->testResults) {
        if (!result.passed) {
            failed.push_back(result);
        }
    }
    
    return failed;
}

std::vector<TestResult> AdvancedTestFramework::getPassedResults() const {
    std::vector<TestResult> passed;
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    for (const auto& result : pImpl->testResults) {
        if (result.passed) {
            passed.push_back(result);
        }
    }
    
    return passed;
}

std::vector<TestResult> AdvancedTestFramework::getResultsInSuite(const std::string& suite) const {
    std::vector<TestResult> suiteResults;
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    for (const auto& result : pImpl->testResults) {
        if (result.testSuite == suite) {
            suiteResults.push_back(result);
        }
    }
    
    return suiteResults;
}

double AdvancedTestFramework::getOverallPassRate() const {
    if (pImpl->totalTests == 0) return 0.0;
    return static_cast<double>(pImpl->passedTests) / pImpl->totalTests * 100.0;
}

size_t AdvancedTestFramework::getTotalTests() const {
    return pImpl->totalTests;
}

size_t AdvancedTestFramework::getPassedTests() const {
    return pImpl->passedTests;
}

size_t AdvancedTestFramework::getFailedTests() const {
    return pImpl->failedTests;
}

std::chrono::milliseconds AdvancedTestFramework::getTotalExecutionTime() const {
    return pImpl->totalExecutionTime;
}

std::string AdvancedTestFramework::generateSummaryReport() const {
    std::stringstream ss;
    ss << "NLM Test Summary Report\n";
    ss << "======================\n";
    ss << "Total Tests: " << pImpl->totalTests << "\n";
    ss << "Passed: " << pImpl->passedTests << "\n";
    ss << "Failed: " << pImpl->failedTests << "\n";
    ss << "Pass Rate: " << std::fixed << std::setprecision(2) 
       << getOverallPassRate() << "%\n";
    ss << "Total Execution Time: " 
       << std::chrono::duration_cast<std::chrono::seconds>(pImpl->totalExecutionTime).count() << " seconds\n";
    
    return ss.str();
}

std::string AdvancedTestFramework::generateDetailedReport() const {
    std::stringstream ss;
    ss << "NLM Detailed Test Report\n";
    ss << "========================\n";
    ss << "\n";
    
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    for (const auto& suite : getTestSuites()) {
        ss << "Suite: " << suite << "\n";
        auto suiteResults = getResultsInSuite(suite);
        
        for (const auto& result : suiteResults) {
            ss << "  Test: " << result.testName << "\n";
            ss << "    Status: " << (result.passed ? "PASSED" : "FAILED") << "\n";
            ss << "    Execution Time: " << result.executionTime.count() << "ms\n";
            if (!result.errorMessage.empty()) {
                ss << "    Error: " << result.errorMessage << "\n";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

bool AdvancedTestFramework::exportResults(const std::string& filename, const std::string& format) const {
    try {
        if (format == "json") {
            nlohmann::json j;
            
            // Convert test results to JSON
            j["total_tests"] = pImpl->totalTests;
            j["passed_tests"] = pImpl->passedTests;
            j["failed_tests"] = pImpl->failedTests;
            j["pass_rate"] = getOverallPassRate();
            
            std::vector<nlohmann::json> resultsJson;
            std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
            
            for (const auto& result : pImpl->testResults) {
                nlohmann::json testJson;
                testJson["test_name"] = result.testName;
                testJson["test_suite"] = result.testSuite;
                testJson["passed"] = result.passed;
                testJson["error_message"] = result.errorMessage;
                testJson["execution_time_ms"] = result.executionTime.count();
                testJson["max_execution_time_ms"] = result.maxExecutionTime.count();
                testJson["timeout_occurred"] = result.timeoutOccurred;
                
                resultsJson.push_back(testJson);
            }
            
            j["test_results"] = resultsJson;
            
            std::ofstream file(filename);
            if (!file.is_open()) {
                pImpl->logger->error("Failed to open output file: " + filename);
                return false;
            }
            
            file << j.dump(2);
            
        } else if (format == "xml") {
            // XML export implementation
            std::ofstream file(filename);
            if (!file.is_open()) {
                pImpl->logger->error("Failed to open output file: " + filename);
                return false;
            }
            
            file << "<test_results>\n";
            std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
            
            for (const auto& result : pImpl->testResults) {
                file << "  <test>\n";
                file << "    <name>" << result.testName << "</name>\n";
                file << "    <suite>" << result.testSuite << "</suite>\n";
                file << "    <passed>" << (result.passed ? "true" : "false") << "</passed>\n";
                if (!result.errorMessage.empty()) {
                    file << "    <error>" << result.errorMessage << "</error>\n";
                }
                file << "    <execution_time_ms>" << result.executionTime.count() << "</execution_time_ms>\n";
                file << "  </test>\n";
            }
            
            file << "</test_results>\n";
            
        } else if (format == "csv") {
            // CSV export implementation
            std::ofstream file(filename);
            if (!file.is_open()) {
                pImpl->logger->error("Failed to open output file: " + filename);
                return false;
            }
            
            file << "TestSuite,TestName,Passed,ErrorMessage,ExecutionTimeMs,TimeoutOccurred\n";
            std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
            
            for (const auto& result : pImpl->testResults) {
                // Escape commas and quotes in CSV
                std::string suite = result.testSuite;
                std::string name = result.testName;
                std::string error = result.errorMessage;
                
                // Simple CSV escaping
                auto escape = [](std::string& str) {
                    if (str.find(',') != std::string::npos) {
                        str = "\"" + str + "\"";
                    }
                };
                
                escape(suite);
                escape(name);
                escape(error);
                
                file << suite << "," << name << "," 
                     << (result.passed ? "true" : "false") << "," 
                     << "\"" << error << "\","
                     << result.executionTime.count() << "," 
                     << (result.timeoutOccurred ? "true" : "false") << "\n";
            }
        }
        
        pImpl->logger->info("Test results exported to: " + filename + " (format: " + format + ")");
        return true;
        
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to export results: " + std::string(e.what()));
        return false;
    }
}

void AdvancedTestFramework::enablePerformanceMonitoring(bool enable) {
    pImpl->performanceMonitoringEnabled = enable;
    pImpl->logger->info("Performance monitoring " + std::string(enable ? "enabled" : "disabled"));
}

void AdvancedTestFramework::startMeasurement(const std::string& metric) {
    if (!pImpl->performanceMonitoringEnabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->metricStarts[metric] = std::chrono::system_clock::now();
}

void AdvancedTestFramework::stopMeasurement(const std::string& metric) {
    if (!pImpl->performanceMonitoringEnabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto it = pImpl->metricStarts.find(metric);
    if (it != pImpl->metricStarts.end()) {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - it->second);
        
        pImpl->metricValues[metric].push_back(duration);
        pImpl->metricStarts.erase(it);
    }
}

double AdvancedTestFramework::getMeasurement(const std::string& metric) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto it = pImpl->metricValues.find(metric);
    if (it != pImpl->metricValues.end() && !it->second.empty()) {
        double sum = 0.0;
        for (const auto& duration : it->second) {
            sum += duration.count();
        }
        return sum / it->second.size();
    }
    
    return 0.0;
}

std::map<std::string, double> AdvancedTestFramework::getAllMeasurements() const {
    std::map<std::string, double> measurements;
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    for (const auto& pair : pImpl->metricValues) {
        measurements[pair.first] = getMeasurement(pair.first);
    }
    
    return measurements;
}

void AdvancedTestFramework::clearResults() {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->testResults.clear();
    pImpl->logger->info("Test results cleared");
}

void AdvancedTestFramework::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->totalTests = 0;
    pImpl->passedTests = 0;
    pImpl->failedTests = 0;
    pImpl->totalExecutionTime = std::chrono::milliseconds(0);
    pImpl->logger->info("Statistics reset");
}

void AdvancedTestFramework::runPluginTests() {
    pImpl->logger->info("Running plugin system tests...");
    runTestSuite("PluginSystem");
}

void AdvancedTestFramework::runCLITests() {
    pImpl->logger->info("Running CLI tests...");
    runTestSuite("CLI");
}

void AdvancedTestFramework::runIntegrationTests() {
    pImpl->logger->info("Running integration tests...");
    runTestSuite("Integration");
}

void AdvancedTestFramework::runStressTests() {
    pImpl->logger->info("Running stress tests...");
    runTestSuite("Stress");
}

void AdvancedTestFramework::runValidationTests() {
    pImpl->logger->info("Running validation tests...");
    runTestSuite("Validation");
}

void AdvancedTestFramework::runBenchmarkTests() {
    pImpl->logger->info("Running benchmark tests...");
    runTestSuite("Benchmark");
}

void AdvancedTestFramework::runRealWorldTests() {
    pImpl->logger->info("Running real-world scenario tests...");
    runTestSuite("RealWorld");
}

void AdvancedTestFramework::pause() {
    pImpl->paused = true;
    pImpl->logger->info("Test execution paused");
}

void AdvancedTestFramework::resume() {
    {
        std::lock_guard<std::mutex> lock(pImpl->pauseMutex);
        pImpl->paused = false;
    }
    pImpl->pauseCV.notify_all();
    pImpl->logger->info("Test execution resumed");
}

void AdvancedTestFramework::stop() {
    pImpl->stopRequested = true;
    {
        std::lock_guard<std::mutex> lock(pImpl->pauseMutex);
        pImpl->paused = false;
    }
    pImpl->pauseCV.notify_all();
    pImpl->logger->info("Test execution stopped");
}

bool AdvancedTestFramework::isRunning() const {
    return pImpl->running;
}

AdvancedTestFramework& AdvancedTestFramework::getInstance() {
    static AdvancedTestFramework instance;
    return instance;
}

void AdvancedTestFramework::initialize() {
    getInstance();
}

void AdvancedTestFramework::shutdown() {
    getInstance().stop();
}

} // namespace tests
} // namespace nlm
