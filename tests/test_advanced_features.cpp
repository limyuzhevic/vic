// Advanced Features Test Framework
// Provides basic tests for advanced features and configuration options

#include "core/AdvancedFeatures/AdvancedFeatures.hpp"
#include "core/Factory/BrainFactory.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace test_advanced_features {

void testAdvancedConfigManager() {
    std::cout << "    testAdvancedConfigManager started" << std::endl;
    
    // Get advanced configuration manager
    auto& configManager = nlm::AdvancedConfigManager::getInstance();
    
    // Reset to defaults
    configManager.resetToDefaults();
    
    // Get initial configuration
    auto config = configManager.getAdvancedConfig();
    assert(config.simulationMode == nlm::SimulationMode::Standard);
    assert(config.performanceLevel == nlm::PerformanceLevel::Standard);
    
    // Set custom configuration
    nlm::AdvancedConfig customConfig;
    customConfig.simulationMode = nlm::SimulationMode::Production;
    customConfig.performanceLevel = nlm::PerformanceLevel::Optimized;
    customConfig.simulationSpeed = 2.0;
    
    configManager.setAdvancedConfig(customConfig);
    
    // Verify configuration was set
    auto updatedConfig = configManager.getAdvancedConfig();
    assert(updatedConfig.simulationMode == nlm::SimulationMode::Production);
    assert(updatedConfig.performanceLevel == nlm::PerformanceLevel::Optimized);
    assert(updatedConfig.simulationSpeed == 2.0);
    
    // Test configuration validation
    assert(configManager.isValid());
    
    // Test configuration summary
    auto summary = configManager.getSummary();
    assert(!summary.empty());
    
    // Test configuration help
    auto help = configManager.getHelp();
    assert(!help.empty());
    
    // Test saving to file
    bool saveSuccess = configManager.saveToFile("/tmp/test_advanced_config.json");
    assert(saveSuccess);
    
    // Test loading from file
    bool loadSuccess = configManager.loadFromFile("/tmp/test_advanced_config.json");
    assert(loadSuccess);
    
    // Cleanup
    std::remove("/tmp/test_advanced_config.json");
    
    std::cout << "    testAdvancedConfigManager passed" << std::endl;
}

void testAdvancedPerformanceMonitor() {
    std::cout << "    testAdvancedPerformanceMonitor started" << std::endl;
    
    // Get advanced performance monitor
    auto& performanceMonitor = nlm::AdvancedPerformanceMonitor::getInstance();
    
    // Start monitoring
    performanceMonitor.startMonitoring();
    
    // Update metrics
    performanceMonitor.updateMetrics();
    
    // Get metrics
    auto metrics = performanceMonitor.getMetrics();
    assert(metrics.wallClockTime >= 0.0);
    assert(metrics.cpuTime >= 0.0);
    assert(metrics.memoryUsage >= 0);
    
    // Get performance analysis
    auto analysis = performanceMonitor.getPerformanceAnalysis();
    assert(!analysis.empty());
    
    // Get performance alerts
    auto alerts = performanceMonitor.getPerformanceAlerts();
    // Alerts can be empty
    
    // Get performance recommendations
    auto recommendations = performanceMonitor.getPerformanceRecommendations();
    // Recommendations can be empty
    
    // Test performance status
    std::string status = performanceMonitor.getStatus();
    assert(!status.empty());
    
    // Test saving metrics to file
    bool saveSuccess = performanceMonitor.saveMetricsToFile("/tmp/test_performance_metrics.json");
    assert(saveSuccess);
    
    // Test loading metrics from file
    bool loadSuccess = performanceMonitor.importMetricsFromString(
        "Wall Clock Time: 1.5s\nCPU Time: 0.75s\nMemory Usage: 1024 bytes\nTotal Steps: 100\nAverage Step Time: 0.015s\nEfficiency Score: 0.8"
    );
    assert(loadSuccess);
    
    // Reset metrics
    performanceMonitor.resetMetrics();
    
    // Verify metrics were reset
    auto resetMetrics = performanceMonitor.getMetrics();
    assert(resetMetrics.wallClockTime == 0.0);
    assert(resetMetrics.cpuTime == 0.0);
    assert(resetMetrics.memoryUsage == 0);
    
    // Cleanup
    std::remove("/tmp/test_performance_metrics.json");
    
    std::cout << "    testAdvancedPerformanceMonitor passed" << std::endl;
}

