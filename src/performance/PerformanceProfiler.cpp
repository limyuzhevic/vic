#include "PerformanceProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <unordered_set>

namespace nlm {

PerformanceMeasurement::PerformanceMeasurement()
    : operation_(""),
      context_(""),
      startTime_(std::chrono::system_clock::now()),
      active_(false)
{
}

PerformanceMeasurement::PerformanceMeasurement(const std::string& operation, const std::string& context)
    : operation_(operation),
      context_(context),
      startTime_(std::chrono::system_clock::now()),
      active_(true)
{
}

PerformanceMeasurement::~PerformanceMeasurement() {
    if (active_) {
        stopMeasurement(*this);
    }
}

double PerformanceMeasurement::getElapsedTime() const {
    if (!active_) return 0.0;
    
    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
    return duration.count() / 1000000.0; // Convert to seconds
}

std::string PerformanceMeasurement::getFormattedTime() const {
    double elapsed = getElapsedTime();
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << elapsed << "s";
    return oss.str();
}

PerformanceProfiler::PerformanceProfiler()
    : profilingInterval_(0.1),
      autoDetectBottlenecks_(true),
      enableFunctionProfiling_(true),
      enableAlgorithmProfiling_(true),
      minSamples_(5),
      totalMeasurements_(0),
      totalTime_(0.0)
{
}

PerformanceProfiler::PerformanceProfiler(std::shared_ptr<DebuggingSystem> debuggingSystem)
    : profilingInterval_(0.1),
      autoDetectBottlenecks_(true),
      enableFunctionProfiling_(true),
      enableAlgorithmProfiling_(true),
      minSamples_(5),
      debuggingSystem_(debuggingSystem),
      totalMeasurements_(0),
      totalTime_(0.0)
{
}

PerformanceProfiler::~PerformanceProfiler() = default;

PerformanceMeasurement PerformanceProfiler::startMeasurement(const std::string& operation, const std::string& context) {
    PerformanceMeasurement measurement(operation, context);
    return measurement;
}

void PerformanceProfiler::stopMeasurement(PerformanceMeasurement& measurement) {
    if (!measurement.isActive()) return;
    
    double elapsedTime = measurement.getElapsedTime();
    
    Measurement m;
    m.timestamp = measurement.getStartTime();
    m.operation = measurement.getOperation();
    m.time = elapsedTime;
    m.context = measurement.getContext();
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    measurements_.push_back(m);
    
    totalMeasurements_++;
    totalTime_ += elapsedTime;
    
    // Update debugging system if available
    if (debuggingSystem_) {
        debuggingSystem_->recordPerformanceMetric(m.operation, m.time);
    }
    
    // Update function call profiler if available
    if (enableFunctionProfiling_ && functionCallProfiler_) {
        functionCallProfiler_->recordFunctionCall(m.operation, m.time, m.context);
    }
    
    // Update algorithm profiler if available
    if (enableAlgorithmProfiling_ && algorithmProfiler_) {
        algorithmProfiler_->recordAlgorithmOperation(m.operation, m.operation, m.time, m.context);
    }
    
    // Update bottleneck detection if enabled
    if (autoDetectBottlenecks_) {
        updateBottleneckDetection(m.operation, m.time);
    }
}

void PerformanceProfiler::recordPerformance(const std::string& operation, double time, const std::string& context) {
    Measurement m;
    m.timestamp = std::chrono::system_clock::now();
    m.operation = operation;
    m.time = time;
    m.context = context;
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    measurements_.push_back(m);
    
    totalMeasurements_++;
    totalTime_ += time;
    
    // Update function call profiler if available
    if (enableFunctionProfiling_ && functionCallProfiler_) {
        functionCallProfiler_->recordFunctionCall(operation, time, context);
    }
    
    // Update algorithm profiler if available
    if (enableAlgorithmProfiling_ && algorithmProfiler_) {
        algorithmProfiler_->recordAlgorithmOperation(operation, operation, time, context);
    }
    
    // Update bottleneck detection if enabled
    if (autoDetectBottlenecks_) {
        updateBottleneckDetection(operation, time);
    }
    
    // Update debugging system if available
    if (debuggingSystem_) {
        debuggingSystem_->recordPerformanceMetric(operation, time);
    }
}

void PerformanceProfiler::recordFunctionCall(const std::string& functionName, double time, 
                                            const std::string& context) {
    if (enableFunctionProfiling_ && functionCallProfiler_) {
        functionCallProfiler_->recordFunctionCall(functionName, time, context);
    }
}

void PerformanceProfiler::recordAlgorithmOperation(const std::string& algorithmName, 
                                                  const std::string& operation, double time,
                                                  const std::string& parameters) {
    if (enableAlgorithmProfiling_ && algorithmProfiler_) {
        algorithmProfiler_->recordAlgorithmOperation(algorithmName, operation, time, parameters);
    }
}

double PerformanceProfiler::getAverageTime(const std::string& operation) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(measurementsMutex_);
        for (const auto& m : measurements_) {
            if (m.operation == operation) {
                times.push_back(m.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double PerformanceProfiler::getMinTime(const std::string& operation) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(measurementsMutex_);
        for (const auto& m : measurements_) {
            if (m.operation == operation) {
                times.push_back(m.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    return *std::min_element(times.begin(), times.end());
}

double PerformanceProfiler::getMaxTime(const std::string& operation) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(measurementsMutex_);
        for (const auto& m : measurements_) {
            if (m.operation == operation) {
                times.push_back(m.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    return *std::max_element(times.begin(), times.end());
}

double PerformanceProfiler::getStandardDeviation(const std::string& operation) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(measurementsMutex_);
        for (const auto& m : measurements_) {
            if (m.operation == operation) {
                times.push_back(m.time);
            }
        }
    }
    
    return calculateStandardDeviation(times);
}

std::vector<std::string> PerformanceProfiler::getOperations() const {
    std::vector<std::string> operations;
    {
        std::lock_guard<std::mutex> lock(measurementsMutex_);
        std::unordered_set<std::string> uniqueOps;
        for (const auto& m : measurements_) {
            uniqueOps.insert(m.operation);
        }
        operations.assign(uniqueOps.begin(), uniqueOps.end());
    }
    return operations;
}

std::vector<std::string> PerformanceProfiler::getFunctionCalls() const {
    if (functionCallProfiler_) {
        return functionCallProfiler_->getFunctionCalls();
    }
    return {};
}

std::vector<std::string> PerformanceProfiler::getAlgorithmOperations() const {
    if (algorithmProfiler_) {
        return algorithmProfiler_->getAlgorithmOperations();
    }
    return {};
}

size_t PerformanceProfiler::getSampleCount(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    size_t count = 0;
    for (const auto& m : measurements_) {
        if (m.operation == operation) {
            count++;
        }
    }
    return count;
}

size_t PerformanceProfiler::getTotalFunctionCalls() const {
    if (functionCallProfiler_) {
        return functionCallProfiler_->getTotalCalls();
    }
    return 0;
}

size_t PerformanceProfiler::getTotalAlgorithmOperations() const {
    if (algorithmProfiler_) {
        return algorithmProfiler_->getTotalOperations();
    }
    return 0;
}

std::string PerformanceProfiler::analyzeBottlenecks() const {
    std::ostringstream oss;
    oss << "Performance Bottleneck Analysis:\n";
    oss << "===============================\n\n";
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    for (const auto& m : measurements_) {
        size_t count = getSampleCount(m.operation);
        if (count < minSamples_) continue;
        
        double avgTime = getAverageTime(m.operation);
        double minTime = getMinTime(m.operation);
        double maxTime = getMaxTime(m.operation);
        double stddev = getStandardDeviation(m.operation);
        
        oss << "Operation: " << m.operation << "\n";
        oss << "  Samples: " << count << "\n";
        oss << "  Average: " << avgTime << "s\n";
        oss << "  Min: " << minTime << "s\n";
        oss << "  Max: " << maxTime << "s\n";
        oss << "  Std Dev: " << stddev << "s\n";
        oss << "  Coefficient of Variation: " << (avgTime > 0 ? (stddev / avgTime) : 0) << "\n";
        
        if (isBottleneck(m.operation, avgTime)) {
            oss << "  *** BOTTLENECK DETECTED ***\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::analyzeFunctionLevelBottlenecks() const {
    std::ostringstream oss;
    oss << "Function-Level Bottleneck Analysis:\n";
    oss << "=================================\n\n";
    
    if (!functionCallProfiler_) {
        oss << "Function call profiler not available.\n";
        return oss.str();
    }
    
    auto functionCalls = functionCallProfiler_->getFunctionCalls();
    
    for (const auto& func : functionCalls) {
        double avgTime = functionCallProfiler_->getAverageTime(func);
        size_t count = functionCallProfiler_->getCallCount(func);
        
        if (count < minSamples_) continue;
        
        oss << "Function: " << func << "\n";
        oss << "  Calls: " << count << "\n";
        oss << "  Average time per call: " << avgTime << "s\n";
        
        // Check if function is a bottleneck
        if (avgTime > profilingInterval_ * 10) {
            oss << "  *** BOTTLENECK DETECTED ***\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::analyzeAlgorithmLevelBottlenecks() const {
    std::ostringstream oss;
    oss << "Algorithm-Level Bottleneck Analysis:\n";
    oss << "====================================\n\n";
    
    if (!algorithmProfiler_) {
        oss << "Algorithm profiler not available.\n";
        return oss.str();
    }
    
    auto algorithmOps = algorithmProfiler_->getAlgorithmOperations();
    
    for (const auto& algo : algorithmOps) {
        auto ops = algorithmProfiler_->getAlgorithmOperations(algo);
        
        for (const auto& op : ops) {
            double avgTime = algorithmProfiler_->getAverageTime(algo, op);
            size_t count = algorithmProfiler_->getOperationCount(algo, op);
            
            if (count < minSamples_) continue;
            
            oss << "Algorithm: " << algo << ", Operation: " << op << "\n";
            oss << "  Operations: " << count << "\n";
            oss << "  Average time per operation: " << avgTime << "s\n";
            
            // Check if algorithm operation is a bottleneck
            if (avgTime > profilingInterval_ * 10) {
                oss << "  *** BOTTLENECK DETECTED ***\n";
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string PerformanceProfiler::analyzeTrends() const {
    std::ostringstream oss;
    oss << "Performance Trend Analysis:\n";
    oss << "==========================\n\n";
    
    // Simple trend analysis (last vs first measurements)
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    if (measurements_.size() < 2) {
        oss << "Insufficient data for trend analysis.\n";
        return oss.str();
    }
    
    // Sort by timestamp
    std::vector<Measurement> sorted = measurements_;
    std::sort(sorted.begin(), sorted.end(),
        [](const Measurement& a, const Measurement& b) {
            return a.timestamp < b.timestamp;
        });
    
    // Calculate average for first and last halves
    size_t half = sorted.size() / 2;
    double firstHalfSum = 0.0, secondHalfSum = 0.0;
    size_t firstHalfCount = 0, secondHalfCount = 0;
    
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (i < half) {
            firstHalfSum += sorted[i].time;
            firstHalfCount++;
        } else {
            secondHalfSum += sorted[i].time;
            secondHalfCount++;
        }
    }
    
    double firstAvg = firstHalfCount > 0 ? (firstHalfSum / firstHalfCount) : 0.0;
    double secondAvg = secondHalfCount > 0 ? (secondHalfSum / secondHalfCount) : 0.0;
    
    oss << "Overall Trend:\n";
    oss << "  First half average: " << firstAvg << "s\n";
    oss << "  Second half average: " << secondAvg << "s\n";
    oss << "  Difference: " << (secondAvg - firstAvg) << "s\n";
    
    if (secondAvg > firstAvg * 1.1) {
        oss << "  Trend: Worsening (increasing time)\n";
    } else if (secondAvg < firstAvg * 0.9) {
        oss << "  Trend: Improving (decreasing time)\n";
    } else {
        oss << "  Trend: Stable\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::analyzeCorrelations(const std::string& operation1, 
                                                    const std::string& operation2) const {
    std::ostringstream oss;
    oss << "Correlation Analysis: " << operation1 << " vs " << operation2 << "\n";
    oss << "========================\n\n";
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    // Collect paired measurements (simple correlation based on timestamps)
    std::vector<std::pair<double, double>> pairs;
    for (const auto& m1 : measurements_) {
        if (m1.operation != operation1) continue;
        
        // Find measurement with similar timestamp from operation2
        for (const auto& m2 : measurements_) {
            if (m2.operation != operation2) continue;
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                m1.timestamp - m2.timestamp).count();
            if (std::abs(duration) < 100000) { // Within 0.1 seconds
                pairs.emplace_back(m1.time, m2.time);
                break;
            }
        }
    }
    
    if (pairs.empty()) {
        oss << "Insufficient paired data for correlation analysis.\n";
        return oss.str();
    }
    
    // Calculate Pearson correlation
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
    for (const auto& pair : pairs) {
        double x = pair.first;
        double y = pair.second;
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
        sumY2 += y * y;
    }
    
    size_t n = pairs.size();
    double denominator = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
    
    if (denominator > 0) {
        double correlation = (n * sumXY - sumX * sumY) / denominator;
        oss << "Pearson Correlation: " << correlation << "\n";
        
        if (std::abs(correlation) > 0.7) {
            oss << "Strong correlation detected!\n";
        } else if (std::abs(correlation) > 0.3) {
            oss << "Moderate correlation detected.\n";
        } else {
            oss << "Weak or no correlation.\n";
        }
    } else {
        oss << "Cannot calculate correlation (insufficient variance).\n";
    }
    
    return oss.str();
}

void PerformanceProfiler::recordResourceUsage(double cpu, double memory, size_t allocations, size_t threads) {
    ResourceUsage usage;
    usage.cpuUsage = cpu;
    usage.memoryUsage = memory;
    usage.memoryAllocations = allocations;
    usage.threadCount = threads;
    usage.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(resourceMutex_);
    resourceHistory_.push_back(usage);
    
    // Keep only last 1000 entries
    if (resourceHistory_.size() > 1000) {
        resourceHistory_.erase(resourceHistory_.begin());
    }
}

std::vector<PerformanceProfiler::ResourceUsage> PerformanceProfiler::getResourceHistory() const {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    return resourceHistory_;
}

bool PerformanceProfiler::exportProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string PerformanceProfiler::exportProfileAsString() const {
    std::ostringstream oss;
    oss << "Performance Profile\n";
    oss << "===================\n\n";
    
    oss << "Total Measurements: " << totalMeasurements_ << "\n";
    oss << "Total Time: " << totalTime_ << "s\n";
    oss << "Average Time per Measurement: " << (totalMeasurements_ > 0 ? totalTime_ / totalMeasurements_ : 0.0) << "s\n\n";
    
    oss << "Operations:\n";
    std::vector<std::string> operations = getOperations();
    for (const auto& op : operations) {
        size_t count = getSampleCount(op);
        oss << "  " << op << ": " << count << " samples, "
            << "avg=" << getAverageTime(op) << "s, "
            << "min=" << getMinTime(op) << "s, "
            << "max=" << getMaxTime(op) << "s\n";
    }
    
    // Add function call information if available
    if (functionCallProfiler_) {
        oss << "\nFunction Calls:\n";
        auto functionCalls = functionCallProfiler_->getFunctionCalls();
        for (const auto& func : functionCalls) {
            size_t count = functionCallProfiler_->getCallCount(func);
            double avgTime = functionCallProfiler_->getAverageTime(func);
            oss << "  " << func << ": " << count << " calls, "
                << "avg=" << avgTime << "s\n";
        }
    }
    
    // Add algorithm operation information if available
    if (algorithmProfiler_) {
        oss << "\nAlgorithm Operations:\n";
        auto algorithmOps = algorithmProfiler_->getAlgorithmOperations();
        for (const auto& algo : algorithmOps) {
            auto ops = algorithmProfiler_->getAlgorithmOperations(algo);
            for (const auto& op : ops) {
                size_t count = algorithmProfiler_->getOperationCount(algo, op);
                double avgTime = algorithmProfiler_->getAverageTime(algo, op);
                oss << "  " << algo << "." << op << ": " << count << " operations, "
                    << "avg=" << avgTime << "s\n";
            }
        }
    }
    
    oss << "\nResource Usage:\n";
    std::lock_guard<std::mutex> lock(resourceMutex_);
    oss << "  Samples: " << resourceHistory_.size() << "\n";
    if (!resourceHistory_.empty()) {
        double avgCpu = 0.0, avgMem = 0.0;
        for (const auto& usage : resourceHistory_) {
            avgCpu += usage.cpuUsage;
            avgMem += usage.memoryUsage;
        }
        avgCpu /= resourceHistory_.size();
        avgMem /= resourceHistory_.size();
        oss << "  Average CPU: " << avgCpu << "%\n";
        oss << "  Average Memory: " << avgMem << " MB\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::getComprehensivePerformanceReport() const {
    std::ostringstream oss;
    oss << "=== COMPREHENSIVE PERFORMANCE REPORT ===\n\n";
    
    oss << "Performance Summary:\n";
    oss << "  Total Measurements: " << totalMeasurements_ << "\n";
    oss << "  Total Time: " << totalTime_ << "s\n";
    oss << "  Average per Measurement: " << (totalMeasurements_ > 0 ? totalTime_ / totalMeasurements_ : 0.0) << "s\n";
    oss << "  Total Function Calls: " << getTotalFunctionCalls() << "\n";
    oss << "  Total Algorithm Operations: " << getTotalAlgorithmOperations() << "\n\n";
    
    oss << analyzeBottlenecks();
    oss << "\n";
    oss << analyzeFunctionLevelBottlenecks();
    oss << "\n";
    oss << analyzeAlgorithmLevelBottlenecks();
    oss << "\n";
    oss << analyzeTrends();
    
    return oss.str();
}

std::string PerformanceProfiler::getOptimizationRecommendations() const {
    std::ostringstream oss;
    oss << "=== OPTIMIZATION RECOMMENDATIONS ===\n\n";
    
    // Analyze bottlenecks and provide recommendations
    auto bottlenecks = analyzeBottlenecks();
    
    // Check for common optimization opportunities
    oss << "Potential Optimization Areas:\n";
    
    // Function-level optimizations
    if (functionCallProfiler_) {
        auto functionCalls = functionCallProfiler_->getFunctionCalls();
        for (const auto& func : functionCalls) {
            double avgTime = functionCallProfiler_->getAverageTime(func);
            size_t count = functionCallProfiler_->getCallCount(func);
            
            if (count >= minSamples_ && avgTime > profilingInterval_ * 5) {
                oss << "  1. Optimize function '" << func << "' (avg time: " << avgTime << "s)\n";
                oss << "     - Consider inline implementation\n";
                oss << "     - Look for SIMD optimizations\n";
                oss << "     - Reduce function call overhead\n";
            }
        }
    }
    
    // Algorithm-level optimizations
    if (algorithmProfiler_) {
        auto algorithmOps = algorithmProfiler_->getAlgorithmOperations();
        for (const auto& algo : algorithmOps) {
            auto ops = algorithmProfiler_->getAlgorithmOperations(algo);
            for (const auto& op : ops) {
                double avgTime = algorithmProfiler_->getAverageTime(algo, op);
                size_t count = algorithmProfiler_->getOperationCount(algo, op);
                
                if (count >= minSamples_ && avgTime > profilingInterval_ * 5) {
                    oss << "  2. Optimize algorithm '" << algo << "' operation '" << op << "' (avg time: " << avgTime << "s)\n";
                    oss << "     - Consider parallelizing operations\n";
                    oss << "     - Look for cache-friendly data layouts\n";
                    oss << "     - Optimize memory access patterns\n";
                }
            }
        }
    }
    
    // Memory usage optimizations
    oss << "\nMemory Optimization Recommendations:\n";
    if (debuggingSystem_) {
        // Get memory statistics from debugging system if available
        // This would integrate with the MemoryProfiler
    }
    oss << "  - Profile memory allocation patterns\n";
    oss << "  - Look for memory leaks and excessive allocations\n";
    oss << "  - Consider memory pooling for frequently allocated objects\n";
    oss << "  - Optimize data structures for cache locality\n";
    
    // Parallel optimization recommendations
    oss << "\nParallel Optimization Recommendations:\n";
    oss << "  - Identify parallelizable operations\n";
    oss << "  - Balance workload across threads\n";
    oss << "  - Minimize shared resource contention\n";
    oss << "  - Consider task-based parallelism\n";
    
    return oss.str();
}

void PerformanceProfiler::updateBottleneckDetection(const std::string& operation, double time) {
    if (getSampleCount(operation) >= minSamples_) {
        double avgTime = getAverageTime(operation);
        if (isBottleneck(operation, avgTime)) {
            Logger::getGlobal().warning("Performance bottleneck detected in operation: " + operation + ", avg time: " + std::to_string(avgTime) + "s");
        }
    }
}

bool PerformanceProfiler::isBottleneck(const std::string& operation, double avgTime) const {
    if (avgTime < 0.001) return false; // Less than 1ms is not a bottleneck
    return avgTime > profilingInterval_ * 10; // More than 10x profiling interval is a bottleneck
}

double PerformanceProfiler::calculateStandardDeviation(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    
    double sqSum = 0.0;
    for (double value : values) {
        sqSum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sqSum / values.size());
}

} // namespace nlm
