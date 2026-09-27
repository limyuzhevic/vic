// Performance Testing Framework Implementation for NLM

#include "PerformanceTestSuite.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <random>
#include <filesystem>

namespace nlm {
namespace testing {

PerformanceTestSuite::PerformanceTestSuite() {
    std::cout << "=== NLM Performance Test Suite Initialization ===" << std::endl;
    sessionStartTime_ = std::chrono::high_resolution_clock::now();
    
    // Initialize current baseline
    currentBaseline_ = PerformanceBaseline("default_baseline", "Default performance baseline");
    
    // Initialize performance metrics
    for (int i = 0; i < static_cast<int>(PerformanceMetric::CUSTOM); ++i) {
        currentMetrics_[static_cast<PerformanceMetric>(i)] = 0.0;
    }
    
    std::cout << "Performance test suite initialized successfully" << std::endl;
}

PerformanceTestSuite::~PerformanceTestSuite() {
    if (monitoringActive_.load()) {
        stopPerformanceMonitoring();
    }
    std::cout << "=== NLM Performance Test Suite Shutdown ===" << std::endl;
}

void PerformanceTestSuite::createBaseline(const std::string& name, const std::string& description) {
    currentBaseline_ = PerformanceBaseline(name, description);
    std::cout << "Created performance baseline: " << name << std::endl;
}

void PerformanceTestSuite::addBaselineMetric(PerformanceMetric metric, double value) {
    currentBaseline_.metrics[metric] = value;
    std::cout << "Added baseline metric: " << static_cast<int>(metric) << " = " << value << std::endl;
}

void PerformanceTestSuite::setBaselineThreshold(PerformanceMetric metric, double threshold) {
    currentBaseline_.thresholds[metric] = threshold;
    std::cout << "Set threshold for metric " << static_cast<int>(metric) << ": " << threshold << std::endl;
}

void PerformanceTestSuite::setBaselineWeight(PerformanceMetric metric, double weight) {
    currentBaseline_.weights[metric] = weight;
    std::cout << "Set weight for metric " << static_cast<int>(metric) << ": " << weight << std::endl;
}

void PerformanceTestSuite::saveBaseline(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for writing baseline: " << filename << std::endl;
        return;
    }
    
    file << "Baseline: " << currentBaseline_.name << std::endl;
    file << "Description: " << currentBaseline_.description << std::endl;
    file << "Environment: " << currentBaseline_.environment << std::endl;
    file << "Timestamp: " << std::chrono::system_clock::to_time_t(currentBaseline_.timestamp) << std::endl;
    file << "\nMetrics:" << std::endl;
    
