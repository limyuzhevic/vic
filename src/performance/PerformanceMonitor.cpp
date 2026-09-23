// Performance monitoring and profiling implementation
// Provides actual implementations for performance monitoring system

#include "PerformanceMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <numeric>
#include <algorithm>
#include <sys/resource.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

namespace nlm {

// Console performance logger implementation
class ConsolePerformanceLogger : public ILogger {
public:
    ConsolePerformanceLogger() : minLevel_(LogLevel::Info) {}
    explicit ConsolePerformanceLogger(LogLevel level) : minLevel_(level) {}
    
    void log(const LogEntry& entry) override {
        if (entry.level < minLevel_) return;
        
        std::string levelStr = logLevelToString(entry.level);
        std::string timestamp = formatTimestamp(entry.timestamp);
        
        std::cout << "[" << timestamp << "] "
                  << "[" << levelStr << "] "
                  << entry.message << std::endl;
    }
    
    void setLevel(LogLevel level) override {
        minLevel_ = level;
    }
    
    LogLevel getLevel() const override {
        return minLevel_;
    }
    
private:
    LogLevel minLevel_;
    
    std::string logLevelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
        return ss.str();
    }
};

// System resource monitor implementation
SystemResourceMonitor::SystemResourceMonitor() 
    : updateInterval_(1.0)
    , cpuUsage_(0.0)
    , memoryUsage_(0)
    , activeThreadCount_(0)
    , systemMemorySize_(0)
    , monitoring_(false)
    , previousIdle_(0)
    , previousTotal_(0) {
    // Initialize previous values
    getCurrentCPUUsage(); // This will set initial values
}

double SystemResourceMonitor::getCurrentCPUUsage() {
    // Read from /proc/stat (Linux)
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) {
        return 0.0; // Fallback value
    }
    
    std::string line;
    std::getline(statFile, line);
    if (line.substr(0, 3) != "cpu ") {
        return 0.0;
    }
    
    std::istringstream iss(line);
    std::string cpuLabel;
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal, total;
    
    iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    
    unsigned long totalLoad = user + nice + system + idle + iowait + irq + softirq + steal;
    
    if (fileLines.size() < 2) {
        previousIdle_ = idle;
        previousTotal_ = totalLoad;
        return 0.0;
    }
    
    unsigned long idleDiff = idle - previousIdle_;
    unsigned long totalDiff = totalLoad - previousTotal_;
    
    if (totalDiff == 0) {
        return 0.0;
    }
    
    double cpuUsage = (1.0 - (static_cast<double>(idleDiff) / totalDiff)) * 100.0;
    
    previousIdle_ = idle;
    previousTotal_ = totalLoad;
    
    return cpuUsage;
}

size_t SystemResourceMonitor::getCurrentMemoryUsage() {
    // Read from /proc/self/status (Linux)
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) {
        return 0; // Fallback value
    }
    
    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("VmRSS:") == 0) {
            std::istringstream iss(line);
            std::string label;
            size_t memoryKb;
            iss >> label >> memoryKb;
            return memoryKb * 1024; // Convert to bytes
        }
    }
    
    return 0;
}

size_t SystemResourceMonitor::getCurrentThreadCount() {
    // Read from /proc/self/status (Linux)
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) {
        return 0; // Fallback value
    }
    
    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("Threads:") == 0) {
            std::istringstream iss(line);
            std::string label;
            size_t threadCount;
            iss >> label >> threadCount;
            return threadCount;
        }
    }
    
    return 0;
}

size_t SystemResourceMonitor::getSystemMemorySize() {
    if (systemMemorySize_ == 0) {
        // Read from /proc/meminfo (Linux)
        std::ifstream meminfoFile("/proc/meminfo");
        if (meminfoFile.is_open()) {
            std::string line;
            while (std::getline(meminfoFile, line)) {
                if (line.find("MemTotal:") == 0) {
                    std::istringstream iss(line);
                    std::string label;
                    size_t memoryKb;
                    iss >> label >> memoryKb;
                    systemMemorySize_ = memoryKb * 1024; // Convert to bytes
                    break;
                }
            }
        }
        
        // Fallback if not found
        if (systemMemorySize_ == 0) {
            systemMemorySize_ = 8 * 1024 * 1024 * 1024; // Default to 8GB
        }
    }
    
    return systemMemorySize_;
}

