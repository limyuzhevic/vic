#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "Monitoring.hpp"

namespace nlm {

class PerformanceReporter {
private:
    struct ReportConfig {
        bool includeTimestamps = true;
        bool includeComponents = true;
        bool includeTags = false;
        bool exportRawData = true;
        bool exportStatistics = true;
        bool exportAnomalies = true;
        std::string outputDirectory = "./reports";
    };

    std::mutex reportMutex_;
    ReportConfig config_;
    std::unordered_map<std::string, std::vector<MetricSample>> historicalReports_;

public:
    PerformanceReporter() = default;

    // Report generation
    std::string generateDetailedReport(const std::string& metricName = "",
                                       std::chrono::duration<double> range = std::chrono::hours(24)) {
        std::lock_guard<std::mutex> lock(reportMutex_);
        
        std::stringstream ss;
        ss << "=== Detailed Performance Report ===\n";
        ss << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        if (!metricName.empty()) {
            ss << "Metric: " << metricName << "\n";
            ss << "Range: Last " << range.count() << " seconds\n\n";
            
            auto monitor = &PerformanceMonitor::getInstance();
            auto metrics = monitor->getMetrics(metricName, range);
            
            if (metrics.empty()) {
                ss << "No data available for this metric.\n";
                return ss.str();
            }
            
            ss << "Data Points: " << metrics.size() << "\n";
            ss << "\n";
            
            // Raw data section
            if (config_.exportRawData) {
                ss << "Raw Data:\n";
                for (size_t i = 0; i < std::min<size_t>(10, metrics.size()); ++i) {
                    const auto& sample = metrics[i];
                    ss << "  [" << sample.timestamp.time_since_epoch().count() << "] "
                       << "Component: " << sample.component << ", "
                       << "Value: " << sample.value << "\n";
                }
                if (metrics.size() > 10) {
                    ss << "  ... and " << metrics.size() - 10 << " more points\n";
                }
                ss << "\n";
            }
            
            // Statistics section
            if (config_.exportStatistics) {
                ss << "Statistics:\n";
                ss << "  Min: " << getMinValue(metrics) << "\n";
                ss << "  Max: " << getMaxValue(metrics) << "\n";
                ss << "  Mean: " << getMeanValue(metrics) << "\n";
                ss << "  Median: " << getMedianValue(metrics) << "\n";
                ss << "  Standard Deviation: " << getStdDevValue(metrics) << "\n";
                ss << "\n";
            }
            
            // Component breakdown
            if (config_.includeComponents) {
                ss << "Component Breakdown:\n";
                std::unordered_map<std::string, std::vector<MetricSample>> byComponent;
                for (const auto& sample : metrics) {
                    byComponent[sample.component].push_back(sample);
                }
                
                for (const auto& [component, samples] : byComponent) {
                    ss << "  " << component << ": " << samples.size() << " points, "
                       << "avg: " << getMeanValue(samples) << "\n";
                }
                ss << "\n";
            }
            
            // Tags breakdown
            if (config_.includeTags) {
                ss << "Tags Breakdown:\n";
                // Implementation for tags analysis
                ss << "  (Tags analysis would go here)\n\n";
            }
        } else {
            // Generate overview report
            ss << "=== Overview Report ===\n\n";
            
            auto monitor = &PerformanceMonitor::getInstance();
            std::vector<std::string> metricNames;
            
            // Get all registered components
            ss << "Registered Components:\n";
            // Note: PerformanceMonitor doesn't expose component registry directly
            // This would need to be added to Monitoring.hpp
            ss << "  (Component listing would go here)\n\n";
            
            ss << "Recent Activity Summary:\n";
            // Show metrics from the last hour
            auto recentRange = std::chrono::hours(1);
            
            ss << "  Metrics from last hour:\n";
            // Note: This would query all metrics
            ss << "  (Metric listing would go here)\n\n";
        }
        
        // Anomaly detection
        if (config_.exportAnomalies) {
            ss << "Anomaly Detection:\n";
            // Anomaly detection would be implemented here
            ss << "  (Anomaly detection would go here)\n\n";
        }
        
        return ss.str();
    }