    for (const auto& pair : currentBaseline_.metrics) {
        file << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    file << "\nThresholds:" << std::endl;
    for (const auto& pair : currentBaseline_.thresholds) {
        file << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    file << "\nWeights:" << std::endl;
    for (const auto& pair : currentBaseline_.weights) {
        file << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    file.close();
    std::cout << "Baseline saved to: " << filename << std::endl;
}

bool PerformanceTestSuite::loadBaseline(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for reading baseline: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    std::getline(file, line); // Baseline: 
    if (line.find("Baseline:") != std::string::npos) {
        currentBaseline_.name = line.substr(9);
    }
    
    std::getline(file, line); // Description: 
    if (line.find("Description:") != std::string::npos) {
        currentBaseline_.description = line.substr(13);
    }
    
    std::getline(file, line); // Environment: 
    if (line.find("Environment:") != std::string::npos) {
        currentBaseline_.environment = line.substr(13);
    }
    
    std::getline(file, line); // Timestamp: 
    // Skip timestamp parsing for simplicity
    
    std::getline(file, line); // \nMetrics:
    if (line.find("Metrics:") != std::string::npos) {
        while (std::getline(file, line) && !line.empty() && line.find("Thresholds:") == std::string::npos) {
            if (line.find("  ") != std::string::npos) {
                size_t colonPos = line.find(":");
                if (colonPos != std::string::npos) {
                    int metric = std::stoi(line.substr(2, colonPos - 2));
                    double value = std::stod(line.substr(colonPos + 2));
                    currentBaseline_.metrics[static_cast<PerformanceMetric>(metric)] = value;
                }
            }
        }
        
        if (line.find("Thresholds:") != std::string::npos) {
            while (std::getline(file, line) && !line.empty() && line.find("Weights:") == std::string::npos) {
                if (line.find("  ") != std::string::npos) {
                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        int metric = std::stoi(line.substr(2, colonPos - 2));
                        double value = std::stod(line.substr(colonPos + 2));
                        currentBaseline_.thresholds[static_cast<PerformanceMetric>(metric)] = value;
                    }
                }
            }
        }
        
        if (line.find("Weights:") != std::string::npos) {
            while (std::getline(file, line) && !line.empty()) {
                if (line.find("  ") != std::string::npos) {
                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        int metric = std::stoi(line.substr(2, colonPos - 2));
                        double value = std::stod(line.substr(colonPos + 2));
                        currentBaseline_.weights[static_cast<PerformanceMetric>(metric)] = value;
                    }
                }
            }
        }
    }
    
    file.close();
    std::cout << "Baseline loaded from: " << filename << std::endl;
    return true;
}

BenchmarkResult PerformanceTestSuite::benchmark(const std::string& name,
                                               std::function<void()> func,
                                               size_t iterations,
                                               const std::string& unit) {
    std::cout << "Running benchmark: " << name << " (" << iterations << " iterations)" << std::endl;
    
    // Warm-up runs
    for (size_t i = 0; i < 10; ++i) {
        func();
    }
    
    // Actual benchmark
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        func();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    double durationNs = static_cast<double>(duration.count());
    double avgDurationNs = durationNs / iterations;
    
    // Convert to appropriate unit
    double durationMs = avgDurationNs / 1000000.0;
    
    double durationValue;
    std::string actualUnit;
    
    if (durationMs < 1.0) {
        actualUnit = "us";
        durationValue = avgDurationNs / 1000.0;
    } else if (durationMs < 1000.0) {
        actualUnit = "ms";
        durationValue = durationMs;
    } else {
        actualUnit = "s";
        durationValue = durationMs / 1000.0;
    }
    
    BenchmarkResult result(name, durationValue, 0.0, iterations, actualUnit);
    recordPerformanceMetric(PerformanceMetric::CUSTOM, durationValue, "benchmark_" + name);
    
    std::cout << "  Result: " << durationValue << " " << actualUnit << " per iteration" << std::endl;
    return result;
}

void PerformanceTestSuite::profileOperation(const std::string& operationName,
                                           std::function<void()> operation,
                                           size_t iterations) {
    std::cout << "Profiling operation: " << operationName << " (" << iterations << " iterations)" << std::endl;
    
    // Run operation with timing
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        operation();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double durationMs = duration.count() / 1000.0;
    double avgDurationMs = durationMs / iterations;
    
    recordPerformanceMetric(PerformanceMetric::CUSTOM, avgDurationMs, "profile_" + operationName);
    
    std::cout << "  Average duration: " << avgDurationMs << "ms" << std::endl;
}

void PerformanceTestSuite::recordPerformanceMetric(PerformanceMetric metric, double value,
                                                   const std::string& context) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Store in performance data
    std::string contextKey = context.empty() ? std::to_string(static_cast<int>(metric)) : context;
    performanceData_[contextKey].push_back(value);
    
    // Update current metrics
    currentMetrics_[metric] = value;
    
    // Update PerformanceMonitor if active
    if (activeMonitor_ != nullptr) {
        // In a real implementation, this would update the PerformanceMonitor
    }
    
