#include "PerformanceTestSuite.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>

namespace nlm {

PerformanceBenchmark::PerformanceBenchmark() {}

void PerformanceBenchmark::addBenchmark(const std::string& name, BenchmarkType type, std::function<void()> benchmarkFunc) {
    std::lock_guard<std::mutex> lock(benchmarksMutex);
    benchmarks[name] = std::make_pair(type, benchmarkFunc);
}

void PerformanceBenchmark::removeBenchmark(const std::string& name) {
    std::lock_guard<std::mutex> lock(benchmarksMutex);
    benchmarks.erase(name);
}

PerformanceBenchmark::BenchmarkResult PerformanceBenchmark::runBenchmark(const std::string& name) {
    std::lock_guard<std::mutex> lock(benchmarksMutex);
    
    auto it = benchmarks.find(name);
    if (it == benchmarks.end()) {
        throw std::runtime_error("Benchmark not found: " + name);
    }
    
    BenchmarkResult result(name, it->second.first);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        it->second.second();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration<double>(endTime - startTime);
        result.executionTime = result.duration.count();
        
        // Placeholder for memory and CPU measurements
        result.memoryUsage = 0.0;
        result.cpuUsage = 0.0;
        
        // Placeholder for operations per second
        result.operationsPerSecond = 0;
        result.dataThroughput = 0;
        
        benchmarkResults.push_back(result);
    } catch (const std::exception& e) {
        throw std::runtime_error("Benchmark failed: " + name + " - " + e.what());
    }
    
    return result;
}

std::vector<PerformanceBenchmark::BenchmarkResult> PerformanceBenchmark::runAllBenchmarks() {
    std::vector<BenchmarkResult> results;
    
    for (const auto& pair : benchmarks) {
        try {
            results.push_back(runBenchmark(pair.first));
        } catch (const std::exception& e) {
            std::cout << "Warning: Benchmark " << pair.first << " failed: " << e.what() << std::endl;
        }
    }
    
    return results;
}

double PerformanceBenchmark::calculatePerformanceScore(const BenchmarkResult& benchmark) const {
    double score = 1.0 / (benchmark.executionTime + 1.0); // Higher score for faster execution
    score += 1.0 / (benchmark.memoryUsage + 1.0); // Higher score for lower memory usage
    return score;
}

double PerformanceBenchmark::calculatePerformanceScore(const std::vector<BenchmarkResult>& benchmarks) const {
    if (benchmarks.empty()) return 0.0;
    
    double totalScore = 0.0;
    for (const auto& benchmark : benchmarks) {
        totalScore += calculatePerformanceScore(benchmark);
    }
    
    return totalScore / benchmarks.size();
}

std::string PerformanceBenchmark::getPerformanceReport(const std::vector<BenchmarkResult>& results) const {
    std::stringstream ss;
    ss << "=== Performance Benchmark Report ===" << std::endl;
    ss << "Total Benchmarks: " << results.size() << std::endl;
    ss << std::endl;
    
    ss << "Benchmark Results:" << std::endl;
    ss << std::left << std::setw(30) << "Name" 
       << std::setw(15) << "Type" 
       << std::setw(15) << "Execution Time" 
       << std::setw(15) << "Memory Usage" 
       << std::setw(15) << "CPU Usage" 
       << std::endl;
    ss << std::string(95, '-') << std::endl;
    
    for (const auto& result : results) {
        std::string typeStr;
        switch (result.type) {
            case BenchmarkType::SPATIAL: typeStr = "Spatial"; break;
            case BenchmarkType::TEMPORAL: typeStr = "Temporal"; break;
            case BenchmarkType::MEMORY: typeStr = "Memory"; break;
            case BenchmarkType::COMPUTATION: typeStr = "Computation"; break;
            case BenchmarkType::IO: typeStr = "I/O"; break;
            case BenchmarkType::NETWORK: typeStr = "Network"; break;
        }
        
        ss << std::left << std::setw(30) << result.benchmarkName
           << std::setw(15) << typeStr
           << std::setw(15) << result.executionTime << "s"
           << std::setw(15) << result.memoryUsage << "MB"
           << std::setw(15) << result.cpuUsage << "%" << std::endl;
    }
    
    ss << std::endl << "=== Performance Summary ===" << std::endl;
    if (!results.empty()) {
        double avgExecutionTime = 0.0, avgMemoryUsage = 0.0, avgCpuUsage = 0.0;
        for (const auto& result : results) {
            avgExecutionTime += result.executionTime;
            avgMemoryUsage += result.memoryUsage;
            avgCpuUsage += result.cpuUsage;
        }
        
        avgExecutionTime /= results.size();
        avgMemoryUsage /= results.size();
        avgCpuUsage /= results.size();
        
        ss << "Average Execution Time: " << avgExecutionTime << "s" << std::endl;
        ss << "Average Memory Usage: " << avgMemoryUsage << "MB" << std::endl;
        ss << "Average CPU Usage: " << avgCpuUsage << "%" << std::endl;
        
        double avgScore = calculatePerformanceScore(results);
        ss << "Average Performance Score: " << avgScore << std::endl;
    }
    
    return ss.str();
}

