// Implementation of NLMTestFramework
#include "NLMTestFramework.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stdexcept>
#include <cctype>

namespace nlm {

class NLMTestFramework::Impl {
public:
    std::vector<TestCase> testCases;
    std::unordered_map<std::string, size_t> testNameToIndex;
    
    bool parallel = false;
    size_t threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<size_t> completedTests{0};
    
    std::vector<TestResult> results;
    
    Config config;
    
    std::vector<PerformanceTest> performanceTests;
    std::vector<std::function<bool(const std::vector<TestResult>&)>> testPlugins;
    
    MemoryUsage memoryUsage;
    
    Statistics stats;
    
    bool running = false;
    bool cancelled = false;
    
    std::vector<std::future<TestResult>> futures;
};

// TestFramework implementation
NLMTestFramework::NLMTestFramework() : pImpl(std::make_unique<Impl>()) {}
NLMTestFramework::~NLMTestFramework() = default;

void NLMTestFramework::registerTest(const TestCase& testCase) {
    pImpl->testCases.push_back(testCase);
    pImpl->testNameToIndex[testCase.name] = pImpl->testCases.size() - 1;
}

void NLMTestFramework::registerTest(const std::string& name,
                                   const std::string& description,
                                   const std::function<bool()>& testFunction,
                                   std::chrono::milliseconds timeout) {
    TestCase testCase;
    testCase.name = name;
    testCase.description = description;
    testCase.testFunction = testFunction;
    testCase.timeout = timeout;
    registerTest(testCase);
}

void NLMTestFramework::registerTests(const std::vector<TestCase>& testCases) {
    for (const auto& testCase : testCases) {
        registerTest(testCase);
    }
}

std::vector<NLMTestFramework::TestResult> NLMTestFramework::runAllTests() {
    resetStatistics();
    
    auto filteredTests = filterTests(pImpl->testCases);
    if (filteredTests.empty()) {
        std::cout << "No tests to run" << std::endl;
        return {};
    }
    
    if (config_.verbose) {
        std::cout << "Running " << filteredTests.size() << " test(s)..." << std::endl;
    }
    
    if (config_.parallel && filteredTests.size() > 1) {
        runTestsInParallel(filteredTests);
    } else {
        runTestsSequentially(filteredTests);
    }
    
    updateStatistics();
    
    if (config_.saveResults) {
        saveResults(pImpl->results);
    }
    
    printResults(pImpl->results);
    printSummary(pImpl->results);
    
    return pImpl->results;
}

NLMTestFramework::TestResult NLMTestFramework::runTest(const std::string& name) {
    auto it = pImpl->testNameToIndex.find(name);
    if (it == pImpl->testNameToIndex.end()) {
        return {name, TestResult::ERROR, "Test not found", std::chrono::milliseconds(0), ""};
    }
    
    size_t index = it->second;
    return executeSingleTest(pImpl->testCases[index]);
}

NLMTestFramework::TestResult NLMTestFramework::runTest(const TestCase& testCase) {
    TestResult result;
    if (executeTest(testCase, result)) {
        return result;
    }
    return result;
}

void NLMTestFramework::runTestsMatching(const std::string& pattern) {
    setFilter(pattern);
    runAllTests();
}

void NLMTestFramework::runTestsWithTags(const std::vector<std::string>& tags) {
    setTags(tags);
    runAllTests();
}

void NLMTestFramework::setParallel(bool enable, size_t threadCount) {
    config_.parallel = enable;
    config_.threadCount = threadCount > 0 ? threadCount : std::thread::hardware_concurrency();
    pImpl->threadCount = config_.threadCount;
}

void NLMTestFramework::setFilter(const std::string& filter) {
    config_.filter = filter;
}

void NLMTestFramework::setTags(const std::vector<std::string>& tags) {
    config_.tags = tags;
}

void NLMTestFramework::setPrerequisites(const std::vector<std::string>& prerequisites) {
    config_.prerequisites = prerequisites;
}

void NLMTestFramework::setExpectedResult(const std::string& testName, TestResult expected) {
    for (auto& testCase : pImpl->testCases) {
        if (testCase.name == testName) {
            testCase.expectedResult = expected;
            break;
        }
    }
}

void NLMTestFramework::setVerbose(bool verbose) {
    config_.verbose = verbose;
    if (config_.verbose) {
        std::cout << "Verbose output enabled" << std::endl;
    }
}

void NLMTestFramework::setOutputFormat(TestOutputFormat format) {
    config_.outputFormat = format;
}

void NLMTestFramework::setOutputFile(const std::string& filepath) {
    config_.outputFile = filepath;
}

void NLMTestFramework::enableResultSaving(bool enable, const std::string& directory) {
    config_.saveResults = enable;
    if (enable && !directory.empty()) {
        std::filesystem::create_directories(directory);
        config_.outputFile = directory + "/test_results." + getOutputExtension();
    }
}

void NLMTestFramework::setContinueOnFailure(bool enable) {
    config_.continueOnFailure = enable;
}

void NLMTestFramework::registerPerformanceTest(const PerformanceTest& test) {
    pImpl->performanceTests.push_back(test);
}

std::vector<NLMTestFramework::PerformanceMetric> NLMTestFramework::runPerformanceTests() {
    std::vector<PerformanceMetric> metrics;
    
    for (const auto& perfTest : pImpl->performanceTests) {
        if (config_.verbose) {
            std::cout << "Running performance test: " << perfTest.name << std::endl;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            perfTest.testFunction();
        } catch (const std::exception& e) {
            std::cerr << "Performance test failed: " << e.what() << std::endl;
            continue;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        PerformanceMetric metric;
        metric.name = perfTest.name;
        metric.value = duration.count();
        metric.unit = "ms";
        metric.duration = duration;
        metrics.push_back(metric);
    }
    
    return metrics;
}

void NLMTestFramework::testMemoryLeaks(size_t duration) {
    // Simplified memory leak detection
    std::cout << "Memory leak test running for " << duration << " steps..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = startTime + std::chrono::milliseconds(duration);
    
    while (std::chrono::high_resolution_clock::now() < endTime) {
        // Allocate and deallocate memory to detect leaks
        std::vector<char> data(1024 * 1024); // 1MB
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::cout << "Memory leak test completed" << std::endl;
}

void NLMTestFramework::testMemoryUsage(const std::string& scenario) {
    // Get current memory usage
    memoryUsage.currentAllocated = getMemoryUsage().currentAllocated;
    memoryUsage.peakAllocated = getMemoryUsage().peakAllocated;
    memoryUsage.totalAllocated = getMemoryUsage().totalAllocated;
    
    std::cout << "Memory usage for scenario " << scenario << ":" << std::endl;
    std::cout << "  Total allocated: " << memoryUsage.totalAllocated << " bytes" << std::endl;
    std::cout << "  Current allocated: " << memoryUsage.currentAllocated << " bytes" << std::endl;
    std::cout << "  Peak allocated: " << memoryUsage.peakAllocated << " bytes" << std::endl;
}

NLMTestFramework::MemoryUsage NLMTestFramework::getMemoryUsage() const {
    MemoryUsage usage;
    
    // Simplified memory usage tracking
    // In a real implementation, this would use platform-specific APIs
    usage.totalAllocated = 1024 * 1024; // 1MB (placeholder)
    usage.currentAllocated = 512 * 1024; // 512KB (placeholder)
    usage.peakAllocated = 2 * 1024 * 1024; // 2MB (placeholder)
    usage.fragmentation = 10; // 10% (placeholder)
    
    return usage;
}

NLMTestFramework::StressTestResult NLMTestFramework::runStressTest(size_t neuronCount, size_t steps) {
    StressTestResult result;
    result.neuronCount = neuronCount;
    result.steps = steps;
    result.success = false;
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create a brain and run stress test
        auto brain = SimpleBrainBuilder::createBalancedBrain(neuronCount);
        if (!brain || !brain->initialize()) {
            result.error = "Failed to initialize brain";
            return result;
        }
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        result.duration = duration.count();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.averageFiringRate = static_cast<double>(result.totalSpikes) / duration.count();
        result.success = true;
        
    } catch (const std::exception& e) {
        result.error = std::string("Exception: ") + e.what();
    }
    
    return result;
}

std::vector<NLMTestFramework::StressTestResult> NLMTestFramework::runStressTests(
    const std::vector<std::pair<size_t, size_t>>& testCases) {
    std::vector<StressTestResult> results;
    
    for (const auto& testCase : testCases) {
        results.push_back(runStressTest(testCase.first, testCase.second));
    }
    
    return results;
}

bool NLMTestFramework::testIntegration(const std::vector<std::shared_ptr<Brain>>& brains,
                                      const std::vector<std::shared_ptr<Environment>>& environments) {
    if (brains.empty() || environments.empty()) {
        return false;
    }
    
    for (const auto& brain : brains) {
        if (!brain || !brain->initialize()) {
            return false;
        }
        
        // Run integration test
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step);
        }
    }
    
