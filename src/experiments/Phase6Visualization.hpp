#pragma once

/**
 * Phase6Visualization - Enhanced visualization for Phase 6 integration demo
 * 
 * Provides comprehensive visualization of Phase 6 integration,
 * including system integration status, performance metrics, and debugging information.
 */

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "Phase6IntegrationResult.hpp"
#include "Phase6Config.hpp"

namespace nlm {

/**
 * Enhanced visualization for Phase 6 integration
 */
class Phase6Visualization {
public:
    Phase6Visualization();
    ~Phase6Visualization();
    
    /**
     * Initialize visualization with given dimensions
     */
    bool initialize(int width, int height);
    
    /**
     * Render the complete Phase 6 integration visualization
     */
    void render(const Phase6IntegrationResult& result, const Phase6Config& config);
    
    /**
     * Render system integration status
     */
    void renderSystemStatus(const Phase6IntegrationResult& result);
    
    /**
     * Render performance metrics
     */
    void renderPerformanceMetrics(const Phase6IntegrationResult& result);
    
    /**
     * Render integration verification results
     */
    void renderIntegrationResults(const Phase6IntegrationResult& result);
    
    /**
     * Render debugging information
     */
    void renderDebugInfo(const Phase6IntegrationResult& result);
    
    /**
     * Set the current time for display
     */
    void setCurrentTime(const std::string& time) {
        currentTime_ = time;
    }
    
    /**
     * Generate ASCII art representation of system status
     */
    std::string generateASCIIArt(const Phase6IntegrationResult& result);
    
    /**
     * Generate HTML report for Phase 6 integration
     */
    std::string generateHTMLReport(const Phase6IntegrationResult& result, const Phase6Config& config);
    
    /**
     * Export metrics to CSV format
     */
    std::string exportMetricsCSV(const Phase6IntegrationResult& result);
    
    /**
     * Check if visualization is active
     */
    bool isActive() const;
    
    /**
     * Get current timestamp
     */
    std::string getCurrentTimeString() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string currentTime_;
    
    // Helper methods for rendering
    void renderHeader();
    void renderSystemBars(const Phase6IntegrationResult& result);
    void renderPerformanceChart(const Phase6IntegrationResult& result);
    void renderIntegrationGauges(const Phase6IntegrationResult& result);
    std::string formatMetric(const std::string& name, float value, const std::string& unit = "");
    std::string getStatusColor(bool connected);
    std::string getStatusSymbol(bool connected);
};

} // namespace nlm
