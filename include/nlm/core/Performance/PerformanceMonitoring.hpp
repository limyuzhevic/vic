// Performance Monitoring and Metrics Collection System for NLM
// Advanced performance monitoring with comprehensive metrics, profiling, and optimization insights

#include "core/Performance/PerformanceMonitoring.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <numeric>
#include <algorithm>

namespace nlm {

// Performance metrics
struct PerformanceMetrics {
    std::chrono::system_clock::time_point timestamp;
    double cpuUsage;           // CPU usage percentage (0-100)
    double memoryUsage;        // Memory usage percentage (0-100)
    size_t memoryAllocated;    // Memory allocated in bytes
    size_t memoryPeak;         // Peak memory usage in bytes
    size_t memoryTotal;        // Total available memory in bytes
    size_t threadCount;        // Number of active threads
    double networkUpload;      // Network upload speed in bytes/second
    double networkDownload;    // Network download speed in bytes/second
    double diskRead;           // Disk read speed in bytes/second
    double diskWrite;          // Disk write speed in bytes/second
    
    // Neural computation metrics
    size_t neuronCount;        // Number of active neurons
    size_t synapseCount;        // Number of active synapses
    size_t spikeCount;          // Number of spikes generated this interval
    size_t stepCount;           // Number of simulation steps completed
    double firingRate;         // Average firing rate (spikes per second)
    double computationTime;    // Computation time in seconds
    
    // System metrics
    double temperature;       // System temperature in Celsius
    double voltage;           // CPU voltage
    double frequency;         // CPU frequency in GHz
    double loadAverage;        // System load average (1, 5, 15 min)
    