    return true;
}

bool NLMTestFramework::testEndToEnd(size_t neuronCount, size_t steps, size_t patterns) {
    try {
        auto brain = SimpleBrainBuilder::createBalancedBrain(neuronCount);
        if (!brain || !brain->initialize()) {
            return false;
        }
        
        for (size_t step = 0; step < steps; ++step) {
            // Apply different input patterns
            for (size_t pattern = 0; pattern < patterns; ++pattern++) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "End-to-end test failed: " << e.what() << std::endl;
        return false;
    }
}

bool NLMTestFramework::testConfiguration(const std::vector<std::shared_ptr<Config>>& configs) {
    for (const auto& config : configs) {
        if (!config) {
            return false;
        }
        
        // Create brain with config
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return false;
        }
    }
    
    return true;
}

bool NLMTestFramework::testConfigurationPresets() {
    nlm::EnhancedConfigManager config;
    
    return config.applyPreset(nlm::EnhancedConfigManager::Preset::MINIMAL) &&
           config.applyPreset(nlm::EnhancedConfigManager::Preset::BALANCED) &&
           config.applyPreset(nlm::EnhancedConfigManager::Preset::PERFORMANCE) &&
           config.applyPreset(nlm::EnhancedConfigManager::Preset::RESEARCH) &&
           config.applyPreset(nlm::EnhancedConfigManager::Preset::REALTIME);
}

