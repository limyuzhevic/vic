// PerformanceReport implementation
// Performance reporting system implementation

#include "PerformanceReport.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

PerformanceReport::PerformanceReport(const std::string& title) {
    reportTitle = title;
    generationTime = std::chrono::high_resolution_clock::now();
}

void PerformanceReport::addPerformanceMetrics(const PerformanceMetrics& metrics) {
    performanceData.push_back(metrics);
}

void PerformanceReport::addBenchmarkData(const std::string& benchmarkName, const std::vector<double>& data) {
    benchmarkData[benchmarkName] = data;
}

void PerformanceReport::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

void PerformanceReport::exportToJSON(const std::string& filename) const {
    // Implementation for exporting to JSON format
    // This would create a JSON file with the performance report data
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "{\n";
        file << "  \"title\": \"" << reportTitle << "\",\n";
        file << "  \"generationTime\": \"" << getCurrentTimestamp() << "\",\n";
        file << "  \"metrics\": [\n";
        for (size_t i = 0; i < performanceData.size(); ++i) {
            if (i > 0) file << ",\n";
            file << formatMetricsAsJSON(performanceData[i]);
        }
        file << "\n],\n";
        file << "  \"benchmarks\": {\n";
        size_t benchIndex = 0;
        for (const auto& pair : benchmarkData) {
            if (benchIndex > 0) file << ",\n";
            file << "    \"" << pair.first << "\": [\n";
            for (size_t j = 0; j < pair.second.size(); ++j) {
                if (j > 0) file << ",\n";
                file << "      " << pair.second[j];
            }
            file << "\n    ]";
            benchIndex++;
        }
        file << "\n  }\n";
        file << "}\n";
        file.close();
    }
}

void PerformanceReport::exportToCSV(const std::string& filename) const {
    // Implementation for exporting to CSV format
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "Timestamp,CPU_Time,Wall_Clock_Time,Memory_Usage,Active_Threads,Steps_Per_Second\n";
        for (const auto& metrics : performanceData) {
            file << getTimestampString(metrics.timestamp) << ",";
            file << std::fixed << std::setprecision(4) << metrics.cpuTime << ",";
            file << std::fixed << std::setprecision(4) << metrics.wallClockTime << ",";
            file << metrics.memoryUsage << ",";
            file << metrics.activeThreads << ",";
            file << std::fixed << std::setprecision(4) << metrics.stepsPerSecond << "\n";
        }
        file.close();
    }
}

void PerformanceReport::exportToHTML(const std::string& filename) const {
    // Implementation for exporting to HTML format
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "<!DOCTYPE html>\n";
        file << "<html><head><title>" << reportTitle << "</title></head>\n";
        file << "<body><h1>" << reportTitle << "</h1>\n";
        file << "<h2>Performance Metrics</h2>\n";
        file << "<table border=\"1\">\n";
        file << "<tr><th>Timestamp</th><th>CPU Time</th><th>Wall Clock</th><th>Memory</th><th>Active Threads</th><th>Steps/sec</th></tr>\n";
        for (const auto& metrics : performanceData) {
            file << "<tr><td>" << getTimestampString(metrics.timestamp) << "</td><td>";
            file << std::fixed << std::setprecision(4) << metrics.cpuTime << "</td><td>";
            file << std::fixed << std::setprecision(4) << metrics.wallClockTime << "</td><td>";
            file << metrics.memoryUsage << "</td><td>" << metrics.activeThreads << "</td><td>";
            file << std::fixed << std::setprecision(4) << metrics.stepsPerSecond << "</td></tr>\n";
        }
        file << "</table></body></html>\n";
        file.close();
    }
}

void PerformanceReport::exportToText(const std::string& filename) const {
    // Implementation for exporting to text format
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "Performance Report: " << reportTitle << "\n";
        file << "Generated: " << getTimestampString(generationTime) << "\n\n";
        file << "Performance Metrics:\n";
        for (const auto& metrics : performanceData) {
            file << formatMetricsAsText(metrics) << "\n";
        }
        file.close();
    }
}

void PerformanceReport::exportToXML(const std::string& filename) const {
    // Implementation for exporting to XML format
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << "<performanceReport>\n";
        file << "  <title>" << reportTitle << "</title>\n";
        file << "  <generationTime>" << getTimestampString(generationTime) << "</generationTime>\n";
        file << "  <metrics>\n";
        for (const auto& metrics : performanceData) {
            file << "    <metric>";
            file << "      <cpuTime>" << metrics.cpuTime << "</cpuTime>";
            file << "      <wallClockTime>" << metrics.wallClockTime << "</wallClockTime>";
            file << "      <memoryUsage>" << metrics.memoryUsage << "</memoryUsage>";
            file << "      <activeThreads>" << metrics.activeThreads << "</activeThreads>";
            file << "      <stepsPerSecond>" << metrics.stepsPerSecond << "</stepsPerSecond>";
            file << "    </metric>\n";
        }
        file << "  </metrics>\n";
        file << "</performanceReport>\n";
        file.close();
    }
}

