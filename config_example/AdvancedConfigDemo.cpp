// Advanced Configuration Example
// Demonstrates the new advanced configuration system for NLM
// 
// This program shows:
// - Basic configuration usage (backward compatible)
// - Advanced configuration features
// - Configuration validation
// - Configuration export/import
// - Configuration presets
// - Configuration management
// 
// This helps users understand how to:
// - Use the simplified configuration interface
// - Access advanced features when needed
// - Validate and manage configurations
// - Create and use configuration presets
// - Export and import configurations

#include "ConfigManager.hpp"
#include "AdvancedConfig.hpp"
#include <iostream>
#include <fstream>
#include <vector>

void demonstrateBasicConfiguration() {
    std::cout << "=== Basic Configuration Demo ===" << std::endl;
    
    nlm::ConfigManager config;
    
    // Initialize with default configuration
    config.initialize("configs/default.cfg");
    
    // Set some basic configuration values
    config.set("neuron_count", static_cast<size_t>(1000));
    config.set("region_count", static_cast<size_t>(2));
    config.set("connection_probability", 0.15f);
    
    // Get configuration values with defaults
    size_t neuronCount = config.getOr("neuron_count", 500);
    size_t regionCount = config.getOr("region_count", 1);
    float connectionProb = config.getOr("connection_probability", 0.1f);
    
    std::cout << "Neurons: " << neuronCount << std::endl;
    std::cout << "Regions: " << regionCount << std::endl;
    std::cout << "Connection Probability: " << connectionProb << std::endl;
    
    // Check if configuration exists
    if (config.exists("plasticity_learning_rate")) {
        float learningRate = config.getOr("plasticity_learning_rate", 0.01f);
        std::cout << "Plasticity Learning Rate: " << learningRate << std::endl;
    }
}

void demonstrateAdvancedConfiguration() {
    std::cout << "\n=== Advanced Configuration Demo ===" << std::endl;
    
    nlm::ConfigManager config;
    config.enableAdvancedMode();
    
    // Access advanced configuration
    nlm::AdvancedConfig& advancedConfig = config.getAdvancedConfig();
    
    // Set configuration using advanced features
    advancedConfig.set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Programmatic);
    advancedConfig.set("stdp_ltp_weight", 0.025f, nlm::ConfigSource::Programmatic);
    advancedConfig.set("stdp_ltd_weight", 0.03f, nlm::ConfigSource::Programmatic);
    
    // Get configuration statistics
    auto stats = config.getStats();
    std::cout << "Total configuration entries: " << stats.totalEntries << std::endl;
    std::cout << "Default entries: " << stats.defaultEntries << std::endl;
    std::cout << "Programmatic entries: " << stats.programmaticEntries << std::endl;
    
    // Validate configuration
    std::vector<std::string> errors = config.validateAll();
    if (!errors.empty()) {
        std::cout << "Configuration errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    } else {
        std::cout << "Configuration is valid!" << std::endl;
    }
    
    // Export configuration
    std::string configExport = config.exportToString();
    std::cout << "Configuration export (first 500 chars):" << std::endl;
    std::cout << configExport.substr(0, std::min(500, static_cast<int>(configExport.length()))) << "..." << std::endl;
}

void demonstrateConfigurationPresets() {
    std::cout << "\n=== Configuration Presets Demo ===" << std::endl;
    
    nlm::ConfigManager config;
    config.enableAdvancedMode();
    
    // List available presets
    std::vector<std::string> presets = config.listPresets();
    std::cout << "Available presets:" << std::endl;
    for (const auto& preset : presets) {
        std::cout << "  - " << preset << std::endl;
    }
    
    // Create a custom preset
    if (config.createPreset("research_mode", "Configuration optimized for research experiments")) {
        std::cout << "Created 'research_mode' preset" << std::endl;
    }
    
    // Apply a preset
    if (config.applyPreset("nlm_default")) {
        std::cout << "Applied 'nlm_default' preset" << std::endl;
    }
}

void demonstrateConfigurationFileOperations() {
    std::cout << "\n=== Configuration File Operations Demo ===" << std::endl;
    
    nlm::ConfigManager config;
    
    // Export configuration to file
    std::string configFile = "advanced_config.cfg";
    if (config.exportToFile(configFile)) {
        std::cout << "Configuration exported to: " << configFile << std::endl;
    }
    
    // Import configuration from file
    nlm::ConfigManager importedConfig;
    if (importedConfig.importFromFile(configFile)) {
        std::cout << "Configuration imported from: " << configFile << std::endl;
        
        // Verify import
        size_t neurons = importedConfig.getOr("neuron_count", 0);
        std::cout << "Imported neuron count: " << neurons << std::endl;
    }
    
    // Create advanced configuration file
    std::string advancedFile = "advanced_config_detailed.cfg";
    if (config.createAdvancedConfigFile(advancedFile)) {
        std::cout << "Advanced configuration file created: " << advancedFile << std::endl;
    }
}

void demonstrateConfigurationValidation() {
    std::cout << "\n=== Configuration Validation Demo ===" << std::endl;
    
    nlm::ConfigManager config;
    config.enableAdvancedMode();
    
    // Set some invalid configuration values
    config.set("neuron_count", static_cast<size_t>(-100)); // Invalid!
    config.set("connection_probability", 1.5f); // Invalid! (> 1.0)
    config.set("simulation_timestep", -0.001f); // Invalid! (negative)
    
    // Validate configuration
    std::vector<std::string> errors = config.validateAll();
    if (!errors.empty()) {
        std::cout << "Configuration validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    } else {
        std::cout << "Configuration is valid!" << std::endl;
    }
    
    // Fix configuration errors
    if (config.fixConfigErrors()) {
        std::cout << "Configuration errors fixed!" << std::endl;
        
        // Validate again
        errors = config.validateAll();
        if (errors.empty()) {
            std::cout << "Configuration is now valid!" << std::endl;
        }
    }
}

void demonstrateConfigurationExportImport() {
    std::cout << "\n=== Configuration Export/Import Demo ===" << std::endl;
    
    nlm::ConfigManager config;
    config.enableAdvancedMode();
    
    // Set some configuration values
    config.set("neuron_count", static_cast<size_t>(1500));
    config.set("region_count", static_cast<size_t>(3));
    config.set("stdp_ltp_weight", 0.02f);
    config.set("stdp_ltd_weight", 0.018f);
    
    // Export configuration with metadata
    std::string metadata = "Research experiment configuration, version 1.0";
    std::string creator = "NLM Configuration Manager";
    std::string configWithMetadata = config.exportWithFullMetadata(metadata, creator);
    
    std::cout << "Configuration with metadata (first 500 chars):" << std::endl;
    std::cout << configWithMetadata.substr(0, std::min(500, static_cast<int>(configWithMetadata.length()))) << "..." << std::endl;
    
    // Import configuration
    nlm::ConfigManager importedConfig;
    importedConfig.enableAdvancedMode();
    
    if (importedConfig.importWithFullMetadata(configWithMetadata, metadata, creator)) {
        std::cout << "Configuration imported successfully!" << std::endl;
        
        // Verify import
        size_t neurons = importedConfig.getOr("neuron_count", 0);
        size_t regions = importedConfig.getOr("region_count", 0);
        float stdpLtp = importedConfig.getOr("stdp_ltp_weight", 0.0f);
        
        std::cout << "Imported configuration:" << std::endl;
        std::cout << "  Neurons: " << neurons << std::endl;
        std::cout << "  Regions: " << regions << std::endl;
        std::cout << "  STDP LTP weight: " << stdpLtp << std::endl;
    }
}

void demonstrateConfigurationManagement() {
    std::cout << "\n=== Configuration Management Demo ===" << std::endl;
    
    nlm::ConfigManager config;
    config.enableAdvancedMode();
    
    // Show configuration summary
    config.logSummary();
    
    // Create backup
    std::string backupFile = "config_backup.cfg";
    if (config.backup(backupFile)) {
        std::cout << "Configuration backed up to: " << backupFile << std::endl;
    }
    
    // Make some changes
    config.set("neuron_count", static_cast<size_t>(2500));
    config.set("plasticity_learning_rate", 0.02f);
    
    // Show status again
    std::cout << "\nAfter configuration changes:" << std::endl;
    config.logSummary();
    
    // Restore from backup
    nlm::ConfigManager restoredConfig;
    if (restoredConfig.restore(backupFile)) {
        std::cout << "Configuration restored from backup" << std::endl;
        
        // Verify restoration
        size_t neurons = restoredConfig.getOr("neuron_count", 0);
        std::cout << "Restored neuron count: " << neurons << std::endl;
    }
}