    std::string generateJSONReport(const std::string& metricName = "",
                                   std::chrono::duration<double> range = std::chrono::hours(24)) {
        std::lock_guard<std::mutex> lock(reportMutex_);
        
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"reportType\": \"detailed\",\n";
        ss << "  \"generatedAt\": \"" << getCurrentTimestamp() << "\",\n";
        
        if (!metricName.empty()) {
            ss << "  \"metric\": \"" << metricName << "\",\n";
            ss << "  \"rangeSeconds\": " << range.count() << ",\n";
            
            auto monitor = &PerformanceMonitor::getInstance();
            auto metrics = monitor->getMetrics(metricName, range);
            
            ss << "  \"dataPoints\": " << metrics.size() << ",\n";
            ss << "  \"data\": [\n";
            
            for (size_t i = 0; i < metrics.size(); ++i) {
                const auto& sample = metrics[i];
                ss << "    {\n";
                ss << "      \"timestamp\": " << sample.timestamp.time_since_epoch().count() << ",\n";
                ss << "      \"value\": " << sample.value << ",\n";
                ss << "      \"component\": \"" << sample.component << "\",\n";
                ss << "      \"type\": \"" << static_cast<int>(sample.type) << "\"\n";
                if (i < metrics.size() - 1) ss << "    },\n";
                else ss << "    }\n";
            }
            ss << "  ],\n";
            
            // Add statistics
            ss << "  \"statistics\": {\n";
            ss << "    \"min\": " << getMinValue(metrics) << ",\n";
            ss << "    \"max\": " << getMaxValue(metrics) << ",\n";
            ss << "    \"mean\": " << getMeanValue(metrics) << ",\n";
            ss << "    \"median\": " << getMedianValue(metrics) << ",\n";
            ss << "    \"stddev\": " << getStdDevValue(metrics) << "\n";
            ss << "  }\n";
        }
        
        ss << "}\n";
        return ss.str();
    }

    std::string generateCSVReport(const std::string& metricName = "",
                                  std::chrono::duration<double> range = std::chrono::hours(24)) {
        std::lock_guard<std::mutex> lock(reportMutex_);
        
        std::stringstream ss;
        ss << "timestamp,value,component,type\n";
        
        auto monitor = &PerformanceMonitor::getInstance();
        std::vector<MetricSample> metrics;
        
        if (!metricName.empty()) {
            metrics = monitor->getMetrics(metricName, range);
        } else {
            // Get all metrics
            // This would need access to all metric names
            // For now, create a simple overview
            ss << "report,generated_at\n";
            ss << "overview," << getCurrentTimestamp() << "\n";
            return ss.str();
        }
        
        for (const auto& sample : metrics) {
            ss << sample.timestamp.time_since_epoch().count() << ","
               << sample.value << ","
               << sample.component << ","
               << static_cast<int>(sample.type) << "\n";
        }
        
        return ss.str();
    }

    std::string generateHTMLReport(const std::string& metricName = "",
                                  std::chrono::duration<double> range = std::chrono::hours(24)) {
        std::lock_guard<std::mutex> lock(reportMutex_);
        
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n";
        ss << "<html><head><title>Performance Report</title>";
        ss << "<style>body { font-family: Arial, sans-serif; margin: 20px; } ";
        ss << "table { border-collapse: collapse; width: 100%; } ";
        ss << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; } ";
        ss << "th { background-color: #f2f2f2; } ";
        ss << "tr:nth-child(even) { background-color: #f9f9f9; }";
        ss << "</style></head><body>\n";
        
        ss << "<h1>Performance Metrics Report</h1>\n";
        ss << "<p>Generated: " << getCurrentTimestamp() << "</p>\n";
        
        if (!metricName.empty()) {
            ss << "<h2>Metric: " << metricName << "</h2>\n";
            ss << "<p>Range: Last " << range.count() << " seconds</p>\n";
            
            auto monitor = &PerformanceMonitor::getInstance();
            auto metrics = monitor->getMetrics(metricName, range);
            
            ss << "<table>\n";
            ss << "<tr><th>Timestamp</th><th>Value</th><th>Component</th><th>Type</th></tr>\n";
            
            for (const auto& sample : metrics) {
                ss << "<tr><td>" << sample.timestamp.time_since_epoch().count() << "</td>";
                ss << "<td>" << sample.value << "</td>";
                ss << "<td>" << sample.component << "</td>";
                ss << "<td>" << static_cast<int>(sample.type) << "</td></tr>\n";
            }
            ss << "</table>\n";
        }
        
        ss << "</body></html>\n";
        return ss.str();
    }

