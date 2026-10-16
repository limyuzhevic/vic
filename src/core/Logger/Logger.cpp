#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <ctime>

namespace nlm {

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

// Helper functions
std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string levelToColor(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

} // namespace nlm

// FileLogger implementation
struct FileLogger::Impl {
    std::string filepath;
    std::ofstream fileOut;
    LogLevel level = LogLevel::Info;
    std::mutex mutex;
    
    LogRotationConfig rotationConfig;
    size_t currentFileSize = 0;
    int fileIndex = 0;
    
    std::function<bool(const LogEntry&)> logHook;
    std::function<bool(const NeuralStats&)> neuralStatsHook;
    std::function<bool(const PerformanceInfo&)> performanceHook;
    
    std::vector<std::shared_ptr<LogEventCallback>> eventCallbacks;
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->rotationConfig.maxFileSizeMB = 10;
    pImpl->rotationConfig.maxFiles = 5;
    pImpl->rotationConfig.compressOldFiles = false;
    pImpl->rotationConfig.filePattern = "logger_%Y%m%d_%H%M%S.log";
}

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->filepath = filepath;
    pImpl->level = level;
    
    pImpl->rotationConfig.maxFileSizeMB = 10;
    pImpl->rotationConfig.maxFiles = 5;
    pImpl->rotationConfig.compressOldFiles = false;
    pImpl->rotationConfig.filePattern = "logger_%Y%m%d_%H%M%S.log";
    
    pImpl->fileOut.open(filepath, std::ios::app);
}

FileLogger::~FileLogger() {
    close();
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->rotateIfNeeded();
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Apply log hook if set
    if (pImpl->logHook && !pImpl->logHook(entry)) {
        return;
    }
    
    // Write to file
    pImpl->fileOut << output << "\n";
    pImpl->fileOut.flush();
    pImpl->currentFileSize += output.size() + 1; // +1 for newline
    
    // Call event callbacks
    for (auto& callback : pImpl->eventCallbacks) {
        callback->onLogEntryAdded(entry);
    }
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::logPerformance(const PerformanceInfo& perf) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[PERF] " << perf.operation << " - Duration: " 
        << std::fixed << std::setprecision(3) << perf.duration << "s"
        << " - Category: " << perf.category;
    
    std::string output = oss.str();
    
    if (pImpl->performanceHook && !pImpl->performanceHook(perf)) {
        return;
    }
    
    pImpl->fileOut << output << "\n";
    pImpl->fileOut.flush();
}

void FileLogger::logNeuralStats(const NeuralStats& stats) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[NEURAL] Spikes: " << stats.totalSpikes 
        << " - Active: " << stats.activeNeurons
        << " - Firing: " << stats.firingNeurons
        << " - Firing Rate: " << std::fixed << std::setprecision(2) << stats.averageFiringRate << "Hz";
    
    std::string output = oss.str();
    
    if (pImpl->neuralStatsHook && !pImpl->neuralStatsHook(stats)) {
        return;
    }
    
    pImpl->fileOut << output << "\n";
    pImpl->fileOut.flush();
}

void FileLogger::logEvent(const std::string& eventType, const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[EVENT] " << eventType << " - " << data;
    
    pImpl->fileOut << oss.str() << "\n";
    pImpl->fileOut.flush();
}

void FileLogger::addEventCallback(std::shared_ptr<LogEventCallback> callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->eventCallbacks.push_back(callback);
}

void FileLogger::setLogRotation(const LogRotationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationConfig = config;
}

void FileLogger::close() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool FileLogger::isOpen() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->fileOut.is_open();
}

std::string FileLogger::getCurrentFilePath() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->filepath;
}

void FileLogger::setLogHook(std::function<bool(const LogEntry&)> hook) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logHook = hook;
}

void FileLogger::setNeuralStatsHook(std::function<bool(const NeuralStats&)> hook) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->neuralStatsHook = hook;
}

void FileLogger::setPerformanceHook(std::function<bool(const PerformanceInfo&)> hook) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceHook = hook;
}

