// IntegrationTest.cpp - Implementation of NLM Performance Profiling Test Suite
// Comprehensive tests for all profiling components

#include "IntegrationTest.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <pugixml.hpp>
#include <cmath>
#include <set>
#include <queue>
#include <regex>
#include <numeric>

namespace nlm {

// TestHelper implementation
TestHelper::TestHelper() {
}

double TestHelper::randomDouble(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

int64_t TestHelper::randomInt64(int64_t min, int64_t max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> dis(min, max);
    return dis(gen);
}

uint64_t TestHelper::randomUint64(uint64_t min, uint64_t max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(min, max);
    return dis(gen);
}

size_t TestHelper::randomSize(size_t min, size_t max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(min, max);
    return dis(gen);
}

std::string TestHelper::randomString(size_t length) {
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, charset.size() - 1);
    
    std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }
    
    return result;
}

bool TestHelper::stringsEqualIgnoreCase(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(a[i]) != std::tolower(b[i])) {
            return false;
        }
    }
    
    return true;
}

std::string TestHelper::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

std::string TestHelper::toUpper(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}

bool TestHelper::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string TestHelper::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

bool TestHelper::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string TestHelper::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool TestHelper::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

void TestHelper::createDirectory(const std::string& path) {
    std::filesystem::create_directories(path);
}

double TestHelper::mean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double TestHelper::variance(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    if (mean == 0.0) mean = TestHelper::mean(values);
    
    double sumSquaredDiff = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSquaredDiff += diff * diff;
    }
    
    return sumSquaredDiff / values.size();
}

double TestHelper::standardDeviation(const std::vector<double>& values, double mean) {
    return std::sqrt(variance(values, mean));
}

double TestHelper::median(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
    } else {
        return sorted[n/2];
    }
}

double TestHelper::percentile(const std::vector<double>& values, double percentile) {
    if (values.empty()) return 0.0;
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    double position = (percentile / 100.0) * (sorted.size() - 1);
    size_t lowerIndex = static_cast<size_t>(std::floor(position));
    size_t upperIndex = static_cast<size_t>(std::ceil(position));
    
    if (lowerIndex == upperIndex) {
        return sorted[lowerIndex];
    }
    
    double weight = position - lowerIndex;
    return sorted[lowerIndex] * (1.0 - weight) + sorted[upperIndex] * weight;
}

double TestHelper::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumXX = 0.0;
    double sumYY = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += x[i] * y[i];
        sumXX += x[i] * x[i];
        sumYY += y[i] * y[i];
    }
    
    double numerator = (x.size() * sumXY - sumX * sumY);
    double denominator = std::sqrt((x.size() * sumXX - sumX * sumX) * (x.size() * sumYY - sumY * sumY));
    
    if (denominator == 0) return 0.0;
    
    return numerator / denominator;
}

std::string TestHelper::formatTimeNs(double timeNs) {
    std::stringstream ss;
    if (timeNs >= 1e9) {
        ss << std::fixed << std::setprecision(3) << (timeNs / 1e9) << "s";
    } else if (timeNs >= 1e6) {
        ss << std::fixed << std::setprecision(3) << (timeNs / 1e6) << "ms";
    } else if (timeNs >= 1e3) {
        ss << std::fixed << std::setprecision(3) << (timeNs / 1e3) << "us";
    } else {
        ss << std::fixed << std::setprecision(3) << timeNs << "ns";
    }
    return ss.str();
}

std::string TestHelper::formatMemoryBytes(size_t bytes) {
    std::stringstream ss;
    if (bytes >= 1024 * 1024 * 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / (1024 * 1024 * 1024)) << "GB";
    } else if (bytes >= 1024 * 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / (1024 * 1024)) << "MB";
    } else if (bytes >= 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / 1024) << "KB";
    } else {
        ss << bytes << "B";
    }
    return ss.str();
}

std::string TestHelper::formatPercentage(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << value << "%";
    return ss.str();
}

bool TestHelper::isWithinRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool TestHelper::isWithinRange(int64_t value, int64_t min, int64_t max) {
    return value >= min && value <= max;
}

bool TestHelper::isWithinRange(size_t value, size_t min, size_t max) {
    return value >= min && value <= max;
}

bool TestHelper::stringsAreEqualIgnoringWhitespace(const std::string& a, const std::string& b) {
    std::string aClean, bClean;
    for (char c : a) {
        if (!std::isspace(c)) aClean += c;
    }
    for (char c : b) {
        if (!std::isspace(c)) bClean += c;
    }
    return aClean == bClean;
}