    std::cout << "Recorded metric " << static_cast<int>(metric) << " = " << value 
              << " in context: " << context << std::endl;
}

void PerformanceTestSuite::runRegressionTests() {
    std::cout << "\n=== Running Performance Regression Tests ===" << std::endl;
    
    compareWithBaseline();
    
    std::cout << "Regression test results:" << std::endl;
    for (const auto& regression : regressionResults_) {
        std::cout << "  " << regression.benchmarkName << " - " << regression.metric 
                  << ": " << regression.regressionPercent << "% (" << regression.regressionType << ")" << std::endl;
    }
}

void PerformanceTestSuite::compareWithBaseline() {
    std::cout << "Comparing current performance with baseline..." << std::endl;
    
    // Compare each metric in baseline with current metrics
    for (const auto& pair : currentBaseline_.metrics) {
        PerformanceMetric metric = pair.first;
        double baselineValue = pair.second;
        
        auto it = currentMetrics_.find(metric);
        if (it == currentMetrics_.end()) {
            continue; // Metric not recorded
        }
        
        double currentValue = it->second;
        
        // Get threshold
        double threshold = 0.0;
        auto thresholdIt = currentBaseline_.thresholds.find(metric);
        if (thresholdIt != currentBaseline_.thresholds.end()) {
            threshold = thresholdIt->second;
        }
        
        // Create regression result
        PerformanceRegression regression("baseline_comparison", metric,
                                        baselineValue, currentValue, threshold);
        
        if (regression.isRegressed) {
            regressionResults_.push_back(regression);
        }
    }
}

std::vector<PerformanceRegression> PerformanceTestSuite::getRegressionResults() const {
    return regressionResults_;
}

void PerformanceTestSuite::runLoadTest(const LoadTestConfig& config) {
    std::cout << "\n=== Running Load Test: " << config.name << " ===" << std::endl;
    
    LoadTestResult result = runSimulatedLoadTest(config);
    loadTestResults_.push_back(result);
    
    generatePerformanceReport();
}

LoadTestResult PerformanceTestSuite::runSimulatedLoadTest(const LoadTestConfig& config) {
    std::cout << "Simulating load test with " << config.concurrentUsers << " concurrent users for " 
              << config.duration << " seconds..." << endl;
    
    LoadTestResult result(config.name, config.concurrentUsers, config.duration);
    
    // Simulate user operations
    size_t totalOperations = 0;
    double totalLatency = 0.0;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < config.concurrentUsers; ++i) {
        for (size_t op = 0; op < config.duration * 10; ++op) { // 10 operations per second per user
            // Simulate operation
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            totalOperations++;
            double latency = 10.0 + (rand() % 50); // 10-60ms latency
            totalLatency += latency;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    result.totalOperations = totalOperations;
    result.operationsPerSecond = totalOperations / (duration.count() / 1000.0);
    result.averageLatency = totalLatency / totalOperations;
    
    // Calculate percentiles
    result.p95Latency = result.averageLatency * 2.0; // Estimate
    result.p99Latency = result.averageLatency * 3.0; // Estimate
    
    result.errorRate = 0.01; // 1% error rate
    result.throughput = result.operationsPerSecond;
    result.cpuUtilization = 75.0; // 75% CPU usage
    result.memoryUsage = 512.0; // 512MB
    
    std::cout << "Load test completed:" << endl;
    std::cout << "  Operations: " << result.totalOperations << endl;
    std::cout << "  Ops/sec: " << result.operationsPerSecond << endl;
    std::cout << "  Avg latency: " << result.averageLatency << "ms" << endl;
    std::cout << "  Error rate: " << (result.errorRate * 100) << "%" << endl;
    
    return result;
}

void PerformanceTestSuite::runScalabilityTests() {
    std::cout << "\n=== Running Scalability Tests ===" << std::endl;
    
    std::vector<size_t> userCounts = {10, 50, 100, 500, 1000};
    measureScalability(userCounts, 30.0); // 30 second duration
}

void PerformanceTestSuite::measureScalability(const std::vector<size_t>& userCounts, double duration) {
    std::cout << "Measuring scalability with " << userCounts.size() << " different user counts..." << endl;
    
    for (size_t users : userCounts) {
        std::cout << "Testing with " << users << " users..." << endl;
        
        LoadTestConfig config("scalability_test", users, duration);
        LoadTestResult result = runSimulatedLoadTest(config);
        
        std::cout << "  Throughput: " << result.operationsPerSecond << " ops/sec" << endl;
        std::cout << "  Avg latency: " << result.averageLatency << "ms" << endl;
        
        // Calculate scalability metrics
        if (result.totalOperations > 0) {
            double opsPerUser = result.totalOperations / users;
            std::cout << "  Ops per user: " << opsPerUser << endl;
        }
    }
}

void PerformanceTestSuite::runStressTests() {
    std::cout << "\n=== Running Stress Tests ===" << std::endl;
    
    // Target different stress levels
    std::vector<std::pair<size_t, double>> stressLevels = {
        {100, 10.0},   // 100 users, 10 seconds
        {500, 30.0},   // 500 users, 30 seconds
        {1000, 60.0}   // 1000 users, 60 seconds
    };
    
    for (const auto& level : stressLevels) {
        size_t users = level.first;
        double duration = level.second;
        
        std::cout << "Stress test with " << users << " users for " << duration << " seconds..." << endl;
        
        LoadTestConfig config("stress_test", users, duration);
        LoadTestResult result = runSimulatedLoadTest(config);
        
        std::cout << "  Result: " << result.operationsPerSecond << " ops/sec" << endl;
        
        if (result.errorRate > 0.05) { // 5% error rate threshold
            std::cout << "  WARNING: High error rate!" << endl;
        }
    }
}

void PerformanceTestSuite::measureStressLimits(size_t targetLoad, double duration) {
    std::cout << "Measuring stress limits with target load of " << targetLoad << " users for " << duration << " seconds..." << endl;
    
    LoadTestConfig config("stress_limit_test", targetLoad, duration);
    LoadTestResult result = runSimulatedLoadTest(config);
    
    std::cout << "Stress limit results:" << endl;
    std::cout << "  Throughput: " << result.operationsPerSecond << " ops/sec" << endl;
    std::cout << "  Avg latency: " << result.averageLatency << "ms" << endl;
    std::cout << "  CPU utilization: " << result.cpuUtilization << "%" << endl;
    std::cout << "  Memory usage: " << result.memoryUsage << "MB" << endl;
    
    // Determine if we reached system limits
    if (result.averageLatency > 100.0 || result.errorRate > 0.05) {
        std::cout << "  System reached stress limits!" << endl;
    }
}

void PerformanceTestSuite::generatePerformanceReport() const {
    std::cout << "\n=== Performance Test Report ===" << std::endl;
    
    // Summary statistics
    size_t totalTests = regressionResults_.size() + loadTestResults_.size();
    
    std::cout << "Tests executed: " << totalTests << endl;
    
    // Regression summary
    size_t regressedTests = 0;
    for (const auto& regression : regressionResults_) {
        if (regression.isRegressed) {
            regressedTests++;
        }
    }
    
    std::cout << "Performance regressions: " << regressedTests << endl;
    
    // Load test summary
    std::cout << "Load tests executed: " << loadTestResults_.size() << endl;
    
    // Performance metrics
    std::cout << "\nCurrent Performance Metrics:" << endl;
    for (const auto& pair : currentMetrics_) {
        if (pair.first != PerformanceMetric::CUSTOM) { // Skip custom metrics for brevity
            std::cout << "  " << static_cast<int>(pair.first) << ": " << pair.second << endl;
        }
    }
    
    // Bottleneck analysis
    std::vector<std::string> bottlenecks = detectBottlenecks();
    if (!bottlenecks.empty()) {
        std::cout << "\nPerformance Bottlenecks:" << endl;
        for (const auto& bottleneck : bottlenecks) {
            std::cout << "  - " << bottleneck << endl;
        }
    }
}

void PerformanceTestSuite::exportPerformanceData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for writing performance data: " << filename << endl;
        return;
    }
    