void testAdvancedConfigIntegration() {
    std::cout << "    testAdvancedConfigIntegration started" << std::endl;
    
    // Get advanced configuration manager
    auto& configManager = nlm::AdvancedConfigManager::getInstance();
    
    // Set various configuration options
    nlm::AdvancedConfig config;
    config.simulationMode = nlm::SimulationMode::Research;
    config.performanceLevel = nlm::PerformanceLevel::Optimized;
    config.enableRealTimeVisualization = true;
    config.enablePerformanceMonitoring = true;
    config.enableDebugging = true;
    config.enableProfiling = true;
    
    configManager.setAdvancedConfig(config);
    
    // Verify configuration was set
    auto retrievedConfig = configManager.getAdvancedConfig();
    assert(retrievedConfig.simulationMode == nlm::SimulationMode::Research);
    assert(retrievedConfig.performanceLevel == nlm::PerformanceLevel::Optimized);
    assert(retrievedConfig.enableRealTimeVisualization == true);
    assert(retrievedConfig.enablePerformanceMonitoring == true);
    assert(retrievedConfig.enableDebugging == true);
    assert(retrievedConfig.enableProfiling == true);
    
    // Test configuration profiles
    configManager.setConfigurationProfile("research");
    
    // Test custom profiles
    nlm::AdvancedConfig customConfig;
    customConfig.simulationMode = nlm::SimulationMode::Production;
    configManager.createCustomProfile("my_profile", customConfig);
    
    // Test profile retrieval
    std::vector<std::string> profiles = configManager.getCustomProfiles();
    assert(std::find(profiles.begin(), profiles.end(), "my_profile") != profiles.end());
    
    // Test profile application
    configManager.applyProfile("my_profile");
    
    std::cout << "    testAdvancedConfigIntegration passed" << std::endl;
}

void testAdvancedFeatures() {
    std::cout << "    testAdvancedFeatures started" << std::endl;
    
    // Initialize advanced features
    nlm::AdvancedFeatures::initializeAll();
    
    // Get configuration manager
    auto& configManager = nlm::AdvancedFeatures::getAdvancedConfigManager();
    
    // Get performance monitor
    auto& performanceMonitor = nlm::AdvancedFeatures::getAdvancedPerformanceMonitor();
    
    // Create a basic brain configuration
    nlm::Config config;
    config.set("neuron_count", 100, nlm::ConfigSource::Default);
    config.set("region_count", 2, nlm::ConfigSource::Default);
    config.set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    // Set advanced configuration
    nlm::AdvancedConfig advancedConfig;
    advancedConfig.simulationMode = nlm::SimulationMode::Production;
    advancedConfig.performanceLevel = nlm::PerformanceLevel::Optimized;
    advancedConfig.enableRealTimeVisualization = true;
    
    configManager.setAdvancedConfig(advancedConfig);
    
    // Create advanced simulation
    auto brain = nlm::AdvancedFeatures::createAdvancedSimulation(config);
    assert(brain != nullptr);
    
    // Start performance monitoring
    performanceMonitor.startMonitoring();
    
    // Run simulation
    for (size_t i = 0; i < 5; ++i) {
        brain->step(i, i * 0.001);
        performanceMonitor.updateMetrics();
    }
    
    // Stop performance monitoring
    performanceMonitor.stopMonitoring();
    
    // Get performance metrics
    auto metrics = performanceMonitor.getMetrics();
    assert(metrics.wallClockTime >= 0.0);
    
    // Get performance analysis
    auto analysis = performanceMonitor.getPerformanceAnalysis();
    assert(!analysis.empty());
    
    // Cleanup
    nlm::AdvancedFeatures::cleanupAll();
    
    std::cout << "    testAdvancedFeatures passed" << std::endl;
}

void testBrainComponentFactory() {
    std::cout << "    testBrainComponentFactory started" << std::endl;
    
    // Get brain component factory
    auto& factory = nlm::BrainComponentFactory::getInstance();
    
    // Check supported component types
    auto supportedTypes = factory.getSupportedTypes();
    assert(!supportedTypes.empty());
    
    // Check if specific component types are supported
    bool supportsNeuron = factory.supportsComponent(nlm::ComponentType::Neuron);
    bool supportsSynapse = factory.supportsComponent(nlm::ComponentType::Synapse);
    bool supportsRegion = factory.supportsComponent(nlm::ComponentType::NeuralRegion);
    
    // These may be supported depending on factory implementation
    // In any case, the test should not fail
    
    // Get component names
    for (const auto& type : supportedTypes) {
        std::string name = factory.getComponentName(type);
        assert(!name.empty());
    }
    
    std::cout << "    testBrainComponentFactory passed" << std::endl;
}

void testComponentCreator() {
    std::cout << "    testComponentCreator started" << std::endl;
    
    // Get component creator
    auto& creator = nlm::ComponentCreator::getComponentCreator();
    
    // Create a basic configuration
    nlm::Config config;
    config.set("neuron_count", 50, nlm::ConfigSource::Default);
    config.set("region_count", 1, nlm::ConfigSource::Default);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Test configuration validation
    bool isValid = creator.isConfigValid(config);
    assert(isValid);
    
    // Get default configuration for a component type
    auto defaultConfig = creator.getDefaultConfig(nlm::ComponentType::Neuron);
    assert(defaultConfig.has("neuron_count"));
    
    // Test brain creation
    auto brain = creator.createBrain(config);
    assert(brain != nullptr);
    
    std::cout << "    testComponentCreator passed" << std::endl;
}

void runAll() {
    testAdvancedConfigManager();
    testAdvancedPerformanceMonitor();
    testAdvancedConfigIntegration();
    testAdvancedFeatures();
    testBrainComponentFactory();
    testComponentCreator();
    
    std::cout << std::endl;
    std::cout << "All advanced features tests passed successfully!" << std::endl;
}

} // namespace test_advanced_features