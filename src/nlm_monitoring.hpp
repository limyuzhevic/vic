#pragma once

// Main monitoring and visualization system header
// This file includes all the components for real-time monitoring, visualization,
// performance tracking, and export functionality

// Core components
#include "src/performance/MetricsRecorder.hpp"
#include "src/performance/MonitoringManager.hpp"
#include "src/visualization/VisualizationEngine.hpp"
#include "src/export/ExportManager.hpp"
#include "src/web/WebInterface.hpp"

// Integration utilities
#include "src/integration/BrainMonitor.hpp"
#include "src/integration/SystemMonitor.hpp"

namespace nlm {

// Initialize the complete monitoring and visualization system
inline bool initializeMonitoringSystem(const MonitoringConfig& monitoringConfig = MonitoringConfig(),
                                        const VisualizationConfig& visualizationConfig = VisualizationConfig(),
                                        const ExportConfig& exportConfig = ExportConfig(),
                                        const WebInterfaceConfig& webConfig = WebInterfaceConfig()) {
    bool success = true;
    
    // Initialize MetricsRecorder
    MetricsRecorder::getInstance().configure({});
    
    // Initialize MonitoringManager
    if (!MonitoringManager::getInstance().initialize(monitoringConfig)) {
        NLM_LOG_ERROR("Failed to initialize MonitoringManager");
        success = false;
    }
    
    // Initialize VisualizationEngine
    if (!VisualizationEngine::getInstance().initialize(visualizationConfig)) {
        NLM_LOG_ERROR("Failed to initialize VisualizationEngine");
        success = false;
    }
    
    // Initialize ExportManager
    if (!ExportManager::getInstance().initialize(exportConfig)) {
        NLM_LOG_ERROR("Failed to initialize ExportManager");
        success = false;
    }
    
    // Initialize WebInterface (only if enabled)
    if (webConfig.enableWebInterface) {
        if (!WebInterface::getInstance().initialize(webConfig)) {
            NLM_LOG_ERROR("Failed to initialize WebInterface");
            success = false;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Monitoring and Visualization system initialized successfully");
    }
    
    return success;
}

// Start the monitoring system
inline void startMonitoringSystem() {
    MonitoringManager::getInstance().start();
    VisualizationEngine::getInstance().start();
    WebInterface::getInstance().start();
    NLM_LOG_INFO("Monitoring and Visualization system started");
}

// Stop the monitoring system
inline void stopMonitoringSystem() {
    MonitoringManager::getInstance().stop();
    VisualizationEngine::getInstance().stop();
    WebInterface::getInstance().stop();
    NLM_LOG_INFO("Monitoring and Visualization system stopped");
}

// Update the system with Brain data
inline void updateMonitoringSystem(const Brain& brain, Timestamp currentTime, SimulationStep currentStep) {
    // Update MonitoringManager
    MonitoringManager::getInstance().update(brain, currentTime, currentStep);
    
    // Update VisualizationEngine
    VisualizationEngine::getInstance().update(brain, currentTime, currentStep);
    
    // Record metrics
    MetricsRecorder::getInstance().recordNeuralMetrics(brain, currentTime, currentStep);
    
    // Update WebInterface (if running)
    if (WebInterface::getInstance().isRunning()) {
        WebInterface::updateGlobal(brain, currentTime, currentStep);
    }
}

// Export data
inline bool exportData(const std::string& filename, const std::string& format) {
    return ExportManager::getInstance().exportGlobal(filename, format);
}

// Get system status
inline std::string getSystemStatus() {
    std::ostringstream oss;
    oss << "=== NLM Monitoring and Visualization System ===\n";
    oss << "MonitoringManager: " << (MonitoringManager::getInstance().isRunning() ? "Running" : "Stopped") << "\n";
    oss << "VisualizationEngine: " << (VisualizationEngine::getInstance().isRunning() ? "Running" : "Stopped") << "\n";
    oss << "ExportManager: " << (ExportManager::getInstance().isRunning() ? "Running" : "Stopped") << "\n";
    oss << "WebInterface: " << (WebInterface::getInstance().isRunning() ? "Running" : "Stopped") << "\n";
    
    oss << "\n=== Metrics ===\n";
    oss << MonitoringManager::getInstance().getStats().toString() << "\n";
    
    oss << "\n=== MetricsRecorder Stats ===\n";
    oss << MetricsRecorder::getInstance().summary() << "\n";
    
    return oss.str();
}

// Configure monitoring parameters
inline void configureMonitoring(const MonitoringConfig& config) {
    MonitoringManager::getInstance().configure(config);
}

// Configure visualization parameters
inline void configureVisualization(const VisualizationConfig& config) {
    VisualizationEngine::getInstance().configure(config);
}

// Configure export parameters
inline void configureExport(const ExportConfig& config) {
    ExportManager::getInstance().configure(config);
}

// Configure web interface parameters
inline void configureWebInterface(const WebInterfaceConfig& config) {
    WebInterface::getInstance().configure(config);
}

} // namespace nlm