void demonstrateConfigurationCommands() {
    std::cout << "\n=== Configuration Commands Demo ===" << std::endl;
    
    nlm::ConfigManager config;
    config.enableAdvancedMode();
    
    // Demonstrate various configuration commands
    std::cout << "1. Get configuration keys by pattern:" << std::endl;
    std::vector<std::string> keys = config.getKeysByPattern("neuron_*");
    for (const auto& key : keys) {
        std::cout << "   - " << key << std::endl;
    }
    
    std::cout << "\n2. Get configuration sections:" << std::endl;
    std::vector<std::string> sections = config.getSectionKeys(nlm::AdvancedConfig::Section::Neural);
    for (const auto& section : sections) {
        std::cout << "   - " << section << std::endl;
    }
    
    std::cout << "\n3. Get configuration statistics:" << std::endl;
    auto stats = config.getStats();
    std::cout << "   Total entries: " << stats.totalEntries << std::endl;
    std::cout << "   Default entries: " << stats.defaultEntries << std::endl;
    std::cout << "   Programmatic entries: " << stats.programmaticEntries << std::endl;
    
    std::cout << "\n4. Get configuration density:" << std::endl;
    float density = config.getConfigDensity();
    std::cout << "   Density: " << density << std::endl;
    
    std::cout << "\n5. Get configuration health score:" << std::endl;
    float health = config.getHealthScore();
    std::cout << "   Health score: " << health << std::endl;
    
    std::cout << "\n6. Get configuration recommendations:" << std::endl;
    std::vector<std::string> recommendations = config.getRecommendations();
    for (const auto& recommendation : recommendations) {
        std::cout << "   - " << recommendation << std::endl;
    }
    
    std::cout << "\n7. Get configuration warnings:" << std::endl;
    std::vector<std::string> warnings = config.getWarnings();
    for (const auto& warning : warnings) {
        std::cout << "   - " << warning << std::endl;
    }
}

void demonstrateConfigurationCLI() {
    std::cout << "\n=== Configuration CLI Demo ===" << std::endl;
    
    std::cout << "Example command line arguments:" << std::endl;
    std::cout << "  --neuron_count=2000" << std::endl;
    std::cout << "  --region_count=4" << std::endl;
    std::cout << "  --connection_probability=0.2" << std::endl;
    std::cout << "  --stdp_ltp_weight=0.03" << std::endl;
    std::cout << "  --stdp_ltd_weight=0.028" << std::endl;
    std::cout << "  --plasticity_learning_rate=0.02" << std::endl;
    std::cout << "  --simulation_timestep=0.0005" << std::endl;
    
    std::cout << "\nExample command line with spaces:" << std::endl;
    std::cout << "  --neuron_count 1000" << std::endl;
    std::cout << "  --region_count 2" << std::endl;
    std::cout << "  --connection_probability 0.15" << std::endl;
    
    std::cout << "\nExample for advanced users:" << std::endl;
    std::cout << "  --config=advanced_config.cfg" << std::endl;
    std::cout << "  --enable_advanced_config" << std::endl;
    std::cout << "  --export_config=exported_config.cfg" << std::endl;
    std::cout << "  --import_config=imported_config.cfg" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "=== Advanced NLM Configuration System Demo ===" << std::endl;
    std::cout << "This program demonstrates the new advanced configuration system for NLM." << std::endl;
    std::cout << "It shows how to use both basic and advanced configuration features." << std::endl;
    
    // Run demonstrations
    demonstrateBasicConfiguration();
    demonstrateAdvancedConfiguration();
    demonstrateConfigurationPresets();
    demonstrateConfigurationFileOperations();
    demonstrateConfigurationValidation();
    demonstrateConfigurationExportImport();
    demonstrateConfigurationManagement();
    demonstrateConfigurationCommands();
    demonstrateConfigurationCLI();
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "The advanced configuration system provides:" << std::endl;
    std::cout << "  - Backward compatibility with existing configuration system" << std::endl;
    std::cout << "  - Advanced configuration features with type safety" << std::endl;
    std::cout << "  - Configuration validation and error correction" << std::endl;
    std::cout << "  - Configuration presets for common use cases" << std::endl;
    std::cout << "  - Configuration export/import capabilities" << std::endl;
    std::cout << "  - Configuration management and backup" << std::endl;
    std::cout << "  - Command line argument parsing" << std::endl;
    std::cout << "  - Configuration statistics and health monitoring" << std::endl;
    std::cout << "  - Configuration recommendations and suggestions" << std::endl;
    
    return 0;
}