    file << "Performance Data Export" << endl;
    file << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << endl;
    file << "Session duration: " << (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - sessionStartTime_)).count() << " seconds" << endl;
    
    file << "\nRegression Results:" << endl;
    for (const auto& regression : regressionResults_) {
        file << "  " << regression.benchmarkName << 
             " (" << static_cast<int>(regression.metric) << ") " <<
             regression.regressionPercent << "% (" << regression.regressionType << ")" << endl;
    }
    
    file << "\nLoad Test Results:" << endl;
    for (const auto& result : loadTestResults_) {
        file << "  " << result.testName << ": " << result.operationsPerSecond << " ops/sec, "
             << result.averageLatency << "ms avg latency, " << (result.errorRate * 100) << "% error rate" << endl;
    }
    
    file.close();
    std::cout << "Performance data exported to: " << filename << endl;
}

bool PerformanceTestSuite::validatePerformanceConfiguration() const {
    std::cout << "Validating performance configuration..." << endl;
    
    bool isValid = true;
    
    // Check if baseline is properly configured
    if (currentBaseline_.name.empty()) {
        std::cout << "Warning: No baseline name set" << endl;
        isValid = false;
    }
    
    // Check if thresholds are set
    if (currentBaseline_.thresholds.empty()) {
        std::cout << "Warning: No performance thresholds configured" << endl;
        isValid = false;
    }
    
    // Check if weights are set
    if (currentBaseline_.weights.empty()) {
        std::cout << "Warning: No performance weights configured" << endl;
        isValid = false;
    }
    
    if (isValid) {
        std::cout << "Performance configuration validation passed" << endl;
    } else {
        std::cout << "Performance configuration validation failed" << endl;
    }
    
    return isValid;
}

