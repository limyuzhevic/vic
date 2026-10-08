#pragma once

#include "VisualizationInterface.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <deque>
#include <glm/glm.hpp>

namespace nlm {

class Brain;
class PerformanceMetrics;

class PerformanceDashboard : public VisualizationInterface {
public:
    PerformanceDashboard();
    ~PerformanceDashboard() override;
    
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    bool supports3D() const override;
    std::string getName() const override;
    std::string getVersion() const override;
    void onResize(int width, int height) override;
    void setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    void setComponentEnabled(const std::string& component, bool enabled) override;
    bool isComponentEnabled(const std::string& component) const override;
    void addEventCallback(const std::string& eventType, std::function<void(const std::string&)> callback) override;
    bool exportState(const std::string& filename, const std::string& format) override;
    bool importState(const std::string& filename, const std::string& format) override;
    
    // Performance monitoring specific methods
    void monitorSystemPerformance(const PerformanceMetrics& metrics);
    void visualizeCPUUsage(double usage, const glm::vec3& position, float size);
    void visualizeMemoryUsage(double used, double total, const glm::vec3& position, float size);
    void visualizeNetworkActivity(double activity, const glm::vec3& position, float size);
    void visualizeSimulationTime(double elapsed, double target, const glm::vec3& position, float size);
    
    // Performance metrics visualization
    enum class MetricType {
        CPU,
        MEMORY,
        GPU,
        NETWORK,
        DISK,
        SIMULATION_TIME,
        NEURAL_ACTIVITY,
        PLOTTICITY,
        SPIKE_RATE,
        CONNECTIONS
    };
    
    void addMetric(MetricType type, double value, double timestamp);
    void removeMetric(MetricType type);
    void clearMetrics();
    
    // Real-time metrics display
    void visualizeRealTimeMetrics(const glm::vec3& position, float width, float height);
    void visualizeMetricHistory(MetricType type, const glm::vec3& position, float width, float height);
    void visualizeMetricGraph(MetricType type, const glm::vec3& position, float width, float height, double timeWindow);
    
    // Dashboard layout
    enum class DashboardLayout {
        GRID,
        TILED,
        HORIZONTAL,
        VERTICAL,
        CUSTOM
    };
    
    void setDashboardLayout(DashboardLayout layout);
    void setWidgetSize(const std::string& widgetId, float width, float height);
    void setWidgetPosition(const std::string& widgetId, const glm::vec2& position);
    
    // Threshold and alerting
    void setThreshold(MetricType type, double threshold, bool above = true);
    void clearThresholds();
    void checkThresholds();
    std::vector<MetricType> getAlertedMetrics() const;
    
    // Performance analysis
    void analyzePerformanceTrends(double analysisWindow);
    void detectPerformanceAnomalies(double sensitivity);
    void visualizeBottlenecks(const Brain& brain);
    void visualizeOptimizationOpportunities(const Brain& brain);
    
    // Performance alerts
    void addAlert(const std::string& message, MetricType type, double value);
    void removeAlert(const std::string& alertId);
    void clearAlerts();
    void displayAlerts(const glm::vec3& position, float width, float height);
    
    // Export and reporting
    void generatePerformanceReport(const std::string& filename, const std::string& format = "json");
    void exportMetrics(const std::string& filename, const std::string& format = "csv");
    void importMetrics(const std::string& filename, const std::string& format = "csv");
    
    // Performance recommendations
    void generatePerformanceRecommendations();
    void displayRecommendations(const glm::vec3& position, float width, float height);
    
    // Historical data management
    void enableHistoricalDataRecording(bool enable);
    void setHistoricalDataDuration(double duration);
    void cleanupOldData();
    double getHistoricalDataSize() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm