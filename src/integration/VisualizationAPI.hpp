#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "Brain.hpp"
#include "visualization/VisualizationCore.hpp"
#include "debugging/DebugConsole.hpp"
#include "analysis/DataAnalyzer.hpp"
#include "profiling/PerformanceProfiler.hpp"

namespace nlm {

// Visualization and debugging API
class VisualizationAPI {
public:
    VisualizationAPI();
    ~VisualizationAPI();

    // Initialize API
    bool initialize();

    // Close API
    void close();

    // Check if API is active
    bool isActive() const;

    // Get visualization core
    std::shared_ptr<VisualizationCore> getVisualizationCore() const;

    // Get debug console
    std::shared_ptr<DebugConsole> getDebugConsole() const;

    // Get data analyzer
    std::shared_ptr<DataAnalyzer> getDataAnalyzer() const;

    // Get performance profiler
    std::shared_ptr<PerformanceProfiler> getPerformanceProfiler() const;

    // Get brain
    std::shared_ptr<Brain> getBrain() const;

    // Set brain
    void setBrain(std::shared_ptr<Brain> brain);

    // Start real-time visualization
    void startRealTimeVisualization(double fps = 60.0);

    // Stop real-time visualization
    void stopRealTimeVisualization();

    // Enable/disable real-time visualization
    void setRealTimeVisualizationEnabled(bool enabled);
    bool isRealTimeVisualizationEnabled() const;

    // Start real-time monitoring
    void startRealTimeMonitoring();

    // Stop real-time monitoring
    void stopRealTimeMonitoring();

    // Enable/disable real-time monitoring
    void setRealTimeMonitoringEnabled(bool enabled);
    bool isRealTimeMonitoringEnabled() const;

    // Update all systems
    void update(double deltaTime = 0.0); // 0.0 for auto-detected delta time

    // Render visualizations
    void render();

    // Process debug input
    bool processDebugInput(const std::string& command);

    // Export all data
    bool exportAllData(const std::string& directory, const std::vector<std::string>& formats = {"csv", "json", "hdf5"});

    // Import data
    bool importData(const std::string& directory, const std::string& format = "auto");

    // Get status
    std::string getStatus() const;

    // Get performance statistics
    std::string getPerformanceStats() const;

    // Get system information
    std::string getSystemInfo() const;

    // Enable visualization component
    void enableVisualizationComponent(const std::string& componentName);

    // Disable visualization component
    void disableVisualizationComponent(const std::string& componentName);

    // Check if visualization component is enabled
    bool isVisualizationComponentEnabled(const std::string& componentName) const;

    // Register custom visualization
    bool registerCustomVisualization(const std::string& name, 
                                      std::function<void(const Brain&)> visualizationFunc);

    // Unregister custom visualization
    bool unregisterCustomVisualization(const std::string& name);

    // Get custom visualizations
    std::vector<std::string> getCustomVisualizations() const;

    // Set update rate for components
    void setUpdateRate(const std::string& componentName, double hz);

    // Get update rate for component
    double getUpdateRate(const std::string& componentName) const;

    // Check if component has data
    bool hasComponentData(const std::string& componentName) const;

    // Get component data
    std::string getComponentData(const std::string& componentName) const;

    // Set configuration
    void setConfig(const std::string& key, const std::string& value);

    // Get configuration
    std::string getConfig(const std::string& key) const;

    // Save configuration
    bool saveConfig(const std::string& filename) const;

    // Load configuration
    bool loadConfig(const std::string& filename);

    // Reset to default configuration
    void resetConfig();

    // Check for errors
    bool hasErrors() const;

    // Get error messages
    std::vector<std::string> getErrorMessages() const;

    // Clear errors
    void clearErrors();

    // Get warning messages
    std::vector<std::string> getWarningMessages() const;

    // Clear warnings
    void clearWarnings();

    // Get information messages
    std::vector<std::string> getInfoMessages() const;

    // Clear information messages
    void clearInfoMessages();

    // Log message
    void logMessage(const std::string& message, const std::string& level = "info");

    // Set log level
    void setLogLevel(const std::string& level);

    // Get log level
    std::string getLogLevel() const;

    // Get available metrics
    std::vector<std::string> getAvailableMetrics() const;

    // Get available visualizations
    std::vector<std::string> getAvailableVisualizations() const;

    // Get available debug commands
    std::vector<std::string> getAvailableDebugCommands() const;

    // Get available data formats
    std::vector<std::string> getAvailableDataFormats() const;

    // Get available export formats
    std::vector<std::string> getAvailableExportFormats() const;

    // Get system health
    std::string getSystemHealth() const;

    // Get resource usage
    struct ResourceUsage {
        double cpuUsage;
        size_t memoryUsage;
        double networkUsage;
        double diskUsage;
        
        ResourceUsage() : cpuUsage(0.0), memoryUsage(0), networkUsage(0.0), diskUsage(0.0) {}
    };

    ResourceUsage getResourceUsage() const;

    // Get uptime
    double getUptime() const;

    // Get version
    std::string getVersion() const;

    // Get build information
    std::string getBuildInfo() const;

    // Get compilation information
    std::string getCompilationInfo() const;

    // Get license information
    std::string getLicenseInfo() const;

    // Get copyright information
    std::string getCopyrightInfo() const;

    // Get help
    std::string getHelp() const;

    // Get about information
    std::string getAbout() const;

    // Exit application
    void exit();

    // Restart application
    void restart();

    // Check if application should exit
    bool shouldExit() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm