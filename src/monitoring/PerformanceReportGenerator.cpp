#include "PerformanceReportGenerator.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

PerformanceReportGenerator::PerformanceReportGenerator(
    std::shared_ptr<HistoricalMetricsStore> historicalStore,
    std::shared_ptr<PerformanceAnalyzer> analyzer,
    const ReportConfig& config)
    : historicalStore_(historicalStore), analyzer_(analyzer), config_(config) {}

bool PerformanceReportGenerator::generateReport(const std::string& reportName) {
    // Get all metrics
    std::vector<MetricData> metrics = historicalStore_->getAllSamples();
    
    if (metrics.empty()) {
        return false;
    }
    
    return generateReport(reportName, metrics);
}

bool PerformanceReportGenerator::generateReport(const std::string& reportName,
                                                const std::vector<MetricData>& metrics) {
    // Store report data
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (reports_.find(reportName) != reports_.end()) {
            return false; // Report already exists
        }
        
        // Generate report in background
        reportThreads_[reportName] = std::thread(&PerformanceReportGenerator::generateReportWorker,
                                                 this, reportName, metrics);
    }
    
    return true;
}

bool PerformanceReportGenerator::generateReport(const std::string& reportName,
                                                const std::chrono::system_clock::time_point& start,
                                                const std::chrono::system_clock::time_point& end) {
    // Get metrics for time range
    std::vector<MetricData> metrics = historicalStore_->retrieveMetrics(
        "", start, end // Get all metrics
    );
    
    if (metrics.empty()) {
        return false;
    }
    
    return generateReport(reportName, metrics);
}

std::string PerformanceReportGenerator::getReportData(const std::string& reportName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = reports_.find(reportName);
    if (it == reports_.end()) {
        return ""; // Report not found
    }
    
    return it->second;
}

std::string PerformanceReportGenerator::exportToJSON(const std::string& reportName) const {
    std::string reportData = getReportData(reportName);
    if (reportData.empty()) {
        return ""; // Report not found
    }
    
    try {
        json report = json::parse(reportData);
        return report.dump(2);
    } catch (const json::exception& e) {
        return ""; // Parse error
    }
}

std::string PerformanceReportGenerator::exportToHTML(const std::string& reportName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = reports_.find(reportName);
    if (it == reports_.end()) {
        return ""; // Report not found
    }
    
    // For now, just return the JSON data as a simple HTML page
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "    <title>" << reportName << "</title>\n";
    html << "    <style>\n";
    html << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    html << "        pre { background: #f5f5f5; padding: 10px; overflow-x: auto; }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <h1>" << reportName << "</h1>\n";
    html << "    <pre>" << it->second << "</pre>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string PerformanceReportGenerator::exportToCSV(const std::string& reportName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = reports_.find(reportName);
    if (it == reports_.end()) {
        return ""; // Report not found
    }
    
    // For now, just return the JSON data as CSV
    // In a real implementation, you would parse the JSON and convert to CSV format
    return "CSV export not yet implemented for JSON data";
}

PerformanceReportGenerator::ReportMetadata PerformanceReportGenerator::getReportMetadata(
    const std::string& reportName) const {
    ReportMetadata metadata(reportName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = reports_.find(reportName);
    if (it == reports_.end()) {
        return metadata; // Report not found
    }
    
    // Get report size
    metadata.size = it->second.size();
    metadata.generatedTime = std::chrono::system_clock::now();
    metadata.format = "JSON";
    metadata.contentType = "application/json";
    
    return metadata;
}

std::vector<PerformanceReportGenerator::ReportMetadata> PerformanceReportGenerator::listReports() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ReportMetadata> reports;
    
    for (const auto& pair : reports_) {
        ReportMetadata metadata(pair.first);
        metadata.size = pair.second.size();
        metadata.generatedTime = std::chrono::system_clock::now();
        metadata.format = "JSON";
        metadata.contentType = "application/json";
        
        reports.push_back(metadata);
    }
    
    return reports;
}

void PerformanceReportGenerator::cleanupOldReports() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::system_clock::now();
    auto cutoffTime = now - std::chrono::hours(24 * config_.getReportRetentionDays());
    
    // TODO: Implement report retention logic
    // This would remove old report files from disk storage
}