    PerformanceMetrics() : 
        cpuUsage(0.0), memoryUsage(0.0), memoryAllocated(0), memoryPeak(0), memoryTotal(0),
        threadCount(0), networkUpload(0.0), networkDownload(0.0), diskRead(0.0), diskWrite(0.0),
        neuronCount(0), synapseCount(0), spikeCount(0), stepCount(0), firingRate(0.0), 
        computationTime(0.0), temperature(0.0), voltage(0.0), frequency(0.0), loadAverage(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Performance sample for historical data
struct PerformanceSample {
    std::chrono::system_clock::time_point timestamp;
    PerformanceMetrics metrics;
    std::map<std::string, std::string> metadata;
    
    PerformanceSample() {}
    PerformanceSample(const PerformanceMetrics& metrics, const std::map<std::string, std::string>& meta = std::map<std::string, std::string>()) 
        : metrics(metrics), metadata(meta) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Performance trend analysis
struct PerformanceTrend {
    double cpuTrend;           // CPU usage trend (positive = increasing)
    double memoryTrend;        // Memory usage trend
    double computationTrend;   // Computation trend
    double firingTrend;        // Firing rate trend
    
    PerformanceTrend() : cpuTrend(0.0), memoryTrend(0.0), computationTrend(0.0), firingTrend(0.0) {}
};

// Performance alert
struct PerformanceAlert {
    enum class AlertType {
        CPUUsageHigh,
        MemoryUsageHigh,
        SpikeRateHigh,
        ComputationSlow,
        NetworkCongested,
        DiskUsageHigh,
        TemperatureHigh,
        ResourceExhausted,
        Custom
    };
    
    AlertType type;
    std::string message;
    double value;
    double threshold;
    std::chrono::system_clock::time_point timestamp;
    std::string severity;
    
    PerformanceAlert() : type(AlertType::Custom), value(0.0), threshold(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    PerformanceAlert(AlertType t, const std::string& msg, double val, double thresh)
        : type(t), message(msg), value(val), threshold(thresh) {
        timestamp = std::chrono::system_clock::now();
        severity = (val > thresh * 1.5) ? "CRITICAL" : 
                   (val > thresh * 1.2) ? "WARNING" : "INFO";
    }
};

// Performance benchmark
struct PerformanceBenchmark {
    std::string benchmarkName;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double duration;           // Total duration in seconds
    PerformanceMetrics metrics;
    std::vector<PerformanceSample> samples;
    
    PerformanceBenchmark() : duration(0.0) {
        startTime = std::chrono::system_clock::now();
    }
    
    void stop() {
        endTime = std::chrono::system_clock::now();
        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        duration = durationMs.count() / 1000.0;
    }
    
    double getFPS() const {
        return (duration > 0.0) ? samples.size() / duration : 0.0;
    }
    
    double getAverageMetric(const std::string& metricName) const {
        // Implementation would access specific metric from samples
        return 0.0;
    }
};

// Performance monitor interface
class PerformanceMonitor {
public:
    virtual ~PerformanceMonitor() = default;
    virtual void startMonitoring() = 0;
    virtual void stopMonitoring() = 0;
    virtual void recordSample(const PerformanceMetrics& metrics) = 0;
    virtual std::vector<PerformanceSample> getSamples(size_t maxSamples = 0) const = 0;
    virtual PerformanceMetrics getCurrentMetrics() const = 0;
    virtual bool isMonitoring() const = 0;
    virtual void setSamplingInterval(double intervalSeconds) = 0;
    virtual double getSamplingInterval() const = 0;
    virtual void addAlertListener(std::function<void(const PerformanceAlert&)>) = 0;
    virtual void removeAlertListener(std::function<void(const PerformanceAlert&)>) = 0;
    virtual std::vector<PerformanceAlert> getAlerts() const = 0;
    virtual void clearAlerts() = 0;
    virtual PerformanceTrend analyzeTrend(size_t sampleCount = 10) const = 0;
    virtual PerformanceBenchmark startBenchmark(const std::string& name) = 0;
    virtual void endBenchmark(PerformanceBenchmark& benchmark) = 0;
    virtual void exportMetrics(const std::string& filepath) const = 0;
    virtual bool importMetrics(const std::string& filepath) = 0;
};

// Default performance monitor
class DefaultPerformanceMonitor : public PerformanceMonitor {
public:
    DefaultPerformanceMonitor() : samplingInterval(0.1), isMonitoringActive(false), sampleCount(0) {
        NLM_LOG_INFO("Performance monitoring initialized");
    }
    
    void startMonitoring() override {
        if (!isMonitoringActive) {
            isMonitoringActive = true;
            NLM_LOG_INFO("Performance monitoring started");
        }
    }
    
    void stopMonitoring() override {
        if (isMonitoringActive) {
            isMonitoringActive = false;
            NLM_LOG_INFO("Performance monitoring stopped");
        }
    }
    
    void recordSample(const PerformanceMetrics& metrics) override {
        auto lock = LockGuard(mutex);
        
        PerformanceSample sample(metrics);
        samples.push_back(sample);
        sampleCount++;
        
        // Check for performance alerts
        checkForAlerts(metrics);
        
        // Limit sample history if needed
        if (maxSamples > 0 && samples.size() > maxSamples) {
            samples.erase(samples.begin(), samples.begin() + (samples.size() - maxSamples));
        }
        
        NLM_LOG_DEBUG("Performance sample recorded: " + std::to_string(metrics.computationTime) + "s computation");
    }
    
    std::vector<PerformanceSample> getSamples(size_t maxSamples) const override {
        auto lock = LockGuard(mutex);
        
        if (maxSamples == 0 || maxSamples >= samples.size()) {
            return samples;
        }
        
        std::vector<PerformanceSample> result(samples.end() - maxSamples, samples.end());
        return result;
    }
    
    PerformanceMetrics getCurrentMetrics() const override {
        auto lock = LockGuard(mutex);
        
        PerformanceMetrics currentMetrics;
        if (!samples.empty()) {
            currentMetrics = samples.back().metrics;
        }
        
        // Update with real-time system metrics
        updateSystemMetrics(currentMetrics);
        
        return currentMetrics;
    }
    
    bool isMonitoring() const override {
        return isMonitoringActive;
    }
    
    void setSamplingInterval(double intervalSeconds) override {
        samplingInterval = intervalSeconds;
    }
    
    double getSamplingInterval() const override {
        return samplingInterval;
    }
    
    void addAlertListener(std::function<void(const PerformanceAlert&)> listener) override {
        alertListeners.push_back(listener);
    }
    
    void removeAlertListener(std::function<void(const PerformanceAlert&)> listener) override {
        alertListeners.erase(
            std::remove_if(alertListeners.begin(), alertListeners.end(),
                [listener](const std::function<void(const PerformanceAlert&)>& l) { return l.target<void(*)(const PerformanceAlert&)>() == listener.target<void(*)(const PerformanceAlert&)>(); }),
            alertListeners.end()
        );
    }
    
    std::vector<PerformanceAlert> getAlerts() const override {
        return alerts;
    }
    
    void clearAlerts() override {
        alerts.clear();
    }
    
    PerformanceTrend analyzeTrend(size_t sampleCount) const override {
        PerformanceTrend trend;
        
        if (samples.size() < 2) {
            return trend;
        }
        
        size_t startIdx = std::max<size_t>(0, samples.size() - sampleCount);
        std::vector<double> cpuValues, memoryValues, computationValues, firingValues;
        
        for (size_t i = startIdx; i < samples.size(); ++i) {
            cpuValues.push_back(samples[i].metrics.cpuUsage);
            memoryValues.push_back(samples[i].metrics.memoryUsage);
            computationValues.push_back(samples[i].metrics.computationTime);
            firingValues.push_back(samples[i].metrics.firingRate);
        }
        
        if (cpuValues.size() >= 2) {
            trend.cpuTrend = calculateTrend(cpuValues);
        }
        if (memoryValues.size() >= 2) {
            trend.memoryTrend = calculateTrend(memoryValues);
        }
        if (computationValues.size() >= 2) {
            trend.computationTrend = calculateTrend(computationValues);
        }
        if (firingValues.size() >= 2) {
            trend.firingTrend = calculateTrend(firingValues);
        }
        
        return trend;
    }
    
    PerformanceBenchmark startBenchmark(const std::string& name) override {
        PerformanceBenchmark benchmark;
        benchmark.benchmarkName = name;
        benchmarks.push_back(benchmark);
        currentBenchmark = benchmarks.size() - 1;
        return benchmarks[currentBenchmark];
    }
    
    void endBenchmark(PerformanceBenchmark& benchmark) override {
        if (currentBenchmark < benchmarks.size()) {
            benchmarks[currentBenchmark].stop();
            currentBenchmark = -1;
        }
    }
    
    void exportMetrics(const std::string& filepath) const override {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to export metrics to file: " + filepath);
            return;
        }
        
        file << "# NLM Performance Metrics Export" << std::endl;
        file << "# Generated: " << getCurrentTimestamp() << std::endl;
        file << std::endl;
        
        file << "Samples:" << std::endl;
        for (const auto& sample : samples) {
            file << "  Time: " << formatTime(sample.timestamp) << std::endl;
            file << "  CPU Usage: " << sample.metrics.cpuUsage << "%" << std::endl;
            file << "  Memory Usage: " << sample.metrics.memoryUsage << "%" << std::endl;
            file << "  Computation Time: " << sample.metrics.computationTime << "s" << std::endl;
            file << "  Spike Count: " << sample.metrics.spikeCount << std::endl;
            file << "  Firing Rate: " << sample.metrics.firingRate << " Hz" << std::endl;
            file << std::endl;
        }
        
        file.close();
        
        NLM_LOG_INFO("Performance metrics exported to: " + filepath);
    }
    
    bool importMetrics(const std::string& filepath) override {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to import metrics from file: " + filepath);
            return false;
        }
        
        std::string line;
        PerformanceSample currentSample;
        bool inSampleSection = false;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line.find("Samples:") != std::string::npos) {
                inSampleSection = true;
                continue;
            }
            
            if (line.find("  Time:") != std::string::npos) {
                if (!currentSample.metrics.cpuUsage) { // New sample
                    samples.push_back(currentSample);
                    currentSample = PerformanceSample();
                }
            }
            
            if (inSampleSection) {
                if (line.find("  Time:") != std::string::npos) {
                    // Extract time (simple implementation)
                    // In a real implementation, this would parse the timestamp
                } else if (line.find("  CPU Usage:") != std::string::npos) {
                    // Extract CPU usage
                    size_t colonPos = line.find(":");
                    if (colonPos != std::string::npos) {
                        std::string valueStr = line.substr(colonPos + 1);
                        valueStr.erase(0, valueStr.find_first_not_of(" "));
                        valueStr.erase(valueStr.find_last_not_of(" ") + 1);
                        try {
                            currentSample.metrics.cpuUsage = std::stod(valueStr);
                            // Remove "%" sign
                            if (!currentSample.metrics.cpuUsage) {
                                currentSample.metrics.cpuUsage = 0.0;
                            }
                        } catch (...) {}
                    }
                } else if (line.find("  Memory Usage:") != std::string::npos) {
                    // Similar parsing for other metrics...
                }
            }
        }
        
        // Add last sample
        if (currentSample.metrics.cpuUsage) {
            samples.push_back(currentSample);
        }
        
        file.close();
        
        NLM_LOG_INFO("Performance metrics imported from: " + filepath);
        return true;
    }
    
protected:
    virtual void checkForAlerts(const PerformanceMetrics& metrics) {
        std::vector<PerformanceAlert> newAlerts;
        
        // Check CPU usage
        if (metrics.cpuUsage > 90.0) {
            newAlerts.emplace_back(PerformanceAlert::AlertType::CPUUsageHigh,
                                   "High CPU usage detected",
                                   metrics.cpuUsage, 90.0);
        }
        
        // Check memory usage
        if (metrics.memoryUsage > 85.0) {
            newAlerts.emplace_back(PerformanceAlert::AlertType::MemoryUsageHigh,
                                   "High memory usage detected",
                                   metrics.memoryUsage, 85.0);
        }
        
        // Check spike rate
        if (metrics.firingRate > 1000.0) { // 1kHz threshold
            newAlerts.emplace_back(PerformanceAlert::AlertType::SpikeRateHigh,
                                   "High spike rate detected",
                                   metrics.firingRate, 1000.0);
        }
        
        // Check computation time
        if (metrics.computationTime > 0.1) { // 100ms threshold
            newAlerts.emplace_back(PerformanceAlert::AlertType::ComputationSlow,
                                   "Slow computation detected",
                                   metrics.computationTime, 0.1);
        }
        
        // Check neuron count
        if (metrics.neuronCount > 50000) { // 50k neurons threshold
            newAlerts.emplace_back(PerformanceAlert::AlertType::ResourceExhausted,
                                   "High neuron count detected",
                                   static_cast<double>(metrics.neuronCount), 50000.0);
        }
        
        // Add new alerts
        for (const auto& alert : newAlerts) {
            alerts.push_back(alert);
            
            // Notify alert listeners
            for (const auto& listener : alertListeners) {
                try {
                    listener(alert);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Alert listener error: ") + e.what());
                }
            }
        }
    }
    