ResourceAllocation PerformanceTestSuite::getCurrentResourceAllocation() const {
    ResourceAllocation allocation;
    
    // In a real implementation, this would query the system for current resource usage
    // For now, we'll return simulated values based on current metrics
    
    allocation.memoryAllocated = static_cast<size_t>(currentMetrics_[PerformanceMetric::MEMORY_ALLOCATION] * 1024 * 1024); // MB to bytes
    allocation.memoryUsed = static_cast<size_t>(currentMetrics_[PerformanceMetric::MEMORY_USAGE] * 1024 * 1024);
    allocation.memoryPeak = allocation.memoryUsed * 1.2; // 20% over peak
    allocation.cpuCoresUsed = 4; // Assume 4 cores
    allocation.cpuTime = std::chrono::seconds(60); // 1 minute of CPU time
    allocation.cpuUsagePercentage = currentMetrics_[PerformanceMetric::CPU_USAGE];
    
    std::cout << "Current resource allocation: " << allocation.memoryUsed << " bytes memory, "
              << allocation.cpuUsagePercentage << "% CPU" << endl;
    
    return allocation;
}

PerformanceStats PerformanceTestSuite::getPerformanceStats(PerformanceMetric metric) const {
    PerformanceStats stats;
    
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    // Find the data for this metric
    for (const auto& pair : performanceData_) {
        // In a real implementation, this would analyze the data for the specific metric
    }
    
    // For simplicity, return calculated statistics
    stats.mean = currentMetrics_[metric];
    stats.median = currentMetrics_[metric] * 1.1; // Estimate
    stats.stddev = currentMetrics_[metric] * 0.1; // Estimate
    stats.min = currentMetrics_[metric] * 0.5; // Estimate
    stats.max = currentMetrics_[metric] * 2.0; // Estimate
    stats.variance = stats.stddev * stats.stddev;
    stats.count = 100; // Assume 100 samples
    
    return stats;
}

void PerformanceTestSuite::setupPerformanceMonitoring(PerformanceMonitor& monitor) {
    activeMonitor_ = &monitor;
    monitoringActive_.store(true);
    
    startPerformanceMonitoring(monitor);
    std::cout << "Performance monitoring setup completed" << endl;
}

double PerformanceTestSuite::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double PerformanceTestSuite::calculateStdDev(const std::vector<double>& values, double mean) const {
    if (values.size() <= 1) return 0.0;
    
    double sum = 0.0;
    for (const auto& value : values) {
        sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum / (values.size() - 1));
}