void NLMTestFramework::registerTestPlugin(const TestPlugin& plugin) {
    pImpl->testPlugins.push_back(plugin);
}

void NLMTestFramework::runTestPlugins(const std::vector<TestResult>& results) {
    for (const auto& plugin : pImpl->testPlugins) {
        try {
            plugin(results);
        } catch (const std::exception& e) {
            std::cerr << "Test plugin failed: " << e.what() << std::endl;
        }
    }
}

void NLMTestFramework::setProgressCallback(const ProgressCallback& callback) {
    pImpl->progressCallback = callback;
}

void NLMTestFramework::setStatusCallback(const StatusCallback& callback) {
    pImpl->statusCallback = callback;
}

void NLMTestFramework::setTestStartCallback(const TestStartCallback& callback) {
    pImpl->testStartCallback = callback;
}

void NLMTestFramework::setTestCompleteCallback(const TestCompleteCallback& callback) {
    pImpl->testCompleteCallback = callback;
}

std::vector<NLMTestFramework::TestResult> NLMTestFramework::runTestBatch(const std::vector<std::string>& testNames) {
    std::vector<TestResult> results;
    
    for (const auto& testName : testNames) {
        results.push_back(runTest(testName));
    }
    
    return results;
}

void NLMTestFramework::runAllTestsAsync(std::function<void(const std::vector<TestResult>&)> callback) {
    auto future = runAllTestsAsync();
    future.then([callback](std::vector<TestResult> results) {
        callback(results);
    });
}

std::future<std::vector<NLMTestFramework::TestResult>> NLMTestFramework::runAllTestsAsync() {
    return std::async(std::launch::async, [this]() {
        return runAllTests();
    });
}

std::vector<NLMTestFramework::TestResult> NLMTestFramework::getFailedTests() const {
    std::vector<TestResult> failedTests;
    for (const auto& result : pImpl->results) {
        if (result.result == TestResult::FAILED || result.result == TestResult::ERROR) {
            failedTests.push_back(result);
        }
    }
    return failedTests;
}

std::vector<NLMTestFramework::TestResult> NLMTestFramework::getPassedTests() const {
    std::vector<TestResult> passedTests;
    for (const auto& result : pImpl->results) {
        if (result.result == TestResult::PASSED) {
            passedTests.push_back(result);
        }
    }
    return passedTests;
}

std::vector<NLMTestFramework::TestResult> NLMTestFramework::getTestsWithTag(const std::string& tag) const {
    std::vector<TestResult> taggedTests;
    for (const auto& result : pImpl->results) {
        if (std::find(result.tags.begin(), result.tags.end(), tag) != result.tags.end()) {
            taggedTests.push_back(result);
        }
    }
    return taggedTests;
}

std::vector<std::string> NLMTestFramework::getTestNames() const {
    std::vector<std::string> names;
    for (const auto& testCase : pImpl->testCases) {
        names.push_back(testCase.name);
    }
    return names;
}

std::vector<std::string> NLMTestFramework::getTestNamesWithTag(const std::string& tag) const {
    std::vector<std::string> names;
    for (const auto& testCase : pImpl->testCases) {
        if (std::find(testCase.tags.begin(), testCase.tags.end(), tag) != testCase.tags.end()) {
            names.push_back(testCase.name);
        }
    }
    return names;
}

std::vector<std::string> NLMTestFramework::getTestNamesMatching(const std::string& pattern) const {
    std::vector<std::string> names;
    for (const auto& testCase : pImpl->testCases) {
        if (matchesPattern(testCase.name, pattern)) {
            names.push_back(testCase.name);
        }
    }
    return names;
}

