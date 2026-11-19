#include "AdvancedConfigurationManager.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

void testEnhancedConfigValidation() {
    std::cout << "=== Testing Enhanced Configuration Validation ===" << std::endl;
    
    ConfigurationManager configManager;
    
    // Set some parameters
    configManager.setParameter("neuron_count", 500);
    configManager.setParameter("connection_probability", 0.2);
    configManager.setParameter("random_seed", 12345);
    
    // Add validation rules
    configManager.addValidationRule("neuron_count", 
        [](const ConfigValue& v) {
            return std::get<int>(v) > 0 && std::get<int>(v) <= 1000000;
        }, "Neuron count must be between 1 and 1,000,000");
    
    configManager.addValidationRule("connection_probability",
        [](const ConfigValue& v) {
            return std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
        }, "Connection probability must be between 0.0 and 1.0");
    
    // Test validation
    std::vector<std::string> errors, warnings;
    bool valid = configManager.validateConfiguration(errors, warnings);
    
    std::cout << "Configuration valid: " << (valid ? "YES" : "NO") << std::endl;
    if (!errors.empty()) {
        std::cout << "Errors:" << std::endl;
        for (const auto& error : errors) std::cout << "  - " << error << std::endl;
    }
    if (!warnings.empty()) {
        std::cout << "Warnings:" << std::endl;
        for (const auto& warning : warnings) std::cout << "  - " << warning << std::endl;
    }
    
    assert(valid);
    std::cout << "✓ Enhanced configuration validation test passed" << std::endl << std::endl;
}

void testConfigurationExportImport() {
    std::cout << "=== Testing Configuration Export/Import ===" << std::endl;
    
    ConfigurationManager configManager;
    
    // Set up test configuration
    configManager.setParameter("neuron_count", 1000);
    configManager.setParameter("region_count", 3);
    configManager.setParameter("connection_probability", 0.15);
    configManager.setParameter("simulation_timestep", 0.001);
    configManager.setParameter("random_seed", 42);
    configManager.setParameter("log_level", "INFO");
    
    // Export to JSON
    std::cout << "Exporting to JSON..." << std::endl;
    bool jsonSuccess = configManager.exportToJSON("test_config.json");
    assert(jsonSuccess);
    std::cout << "✓ JSON export successful" << std::endl;
    
    // Export to YAML
    std::cout << "Exporting to YAML..." << std::endl;
    bool yamlSuccess = configManager.exportToYAML("test_config.yaml");
    assert(yamlSuccess);
    std::cout << "✓ YAML export successful" << std::endl;
    
    // Create backup
    std::cout << "Creating backup..." << std::endl;
    bool backupSuccess = configManager.createBackup();
    assert(backupSuccess);
    std::cout << "✓ Backup creation successful" << std::endl;
    
    // Get configuration statistics
    std::cout << "Configuration statistics:" << std::endl;
    std::cout << configManager.getConfigurationStats() << std::endl;
    
    std::cout << "✓ Configuration export/import test passed" << std::endl << std::endl;
}

void testConfigurationComparison() {
    std::cout << "=== Testing Configuration Comparison ===" << std::endl;
    
    ConfigurationManager config1, config2;
    
    // Set up different configurations
    config1.setParameter("neuron_count", 500);
    config1.setParameter("connection_probability", 0.1);
    config1.setParameter("simulation_timestep", 0.001);
    
    config2.setParameter("neuron_count", 1000);
    config2.setParameter("connection_probability", 0.2);
    config2.setParameter("simulation_timestep", 0.002);
    config2.setParameter("random_seed", 123);
    
    // Compare configurations
    std::vector<std::string> differences;
    bool similar = config1.compareConfigurations(config2, differences);
    
    std::cout << "Configurations are different: " << (similar ? "YES" : "NO") << std::endl;
    if (!differences.empty()) {
        std::cout << "Differences:" << std::endl;
        for (const auto& diff : differences) std::cout << "  - " << diff << std::endl;
    }
    
    // Get detailed comparison
    std::string detailedReport = config1.getDetailedComparisonReport(config2);
    std::cout << "Detailed comparison report:\n" << detailedReport;
    
    std::cout << "✓ Configuration comparison test passed" << std::endl << std::endl;
}

void testConfigurationOptimization() {
    std::cout << "=== Testing Configuration Optimization ===" << std::endl;
    
    ConfigurationManager configManager;
    
    // Set up a typical configuration
    configManager.setParameter("neuron_count", 2000);
    configManager.setParameter("max_simulation_steps", 50000);
    configManager.setParameter("connection_probability", 0.3);
    
    // Get optimization suggestions
    std::string optimizationReport = configManager.getOptimizationReport();
    std::cout << "Optimization suggestions:\n" << optimizationReport;
    
    // Perform optimization
    std::cout << "Performing optimization..." << std::endl;
    bool optimized = configManager.optimizeConfiguration();
    assert(optimized);
    
    std::cout << "✓ Configuration optimization test passed" << std::endl << std::endl;
}