    virtual void updateSystemMetrics(PerformanceMetrics& metrics) {
        // This would update metrics with real-time system data
        // For now, it's a placeholder
        metrics.cpuUsage = std::min(100.0, metrics.cpuUsage + ((rand() % 20) / 100.0 - 0.1));
        metrics.memoryUsage = std::min(100.0, metrics.memoryUsage + ((rand() % 30) / 100.0 - 0.15));
        metrics.computationTime = (rand() % 100) / 1000.0;
    }
    
    virtual double calculateTrend(const std::vector<double>& values) const {
        if (values.size() < 2) return 0.0;
        
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
        size_t n = values.size();
        
        for (size_t i = 0; i < n; ++i) {
            double x = static_cast<double>(i);
            double y = values[i];
            
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
            sumY2 += y * y;
        }
        
        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;
        
        // Calculate correlation coefficient
        double correlation = (n * sumXY - sumX * sumY) / std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
        
        // Normalize slope to [-1, 1]
        double normalizedSlope = slope / std::max(1.0, *std::max_element(values.begin(), values.end()) - *std::min_element(values.begin(), values.end()));
        
        return normalizedSlope;
    }
    
    virtual std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto inTimeT = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&inTimeT), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    virtual std::string formatTime(const std::chrono::system_clock::time_point& time) {
        auto inTimeT = std::chrono::system_clock::to_time_t(time);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&inTimeT), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
private:
    mutable std::mutex mutex;
    std::vector<PerformanceSample> samples;
    std::vector<PerformanceAlert> alerts;
    std::vector<PerformanceBenchmark> benchmarks;
    std::vector<std::function<void(const PerformanceAlert&)>> alertListeners;
    
    bool isMonitoringActive;
    double samplingInterval;
    size_t sampleCount;
    size_t maxSamples;
    int currentBenchmark;
    
    // CPU affinity and thread management
    std::vector<std::thread> monitorThreads;
    std::atomic<bool> stopMonitoring;
};

// Performance profiling
class PerformanceProfiler {
public:
    struct ProfileData {
        std::string functionName;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double duration;           // in seconds
        std::map<std::string, std::string> metadata;
        
        ProfileData(const std::string& name) : functionName(name), duration(0.0) {
            startTime = std::chrono::system_clock::now();
        }
        
        void stop() {
            endTime = std::chrono::system_clock::now();
            auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            duration = durationMs.count() / 1000.0;
        }
    };
    
    PerformanceProfiler() : currentProfile(nullptr) {}
    
    void startProfile(const std::string& functionName) {
        ProfileData profile(functionName);
        profileStack.push(profile);
    }
    
    void stopProfile() {
        if (!profileStack.empty()) {
            ProfileData profile = profileStack.top();
            profile.stop();
            profileStack.pop();
            
            // Store profile data
            profileData.push_back(profile);
            
            // Print profile if this is the top level profile
            if (profileStack.empty()) {
                printProfileReport();
            }
        }
    }
    
    std::vector<ProfileData> getProfileData() const {
        return profileData;
    }
    