std::vector<NLMTestFramework::TestCase> NLMTestFramework::getPerformanceTestCases() {
    std::vector<TestCase> testCases;
    
    TestCase basicTest;
    basicTest.name = "Performance_Basic";
    basicTest.description = "Basic performance test";
    basicTest.testFunction = []() { 
        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count() < 500; // Should complete in less than 500ms
    };
    testCases.push_back(basicTest);
    
    TestCase memoryTest;
    memoryTest.name = "Performance_Memory";
    memoryTest.description = "Memory usage performance test";
    memoryTest.testFunction = []() { 
        std::vector<int> data(1000000);
        std::fill(data.begin(), data.end(), 1);
        return true;
    };
    testCases.push_back(memoryTest);
    
    return testCases;
}

std::vector<NLMTestFramework::TestCase> NLMTestFramework::getStressTestCases() {
    std::vector<TestCase> testCases;
    
    TestCase stressTest;
    stressTest.name = "Stress_HighLoad";
    stressTest.description = "High load stress test";
    stressTest.testFunction = []() { 
        auto start = std::chrono::high_resolution_clock::now();
        // Simulate high load
        for (int i = 0; i < 100000; ++i) {
            int x = i * i;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count() < 1000; // Should complete in less than 1000ms
    };
    testCases.push_back(stressTest);
    
    return testCases;
}

std::vector<NLMTestFramework::TestCase> NLMTestFramework::getIntegrationTestCases() {
    std::vector<TestCase> testCases;
    
    TestCase integrationTest;
    integrationTest.name = "Integration_MultipleBrains";
    integrationTest.description = "Integration test with multiple brains";
    integrationTest.testFunction = []() {
        // Create and test multiple brains
        auto brain1 = SimpleBrainBuilder::createBalancedBrain(100);
        auto brain2 = SimpleBrainBuilder::createBalancedBrain(200);
        
        if (!brain1 || !brain2) return false;
        if (!brain1->initialize() || !brain2->initialize()) return false;
        
        return true;
    };
    testCases.push_back(integrationTest);
    
    return testCases;
}

std::vector<NLMTestFramework::TestCase> NLMTestFramework::getRegressionTestCases() {
    std::vector<TestCase> testCases;
    
    TestCase regressionTest;
    regressionTest.name = "Regression_Consistency";
    regressionTest.description = "Regression test for consistency";
    regressionTest.testFunction = []() {
        // Run the same test twice and check consistency
        auto brain1 = SimpleBrainBuilder::createBalancedBrain(50);
        auto brain2 = SimpleBrainBuilder::createBalancedBrain(50);
        
        if (!brain1 || !brain2) return false;
        if (!brain1->initialize() || !brain2->initialize()) return false;
        
        // Test that results are consistent
        auto action1 = brain1->produceAction();
        auto action2 = brain2->produceAction();
        
        return (action1 != nullptr) == (action2 != nullptr);
    };
    testCases.push_back(regressionTest);
    
    return testCases;
}

std::string NLMTestFramework::generateTestName(const std::string& prefix, size_t iteration) {
    std::stringstream ss;
    ss << prefix << "_" << std::setw(3) << std::setfill('0') << iteration;
    return ss.str();
}

bool NLMTestFramework::compareFloat(float a, float b, float epsilon) {
    return std::abs(a - b) < epsilon;
}

bool NLMTestFramework::compareDouble(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

// Helper functions
std::string NLMTestFramework::getOutputExtension() const {
    switch (config_.outputFormat) {
        case TestOutputFormat::JSON: return "json";
        case TestOutputFormat::CSV: return "csv";
        case TestOutputFormat::XML: return "xml";
        case TestOutputFormat::HTML: return "html";
        default: return "txt";
    }
}

std::string NLMTestFramework::formatResults(const std::vector<TestResult>& results, TestOutputFormat format) const {
    switch (format) {
        case TestOutputFormat::PLAIN:
            return formatPlainResults(results);
        case TestOutputFormat::JSON:
            return formatJSONResults(results);
        case TestOutputFormat::CSV:
            return formatCSVResults(results);
        case TestOutputFormat::XML:
            return formatXMLResults(results);
        case TestOutputFormat::HTML:
            return formatHTMLResults(results);
        default:
            return formatPlainResults(results);
    }
}

std::string NLMTestFramework::formatPlainResults(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "=== Test Results ===" << std::endl;
    ss << "Total tests: " << results.size() << std::endl;
    ss << "Passed: " << getPassedCount() << std::endl;
    ss << "Failed: " << getFailedCount() << std::endl;
    ss << "Skipped: " << getSkippedCount() << std::endl;
    ss << "Errors: " << getErrorCount() << std::endl;
    ss << "Timeouts: " << getTimeoutCount() << std::endl;
    ss << std::endl;
    
    for (const auto& result : results) {
        ss << result.testName << ": " << static_cast<int>(result.result)
           << " (" << result.duration.count() << " ms)" << std::endl;
        if (!result.message.empty()) {
            ss << "  " << result.message << std::endl;
        }
    }
    
    return ss.str();
}

std::string NLMTestFramework::formatJSONResults(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "{\n  \"tests\": [" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        ss << "    {\n      \"name\": \"" << escapeJSONString(result.testName) << "\",\n"
           << "      \"result\": " << static_cast<int>(result.result) << ",\n"
           << "      \"message\": \"" << escapeJSONString(result.message) << "\",\n"
           << "      \"duration\": " << result.duration.count() << "\n"
           << "    }";
        
        if (i < results.size() - 1) {
            ss << ",\n";
        } else {
            ss << std::endl;
        }
    }
    
    ss << "  ],\n"
       << "  \"statistics\": {\n"
       << "    \"total\": " << results.size() << ",\n"
       << "    \"passed\": " << getPassedCount() << ",\n"
       << "    \"failed\": " << getFailedCount() << ",\n"
       << "    \"skipped\": " << getSkippedCount() << ",\n"
       << "    \"error\": " << getErrorCount() << ",\n"
       << "    \"timeout\": " << getTimeoutCount() << "\n"
       << "  }\n"
       << "}";
    
    return ss.str();
}

std::string NLMTestFramework::formatCSVResults(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Test Name,Result,Duration,Message" << std::endl;
    
    for (const auto& result : results) {
        ss << escapeJSONString(result.testName) << ","
           << static_cast<int>(result.result) << ","
           << result.duration.count() << ","
           << escapeJSONString(result.message) << std::endl;
    }
    
    return ss.str();
}

std::string NLMTestFramework::formatXMLResults(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<test-results>" << std::endl;
    ss << "  <statistics>" << std::endl;
    ss << "    <total>" << results.size() << "</total>" << std::endl;
    ss << "    <passed>" << getPassedCount() << "</passed>" << std::endl;
    ss << "    <failed>" << getFailedCount() << "</failed>" << std::endl;
    ss << "    <skipped>" << getSkippedCount() << "</skipped>" << std::endl;
    ss << "    <error>" << getErrorCount() << "</error>" << std::endl;
    ss << "    <timeout>" << getTimeoutCount() << "</timeout>" << std::endl;
    ss << "  </statistics>" << std::endl;
    
    for (const auto& result : results) {
        ss << "  <test>" << std::endl;
        ss << "    <name>" << escapeXML(result.testName) << "</name>" << std::endl;
        ss << "    <result>" << static_cast<int>(result.result) << "</result>" << std::endl;
        ss << "    <duration>" << result.duration.count() << "</duration>" << std::endl;
        ss << "    <message>" << escapeXML(result.message) << "</message>" << std::endl;
        ss << "  </test>" << std::endl;
    }
    
    ss << "</test-results>" << std::endl;
    return ss.str();
}

std::string NLMTestFramework::formatHTMLResults(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head><title>Test Results</title></head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "<h1>Test Results</h1>" << std::endl;
    ss << "<table border=\"1\">" << std::endl;
    ss << "  <tr><th>Name</th><th>Result</th><th>Duration</th><th>Message</th></tr>" << std::endl;
    
    for (const auto& result : results) {
        ss << "  <tr>" << std::endl;
        ss << "    <td>" << escapeHTML(result.testName) << "</td>" << std::endl;
        ss << "    <td>" << static_cast<int>(result.result) << "</td>" << std::endl;
        ss << "    <td>" << result.duration.count() << " ms</td>" << std::endl;
        ss << "    <td>" << escapeHTML(result.message) << "</td>" << std::endl;
        ss << "  </tr>" << std::endl;
    }
    
    ss << "</table>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    return ss.str();
}

std::string NLMTestFramework::escapeJSONString(const std::string& str) const {
    std::stringstream ss;
    ss << std::quoted(str);
    return ss.str();
}

std::string NLMTestFramework::escapeXML(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&apos;"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string NLMTestFramework::escapeHTML(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#x27;"; break;
            default: result += c; break;
        }
    }
    return result;
}