void FileLogger::rotateIfNeeded() {
    if (pImpl->currentFileSize >= pImpl->rotationConfig.maxFileSizeMB * 1024 * 1024) {
        close();
        pImpl->fileIndex++;
        
        // Generate new filename
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_time;
        localtime_r(&time, &tm_time);
        
        std::ostringstream oss;
        oss << "logger_";
        oss << std::put_time(&tm_time, "%Y%m%d_%H%M%S");
        oss << ".log";
        
        pImpl->filepath = oss.str();
        pImpl->fileOut.open(pImpl->filepath, std::ios::app);
        
        if (pImpl->fileOut.is_open()) {
            // Notify about file rotation
            for (auto& callback : pImpl->eventCallbacks) {
                callback->onFileRotation(pImpl->filepath, pImpl->filepath);
            }
        }
        
        pImpl->currentFileSize = 0;
    }
}

std::string FileLogger::getRotatedFilePath(int index) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (index == pImpl->fileIndex) {
        return pImpl->filepath;
    }
    
    // Simple implementation for rotated files
    std::ostringstream oss;
    oss << "logger_";
    if (index >= 0) {
        oss << std::setfill('0') << std::setw(3) << index;
    }
    oss << ".log";
    return oss.str();
}

} // namespace nlm

// StatisticsCollector implementation
struct StatisticsCollector::Impl {
    // Neural activity statistics
    size_t totalSpikes = 0;
    size_t activeNeurons = 0;
    size_t firingNeurons = 0;
    float averageFiringRate = 0.0f;
    float excitationInhibitionRatio = 0.0f;
    
    std::map<std::string, double> firingRates;
    std::vector<std::pair<double, size_t>> spikeHistogram;
    
    // Performance metrics
    std::vector<PerformanceInfo> performanceHistory;
    std::vector<std::pair<double, size_t>> simulationHistory;
    std::vector<uint64_t> memoryHistory;
    
    // System-level metrics
    std::vector<double> temperatures;
    std::vector<double> networkLatencies;
    std::vector<double> dataThroughputs;
    
    // Statistical analysis helpers
    std::map<std::string, std::vector<double>> metricHistory;
};

StatisticsCollector::StatisticsCollector() : pImpl(std::make_unique<Impl>()) {}

StatisticsCollector::~StatisticsCollector() = default;

void StatisticsCollector::recordSpike(size_t neuronId, double time) {
    pImpl->totalSpikes++;
    pImpl->firingNeurons++;
    pImpl->averageFiringRate = static_cast<float>(pImpl->totalSpikes) / (pImpl->spikeHistogram.size() + 1);
    pImpl->spikeHistogram.push_back({time, neuronId});
}

void StatisticsCollector::recordFiringRate(float rate) {
    pImpl->firingRates["average"] = rate;
    pImpl->averageFiringRate = rate;
}

void StatisticsCollector::recordExcitationInhibitionRatio(float ratio) {
    pImpl->excitationInhibitionRatio = ratio;
}

void StatisticsCollector::updateNeuralPopulationStats(const std::string& population, float rate) {
    pImpl->firingRates[population] = rate;
}

void StatisticsCollector::recordOperation(const std::string& operation, double duration, uint64_t memoryUsage) {
    PerformanceInfo perf;
    perf.operation = operation;
    perf.duration = duration;
    perf.memoryUsage = memoryUsage;
    perf.startTime = std::chrono::system_clock::now();
    perf.endTime = std::chrono::system_clock::now();
    
    pImpl->performanceHistory.push_back(perf);
    pImpl->metricHistory["duration"].push_back(duration);
    pImpl->metricHistory["memoryUsage"].push_back(static_cast<double>(memoryUsage));
}

void StatisticsCollector::recordSimulationStep(double stepDuration, size_t stepNumber) {
    pImpl->simulationHistory.push_back({stepDuration, stepNumber});
    pImpl->metricHistory["stepDuration"].push_back(stepDuration);
}

void StatisticsCollector::recordMemoryUsage(uint64_t memoryBytes) {
    pImpl->memoryHistory.push_back(memoryBytes);
    pImpl->metricHistory["memoryUsage"].push_back(static_cast<double>(memoryBytes));
}

void StatisticsCollector::recordTemperature(double temp) {
    pImpl->temperatures.push_back(temp);
    pImpl->metricHistory["temperature"].push_back(temp);
}

void StatisticsCollector::recordNetworkLatency(double latency) {
    pImpl->networkLatencies.push_back(latency);
    pImpl->metricHistory["networkLatency"].push_back(latency);
}

void StatisticsCollector::recordDataThroughput(double throughput) {
    pImpl->dataThroughputs.push_back(throughput);
    pImpl->metricHistory["dataThroughput"].push_back(throughput);
}

NeuralStats StatisticsCollector::getNeuralStats() const {
    NeuralStats stats;
    stats.totalSpikes = pImpl->totalSpikes;
    stats.activeNeurons = pImpl->activeNeurons;
    stats.firingNeurons = pImpl->firingNeurons;
    stats.averageFiringRate = pImpl->averageFiringRate;
    stats.excitationInhibitionRatio = pImpl->excitationInhibitionRatio;
    stats.firingRates = pImpl->firingRates;
    stats.spikeHistogram = pImpl->spikeHistogram;
    return stats;
}

std::vector<PerformanceInfo> StatisticsCollector::getPerformanceStats(size_t maxCount) const {
    if (pImpl->performanceHistory.size() <= maxCount) {
        return pImpl->performanceHistory;
    }
    
    size_t start = pImpl->performanceHistory.size() - maxCount;
    return std::vector<PerformanceInfo>(pImpl->performanceHistory.begin() + start, pImpl->performanceHistory.end());
}

std::map<std::string, std::vector<double>> StatisticsCollector::getMetricHistory() const {
    return pImpl->metricHistory;
}

double StatisticsCollector::calculateMovingAverage(const std::string& metric, size_t windowSize) const {
    if (!pImpl->metricHistory.count(metric) || pImpl->metricHistory.at(metric).size() < windowSize) {
        return 0.0;
    }
    
    const auto& values = pImpl->metricHistory.at(metric);
    double sum = 0.0;
    size_t start = values.size() - windowSize;
    
    for (size_t i = start; i < values.size(); ++i) {
        sum += values[i];
    }
    
    return sum / windowSize;
}

double StatisticsCollector::calculateStandardDeviation(const std::string& metric) const {
    if (!pImpl->metricHistory.count(metric) || pImpl->metricHistory.at(metric).size() < 2) {
        return 0.0;
    }
    
    const auto& values = pImpl->metricHistory.at(metric);
    double sum = 0.0, sumSq = 0.0;
    
    for (double value : values) {
        sum += value;
        sumSq += value * value;
    }
    
    double mean = sum / values.size();
    return std::sqrt((sumSq / values.size()) - (mean * mean));
}

std::string StatisticsCollector::generateStatisticalReport() const {
    std::ostringstream oss;
    oss << "=== Neural Statistics Report ===" << std::endl;
    
    NeuralStats stats = getNeuralStats();
    oss << "Spikes: " << stats.totalSpikes << std::endl;
    oss << "Active Neurons: " << stats.activeNeurons << std::endl;
    oss << "Firing Neurons: " << stats.firingNeurons << std::endl;
    oss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
        << stats.averageFiringRate << " Hz" << std::endl;
    oss << "Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(3) 
        << stats.excitationInhibitionRatio << std::endl;
    
    oss << "\n=== Performance Metrics ===" << std::endl;
    auto perfStats = getPerformanceStats();
    oss << "Recorded operations: " << perfStats.size() << std::endl;
    if (!perfStats.empty()) {
        double totalDuration = 0.0;
        for (const auto& perf : perfStats) {
            totalDuration += perf.duration;
        }
        oss << "Average operation duration: " << std::fixed << std::setprecision(3) 
            << (totalDuration / perfStats.size()) << "s" << std::endl;
    }
    
    oss << "\n=== System Metrics ===" << std::endl;
    oss << "Metric history points: " << pImpl->metricHistory.size() << std::endl;
    
    return oss.str();
}

bool StatisticsCollector::exportToFile(const std::string& filepath) const {
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        return false;
    }
    
    outFile << generateStatisticalReport();
    outFile.close();
    return true;
}

bool StatisticsCollector::importFromFile(const std::string& filepath) {
    // Simple import implementation - would normally parse the file
    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        return false;
    }
    
    // Clear current data
    pImpl->metricHistory.clear();
    pImpl->performanceHistory.clear();
    
    inFile.close();
    return true;
}