double PerformanceReport::calculateAverageCpuUsage() const {
    if (performanceData.empty()) return 0.0;
    double total = 0.0;
    for (const auto& metrics : performanceData) {
        total += metrics.cpuTime;
    }
    return total / performanceData.size();
}

double PerformanceReport::calculatePeakMemoryUsage() const {
    if (performanceData.empty()) return 0.0;
    double peak = 0.0;
    for (const auto& metrics : performanceData) {
        peak = std::max(peak, static_cast<double>(metrics.memoryUsage));
    }
    return peak;
}

double PerformanceReport::calculateAverageStepTime() const {
    if (performanceData.empty()) return 0.0;
    double total = 0.0;
    for (const auto& metrics : performanceData) {
        total += metrics.wallClockTime;
    }
    return total / performanceData.size();
}

double PerformanceReport::calculatePerformanceTrend() const {
    if (performanceData.size() < 2) return 0.0;
    
    std::vector<double> cpuTimes;
    for (const auto& metrics : performanceData) {
        cpuTimes.push_back(metrics.cpuTime);
    }
    
    double sum = std::accumulate(cpuTimes.begin(), cpuTimes.end(), 0.0);
    double mean = sum / cpuTimes.size();
    
    double sumSq = 0.0;
    for (const auto& value : cpuTimes) {
        sumSq += (value - mean) * (value - mean);
    }
    
    double stddev = std::sqrt(sumSq / cpuTimes.size());
    
    return stddev / mean; // Coefficient of variation
}

bool PerformanceReport::detectPerformanceRegression(double threshold) const {
    if (performanceData.size() < 2) return false;
    
    double avgFirstHalf = 0.0;
    double avgSecondHalf = 0.0;
    
    size_t firstHalfSize = performanceData.size() / 2;
    for (size_t i = 0; i < firstHalfSize; ++i) {
        avgFirstHalf += performanceData[i].cpuTime;
    }
    avgFirstHalf /= firstHalfSize;
    
    for (size_t i = firstHalfSize; i < performanceData.size(); ++i) {
        avgSecondHalf += performanceData[i].cpuTime;
    }
    avgSecondHalf /= (performanceData.size() - firstHalfSize);
    
    double improvement = (avgFirstHalf - avgSecondHalf) / avgFirstHalf;
    
    return improvement > threshold;
}

std::string PerformanceReport::formatMetricsAsText(const PerformanceMetrics& metrics) const {
    std::stringstream ss;
    ss << "Metrics at " << getTimestampString(metrics.timestamp) << ":\n";
    ss << "  CPU Time: " << std::fixed << std::setprecision(4) << metrics.cpuTime << "s\n";
    ss << "  Wall Clock Time: " << std::fixed << std::setprecision(4) << metrics.wallClockTime << "s\n";
    ss << "  Memory Usage: " << metrics.memoryUsage << " bytes\n";
    ss << "  Active Threads: " << metrics.activeThreads << "\n";
    ss << "  Steps per Second: " << std::fixed << std::setprecision(4) << metrics.stepsPerSecond << "\n";
    ss << "  Thread Utilization: " << std::fixed << std::setprecision(4) << metrics.threadUtilization << "\n";
    ss << "  I/O Usage: " << metrics.readOperations << " reads, " << metrics.writeOperations << " writes\n";
    return ss.str();
}

std::string PerformanceReport::formatBenchmarkResults(const std::string& benchmarkName) const {
    if (benchmarkData.find(benchmarkName) == benchmarkData.end()) {
        return "Benchmark not found: " + benchmarkName;
    }
    
    std::stringstream ss;
    ss << "Benchmark Results: " << benchmarkName << "\n";
    const auto& data = benchmarkData.at(benchmarkName);
    
    ss << "  Data points: " << data.size() << "\n";
    ss << "  Mean: " << calculateMean(data) << "\n";
    ss << "  Median: " << calculateMedian(data) << "\n";
    ss << "  Standard Deviation: " << calculateStandardDeviation(data) << "\n";
    ss << "  Min: " << *std::min_element(data.begin(), data.end()) << "\n";
    ss << "  Max: " << *std::max_element(data.begin(), data.end()) << "\n";
    
    return ss.str();
}