    void clearProfileData() {
        profileData.clear();
    }
    
    double getTotalProfileTime() const {
        double total = 0.0;
        for (const auto& profile : profileData) {
            total += profile.duration;
        }
        return total;
    }
    
    ProfileData getProfileForFunction(const std::string& functionName) const {
        for (const auto& profile : profileData) {
            if (profile.functionName == functionName) {
                return profile;
            }
        }
        return ProfileData(""); // Return empty profile if not found
    }
    
    void printProfileReport() const {
        std::cout << std::endl;
        std::cout << "PERFORMANCE PROFILE REPORT" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << std::endl;
        
        double totalTime = getTotalProfileTime();
        
        // Sort profiles by duration (descending)
        std::vector<ProfileData> sortedProfiles = profileData;
        std::sort(sortedProfiles.begin(), sortedProfiles.end(),
            [](const ProfileData& a, const ProfileData& b) { return a.duration > b.duration; });
        
        std::cout << "Top Functions by Execution Time:" << std::endl;
        for (const auto& profile : sortedProfiles) {
            std::cout << "  " << std::left << std::setw(30) << profile.functionName 
                      << " : " << std::right << std::setw(10) << profile.duration 
                      << "s (" << (profile.duration / totalTime * 100.0) << "%)" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Summary:" << std::endl;
        std::cout << "  Total Profiles: " << profileData.size() << std::endl;
        std::cout << "  Total Time: " << totalTime << "s" << std::endl;
        std::cout << "  Average Time: " << (profileData.empty() ? 0.0 : totalTime / profileData.size()) << "s" << std::endl;
        std::cout << "  Deepest Nesting: " << getMaxNestingLevel() << std::endl;
        std::cout << std::endl;
    }
    
protected:
    virtual size_t getMaxNestingLevel() const {
        size_t maxLevel = 0;
        size_t currentLevel = 0;
        
        for (const auto& profile : profileData) {
            // Estimate nesting level based on function name (simplified)
            if (profile.functionName.find("::") != std::string::npos) {
                currentLevel = std::count(profile.functionName.begin(), profile.functionName.end(), ':') / 2 + 1;
            } else {
                currentLevel = 1;
            }
            
            maxLevel = std::max(maxLevel, currentLevel);
        }
        
        return maxLevel;
    }
    
private:
    std::stack<ProfileData> profileStack;
    std::vector<ProfileData> profileData;
};

// Global performance monitoring infrastructure
class GlobalPerformanceMonitoring {
public:
    static GlobalPerformanceMonitoring& getInstance() {
        static GlobalPerformanceMonitoring instance;
        return instance;
    }
    