    // Export functionality
    void exportMetrics(const std::string& filepath, ReportFormat format = ReportFormat::Text,
                      const std::string& metricName = "") {
        std::lock_guard<std::mutex> lock(reportMutex_);
        
        std::string content;
        std::string extension;
        
        switch (format) {
            case ReportFormat::Text:
                content = generateDetailedReport(metricName);
                extension = ".txt";
                break;
            case ReportFormat::JSON:
                content = generateJSONReport(metricName);
                extension = ".json";
                break;
            case ReportFormat::CSV:
                content = generateCSVReport(metricName);
                extension = ".csv";
                break;
            case ReportFormat::HTML:
                content = generateHTMLReport(metricName);
                extension = ".html";
                break;
        }
        
        // Ensure directory exists
        std::string fullPath = filepath + extension;
        size_t lastSlash = fullPath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            std::string dir = fullPath.substr(0, lastSlash);
            // Create directory (simplified - in real implementation use filesystem)
        }
        
        // Write to file
        std::ofstream file(fullPath, std::ios::trunc);
        if (file.is_open()) {
            file << content;
            file.close();
        }
    }

    void exportAllMetrics(const std::string& directory) {
        std::lock_guard<std::mutex> lock(reportMutex_);
        
        // Export metrics for all available metrics
        auto monitor = &PerformanceMonitor::getInstance();
        
        // Note: This would need access to all metric names
        // For now, create a summary report
        std::string summary = "=== Performance Monitoring Summary ===\n";
        summary += "Generated: " + getCurrentTimestamp() + "\n\n";
        
        // Add summary information
        summary += "Core Infrastructure:\n";
        summary += "  - PerformanceMonitor: Active\n";
        summary += "  - PerformanceProfiler: Available\n";
        summary += "  - MetricCollector: Active\n";
        summary += "  - PerformanceReporter: Active\n\n";
        
        summary += "Metrics Categories:\n";
        summary += "  - System-level metrics (CPU, memory, I/O)\n";
        summary += "  - Simulation-level metrics (neuron firing, synapse activity)\n";
        summary += "  - Application-level metrics (experiment progress, throughput)\n";
        summary += "  - Specialized metrics (SIMD utilization, cache hit rates)\n\n";
        
        std::string filepath = directory + "/summary_" + getCurrentTimestamp() + ".txt";
        std::ofstream file(filepath, std::ios::trunc);
        if (file.is_open()) {
            file << summary;
            file.close();
        }
    }

    // Configuration
    void setReportConfig(const ReportConfig& config) {
        std::lock_guard<std::mutex> lock(reportMutex_);
        config_ = config;
    }

    ReportConfig getReportConfig() const {
        std::lock_guard<std::mutex> lock(reportMutex_);
        return config_;
    }

private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time), "%Y-%m-%d %H:%M:%S UTC");
        return ss.str();
    }

    double getMinValue(const std::vector<MetricSample>& metrics) {
        if (metrics.empty()) return 0.0;
        double min = metrics[0].value;
        for (const auto& sample : metrics) {
            if (sample.value < min) min = sample.value;
        }
        return min;
    }

    double getMaxValue(const std::vector<MetricSample>& metrics) {
        if (metrics.empty()) return 0.0;
        double max = metrics[0].value;
        for (const auto& sample : metrics) {
            if (sample.value > max) max = sample.value;
        }
        return max;
    }

    double getMeanValue(const std::vector<MetricSample>& metrics) {
        if (metrics.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& sample : metrics) {
            sum += sample.value;
        }
        return sum / metrics.size();
    }

    double getMedianValue(const std::vector<MetricSample>& metrics) {
        if (metrics.empty()) return 0.0;
        std::vector<double> values;
        for (const auto& sample : metrics) {
            values.push_back(sample.value);
        }
        std::sort(values.begin(), values.end());
        size_t mid = values.size() / 2;
        if (values.size() % 2 == 0) {
            return (values[mid - 1] + values[mid]) / 2.0;
        } else {
            return values[mid];
        }
    }

    double getStdDevValue(const std::vector<MetricSample>& metrics) {
        if (metrics.size() < 2) return 0.0;
        
        double mean = getMeanValue(metrics);
        double sumSquares = 0.0;
        for (const auto& sample : metrics) {
            sumSquares += (sample.value - mean) * (sample.value - mean);
        }
        
        return std::sqrt(sumSquares / (metrics.size() - 1));
    }
};

} // namespace nlm
