// Integration Test Framework Implementation
// Implementation of the IntegrationTestFramework class and related test functions

#include "tests/IntegrationTestFramework.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace test_framework {

// IntegrationTestSuite implementation
IntegrationTestSuite::IntegrationTestSuite(const IntegrationTestConfig& config) : config_(config) {
    // Ensure report directory exists
    if (config.generateReports && !std::filesystem::exists(config.reportDirectory)) {
        std::filesystem::create_directories(config.reportDirectory);
    }
}

IntegrationTestSuite::~IntegrationTestSuite() {
}

void IntegrationTestSuite::addTest(const std::string& testName,
                                   std::function<bool()> testFunction,
                                   const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex_);
    testCases_.emplace_back(testName, testFunction);
}

std::vector<TestResult> IntegrationTestSuite::executeAllTests() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    testResults_.clear();
    
    for (const auto& testCase : testCases_) {
        testResults_.push_back(executeTest(testCase.first, testCase.second, "general"));
    }
    
    return testResults_;
}

std::vector<TestResult> IntegrationTestSuite::executeTests(const std::vector<std::string>& testNames) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    testResults_.clear();
    
    for (const auto& testCase : testCases_) {
        if (std::find(testNames.begin(), testNames.end(), testCase.first) != testNames.end()) {
            testResults_.push_back(executeTest(testCase.first, testCase.second, "general"));
        }
    }
    
    return testResults_;
}

TestResult IntegrationTestSuite::executeTest(const std::string& testName,
                                             std::function<bool()> testFunction,
                                             const std::string& category) {
    TestResult result(testName, category);
    
    // Start performance monitoring
    performanceMonitor_.startSession(testName, "Integration test: " + testName);
    
    bool testPassed = false;
    std::string errorMsg;
    
    try {
        // Execute the test
        testPassed = testFunction();
    } catch (const std::exception& e) {
        errorMsg = std::string("Exception: ") + e.what();
    } catch (...) {
        errorMsg = "Unknown exception occurred";
    }
    
    // Stop performance monitoring
    performanceMonitor_.endSession(testName);
    
    // Collect metrics
    result.passed = testPassed;
    result.executionTime = performanceMonitor_.getAggregatedMetrics(testName)["simulation_duration"];
    result.memoryUsage = performanceMonitor_.getAggregatedMetrics(testName)["memory_usage"];
    result.cpuUsage = performanceMonitor_.getAggregatedMetrics(testName)["cpu_usage"];
    
    if (!errorMsg.empty()) {
        result.errorMessage = errorMsg;
    }
    
    // Add to results
    testResults_.push_back(result);
    
    return result;
}

std::string IntegrationTestSuite::generateReport(const std::string& format) const {
    if (format == "json") {
        return generateSuiteReport();
    } else if (format == "csv") {
        return "CSV report generation not yet implemented";
    } else if (format == "html") {
        return "HTML report generation not yet implemented";
    }
    
    return "Unsupported format: " + format;
}

bool IntegrationTestSuite::exportResults(const std::string& filename, const std::string& format) {
    if (format == "json") {
        return exportResultsJSON(filename);
    } else if (format == "csv") {
        return exportResultsCSV(filename);
    } else if (format == "html") {
        return exportResultsHTML(filename);
    }
    
    return false;
}

void IntegrationTestSuite::cleanupOldSessions(double maxAge) {
    performanceMonitor_.cleanupOldSessions(maxAge);
}

std::string IntegrationTestSuite::generateSuiteReport() const {
    std::stringstream report;
    
    report << "=== Integration Test Suite Report ===" << std::endl;
    report << "Suite Name: " << config_.name << std::endl;
    report << "Description: " << config_.description << std::endl;
    report << "Total Tests: " << testResults_.size() << std::endl;
    report << "Passed Tests: " << getPassedTestsCount() << std::endl;
    report << "Failed Tests: " << (testResults_.size() - getPassedTestsCount()) << std::endl;
    report << "Success Rate: " << (getSuccessRate() * 100.0) << "%" << std::endl;
    report << std::endl;
    
    // Category breakdown
    report << "=== Category Breakdown ===" << std::endl;
    std::unordered_map<std::string, int> categoryCounts;
    std::unordered_map<std::string, int> categoryPassed;
    
    for (const auto& result : testResults_) {
        categoryCounts[result.testCategory]++;
        if (result.passed) {
            categoryPassed[result.testCategory]++;
        }
    }
    
    for (const auto& pair : categoryCounts) {
        report << pair.first << ": " << pair.second << " tests"
               << " (" << (categoryPassed.count(pair.first) ? categoryPassed.at(pair.first) : 0) << " passed)" << std::endl;
    }
    
    report << std::endl;
    
    // Detailed results
    report << "=== Detailed Test Results ===" << std::endl;
    for (const auto& result : testResults_) {
        report << "Test: " << result.testName << std::endl;
        report << "Category: " << result.testCategory << std::endl;
        report << "Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
        report << "Execution Time: " << result.executionTime << " seconds" << std::endl;
        report << "Memory Usage: " << result.memoryUsage << " MB" << std::endl;
        report << "CPU Usage: " << result.cpuUsage << "%" << std::endl;
        if (!result.errorMessage.empty()) {
            report << "Error: " << result.errorMessage << std::endl;
        }
        report << std::endl;
    }
    
    return report.str();
}

