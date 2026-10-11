#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <functional>
#include <map>
#include <iostream>
#include <fstream>

namespace nlm {

/**
 * Integration visualization system for Phase 6
 * 
 * Provides comprehensive visualization of system integration,
 * dependencies, flow, and health monitoring.
 */
class IntegrationVisualizer {
public:
    /**
     * Structure representing integration components
     */
    struct IntegrationComponent {
        std::string id;
        std::string name;
        std::string type; // "system", "module", "submodule"
        std::string status; // "active", "inactive", "error", "warning"
        double health; // 0.0 to 1.0
        std::vector<std::string> dependencies;
        std::vector<std::string> dependents;
        double lastUpdateTime;
        std::string lastUpdateMessage;
        
        IntegrationComponent() : health(0.0), lastUpdateTime(0.0) {}
    };
    
    /**
     * Structure representing system integration status
     */
    struct IntegrationStatus {
        double timestamp;
        std::vector<IntegrationComponent> components;
        std::vector<std::pair<std::string, std::string>> connections; // from->to
        std::map<std::string, double> metrics;
        std::string overallStatus; // "healthy", "degraded", "failed"
        
        IntegrationStatus() : timestamp(0.0), overallStatus("unknown") {}
    };
    
    /**
     * Graph visualization options
     */
    struct GraphOptions {
        bool showLabels; // Show component names
        bool showHealth; // Show health indicators
        bool showConnections; // Show dependency arrows
        std::string layout; // "tree", "radial", "force-directed"
        double nodeSize; // Base node size
        double lineWidth; // Connection line width
        std::string style; // "minimal", "detailed", "colorful"
    };
    
    /**
     * System status dashboard options
     */
    struct DashboardOptions {
        bool showSystemHealth; // Overall system health
        bool showComponentStatus; // Individual component status
        bool showMetrics; // Show metrics
        bool showRecentEvents; // Show recent events
        double refreshInterval; // Seconds
        std::vector<std::string> metricsToShow;
        std::string theme; // "light", "dark", "high-contrast"
    };
    
    /**
     * Integration health monitor options
     */
    struct HealthMonitorOptions {
        bool showAlertThresholds; // Show alert thresholds
        bool showRecoveryTime; // Show estimated recovery time
        bool showTrend; // Show health trends
        std::vector<double> alertThresholds; // Health levels for alerts
        std::string alertColor; // Color for alerts
    };
    
public:
    IntegrationVisualizer();
    ~IntegrationVisualizer();
    
    /**
     * Initialize the visualizer
     */
    bool initialize();
    
    /**
     * Update visualization data from brain systems
     */
    void update(std::shared_ptr<class Brain> brain, std::shared_ptr<class Config> config);
    
    /**
     * Render integration graph
     */
    void renderIntegrationGraph(const GraphOptions& options) const;
    
    /**
     * Render system status dashboard
     */
    void renderDashboard(const DashboardOptions& options) const;
    
    /**
     * Render health monitor
     */
    void renderHealthMonitor(const HealthMonitorOptions& options) const;
    
    /**
     * Render dependency visualization
     */
    void renderDependencyGraph() const;
    
    /**
     * Render flow diagram
     */
    void renderFlowDiagram() const;
    
    /**
     * Get current integration status
     */
    IntegrationStatus getIntegrationStatus() const { return currentStatus_; }
    
    /**
     * Set update callback
     */
    void setUpdateCallback(std::function<void(const IntegrationStatus&)> callback) {
        updateCallback_ = callback;
    }
    
    /**
     * Export integration graph to file
     */
    bool exportGraph(const std::string& filename, const std::string& format = "dot") const;
    
    /**
     * Generate HTML visualization
     */
    bool generateHTMLReport(const std::string& filename) const;
    
    /**
     * Generate JSON report
     */
    bool generateJSONReport(const std::string& filename) const;
    
    /**
     * Save visualization state
     */
    bool saveState(const std::string& filename) const;
    
    /**
     * Load visualization state
     */
    bool loadState(const std::string& filename);
    
    /**
     * Generate HTML visualization
     */
    void generateHTML(const std::string& filename, const IntegrationStatus& status) const;
    
    /**
     * Generate DOT format graph
     */
    void generateDOT(const std::string& filename, const IntegrationStatus& status) const;
    
    /**
     * Generate JSON report
     */
    void generateJSON(const std::string& filename, const IntegrationStatus& status) const;
    
    /**
     * Is visualization active
     */
    bool isActive() const { return active_; }
    
    /**
     * Set update rate
     */
    void setUpdateRate(double hz) { updateRate_ = hz; }
    
    /**
     * Get update rate
     */
    double getUpdateRate() const { return updateRate_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    IntegrationStatus currentStatus_;
    bool active_;
    double updateRate_;
    std::function<void(const IntegrationStatus&)> updateCallback_;
    
    // Helper methods
    void collectSystemComponents(std::shared_ptr<class Brain> brain, std::shared_ptr<class Config> config);
    void analyzeDependencies();
    void updateComponentHealth();
    void detectIntegrationIssues();
};

} // namespace nlm