void PerformanceReportGenerator::generateReportWorker(const std::string& reportName,
                                                     const std::vector<MetricData>& metrics) {
    try {
        // Generate all report components
        ExecutiveSummary executiveSummary = generateExecutiveSummary(metrics);
        std::vector<TrendAnalysis> trends = generateTrendAnalysis(metrics);
        std::vector<AnomalyReport> anomalies = generateAnomalyReport(metrics);
        std::vector<Recommendation> recommendations = generateRecommendations(metrics);
        
        // Generate report data
        std::string reportData = generateJSONReport(
            reportName, executiveSummary, trends, anomalies, recommendations
        );
        
        // Store report
        {
            std::lock_guard<std::mutex> lock(mutex_);
            reports_[reportName] = reportData;
        }
        
        // Save to file if configured
        if (!config_.outputPath.empty()) {
            std::string filePath = config_.outputPath + "/" + reportName + ".json";
            std::ofstream file(filePath, std::ios::trunc);
            if (file.is_open()) {
                file << reportData;
                file.close();
            }
        }
        
        // Clean up thread
        {
            std::lock_guard<std::mutex> lock(mutex_);
            reportThreads_.erase(reportName);
        }
        
    } catch (const std::exception& e) {
        // Clean up thread on error
        {
            std::lock_guard<std::mutex> lock(mutex_);
            reportThreads_.erase(reportName);
        }
    }
}

ExecutiveSummary PerformanceReportGenerator::generateExecutiveSummary(
    const std::vector<MetricData>& metrics) const {
    ExecutiveSummary summary;
    
    summary.reportDate = std::chrono::system_clock::now();
    summary.totalMetricsAnalyzed = metrics.size();
    
    // Calculate performance score
    summary.overallPerformanceScore = calculatePerformanceScore(metrics);
    
    // Generate key findings
    std::stringstream findings;
    for (const auto& metric : metrics) {
        if (metric.value == 0.0) {
            findings << "Metric " << metric.name << " has zero value. ";
        }
    }
    summary.keyFindings = findings.str();
    
    // Generate recommendations
    std::stringstream recommendations;
    if (metrics.size() > 10) {
        recommendations << "Consider optimizing performance metrics. ";
    }
    if (summary.overallPerformanceScore < 50.0) {
        recommendations << "Immediate attention required for performance issues. ";
    }
    summary.recommendations = recommendations.str();
    
    // Determine health status
    summary.healthStatus = determineHealthStatus(summary.overallPerformanceScore, summary.totalMetricsAnalyzed);
    
    return summary;
}

std::vector<TrendAnalysis> PerformanceReportGenerator::generateTrendAnalysis(
    const std::vector<MetricData>& metrics) const {
    std::vector<TrendAnalysis> trends;
    
    if (!analyzer_) {
        return trends; // No analyzer available
    }
    
    // Group metrics by name
    std::map<std::string, std::vector<MetricData>> metricGroups;
    for (const auto& metric : metrics) {
        metricGroups[metric.name].push_back(metric);
    }
    
    // Analyze trends for each metric group
    for (const auto& pair : metricGroups) {
        const auto& name = pair.first;
        const auto& metricData = pair.second;
        
        if (metricData.size() < 2) {
            continue; // Not enough data for trend analysis
        }
        
        TrendAnalysis trend(name);
        
        // Calculate current and previous values (simple approach)
        trend.currentValue = metricData.back().value;
        trend.previousValue = metricData.front().value;
        
        if (trend.previousValue != 0.0) {
            trend.trendPercentage = ((trend.currentValue - trend.previousValue) / trend.previousValue) * 100.0;
        }
        
        // Determine trend direction
        if (trend.trendPercentage > 5.0) {
            trend.trendDirection = "increasing";
        } else if (trend.trendPercentage < -5.0) {
            trend.trendDirection = "decreasing";
        } else {
            trend.trendDirection = "stable";
        }
        
        trend.analysisDate = std::chrono::system_clock::now();
        
        trends.push_back(trend);
    }
    
    return trends;
}