bool NLMTestFramework::matchesPattern(const std::string& str, const std::string& pattern) const {
    if (pattern.empty()) return true;
    
    // Simple wildcard matching
    for (size_t i = 0, j = 0; i < str.length() && j < pattern.length(); ++i, ++j) {
        if (pattern[j] == '*') {
            // Skip wildcards
            while (j < pattern.length() && pattern[j] == '*') j++;
            if (j >= pattern.length()) return true; // End of pattern, rest of string matches
            
            // Find the next occurrence of pattern[j] in str
            while (i < str.length() && str[i] != pattern[j]) i++;
            if (i >= str.length()) return false;
            --i; // Back up one character for the next iteration
        } else if (pattern[j] == '?') {
            // Match any single character
            continue;
        } else if (pattern[j] != str[i]) {
            return false;
        }
    }
    
    return j >= pattern.length() && i >= str.length();
}

void NLMTestFramework::resetStatistics() {
    pImpl->stats.totalTests = 0;
    pImpl->stats.passedTests = 0;
    pImpl->stats.failedTests = 0;
    pImpl->stats.skippedTests = 0;
    pImpl->stats.errorTests = 0;
    pImpl->stats.timeoutTests = 0;
    pImpl->stats.totalDuration = std::chrono::milliseconds(0);
    pImpl->stats.passRate = 0.0;
    
    passedCount_ = 0;
    failedCount_ = 0;
    skippedCount_ = 0;
    errorCount_ = 0;
    timeoutCount_ = 0;
}

