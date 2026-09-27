// Implementation of PerformanceReporter

#include "nlm/performance/PerformanceReporter.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <cmath>

namespace nlm {

PerformanceReporter::PerformanceReporter(
    const std::string& outputDirectory,
    bool enableRealTimeUpdates,
    std::chrono::milliseconds updateInterval)
    : outputDirectory_(outputDirectory)
    , enableRealTimeUpdates_(enableRealTimeUpdates)
    , updateInterval_(updateInterval) {
    ensureOutputDirectoryExists();
}

void PerformanceReporter::start() {
    if (running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    running_.store(true, std::memory_order_relaxed);
    reportingThread_ = std::thread(&PerformanceReporter::reportingThreadFunc, this);
}

void PerformanceReporter::stop() {
    if (!running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    {
        std::unique_lock<std::mutex> lock(stopMutex_);
        running_.store(false, std::memory_order_relaxed);
        stopCondition_.notify_one();
    }
    
    if (reportingThread_.joinable()) {
        reportingThread_.join();
    }
}

void PerformanceReporter::reportingThreadFunc() {
    while (running_.load(std::memory_order_relaxed)) {
        std::unique_lock<std::mutex> lock(stopMutex_);
        if (stopCondition_.wait_for(lock, updateInterval_, 
                                   [this] { return !running_.load(std::memory_order_relaxed); })) {
            break;
        }
        lock.unlock();
        
        if (enableRealTimeUpdates_) {
            generateScheduledReports();
        }
    }
}

std::string PerformanceReporter::generateDashboard(
    const std::vector<DashboardWidget>& dashboardWidgets) {
    std::string dashboardPath = outputDirectory_ + "/dashboard_" + 
                                timestampToString(std::chrono::high_resolution_clock::now()) + ".html";
    
    std::ofstream file(dashboardPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open dashboard file: " + dashboardPath);
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "  <title>NLM Performance Dashboard</title>" << std::endl;
    file << "  <style>" << std::endl;
    file << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    file << "    .widget { border: 1px solid #ccc; padding: 15px; margin: 10px; }" << std::endl;
    file << "    .gauge { width: 200px; height: 200px; }" << std::endl;
    file << "    .chart { width: 100%; height: 400px; }" << std::endl;
    file << "  </style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "  <h1>NLM Performance Dashboard</h1>" << std::endl;
    file << "  <p>Generated: " << timestampToString(std::chrono::high_resolution_clock::now()) << "</p>" << std::endl;
    file << std::endl;
    
    for (const auto& widget : dashboardWidgets) {
        file << "  <div class=\"widget\">" << std::endl;
        file << "    <h3>" << widget.title << "</h3>" << std::endl;
        file << "    <div class=\"" << widget.widgetType << "\">" << std::endl;
        file << "      <!-- Widget content would be generated here -->" << std::endl;
        file << "    </div>" << std::endl;
        file << "  </div>" << std::endl;
        file << std::endl;
    }
    
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    
    // Notify callbacks
    VisualizationData visualization;
    visualization.visualizationId = "dashboard_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    visualization.visualizationType = "dashboard";
    visualization.dataFormat = "html";
    visualization.filePath = dashboardPath;
    visualization.lastUpdated = std::chrono::high_resolution_clock::now();
    
    for (const auto& callback : visualizationCallbacks_) {
        callback(visualization);
    }
    
    return dashboardPath;
}

std::string PerformanceReporter::generateChart(
    const ChartConfig& config,
    const std::vector<PerformanceDataPoint>& dataPoints) {
    std::string chartPath = outputDirectory_ + "/chart_" + 
                           std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".html";
    
    std::ofstream file(chartPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open chart file: " + chartPath);
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "  <title>" << config.title << "</title>" << std::endl;
    file << "  <style>" << std::endl;
    file << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    file << "    .chart-container { width: " << config.width << "px; height: " << config.height << "px; }" << std::endl;
    file << "    .axis { stroke: #ccc; stroke-width: 1; }" << std::endl;
    file << "    .line { fill: none; stroke: steelblue; stroke-width: 2; }" << std::endl;
    file << "  </style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "  <h1>" << config.title << "</h1>" << std::endl;
    file << "  <div class=\"chart-container\">" << std::endl;
    file << "    <svg width=\"100%\" height=\"100%\">" << std::endl;
    file << "      <!-- Chart would be rendered here using D3.js or similar -->" << std::endl;
    file << "      <rect width=\"100%\" height=\"100%\" fill=\"#f0f0f0\"/>" << std::endl;
    file << "      <text x=\"50%\" y=\"50%\" text-anchor=\"middle\" font-size=\"20px\" fill=\"#666\">" << std::endl;
    file << "        Chart generation requires JavaScript implementation" << std::endl;
    file << "      </text>" << std::endl;
    file << "    </svg>" << std::endl;
    file << "  </div>" << std::endl;
    file << "  <p>Chart type: " << config.chartType << ", Series: ";
    
    for (size_t i = 0; i < config.seriesIds.size(); ++i) {
        if (i > 0) file << ", ";
        file << config.seriesIds[i];
    }
    file << "</p>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    
    // Notify callbacks
    VisualizationData visualization;
    visualization.visualizationId = "chart_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    visualization.visualizationType = "chart";
    visualization.dataFormat = "html";
    visualization.filePath = chartPath;
    visualization.lastUpdated = std::chrono::high_resolution_clock::now();
    
    for (const auto& callback : visualizationCallbacks_) {
        callback(visualization);
    }
    
    return chartPath;
}

PerformanceReport PerformanceReporter::generateReport(
    const std::string& reportType,
    bool includeCharts,
    bool includeMetrics) {
    PerformanceReport report;
    report.generationTime = std::chrono::high_resolution_clock::now();
    report.reportId = "report_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    report.reportType = reportType;
    report.title = "NLM Performance Report";
    report.format = "html";
    
    // Generate report content
    std::stringstream content;
    content << "<!DOCTYPE html>" << std::endl;
    content << "<html>" << std::endl;
    content << "<head>" << std::endl;
    content << "  <title>" << report.title << "</title>" << std::endl;
    content << "  <style>" << std::endl;
    content << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    content << "    .section { margin: 20px 0; padding: 20px; border: 1px solid #ddd; }" << std::endl;
    content << "  </style>" << std::endl;
    content << "</head>" << std::endl;
    content << "<body>" << std::endl;
    content << "  <h1>" << report.title << "</h1>" << std::endl;
    content << "  <p>Generated: " << timestampToString(report.generationTime) << "</p>" << std::endl;
    content << "  <p>Report ID: " << report.reportId << "</p>" << std::endl;
    
    if (includeMetrics) {
        content << "  <div class=\"section\">" << std::endl;
        content << "    <h2>Performance Metrics</h2>" << std::endl;
        content << "    <!-- Metrics would be included here -->" << std::endl;
        content << "  </div>" << std::endl;
    }
    
    if (includeCharts) {
        content << "  <div class=\"section\">" << std::endl;
        content << "    <h2>Performance Charts</h2>" << std::endl;
        content << "    <!-- Charts would be included here -->" << std::endl;
        content << "  </div>" << std::endl;
    }
    
    content << "</body>" << std::endl;
    content << "</html>" << std::endl;
    
    report.content = content.str();
    
    // Save report
    report.filePath = saveReport(report);
    
    // Notify callbacks
    for (const auto& callback : reportCallbacks_) {
        callback(report);
    }
    
    return report;
}

std::string PerformanceReporter::saveReport(const PerformanceReport& report) {
    std::string reportPath = outputDirectory_ + "/report_" + report.reportId + "." + report.format;
    
    std::ofstream file(reportPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open report file: " + reportPath);
    }
    
    file << report.content;
    file.close();
    
    // Add to report history
    std::lock_guard<std::mutex> lock(reportsMutex_);
    reports_.push_back(report);
    
    // Maintain history size
    while (reports_.size() > 1000) {
        reports_.erase(reports_.begin());
    }
    
    // Update statistics
    stats_.totalReportsGenerated++;
    stats_.lastReportTime = report.generationTime;
    
    return reportPath;
}

PerformanceReport PerformanceReporter::loadReport(const std::string& filePath) {
    PerformanceReport report;
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open report file: " + filePath);
    }
    
    // Read report content
    std::stringstream buffer;
    buffer << file.rdbuf();
    report.content = buffer.str();
    
    file.close();
    
    return report;
}

std::string PerformanceReporter::exportPerformanceData(
    const std::string& format,
    const std::vector<std::string>& metrics) {
    std::string exportPath = outputDirectory_ + "/export_" + 
                            std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "." + format;
    
    std::ofstream file(exportPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open export file: " + exportPath);
    }
    
    if (format == "csv") {
        // CSV export format
        file << "timestamp";
        for (const auto& metric : metrics) {
            file << "," << metric;
        }
        file << std::endl;
        
        // Generate data (simplified)
        auto now = std::chrono::high_resolution_clock::now();
        double baseTime = std::chrono::duration<double>(now.time_since_epoch()).count();
        
        for (int i = 0; i < 100; ++i) {
            double time = baseTime + i * 60.0; // 60-second intervals
            file << time;
            for (const auto& metric : metrics) {
                // Generate random metric values
                double value = 50.0 + (std::rand() % 100); // 50-150 range
                file << "," << value;
            }
            file << std::endl;
        }
    }
    
    file.close();
    
    return exportPath;
}

std::vector<VisualizationData> PerformanceReporter::getVisualizations(const std::string& visualizationType) const {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    std::vector<VisualizationData> visualizations;
    
    if (visualizationType.empty()) {
        return visualizations_;
    }
    
    for (const auto& visualization : visualizations_) {
        if (visualization.visualizationType == visualizationType) {
            visualizations.push_back(visualization);
        }
    }
    
    return visualizations;
}

std::vector<PerformanceReport> PerformanceReporter::getRecentReports(size_t count) const {
    std::lock_guard<std::mutex> lock(reportsMutex_);
    
    std::vector<PerformanceReport> recentReports;
    size_t actualCount = std::min(count, reports_.size());
    
    if (actualCount == 0) {
        return recentReports;
    }
    
    size_t startIndex = reports_.size() - actualCount;
    recentReports.reserve(actualCount);
    
    for (size_t i = startIndex; i < reports_.size(); ++i) {
        recentReports.push_back(reports_[i]);
    }
    
    return recentReports;
}

void PerformanceReporter::addReportCallback(const ReportCallback& callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    reportCallbacks_.push_back(callback);
}

void PerformanceReporter::addVisualizationCallback(const VisualizationCallback& callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    visualizationCallbacks_.push_back(callback);
}

std::string PerformanceReporter::generatePerformanceSummary() const {
    std::stringstream summary;
    
    summary << "=== NLM Performance Summary ===" << std::endl;
    
    if (reports_.empty()) {
        summary << "No reports available." << std::endl;
        return summary.str();
    }
    
    // Calculate summary statistics
    size_t totalReports = reports_.size();
    double avgReportSize = 0.0;
    
    for (const auto& report : reports_) {
        avgReportSize += report.content.size();
    }
    
    if (totalReports > 0) {
        avgReportSize /= totalReports;
    }
    
    summary << "Total Reports Generated: " << totalReports << std::endl;
    summary << "Average Report Size: " << avgReportSize << " bytes" << std::endl;
    summary << "Output Directory: " << outputDirectory_ << std::endl;
    
    return summary.str();
}

PerformanceReport PerformanceReporter::generateComparisonReport(
    const std::vector<std::string>& comparisonData) const {
    PerformanceReport report;
    report.generationTime = std::chrono::high_resolution_clock::now();
    report.reportId = "comparison_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    report.reportType = "comparison";
    report.title = "NLM Performance Comparison Report";
    report.format = "html";
    
    // Generate comparison content
    std::stringstream content;
    content << "<!DOCTYPE html>" << std::endl;
    content << "<html>" << std::endl;
    content << "<head>" << std::endl;
    content << "  <title>" << report.title << "</title>" << std::endl;
    content << "</head>" << std::endl;
    content << "<body>" << std::endl;
    content << "  <h1>" << report.title << "</h1>" << std::endl;
    content << "  <p>Generated: " << timestampToString(report.generationTime) << "</p>" << std::endl;
    
    content << "  <h2>Comparison Data</h2>" << std::endl;
    content << "  <ul>" << std::endl;
    for (const auto& data : comparisonData) {
        content << "    <li>" << data << "</li>" << std::endl;
    }
    content << "  </ul>" << std::endl;
    
    content << "</body>" << std::endl;
    content << "</html>" << std::endl;
    
    report.content = content.str();
    
    // Save report
    report.filePath = saveReport(report);
    
    return report;
}

std::string PerformanceReporter::generateHistoricalAnalysis(
    std::chrono::high_resolution_clock::duration timeWindow) const {
    std::stringstream analysis;
    
    analysis << "=== NLM Historical Performance Analysis ===" << std::endl;
    analysis << "Time Window: " << timeWindow.count() << " seconds" << std::endl;
    analysis << std::endl;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto cutoffTime = now - timeWindow;
    
    // Analyze historical data
    size_t reportCount = 0;
    for (const auto& report : reports_) {
        if (report.generationTime >= cutoffTime) {
            reportCount++;
        }
    }
    
    analysis << "Reports Generated in Time Window: " << reportCount << std::endl;
    analysis << "Average Reports per Hour: " << (reportCount * 3600.0 / timeWindow.count()) << std::endl;
    
    return analysis.str();
}

PerformanceReporter::ReportStats PerformanceReporter::getReportStats() const {
    std::lock_guard<std::mutex> lock(reportsMutex_);
    return stats_;
}

void PerformanceReporter::generateScheduledReports() {
    // Generate scheduled reports (simplified implementation)
    auto now = std::chrono::high_resolution_clock::now();
    
    // Generate hourly report
    if (now.second() % 3600 == 0) {
        PerformanceReport report = generateReport("hourly", true, true);
        report.filePath = saveReport(report);
    }
    
    // Generate daily summary
    if (now.hour() % 24 == 0 && now.minute() == 0 && now.second() < 10) {
        PerformanceReport report = generateReport("daily_summary", true, true);
        report.filePath = saveReport(report);
    }
}

void PerformanceReporter::ensureOutputDirectoryExists() const {
    if (!std::filesystem::exists(outputDirectory_)) {
        std::filesystem::create_directories(outputDirectory_);
    }
}

void PerformanceReporter::cleanupOldReports() const {
    std::lock_guard<std::mutex> lock(reportsMutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto cutoffTime = now - std::chrono::days(30); // Delete reports older than 30 days
    
    auto it = reports_.begin();
    while (it != reports_.end()) {
        if (it->generationTime < cutoffTime) {
            it = reports_.erase(it);
        } else {
            ++it;
        }
    }
}

std::string PerformanceReporter::timestampToString(const std::chrono::high_resolution_clock::time_point& timestamp) const {
    auto duration = timestamp.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration % std::chrono::seconds(1));
    
    std::stringstream ss;
    ss << seconds.count() << "." << std::setfill('0') << std::setw(3) << milliseconds.count();
    return ss.str();
}
