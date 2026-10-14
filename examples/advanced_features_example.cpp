// Advanced Features Example
// Demonstrates the use of advanced features in NLM

#include "core/AdvancedFeatures/AdvancedFeatures.hpp"
#include "brain/Brain.hpp"
#include <iostream>

void printAdvancedFeaturesExample() {
    std::cout << "=== Advanced Features Example ===" << std::endl;
    std::cout << "This example demonstrates the use of advanced features in NLM." << std::endl;
    std::cout << std::endl;
    
    // Get advanced configuration manager
    auto& configManager = nlm::AdvancedFeatures::getAdvancedConfigManager();
    
    // Get advanced performance monitor
    auto& performanceMonitor = nlm::AdvancedFeatures::getAdvancedPerformanceMonitor();
    
    // Initialize advanced features
    nlm::AdvancedFeatures::initializeAll();
    
    // Enable some advanced features
    std::vector<std::string> features = {"performance_monitoring", "visualization", "export"};
    nlm::AdvancedFeatures::enableAdvancedFeatures(features);
    
    // Create a basic brain configuration
    nlm::Config config;
    config.set("neuron_count", 1000, nlm::ConfigSource::Default);
    config.set("region_count", 2, nlm::ConfigSource::Default);
    config.set("connection_probability", 0.15, nlm::ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    
    // Set advanced configuration
    nlm::AdvancedConfig advancedConfig;
    advancedConfig.simulationMode = nlm::SimulationMode::Production;
    advancedConfig.performanceLevel = nlm::PerformanceLevel::Optimized;
    advancedConfig.simulationSpeed = 1.0;
    advancedConfig.enableRealTimeVisualization = true;
    advancedConfig.enablePerformanceMonitoring = true;
    
    configManager.setAdvancedConfig(advancedConfig);
    
    // Set advanced visualization configuration
    nlm::AdvancedVisualization visualizationConfig;
    visualizationConfig.mode = nlm::VisualizationMode::Advanced;
    visualizationConfig.enable3D = false;
    visualizationConfig.enableAnimation = true;
    visualizationConfig.animationSpeed = 1.0;
    visualizationConfig.showNeurons = true;
    visualizationConfig.showSynapses = true;
    visualizationConfig.showWeights = true;
    visualizationConfig.showActivity = true;
    
    configManager.setVisualizationConfig(visualizationConfig);
    
    // Set advanced export configuration
    nlm::AdvancedExport exportConfig;
    exportConfig.format = nlm::ExportFormat::JSON;
    exportConfig.compress = true;
    exportConfig.includeMetadata = true;
    exportConfig.includeDebugInfo = false;
    
    configManager.setExportConfig(exportConfig);
    
    // Create advanced simulation
    auto brain = nlm::AdvancedFeatures::createAdvancedSimulation(config);
    
    if (brain) {
        std::cout << "Created advanced simulation successfully" << std::endl;
        std::cout << "Brain statistics:" << std::endl;
        brain->logStatus();
    } else {
        std::cout << "Failed to create advanced simulation" << std::endl;
        return;
    }
    
    // Start performance monitoring
    performanceMonitor.startMonitoring();
    
    // Run simulation for a few steps
    for (size_t i = 0; i < 10; ++i) {
        brain->step(i, i * 0.001);
        performanceMonitor.updateMetrics();
    }
    
    // Stop performance monitoring
    performanceMonitor.stopMonitoring();
    
    // Get performance metrics
    auto metrics = performanceMonitor.getMetrics();
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "Wall Clock Time: " << metrics.wallClockTime << "s" << std::endl;
    std::cout << "CPU Time: " << metrics.cpuTime << "s" << std::endl;
    std::cout << "Memory Usage: " << metrics.memoryUsage << " bytes" << std::endl;
    std::cout << "Total Steps: " << metrics.totalSteps << std::endl;
    std::cout << "Average Step Time: " << metrics.averageStepTime << "s" << std::endl;
    std::cout << "Efficiency Score: " << metrics.efficiencyScore << std::endl;
    
    // Get performance analysis
    std::string analysis = performanceMonitor.getPerformanceAnalysis();
    std::cout << "\nPerformance Analysis:" << std::endl;
    std::cout << analysis << std::endl;
    
    // Get performance alerts
    auto alerts = performanceMonitor.getPerformanceAlerts();
    if (!alerts.empty()) {
        std::cout << "\nPerformance Alerts:" << std::endl;
        for (const auto& alert : alerts) {
            std::cout << "  - " << alert << std::endl;
        }
    }
    
    // Show advanced configuration summary
    std::cout << "\nAdvanced Configuration Summary:" << std::endl;
    std::cout << configManager.getSummary() << std::endl;
    
    // Cleanup
    nlm::AdvancedFeatures::cleanupAll();
    
    std::cout << "\n=== Advanced Features Example Complete ===" << std::endl;
}

int main() {
    printAdvancedFeaturesExample();
    return 0;
}