std::vector<AnomalyReport> PerformanceReportGenerator::generateAnomalyReport(
    const std::vector<MetricData>& metrics) const {
    std::vector<AnomalyReport> anomalies;
    
    if (!analyzer_) {
        return anomalies; // No analyzer available
    }
    
    // Get anomaly data from analyzer
    auto anomalyResults = analyzer_->detectAnomalies(metrics);
    
    for (const auto& anomalyResult : anomalyResults) {
        AnomalyReport anomalyReport("anomaly_" + std::to_string(std::rand()));
        
        anomalyReport.metricName = anomalyResult.metricName;
        anomalyReport.anomalyType = anomalyResult.anomalyType;
        anomalyReport.anomalousValue = anomalyResult.anomalousValue;
        anomalyReport.expectedValue = anomalyResult.expectedValue;
        anomalyReport.severity = anomalyResult.severity;
        anomalyReport.detectionTime = anomalyResult.detectionTime;
        
        // Generate description
        std::stringstream description;
        description << "Metric " << anomalyResult.metricName << " has " << anomalyResult.anomalyType
                   << " value " << anomalyResult.anomalousValue
                   << " (expected: " << anomalyResult.expectedValue << ")";
        anomalyReport.description = description.str();
        
        // Generate suggested action
        std::stringstream action;
        if (anomalyResult.severity > 0.8) {
            action << "Immediate investigation required. Consider reducing load or adjusting parameters.";
        } else if (anomalyResult.severity > 0.5) {
            action << "Monitor closely and plan investigation within 24 hours.";
        } else {
            action << "Monitor and log for future analysis.";
        }
        anomalyReport.suggestedAction = action.str();
        
        anomalies.push_back(anomalyReport);
    }
    
    return anomalies;
}

std::vector<Recommendation> PerformanceReportGenerator::generateRecommendations(
    const std::vector<MetricData>& metrics) const {
    std::vector<Recommendation> recommendations;
    
    // Analyze metrics and generate recommendations
    for (const auto& metric : metrics) {
        // Check for problematic values
        if (metric.value == 0.0 && metric.category.find("performance") != std::string::npos) {
            Recommendation rec("rec_" + std::to_string(std::rand()));
            rec.category = "performance";
            rec.title = "Optimize " + metric.name + " metric";
            rec.description = "The " + metric.name + " metric has a zero value. Consider investigating potential issues with the underlying system component.";
            rec.priority = "high";
            rec.impactScore = 80.0;
            rec.suggestedDate = std::chrono::system_clock::now() + std::chrono::hours(24);
            rec.isImplemented = false;
            
            recommendations.push_back(rec);
        }
    }
    
    return recommendations;
}