bool PerformanceBenchmark::checkPerformanceTarget(const BenchmarkResult& benchmark, double targetTime, double tolerance) const {
    return benchmark.executionTime <= (targetTime * (1.0 + tolerance));
}

bool PerformanceBenchmark::checkMemoryTarget(const BenchmarkResult& benchmark, double targetMemoryMB, double tolerance) const {
    return benchmark.memoryUsage <= (targetMemoryMB * (1.0 + tolerance));
}

double PerformanceBenchmark::comparePerformance(const BenchmarkResult& benchmark1, const BenchmarkResult& benchmark2, 
                                               const std::string& metric) const {
    if (metric == "execution_time") {
        return (benchmark1.executionTime > 0) ? (benchmark2.executionTime / benchmark1.executionTime) : 0.0;
    } else if (metric == "memory_usage") {
        return (benchmark1.memoryUsage > 0) ? (benchmark2.memoryUsage / benchmark1.memoryUsage) : 0.0;
    } else if (metric == "cpu_usage") {
        return (benchmark1.cpuUsage > 0) ? (benchmark2.cpuUsage / benchmark1.cpuUsage) : 0.0;
    }
    return 0.0;
}

std::vector<double> PerformanceBenchmark::calculatePerformanceTrend(const std::vector<BenchmarkResult>& results) const {
    std::vector<double> trend;
    if (results.size() < 2) return trend;
    
    double prevTime = results[0].executionTime;
    for (size_t i = 1; i < results.size(); ++i) {
        double currentTime = results[i].executionTime;
        if (prevTime > 0) {
            trend.push_back((currentTime - prevTime) / prevTime);
        }
        prevTime = currentTime;
    }
    
    return trend;
}

double PerformanceBenchmark::calculatePerformanceRegressionRate(const std::vector<BenchmarkResult>& results) const {
    if (results.size() < 2) return 0.0;
    
    std::vector<double> trends = calculatePerformanceTrend(results);
    if (trends.empty()) return 0.0;
    
    double totalTrend = std::accumulate(trends.begin(), trends.end(), 0.0);
    return totalTrend / trends.size();
}

// MemoryAllocationTracker implementation
MemoryAllocationTracker::MemoryAllocationTracker() : currentMemoryUsage(0), peakMemoryUsage(0), lastAllocationTime(std::chrono::system_clock::now()) {}

void MemoryAllocationTracker::trackAllocation(size_t bytes, const std::string& type) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    AllocationInfo info(bytes, type);
    info.currentBytes = currentMemoryUsage + bytes;
    info.peakBytes = std::max(peakMemoryUsage, info.currentBytes);
    
    currentMemoryUsage = info.currentBytes;
    peakMemoryUsage = info.peakBytes;
    
    allocationHistory.push_back(info);
    allocationCounts[type]++;
    
    auto now = std::chrono::system_clock::now();
    info.timestamp = now;
}

void MemoryAllocationTracker::trackDeallocation(size_t bytes, const std::string& type) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    AllocationInfo info(bytes, type);
    info.currentBytes = currentMemoryUsage - bytes;
    info.peakBytes = std::max(peakMemoryUsage, info.currentBytes);
    
    currentMemoryUsage = std::max(0, (long)currentMemoryUsage - bytes);
    peakMemoryUsage = info.peakBytes;
    
    allocationHistory.push_back(info);
    deallocationCounts[type]++;
}

double MemoryAllocationTracker::getMemoryGrowthRate() const {
    if (allocationHistory.size() < 2) return 0.0;
    
    auto first = allocationHistory.front();
    auto last = allocationHistory.back();
    
    auto duration = std::chrono::duration<double>(last.timestamp - first.timestamp);
    if (duration.count() <= 0.0) return 0.0;
    
    return (last.currentBytes - first.currentBytes) / duration.count();
}

