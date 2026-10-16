#include "PerformanceMetrics.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <sys/resource.h> // For memory usage
#include <unistd.h> // For getrusage
#include <map>
#include <unordered_set>

namespace nlm {

struct PerformanceMetrics::Impl {
    std::unordered_map<std::string, PerformanceMetric> metrics;
    std::unordered_map<std::string, std::chrono::microseconds> timerDurations;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timerStarts;
    std::unordered_map<std::string, std::chrono::microseconds> timerTotalDurations;
    
    std::vector<size_t> memoryHistory;
    size_t peakMemoryUsage{0};
    
    std::vector<double> cpuUsageHistory;
    std::vector<std::pair<size_t, std::chrono::microseconds>> ioHistory;
    
    std::chrono::high_resolution_clock::time_point startTime;
    
    mutable std::mutex metricsMutex;
    mutable std::mutex memoryMutex;
    mutable std::mutex cpuMutex;
    mutable std::mutex ioMutex;
    
    bool initialized{false};
};

PerformanceMetrics::PerformanceMetrics() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->initialized = true;
}

PerformanceMetrics::PerformanceMetrics(bool initialized) : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->initialized = initialized;
}

PerformanceMetrics::~PerformanceMetrics() = default;

void PerformanceMetrics::recordMetric(const std::string& name, PerformanceCategory category,
                     double value, const std::string& unit,
                     const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    PerformanceMetric metric;
    metric.name = name;
    metric.category = category;
    metric.value = value;
    metric.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
    metric.unit = unit;
    metric.metadata = metadata;
    
    pImpl->metrics[name] = metric;
}

double PerformanceMetrics::getMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto it = pImpl->metrics.find(name);
    if (it != pImpl->metrics.end()) {
        return it->second.value;
    }
    return 0.0;
}

std::vector<PerformanceMetric> PerformanceMetrics::getMetrics(PerformanceCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<PerformanceMetric> result;
    for (const auto& [name, metric] : pImpl->metrics) {
        if (metric.category == category) {
            result.push_back(metric);
        }
    }
    return result;
}

std::vector<PerformanceMetric> PerformanceMetrics::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<PerformanceMetric> result;
    result.reserve(pImpl->metrics.size());
    for (const auto& [name, metric] : pImpl->metrics) {
        result.push_back(metric);
    }
    return result;
}

void PerformanceMetrics::startTimer(const std::string& timerName) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->timerStarts[timerName] = std::chrono::high_resolution_clock::now();
}

std::chrono::microseconds PerformanceMetrics::stopTimer(const std::string& timerName) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto it = pImpl->timerStarts.find(timerName);
    if (it == pImpl->timerStarts.end()) {
        return std::chrono::microseconds(0);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second);
    
    pImpl->timerDurations[timerName] = duration;
    pImpl->timerTotalDurations[timerName] += duration;
    pImpl->timerStarts.erase(it);
    
    return duration;
}

std::chrono::microseconds PerformanceMetrics::getTimerDuration(const std::string& timerName) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto it = pImpl->timerDurations.find(timerName);
    if (it != pImpl->timerDurations.end()) {
        return it->second;
    }
    
    auto totalIt = pImpl->timerTotalDurations.find(timerName);
    if (totalIt != pImpl->timerTotalDurations.end()) {
        return totalIt->second;
    }
    
    return std::chrono::microseconds(0);
}

std::unordered_map<std::string, std::chrono::microseconds> PerformanceMetrics::getAllTimers() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::unordered_map<std::string, std::chrono::microseconds> result;
    
    // Add individual timer durations
    for (const auto& [name, duration] : pImpl->timerDurations) {
        result[name] = duration;
    }
    
    // Add accumulated durations
    for (const auto& [name, total] : pImpl->timerTotalDurations) {
        if (result.find(name) == result.end()) {
            result[name] = total;
        }
    }
    
    return result;
}

void PerformanceMetrics::recordMemorySnapshot() {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    // Get current memory usage (Linux-specific)
    size_t memoryUsage = 0;
    
    // Try to get memory usage from /proc/self/status
    std::ifstream statusFile("/proc/self/status");
    if (statusFile.is_open()) {
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.compare(0, 6, "VmRSS:") == 0) {
                // Parse VmRSS (resident set size)
                size_t pos = line.find_first_of(' ');
                if (pos != std::string::npos) {
                    std::string valueStr = line.substr(pos + 1);
                    memoryUsage = std::stoull(valueStr) * 1024; // Convert kB to bytes
                }
                break;
            }
        }
        statusFile.close();
    }
    
    pImpl->memoryHistory.push_back(memoryUsage);
    pImpl->peakMemoryUsage = std::max(pImpl->peakMemoryUsage, memoryUsage);
    
    // Keep history manageable
    if (pImpl->memoryHistory.size() > 1000) {
        pImpl->memoryHistory.erase(pImpl->memoryHistory.begin());
    }
}