// RealTimeMonitor implementation
struct RealTimeMonitor::Impl {
    std::map<std::string, double> customMetrics;
    std::map<std::string, std::vector<std::pair<double, double>>> timeSeries;
    std::map<std::string, std::pair<double, double>> currentProfiles;
    size_t totalSteps = 0;  // Add this member
    std::mutex mutex;
};

RealTimeMonitor::RealTimeMonitor() : pImpl(std::make_unique<Impl>()) {}

RealTimeMonitor::~RealTimeMonitor() = default;

void RealTimeMonitor::startProgressIndicator(const std::string& label, size_t totalSteps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalSteps = totalSteps;
    std::cout << "Progress: " << label << " (0/" << totalSteps << ")" << std::endl;
}

void RealTimeMonitor::updateProgress(size_t currentStep, const std::string& details) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::cout << "\rProgress: " << currentStep << "/" << totalSteps_ 
              << " - " << details << std::flush;
}

void RealTimeMonitor::completeProgress(const std::string& finalMessage) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::cout << "\nProgress: " << finalMessage << std::endl;
}

void RealTimeMonitor::startProfile(const std::string& operation, const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->currentProfiles[operation] = {std::chrono::high_resolution_clock::now().time_since_epoch().count(), 0.0};
}

void RealTimeMonitor::endProfile(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->currentProfiles.count(operation)) {
        auto& profile = pImpl->currentProfiles[operation];
        double duration = (std::chrono::high_resolution_clock::now().time_since_epoch().count() - profile.first) / 1e9;
        profile.second = duration;
        
        std::cout << "Profile: " << operation << " - Duration: " 
                  << std::fixed << std::setprecision(3) << duration << "s" << std::endl;
    }
}

std::map<std::string, double> RealTimeMonitor::getProfileSummary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<std::string, double> summary;
    
    for (const auto& pair : pImpl->currentProfiles) {
        summary[pair.first] = pair.second;
    }
    
    return summary;
}

void RealTimeMonitor::addCustomMetric(const std::string& name, double value, const std::string& unit) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->customMetrics[name] = value;
    
    std::cout << "Metric: " << name << " = " << std::fixed << std::setprecision(3) << value << " " << unit << std::endl;
}

double RealTimeMonitor::getCustomMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->customMetrics.count(name)) {
        return pImpl->customMetrics.at(name);
    }
    return 0.0;
}

double RealTimeMonitor::calculateCorrelation(const std::string& metric1, const std::string& metric2) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->timeSeries.count(metric1) || !pImpl->timeSeries.count(metric2)) {
        return 0.0;
    }
    
    const auto& series1 = pImpl->timeSeries.at(metric1);
    const auto& series2 = pImpl->timeSeries.at(metric2);
    
    if (series1.size() != series2.size() || series1.empty()) {
        return 0.0;
    }
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
    
    for (size_t i = 0; i < series1.size(); ++i) {
        double x = series1[i].second;
        double y = series2[i].second;
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
        sumY2 += y * y;
    }
    
    double n = series1.size();
    double denominator = std::sqrt((sumX2 - sumX * sumX / n) * (sumY2 - sumY * sumY / n));
    
    if (denominator == 0.0) {
        return 0.0;
    }
    
    return (sumXY - sumX * sumY / n) / denominator;
}

double RealTimeMonitor::calculateRegression(const std::string& dependent, const std::string& independent) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->timeSeries.count(dependent) || !pImpl->timeSeries.count(independent)) {
        return 0.0;
    }
    
    const auto& ySeries = pImpl->timeSeries.at(dependent);
    const auto& xSeries = pImpl->timeSeries.at(independent);
    
    if (ySeries.size() != xSeries.size() || ySeries.empty()) {
        return 0.0;
    }
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    
    for (size_t i = 0; i < ySeries.size(); ++i) {
        double x = xSeries[i].second;
        double y = ySeries[i].second;
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double n = ySeries.size();
    double denominator = (n * sumX2 - sumX * sumX);
    
    if (denominator == 0.0) {
        return 0.0;
    }
    
    return (n * sumXY - sumX * sumY) / denominator;
}