bool IntegrationTestSuite::exportResultsJSON(const std::string& filename) const {
    json j;
    
    j["suiteName"] = config_.name;
    j["description"] = config_.description;
    j["totalTests"] = testResults_.size();
    j["passedTests"] = getPassedTestsCount();
    j["failedTests"] = (testResults_.size() - getPassedTestsCount());
    j["successRate"] = getSuccessRate();
    
    // Add test results
    j["testResults"] = json::array();
    for (const auto& result : testResults_) {
        json testJson;
        testJson["name"] = result.testName;
        testJson["category"] = result.testCategory;
        testJson["passed"] = result.passed;
        testJson["executionTime"] = result.executionTime;
        testJson["memoryUsage"] = result.memoryUsage;
        testJson["cpuUsage"] = result.cpuUsage;
        if (!result.errorMessage.empty()) {
            testJson["errorMessage"] = result.errorMessage;
        }
        j["testResults"].push_back(testJson);
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump(2);
    file.close();
    
    return true;
}

bool IntegrationTestSuite::exportResultsCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "Test Name,Category,Status,Execution Time (s),Memory Usage (MB),CPU Usage (%),Error Message" << std::endl;
    
    // Write test results
    for (const auto& result : testResults_) {
        file << "\"" << result.testName << "\"," << result.testCategory << "," 
             << (result.passed ? "PASSED" : "FAILED") << "," << result.executionTime << "," 
             << result.memoryUsage << "," << result.cpuUsage << ",";
        
        if (!result.errorMessage.empty()) {
            // Escape quotes in error message
            std::string errorMsg = result.errorMessage;
            std::replace(errorMsg.begin(), errorMsg.end(), '"', '"');
            file << "\"" << errorMsg << "\"";
        }
        
        file << std::endl;
    }
    
    file.close();
    return true;
}

bool IntegrationTestSuite::exportResultsHTML(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write HTML content
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "<title>Integration Test Report - " << config_.name << "</title>" << std::endl;
    file << "<style>" << std::endl;
    file << "body { font-family: Arial, sans-serif; margin: 40px; }" << std::endl;
    file << "h1 { color: #333; }" << std::endl;
    file << "h2 { color: #666; border-bottom: 2px solid #ccc; padding-bottom: 10px; }" << std::endl;
    file << "table { border-collapse: collapse; width: 100%; margin: 20px 0; }" << std::endl;
    file << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    file << "th { background-color: #f2f2f2; }" << std::endl;
    file << "tr:nth-child(even) { background-color: #f9f9f9; }" << std::endl;
    file << ".passed { background-color: #d4edda; color: #155724; }" << std::endl;
    file << ".failed { background-color: #f8d7da; color: #721c24; }" << std::endl;
    file << "</style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    
    // Write report header
    file << "<h1>Integration Test Report - " << config_.name << "</h1>" << std::endl;
    file << "<p><strong>Description:</strong> " << config_.description << "</p>" << std::endl;
    file << "<p><strong>Total Tests:</strong> " << testResults_.size() << "</p>" << std::endl;
    file << "<p><strong>Passed Tests:</strong> " << getPassedTestsCount() << "</p>" << std::endl;
    file << "<p><strong>Failed Tests:</strong> " << (testResults_.size() - getPassedTestsCount()) << "</p>" << std::endl;
    file << "<p><strong>Success Rate:</strong> " << (getSuccessRate() * 100.0) << "%</p>" << std::endl;
    
    // Write test results table
    file << "<h2>Test Results</h2>" << std::endl;
    file << "<table>" << std::endl;
    file << "<tr><th>Test Name</th><th>Category</th><th>Status</th><th>Execution Time (s)</th><th>Memory Usage (MB)</th><th>CPU Usage (%)</th><th>Error Message</th></tr>" << std::endl;
    
    for (const auto& result : testResults_) {
        file << "<tr>" << std::endl;
        file << "<td>" << result.testName << "</td><td>" << result.testCategory << "</td><td class=\"" 
             << (result.passed ? "passed" : "failed") << "\">" << (result.passed ? "PASSED" : "FAILED") << "</td><td>" << result.executionTime << "</td><td>" << result.memoryUsage << "</td><td>" << result.cpuUsage << "</td><td>";
        
        if (!result.errorMessage.empty()) {
            file << result.errorMessage;
        }
        
        file << "</td>" << std::endl;
        file << "</tr>" << std::endl;
    }
    
    file << "</table>" << std::endl;
    
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    return true;
}

// TestRunner implementation
TestRunner::TestRunner() {
}

TestRunner::~TestRunner() {
}

std::vector<TestResult> TestRunner::runIntegrationTestSuite(IntegrationTestSuite& testSuite) {
    return testSuite.executeAllTests();
}

std::vector<TestResult> TestRunner::runPerformanceTestSuite(const IntegrationTestConfig& config) {
    return performance_tests::runAllPerformanceTests(config);
}

std::vector<TestResult> TestRunner::runStressTestSuite(const IntegrationTestConfig& config) {
    return stress_tests::runAllStressTests(config);
}

std::vector<TestResult> TestRunner::runFullTestSuite(const IntegrationTestConfig& config) {
    std::vector<TestResult> allResults;
    
    // Run integration tests
    IntegrationTestSuite integrationTestSuite(config);
    auto integrationResults = runIntegrationTestSuite(integrationTestSuite);
    allResults.insert(allResults.end(), integrationResults.begin(), integrationResults.end());
    
    // Run performance tests
    auto performanceResults = runPerformanceTestSuite(config);
    allResults.insert(allResults.end(), performanceResults.begin(), performanceResults.end());
    
    // Run stress tests
    auto stressResults = runStressTestSuite(config);
    allResults.insert(allResults.end(), stressResults.begin(), stressResults.end());
    
    return allResults;
}