std::string PerformanceReportGenerator::generateHTMLReport(
    const std::string& reportName,
    const ExecutiveSummary& executiveSummary,
    const std::vector<TrendAnalysis>& trends,
    const std::vector<AnomalyReport>& anomalies,
    const std::vector<Recommendation>& recommendations) const {
    
    std::stringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "    <title>" << reportName << " - Performance Report</title>\n";
    html << "    <style>\n";
    html << "        body { font-family: Arial, sans-serif; margin: 20px; line-height: 1.6; }\n";
    html << "        .header { background: #f0f0f0; padding: 20px; border-radius: 8px; margin-bottom: 20px; }\n";
    html << "        .summary { background: #e8f4fd; padding: 15px; border-left: 4px solid #2196F3; margin: 10px 0; }\n";
    html << "        .trend { background: #f9f9f9; padding: 10px; border-left: 4px solid #4CAF50; margin: 5px 0; }\n";
    html << "        .anomaly { background: #ffebee; padding: 10px; border-left: 4px solid #f44336; margin: 5px 0; }\n";
    html << "        .recommendation { background: #fff3e0; padding: 10px; border-left: 4px solid #ff9800; margin: 5px 0; }\n";
    html << "        .good { color: #4CAF50; font-weight: bold; }\n";
    html << "        .warning { color: #ff9800; font-weight: bold; }\n";
    html << "        .critical { color: #f44336; font-weight: bold; }\n";
    html << "        table { border-collapse: collapse; width: 100%; margin: 10px 0; }\n";
    html << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    html << "        th { background-color: #f2f2f2; }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    
    // Header
    html << "<div class=\"header\">\n";
    html << "    <h1>" << reportName << "</h1>\n";
    html << "    <p><strong>Generated:</strong> " << std::put_time(std::localtime(&std::time(nullptr)), "%Y-%m-%d %H:%M:%S") << "</p>\n";
    html << "    <p><strong>Description:</strong> " << config_.description << "</p>\n";
    html << "    <p><strong>Date Range:</strong> " << config_.dateRange << "</p>\n";
    html << "</div>\n";
    
    // Executive Summary
    html << "<div class=\"summary\">\n";
    html << "    <h2>Executive Summary</h2>\n";
    html << "    <p><strong>Overall Performance Score:</strong> <span class=\"" << 
        (executiveSummary.healthStatus == "good" ? "good" : 
         executiveSummary.healthStatus == "warning" ? "warning" : "critical") << "\">" << 
        executiveSummary.overallPerformanceScore << "</span></p>\n";
    html << "    <p><strong>Health Status:</strong> <span class=\"" << 
        (executiveSummary.healthStatus == "good" ? "good" : 
         executiveSummary.healthStatus == "warning" ? "warning" : "critical") << "\">" << 
        executiveSummary.healthStatus << "</span></p>\n";
    html << "    <p><strong>Metrics Analyzed:</strong> " << executiveSummary.totalMetricsAnalyzed << "</p>\n";
    html << "    <p><strong>Anomalies Detected:</strong> " << executiveSummary.anomalyCount << "</p>\n";
    html << "    <p><strong>Key Findings:</strong> " << executiveSummary.keyFindings << "</p>\n";
    html << "    <p><strong>Recommendations:</strong> " << executiveSummary.recommendations << "</p>\n";
    html << "</div>\n";
    
    // Trends
    if (!trends.empty()) {
        html << "<h2>Performance Trends</h2>\n";
        html << "<table>\n";
        html << "<tr><th>Metric</th><th>Current Value</th><th>Previous Value</th><th>Trend</th><th>Percentage Change</th></tr>\n";
        
        for (const auto& trend : trends) {
            html << "<tr><td>" << trend.metricName << "</td><td>" << trend.currentValue << "</td><td>" << trend.previousValue << "</td><td>" << trend.trendDirection << "</td><td>" << trend.trendPercentage << "%</td></tr>\n";
        }
        
        html << "</table>\n";
    }
    
    // Anomalies
    if (!anomalies.empty()) {
        html << "<h2>Anomalies Detected</h2>\n";
        
        for (const auto& anomaly : anomalies) {
            html << "<div class=\"anomaly\">\n";
            html << "    <h3>" << anomaly.metricName << " - " << anomaly.anomalyType << "</h3>\n";
            html << "    <p><strong>Value:</strong> " << anomaly.anomalousValue << " (expected: " << anomaly.expectedValue << ")</p>\n";
            html << "    <p><strong>Severity:</strong> " << anomaly.severity << "</p>\n";
            html << "    <p><strong>Description:</strong> " << anomaly.description << "</p>\n";
            html << "    <p><strong>Suggested Action:</strong> " << anomaly.suggestedAction << "</p>\n";
            html << "</div>\n";
        }
    }
    
    // Recommendations
    if (!recommendations.empty()) {
        html << "<h2>Recommendations</h2>\n";
        
        for (const auto& recommendation : recommendations) {
            html << "<div class=\"recommendation\">\n";
            html << "    <h3>" << recommendation.title << "</h3>\n";
            html << "    <p><strong>Priority:</strong> <span class=\"" << 
                (recommendation.priority == "critical" ? "critical" : 
                 recommendation.priority == "high" ? "warning" : "good") << "\">" << 
                recommendation.priority << "</span></p>\n";
            html << "    <p><strong>Description:</strong> " << recommendation.description << "</p>\n";
            html << "    <p><strong>Impact Score:</strong> " << recommendation.impactScore << "</p>\n";
            html << "    <p><strong>Suggested Date:</strong> " << std::put_time(std::localtime(&std::time(nullptr)), "%Y-%m-%d") << "</p>\n";
            html << "</div>\n";
        }
    }
    
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string PerformanceReportGenerator::generateJSONReport(
    const std::string& reportName,
    const ExecutiveSummary& executiveSummary,
    const std::vector<TrendAnalysis>& trends,
    const std::vector<AnomalyReport>& anomalies,
    const std::vector<Recommendation>& recommendations) const {
    
    json report;
    
    report["reportName"] = reportName;
    report["generatedAt"] = std::chrono::system_clock::now();
    report["title"] = config_.title;
    report["description"] = config_.description;
    report["dateRange"] = config_.dateRange;
    
    // Executive summary
    json summary;
    summary["overallPerformanceScore"] = executiveSummary.overallPerformanceScore;
    summary["totalMetricsAnalyzed"] = executiveSummary.totalMetricsAnalyzed;
    summary["anomalyCount"] = executiveSummary.anomalyCount;
    summary["keyFindings"] = executiveSummary.keyFindings;
    summary["recommendations"] = executiveSummary.recommendations;
    summary["healthStatus"] = executiveSummary.healthStatus;
    
    report["executiveSummary"] = summary;
    
    // Trends
    json trendsArray = json::array();
    for (const auto& trend : trends) {
        json trendJson;
        trendJson["metricName"] = trend.metricName;
        trendJson["currentValue"] = trend.currentValue;
        trendJson["previousValue"] = trend.previousValue;
        trendJson["trendPercentage"] = trend.trendPercentage;
        trendJson["trendDirection"] = trend.trendDirection;
        trendJson["analysisDate"] = trend.analysisDate;
        
        trendsArray.push_back(trendJson);
    }
    
    report["trends"] = trendsArray;
    
    // Anomalies
    json anomaliesArray = json::array();
    for (const auto& anomaly : anomalies) {
        json anomalyJson;
        anomalyJson["anomalyId"] = anomaly.anomalyId;
        anomalyJson["metricName"] = anomaly.metricName;
        anomalyJson["anomalyType"] = anomaly.anomalyType;
        anomalyJson["anomalousValue"] = anomaly.anomalousValue;
        anomalyJson["expectedValue"] = anomaly.expectedValue;
        anomalyJson["severity"] = anomaly.severity;
        anomalyJson["detectionTime"] = anomaly.detectionTime;
        anomalyJson["description"] = anomaly.description;
        anomalyJson["suggestedAction"] = anomaly.suggestedAction;
        
        anomaliesArray.push_back(anomalyJson);
    }
    
    report["anomalies"] = anomaliesArray;
    
    // Recommendations
    json recommendationsArray = json::array();
    for (const auto& recommendation : recommendations) {
        json recommendationJson;
        recommendationJson["id"] = recommendation.id;
        recommendationJson["category"] = recommendation.category;
        recommendationJson["title"] = recommendation.title;
        recommendationJson["description"] = recommendation.description;
        recommendationJson["priority"] = recommendation.priority;
        recommendationJson["impactScore"] = recommendation.impactScore;
        recommendationJson["suggestedDate"] = recommendation.suggestedDate;
        recommendationJson["isImplemented"] = recommendation.isImplemented;
        recommendationJson["implementationNotes"] = recommendation.implementationNotes;
        
        recommendationsArray.push_back(recommendationJson);
    }
    
    report["recommendations"] = recommendationsArray;
    
    return report.dump(2);
}

double PerformanceReportGenerator::calculatePerformanceScore(
    const std::vector<MetricData>& metrics) const {
    if (metrics.empty()) return 0.0;
    
    double totalScore = 0.0;
    size_t validMetrics = 0;
    
    for (const auto& metric : metrics) {
        // Simple scoring: normalize value to 0-100 scale
        double normalizedValue = (metric.value / config_.getMaxMetricValue()) * 100.0;
        totalScore += normalizedValue;
        validMetrics++;
    }
    
    return validMetrics > 0 ? totalScore / validMetrics : 0.0;
}

std::string PerformanceReportGenerator::determineHealthStatus(
    double performanceScore, size_t anomalyCount) const {
    if (performanceScore >= 90.0 && anomalyCount == 0) {
        return "good";
    } else if (performanceScore >= 70.0 && anomalyCount <= 3) {
        return "warning";
    } else {
        return "critical";
    }
}

} // namespace nlm