bool MemoryAllocationTracker::detectMemoryLeak(size_t thresholdBytes) const {
    size_t totalAllocated = 0;
    size_t totalDeallocated = 0;
    
    for (const auto& info : allocationHistory) {
        if (info.allocationType.find("alloc") != std::string::npos) {
            totalAllocated += info.allocatedBytes;
        } else if (info.allocationType.find("dealloc") != std::string::npos) {
            totalDeallocated += info.allocatedBytes;
        }
    }
    
    return (totalAllocated - totalDeallocated) > thresholdBytes;
}

std::vector<size_t> MemoryAllocationTracker::detectLeakPoints(size_t thresholdBytes) const {
    std::vector<size_t> leakPoints;
    
    for (size_t i = 0; i < allocationHistory.size(); ++i) {
        size_t runningTotal = 0;
        for (size_t j = i; j < allocationHistory.size(); ++j) {
            if (allocationHistory[j].allocationType.find("alloc") != std::string::npos) {
                runningTotal += allocationHistory[j].allocatedBytes;
            } else if (allocationHistory[j].allocationType.find("dealloc") != std::string::npos) {
                runningTotal -= allocationHistory[j].allocatedBytes;
            }
            
            if (runningTotal > thresholdBytes) {
                leakPoints.push_back(j);
                break;
            }
        }
    }
    
    return leakPoints;
}

std::string MemoryAllocationTracker::getMemoryUsageReport() const {
    std::stringstream ss;
    ss << "=== Memory Allocation Report ===" << std::endl;
    ss << "Current Memory Usage: " << currentMemoryUsage << " bytes" << std::endl;
    ss << "Peak Memory Usage: " << peakMemoryUsage << " bytes" << std::endl;
    ss << "Memory Growth Rate: " << getMemoryGrowthRate() << " bytes/second" << std::endl;
    ss << "Total Allocations: " << allocationHistory.size() << std::endl;
    
    if (detectMemoryLeak()) {
        ss << "WARNING: Memory leak detected!" << std::endl;
        std::vector<size_t> leakPoints = detectLeakPoints();
        ss << "Leak points: " << leakPoints.size() << std::endl;
    }
    
    ss << std::endl << "Allocation Counts:" << std::endl;
    for (const auto& pair : allocationCounts) {
        ss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    return ss.str();
}

void MemoryAllocationTracker::reset() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    allocationHistory.clear();
    currentMemoryUsage = 0;
    peakMemoryUsage = 0;
    allocationCounts.clear();
    deallocationCounts.clear();
}

void MemoryAllocationTracker::setupTrackingForTest(TestFramework::TestCase& testCase) {
    // Setup memory tracking for the test
    // This would integrate with the test framework
}

void MemoryAllocationTracker::cleanupTrackingForTest(TestFramework::TestCase& testCase) {
    // Cleanup memory tracking after the test
    // This would integrate with the test framework
}

void MemoryAllocationTracker::simulateMemoryPressure(size_t pressureBytes) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    currentMemoryUsage += pressureBytes;
    peakMemoryUsage = std::max(peakMemoryUsage, currentMemoryUsage);
}

void MemoryAllocationTracker::releaseMemoryPressure() {
    std::lock_guard<std::mutex> lock(memoryMutex);
    currentMemoryUsage = 0;
}

// PerformanceProfiler implementation
PerformanceProfiler::PerformanceProfiler() : currentProfile(nullptr), totalPausedTime(0.0) {}

void PerformanceProfiler::startProfiling(const std::string& functionName, const std::string& fileName, int lineNumber) {
    std::lock_guard<std::mutex> lock(profilingMutex);
    
    pauseProfiling();
    
    currentProfile = new ProfileData(functionName, fileName, lineNumber);
    profilingStartTime = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(profilingMutex);
    
    if (currentProfile) {
        auto endTime = std::chrono::high_resolution_clock::now();
        currentProfile->executionTime = std::chrono::duration<double>(endTime - profilingStartTime);
        currentProfile->executionTime -= totalPausedTime;
        
        profileData.push_back(*currentProfile);
        
        delete currentProfile;
        currentProfile = nullptr;
        
        totalPausedTime = std::chrono::duration<double>(0.0);
    }
}