// Performance profiler implementation
PerformanceMetrics PerformanceProfiler::getOverallMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    PerformanceMetrics summary = currentMetrics_;
    
    if (!stepTimes_.empty()) {
        double sum = std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0);
        summary.totalTime = sum;
        summary.wallClockTime = totalTime_;
        
        // Calculate average active neurons from spike counts
        if (!spikeCounts_.empty()) {
            double totalSpikes = std::accumulate(spikeCounts_.begin(), spikeCounts_.end(), 0.0);
            summary.averageActiveNeurons = static_cast<size_t>(totalSpikes / stepTimes_.size());
        }
        
        // Calculate average firing rate
        if (!firingRates_.empty()) {
            double sumFiringRates = std::accumulate(firingRates_.begin(), firingRates_.end(), 0.0);
            summary.averageFiringRate = static_cast<float>(sumFiringRates / firingRates_.size());
        }
        
        // Calculate learning rate trend
        if (!learningRates_.empty()) {
            double sumLearning = std::accumulate(learningRates_.begin(), learningRates_.end(), 0.0);
            summary.learningRate = static_cast<float>(sumLearning / learningRates_.size());
        }
        
        // Calculate plasticity trend
        if (!plasticities_.empty()) {
            double sumPlasticity = std::accumulate(plasticities_.begin(), plasticities_.end(), 0.0);
            summary.plasticity = static_cast<float>(sumPlasticity / plasticities_.size());
        }
        
        // Calculate prediction accuracy trend
        if (!predictionAccuracies_.empty()) {
            double sumAccuracy = std::accumulate(predictionAccuracies_.begin(), predictionAccuracies_.end(), 0.0);
            summary.predictionAccuracy = static_cast<float>(sumAccuracy / predictionAccuracies_.size());
        }
    }
    
    return summary;
}