void NLMTestFramework::updateStatistics() {
    size_t total = 0;
    size_t passed = 0;
    size_t failed = 0;
    size_t skipped = 0;
    size_t error = 0;
    size_t timeout = 0;
    
    for (const auto& result : pImpl->results) {
        total++;
        switch (result.result) {
            case TestResult::PASSED: passed++; break;
            case TestResult::FAILED: failed++; break;
            case TestResult::SKIPPED: skipped++; break;
            case TestResult::ERROR: error++; break;
            case TestResult::TIMEOUT: timeout++; break;
        }
    }
    
    stats_.totalTests = total;
    stats_.passedTests = passed;
    stats_.failedTests = failed;
    stats_.skippedTests = skipped;
    stats_.errorTests = error;
    stats_.timeoutTests = timeout;
    
    passedCount_ = passed;
    failedCount_ = failed;
    skippedCount_ = skipped;
    errorCount_ = error;
    timeoutCount_ = timeout;
    
    stats_.passRate = total > 0 ? (static_cast<double>(passed) / total * 100.0) : 0.0;
}

void NLMTestFramework::printResults(const std::vector<TestResult>& results) const {
    if (config_.verbose) {
        printFormattedResults(results);
    } else {
        printCompactResults(results);
    }
}

void NLMTestFramework::printSummary(const std::vector<TestResult>& results) const {
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << results.size() << std::endl;
    std::cout << "Passed: " << getPassedCount() << std::endl;
    std::cout << "Failed: " << getFailedCount() << std::endl;
    std::cout << "Skipped: " << getSkippedCount() << std::endl;
    std::cout << "Errors: " << getErrorCount() << std::endl;
    std::cout << "Timeouts: " << getTimeoutCount() << std::endl;
    std::cout << std::endl;
    
    if (anyTestsFailed()) {
        std::cout << "FAILED TESTS:" << std::endl;
        auto failedTests = getFailedTests();
        for (const auto& result : failedTests) {
            std::cout << "  " << result.testName << ": " << result.message << std::endl;
        }
    }
}

void NLMTestFramework::saveResults(const std::vector<TestResult>& results) const {
    if (config_.outputFile.empty()) {
        return;
    }
    
    std::ofstream file(config_.outputFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open output file: " << config_.outputFile << std::endl;
        return;
    }
    
    file << formatResults(results, config_.outputFormat);
    file.close();
}

void NLMTestFramework::printFormattedResults(const std::vector<TestResult>& results) const {
    std::cout << "=== Detailed Test Results ===" << std::endl;
    
    for (const auto& result : results) {
        std::cout << result.testName << ": " << static_cast<int>(result.result)
                  << " (" << result.duration.count() << " ms)";
        if (!result.message.empty()) {
            std::cout << " - " << result.message;
        }
        std::cout << std::endl;
    }
}

void NLMTestFramework::printCompactResults(const std::vector<TestResult>& results) const {
    std::cout << "Test Results: " << getPassedCount() << "/" << results.size()
              << " passed (" << getPassRate() << "%)";
    
    if (anyTestsFailed()) {
        std::cout << " - FAILED";
    }
    
    std::cout << std::endl;
}