std::string RealTimeMonitor::analyzeTrends(const std::vector<std::string>& metrics) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "=== Trend Analysis ===" << std::endl;
    
    for (const auto& metric : metrics) {
        if (pImpl->timeSeries.count(metric)) {
            const auto& series = pImpl->timeSeries.at(metric);
            if (series.size() >= 2) {
                double first = series.front().second;
                double last = series.back().second;
                double change = last - first;
                double percentChange = (first != 0.0) ? (change / first) * 100.0 : 0.0;
                
                oss << metric << ": " << std::fixed << std::setprecision(2) 
                    << change << " (" << std::setprecision(1) << percentChange << "%)" << std::endl;
            }
        }
    }
    
    return oss.str();
}

std::string RealTimeMonitor::extractTimeSeriesData(const std::string& metric, size_t points) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    
    if (pImpl->timeSeries.count(metric)) {
        const auto& series = pImpl->timeSeries.at(metric);
        size_t start = std::max(0, static_cast<ptrdiff_t>(series.size() - points));
        
        oss << metric << ":\n";
        for (size_t i = start; i < series.size(); ++i) {
            oss << series[i].first << ": " << series[i].second << std::endl;
        }
    }
    
    return oss.str();
}

std::string RealTimeMonitor::extractStatisticalData(const std::string& metric) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    
    if (pImpl->timeSeries.count(metric)) {
        const auto& series = pImpl->timeSeries.at(metric);
        if (!series.empty()) {
            double sum = 0.0, min = series.front().second, max = series.front().second;
            
            for (const auto& point : series) {
                sum += point.second;
                if (point.second < min) min = point.second;
                if (point.second > max) max = point.second;
            }
            
            double mean = sum / series.size();
            double variance = 0.0;
            
            for (const auto& point : series) {
                variance += (point.second - mean) * (point.second - mean);
            }
            variance /= series.size();
            
            oss << "Mean: " << std::fixed << std::setprecision(3) << mean << std::endl;
            oss << "Min: " << std::fixed << std::setprecision(3) << min << std::endl;
            oss << "Max: " << std::fixed << std::setprecision(3) << max << std::endl;
            oss << "Std Dev: " << std::fixed << std::setprecision(3) << std::sqrt(variance) << std::endl;
        }
    }
    
    return oss.str();
}

// LogAnalyzer implementation
struct LogAnalyzer::Impl {
    std::vector<LogEntry> logEntries;
    std::vector<PerformanceInfo> performanceEntries;
    std::vector<NeuralStats> neuralStatsEntries;
    std::mutex mutex;
};

LogAnalyzer::LogAnalyzer() : pImpl(std::make_unique<Impl>()) {}

LogAnalyzer::~LogAnalyzer() = default;

bool LogAnalyzer::parseLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple log parsing - would be more sophisticated in practice
        if (line.find("[PERF]") != std::string::npos) {
            // Performance log entry
            PerformanceInfo perf;
            // Extract operation name, duration, category from line
            perf.operation = "unknown";
            perf.duration = 0.0;
            perf.category = "unknown";
            pImpl->performanceEntries.push_back(perf);
        } else if (line.find("[NEURAL]") != std::string::npos) {
            // Neural stats log entry
            NeuralStats stats;
            // Extract neural stats from line
            stats.totalSpikes = 0;
            stats.activeNeurons = 0;
            pImpl->neuralStatsEntries.push_back(stats);
        } else if (line.find("[") != std::string::npos && line.find("]") != std::string::npos) {
            // Log entry
            LogEntry entry;
            // Extract level, timestamp, message from line
            entry.level = LogLevel::Info;
            entry.message = line;
            pImpl->logEntries.push_back(entry);
        }
    }
    
    file.close();
    return true;
}

std::vector<LogEntry> LogAnalyzer::getLogEntries() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->logEntries;
}

std::vector<PerformanceInfo> LogAnalyzer::getPerformanceLogs() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->performanceEntries;
}

std::vector<NeuralStats> LogAnalyzer::getNeuralStatsLogs() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->neuralStatsEntries;
}

double LogAnalyzer::analyzeMetricFrequency(const std::string& pattern) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t count = 0;
    for (const auto& entry : pImpl->logEntries) {
        if (entry.message.find(pattern) != std::string::npos) {
            count++;
        }
    }
    
    return static_cast<double>(count);
}