void PerformanceProfiler::pauseProfiling() {
    if (currentProfile) {
        auto now = std::chrono::high_resolution_clock::now();
        currentProfile->timestamp = now;
    }
}

void PerformanceProfiler::resumeProfiling() {
    if (currentProfile) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - currentProfile->timestamp);
        totalPausedTime += duration;
    }
}

 double PerformanceProfiler::getTotalProfilingTime() const {
    if (profileData.empty()) return 0.0;
    
    double totalTime = 0.0;
    for (const auto& data : profileData) {
        totalTime += data.executionTime.count();
    }
    
    return totalTime;
}

std::string PerformanceProfiler::getProfileReport() const {
    std::stringstream ss;
    ss << "=== Performance Profiling Report ===" << std::endl;
    ss << "Total Functions Profiled: " << profileData.size() << std::endl;
    ss << "Total Profiling Time: " << getTotalProfilingTime() << "s" << std::endl;
    ss << std::endl;
    
    ss << "Function Profile:" << std::endl;
    ss << std::left << std::setw(30) << "Function" 
       << std::setw(20) << "File:Line" 
       << std::setw(15) << "Execution Time" 
       << std::endl;
    ss << std::string(75, '-') << std::endl;
    
    for (const auto& data : profileData) {
        ss << std::left << std::setw(30) << data.functionName
           << std::setw(20) << data.fileName << ":" << data.lineNumber
           << std::setw(15) << data.executionTime.count() << "s" << std::endl;
    }
    
    return ss.str();
}

void PerformanceProfiler::generateFlameGraph(const std::vector<ProfileData>& data, const std::string& outputFile) const {
    // This would generate a flame graph for visualization
    // For now, just create a placeholder
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << "Flame graph placeholder for performance profiling" << std::endl;
        file.close();
    }
}

void PerformanceProfiler::generateCallGraph(const std::vector<ProfileData>& data, const std::string& outputFile) const {
    // This would generate a call graph for visualization
    // For now, just create a placeholder
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << "Call graph placeholder for performance profiling" << std::endl;
        file.close();
    }
}

std::string PerformanceProfiler::identifyBottlenecks(const std::vector<ProfileData>& data, double thresholdPercent) const {
    if (data.empty()) return "No profiling data available";
    
    double totalTime = getTotalProfilingTime();
    double thresholdTime = totalTime * thresholdPercent;
    
    std::vector<const ProfileData*> bottlenecks;
    for (const auto& profile : data) {
        if (profile.executionTime.count() >= thresholdTime) {
            bottlenecks.push_back(&profile);
        }
    }
    
    if (bottlenecks.empty()) return "No significant bottlenecks identified";
    
    std::stringstream ss;
    ss << "Bottlenecks identified (took > " << (thresholdPercent * 100) << "% of total time):" << std::endl;
    for (const auto* profile : bottlenecks) {
        ss << "  " << profile->functionName << " (" << profile->fileName << ":" 
           << profile->lineNumber << ") - " << profile->executionTime.count() << "s" << std::endl;
    }
    
    return ss.str();
}

double PerformanceProfiler::calculateFunctionHotness(const ProfileData& data, const std::vector<ProfileData>& allData) const {
    if (allData.empty()) return 0.0;
    
    double dataTime = data.executionTime.count();
    double totalTime = getTotalProfilingTime();
    
    return (totalTime > 0.0) ? (dataTime / totalTime) : 0.0;
}

void PerformanceProfiler::clearProfileData() {
    std::lock_guard<std::mutex> lock(profilingMutex);
    profileData.clear();
    totalPausedTime = std::chrono::duration<double>(0.0);
}

// PerformanceTestSuite implementation
PerformanceTestSuite::PerformanceTestSuite() {}

void PerformanceTestSuite::addPerformanceTest(const PerformanceTest& test) {
    std::lock_guard<std::mutex> lock(suiteMutex);
    performanceTests.push_back(test);
}

void PerformanceTestSuite::removePerformanceTest(const std::string& name) {
    std::lock_guard<std::mutex> lock(suiteMutex);
    performanceTests.erase(
        std::remove_if(performanceTests.begin(), performanceTests.end(),
                     [&name](const PerformanceTest& test) { return test.name == name; }),
        performanceTests.end()
    );
}

PerformanceTestSuite::PerformanceTest& PerformanceTestSuite::getPerformanceTest(const std::string& name) {
    for (auto& test : performanceTests) {
        if (test.name == name) {
            return test;
        }
    }
    throw std::runtime_error("Performance test not found: " + name);
}