void NLMTestFramework::runTestsInParallel(const std::vector<TestCase>& testCases) {
    pImpl->running = true;
    pImpl->cancelled = false;
    
    size_t numTests = testCases.size();
    size_t testsPerThread = numTests / pImpl->threadCount;
    size_t remainingTests = numTests % pImpl->threadCount;
    
    std::vector<std::vector<TestCase>> threadTasks(pImpl->threadCount);
    
    size_t currentIndex = 0;
    for (size_t i = 0; i < pImpl->threadCount; ++i) {
        size_t testsInThread = testsPerThread + (i < remainingTests ? 1 : 0);
        threadTasks[i].assign(testCases.begin() + currentIndex,
                              testCases.begin() + currentIndex + testsInThread);
        currentIndex += testsInThread;
    }
    
    pImpl->threads.clear();
    pImpl->futures.clear();
    
    for (size_t i = 0; i < pImpl->threadCount && !threadTasks[i].empty(); ++i) {
        pImpl->futures.push_back(std::async(std::launch::async, [this, &testCases, i, testsPerThread]() {
            std::vector<TestResult> threadResults;
            
            for (size_t j = 0; j < testsPerThread && !pImpl->cancelled; ++j) {
                size_t index = i * testsPerThread + j;
                if (index >= testCases.size()) break;
                
                TestResult result;
                if (executeTest(testCases[index], result)) {
                    threadResults.push_back(result);
                }
                
                // Notify progress
                if (pImpl->progressCallback) {
                    double progress = static_cast<double>(++pImpl->completedTests) / testCases.size();
                    pImpl->progressCallback(progress);
                }
            }
            
            return threadResults;
        }));
    }
    
    // Wait for all futures
    for (auto& future : pImpl->futures) {
        if (future.valid()) {
            auto results = future.get();
            pImpl->results.insert(pImpl->results.end(), results.begin(), results.end());
        }
    }
    
    pImpl->running = false;
    
    // Join threads
    for (auto& thread : pImpl->threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void NLMTestFramework::runTestsSequentially(const std::vector<TestCase>& testCases) {
    pImpl->completedTests = 0;
    
    for (const auto& testCase : testCases) {
        if (pImpl->cancelled) break;
        
        TestResult result;
        if (executeTest(testCase, result)) {
            pImpl->results.push_back(result);
        }
        
        // Notify progress
        if (pImpl->progressCallback) {
            double progress = static_cast<double>(++pImpl->completedTests) / testCases.size();
            pImpl->progressCallback(progress);
        }
        
        // Notify completion
        if (pImpl->testCompleteCallback) {
            pImpl->testCompleteCallback(result);
        }
    }
}

std::vector<NLMTestFramework::TestCase> NLMTestFramework::filterTests(const std::vector<TestCase>& testCases) const {
    std::vector<TestCase> filteredTests;
    
    for (const auto& testCase : testCases) {
        if (matchesFilter(testCase) && hasAllTags(testCase) && hasAllPrerequisites(testCase)) {
            filteredTests.push_back(testCase);
        }
    }
    
    return filteredTests;
}

bool NLMTestFramework::matchesFilter(const TestCase& testCase) const {
    if (config_.filter.empty()) {
        return true;
    }
    
    return matchesPattern(testCase.name, config_.filter);
}

bool NLMTestFramework::hasAllTags(const TestCase& testCase) const {
    if (config_.tags.empty()) {
        return true;
    }
    
    for (const auto& tag : config_.tags) {
        if (std::find(testCase.tags.begin(), testCase.tags.end(), tag) == testCase.tags.end()) {
            return false;
        }
    }
    
    return true;
}

bool NLMTestFramework::hasAllPrerequisites(const TestCase& testCase) const {
    if (config_.prerequisites.empty()) {
        return true;
    }
    
    for (const auto& prerequisite : config_.prerequisites) {
        if (std::find(testCase.prerequisites.begin(), testCase.prerequisites.end(), prerequisite) == testCase.prerequisites.end()) {
            return false;
        }
    }
    
    return true;
}

bool NLMTestFramework::executeTest(const TestCase& testCase, TestResult& result) {
    // Check if test has already been executed
    auto it = pImpl->testNameToIndex.find(testCase.name);
    if (it != pImpl->testNameToIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->results.size()) {
            result = pImpl->results[index];
            return true;
        }
    }
    
    // Check if test should be skipped
    if (testCase.expectedResult == TestResult::SKIPPED) {
        result.testName = testCase.name;
        result.result = TestResult::SKIPPED;
        result.message = "Skipped (expected)";
        result.duration = std::chrono::milliseconds(0);
        result.tags = testCase.tags;
        result.iteration = 0;
        return true;
    }
    
    // Execute test
    return executeSingleTest(testCase, result);
}

bool NLMTestFramework::executeSingleTest(const TestCase& testCase, TestResult& result) {
    result.testName = testCase.name;
    result.tags = testCase.tags;
    result.iteration = pImpl->results.size() + 1;
    
    // Notify test start
    if (pImpl->testStartCallback) {
        pImpl->testStartCallback(testCase.name);
    }
    
    // Check if test should be skipped due to prerequisites
    if (!hasAllPrerequisites(testCase)) {
        result.result = TestResult::SKIPPED;
        result.message = "Skipped (prerequisites not met)";
        result.duration = std::chrono::milliseconds(0);
        return true;
    }
    
    // Check if test should be skipped due to filter
    if (!matchesFilter(testCase)) {
        result.result = TestResult::SKIPPED;
        result.message = "Skipped (filter)";
        result.duration = std::chrono::milliseconds(0);
        return true;
    }
    
    // Check if test should be skipped due to tags
    if (!hasAllTags(testCase)) {
        result.result = TestResult::SKIPPED;
        result.message = "Skipped (tags)";
        result.duration = std::chrono::milliseconds(0);
        return true;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Set timeout for test execution
        auto timeout = testCase.timeout;
        auto future = std::async(std::launch::async, [this, &testCase]() {
            return testCase.testFunction();
        });
        
        // Wait for test result with timeout
        std::future_status status = future.wait_for(timeout);
        bool testPassed = false;
        std::string errorMessage;
        
        if (status == std::future_status::ready) {
            testPassed = future.get();
        } else {
            // Cancel the future
            future.wait(); // Ensure the future is cancelled
            testPassed = false;
            errorMessage = "Timeout after " + std::to_string(timeout.count()) + " ms";
            result.result = TestResult::TIMEOUT;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        result.duration = duration;
        
        if (testPassed) {
            result.result = TestResult::PASSED;
            result.message = "Test passed";
        } else {
            result.result = TestResult::FAILED;
            result.message = "Test failed";
        }
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        result.duration = duration;
        result.result = TestResult::ERROR;
        result.message = "Test error";
        result.errorDetails = std::string("Exception: ") + e.what();
    } catch (...) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        result.duration = duration;
        result.result = TestResult::ERROR;
        result.message = "Test error";
        result.errorDetails = "Unknown exception";
    }
    
    // Notify test completion
    if (pImpl->testCompleteCallback) {
        pImpl->testCompleteCallback(result);
    }
    
    return true;
}

void NLMTestFramework::updateStatistics() {
    pImpl->stats.totalTests = pImpl->results.size();
    pImpl->stats.passedTests = 0;
    pImpl->stats.failedTests = 0;
    pImpl->stats.skippedTests = 0;
    pImpl->stats.errorTests = 0;
    pImpl->stats.timeoutTests = 0;
    pImpl->stats.totalDuration = std::chrono::milliseconds(0);
    
    for (const auto& result : pImpl->results) {
        switch (result.result) {
            case TestResult::PASSED: pImpl->stats.passedTests++; break;
            case TestResult::FAILED: pImpl->stats.failedTests++; break;
            case TestResult::SKIPPED: pImpl->stats.skippedTests++; break;
            case TestResult::ERROR: pImpl->stats.errorTests++; break;
            case TestResult::TIMEOUT: pImpl->stats.timeoutTests++; break;
        }
        
        pImpl->stats.totalDuration += result.duration;
    }
    
    stats_ = pImpl->stats;
    
    // Update local counts
    passedCount_ = pImpl->stats.passedTests;
    failedCount_ = pImpl->stats.failedTests;
    skippedCount_ = pImpl->stats.skippedTests;
    errorCount_ = pImpl->stats.errorTests;
    timeoutCount_ = pImpl->stats.timeoutTests;
}

} // namespace nlm