    void registerMonitor(std::unique_ptr<PerformanceMonitor> monitor) {
        monitors.push_back(std::move(monitor));
    }
    
    void unregisterMonitor(PerformanceMonitor* monitor) {
        monitors.erase(
            std::remove_if(monitors.begin(), monitors.end(),
                [monitor](const std::unique_ptr<PerformanceMonitor>& m) { return m.get() == monitor; }),
            monitors.end()
        );
    }
    
    void setDefaultMonitor(std::unique_ptr<PerformanceMonitor> monitor) {
        defaultMonitor = std::move(monitor);
    }
    
    void startMonitoring() {
        if (defaultMonitor) {
            defaultMonitor->startMonitoring();
        }
        
        for (const auto& monitor : monitors) {
            monitor->startMonitoring();
        }
    }
    
    void stopMonitoring() {
        if (defaultMonitor) {
            defaultMonitor->stopMonitoring();
        }
        
        for (const auto& monitor : monitors) {
            monitor->stopMonitoring();
        }
    }
    
    void recordSample(const PerformanceMetrics& metrics) {
        if (defaultMonitor) {
            defaultMonitor->recordSample(metrics);
        }
        
        for (const auto& monitor : monitors) {
            monitor->recordSample(metrics);
        }
    }
    
    PerformanceMetrics getCurrentMetrics() const {
        if (defaultMonitor) {
            return defaultMonitor->getCurrentMetrics();
        }
        
        PerformanceMetrics emptyMetrics;
        return emptyMetrics;
    }
    
    void addAlertListener(std::function<void(const PerformanceAlert&)> listener) {
        if (defaultMonitor) {
            defaultMonitor->addAlertListener(listener);
        }
        
        for (const auto& monitor : monitors) {
            monitor->addAlertListener(listener);
        }
    }
    
    void removeAlertListener(std::function<void(const PerformanceAlert&)> listener) {
        if (defaultMonitor) {
            defaultMonitor->removeAlertListener(listener);
        }
        
        for (const auto& monitor : monitors) {
            monitor->removeAlertListener(listener);
        }
    }
    
    PerformanceBenchmark startBenchmark(const std::string& name) {
        if (defaultMonitor) {
            return defaultMonitor->startBenchmark(name);
        }
        
        PerformanceBenchmark emptyBenchmark;
        emptyBenchmark.benchmarkName = name;
        return emptyBenchmark;
    }
    
    void endBenchmark(PerformanceBenchmark& benchmark) {
        if (defaultMonitor) {
            defaultMonitor->endBenchmark(benchmark);
        }
    }
    
    std::vector<PerformanceAlert> getAlerts() const {
        std::vector<PerformanceAlert> allAlerts;
        
        if (defaultMonitor) {
            auto defaultAlerts = defaultMonitor->getAlerts();
            allAlerts.insert(allAlerts.end(), defaultAlerts.begin(), defaultAlerts.end());
        }
        
        for (const auto& monitor : monitors) {
            auto monitorAlerts = monitor->getAlerts();
            allAlerts.insert(allAlerts.end(), monitorAlerts.begin(), monitorAlerts.end());
        }
        
        return allAlerts;
    }
    