bool PerformanceProfiler::saveMetrics(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open metrics file: " + filename);
        return false;
    }
    
    // Write metrics in JSON format
    file << "{\n";
    file << "  \"metadata\": {\n";
    file << "    \"totalSteps\": " << currentStep_ << ",\n";
    file << "    \"startTime\": \"" << getCurrentTimestamp() << "\"\n";
    file << "  },\n";
    
    // Write overall metrics
    PerformanceMetrics summary = getOverallMetrics();
    file << "  \"overallMetrics\": {\n";
    file << "    \"totalTime\": " << summary.totalTime << ",\n";
    file << "    \"wallClockTime\": " << summary.wallClockTime << ",\n";
    file << "    \"totalSpikes\": " << summary.totalSpikes << ",\n";
    file << "    \"averageActiveNeurons\": " << summary.averageActiveNeurons << ",\n";
    file << "    \"averageFiringRate\": " << summary.averageFiringRate << ",\n";
    file << "    \"peakMemoryUsage\": " << summary.peakMemoryUsage << ",\n";
    file << "    \"cpuUsage\": " << summary.cpuUsage << ",\n";
    file << "    \"learningRate\": " << summary.learningRate << ",\n";
    file << "    \"plasticity\": " << summary.plasticity << ",\n";
    file << "    \"predictionAccuracy\": " << summary.predictionAccuracy << "\n";
    file << "  },\n";
    
    // Write step-by-step metrics
    file << "  \"stepMetrics\": [\n";
    for (size_t i = 0; i < stepTimes_.size(); ++i) {
        file << "    {\"step\": " << i << ", \"time\": " << stepTimes_[i];
        
        if (i < spikeCounts_.size()) {
            file << ", \"spikes\": " << spikeCounts_[i];
        }
        
        if (i < firingRates_.size()) {
            file << ", \"firingRate\": " << firingRates_[i];
        }
        
        if (i < memoryUsages_.size()) {
            file << ", \"memoryUsage\": " << memoryUsages_[i];
        }
        
        file << "}";
        
        if (i < stepTimes_.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "  ],\n";
    
    // Write learning metrics
    file << "  \"learningMetrics\": [\n";
    for (size_t i = 0; i < learningRates_.size(); ++i) {
        file << "    {\"step\": " << i << ", \"learningRate\": " << learningRates_[i]
             << ", \"plasticity\": " << plasticities_[i]
             << ", \"predictionAccuracy\": " << predictionAccuracies_[i] << "}";
        
        if (i < learningRates_.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    NLM_LOG_INFO("Metrics saved to: " + filename);
    return true;
}

std::string PerformanceProfiler::generatePerformanceReport() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream report;
    report << "NLM Performance Report" << std::endl;
    report << "=" << std::string(50, '=') << std::endl;
    report << "Generated at: " << getCurrentTimestamp() << std::endl;
    report << "Total Steps: " << currentStep_ << std::endl;
    report << std::endl;
    
    PerformanceMetrics summary = getOverallMetrics();
    
    report << "Performance Summary:" << std::endl;
    report << "  Execution Time: " << summary.totalTime << " seconds" << std::endl;
    report << "  Steps per Second: " << (currentStep_ / std::max(summary.totalTime, 1.0)) << std::endl;
    report << "  Average Step Time: " << (summary.totalTime / std::max<size_t>(currentStep_, 1)) << " seconds" << std::endl;
    report << "  Total Neural Activity: " << summary.totalSpikes << " spikes" << std::endl;
    report << "  Average Firing Rate: " << summary.averageFiringRate << " Hz" << std::endl;
    report << "  Memory Efficiency: " << (summary.totalSpikes / std::max(summary.peakMemoryUsage, 1)) << " spikes/byte" << std::endl;
    report << "  Learning Progress: " << (summary.learningRate > 0.0f ? "Improving" : "Stable") << std::endl;
    report << "  Prediction Accuracy: " << (summary.predictionAccuracy > 0.0f ? "Active" : "Inactive") << std::endl;
    
    report << std::endl;
    report << "Step-by-Step Performance:" << std::endl;
    report << "  Min Step Time: " << (stepTimes_.empty() ? 0.0 : *std::min_element(stepTimes_.begin(), stepTimes_.end())) << " seconds" << std::endl;
    report << "  Max Step Time: " << (stepTimes_.empty() ? 0.0 : *std::max_element(stepTimes_.begin(), stepTimes_.end())) << " seconds" << std::endl;
    report << "  Median Step Time: " << (stepTimes_.empty() ? 0.0 : stepTimes_[stepTimes_.size() / 2]) << " seconds" << std::endl;
    
    if (!spikeCounts_.empty()) {
        double avgSpikes = std::accumulate(spikeCounts_.begin(), spikeCounts_.end(), 0.0) / spikeCounts_.size();
        report << "  Average Spikes per Step: " << avgSpikes << std::endl;
    }
    
    if (!firingRates_.empty()) {
        double avgFiringRate = std::accumulate(firingRates_.begin(), firingRates_.end(), 0.0) / firingRates_.size();
        report << "  Average Firing Rate: " << avgFiringRate << " Hz" << std::endl;
    }
    
    return report.str();
}

// Performance metrics collector implementation
PerformanceMetricsCollector::PerformanceMetricsCollector() : profiler_(std::make_shared<PerformanceProfiler>()), 
                                         monitor_(std::make_shared<SystemResourceMonitor>()) {
}

void PerformanceMetricsCollector::startCollection() {
    profiler_->reset();
    monitor_->startMonitoring();
}

void PerformanceMetricsCollector::stopCollection() {
    monitor_->stopMonitoring();
}

} // namespace nlm