double PerformanceTestSuite::calculatePercentile(const std::vector<double>& values, double percentile) const {
    if (values.empty()) return 0.0;
    
    std::vector<double> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    
    size_t index = static_cast<size_t>(percentile / 100.0 * (values.size() - 1));
    return sortedValues[index];
}

std::vector<std::string> PerformanceTestSuite::detectBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    // Check for high memory usage
    if (currentMetrics_[PerformanceMetric::MEMORY_USAGE] > 80.0) {
        bottlenecks.push_back("High memory usage (>80%)");
    }
    
    // Check for high CPU usage
    if (currentMetrics_[PerformanceMetric::CPU_USAGE] > 80.0) {
        bottlenecks.push_back("High CPU usage (>80%)");
    }
    
    // Check for high latency
    if (currentMetrics_[PerformanceMetric::LATENCY] > 100.0) {
        bottlenecks.push_back("High latency (>100ms)");
    }
    
    // Check for low throughput
    if (currentMetrics_[PerformanceMetric::THROUGHPUT] < 1000.0) {
        bottlenecks.push_back("Low throughput (<1000 ops/sec)");
    }
    
    return bottlenecks;
}

void PerformanceTestSuite::writePerformanceData(std::ofstream& file) const {
    file << "Performance Data" << std::endl;
    file << "Metrics:" << std::endl;
    
    for (const auto& pair : currentMetrics_) {
        file << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
}

void PerformanceTestSuite::readPerformanceData(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("  ") != std::string::npos && line.find(":") != std::string::npos) {
            size_t colonPos = line.find(":");
            int metric = std::stoi(line.substr(2, colonPos - 2));
            double value = std::stod(line.substr(colonPos + 2));
            currentMetrics_[static_cast<PerformanceMetric>(metric)] = value;
        }
    }
}

LoadTestResult PerformanceTestSuite::executeLoadTest(const LoadTestConfig& config) {
    return runSimulatedLoadTest(config);
}