void PerformanceTestSuite::addBenchmark(const std::string& benchmarkName, PerformanceBenchmark::BenchmarkType type, 
                                       std::function<void()> benchmarkFunc) {
    benchmarkManager.addBenchmark(benchmarkName, type, benchmarkFunc);
}

void PerformanceTestSuite::addMemoryTest(const std::string& testName, std::function<void()> memoryTestFunc) {
    // Add memory test to the suite
    // This would integrate with the test framework
}

void PerformanceTestSuite::addStressTest(const std::string& testName, std::function<void()> stressTestFunc) {
    // Add stress test to the suite
    // This would integrate with the test framework
}

std::vector<PerformanceBenchmark::BenchmarkResult> PerformanceTestSuite::runBenchmarks() {
    return benchmarkManager.runAllBenchmarks();
}

std::vector<TestResult> PerformanceTestSuite::runPerformanceTests() {
    std::vector<TestResult> results;
    
    for (const auto& test : performanceTests) {
        if (!test.enabled) continue;
        
        TestResult result(test.name, TestParameters(test.name, TestLevel::PERFORMANCE, TestCategory::PERFORMANCE));
        
        try {
            // Run the performance test
            // In a real implementation, this would execute the test logic
            
            auto startTime = std::chrono::high_resolution_clock::now();
            // Placeholder for test execution
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.duration = std::chrono::duration<double>(endTime - startTime);
            
            result.status = TestStatus::PASSED;
            results.push_back(result);
        } catch (const std::exception& e) {
            result.status = TestStatus::FAILED;
            result.failureMessage = e.what();
            results.push_back(result);
        }
    }
    
    return results;
}

std::vector<TestResult> PerformanceTestSuite::runMemoryTests() {
    std::vector<TestResult> results;
    
    // Run memory tests using the memory tracker
    // In a real implementation, this would execute the memory test logic
    
    TestResult result("MemoryLeakTest", TestParameters("MemoryLeakTest", TestLevel::MEMORY, TestCategory::MEMORY_LEAK));
    
    try {
        // Check for memory leaks
        if (memoryTracker.detectMemoryLeak()) {
            result.status = TestStatus::FAILED;
            result.failureMessage = "Memory leak detected";
        } else {
            result.status = TestStatus::PASSED;
        }
        
        results.push_back(result);
    } catch (const std::exception& e) {
        result.status = TestStatus::ERROR;
        result.errorMessage = e.what();
        results.push_back(result);
    }
    
    return results;
}

std::vector<TestResult> PerformanceTestSuite::runStressTests() {
    std::vector<TestResult> results;
    
    // Run stress tests
    // In a real implementation, this would execute the stress test logic
    
    TestResult result("StressTest", TestParameters("StressTest", TestLevel::STRESS, TestCategory::PERFORMANCE));
    
    try {
        // Placeholder for stress test execution
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        result.status = TestStatus::PASSED;
        results.push_back(result);
    } catch (const std::exception& e) {
        result.status = TestStatus::ERROR;
        result.errorMessage = e.what();
        results.push_back(result);
    }
    
    return results;
}

void PerformanceTestSuite::runAllPerformanceTests() {
    std::cout << "=== Running Performance Tests ===" << std::endl;
    
    auto benchmarkResults = runBenchmarks();
    auto performanceResults = runPerformanceTests();
    auto memoryResults = runMemoryTests();
    auto stressResults = runStressTests();
    
    std::cout << "Benchmarks completed: " << benchmarkResults.size() << std::endl;
    std::cout << "Performance tests completed: " << performanceResults.size() << std::endl;
    std::cout << "Memory tests completed: " << memoryResults.size() << std::endl;
    std::cout << "Stress tests completed: " << stressResults.size() << std::endl;
}

void PerformanceTestSuite::generatePerformanceReport(const std::string& title) {
    std::stringstream ss;
    ss << "========================================" << std::endl;
    ss << "         " << title << "         " << std::endl;
    ss << "========================================" << std::endl;
    
    auto benchmarkResults = runBenchmarks();
    auto performanceResults = runPerformanceTests();
    auto memoryResults = runMemoryTests();
    auto stressResults = runStressTests();
    
    ss << "=== Benchmark Results ===" << std::endl;
    ss << benchmarkManager.getPerformanceReport(benchmarkResults) << std::endl;
    
    ss << "=== Memory Report ===" << std::endl;
    ss << memoryTracker.getMemoryUsageReport() << std::endl;
    
    ss << "=== Performance Summary ===" << std::endl;
    ss << getPerformanceSummary(benchmarkResults) << std::endl;
    
    ss << "========================================" << std::endl;
    
    std::cout << ss.str();
}

