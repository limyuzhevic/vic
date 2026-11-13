#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

    class VisualizationTools {
    public:
        struct ChartData {
            std::string chartType; // "line", "bar", "histogram", "pie"
            std::string chartName;
            std::vector<std::pair<double, double>> dataPoints; // time-value pairs
            std::vector<std::string> labels;
            std::string unit;
            std::chrono::system_clock::time_point timestamp;
        };

        struct SystemVisualization {
            std::chrono::system_clock::time_point timestamp;
            std::string title;
            std::vector<ChartData> charts;
            std::unordered_map<std::string, std::string> visualizations;
            std::vector<std::string> errorTrends;
            std::vector<std::string> performanceMetrics;
        };

    public:
        VisualizationTools();
        ~VisualizationTools();

        // Real-time data visualization
        void updateVisualization(const std::string& metricName, double value,
                                 const std::string& chartType = "line",
                                 const std::vector<std::string>& labels = {});

        void addDataPoint(const std::string& chartName, double xValue, double yValue,
                         const std::string& label = "", const std::string& color = "blue");

        void createChart(const ChartData& chartData);

        // Performance graphs
        void plotPerformance(const std::vector<double>& firingRates,
                           const std::vector<double>& memoryUsage,
                           const std::vector<double>& cpuUsage,
                           const std::chrono::system_clock::time_point& startTime);

        void plotLearningProgress(const std::vector<double>& progressValues,
                                 const std::vector<std::string>& algorithmNames);

        void plotErrorTrends(const std::vector<std::pair<std::chrono::system_clock::time_point, std::string>>& errorData);

        // System state visualization
        void visualizeSystemState(const std::unordered_map<std::string, std::string>& systemState,
                                const std::vector<std::string>& activeComponents,
                                double cpuUsagePercent = 0.0,
                                double memoryUsagePercent = 0.0);

        void generateSystemDashboard(const std::string& title,
                                    const std::vector<ChartData>& dashboardCharts);

        // Error trend analysis
        void trackErrorFrequency(const std::string& errorType, const std::string& errorMessage,
                               const std::unordered_map<std::string, std::string>& context = {});

        void plotErrorFrequency(const std::vector<std::string>& errorTypes,
                               const std::vector<int>& frequencies);

        // Visualization queries
        SystemVisualization getCurrentVisualization() const;
        std::vector<SystemVisualization> getVisualizationHistory() const;
        ChartData getChartData(const std::string& chartName) const;
        std::vector<ChartData> getAllCharts() const;

        // Visualization management
        void clearAllVisualizations();
        void clearChart(const std::string& chartName);
        bool hasChart(const std::string& chartName) const;

        // Generate reports
        std::string generateVisualizationReport() const;
        std::string generatePerformanceReport() const;
        std::string generateErrorAnalysisReport() const;

        // Real-time monitoring
        void startRealTimeMonitoring(std::chrono::milliseconds interval = std::chrono::milliseconds(1000));
        void stopRealTimeMonitoring();
        bool isRealTimeMonitoringRunning() const;

        // Visualization export
        std::string exportVisualizationsToJSON() const;
        std::string exportVisualizationsToHTML() const;
        std::string exportChartsToCSV() const;

        // Utility methods
        std::string generateChartHTML(const ChartData& chartData) const;
        std::string generateDashboardHTML(const std::vector<ChartData>& charts) const;
        std::string generateInteractiveDashboard() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;

        // Thread safety
        mutable std::mutex mutex_;

        // Helper methods
        std::string sanitizeChartName(const std::string& name) const;
        std::string chartTypeToFormat(const std::string& chartType) const;
        std::string colorToHex(const std::string& colorName) const;
    };

} // namespace nlm