std::map<std::string, double> LogAnalyzer::analyzeLogLevelDistribution() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<std::string, double> distribution;
    
    size_t total = pImpl->logEntries.size();
    if (total == 0) {
        return distribution;
    }
    
    std::map<LogLevel, size_t> levelCounts;
    for (const auto& entry : pImpl->logEntries) {
        levelCounts[entry.level]++;
    }
    
    for (const auto& pair : levelCounts) {
        distribution[ConsoleLogger::levelToString(pair.first)] = 
            static_cast<double>(pair.second) / total * 100.0;
    }
    
    return distribution;
}

std::vector<std::pair<std::string, double>> LogAnalyzer::analyzeEventFrequency(const std::string& eventType) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::pair<std::string, double>> results;
    
    size_t count = 0;
    for (const auto& entry : pImpl->logEntries) {
        if (entry.message.find(eventType) != std::string::npos) {
            count++;
        }
    }
    
    results.push_back({eventType, static_cast<double>(count)});
    return results;
}

std::vector<LogEntry> LogAnalyzer::searchLogs(const std::string& query, LogLevel minLevel) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> results;
    
    for (const auto& entry : pImpl->logEntries) {
        if (entry.level >= minLevel && entry.message.find(query) != std::string::npos) {
            results.push_back(entry);
        }
    }
    
    return results;
}

std::vector<LogEntry> LogAnalyzer::filterLogs(const std::string& filterExpression, const std::map<std::string, std::string>& context) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> results;
    
    for (const auto& entry : pImpl->logEntries) {
        if (entry.message.find(filterExpression) != std::string::npos) {
            bool contextMatch = true;
            for (const auto& pair : context) {
                if (entry.message.find(pair.first) == std::string::npos) {
                    contextMatch = false;
                    break;
                }
            }
            if (contextMatch) {
                results.push_back(entry);
            }
        }
    }
    
    return results;
}

std::string LogAnalyzer::extractTimeSeriesData(const std::string& metric, const std::string& startTime, const std::string& endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "Time series for " << metric << ":\n";
    
    for (const auto& entry : pImpl->logEntries) {
        // Simple time extraction (would need proper timestamp parsing)
        oss << entry.message << std::endl;
    }
    
    return oss.str();
}

std::string LogAnalyzer::extractStatisticalSummary(const std::string& metric, const std::string& startTime, const std::string& endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "Statistical summary for " << metric << ":\n";
    oss << "Total entries: " << pImpl->logEntries.size() << std::endl;
    return oss.str();
}

} // namespace nlm

// LogAnalyzer implementation
struct LogAnalyzer::Impl {
    std::vector<LogEntry> logEntries;
    std::vector<PerformanceInfo> performanceEntries;
    std::vector<NeuralStats> neuralStatsEntries;
    std::mutex mutex;
};

LogAnalyzer::LogAnalyzer() : pImpl(std::make_unique<Impl>()) {}

LogAnalyzer::~LogAnalyzer() = default;

bool LogAnalyzer::parseLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple log parsing - would be more sophisticated in practice
        if (line.find("[PERF]") != std::string::npos) {
            // Performance log entry
            PerformanceInfo perf;
            // Extract operation name, duration, category from line
            perf.operation = "unknown";
            perf.duration = 0.0;
            perf.category = "unknown";
            pImpl->performanceEntries.push_back(perf);
        } else if (line.find("[NEURAL]") != std::string::npos) {
            // Neural stats log entry
            NeuralStats stats;
            // Extract neural stats from line
            stats.totalSpikes = 0;
            stats.activeNeurons = 0;
            pImpl->neuralStatsEntries.push_back(stats);
        } else if (line.find("[") != std::string::npos && line.find("]") != std::string::npos) {
            // Log entry
            LogEntry entry;
            // Extract level, timestamp, message from line
            entry.level = LogLevel::Info;
            entry.message = line;
            pImpl->logEntries.push_back(entry);
        }
    }
    
    file.close();
    return true;
}

std::vector<LogEntry> LogAnalyzer::getLogEntries() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->logEntries;
}

std::vector<PerformanceInfo> LogAnalyzer::getPerformanceLogs() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->performanceEntries;
}

std::vector<NeuralStats> LogAnalyzer::getNeuralStatsLogs() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->neuralStatsEntries;
}