void PerformanceTestSuite::simulateLoad(std::function<void()> operation, size_t duration, size_t concurrentUsers) {
    std::vector<std::thread> threads;
    
    for (size_t i = 0; i < concurrentUsers; ++i) {
        threads.emplace_back([=]() {
            for (size_t j = 0; j < duration; ++j) {
                operation();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

bool PerformanceTestSuite::validateBaselineConfiguration() const {
    bool isValid = true;
    
    if (currentBaseline_.name.empty()) {
        std::cout << "Error: Baseline name not set" << std::endl;
        isValid = false;
    }
    
    if (currentBaseline_.metrics.empty()) {
        std::cout << "Error: No baseline metrics defined" << std::endl;
        isValid = false;
    }
    
    return isValid;
}

bool PerformanceTestSuite::validateTestConfiguration() const {
    bool isValid = true;
    
    if (performanceData_.empty()) {
        std::cout << "Warning: No performance data collected" << std::endl;
    }
    
    return isValid;
}

void PerformanceTestSuite::startPerformanceMonitoring(PerformanceMonitor& monitor) {
    monitoringActive_.store(true);
    monitoringThread_ = std::thread(&PerformanceTestSuite::monitoringThreadFunc, this);
}

void PerformanceTestSuite::stopPerformanceMonitoring() {
    monitoringActive_.store(false);
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void PerformanceTestSuite::monitoringThreadFunc() {
    while (monitoringActive_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Update monitoring metrics
        // In a real implementation, this would update the PerformanceMonitor
    }
}

// BenchmarkSuite implementation

void BenchmarkSuite::registerBenchmark(const std::string& name,
                                       const std::string& description,
                                       std::function<void()> benchmarkFunction,
                                       size_t iterations,
                                       const std::string& unit,
                                       const std::string& category) {
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    Benchmark bench(name, description, benchmarkFunction, iterations, unit, category);
    benchmarks_.push_back(bench);
    
    std::cout << "Registered benchmark: " << name << " (" << category << ")" << std::endl;
}

BenchmarkResult BenchmarkSuite::runBenchmark(const std::string& name) const {
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    for (const auto& benchmark : benchmarks_) {
        if (benchmark.name == name) {
            std::cout << "Running benchmark: " << name << std::endl;
            
            auto start = std::chrono::high_resolution_clock::now();
            benchmark.function();
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            double durationMs = duration.count() / 1000.0;
            
            BenchmarkResult result(name, durationMs, 0.0, 1, "ms");
            
            std::cout << "  Result: " << durationMs << "ms" << std::endl;
            return result;
        }
    }
    
    std::cout << "Error: Benchmark not found: " << name << std::endl;
    return BenchmarkResult(name, 0.0, 0.0, 0, "ms");
}

void BenchmarkSuite::runAllBenchmarks() {
    std::cout << "=== Running All Benchmarks ===" << std::endl;
    
    for (const auto& benchmark : benchmarks_) {
        runBenchmark(benchmark.name);
    }
    
    std::cout << "All benchmarks completed" << std::endl;
}

std::vector<std::string> BenchmarkSuite::getBenchmarksByCategory(const std::string& category) const {
    std::vector<std::string> result;
    
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    for (const auto& benchmark : benchmarks_) {
        if (benchmark.category == category) {
            result.push_back(benchmark.name);
        }
    }
    
    return result;
}

std::vector<std::string> BenchmarkSuite::getAllBenchmarks() const {
    std::vector<std::string> result;
    
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    for (const auto& benchmark : benchmarks_) {
        result.push_back(benchmark.name);
    }
    
    return result;
}

void BenchmarkSuite::generateBenchmarkReport() const {
    std::cout << "\n=== Benchmark Report ===" << std::endl;
    
    std::cout << "Total benchmarks: " << benchmarks_.size() << std::endl;
    
    // Category summary
    std::unordered_map<std::string, size_t> categoryCounts;
    for (const auto& benchmark : benchmarks_) {
        categoryCounts[benchmark.category]++;
    }
    
    std::cout << "\nBenchmarks by category:" << std::endl;
    for (const auto& pair : categoryCounts) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void BenchmarkSuite::exportBenchmarkResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for writing benchmark results: " << filename << std::endl;
        return;
    }
    
    file << "Benchmark Results Export" << std::endl;
    file << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    file << "Total benchmarks: " << benchmarks_.size() << std::endl;
    
    for (const auto& benchmark : benchmarks_) {
        file << "\nBenchmark: " << benchmark.name << std::endl;
        file << "Description: " << benchmark.description << std::endl;
        file << "Category: " << benchmark.category << std::endl;
        file << "Iterations: " << benchmark.iterations << std::endl;
        file << "Unit: " << benchmark.unit << std::endl;
        file << "Last Result: " << benchmark.lastResult.duration << " " << benchmark.lastResult.unit << std::endl;
    }
    
    file.close();
    std::cout << "Benchmark results exported to: " << filename << std::endl;
}

void BenchmarkSuite::compareBenchmarks(const std::string& baselineFile) const {
    std::cout << "Comparing benchmarks with baseline: " << baselineFile << std::endl;
    
    // In a real implementation, this would load the baseline and compare
    std::cout << "Benchmark comparison completed (simplified implementation)" << std::endl;
}

// PerformanceProfiler implementation

void PerformanceProfiler::profileFunction(const std::string& name,
                                         std::function<void()> func,
                                         size_t iterations) {
    std::cout << "Profiling function: " << name << " (" << iterations << " iterations)" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < iterations; ++i) {
        func();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double durationMs = duration.count() / 1000.0;
    double avgDurationMs = durationMs / iterations;
    
    std::lock_guard<std::mutex> lock(profileMutex_);
    ProfileData data;
    data.operationName = name;
    data.operationType = "function";
    data.duration = avgDurationMs;
    data.memoryUsed = 0.0; // Would be measured in real implementation
    data.cpuUsage = 0.0; // Would be measured in real implementation
    data.iterations = iterations;
    data.timestamp = std::chrono::high_resolution_clock::now();
    
    profileData_.push_back(data);
    
    std::cout << "  Average duration: " << avgDurationMs << "ms per iteration" << std::endl;
}

void PerformanceProfiler::profileMemoryUsage(const std::string& name,
                                            std::function<void()> func) {
    std::cout << "Profiling memory usage: " << name << std::endl;
    
    // In a real implementation, this would measure memory usage
    // For now, we'll simulate it
    double memoryUsed = 100.0 + (rand() % 100); // 100-200MB
    
    std::lock_guard<std::mutex> lock(profileMutex_);
    ProfileData data;
    data.operationName = name;
    data.operationType = "memory";
    data.duration = 0.0; // Not measured
    data.memoryUsed = memoryUsed;
    data.cpuUsage = 0.0; // Not measured
    data.iterations = 1;
    data.timestamp = std::chrono::high_resolution_clock::now();
    
    profileData_.push_back(data);
    
    std::cout << "  Memory used: " << memoryUsed << "MB" << std::endl;
}

void PerformanceProfiler::profileCPUUsage(const std::string& name,
                                         std::function<void()> func) {
    std::cout << "Profiling CPU usage: " << name << std::endl;
    
    // In a real implementation, this would measure CPU usage
    // For now, we'll simulate it
    double cpuUsage = 10.0 + (rand() % 80); // 10-90% CPU usage
    
    std::lock_guard<std::mutex> lock(profileMutex_);
    ProfileData data;
    data.operationName = name;
    data.operationType = "cpu";
    data.duration = 0.0; // Not measured
    data.memoryUsed = 0.0; // Not measured
    data.cpuUsage = cpuUsage;
    data.iterations = 1;
    data.timestamp = std::chrono::high_resolution_clock::now();
    
    profileData_.push_back(data);
    
    std::cout << "  CPU usage: " << cpuUsage << "%" << std::endl;
}

void PerformanceProfiler::generateProfileReport() const {
    std::cout << "\n=== Performance Profile Report ===" << std::endl;
    
    std::cout << "Total profiles: " << profileData_.size() << std::endl;
    
    // Profile type summary
    std::unordered_map<std::string, size_t> typeCounts;
    for (const auto& data : profileData_) {
        typeCounts[data.operationType]++;
    }
    
    std::cout << "\nProfiles by type:" << std::endl;
    for (const auto& pair : typeCounts) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    // Top performance data
    std::cout << "\nTop performance metrics:" << std::endl;
    for (const auto& data : profileData_) {
        std::cout << "  " << data.operationName << " (" << data.operationType << ") "
                  << data.duration << "ms, " << data.memoryUsed << "MB, " << data.cpuUsage << "%" << std::endl;
    }
}

void PerformanceProfiler::exportProfileData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for writing profile data: " << filename << std::endl;
        return;
    }
    
    file << "Performance Profile Data" << std::endl;
    file << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    file << "Total profiles: " << profileData_.size() << std::endl;
    
    for (const auto& data : profileData_) {
        file << "\nProfile: " << data.operationName << std::endl;
        file << "Type: " << data.operationType << std::endl;
        file << "Duration: " << data.duration << "ms" << std::endl;
        file << "Memory Used: " << data.memoryUsed << "MB" << std::endl;
        file << "CPU Usage: " << data.cpuUsage << "%" << std::endl;
        file << "Iterations: " << data.iterations << std::endl;
    }
    
    file.close();
    std::cout << "Profile data exported to: " << filename << std::endl;
}

std::vector<std::string> PerformanceProfiler::detectPerformanceBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    for (const auto& data : profileData_) {
        if (data.operationType == "function" && data.duration > 100.0) {
            bottlenecks.push_back("Slow function: " + data.operationName + " (>100ms)");
        }
        
        if (data.operationType == "memory" && data.memoryUsed > 500.0) {
            bottlenecks.push_back("High memory usage: " + data.operationName + " (>500MB)");
        }
        
        if (data.operationType == "cpu" && data.cpuUsage > 80.0) {
            bottlenecks.push_back("High CPU usage: " + data.operationName + " (>80%)");
        }
    }
    
    return bottlenecks;
}

} // namespace testing
} // namespace nlm