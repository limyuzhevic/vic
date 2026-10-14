#include "TestFramework.hpp"
#include <algorithm>
#include <filesystem>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

namespace nlm {

struct TestFramework::Impl {
    std::vector<std::shared_ptr<TestSuite>> suites;
    std::shared_ptr<TestRunner> testRunner;
    std::vector<std::shared_ptr<TestReporter>> reporters;
    
    bool running;
    double maxExecutionTime;
    size_t memoryLimit;
    size_t threadCount;
    
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    Impl() : running(false), maxExecutionTime(300.0), memoryLimit(1024 * 1024 * 1024), // 1GB
             threadCount(std::thread::hardware_concurrency()), startTime(), endTime() {}
};

TestFramework::TestFramework() : pImpl(std::make_unique<Impl>()) {}

TestFramework::~TestFramework() = default;

TestFramework& TestFramework::getInstance() {
    static TestFramework instance;
    return instance;
}

void TestFramework::destroyInstance() {
    // This is a placeholder - in a real implementation, we would track instances
}

void TestFramework::configure(const std::string& configFile) {
    std::ifstream configFileStream(configFile);
    if (!configFileStream.is_open()) {
        std::cerr << "Failed to open configuration file: " << configFile << std::endl;
        return;
    }
    
    try {
        nlohmann::json config = nlohmann::json::parse(configFileStream);
        
        // Parse configuration
        if (config.contains("max_execution_time")) {
            setMaxExecutionTime(config["max_execution_time"]);
        }
        if (config.contains("memory_limit")) {
            setMemoryLimit(config["memory_limit"]);
        }
        if (config.contains("thread_count")) {
            setThreadCount(config["thread_count"]);
        }
        
        // Load test suites
        if (config.contains("test_suites")) {
            loadTestSuites(config["test_suites"]);
        }
        
        pImpl->suites.clear();
        for (const auto& suiteConfig : config["test_suites"]) {
            std::string suiteName = suiteConfig["name"];
            
            // Create suite instance based on name
            std::shared_ptr<TestSuite> suite;
            if (suiteName == "Neuron Tests") {
                suite = std::make_shared<NeuronTestSuite>();
            } else if (suiteName == "Synapse Tests") {
                suite = std::make_shared<SynapseTestSuite>();
            } else if (suiteName == "Plasticity Tests") {
                suite = std::make_shared<PlasticityTestSuite>();
            } else if (suiteName == "Memory Tests") {
                suite = std::make_shared<MemoryTestSuite>();
            } else if (suiteName == "Neuromodulation Tests") {
                suite = std::make_shared<NeuromodulationTestSuite>();
            } else if (suiteName == "Brain Integration Tests") {
                suite = std::make_shared<BrainIntegrationTestSuite>();
            } else if (suiteName == "Performance Tests") {
                suite = std::make_shared<PerformanceTestSuite>();
            } else if (suiteName == "Environment Tests") {
                suite = std::make_shared<EnvironmentTestSuite>();
            } else if (suiteName == "System Tests") {
                suite = std::make_shared<SystemTestSuite>();
            } else {
                std::cerr << "Unknown test suite: " << suiteName << std::endl;
                continue;
            }
            
            addSuite(suite);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
    }
}

void TestFramework::resetConfiguration() {
    pImpl->suites.clear();
    pImpl->running = false;
    
    // Add default test suites
    pImpl->suites.push_back(std::make_shared<NeuronTestSuite>());
    pImpl->suites.push_back(std::make_shared<SynapseTestSuite>());
    pImpl->suites.push_back(std::make_shared<PlasticityTestSuite>());
    pImpl->suites.push_back(std::make_shared<MemoryTestSuite>());
    pImpl->suites.push_back(std::make_shared<NeuromodulationTestSuite>());
    pImpl->suites.push_back(std::make_shared<BrainIntegrationTestSuite>());
    pImpl->suites.push_back(std::make_shared<PerformanceTestSuite>());
    pImpl->suites.push_back(std::make_shared<EnvironmentTestSuite>());
    pImpl->suites.push_back(std::make_shared<SystemTestSuite>());
}

bool TestFramework::addSuite(std::shared_ptr<TestSuite> suite) {
    if (!suite) {
        return false;
    }
    
    auto it = std::find_if(pImpl->suites.begin(), pImpl->suites.end(),
                          [&suite](const std::shared_ptr<TestSuite>& s) {
                              return s->getName() == suite->getName();
                          });
    
    if (it != pImpl->suites.end()) {
        std::cerr << "Test suite already exists: " << suite->getName() << std::endl;
        return false;
    }
    
    pImpl->suites.push_back(suite);
    return true;
}

bool TestFramework::removeSuite(const std::string& suiteName) {
    auto it = std::find_if(pImpl->suites.begin(), pImpl->suites.end(),
                          [&suiteName](const std::shared_ptr<TestSuite>& s) {
                              return s->getName() == suiteName;
                          });
    
    if (it == pImpl->suites.end()) {
        return false;
    }
    
    pImpl->suites.erase(it);
    return true;
}

bool TestFramework::clearSuites() {
    pImpl->suites.clear();
    return true;
}

std::vector<std::shared_ptr<TestSuite>> TestFramework::getSuites() const {
    return pImpl->suites;
}

std::shared_ptr<TestSuite> TestFramework::getSuite(const std::string& suiteName) const {
    auto it = std::find_if(pImpl->suites.begin(), pImpl->suites.end(),
                          [&suiteName](const std::shared_ptr<TestSuite>& s) {
                              return s->getName() == suiteName;
                          });
    
    if (it != pImpl->suites.end()) {
        return *it;
    }
    
    return nullptr;
}

void TestFramework::setTestRunner(std::shared_ptr<TestRunner> runner) {
    pImpl->testRunner = runner;
}

std::shared_ptr<TestRunner> TestFramework::getTestRunner() const {
    return pImpl->testRunner;
}

void TestFramework::addReporter(std::shared_ptr<TestReporter> reporter) {
    if (reporter) {
        pImpl->reporters.push_back(reporter);
    }
}

void TestFramework::removeReporter(const std::string& name) {
    pImpl->reporters.erase(std::remove_if(pImpl->reporters.begin(), pImpl->reporters.end(),
                                         [&name](const std::shared_ptr<TestReporter>& r) {
                                             return r->getReport() == name;
                                         }),
                          pImpl->reporters.end());
}

std::vector<std::shared_ptr<TestReporter>> TestFramework::getReporters() const {
    return pImpl->reporters;
}

bool TestFramework::runAllTests() {
    if (pImpl->running) {
        std::cerr << "Tests are already running" << std::endl;
        return false;
    }
    
    pImpl->running = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    
    // Report test start
    for (const auto& reporter : pImpl->reporters) {
        reporter->reportSuiteStart("All Tests");
    }
    
    // Run all test suites
    bool allPassed = true;
    for (const auto& suite : pImpl->suites) {
        bool suitePassed = suite->runAllTests();
        allPassed = allPassed && suitePassed;
    }
    
    pImpl->endTime = std::chrono::steady_clock::now();
    pImpl->running = false;
    
    // Report test results
    std::map<std::string, TestResult> allResults;
    for (const auto& suite : pImpl->suites) {
        auto suiteResults = suite->getTestResults();
        allResults.insert(suiteResults.begin(), suiteResults.end());
    }
    
    for (const auto& reporter : pImpl->reporters) {
        reporter->reportSuiteEnd("All Tests", allPassed);
        reporter->reportSummary(allResults);
    }
    
    return allPassed;
}

bool TestFramework::runSuite(const std::string& suiteName) {
    auto suite = getSuite(suiteName);
    if (!suite) {
        std::cerr << "Test suite not found: " << suiteName << std::endl;
        return false;
    }
    
    return suite->runAllTests();
}

bool TestFramework::runTests(const std::vector<std::string>& testNames) {
    bool allPassed = true;
    
    for (const auto& testName : testNames) {
        bool testPassed = false;
        
        // Find and run the specific test
        for (const auto& suite : pImpl->suites) {
            auto testCase = suite->getTestCase(testName);
            if (testCase) {
                testPassed = testCase->run();
                allPassed = allPassed && testPassed;
                break;
            }
        }
        
        if (!testPassed) {
            std::cerr << "Test not found or failed: " << testName << std::endl;
        }
    }
    
    return allPassed;
}

bool TestFramework::isRunning() const {
    return pImpl->running;
}

void TestFramework::stopRunning() {
    pImpl->running = false;
}

void TestFramework::pauseRunning() {
    // Implementation for pausing tests
}

void TestFramework::resumeRunning() {
    // Implementation for resuming tests
}

std::string TestFramework::getFullReport() const {
    std::stringstream ss;
    ss << "=== Test Framework Report ===\n";
    ss << "Total suites: " << pImpl->suites.size() << "\n";
    
    size_t totalTests = 0;
    size_t passedTests = 0;
    size_t failedTests = 0;
    size_t skippedTests = 0;
    size_t errorTests = 0;
    
    for (const auto& suite : pImpl->suites) {
        auto results = suite->getTestResults();
        totalTests += results.size();
        
        for (const auto& pair : results) {
            switch (pair.second) {
                case TestResult::PASSED:
                    passedTests++;
                    break;
                case TestResult::FAILED:
                    failedTests++;
                    break;
                case TestResult::SKIPPED:
                    skippedTests++;
                    break;
                case TestResult::ERROR:
                    errorTests++;
                    break;
                case TestResult::TIMEOUT:
                    failedTests++;
                    break;
            }
        }
    }
    
    ss << "Total tests: " << totalTests << "\n";
    ss << "Passed: " << passedTests << "\n";
    ss << "Failed: " << failedTests << "\n";
    ss << "Skipped: " << skippedTests << "\n";
    ss << "Errors: " << errorTests << "\n";
    
    double executionTime = std::chrono::duration<double>(pImpl->endTime - pImpl->startTime).count();
    ss << "Total execution time: " << executionTime << " seconds\n";
    
    if (passedTests == totalTests) {
        ss << "\n=== ALL TESTS PASSED ===\n";
    } else {
        ss << "\n=== SOME TESTS FAILED ===\n";
    }
    
    return ss.str();
}

std::string TestFramework::getSuiteReport(const std::string& suiteName) const {
    auto suite = getSuite(suiteName);
    if (!suite) {
        return "Test suite not found: " + suiteName;
    }
    
    return suite->getName() + " Report:\n" + suite->getTestResults();
}

std::string TestFramework::getTestReport(const std::string& suiteName, const std::string& testName) const {
    auto suite = getSuite(suiteName);
    if (!suite) {
        return "Test suite not found: " + suiteName;
    }
    
    auto testCase = suite->getTestCase(testName);
    if (!testCase) {
        return "Test not found: " + testName;
    }
    
    return testName + " Report: " + testCase->getTestResults();
}

bool TestFramework::loadTestSuites(const std::string& directory) {
    bool success = true;
    
    if (!std::filesystem::exists(directory)) {
        std::cerr << "Test directory does not exist: " << directory << std::endl;
        return false;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string suiteName = entry.path().stem().string();
            
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                std::cerr << "Failed to open test suite file: " << suiteName << std::endl;
                success = false;
                continue;
            }
            
            try {
                nlohmann::json config = nlohmann::json::parse(file);
                
                // Create test suite based on configuration
                std::shared_ptr<TestSuite> suite;
                if (config.contains("neuron_tests")) {
                    suite = std::make_shared<NeuronTestSuite>();
                } else if (config.contains("synapse_tests")) {
                    suite = std::make_shared<SynapseTestSuite>();
                } else if (config.contains("plasticity_tests")) {
                    suite = std::make_shared<PlasticityTestSuite>();
                } else if (config.contains("memory_tests")) {
                    suite = std::make_shared<MemoryTestSuite>();
                } else if (config.contains("neuromodulation_tests")) {
                    suite = std::make_shared<NeuromodulationTestSuite>();
                } else if (config.contains("brain_integration_tests")) {
                    suite = std::make_shared<BrainIntegrationTestSuite>();
                } else if (config.contains("performance_tests")) {
                    suite = std::make_shared<PerformanceTestSuite>();
                } else if (config.contains("environment_tests")) {
                    suite = std::make_shared<EnvironmentTestSuite>();
                } else if (config.contains("system_tests")) {
                    suite = std::make_shared<SystemTestSuite>();
                } else {
                    std::cerr << "Unknown test suite in file: " << suiteName << std::endl;
                    success = false;
                    continue;
                }
                
                // Setup test suite
                if (suite->setup()) {
                    addSuite(suite);
                } else {
                    std::cerr << "Failed to setup test suite: " << suiteName << std::endl;
                    success = false;
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Error loading test suite from " << suiteName << ": " << e.what() << std::endl;
                success = false;
            }
        }
    }
    
    return success;
}

bool TestFramework::exportTestResults(const std::string& path, const std::string& format) {
    std::filesystem::create_directories(path);
    
    if (format == "text") {
        std::ofstream file(path + "/test_results.txt");
        if (!file.is_open()) {
            return false;
        }
        
        file << getFullReport();
        file.close();
    } else if (format == "json") {
        nlohmann::json jsonReport;
        
        jsonReport["framework"] = "NLM Test Framework";
        jsonReport["version"] = "1.0.0";
        jsonReport["total_suites"] = pImpl->suites.size();
        
        // Collect test results
        nlohmann::json suitesJson = nlohmann::json::array();
        for (const auto& suite : pImpl->suites) {
            nlohmann::json suiteJson;
            suiteJson["name"] = suite->getName();
            suiteJson["description"] = suite->getDescription();
            suiteJson["level"] = static_cast<int>(suite->getLevel());
            
            auto testResults = suite->getTestResults();
            nlohmann::json testsJson = nlohmann::json::array();
            for (const auto& pair : testResults) {
                nlohmann::json testJson;
                testJson["name"] = pair.first;
                testJson["result"] = static_cast<int>(pair.second);
                testJson["level"] = static_cast<int>(TestLevel::UNIT);  // Would need to be obtained from test case
                testsJson.push_back(testJson);
            }
            
            suiteJson["tests"] = testsJson;
            suitesJson.push_back(suiteJson);
        }
        
        jsonReport["suites"] = suitesJson;
        
        std::ofstream file(path + "/test_results.json");
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << jsonReport;
        file.close();
    } else if (format == "csv") {
        std::ofstream file(path + "/test_results.csv");
        if (!file.is_open()) {
            return false;
        }
        
        file << "Suite,Test,Result,Level,Execution Time\n";
        
        for (const auto& suite : pImpl->suites) {
            auto testResults = suite->getTestResults();
            for (const auto& pair : testResults) {
                file << suite->getName() << ","
                     << pair.first << ","
                     << toString(pair.second) << ","
                     << static_cast<int>(TestLevel::UNIT) << ","
                     << "0.0\n";  // Would need to get actual execution time
            }
        }
        
        file.close();
    }
    
    return true;
}

void TestFramework::setMaxExecutionTime(double seconds) {
    pImpl->maxExecutionTime = seconds;
}

void TestFramework::setMemoryLimit(size_t bytes) {
    pImpl->memoryLimit = bytes;
}

void TestFramework::setThreadCount(size_t count) {
    pImpl->threadCount = count;
}

std::string TestFramework::getFrameworkInfo() const {
    std::stringstream ss;
    ss << "=== NLM Test Framework ===\n";
    ss << "Version: 1.0.0\n";
    ss << "Total test suites: " << pImpl->suites.size() << "\n";
    ss << "Max execution time: " << pImpl->maxExecutionTime << " seconds\n";
    ss << "Memory limit: " << pImpl->memoryLimit << " bytes\n";
    ss << "Thread count: " << pImpl->threadCount << "\n";
    ss << "Available test categories:\n";
    
    ss << "1. Neuron Tests - Tests for neuron functionality and properties\n";
    ss << "2. Synapse Tests - Tests for synapse functionality and properties\n";
    ss << "3. Plasticity Tests - Tests for plasticity rules and learning\n";
    ss << "4. Memory Tests - Tests for memory systems and functions\n";
    ss << "5. Neuromodulation Tests - Tests for neuromodulation systems and signaling\n";
    ss << "6. Brain Integration Tests - Tests for integrated brain functionality\n";
    ss << "7. Performance Tests - Performance and scalability tests\n";
    ss << "8. Environment Tests - Tests for environment interaction and adaptation\n";
    ss << "9. System Tests - System integration and end-to-end tests\n";
    
    ss << "\nUsage:\n";
    ss << "  ./run_tests [options]\n";
    ss << "  --suite=<name> Run specific test suite\n";
    ss << "  --test=<name> Run specific test\n";
    ss << "  --output=<format> Export results in specified format (text, json, csv)\n";
    ss << "  --output=<path> Save results to specified path\n";
    ss << "  --help Display this help message\n";
    
    return ss.str();
}

// Implementation of test suite base methods
bool NeuronTestSuite::setup() {
    // Initialize neuron test fixtures
    return true;
}

bool NeuronTestSuite::teardown() {
    // Cleanup neuron test fixtures
    return true;
}

bool SynapseTestSuite::setup() {
    // Initialize synapse test fixtures
    return true;
}

bool SynapseTestSuite::teardown() {
    // Cleanup synapse test fixtures
    return true;
}

bool PlasticityTestSuite::setup() {
    // Initialize plasticity test fixtures
    return true;
}

bool PlasticityTestSuite::teardown() {
    // Cleanup plasticity test fixtures
    return true;
}

bool MemoryTestSuite::setup() {
    // Initialize memory test fixtures
    return true;
}

bool MemoryTestSuite::teardown() {
    // Cleanup memory test fixtures
    return true;
}

bool NeuromodulationTestSuite::setup() {
    // Initialize neuromodulation test fixtures
    return true;
}

bool NeuromodulationTestSuite::teardown() {
    // Cleanup neuromodulation test fixtures
    return true;
}

bool BrainIntegrationTestSuite::setup() {
    // Initialize brain integration test fixtures
    return true;
}

bool BrainIntegrationTestSuite::teardown() {
    // Cleanup brain integration test fixtures
    return true;
}

bool PerformanceTestSuite::setup() {
    // Initialize performance test fixtures
    return true;
}

bool PerformanceTestSuite::teardown() {
    // Cleanup performance test fixtures
    return true;
}

bool EnvironmentTestSuite::setup() {
    // Initialize environment test fixtures
    return true;
}

bool EnvironmentTestSuite::teardown() {
    // Cleanup environment test fixtures
    return true;
}

bool SystemTestSuite::setup() {
    // Initialize system test fixtures
    return true;
}

bool SystemTestSuite::teardown() {
    // Cleanup system test fixtures
    return true;
}

} // namespace nlm