void testConfigurationTemplates() {
    std::cout << "=== Testing Configuration Templates ===" << std::endl;
    
    ConfigurationTemplate template1("Small Brain");
    ConfigurationTemplate template2("Large Brain");
    
    // Create parameter sets
    std::unordered_map<std::string, ConfigValue> smallBrainParams = {
        {"neuron_count", 500},
        {"region_count", 2},
        {"connection_probability", 0.1}
    };
    
    std::unordered_map<std::string, ConfigValue> largeBrainParams = {
        {"neuron_count", 10000},
        {"region_count", 5},
        {"connection_probability", 0.2},
        {"max_simulation_steps", 100000}
    };
    
    // Add parameter sets
    template1.addParameterSet("small", smallBrainParams);
    template2.addParameterSet("large", largeBrainParams);
    
    // List templates
    std::cout << "Available templates:" << std::endl;
    std::vector<std::string> templates = template1.listTemplates();
    for (const auto& t : templates) std::cout << "  - " << t << std::endl;
    
    // Get parameter sets
    auto smallParams = template1.getParameterSet("small");
    std::cout << "Small brain parameters:" << std::endl;
    for (const auto& pair : smallParams) std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    
    std::cout << "✓ Configuration templates test passed" << std::endl << std::endl;
}

void testDiffTool() {
    std::cout << "=== Testing Configuration Diff Tool ===" << std::endl;
    
    ConfigurationManager config1, config2;
    
    // Set up different configurations
    config1.setParameter("neuron_count", 500);
    config1.setParameter("random_seed", 42);
    
    config2.setParameter("neuron_count", 1000);
    config2.setParameter("random_seed", 42);
    config2.setParameter("environment_name", "GridWorld");
    
    // Use diff tool
    ConfigurationDiffTool diffTool;
    std::string diffReport = diffTool.generateDiff(config1, config2);
    std::cout << "Configuration differences:\n" << diffReport;
    
    // Get detailed diff
    std::string detailedDiff = diffTool.showDiffHumanReadable(config1, config2);
    std::cout << "Detailed diff:\n" << detailedDiff;
    
    std::cout << "✓ Configuration diff tool test passed" << std::endl << std::endl;
}

void testPerformanceAnalysis() {
    std::cout << "=== Testing Performance Analysis ===" << std::endl;
    
    ConfigurationManager configManager;
    
    // Set up a typical configuration
    configManager.setParameter("neuron_count", 1500);
    configManager.setParameter("connection_probability", 0.25);
    configManager.setParameter("max_simulation_steps", 20000);
    
    // Get performance analysis
    ConfigurationPerformanceAnalyzer analyzer;
    std::string performanceReport = analyzer.getPerformanceReport(configManager);
    std::cout << "Performance analysis:\n" << performanceReport;
    
    // Get performance recommendations
    std::vector<std::string> recommendations = analyzer.getPerformanceRecommendations(configManager);
    std::cout << "Performance recommendations:" << std::endl;
    for (const auto& rec : recommendations) std::cout << "  - " << rec << std::endl;
    
    std::cout << "✓ Performance analysis test passed" << std::endl << std::endl;
}

void testVisualizationConfig() {
    std::cout << "=== Testing Visualization Configuration ===" << std::endl;
    
    AdvancedVisualizationConfig vizConfig;
    
    // Configure visualization
    std::unordered_map<std::string, ConfigValue> vizSettings = {
        {"update_rate", 30.0},
        {"network_viz", true},
        {"spike_raster", true}
    };
    
    vizConfig.configureVisualization("default", vizSettings);
    
    // List available visualization types
    std::vector<std::string> vizTypes = vizConfig.listAvailableVisualizationTypes();
    std::cout << "Available visualization types:" << std::endl;
    for (const auto& type : vizTypes) std::cout << "  - " << type << std::endl;
    
    // Configure real-time visualization
    vizConfig.enableRealTimeVisualization(60.0); // 60 FPS
    
    std::cout << "✓ Visualization configuration test passed" << std::endl << std::endl;
}

void testExportImportAdvanced() {
    std::cout << "=== Testing Advanced Export/Import ===" << std::endl;
    
    ConfigurationManager configManager;
    
    // Set up test configuration
    configManager.setParameter("neuron_count", 800);
    configManager.setParameter("region_count", 4);
    configManager.setParameter("connection_probability", 0.18);
    configManager.setParameter("random_seed", 12345);
    
    // Export with metadata
    std::cout << "Exporting with metadata..." << std::endl;
    bool exportSuccess = configManager.exportWithMetadata(
        "advanced_config.json",
        "Test configuration for demonstration",
        "NLM Team",
        {"performance", "testing", "research"}
    );
    assert(exportSuccess);
    std::cout << "✓ Advanced export successful" << std::endl;
    
    // Create archive export
    std::cout << "Creating archive export..." << std::endl;
    bool archiveSuccess = configManager.exportAsArchive(
        "config_archive.zip",
        "zip",
        true,  // include logs
        true   // include scripts
    );
    assert(archiveSuccess);
    std::cout << "✓ Archive export successful" << std::endl;
    
    std::cout << "✓ Advanced export/import test passed" << std::endl << std::endl;
}

int main() {
    std::cout << "=== Enhanced Configuration Management Test Suite ===" << std::endl << std::endl;
    
    try {
        testEnhancedConfigValidation();
        testConfigurationExportImport();
        testConfigurationComparison();
        testConfigurationOptimization();
        testConfigurationTemplates();
        testDiffTool();
        testPerformanceAnalysis();
        testVisualizationConfig();
        testExportImportAdvanced();
        
        std::cout << "=== All Tests Passed! ===" << std::endl;
        std::cout << "Enhanced configuration management is working correctly." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}