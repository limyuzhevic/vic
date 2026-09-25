#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace nlm {

// HTML and visualization components
struct HTMLTemplate {
    std::string header;
    std::string styles;
    std::string navbar;
    std::string sidebar;
    std::string mainContent;
    std::string footer;
    
    HTMLTemplate() {
        header = "<!DOCTYPE html>\n<html>\n<head>\n<title>NLM Monitoring Dashboard</title>\n";
        styles = "<style>\n";
        navbar = "</head>\n<body>\n<nav class=\"navbar\">\n";
        sidebar = "</nav>\n<aside class=\"sidebar\">\n";
        mainContent = "</aside>\n<main class=\"main-content\">\n";
        footer = "</main>\n<footer>\n";
    }
    
    std::string getHTML() const {
        return header + styles + "</style>\n" + navbar + "</nav>\n" + 
               sidebar + "</aside>\n" + mainContent + "</main>\n" + 
               footer + "</footer>\n</body>\n</html>";
    }
};

// Log visualization data
struct LogVisualizationData {
    std::vector<std::string> timestamps;
    std::vector<std::string> logLevels;
    std::vector<std::string> categories;
    std::vector<std::string> messages;
    std::map<std::string, std::vector<double>> metricSeries;
    std::map<std::string, std::vector<std::string>> contextData;
    
    void addEntry(const std::string& timestamp, const std::string& level,
                  const std::string& category, const std::string& message) {
        timestamps.push_back(timestamp);
        logLevels.push_back(level);
        categories.push_back(category);
        messages.push_back(message);
    }
    
    void addMetricSeries(const std::string& name, const std::vector<double>& values) {
        metricSeries[name] = values;
    }
    
    void addContextData(const std::string& key, const std::vector<std::string>& values) {
        contextData[key] = values;
    }
};

// Chart configuration
struct ChartConfig {
    std::string type; // line, bar, pie, heatmap
    std::string title;
    std::string xAxisLabel;
    std::string yAxisLabel;
    std::map<std::string, std::string> series;
    std::map<std::string, std::string> styling;
    
    ChartConfig() : type("line"), xAxisLabel("Time"), yAxisLabel("Value") {
        styling["backgroundColor"] = "#ffffff";
        styling["borderColor"] = "#dddddd";
    }
};

// Log formatter interface
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const std::string& data, const std::string& formatType) = 0;
    virtual std::string getFormatName() const = 0;
    virtual std::vector<std::string> getSupportedFormats() const = 0;
};

// JSON formatter implementation
class JSONFormatter : public ILogFormatter {
public:
    JSONFormatter();
    ~JSONFormatter() override;
    
    std::string format(const std::string& data, const std::string& formatType) override;
    std::string getFormatName() const override;
    std::vector<std::string> getSupportedFormats() const override;
    
    std::string formatLogEntry(const nlohmann::json& entry) const;
    std::string formatMetrics(const nlohmann::json& metrics) const;
    std::string formatSystemInfo(const nlohmann::json& systemInfo) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// CSV formatter implementation
class CSVFormatter : public ILogFormatter {
public:
    CSVFormatter();
    ~CSVFormatter() override;
    
    std::string format(const std::string& data, const std::string& formatType) override;
    std::string getFormatName() const override;
    std::vector<std::string> getSupportedFormats() const override;
    
    std::string formatAsCSV(const nlohmann::json& data) const;
    std::string formatMetricsCSV(const nlohmann::json& metrics) const;
    std::string formatHeaders(const std::vector<std::string>& columns) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// HTML formatter implementation
class HTMLFormatter : public ILogFormatter {
public:
    HTMLFormatter();
    ~HTMLFormatter() override;
    
    std::string format(const std::string& data, const std::string& formatType) override;
    std::string getFormatName() const override;
    std::vector<std::string> getSupportedFormats() const override;
    
    std::string createDashboard(const nlohmann::json& dashboardData) const;
    std::string createChart(const ChartConfig& config, const std::vector<double>& data) const;
    std::string createLogViewer(const LogVisualizationData& logData) const;
    std::string createMetricsTable(const nlohmann::json& metrics) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plain text formatter with colors
class PlainTextFormatter : public ILogFormatter {
public:
    PlainTextFormatter();
    ~PlainTextFormatter() override;
    
    std::string format(const std::string& data, const std::string& formatType) override;
    std::string getFormatName() const override;
    std::vector<std::string> getSupportedFormats() const override;
    
    std::string formatColored(const nlohmann::json& entry) const;
    std::string formatSimple(const nlohmann::json& entry) const;
    std::string formatDetailed(const nlohmann::json& entry) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToColorCode(const std::string& level) const;
    std::string levelToName(const std::string& level) const;
    std::string getCurrentTimestamp() const;
};

// Advanced LogFormatter class with multiple formatting capabilities
class LogFormatter {
public:
    LogFormatter();
    ~LogFormatter();
    