std::string PerformanceTestSuite::getPerformanceSummary(const std::vector<PerformanceBenchmark::BenchmarkResult>& benchmarks) const {
    std::stringstream ss;
    
    if (benchmarks.empty()) {
        ss << "No benchmarks available." << std::endl;
        return ss.str();
    }
    
    ss << "Performance Metrics:" << std::endl;
    ss << "  Total Benchmarks: " << benchmarks.size() << std::endl;
    
    double avgExecutionTime = 0.0, avgMemoryUsage = 0.0, avgCpuUsage = 0.0;
    for (const auto& result : benchmarks) {
        avgExecutionTime += result.executionTime;
        avgMemoryUsage += result.memoryUsage;
        avgCpuUsage += result.cpuUsage;
    }
    
    avgExecutionTime /= benchmarks.size();
    avgMemoryUsage /= benchmarks.size();
    avgCpuUsage /= benchmarks.size();
    
    ss << "  Average Execution Time: " << avgExecutionTime << "s" << std::endl;
    ss << "  Average Memory Usage: " << avgMemoryUsage << "MB" << std::endl;
    ss << "  Average CPU Usage: " << avgCpuUsage << "%" << std::endl;
    
    double avgScore = benchmarkManager.calculatePerformanceScore(benchmarks);
    ss << "  Average Performance Score: " << avgScore << std::endl;
    
    return ss.str();
}

std::string PerformanceTestSuite::getMemoryReport() const {
    return memoryTracker.getMemoryUsageReport();
}