// TestBase implementation
std::string TestBase::runTest() {
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    
    if (!setup()) {
        return "FAILED: Setup failed";
    }
    
    std::chrono::steady_clock::time_point setupEnd = std::chrono::steady_clock::now();
    
    if (!execute()) {
        teardown();
        return "FAILED: Test execution failed";
    }
    
    std::chrono::steady_clock::time_point executionEnd = std::chrono::steady_clock::now();
    
    if (!teardown()) {
        return "FAILED: Teardown failed";
    }
    
    std::chrono::steady_clock::time_point teardownEnd = std::chrono::steady_clock::now();
    
    auto setupDuration = std::chrono::duration_cast<std::chrono::milliseconds>(setupEnd - startTime);
    auto executionDuration = std::chrono::duration_cast<std::chrono::milliseconds>(executionEnd - setupEnd);
    auto teardownDuration = std::chrono::duration_cast<std::chrono::milliseconds>(teardownEnd - executionEnd);
    
    std::stringstream ss;
    ss << "PASSED: Setup: " << setupDuration.count() << "ms, "
       << "Execution: " << executionDuration.count() << "ms, "
       << "Teardown: " << teardownDuration.count() << "ms";
    
    return ss.str();
}

// PerformanceProfilerTest implementation
bool PerformanceProfilerTest::setup() {
    try {
        ProfileConfig config;
        config.name = "test_profiler";
        config.enabled = true;
        config.realtime = false;
        config.samplingIntervalMs = 1.0;
        config.maxHistory = 1000;
        
        profiler_ = std::make_shared<NLPerformanceProfiler>("test_profiler", config);
        
        if (!profiler_->isRunning()) {
            profiler_->start();
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool PerformanceProfilerTest::execute() {
    if (!profiler_ || !profiler_->isRunning()) {
        return false;
    }
    
    try {
        // Test basic profiler functionality
        profiler_->recordEvent(ProfileEventType::TIMESTEP_END, "test.cpp", 1, "testFunction");
        
        // Test brain update
        profiler_->onBrainUpdate(1, 100, 5.0);
        
        // Test neuron event
        profiler_->onNeuronEvent("spike", "neuron_1", 1.5, 1000.0);
        
        // Test synapse event
        profiler_->onSynapseEvent("update", 1, 2, 0.5, 2000.0);
        
        // Test session functionality
        auto session = profiler_->getSession();
        if (!session) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool PerformanceProfilerTest::teardown() {
    if (profiler_ && profiler_->isRunning()) {
        profiler_->stop();
    }
    return true;
}

// PerformanceMonitorTest implementation
bool PerformanceMonitorTest::setup() {
    try {
        PerformanceMonitorConfig config;
        config.name = "test_monitor";
        config.enabled = true;
        config.samplingIntervalMs = 1.0;
        config.historySize = 1000;
        config.monitorCPU = true;
        config.monitorMemory = true;
        config.monitorThreads = true;
        config.monitorSimulation = true;
        
        monitor_ = std::make_shared<PerformanceMonitor>("test_monitor", config);
        
        if (!monitor_->isRunning()) {
            monitor_->start();
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool PerformanceMonitorTest::execute() {
    if (!monitor_ || !monitor_->isRunning()) {
        return false;
    }
    
    try {
        // Test monitor update
        monitor_->update(1000000, 10, 5, 100, 50, 0);
        
        // Test memory allocation tracking
        monitor_->recordMemoryAllocation(1024, "test.cpp", 1, "testFunction", false, 0);
        monitor_->recordMemoryDeallocation(512, "test.cpp", 1, "testFunction", false, 0);
        
        // Test thread metrics
        ThreadPerformanceMetrics threadMetrics("test_thread", 1);
        threadMetrics.activeTimePercent = 100.0;
        monitor_->updateThreadMetrics(threadMetrics);
        
        // Test I/O metrics
        IOMetrics ioMetrics;
        ioMetrics.readBytes = 1024;
        ioMetrics.writeBytes = 512;
        monitor_->updateIOMetrics(ioMetrics);
        
        // Test bottleneck detection
        std::string bottlenecks = monitor_->analyzeBottlenecks();
        
        // Test metrics calculation
        PerformanceMetrics metrics = monitor_->calculateCurrentMetrics();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool PerformanceMonitorTest::teardown() {
    if (monitor_ && monitor_->isRunning()) {
        monitor_->stop();
    }
    return true;
}

// ProfilingDataCollectorTest implementation
bool ProfilingDataCollectorTest::setup() {
    try {
        DataCollectionConfig config;
        config.name = "test_collector";
        config.enabled = true;
        config.samplingIntervalMs = 10.0;
        config.maxDataPoints = 10000;
        config.enableRealTime = true;
        config.enablePostMortem = true;
        config.threadSafe = true;
        
        dataCollector_ = std::make_shared<ProfilingDataCollector>("test_collector", config);
        
        if (!dataCollector_->isRunning()) {
            dataCollector_->start();
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ProfilingDataCollectorTest::execute() {
    if (!dataCollector_ || !dataCollector_->isRunning()) {
        return false;
    }
    
    try {
        // Test event recording
        ProfileDataPoint event;
        event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        event.eventType = ProfileEventType::TIMESTEP_END;
        event.sourceFile = "test.cpp";
        event.sourceLine = 1;
        event.functionName = "testFunction";
        event.threadId = "main_thread";
        event.eventData["test"] = "value";
        
        dataCollector_->recordEvent(event);
        
        // Test timestep recording
        TimestepPerformance ts(1);
        ts.wallTimeNs = 1000000;
        ts.neuronUpdates = 10;
        ts.synapseUpdates = 5;
        ts.spikesGenerated = 5;
        ts.spikesPropagated = 3;
        
        dataCollector_->recordTimestepEvent(ts);
        
        // Test memory allocation recording
        MemoryAllocationInfo alloc;
        alloc.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        alloc.size = 1024;
        alloc.file = "test.cpp";
        alloc.line = 1;
        alloc.function = "testFunction";
        alloc.threadId = "main_thread";
        alloc.isPool = false;
        alloc.poolId = 0;
        
        dataCollector_->recordMemoryAllocation(alloc);
        
        // Test thread metrics recording
        ThreadPerformanceMetrics threadMetrics("test_thread", 1);
        dataCollector_->recordThreadMetrics(threadMetrics);
        
        // Test I/O metrics recording
        IOMetrics ioMetrics;
        ioMetrics.readBytes = 1024;
        ioMetrics.writeBytes = 512;
        dataCollector_->recordIOMetrics(ioMetrics);
        
        // Test data access
        std::vector<ProfileDataPoint> events = dataCollector_->getEvents();
        std::vector<TimestepPerformance> timesteps = dataCollector_->getTimestepData();
        std::vector<MemoryAllocationInfo> allocations = dataCollector_->getMemoryAllocations();
        std::vector<ThreadPerformanceMetrics> threads = dataCollector_->getThreadData();
        std::vector<IOMetrics> io = dataCollector_->getIOData();
        
        return !events.empty() && !timesteps.empty();
    } catch (const std::exception& e) {
        return false;
    }
}

bool ProfilingDataCollectorTest::teardown() {
    if (dataCollector_ && dataCollector_->isRunning()) {
        dataCollector_->stop();
    }
    return true;
}

// NLPerformanceAnalyzerTest implementation
bool NLPerformanceAnalyzerTest::setup() {
    try {
        analyzer_ = std::make_shared<NLPerformanceAnalyzer>("test_analyzer");
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NLPerformanceAnalyzerTest::execute() {
    if (!analyzer_) {
        return false;
    }
    
    try {
        // Create test sessions
        std::vector<ProfileSession> sessions;
        
        ProfileSession session1;
        session1.id = "session_1";
        session1.config.name = "Test Session 1";
        session1.startTimestamp = 1000000000000;
        session1.endTimestamp = 1000010000000;
        session1.metrics.avgStepTimeMs = 5.0;
        session1.metrics.cpuUsagePercent = 50.0;
        session1.metrics.totalMemoryBytes = 100 * 1024 * 1024;
        session1.metrics.spikesPerSecond = 500;
        sessions.push_back(session1);
        
        ProfileSession session2;
        session2.id = "session_2";
        session2.config.name = "Test Session 2";
        session2.startTimestamp = 1000020000000;
        session2.endTimestamp = 1000030000000;
        session2.metrics.avgStepTimeMs = 4.5;
        session2.metrics.cpuUsagePercent = 45.0;
        session2.metrics.totalMemoryBytes = 90 * 1024 * 1024;
        session2.metrics.spikesPerSecond = 550;
        sessions.push_back(session2);
        
        // Test bottleneck analysis
        auto bottlenecks = analyzer_->analyzeBottlenecks(sessions);
        
        // Test performance comparison
        auto comparisons = analyzer_->compareRuns(sessions);
        
        // Test trend analysis
        auto trends = analyzer_->analyzeTrends(sessions, "avg_step_time");
        
        // Test automated testing
        auto tests = analyzer_->runAutomatedTests(BenchmarkConfig());
        
        // Test regression analysis
        std::string regressions = analyzer_->analyzeRegressions(sessions);
        
        // Test improvement analysis
        std::string improvements = analyzer_->analyzeImprovements(sessions);
        
        // Test report generation
        AnalysisReportConfig reportConfig;
        reportConfig.name = "test_report";
        reportConfig.outputFormat = "json";
        reportConfig.generateSummary = true;
        reportConfig.generateDetailed = false;
        
        AnalysisReport report = analyzer_->generateReport(reportConfig, sessions);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NLPerformanceAnalyzerTest::teardown() {
    // Cleanup
    return true;
}

// ExportIntegrationTest implementation
bool ExportIntegrationTest::setup() {
    try {
        exportManager_ = std::make_shared<ExportIntegrationManager>("test_export_manager");
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ExportIntegrationTest::execute() {
    if (!exportManager_) {
        return false;
    }
    
    try {
        // Test JSON export
        std::string jsonData = "{\"test\": \"data\"}";
        ExportConfig jsonConfig;
        jsonConfig.name = "test_json";
        jsonConfig.enabled = true;
        jsonConfig.format = ExportFormat::JSON;
        jsonConfig.outputPath = "./test_output.json";
        
        ExportResult jsonResult = exportManager_->exportData(jsonData, jsonConfig);
        
        // Test CSV export
        std::string csvData = "name,value\ntest,1\n";
        ExportConfig csvConfig;
        csvConfig.name = "test_csv";
        csvConfig.enabled = true;
        csvConfig.format = ExportFormat::CSV;
        csvConfig.outputPath = "./test_output.csv";
        
        ExportResult csvResult = exportManager_->exportData(csvData, csvConfig);
        
        // Test YAML export
        std::string yamlData = "test: data\n";
        ExportConfig yamlConfig;
        yamlConfig.name = "test_yaml";
        yamlConfig.enabled = true;
        yamlConfig.format = ExportFormat::YAML;
        yamlConfig.outputPath = "./test_output.yaml";
        
        ExportResult yamlResult = exportManager_->exportData(yamlData, yamlConfig);
        
        // Test XML export
        std::string xmlData = "<root><test>data</test></root>";
        ExportConfig xmlConfig;
        xmlConfig.name = "test_xml";
        xmlConfig.enabled = true;
        xmlConfig.format = ExportFormat::XML;
        xmlConfig.outputPath = "./test_output.xml";
        
        ExportResult xmlResult = exportManager_->exportData(xmlData, xmlConfig);
        
        // Test integration
        IntegrationResult integrationResult = exportManager_->integrateData(jsonData, "test_integration_config");
        
        return jsonResult.status == "completed" && csvResult.status == "completed" &&
               yamlResult.status == "completed" && xmlResult.status == "completed" &&
               integrationResult.status == "completed";
    } catch (const std::exception& e) {
        return false;
    }
}

bool ExportIntegrationTest::teardown() {
    // Cleanup
    return true;
}

// AdvancedProfilerTest implementation
bool AdvancedProfilerTest::setup() {
    try {
        advancedProfiler_ = std::make_shared<AdvancedProfilerManager>("test_advanced_profiler");
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedProfilerTest::execute() {
    if (!advancedProfiler_) {
        return false;
    }
    
    try {
        // Test advanced profiler setup
        advancedProfiler_->start();
        
        // Test dynamic profiling
        advancedProfiler_->evaluateConditions();
        
        // Test memory leak detection
        auto leakResult = advancedProfiler_->detectMemoryLeaks();
        
        // Test race condition detection
        auto raceResult = advancedProfiler_->detectRaceConditions();
        
        // Test CPU cache profiling
        auto cacheProfile = advancedProfiler_->profileCPUCache();
        
        // Test analysis
        std::string analysis = advancedProfiler_->analyzeSystemPerformance();
        
        // Test issues detection
        std::string issues = advancedProfiler_->detectSystemIssues();
        
        // Test optimization recommendations
        std::vector<std::string> recommendations = advancedProfiler_->getOptimizationRecommendations();
        
        advancedProfiler_->stop();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedProfilerTest::teardown() {
    if (advancedProfiler_ && advancedProfiler_->isRunning()) {
        advancedProfiler_->stop();
    }
    return true;
}

// NLMIntegrationManagerTest implementation
bool NLMIntegrationManagerTest::setup() {
    try {
        integrationManager_ = std::make_shared<NLMIntegrationManager>("test_integration_manager");
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NLMIntegrationManagerTest::execute() {
    if (!integrationManager_) {
        return false;
    }
    
    try {
        // Test component initialization
        integrationManager_->initializeComponents();
        
        // Test workflow management
        integrationManager_->startIntegrationWorkflow();
        
        // Test component status
        std::string status = integrationManager_->getManagerStatus();
        
        // Test configuration management
        std::string configFile = "./test_config.yaml";
        integrationManager_->saveConfigurationToFile(configFile);
        
        // Test error handling
        std::vector<std::string> errors = integrationManager_->getErrors();
        
        // Test health checks
        bool health = integrationManager_->performHealthCheck("test_component");
        
        // Test performance checks
        bool performance = integrationManager_->performPerformanceHealthCheck();
        
        // Test data integrity checks
        bool integrity = integrationManager_->performDataIntegrityCheck();
        
        integrationManager_->stopIntegrationWorkflow();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NLMIntegrationManagerTest::teardown() {
    if (integrationManager_ && integrationManager_->isRunning()) {
        integrationManager_->stopIntegrationWorkflow();
    }
    return true;
}

// DebugSystemIntegrationTest implementation
bool DebugSystemIntegrationTest::setup() {
    try {
        integrationManager_ = std::make_shared<NLMIntegrationManager>("test_debug_integration_manager");
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DebugSystemIntegrationTest::execute() {
    if (!integrationManager_) {
        return false;
    }
    
    try {
        // Test debug system integration
        integrationManager_->integrateWithProfiler(profiler_);
        integrationManager_->integrateWithMonitor(monitor_);
        integrationManager_->integrateWithDataCollector(dataCollector_);
        
        // Test logger integration
        Logger::getGlobal().info("Test debug system integration");
        
        // Test event handling
        integrationManager_->emitEvent(IntegrationEvent::SYSTEM_INITIALIZED, {
            {"message", "Debug system integration test"}
        });
        
        // Test health check
        bool healthy = integrationManager_->performHealthCheck("debug_system");
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DebugSystemIntegrationTest::teardown() {
    if (integrationManager_ && integrationManager_->isRunning()) {
        integrationManager_->stopIntegrationWorkflow();
    }
    return true;
}

// PerformanceTestSuite implementation
PerformanceTestSuite::PerformanceTestSuite() {
}

void PerformanceTestSuite::addTest(std::shared_ptr<TestBase> test) {
    tests_.push_back(test);
}

bool PerformanceTestSuite::runAllTests() {
    std::cout << "Running test suite: " << getTestSuiteName() << std::endl;
    
    bool allPassed = true;
    for (const auto& test : tests_) {
        if (test->isEnabled()) {
            TestSuiteResult result = runTest(test);
            results_.push_back(result);
            
            if (!result.passedAll) {
                allPassed = false;
            }
        }
    }
    
    return allPassed;
}

bool PerformanceTestSuite::runTestSuite(const std::string& suiteName) {
    std::cout << "Running test suite: " << suiteName << std::endl;
    
    bool allPassed = true;
    for (const auto& test : tests_) {
        if (test->getTestSuite() == suiteName && test->isEnabled()) {
            TestSuiteResult result = runTest(test);
            results_.push_back(result);
            
            if (!result.passedAll) {
                allPassed = false;
            }
        }
    }
    
    return allPassed;
}

TestSuiteResult PerformanceTestSuite::getTestSuiteResult(const std::string& suiteName) const {
    TestSuiteResult result;
    result.suiteName = suiteName;
    
    for (const auto& test : tests_) {
        if (test->getTestSuite() == suiteName) {
            TestSuiteResult testResult = runTest(test);
            result.testResults.insert(result.testResults.end(), 
                                     testResult.testResults.begin(), 
                                     testResult.testResults.end());
            
            if (!testResult.passedAll) {
                result.passedAll = false;
            }
            
            result.passedCount += testResult.passedCount;
            result.failedCount += testResult.failedCount;
            result.skippedCount += testResult.skippedCount;
            result.totalDuration += testResult.totalDuration;
        }
    }
    
    return result;
}

std::vector<std::string> PerformanceTestSuite::getTestSuiteNames() const {
    std::vector<std::string> suiteNames;
    for (const auto& test : tests_) {
        if (std::find(suiteNames.begin(), suiteNames.end(), test->getTestSuite()) == suiteNames.end()) {
            suiteNames.push_back(test->getTestSuite());
        }
    }
    
    return suiteNames;
}

void PerformanceTestSuite::printTestResults() const {
    std::cout << "\n=== Test Results ===\n\n";
    
    for (const auto& result : results_) {
        std::cout << "Test Suite: " << result.suiteName << std::endl;
        std::cout << "Total Tests: " << result.testResults.size() << std::endl;
        std::cout << "Passed: " << result.passedCount << std::endl;
        std::cout << "Failed: " << result.failedCount << std::endl;
        std::cout << "Skipped: " << result.skippedCount << std::endl;
        std::cout << "Success Rate: " << (result.passedCount / static_cast<double>(result.testResults.size())) * 100.0 << "%\n\n";
        
        for (const auto& test : result.testResults) {
            std::cout << "  " << test.testName << ": " << (test.passed ? "PASSED" : "FAILED") 
                      << " (" << test.duration.count() << "ms)" << std::endl;
            if (!test.message.empty()) {
                std::cout << "    " << test.message << std::endl;
            }
        }
        
        std::cout << "\n";
    }
}

std::string PerformanceTestSuite::generateTestReport() const {
    std::stringstream ss;
    ss << "NLM Performance Profiling Test Report\n";
    ss << "====================================\n\n";
    
    ss << "Total Test Suites: " << results_.size() << "\n";
    ss << "Total Tests: " << getTotalTestsCount() << "\n";
    ss << "Passed Tests: " << getPassedTestsCount() << "\n";
    ss << "Failed Tests: " << getFailedTestsCount() << "\n";
    ss << "Skipped Tests: " << getSkippedTestsCount() << "\n";
    ss << "Success Rate: " << getSuccessRate() << "%\n\n";
    
    ss << "Detailed Results:\n";
    for (const auto& suite : results_) {
        ss << "Suite: " << suite.suiteName << "\n";
        ss << "  Total Tests: " << suite.testResults.size() << "\n";
        ss << "  Passed: " << suite.passedCount << "\n";
        ss << "  Failed: " << suite.failedCount << "\n";
        ss << "  Skipped: " << suite.skippedCount << "\n";
        ss << "  Total Time: " << suite.totalDuration.count() << "ms\n\n";
        
        for (const auto& test : suite.testResults) {
            ss << "  Test: " << test.testName << " - " << (test.passed ? "PASSED" : "FAILED") 
               << " (" << test.duration.count() << "ms)\n";
            if (!test.message.empty()) {
                ss << "    Message: " << test.message << "\n";
            }
            if (!test.errorDetails.empty()) {
                ss << "    Error: " << test.errorDetails << "\n";
            }
        }
        ss << "\n";
    }
    
    return ss.str();
}

bool PerformanceTestSuite::saveTestReport(const std::string& outputPath) const {
    std::string report = generateTestReport();
    return TestHelper::writeFile(outputPath, report);
}

void PerformanceTestSuite::clearResults() {
    results_.clear();
}

TestSuiteResult PerformanceTestSuite::runTest(TestBase* test) {
    TestSuiteResult result;
    result.suiteName = test->getTestSuite();
    result.passedAll = true;
    
    for (int i = 0; i < 2; ++i) { // Run each test twice for reliability
        TestResult testResult;
        testResult.testName = test->getTestName();
        testResult.testSuite = test->getTestSuite();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            if (test->execute()) {
                testResult.passed = true;
                result.passedCount++;
            } else {
                testResult.passed = false;
                result.failedCount++;
                result.passedAll = false;
                testResult.message = "Test execution failed";
            }
        } catch (const std::exception& e) {
            testResult.passed = false;
            result.failedCount++;
            result.passedAll = false;
            testResult.message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        testResult.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (test->teardown()) {
            testResult.message += " (Teardown successful)";
        } else {
            testResult.message += " (Teardown failed)";
        }
        
        result.testResults.push_back(testResult);
    }
    
    result.totalDuration = result.totalDuration + result.testResults.back().duration;
    
    return result;
}

size_t PerformanceTestSuite::getTotalTestsCount() const {
    size_t total = 0;
    for (const auto& test : tests_) {
        if (test->isEnabled()) {
            total++;
        }
    }
    return total;
}

size_t PerformanceTestSuite::getPassedTestsCount() const {
    size_t passed = 0;
    for (const auto& result : results_) {
        passed += result.passedCount;
    }
    return passed;
}

size_t PerformanceTestSuite::getFailedTestsCount() const {
    size_t failed = 0;
    for (const auto& result : results_) {
        failed += result.failedCount;
    }
    return failed;
}

size_t PerformanceTestSuite::getSkippedTestsCount() const {
    size_t skipped = 0;
    for (const auto& test : tests_) {
        if (!test->isEnabled()) {
            skipped++;
        }
    }
    return skipped;
}

double PerformanceTestSuite::getSuccessRate() const {
    size_t total = getTotalTestsCount();
    if (total == 0) return 0.0;
    return (static_cast<double>(getPassedTestsCount()) / total) * 100.0;
}

// IntegrationTestFactory implementation
std::shared_ptr<TestBase> IntegrationTestFactory::createTest(const std::string& testName) {
    if (testName == "PerformanceProfilerTest") {
        return std::make_shared<PerformanceProfilerTest>();
    } else if (testName == "PerformanceMonitorTest") {
        return std::make_shared<PerformanceMonitorTest>();
    } else if (testName == "ProfilingDataCollectorTest") {
        return std::make_shared<ProfilingDataCollectorTest>();
    } else if (testName == "NLPerformanceAnalyzerTest") {
        return std::make_shared<NLPerformanceAnalyzerTest>();
    } else if (testName == "ExportIntegrationTest") {
        return std::make_shared<ExportIntegrationTest>();
    } else if (testName == "AdvancedProfilerTest") {
        return std::make_shared<AdvancedProfilerTest>();
    } else if (testName == "NLMIntegrationManagerTest") {
        return std::make_shared<NLMIntegrationManagerTest>();
    } else if (testName == "DebugSystemIntegrationTest") {
        return std::make_shared<DebugSystemIntegrationTest>();
    }
    
    return nullptr;
}

std::shared_ptr<PerformanceTestSuite> IntegrationTestFactory::createTestSuite(const std::string& suiteName) {
    auto testSuite = std::make_shared<PerformanceTestSuite>();
    
    // Add tests to the test suite
    if (suiteName == "CoreInfrastructure") {
        testSuite->addTest(std::make_shared<PerformanceProfilerTest>());
    } else if (suiteName == "PerformanceMonitoring") {
        testSuite->addTest(std::make_shared<PerformanceMonitorTest>());
    } else if (suiteName == "DataCollection") {
        testSuite->addTest(std::make_shared<ProfilingDataCollectorTest>());
    } else if (suiteName == "AnalysisAndReporting") {
        testSuite->addTest(std::make_shared<NLPerformanceAnalyzerTest>());
    } else if (suiteName == "ExportAndIntegration") {
        testSuite->addTest(std::make_shared<ExportIntegrationTest>());
    } else if (suiteName == "AdvancedFeatures") {
        testSuite->addTest(std::make_shared<AdvancedProfilerTest>());
    } else if (suiteName == "Integration") {
        testSuite->addTest(std::make_shared<NLMIntegrationManagerTest>());
    } else if (suiteName == "DebugIntegration") {
        testSuite->addTest(std::make_shared<DebugSystemIntegrationTest>());
    }
    
    return testSuite;
}

std::vector<std::string> IntegrationTestFactory::getAvailableTests() {
    return {
        "PerformanceProfilerTest",
        "PerformanceMonitorTest",
        "ProfilingDataCollectorTest",
        "NLPerformanceAnalyzerTest",
        "ExportIntegrationTest",
        "AdvancedProfilerTest",
        "NLMIntegrationManagerTest",
        "DebugSystemIntegrationTest"
    };
}

std::vector<std::string> IntegrationTestFactory::getAvailableTestSuites() {
    return {
        "CoreInfrastructure",
        "PerformanceMonitoring",
        "DataCollection",
        "AnalysisAndReporting",
        "ExportAndIntegration",
        "AdvancedFeatures",
        "Integration",
        "DebugIntegration"
    };
}

} // namespace nlm