    // Disable copying, enable moving
    LogFormatter(const LogFormatter&) = delete;
    LogFormatter& operator=(const LogFormatter&) = delete;
    LogFormatter(LogFormatter&&) noexcept;
    LogFormatter& operator=(LogFormatter&&) noexcept;
    
    // Formatting methods
    std::string formatJSON(const std::string& data) const;
    std::string formatCSV(const std::string& data) const;
    std::string formatHTML(const std::string& data) const;
    std::string formatPlainText(const std::string& data, bool colored = true) const;
    std::string formatCustom(const std::string& data, const std::string& format) const;
    
    // Advanced formatting features
    std::string formatDashboard(const nlohmann::json& dashboardData) const;
    std::string formatChart(const ChartConfig& config, const std::vector<double>& data) const;
    std::string formatLogViewer(const LogVisualizationData& logData) const;
    std::string formatMetricsTable(const nlohmann::json& metrics) const;
    std::string formatAlert(const nlohmann::json& alert) const;
    
    // Configuration
    void addFormatter(std::shared_ptr<ILogFormatter> formatter);
    void removeFormatter(const std::string& formatName);
    void clearFormatters();
    bool hasFormatter(const std::string& formatName) const;
    
    // Format management
    void setDefaultFormat(const std::string& format);
    std::string getDefaultFormat() const;
    void registerFormatter(std::shared_ptr<ILogFormatter> formatter);
    void unregisterFormatter(const std::string& formatName);
    
    // Configuration
    void setHTMLTemplate(const HTMLTemplate& template);
    void enableCharts(bool enable);
    void setChartDefaults(const ChartConfig& defaults);
    void setColorScheme(const std::string& scheme);
    
    // Data preparation
    nlohmann::json prepareLogData(const std::string& filename) const;
    nlohmann::json prepareMetricsData(const std::string& filename) const;
    nlohmann::json prepareDashboardData(const std::string& timeframe) const;
    
    // Visualization
    LogVisualizationData extractLogData(const std::string& filename) const;
    std::vector<ChartConfig> createStandardCharts(const LogVisualizationData& data) const;
    std::string generateHTMLReport(const LogVisualizationData& logData,
                                   const std::vector<ChartConfig>& charts) const;
    
    // Export
    bool exportToFile(const std::string& data, const std::string& filename, 
                     const std::string& format = "auto") const;
    std::string exportToString(const std::string& data, const std::string& format) const;
    
    // Static instance management
    static LogFormatter& getInstance();
    static void setGlobal(std::shared_ptr<LogFormatter> formatter);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string detectFormat(const std::string& filename) const;
    nlohmann::json parseLogData(const std::string& filename) const;
    std::string createUniqueFilename(const std::string& baseName, 
                                     const std::string& extension) const;
};

// Log file analyzer
class LogFileAnalyzer {
public:
    LogFileAnalyzer();
    ~LogFileAnalyzer();
    
    // Log analysis
    nlohmann::json analyzeLogFile(const std::string& filename) const;
    nlohmann::json analyzeLogDirectory(const std::string& directory) const;
    std::string getLogSummary(const std::string& filename) const;
    std::string getLogStatistics(const std::string& filename) const;
    
    // Pattern detection
    std::vector<std::string> detectLogPatterns(const std::string& filename) const;
    std::map<std::string, double> detectLogTrends(const std::string& filename) const;
    std::map<std::string, size_t> countLogLevels(const std::string& filename) const;
    std::map<std::string, size_t> countLogCategories(const std::string& filename) const;
    
    // Time analysis
    std::chrono::system_clock::time_point getLogStartTime(const std::string& filename) const;
    std::chrono::system_clock::time_point getLogEndTime(const std::string& filename) const;
    std::chrono::milliseconds getLogDuration(const std::string& filename) const;
    
    // Content analysis
    std::vector<std::string> extractUniqueMessages(const std::string& filename) const;
    std::vector<std::string> extractUniqueCategories(const std::string& filename) const;
    std::vector<std::string> extractErrorMessages(const std::string& filename) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    nlohmann::json parseLogFile(const std::string& filename) const;
    std::string extractTimestampFromLine(const std::string& line) const;
    std::string extractLogLevelFromLine(const std::string& line) const;
    std::string extractCategoryFromLine(const std::string& line) const;
    std::string extractMessageFromLine(const std::string& line) const;
};

// Log export utility
class LogExportUtility {
public:
    LogExportUtility();
    ~LogExportUtility();
    
    // Export functionality
    bool exportLogs(const std::string& sourceFile, const std::string& destination,
                   const std::string& format = "auto") const;
    bool exportLogsToArchive(const std::string& directory, const std::string& archiveName,
                           const std::string& format = "json") const;
    
    // Import functionality
    bool importLogs(const std::string& source, const std::string& destination,
                   const std::string& format = "auto") const;
    
    // Compression
    bool compressFile(const std::string& filename) const;
    bool decompressFile(const std::string& filename) const;
    
    // Validation
    bool validateLogFile(const std::string& filename) const;
    bool validateLogFormat(const std::string& data, const std::string& format) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string createBackupFilename(const std::string& original) const;
};

} // namespace nlm