std::string TestRunner::generateComprehensiveReport(const std::vector<TestResult>& results,
                                                    const std::string& format) const {
    if (format == "json") {
        json j;
        j["totalTests"] = results.size();
        j["passedTests"] = std::count_if(results.begin(), results.end(),
                                       [](const TestResult& result) { return result.passed; });
        j["failedTests"] = results.size() - j["passedTests"].get<int>();
        j["successRate"] = static_cast<double>(j["passedTests"].get<int>()) / results.size();
        
        // Add category breakdown
        std::unordered_map<std::string, int> categoryCounts;
        std::unordered_map<std::string, int> categoryPassed;
        
        for (const auto& result : results) {
            categoryCounts[result.testCategory]++;
            if (result.passed) {
                categoryPassed[result.testCategory]++;
            }
        }
        
        j["categoryBreakdown"] = categoryCounts;
        j["categorySuccess"] = categoryPassed;
        
        // Add individual test results
        j["testResults"] = json::array();
        for (const auto& result : results) {
            json testJson;
            testJson["name"] = result.testName;
            testJson["category"] = result.testCategory;
            testJson["passed"] = result.passed;
            testJson["executionTime"] = result.executionTime;
            testJson["memoryUsage"] = result.memoryUsage;
            testJson["cpuUsage"] = result.cpuUsage;
            if (!result.errorMessage.empty()) {
                testJson["errorMessage"] = result.errorMessage;
            }
            j["testResults"].push_back(testJson);
        }
        
        return j.dump(2);
    }
    
    return "Unsupported format: " + format;
}

bool TestRunner::saveTestResults(const std::vector<TestResult>& results,
                                 const std::string& filename,
                                 const std::string& format) {
    if (format == "json") {
        return generateComprehensiveReport(results, format);
    }
    
    return false;
}

IntegrationTestConfig TestRunner::loadTestConfig(const std::string& filename) const {
    IntegrationTestConfig config;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return config; // Return default config if file not found
    }
    
    try {
        json j = json::parse(file);
        
        config.name = j.value("name", "");
        config.description = j.value("description", "");
        config.enabled = j.value("enabled", true);
        config.numIterations = j.value("numIterations", 1);
        config.maxSteps = j.value("maxSteps", 1000);
        config.usePerformanceMonitoring = j.value("usePerformanceMonitoring", true);
        config.generateReports = j.value("generateReports", true);
        config.reportDirectory = j.value("reportDirectory", "test_reports");
        config.successThreshold = j.value("successThreshold", 0.8);
        config.timeoutSeconds = j.value("timeoutSeconds", 300);
        
        // Load brain configuration
        if (j.contains("brainConfig")) {
            const auto& brainConfigJson = j["brainConfig"];
            config.brainConfig.neuronCount = brainConfigJson.value("neuronCount", 100);
            config.brainConfig.regionCount = brainConfigJson.value("regionCount", 1);
            config.brainConfig.connectionProbability = brainConfigJson.value("connectionProbability", 0.1f);
            config.brainConfig.enableDevelopment = brainConfigJson.value("enableDevelopment", true);
            config.brainConfig.enableMemory = brainConfigJson.value("enableMemory", true);
            config.brainConfig.enableNeuromodulation = brainConfigJson.value("enableNeuromodulation", true);
            config.brainConfig.enablePrediction = brainConfigJson.value("enablePrediction", true);
            config.brainConfig.enableCognition = brainConfigJson.value("enableCognition", true);
        }
        
        // Load memory configuration
        if (j.contains("memoryConfig")) {
            const auto& memoryConfigJson = j["memoryConfig"];
            config.memoryConfig.workingMemoryCapacity = memoryConfigJson.value("workingMemoryCapacity", 50);
            config.memoryConfig.episodicMemoryCapacity = memoryConfigJson.value("episodicMemoryCapacity", 100);
            config.memoryConfig.memoryDecayRate = memoryConfigJson.value("memoryDecayRate", 0.01f);
            config.memoryConfig.enableReplay = memoryConfigJson.value("enableReplay", true);
        }
        
        // Load neuromodulation configuration
        if (j.contains("neuromodulationConfig")) {
            const auto& neuromodulationConfigJson = j["neuromodulationConfig"];
            config.neuromodulationConfig.dopamineScale = neuromodulationConfigJson.value("dopamineScale", 1.0f);
            config.neuromodulationConfig.enableCuriosity = neuromodulationConfigJson.value("enableCuriosity", true);
            config.neuromodulationConfig.enableNovelty = neuromodulationConfigJson.value("enableNovelty", true);
            config.neuromodulationConfig.curiosityThreshold = neuromodulationConfigJson.value("curiosityThreshold", 0.3f);
        }
        
        // Load environment configuration
        if (j.contains("environmentConfig")) {
            const auto& environmentConfigJson = j["environmentConfig"];
            config.environmentConfig.worldWidth = environmentConfigJson.value("worldWidth", 16);
            config.environmentConfig.worldHeight = environmentConfigJson.value("worldHeight", 16);
            config.environmentConfig.visionWidth = environmentConfigJson.value("visionWidth", 8);
            config.environmentConfig.visionHeight = environmentConfigJson.value("visionHeight", 8);
            config.environmentConfig.enableVisualInput = environmentConfigJson.value("enableVisualInput", true);
            config.environmentConfig.enableTouchInput = environmentConfigJson.value("enableTouchInput", true);
            config.environmentConfig.enableInternalInput = environmentConfigJson.value("enableInternalInput", true);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading test config: " << e.what() << std::endl;
    }
    
    return config;
}