size_t PerformanceMetrics::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    if (!pImpl->memoryHistory.empty()) {
        return pImpl->memoryHistory.back();
    }
    return 0;
}

size_t PerformanceMetrics::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    return pImpl->peakMemoryUsage;
}

std::vector<size_t> PerformanceMetrics::getMemoryHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    return pImpl->memoryHistory;
}

void PerformanceMetrics::recordCPUSample(double usage) {
    std::lock_guard<std::mutex> lock(pImpl->cpuMutex);
    
    pImpl->cpuUsageHistory.push_back(usage);
    
    if (pImpl->cpuUsageHistory.size() > 1000) {
        pImpl->cpuUsageHistory.erase(pImpl->cpuUsageHistory.begin());
    }
}

void PerformanceMetrics::recordIOSample(size_t bytes, std::chrono::microseconds duration) {
    std::lock_guard<std::mutex> lock(pImpl->ioMutex);
    
    pImpl->ioHistory.emplace_back(bytes, duration);
    
    if (pImpl->ioHistory.size() > 1000) {
        pImpl->ioHistory.erase(pImpl->ioHistory.begin());
    }
}

double PerformanceMetrics::getAverageCPUUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->cpuMutex);
    
    if (pImpl->cpuUsageHistory.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(pImpl->cpuUsageHistory.begin(), pImpl->cpuUsageHistory.end(), 0.0);
    return sum / pImpl->cpuUsageHistory.size();
}

std::vector<std::pair<size_t, std::chrono::microseconds>> PerformanceMetrics::getIOHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->ioMutex);
    return pImpl->ioHistory;
}

size_t PerformanceMetrics::getMetricCount() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->metrics.size();
}

size_t PerformanceMetrics::getTimerCount() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->timerStarts.size() + pImpl->timerDurations.size() + pImpl->timerTotalDurations.size();
}

std::chrono::microseconds PerformanceMetrics::getTotalMeasurementTime() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(endTime - pImpl->startTime);
}

bool PerformanceMetrics::exportMetrics(const std::string& filename, OutputFormat format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case OutputFormat::Text:
            file << getTextMetricsReport();
            break;
        case OutputFormat::JSON:
            file << getJSONMetricsReport();
            break;
        case OutputFormat::CSV:
            file << getCSVMetricsReport();
            break;
        case OutputFormat::HTML:
            file << getHTMLMetricsReport();
            break;
    }
    
    file.close();
    return true;
}

bool PerformanceMetrics::importMetrics(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple import implementation - would need proper parsing
    file.close();
    return true;
}

void PerformanceMetrics::clearMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->metrics.clear();
}

void PerformanceMetrics::resetTimers() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->timerStarts.clear();
    pImpl->timerDurations.clear();
    pImpl->timerTotalDurations.clear();
}

void PerformanceMetrics::clearMemoryHistory() {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    pImpl->memoryHistory.clear();
    pImpl->peakMemoryUsage = 0;
}