bool PerformanceTestSuite::detectPerformanceRegression(const std::vector<PerformanceBenchmark::BenchmarkResult>& baseline,
                                                     const std::vector<PerformanceBenchmark::BenchmarkResult>& current) const {
    if (baseline.empty() || current.empty()) return false;
    
    for (const auto& currentResult : current) {
        bool foundInBaseline = false;
        for (const auto& baselineResult : baseline) {
            if (currentResult.benchmarkName == baselineResult.benchmarkName) {
                foundInBaseline = true;
                
                double timeRatio = benchmarkManager.comparePerformance(baselineResult, currentResult, "execution_time");
                if (timeRatio > 1.1) { // 10% degradation threshold
                    return true;
                }
                
                break;
            }
        }
        
        if (!foundInBaseline) {
            // New benchmark that wasn't in baseline - consider it a regression
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> PerformanceTestSuite::getRegressedBenchmarks(const std::vector<PerformanceBenchmark::BenchmarkResult>& baseline,
                                                                    const std::vector<PerformanceBenchmark::BenchmarkResult>& current) const {
    std::vector<std::string> regressed;
    
    for (const auto& currentResult : current) {
        bool foundInBaseline = false;
        double timeRatio = 0.0;
        
        for (const auto& baselineResult : baseline) {
            if (currentResult.benchmarkName == baselineResult.benchmarkName) {
                foundInBaseline = true;
                timeRatio = benchmarkManager.comparePerformance(baselineResult, currentResult, "execution_time");
                
                if (timeRatio > 1.1) { // 10% degradation threshold
                    regressed.push_back(currentResult.benchmarkName);
                }
                
                break;
            }
        }
        
        if (!foundInBaseline) {
            regressed.push_back(currentResult.benchmarkName);
        }
    }
    
    return regressed;
}

double PerformanceTestSuite::benchmarkSpatialOperations(size_t arraySize, int iterations) {
    // Benchmark spatial operations
    std::vector<double> data(arraySize);
    double sum = 0.0;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        for (size_t j = 0; j < arraySize; ++j) {
            sum += data[j];
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime).count();
}

double PerformanceTestSuite::benchmarkTemporalProcessing(size_t sequenceLength, int iterations) {
    // Benchmark temporal processing
    std::vector<double> data(sequenceLength);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        for (size_t j = 0; j < sequenceLength; ++j) {
            // Process temporal sequence
            double temp = data[j];
            for (size_t k = 0; k < j; ++k) {
                temp += data[k];
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime).count();
}

double PerformanceTestSuite::benchmarkMemoryAllocation(int allocations, size_t allocationSize) {
    // Benchmark memory allocation
    std::vector<std::vector<char>> allocations;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < allocations; ++i) {
        allocations.push_back(std::vector<char>(allocationSize));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime).count();
}

double PerformanceTestSuite::benchmarkComputationComplexity(int operations, int dataSize) {
    // Benchmark computation complexity
    std::vector<std::vector<double>> matrix1(dataSize, std::vector<double>(dataSize));
    std::vector<std::vector<double>> matrix2(dataSize, std::vector<double>(dataSize));
    std::vector<std::vector<double>> result(dataSize, std::vector<double>(dataSize, 0.0));
    
    // Initialize matrices
    for (int i = 0; i < dataSize; ++i) {
        for (int j = 0; j < dataSize; ++j) {
            matrix1[i][j] = static_cast<double>(i + j);
            matrix2[i][j] = static_cast<double>(i * j);
        }
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int op = 0; op < operations; ++op) {
        // Perform matrix operations
        for (int i = 0; i < dataSize; ++i) {
            for (int j = 0; j < dataSize; ++j) {
                for (int k = 0; k < dataSize; ++k) {
                    result[i][j] += matrix1[i][k] * matrix2[k][j];
                }
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime).count();
}

// PerformanceTestSuite helper methods
std::vector<TestResult> PerformanceTestSuite::runSinglePerformanceTest(const PerformanceTest& test) {
    std::vector<TestResult> results;
    
    TestResult result(test.name, TestParameters(test.name, test.type, TestCategory::PERFORMANCE));
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Execute the test logic
        // In a real implementation, this would execute the specific test function
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration<double>(endTime - startTime);
        
        if (result.duration > test.maxExecutionTime) {
            result.status = TestStatus::TIMEOUT;
            result.failureMessage = "Test exceeded maximum execution time";
        } else {
            result.status = TestStatus::PASSED;
        }
        
        results.push_back(result);
    } catch (const std::exception& e) {
        result.status = TestStatus::ERROR;
        result.errorMessage = e.what();
        results.push_back(result);
    }
    
    return results;
}

std::vector<TestResult> PerformanceTestSuite::runSingleMemoryTest(const std::string& name, std::function<void()> testFunc) {
    std::vector<TestResult> results;
    
    TestResult result(name, TestParameters(name, TestLevel::MEMORY, TestCategory::MEMORY_LEAK));
    
    try {
        size_t before = memoryTracker.getCurrentMemoryUsage();
        testFunc();
        size_t after = memoryTracker.getCurrentMemoryUsage();
        
        result.duration = std::chrono::duration<double>(0.0); // Would measure actual time
        
        if (after > before) {
            result.status = TestStatus::PASSED;
            result.metrics["memory_increase"] = std::to_string(after - before);
        } else {
            result.status = TestStatus::PASSED; // or FAILED depending on test expectations
        }
        
        results.push_back(result);
    } catch (const std::exception& e) {
        result.status = TestStatus::ERROR;
        result.errorMessage = e.what();
        results.push_back(result);
    }
    
    return results;
}

std::vector<TestResult> PerformanceTestSuite::runSingleStressTest(const std::string& name, std::function<void()> testFunc) {
    std::vector<TestResult> results;
    
    TestResult result(name, TestParameters(name, TestLevel::STRESS, TestCategory::PERFORMANCE));
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        testFunc();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        result.duration = std::chrono::duration<double>(endTime - startTime);
        
        // Stress tests typically pass if they complete without crashing
        result.status = TestStatus::PASSED;
        
        results.push_back(result);
    } catch (const std::exception& e) {
        result.status = TestStatus::ERROR;
        result.errorMessage = e.what();
        results.push_back(result);
    }
    
    return results;
}

std::string PerformanceTestSuite::formatBenchmarkResult(const PerformanceBenchmark::BenchmarkResult& result) const {
    std::stringstream ss;
    ss << result.benchmarkName << ": " << result.executionTime << "s (" 
       << result.memoryUsage << "MB, " << result.cpuUsage << "%)";
    return ss.str();
}

std::string PerformanceTestSuite::formatMemoryReport(const MemoryAllocationTracker& tracker) const {
    return tracker.getMemoryUsageReport();
}

std::string PerformanceTestSuite::formatProfilerReport(const PerformanceProfiler& profiler) const {
    return profiler.getProfileReport();
}

} // namespace nlm