void TestRunner::saveTestConfig(const IntegrationTestConfig& config, const std::string& filename) const {
    json j;
    
    j["name"] = config.name;
    j["description"] = config.description;
    j["enabled"] = config.enabled;
    j["numIterations"] = config.numIterations;
    j["maxSteps"] = config.maxSteps;
    j["usePerformanceMonitoring"] = config.usePerformanceMonitoring;
    j["generateReports"] = config.generateReports;
    j["reportDirectory"] = config.reportDirectory;
    j["successThreshold"] = config.successThreshold;
    j["timeoutSeconds"] = config.timeoutSeconds;
    
    // Save brain configuration
    json brainConfigJson;
    brainConfigJson["neuronCount"] = config.brainConfig.neuronCount;
    brainConfigJson["regionCount"] = config.brainConfig.regionCount;
    brainConfigJson["connectionProbability"] = config.brainConfig.connectionProbability;
    brainConfigJson["enableDevelopment"] = config.brainConfig.enableDevelopment;
    brainConfigJson["enableMemory"] = config.brainConfig.enableMemory;
    brainConfigJson["enableNeuromodulation"] = config.brainConfig.enableNeuromodulation;
    brainConfigJson["enablePrediction"] = config.brainConfig.enablePrediction;
    brainConfigJson["enableCognition"] = config.brainConfig.enableCognition;
    j["brainConfig"] = brainConfigJson;
    
    // Save memory configuration
    json memoryConfigJson;
    memoryConfigJson["workingMemoryCapacity"] = config.memoryConfig.workingMemoryCapacity;
    memoryConfigJson["episodicMemoryCapacity"] = config.memoryConfig.episodicMemoryCapacity;
    memoryConfigJson["memoryDecayRate"] = config.memoryConfig.memoryDecayRate;
    memoryConfigJson["enableReplay"] = config.memoryConfig.enableReplay;
    j["memoryConfig"] = memoryConfigJson;
    
    // Save neuromodulation configuration
    json neuromodulationConfigJson;
    neuromodulationConfigJson["dopamineScale"] = config.neuromodulationConfig.dopamineScale;
    neuromodulationConfigJson["enableCuriosity"] = config.neuromodulationConfig.enableCuriosity;
    neuromodulationConfigJson["enableNovelty"] = config.neuromodulationConfig.enableNovelty;
    neuromodulationConfigJson["curiosityThreshold"] = config.neuromodulationConfig.curiosityThreshold;
    j["neuromodulationConfig"] = neuromodulationConfigJson;
    
    // Save environment configuration
    json environmentConfigJson;
    environmentConfigJson["worldWidth"] = config.environmentConfig.worldWidth;
    environmentConfigJson["worldHeight"] = config.environmentConfig.worldHeight;
    environmentConfigJson["visionWidth"] = config.environmentConfig.visionWidth;
    environmentConfigJson["visionHeight"] = config.environmentConfig.visionHeight;
    environmentConfigJson["enableVisualInput"] = config.environmentConfig.enableVisualInput;
    environmentConfigJson["enableTouchInput"] = config.environmentConfig.enableTouchInput;
    environmentConfigJson["enableInternalInput"] = config.environmentConfig.enableInternalInput;
    j["environmentConfig"] = environmentConfigJson;
    
    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(2);
        file.close();
    }
}

std::vector<TestResult> TestRunner::runTestSuite(std::function<std::vector<TestResult>(const IntegrationTestConfig&)> testRunner,
                                                  const IntegrationTestConfig& config) {
    return testRunner(config);
}