// Report generation methods
std::string PerformanceMetrics::getTextMetricsReport() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "=== NLM Performance Metrics Report ===\n\n";
    
    ss << "Metrics by category:\n";
    std::unordered_map<PerformanceCategory, std::vector<PerformanceMetric>> categorizedMetrics;
    
    for (const auto& [name, metric] : pImpl->metrics) {
        categorizedMetrics[metric.category].push_back(metric);
    }
    
    for (const auto& [category, metrics] : categorizedMetrics) {
        ss << static_cast<int>(category) << ". " << enumToString(category) << ":\n";
        for (const auto& metric : metrics) {
            ss << "  " << metric.name << ": " << metric.value << " " << metric.unit
               << " (" << metric.timestamp.count() << " us)\n";
        }
        ss << "\n";
    }
    
    ss << "Memory Statistics:\n";
    ss << "  Current: " << getCurrentMemoryUsage() / 1024.0 / 1024.0 << " MB\n";
    ss << "  Peak: " << getPeakMemoryUsage() / 1024.0 / 1024.0 << " MB\n";
    ss << "  Samples: " << pImpl->memoryHistory.size() << "\n\n";
    
    ss << "CPU Statistics:\n";
    ss << "  Average Usage: " << getAverageCPUUsage() << "%\n";
    ss << "  Samples: " << pImpl->cpuUsageHistory.size() << "\n\n";
    
    ss << "I/O Statistics:\n";
    ss << "  Operations: " << pImpl->ioHistory.size() << "\n";
    double totalBytes = 0;
    std::chrono::microseconds totalDuration = std::chrono::microseconds(0);
    for (const auto& [bytes, duration] : pImpl->ioHistory) {
        totalBytes += bytes;
        totalDuration += duration;
    }
    ss << "  Total Bytes: " << totalBytes << "\n";
    ss << "  Total Duration: " << totalDuration.count() << " us\n";
    
    return ss.str();
}

std::string PerformanceMetrics::getJSONMetricsReport() const {
    // Simple JSON implementation - would use proper JSON library in production
    std::stringstream ss;
    ss << "{\n  \"metrics\": [";
    
    bool first = true;
    for (const auto& [name, metric] : pImpl->metrics) {
        if (!first) ss << ",\n";
        first = false;
        ss << "{\"name\": \"" << name << "\", \"category\": " << static_cast<int>(metric.category)
           << ", \"value\": " << metric.value << ", \"unit\": \"" << metric.unit
           << "\", \"timestamp\": " << metric.timestamp.count() << "}";
    }
    
    ss << "],\n  \"memoryHistory\": [";
    first = true;
    for (size_t usage : pImpl->memoryHistory) {
        if (!first) ss << ",\n";
        first = false;
        ss << usage;
    }
    ss << "],\n  \"cpuUsageHistory\": [";
    first = true;
    for (double usage : pImpl->cpuUsageHistory) {
        if (!first) ss << ",\n";
        first = false;
        ss << usage;
    }
    ss << "]\n}";
    
    return ss.str();
}

std::string PerformanceMetrics::getCSVMetricsReport() const {
    std::stringstream ss;
    ss << "name,category,value,unit,timestamp\n";
    
    for (const auto& [name, metric] : pImpl->metrics) {
        ss << name << "," << static_cast<int>(metric.category) << ","
           << metric.value << ",\"" << metric.unit << "\","
           << metric.timestamp.count() << "\n";
    }
    
    return ss.str();
}

std::string PerformanceMetrics::getHTMLMetricsReport() const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head><title>NLM Performance Metrics</title></head>\n<body>\n";
    ss << "<h1>NLM Performance Metrics Report</h1>\n";
    ss << getTextMetricsReport();
    ss << "</body>\n</html>";
    
    return ss.str();
}

// Helper function to convert enum to string
inline std::string enumToString(PerformanceCategory category) {
    switch (category) {
        case PerformanceCategory::NeuralComputation: return "Neural Computation";
        case PerformanceCategory::MemoryManagement: return "Memory Management";
        case PerformanceCategory::Connectivity: return "Connectivity";
        case PerformanceCategory::Integration: return "Integration";
        case PerformanceCategory::Development: return "Development";
        case PerformanceCategory::Overall: return "Overall";
        default: return "Unknown";
    }
}

// Singleton implementation
namespace {
    std::shared_ptr<PerformanceMetrics> g_metricsInstance;
    std::mutex g_metricsMutex;
}

PerformanceMetrics& PerformanceMetrics::getInstance() {
    std::lock_guard<std::mutex> lock(g_metricsMutex);
    if (!g_metricsInstance) {
        g_metricsInstance = std::shared_ptr<PerformanceMetrics>(new PerformanceMetrics());
    }
    return *g_metricsInstance;
}

void PerformanceMetrics::setInstance(std::shared_ptr<PerformanceMetrics> metrics) {
    std::lock_guard<std::mutex> lock(g_metricsMutex);
    g_metricsInstance = metrics;
}

bool PerformanceMetrics::isInitialized() {
    std::lock_guard<std::mutex> lock(g_metricsMutex);
    return g_metricsInstance != nullptr && g_metricsInstance->pImpl->initialized;
}

} // namespace nlm

nlm::PerformanceMetrics& getGlobalPerformanceMetrics() {
    return nlm::PerformanceMetrics::getInstance();
}