double LogAnalyzer::analyzeMetricFrequency(const std::string& pattern) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t count = 0;
    for (const auto& entry : pImpl->logEntries) {
        if (entry.message.find(pattern) != std::string::npos) {
            count++;
        }
    }
    
    return static_cast<double>(count);
}

std::map<std::string, double> LogAnalyzer::analyzeLogLevelDistribution() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<std::string, double> distribution;
    
    size_t total = pImpl->logEntries.size();
    if (total == 0) {
        return distribution;
    }
    
    std::map<LogLevel, size_t> levelCounts;
    for (const auto& entry : pImpl->logEntries) {
        levelCounts[entry.level]++;
    }
    
    for (const auto& pair : levelCounts) {
        distribution[levelToString(pair.first)] = 
            static_cast<double>(pair.second) / total * 100.0;
    }
    
    return distribution;
}

std::vector<std::pair<std::string, double>> LogAnalyzer::analyzeEventFrequency(const std::string& eventType) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::pair<std::string, double>> results;
    
    size_t count = 0;
    for (const auto& entry : pImpl->logEntries) {
        if (entry.message.find(eventType) != std::string::npos) {
            count++;
        }
    }
    
    results.push_back({eventType, static_cast<double>(count)});
    return results;
}

std::vector<LogEntry> LogAnalyzer::searchLogs(const std::string& query, LogLevel minLevel) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> results;
    
    for (const auto& entry : pImpl->logEntries) {
        if (entry.level >= minLevel && entry.message.find(query) != std::string::npos) {
            results.push_back(entry);
        }
    }
    
    return results;
}

std::vector<LogEntry> LogAnalyzer::filterLogs(const std::string& filterExpression, const std::map<std::string, std::string>& context) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> results;
    
    for (const auto& entry : pImpl->logEntries) {
        if (entry.message.find(filterExpression) != std::string::npos) {
            bool contextMatch = true;
            for (const auto& pair : context) {
                if (entry.message.find(pair.first) == std::string::npos) {
                    contextMatch = false;
                    break;
                }
            }
            if (contextMatch) {
                results.push_back(entry);
            }
        }
    }
    
    return results;
}

std::string LogAnalyzer::extractTimeSeriesData(const std::string& metric, const std::string& startTime, const std::string& endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "Time series for " << metric << ":\n";
    
    for (const auto& entry : pImpl->logEntries) {
        // Simple time extraction (would need proper timestamp parsing)
        oss << entry.message << std::endl;
    }
    
    return oss.str();
}

std::string LogAnalyzer::extractStatisticalSummary(const std::string& metric, const std::string& startTime, const std::string& endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "Statistical summary for " << metric << ":\n";
    oss << "Total entries: " << pImpl->logEntries.size() << std::endl;
    return oss.str();
}

} // namespace nlm

// PerformanceTimer implementation
PerformanceTimer::PerformanceTimer(const std::string& operation, const std::string& category) 
    : operation_(operation), category_(category) {
    startTime_ = std::chrono::high_resolution_clock::now();
    memoryUsage_ = 0; // In a real implementation, this would capture actual memory usage
}

PerformanceTimer::~PerformanceTimer() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_).count() / 1e9;
    
    std::cout << "Performance Timer: " << operation_ << " - Duration: " 
              << std::fixed << std::setprecision(6) << duration << "s" 
              << " - Category: " << category_ << std::endl;
}

double PerformanceTimer::getDuration() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_).count() / 1e9;
    return duration;
}

} // namespace nlm

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    std::shared_ptr<StatisticsCollector> statisticsCollector;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor;
    std::shared_ptr<LogAnalyzer> logAnalyzer;
    
    std::map<std::string, double> systemMetrics;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::addAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger) {
    addLogger(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::setStatisticsCollector(std::shared_ptr<StatisticsCollector> collector) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->statisticsCollector = collector;
}

void Logger::setRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->realTimeMonitor = monitor;
}

void Logger::setLogAnalyzer(std::shared_ptr<LogAnalyzer> analyzer) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logAnalyzer = analyzer;
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line,
                const std::string& function, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.context = context;
    
    // Generate unique IDs
    static std::atomic<uint64_t> operationId{0};
    entry.operationId = ++operationId;
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    // Record in statistics collector if available
    if (pImpl->statisticsCollector) {
        if (level == LogLevel::Error || level == LogLevel::Critical) {
            pImpl->statisticsCollector->recordOperation("log_error", 0.0, 0);
        }
    }
    
    // Record in real time monitor if available
    if (pImpl->realTimeMonitor) {
        pImpl->realTimeMonitor->addCustomMetric("log_messages", 1.0, "count");
    }
    
    // Log to all loggers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Record system metrics
    pImpl->systemMetrics["log_messages"] = pImpl->systemMetrics["log_messages"] + 1.0;
}