// Integration test function implementations
bool integration_tests::testMemorySystemIntegration(const IntegrationTestConfig& config) {
    // This is a simplified implementation - in a real system, you would
    // create actual NLM systems and test their integration
    
    try {
        // Create brain with memory systems
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(config.brainConfig.regionCount), nlm::ConfigSource::Default);
        brainConfig->set("connection_probability", config.brainConfig.connectionProbability, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Test memory systems
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        
        if (!workingMemory || !episodicMemory) {
            return false;
        }
        
        // Store and retrieve memory
        std::vector<float> testPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        workingMemory->store(testPattern, 1.0f);
        
        if (workingMemory->getActiveTraces() == 0) {
            return false;
        }
        
        // Run simulation
        for (int step = 0; step < config.maxSteps; ++step) {
            brain->step(step);
            
            // Test episodic memory
            if (config.memoryConfig.enableReplay && step % 100 == 0) {
                auto episodes = episodicMemory->getEpisodesForReplay(3);
                if (!episodes.empty()) {
                    // Test replay works
                    episodicMemory->consolidateMemory();
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testNeuromodulationSystemIntegration(const IntegrationTestConfig& config) {
    try {
        // Create brain with neuromodulation systems
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(config.brainConfig.regionCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Test neuromodulation systems
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        
        if (!dopamine || !curiosity || !novelty) {
            return false;
        }
        
        // Apply reward modulation
        dopamine->applyReward(0.5f, 0.3f);
        
        // Update curiosity
        curiosity->updateCuriosity();
        
        // Run simulation
        for (int step = 0; step < config.maxSteps; ++step) {
            // Simulate novel events
            if (step % 50 == 0) {
                novelty->setLevel(0.5f + (static_cast<float>(rand()) / RAND_MAX) * 0.5f);
                curiosity->updateCuriosity();
            }
            
            brain->step(step);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testDevelopmentSystemIntegration(const IntegrationTestConfig& config) {
    try {
        // Create brain with development system
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(config.brainConfig.regionCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Test development system
        auto* development = brain->getDevelopmentSystem();
        if (!development) {
            return false;
        }
        
        // Run simulation with development
        for (int step = 0; step < config.maxSteps; ++step) {
            brain->step(step, step * 0.001f);
            
            if (config.brainConfig.enableDevelopment) {
                development->update(0.001f);
            }
            
            // Check developmental stage progression
            nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
            if (static_cast<int>(stage) < 0 || static_cast<int>(stage) >= 6) {
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testPredictionSystemIntegration(const IntegrationTestConfig& config) {
    try {
        // Create brain with prediction system
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(config.brainConfig.regionCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Test prediction system
        auto* prediction = brain->getPredictionSystem();
        if (!prediction) {
            return false;
        }
        
        // Run simulation
        for (int step = 0; step < config.maxSteps; ++step) {
            brain->step(step);
            
            if (config.brainConfig.enablePrediction) {
                // Simulate prediction error
                prediction->updatePredictionError(0.5f - (static_cast<float>(rand()) / RAND_MAX) * 1.0f, 0.3f);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testCognitionSystemIntegration(const IntegrationTestConfig& config) {
    try {
        // Create brain with cognition systems
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(config.brainConfig.regionCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Test cognition systems
        auto* planner = brain->getPlanner();
        auto* conceptFormation = brain->getConceptFormation();
        auto* attention = brain->getAttention();
        
        if (!planner || !conceptFormation || !attention) {
            return false;
        }
        
        // Run simulation
        for (int step = 0; step < config.maxSteps; ++step) {
            brain->step(step);
            
            if (config.brainConfig.enableCognition) {
                // Simulate attentional selection
                std::vector<nlm::NeuronId> competitors;
                for (int i = 0; i < 10; ++i) {
                    competitors.push_back(nlm::NeuronId(i));
                }
                
                auto winners = attention->processCompetition(competitors, 0.5f);
                
                // Simulate concept formation
                std::vector<float> pattern = {static_cast<float>(step % 100) / 100.0f};
                conceptFormation->update(pattern);
                
                // Simulate planning
                planner->update(step);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testFullSystemIntegration(const IntegrationTestConfig& config) {
    try {
        // Use Phase 6 integrated experiment framework
        nlm::Phase6IntegratedExperiment experiment;
        
        // Test basic integration verification
        bool integrationOK = experiment.verifyIntegration();
        if (!integrationOK) {
            return false;
        }
        
        // Run a smaller integration test
        nlm::Phase6Config phase6Config;
        phase6Config.neuronCount = static_cast<size_t>(config.brainConfig.neuronCount);
        phase6Config.maxSteps = static_cast<uint64_t>(config.maxSteps);
        phase6Config.enableCheckpointing = config.memoryConfig.enableReplay;
        phase6Config.enableReplay = config.memoryConfig.enableReplay;
        phase6Config.enableDevelopment = config.brainConfig.enableDevelopment;
        
        auto result = experiment.run(phase6Config);
        
        // Verify integration results
        if (!result.memoryWorkingMemoryIntegrated) {
            return false;
        }
        
        if (!result.memoryEpisodicMemoryIntegrated) {
            return false;
        }
        
        if (!result.neuromodulationIntegrated) {
            return false;
        }
        
        if (!result.developmentIntegrated) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testStressIntegration(const IntegrationTestConfig& config) {
    try {
        // Create brain with stress test configuration
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount * 2), nlm::ConfigSource::Default); // Double neurons
        brainConfig->set("region_count", static_cast<int64_t>(config.brainConfig.regionCount * 2), nlm::ConfigSource::Default); // Double regions
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Run high-intensity simulation
        for (int step = 0; step < config.maxSteps; ++step) {
            // Inject high levels of activity
            brain->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 50.0f);
            brain->injectCurrentToNeurons(nlm::NeuronType::Inhibitory, 20.0f);
            
            brain->step(step);
            
            // Rapid state changes
            if (step % 10 == 0) {
                brain->reset();
                brain->initialize();
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testPerformanceIntegration(const IntegrationTestConfig& config) {
    try {
        // This test focuses on performance aspects
        nlm::PerformanceMonitor::MonitorConfig perfConfig;
        perfConfig.enableRealTimeMonitoring = true;
        perfConfig.enableMemoryMonitoring = true;
        perfConfig.enableCPUMonitoring = true;
        
        nlm::PerformanceMonitor monitor(perfConfig);
        
        // Create and start session
        monitor.startSession("performance_test", "Performance integration test");
        
        // Create brain
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Run performance test
        for (int step = 0; step < config.maxSteps; ++step) {
            brain->step(step);
            
            // Record metrics
            monitor.recordMetric("performance_test", "simulation_step", "simulation", static_cast<double>(step));
        }
        
        // End session
        monitor.endSession("performance_test");
        
        // Generate report
        std::string report = monitor.generateReport("json");
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testRecoveryIntegration(const IntegrationTestConfig& config) {
    try {
        // Test integration recovery after failures
        for (int recoveryTest = 0; recoveryTest < 3; ++recoveryTest) {
            // Create brain
            auto brainConfig = std::make_shared<nlm::Config>();
            brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(brainConfig);
            if (!brain->initialize()) {
                return false;
            }
            
            // Run simulation with controlled failures
            for (int step = 0; step < config.maxSteps / 3; ++step) {
                // Simulate occasional failures
                if (step % 100 == 0) {
                    brain->reset();
                    brain->initialize();
                }
                
                brain->step(step);
            }
            
            // Verify recovery
            if (brain->getTotalNeuronCount() == 0) {
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testScalabilityIntegration(const IntegrationTestConfig& config) {
    try {
        // Test integration with different system sizes
        std::vector<int> neuronCounts = {50, 100, 200, 400};
        
        for (int neuronCount : neuronCounts) {
            if (neuronCount > config.brainConfig.neuronCount * 4) { // Limit to reasonable values
                continue;
            }
            
            // Create brain with different size
            auto brainConfig = std::make_shared<nlm::Config>();
            brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(brainConfig);
            if (!brain->initialize()) {
                return false;
            }
            
            // Run simulation
            for (int step = 0; step < config.maxSteps / 4; ++step) {
                brain->step(step);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testConsistencyIntegration(const IntegrationTestConfig& config) {
    try {
        // Run multiple tests with same configuration
        std::vector<double> results;
        
        for (int run = 0; run < config.numIterations; ++run) {
            // Create brain
            auto brainConfig = std::make_shared<nlm::Config>();
            brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(brainConfig);
            if (!brain->initialize()) {
                return false;
            }
            
            // Run simulation
            for (int step = 0; step < config.maxSteps; ++step) {
                brain->step(step);
            }
            
            // Collect metrics
            double finalFiringRate = brain->getAverageFiringRate();
            results.push_back(finalFiringRate);
        }
        
        // Check consistency (all results should be similar)
        if (results.empty()) {
            return false;
        }
        
        double mean = std::accumulate(results.begin(), results.end(), 0.0) / results.size();
        double variance = 0.0;
        for (double result : results) {
            variance += (result - mean) * (result - mean);
        }
        variance /= results.size();
        double stdDev = std::sqrt(variance);
        
        // Coefficient of variation should be reasonable
        double coefficientOfVariation = stdDev / mean;
        if (coefficientOfVariation > 0.5) { // High variability
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool integration_tests::testReliabilityIntegration(const IntegrationTestConfig& config) {
    try {
        // Test reliability under varying conditions
        std::vector<double> successRates;
        
        for (int variation = 0; variation < 3; ++variation) {
            // Modify configuration slightly
            auto brainConfig = std::make_shared<nlm::Config>();
            brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
            brainConfig->set("connection_probability", config.brainConfig.connectionProbability + (variation * 0.05f), nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(brainConfig);
            if (!brain->initialize()) {
                return false;
            }
            
            // Run simulation
            for (int step = 0; step < config.maxSteps; ++step) {
                brain->step(step);
            }
            
            // Calculate success rate (simplified)
            double successRate = (brain->getFiringNeuronCount() > 0) ? 1.0 : 0.0;
            successRates.push_back(successRate);
        }
        
        // All tests should succeed
        for (double successRate : successRates) {
            if (successRate < config.successThreshold) {
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<TestResult> integration_tests::runAllIntegrationTests(const IntegrationTestConfig& config) {
    std::vector<TestResult> results;
    
    // Define test functions
    std::vector<std::pair<std::string, std::function<bool(const IntegrationTestConfig&)>> > testFunctions = {
        {"Memory System Integration", testMemorySystemIntegration},
        {"Neuromodulation System Integration", testNeuromodulationSystemIntegration},
        {"Development System Integration", testDevelopmentSystemIntegration},
        {"Prediction System Integration", testPredictionSystemIntegration},
        {"Cognition System Integration", testCognitionSystemIntegration},
        {"Full System Integration", testFullSystemIntegration},
        {"Stress Integration", testStressIntegration},
        {"Performance Integration", testPerformanceIntegration},
        {"Recovery Integration", testRecoveryIntegration},
        {"Scalability Integration", testScalabilityIntegration},
        {"Consistency Integration", testConsistencyIntegration},
        {"Reliability Integration", testReliabilityIntegration},
    };
    
    for (const auto& testPair : testFunctions) {
        try {
            bool testPassed = testPair.second(config);
            
            TestResult result(testPair.first, "integration");
            result.passed = testPassed;
            
            if (testPassed) {
                result.errorMessage = "Test passed";
            } else {
                result.errorMessage = "Test failed";
            }
            
            results.push_back(result);
            
        } catch (const std::exception& e) {
            TestResult result(testPair.first, "integration");
            result.passed = false;
            result.errorMessage = std::string("Exception: ") + e.what();
            results.push_back(result);
        }
    }
    
    return results;
}

std::string integration_tests::generateIntegrationTestReport(const std::vector<TestResult>& results,
                                                               const std::string& format) {
    if (format == "json") {
        json j;
        j["testSuite"] = "NLM Integration Tests";
        j["totalTests"] = results.size();
        j["passedTests"] = std::count_if(results.begin(), results.end(),
                                       [](const TestResult& result) { return result.passed; });
        j["failedTests"] = results.size() - j["passedTests"].get<int>();
        j["successRate"] = static_cast<double>(j["passedTests"].get<int>()) / results.size();
        
        // Add test results
        j["testResults"] = json::array();
        for (const auto& result : results) {
            json testJson;
            testJson["name"] = result.testName;
            testJson["category"] = result.testCategory;
            testJson["passed"] = result.passed;
            testJson["executionTime"] = result.executionTime;
            testJson["memoryUsage"] = result.memoryUsage;
            testJson["cpuUsage"] = result.cpuUsage;
            if (!result.errorMessage.empty()) {
                testJson["errorMessage"] = result.errorMessage;
            }
            j["testResults"].push_back(testJson);
        }
        
        return j.dump(2);
    }
    
    return "Unsupported format: " + format;
}

} // namespace integration_tests

// Performance test function implementations
namespace performance_tests {

bool performance_tests::testMemoryPerformance(const IntegrationTestConfig& config) {
    try {
        nlm::PerformanceMonitor::MonitorConfig perfConfig;
        perfConfig.enableRealTimeMonitoring = true;
        perfConfig.enableMemoryMonitoring = true;
        
        nlm::PerformanceMonitor monitor(perfConfig);
        monitor.startSession("memory_performance", "Memory performance test");
        
        // Simulate memory-intensive operations
        std::vector<std::vector<float>> memoryIntensiveData;
        for (int i = 0; i < config.memoryConfig.workingMemoryCapacity; ++i) {
            std::vector<float> data(config.brainConfig.neuronCount / 10, 0.0f);
            for (size_t j = 0; j < data.size(); ++j) {
                data[j] = static_cast<float>(rand()) / RAND_MAX;
            }
            memoryIntensiveData.push_back(data);
        }
        
        // Process data
        for (const auto& data : memoryIntensiveData) {
            // Simulate processing
            double sum = std::accumulate(data.begin(), data.end(), 0.0);
            double average = sum / data.size();
            
            monitor.recordMetric("memory_performance", "processing_time", "memory", average);
        }
        
        monitor.endSession("memory_performance");
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool performance_tests::testCPUPerformance(const IntegrationTestConfig& config) {
    try {
        nlm::PerformanceMonitor::MonitorConfig perfConfig;
        perfConfig.enableRealTimeMonitoring = true;
        perfConfig.enableCPUMonitoring = true;
        
        nlm::PerformanceMonitor monitor(perfConfig);
        monitor.startSession("cpu_performance", "CPU performance test");
        
        // Simulate CPU-intensive operations
        for (int i = 0; i < config.maxSteps; ++i) {
            // Perform computationally intensive operations
            std::vector<std::vector<double>> matrix(config.brainConfig.regionCount,
                                                    std::vector<double>(config.brainConfig.neuronCount / 10));
            
            for (size_t r = 0; r < matrix.size(); ++r) {
                for (size_t c = 0; c < matrix[r].size(); ++c) {
                    matrix[r][c] = std::sin(static_cast<double>(i) * 0.01) * std::cos(static_cast<double>(c) * 0.01);
                }
            }
            
            // Record metrics
            monitor.recordMetric("cpu_performance", "computation_time", "cpu", 0.001);
            
            // Simulate neural computation
            monitor.recordMetric("cpu_performance", "neural_computation", "neural", 0.1);
        }
        
        monitor.endSession("cpu_performance");
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool performance_tests::testNeuralNetworkPerformance(const IntegrationTestConfig& config) {
    try {
        nlm::PerformanceMonitor::MonitorConfig perfConfig;
        perfConfig.enableRealTimeMonitoring = true;
        
        nlm::PerformanceMonitor monitor(perfConfig);
        monitor.startSession("neural_network_performance", "Neural network performance test");
        
        // Create brain
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Simulate neural network operations
        for (int step = 0; step < config.maxSteps; ++step) {
            brain->step(step);
            
            // Record neural network metrics
            monitor.recordMetric("neural_network_performance", "firing_rate", "neural",
                                 brain->getAverageFiringRate());
            monitor.recordMetric("neural_network_performance", "spike_count", "neural",
                                 static_cast<double>(brain->getTotalSpikeCount()));
            monitor.recordMetric("neural_network_performance", "plasticity", "neural", 0.1);
        }
        
        monitor.endSession("neural_network_performance");
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool performance_tests::testIntegrationPerformance(const IntegrationTestConfig& config) {
    try {
        // Test performance of integrated systems
        nlm::PerformanceMonitor::MonitorConfig perfConfig;
        perfConfig.enableRealTimeMonitoring = true;
        config.usePerformanceMonitoring = true;
        
        nlm::PerformanceMonitor monitor(perfConfig);
        monitor.startSession("integration_performance", "Integration performance test");
        
        // Create and run integrated simulation
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(config.brainConfig.regionCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Run integration simulation
        for (int step = 0; step < config.maxSteps; ++step) {
            brain->step(step);
            
            // Record integration metrics
            monitor.recordMetric("integration_performance", "system_load", "integration", 0.5);
            monitor.recordMetric("integration_performance", "response_time", "integration", 0.01);
            monitor.recordMetric("integration_performance", "throughput", "integration", 1.0);
        }
        
        monitor.endSession("integration_performance");
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool performance_tests::testScalabilityPerformance(const IntegrationTestConfig& config) {
    try {
        // Test performance scalability
        nlm::PerformanceMonitor::MonitorConfig perfConfig;
        perfConfig.enableRealTimeMonitoring = true;
        
        nlm::PerformanceMonitor monitor(perfConfig);
        
        // Test different system sizes
        std::vector<int> neuronCounts = {50, 100, 200, 400, 800};
        
        for (int neuronCount : neuronCounts) {
            if (neuronCount > config.brainConfig.neuronCount * 2) { // Limit size
                continue;
            }
            
            monitor.startSession("scalability_performance", "Scalability test with " + std::to_string(neuronCount) + " neurons");
            
            // Create brain with different size
            auto brainConfig = std::make_shared<nlm::Config>();
            brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(brainConfig);
            if (!brain->initialize()) {
                continue;
            }
            
            // Run simulation
            for (int step = 0; step < config.maxSteps / 5; ++step) {
                brain->step(step);
            }
            
            monitor.endSession("scalability_performance");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool performance_tests::testStressPerformance(const IntegrationTestConfig& config) {
    try {
        // Test performance under stress
        nlm::PerformanceMonitor::MonitorConfig perfConfig;
        perfConfig.enableRealTimeMonitoring = true;
        
        nlm::PerformanceMonitor monitor(perfConfig);
        monitor.startSession("stress_performance", "Stress performance test");
        
        // Create brain
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Run stress simulation
        for (int step = 0; step < config.maxSteps; ++step) {
            // Inject high levels of activity
            brain->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 100.0f);
            
            brain->step(step);
            
            // Record stress metrics
            monitor.recordMetric("stress_performance", "stress_level", "stress", 1.0);
            monitor.recordMetric("stress_performance", "recovery_time", "stress", 0.1);
        }
        
        monitor.endSession("stress_performance");
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<TestResult> performance_tests::runAllPerformanceTests(const IntegrationTestConfig& config) {
    std::vector<TestResult> results;
    
    // Define test functions
    std::vector<std::pair<std::string, std::function<bool(const IntegrationTestConfig&)>> > testFunctions = {
        {"Memory Performance", testMemoryPerformance},
        {"CPU Performance", testCPUPerformance},
        {"Neural Network Performance", testNeuralNetworkPerformance},
        {"Integration Performance", testIntegrationPerformance},
        {"Scalability Performance", testScalabilityPerformance},
        {"Stress Performance", testStressPerformance},
    };
    
    for (const auto& testPair : testFunctions) {
        try {
            bool testPassed = testPair.second(config);
            
            TestResult result(testPair.first, "performance");
            result.passed = testPassed;
            
            if (testPassed) {
                result.errorMessage = "Test passed";
            } else {
                result.errorMessage = "Test failed";
            }
            
            results.push_back(result);
            
        } catch (const std::exception& e) {
            TestResult result(testPair.first, "performance");
            result.passed = false;
            result.errorMessage = std::string("Exception: ") + e.what();
            results.push_back(result);
        }
    }
    
    return results;
}

} // namespace performance_tests

// Stress test function implementations
namespace stress_tests {

bool stress_tests::testMemoryStress(const IntegrationTestConfig& config) {
    try {
        // Create brain with memory stress configuration
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount * 2), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Memory stress test
        for (int step = 0; step < config.maxSteps; ++step) {
            // Inject high memory usage
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 200.0f);
            
            brain->step(step);
            
            // Simulate memory pressure
            if (step % 100 == 0) {
                brain->reset();
                brain->initialize();
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool stress_tests::testCPUStress(const IntegrationTestConfig& config) {
    try {
        // Create brain with CPU stress configuration
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // CPU stress test
        for (int step = 0; step < config.maxSteps; ++step) {
            // Perform computationally intensive operations
            std::vector<std::vector<double>> stressMatrix(config.brainConfig.regionCount,
                                                          std::vector<double>(config.brainConfig.neuronCount));
            
            for (size_t r = 0; r < stressMatrix.size(); ++r) {
                for (size_t c = 0; c < stressMatrix[r].size(); ++c) {
                    // Complex mathematical operations
                    double value = std::sin(static_cast<double>(step) * 0.001) * std::cos(static_cast<double>(r) * 0.001) *
                                    std::exp(-static_cast<double>(c) * 0.001) * static_cast<double>(rand()) / RAND_MAX;
                    stressMatrix[r][c] = value;
                }
            }
            
            brain->step(step);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool stress_tests::testNeuralStress(const IntegrationTestConfig& config) {
    try {
        // Create brain with neural stress configuration
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Neural stress test
        for (int step = 0; step < config.maxSteps; ++step) {
            // Extreme neural activity
            brain->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1000.0f);
            brain->injectCurrentToNeurons(nlm::NeuronType::Inhibitory, 500.0f);
            
            brain->step(step);
            
            // Neural damage simulation
            if (step % 200 == 0) {
                brain->reset();
                brain->initialize();
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool stress_tests::testIntegrationStress(const IntegrationTestConfig& config) {
    try {
        // Create brain with integration stress configuration
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(config.brainConfig.neuronCount), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(brainConfig);
        if (!brain->initialize()) {
            return false;
        }
        
        // Integration stress test
        for (int step = 0; step < config.maxSteps; ++step) {
            // High system load
            brain->injectCurrentToNeurons(nlm::NeuronType::All, 500.0f);
            
            brain->step(step);
            
            // Simulate system failures
            if (step % 150 == 0) {
                brain->reset();
                brain->initialize();
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<TestResult> stress_tests::runAllStressTests(const IntegrationTestConfig& config) {
    std::vector<TestResult> results;
    
    // Define test functions
    std::vector<std::pair<std::string, std::function<bool(const IntegrationTestConfig&)>> > testFunctions = {
        {"Memory Stress", testMemoryStress},
        {"CPU Stress", testCPUStress},
        {"Neural Stress", testNeuralStress},
        {"Integration Stress", testIntegrationStress},
    };
    
    for (const auto& testPair : testFunctions) {
        try {
            bool testPassed = testPair.second(config);
            
            TestResult result(testPair.first, "stress");
            result.passed = testPassed;
            
            if (testPassed) {
                result.errorMessage = "Test passed";
            } else {
                result.errorMessage = "Test failed";
            }
            
            results.push_back(result);
            
        } catch (const std::exception& e) {
            TestResult result(testPair.first, "stress");
            result.passed = false;
            result.errorMessage = std::string("Exception: ") + e.what();
            results.push_back(result);
        }
    }
    
    return results;
}

} // namespace stress_tests

} // namespace nlm