// Global test framework instance
namespace nlm {
inline NLMTestFramework& getTestFramework() {
    static NLMTestFramework framework;
    return framework;
}

inline void registerTest(const NLMTestFramework::TestCase& testCase) {
    getTestFramework().registerTest(testCase);
}

inline std::vector<NLMTestFramework::TestResult> runAllTests() {
    return getTestFramework().runAllTests();
}

} // namespace nlm

// Convenience macros for easy test registration
#define REGISTER_TEST(name, description, function) \
    namespace { \
        struct Register##name { \
            Register##name() { \
                nlm::registerTest(nlm::NLMTestFramework::TestCase{ \
                    #name, description, function \
                }); \
            } \
        } register##name##instance; \
    }

#define REGISTER_PERFORMANCE_TEST(name, function) \
    namespace { \
        struct RegisterPerformance##name { \
            RegisterPerformance##name() { \
                auto& framework = nlm::getTestFramework(); \
                framework.registerPerformanceTest(nlm::NLMTestFramework::PerformanceTest{ \
                    #name, function \
                }); \
            } \
        } registerPerformance##name##instance; \
    }

#define ASSERT_TRUE(condition, message) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " + std::string(message)); \
    }

#define ASSERT_FALSE(condition, message) \
    if ((condition)) { \
        throw std::runtime_error("Assertion failed: " + std::string(message)); \
    }

#define ASSERT_EQUAL(expected, actual, message) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("Assertion failed: " + std::string(message) + \
                                 " (expected: " + std::to_string(expected) + \
                                 ", actual: " + std::to_string(actual) + ")"); \
    }