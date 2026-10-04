#include "PerformanceProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>

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
      minSamples_(5),
      totalMeasurements_(0),
      totalTime_(0.0)
{
}

PerformanceProfiler::PerformanceProfiler(std::shared_ptr<DebuggingSystem> debuggingSystem)
    : profilingInterval_(0.1),
      autoDetectBottlenecks_(true),
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
    
    measurement.~PerformanceMeasurement();
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
    
    // Update bottleneck detection if enabled
    if (autoDetectBottlenecks_) {
        updateBottleneckDetection(operation, time);
    }
    
    // Update debugging system if available
    if (debuggingSystem_) {
        debuggingSystem_->recordPerformanceMetric(operation, time);
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

std::string PerformanceProfiler::analyzeCorrelations(const std::string& operation1, const std::string& operation2) const {
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

BottleneckDetector::BottleneckDetector()
    : threshold_(0.1), timeWindow_(60.0), maxOperations_(100) {
}

BottleneckDetector::BottleneckDetector(std::shared_ptr<PerformanceProfiler> profiler)
    : profiler_(profiler), threshold_(0.1), timeWindow_(60.0), maxOperations_(100) {
}

std::vector<std::string> BottleneckDetector::detectBottlenecks() const {
    if (!profiler_) return {};
    
    std::vector<std::string> bottlenecks;
    auto operations = profiler_->getOperations();
    
    for (const auto& op : operations) {
        if (profiler_->getSampleCount(op) < profiler_->getMinSamples()) continue;
        
        double avgTime = profiler_->getAverageTime(op);
        if (isBottleneck(op, avgTime)) {
            bottlenecks.push_back(op);
        }
    }
    
    lastDetectionTime_ = std::chrono::system_clock::now();
    return bottlenecks;
}

std::string BottleneckDetector::getBottleneckReport() const {
    std::ostringstream oss;
    oss << "Bottleneck Detection Report\n";
    oss << "==========================\n\n";
    
    if (!profiler_) {
        oss << "No performance profiler available.\n";
        return oss.str();
    }
    
    auto bottlenecks = detectBottlenecks();
    
    if (bottlenecks.empty()) {
        oss << "No bottlenecks detected.\n";
    } else {
        oss << "Bottlenecks detected:\n";
        for (const auto& bottleneck : bottlenecks) {
            double avgTime = profiler_->getAverageTime(bottleneck);
            oss << "  " << bottleneck << ": " << avgTime << "s (threshold: " << threshold_ << "s)\n";
        }
    }
    
    oss << "\nDetection Time: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - lastDetectionTime_).count() << " seconds ago\n";
    
    return oss.str();
}

double BottleneckDetector::getBottleneckScore(const std::string& operation) const {
    if (!profiler_) return 0.0;
    
    double avgTime = profiler_->getAverageTime(operation);
    return calculateBottleneckScore(std::vector<double>{avgTime});
}

std::vector<BottleneckDetector::TimeSeriesPoint> BottleneckDetector::calculateOperationTimes() const {
    std::vector<TimeSeriesPoint> points;
    // Implementation would depend on the profiler's internal structure
    return points;
}

double BottleneckDetector::calculateBottleneckScore(const std::vector<double>& times) const {
    if (times.empty()) return 0.0;
    
    double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    double variance = 0.0;
    for (double time : times) {
        variance += (time - avg) * (time - avg);
    }
    variance /= times.size();
    
    return avg + std::sqrt(variance); // Score based on average + stddev
}

bool BottleneckDetector::isSignificantDeviation(double current, double average, double stddev) const {
    return stddev > 0 && std::abs(current - average) > 2 * stddev;
}

ResourceMonitor::ResourceMonitor()
    : monitoring_(false), samplingInterval_(0.5), cpuThreshold_(80.0), memoryThreshold_(90.0) {
}

ResourceMonitor::~ResourceMonitor() {
    stop();
}

void ResourceMonitor::start() {
    if (monitoring_) return;
    
    monitoring_ = true;
    monitoringThread_ = std::thread(&ResourceMonitor::monitoringLoop, this);
    
    Logger::getGlobal().info("Resource monitoring started");
}

void ResourceMonitor::stop() {
    if (!monitoring_) return;
    
    monitoring_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    Logger::getGlobal().info("Resource monitoring stopped");
}

void ResourceMonitor::monitoringLoop() {
    while (monitoring_) {
        updateResourceUsage();
        
        std::this_thread::sleep_for(std::chrono::duration<double>(samplingInterval_));
    }
}

void ResourceMonitor::updateResourceUsage() {
    // Simulate resource usage measurement
    // In a real implementation, this would query system resources
    currentCpuUsage_ = 50.0 + (std::rand() % 50); // 50-100%
    currentMemoryUsage_ = 20.0 + (std::rand() % 60); // 20-80 MB
    currentMemoryAllocations_ = 100 + (std::rand() % 200);
    currentThreadCount_ = 4 + (std::rand() % 8);
    
    if (debuggingSystem_) {
        debuggingSystem_->recordResourceUsage(
            currentCpuUsage_, currentMemoryUsage_, 
            currentMemoryAllocations_, currentThreadCount_);
    }
    
    // Add to history
    ResourceUsage usage;
    usage.cpuUsage = currentCpuUsage_;
    usage.memoryUsage = currentMemoryUsage_;
    usage.memoryAllocations = currentMemoryAllocations_;
    usage.threadCount = currentThreadCount_;
    usage.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(historyMutex_);
    resourceHistory_.push_back(usage);
    
    // Keep only last 1000 entries
    if (resourceHistory_.size() > 1000) {
        resourceHistory_.erase(resourceHistory_.begin());
    }
}

std::string ResourceMonitor::getResourceReport() const {
    std::ostringstream oss;
    oss << "Resource Monitor Report\n";
    oss << "======================\n\n";
    
    oss << "Current Usage:\n";
    oss << "  CPU: " << currentCpuUsage_ << "%\n";
    oss << "  Memory: " << currentMemoryUsage_ << " MB\n";
    oss << "  Allocations: " << currentMemoryAllocations_ << "\n";
    oss << "  Threads: " << currentThreadCount_ << "\n\n";
    
    oss << "Thresholds:\n";
    oss << "  CPU: " << cpuThreshold_ << "%\n";
    oss << "  Memory: " << memoryThreshold_ << "%\n\n";
    
    oss << "History:\n";
    oss << "  Samples: " << resourceHistory_.size() << "\n";
    if (!resourceHistory_.empty()) {
        double avgCpu = 0.0, avgMem = 0.0;
        size_t overCpu = 0, overMem = 0;
        for (const auto& usage : resourceHistory_) {
            avgCpu += usage.cpuUsage;
            avgMem += usage.memoryUsage;
            if (usage.cpuUsage > cpuThreshold_) overCpu++;
            if (usage.memoryUsage > memoryThreshold_) overMem++;
        }
        avgCpu /= resourceHistory_.size();
        avgMem /= resourceHistory_.size();
        
        oss << "  Average CPU: " << avgCpu << "%\n";
        oss << "  Average Memory: " << avgMem << " MB\n";
        oss << "  Time over CPU threshold: " << overCpu << " samples\n";
        oss << "  Time over Memory threshold: " << overMem << " samples\n";
    }
    
    return oss.str();
}

} // namespace nlm