std::string PerformanceReport::generateSummary() const {
    std::stringstream ss;
    ss << "Performance Report Summary\n";
    ss << "=========================\n\n";
    ss << "Title: " << reportTitle << "\n";
    ss << "Generated: " << getTimestampString(generationTime) << "\n";
    ss << "Total Metrics Collected: " << performanceData.size() << "\n";
    ss << "\nPerformance Statistics:\n";
    ss << "  Average CPU Usage: " << calculateAverageCpuUsage() << "\n";
    ss << "  Peak Memory Usage: " << calculatePeakMemoryUsage() << " bytes\n";
    ss << "  Average Step Time: " << calculateAverageStepTime() << "s\n";
    ss << "  Performance Trend: " << calculatePerformanceTrend() << "\n";
    ss << "  Regression Detected: " << (detectPerformanceRegression(0.05) ? "Yes" : "No") << "\n";
    
    return ss.str();
}

void PerformanceReport::clear() {
    performanceData.clear();
    benchmarkData.clear();
    metadata.clear();
}

bool PerformanceReport::isEmpty() const {
    return performanceData.empty() && benchmarkData.empty();
}

 size_t PerformanceReport::getDataCount() const {
    return performanceData.size();
}

std::chrono::high_resolution_clock::time_point PerformanceReport::getGenerationTime() const {
    return generationTime;
}

std::string PerformanceReport::getReportTitle() const {
    return reportTitle;
}

double PerformanceReport::calculateStandardDeviation(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    double mean = calculateMean(data);
    double sumSq = 0.0;
    for (const auto& value : data) {
        sumSq += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sumSq / data.size());
}

double PerformanceReport::calculateMean(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double PerformanceReport::calculateMedian(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    size_t size = sorted.size();
    if (size % 2 == 0) {
        return (sorted[size/2 - 1] + sorted[size/2]) / 2.0;
    } else {
        return sorted[size/2];
    }
}

double PerformanceReport::calculatePercentile(const std::vector<double>& data, double percentile) const {
    if (data.empty()) return 0.0;
    
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    size_t index = static_cast<size_t>((percentile / 100.0) * (sorted.size() - 1));
    return sorted[index];
}

void PerformanceReport::compareWithPreviousReport(const PerformanceReport& previousReport) const {
    // Implementation for comparing with previous report
}

void PerformanceReport::identifyPerformanceTrends() const {
    // Implementation for identifying performance trends
}

void PerformanceReport::generatePerformanceAlerts() const {
    // Implementation for generating performance alerts
}

void PerformanceReport::generateDashboardHTML(const std::string& filename) const {
    // Implementation for generating dashboard HTML
}

void PerformanceReport::generateMetricsDashboard(const std::string& filename) const {
    // Implementation for generating metrics dashboard
}

std::string PerformanceReport::escapeXML(const std::string& input) const {
    // Implementation for escaping XML
    return input;
}

std::string PerformanceReport::escapeJSON(const std::string& input) const {
    // Implementation for escaping JSON
    return input;
}

std::string PerformanceReport::formatDuration(double seconds) const {
    std::stringstream ss;
    if (seconds >= 3600) {
        ss << static_cast<int>(seconds / 3600) << "h ";
        seconds = fmod(seconds, 3600);
    }
    if (seconds >= 60) {
        ss << static_cast<int>(seconds / 60) << "m ";
        seconds = fmod(seconds, 60);
    }
    ss << std::fixed << std::setprecision(2) << seconds << "s";
    return ss.str();
}

std::string PerformanceReport::formatBytes(size_t bytes) const {
    std::stringstream ss;
    double value = static_cast<double>(bytes);
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    while (value >= 1024.0 && unit < 4) {
        value /= 1024.0;
        unit++;
    }
    ss << std::fixed << std::setprecision(2) << value << " " << units[unit];
    return ss.str();
}

std::string PerformanceReport::formatPercentage(double value) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << value * 100.0 << "%";
    return ss.str();
}

std::string PerformanceReport::getCurrentTimestamp() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string PerformanceReport::getTimestampString(const std::chrono::high_resolution_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(timestamp));
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string PerformanceReport::formatMetricsAsJSON(const PerformanceMetrics& metrics) const {
    std::stringstream ss;
    ss << "{\n";
    ss << "      \"timestamp\": \"" << getTimestampString(metrics.timestamp) << "\",\n";
    ss << "      \"cpuTime\": " << std::fixed << std::setprecision(4) << metrics.cpuTime << ",\n";
    ss << "      \"wallClockTime\": " << std::fixed << std::setprecision(4) << metrics.wallClockTime << ",\n";
    ss << "      \"memoryUsage\": " << metrics.memoryUsage << ",\n";
    ss << "      \"activeThreads\": " << metrics.activeThreads << ",\n";
    ss << "      \"stepsPerSecond\": " << std::fixed << std::setprecision(4) << metrics.stepsPerSecond << "\n";
    ss << "    }";
    return ss.str();
}

} // namespace nlm