std::chrono::system_clock::time_point Logger::startTiming(const std::string& operation, const std::string& category) {
    if (pImpl->realTimeMonitor) {
        pImpl->realTimeMonitor->startProfile(operation, category);
    }
    return std::chrono::system_clock::now();
}

void Logger::endTiming(const std::chrono::system_clock::time_point& startTime, const std::string& operation, uint64_t memoryUsage) {
    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() / 1e9;
    
    PerformanceInfo perf;
    perf.operation = operation;
    perf.duration = duration;
    perf.memoryUsage = memoryUsage;
    perf.startTime = startTime;
    perf.endTime = endTime;
    
    // Record in statistics collector if available
    if (pImpl->statisticsCollector) {
        pImpl->statisticsCollector->recordOperation(operation, duration, memoryUsage);
    }
    
    // Record in real time monitor if available
    if (pImpl->realTimeMonitor) {
        pImpl->realTimeMonitor->endProfile(operation);
    }
    
    // Log performance information
    logEvent("performance", operation + " - Duration: " + std::to_string(duration) + "s");
}

void Logger::logNeuralStats(const NeuralStats& stats) {
    // Record in statistics collector if available
    if (pImpl->statisticsCollector) {
        // Convert NeuralStats to statistics collector format
        pImpl->statisticsCollector->recordFiringRate(stats.averageFiringRate);
        pImpl->statisticsCollector->recordExcitationInhibitionRatio(stats.excitationInhibitionRatio);
        for (const auto& pair : stats.firingRates) {
            pImpl->statisticsCollector->updateNeuralPopulationStats(pair.first, pair.second);
        }
    }
    
    // Log to all advanced loggers
    for (auto& logger : pImpl->loggers) {
        if (auto advancedLogger = std::dynamic_pointer_cast<IAdvancedLogger>(logger)) {
            advancedLogger->logNeuralStats(stats);
        }
    }
}

void Logger::logEvent(const std::string& eventType, const std::string& data) {
    // Record in statistics collector if available
    if (pImpl->statisticsCollector) {
        pImpl->statisticsCollector->recordOperation("event_" + eventType, 0.0, 0);
    }
    
    // Log to all advanced loggers
    for (auto& logger : pImpl->loggers) {
        if (auto advancedLogger = std::dynamic_pointer_cast<IAdvancedLogger>(logger)) {
            advancedLogger->logEvent(eventType, data);
        }
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Debug, message, file, line, function, context);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Info, message, file, line, function, context);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Warning, message, file, line, function, context);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Error, message, file, line, function, context);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Critical, message, file, line, function, context);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

NeuralStats Logger::getNeuralStats() const {
    if (pImpl->statisticsCollector) {
        return pImpl->statisticsCollector->getNeuralStats();
    }
    
    NeuralStats stats;
    stats.totalSpikes = 0;
    stats.activeNeurons = 0;
    stats.firingNeurons = 0;
    stats.averageFiringRate = 0.0f;
    stats.excitationInhibitionRatio = 0.0f;
    return stats;
}

std::map<std::string, double> Logger::getSystemStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<std::string, double> stats = pImpl->systemMetrics;
    
    // Add real time monitor metrics if available
    if (pImpl->realTimeMonitor) {
        auto profileSummary = pImpl->realTimeMonitor->getProfileSummary();
        stats.insert(profileSummary.begin(), profileSummary.end());
    }
    
    return stats;
}

bool Logger::exportStatistics(const std::string& filepath) const {
    if (pImpl->statisticsCollector) {
        return pImpl->statisticsCollector->exportToFile(filepath);
    }
    return false;
}

bool Logger::importStatistics(const std::string& filepath) {
    if (pImpl->statisticsCollector) {
        return pImpl->statisticsCollector->importFromFile(filepath);
    }
    return false;
}

} // namespace nlm