    void clearAlerts() {
        if (defaultMonitor) {
            defaultMonitor->clearAlerts();
        }
        
        for (const auto& monitor : monitors) {
            monitor->clearAlerts();
        }
    }
    
    PerformanceTrend analyzeTrend(size_t sampleCount = 10) const {
        if (defaultMonitor) {
            return defaultMonitor->analyzeTrend(sampleCount);
        }
        
        return PerformanceTrend();
    }
    
    void exportMetrics(const std::string& filepath) const {
        if (defaultMonitor) {
            defaultMonitor->exportMetrics(filepath);
        }
        
        for (const auto& monitor : monitors) {
            monitor->exportMetrics(filepath);
        }
    }
    
    bool importMetrics(const std::string& filepath) {
        bool success = false;
        
        if (defaultMonitor) {
            success = defaultMonitor->importMetrics(filepath);
        }
        
        for (const auto& monitor : monitors) {
            monitor->importMetrics(filepath);
        }
        
        return success;
    }
    
    void startProfiling(const std::string& functionName) {
        if (!profiler) {
            profiler = std::make_unique<PerformanceProfiler>();
        }
        profiler->startProfile(functionName);
    }
    
    void stopProfiling() {
        if (profiler) {
            profiler->stopProfile();
        }
    }
    
    std::vector<PerformanceProfiler::ProfileData> getProfileData() const {
        if (profiler) {
            return profiler->getProfileData();
        }
        
        return std::vector<PerformanceProfiler::ProfileData>();
    }
    
    void printProfileReport() const {
        if (profiler) {
            profiler->printProfileReport();
        }
    }
    
protected:
    virtual void setProfiler(std::unique_ptr<PerformanceProfiler> profiler) {
        this->profiler = std::move(profiler);
    }
    
private:
    GlobalPerformanceMonitoring() : profiler(nullptr) {
        registerMonitor(std::make_unique<DefaultPerformanceMonitor>());
    }
    
    std::vector<std::unique_ptr<PerformanceMonitor>> monitors;
    std::unique_ptr<PerformanceMonitor> defaultMonitor;
    std::unique_ptr<PerformanceProfiler> profiler;
    
    GlobalPerformanceMonitoring(const GlobalPerformanceMonitoring&) = delete;
    GlobalPerformanceMonitoring& operator=(const GlobalPerformanceMonitoring&) = delete;
};

// Convenience functions for performance monitoring
void startMonitoring() {
    GlobalPerformanceMonitoring::getInstance().startMonitoring();
}

void stopMonitoring() {
    GlobalPerformanceMonitoring::getInstance().stopMonitoring();
}

void recordSample(const PerformanceMetrics& metrics) {
    GlobalPerformanceMonitoring::getInstance().recordSample(metrics);
}

PerformanceMetrics getCurrentMetrics() {
    return GlobalPerformanceMonitoring::getInstance().getCurrentMetrics();
}

void addPerformanceAlertListener(std::function<void(const PerformanceAlert&)> listener) {
    GlobalPerformanceMonitoring::getInstance().addAlertListener(listener);
}

std::vector<PerformanceAlert> getPerformanceAlerts() {
    return GlobalPerformanceMonitoring::getInstance().getAlerts();
}

void clearPerformanceAlerts() {
    GlobalPerformanceMonitoring::getInstance().clearAlerts();
}

PerformanceBenchmark startBenchmark(const std::string& name) {
    return GlobalPerformanceMonitoring::getInstance().startBenchmark(name);
}

void endBenchmark(PerformanceBenchmark& benchmark) {
    GlobalPerformanceMonitoring::getInstance().endBenchmark(benchmark);
}

void startProfiling(const std::string& functionName) {
    GlobalPerformanceMonitoring::getInstance().startProfiling(functionName);
}

void stopProfiling() {
    GlobalPerformanceMonitoring::getInstance().stopProfiling();
}

void printProfileReport() {
    GlobalPerformanceMonitoring::getInstance().printProfileReport();
}

} // namespace nlm
