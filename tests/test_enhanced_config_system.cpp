// Test the enhanced configuration system
// This test verifies that the new configuration system works correctly

#include "core/ConfigSystem.hpp"
#include "core/Config/ConfigBuilder.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_config_system {

void testConfigSystemInitialization() {
    std::cout << "Testing ConfigSystem initialization..." << std::endl;
    
    // Initialize with default config
    bool initialized = ConfigSystem::initialize("configs/default.cfg", 0, nullptr);
    assert(initialized);
    
    // Get configuration
    auto config = ConfigSystem::getConfig();
    assert(config != nullptr);
    
    std::cout << "    ConfigSystem initialization passed" << std::endl;
}

void testConfigValidation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    // Get validator
    auto validator = ConfigSystem::getValidator();
    assert(validator != nullptr);
    
    // Validate configuration
    auto validationResults = ConfigSystem::validate();
    std::cout << "    Validation results: " << validationResults.size() << " issues found" << std::endl;
    
    // If there are validation errors, report them
    for (const auto& result : validationResults) {
        if (!result.valid) {
            std::cout << "    Validation error for key '" << result.key << "': " << result.message << std::endl;
        }
    }
    
    std::cout << "    Configuration validation test completed" << std::endl;
}

void testConfigBuilder() {
    std::cout << "Testing ConfigBuilder functionality..." << std::endl;
    
    ConfigBuilder builder;
    
    // Add a simulation group
    builder.addSimulationGroup();
    
    // Add a plasticity group  
    builder.addPlasticityGroup();
    
    // Add a neuromodulation group
    builder.addNeuromodulationGroup();
    
    // Build configuration
    auto config = builder.buildConfig();
    assert(config != nullptr);
    
    // Check some key parameters
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    assert(config->has("stdp_ltp_weight"));
    assert(config->has("stdp_ltd_weight"));
    assert(config->has("dopamine_baseline"));
    
    std::cout << "    ConfigBuilder functionality test passed" << std::endl;
}

void testJSONExportImport() {
    std::cout << "Testing JSON export/import functionality..." << std::endl;
    
    // Export configuration to JSON
    std::string jsonConfig = ConfigSystem::exportToJson();
    assert(!jsonConfig.empty());
    
    // Import configuration from JSON
    bool imported = ConfigSystem::importFromJson(jsonConfig);
    assert(imported);
    
    // Verify import worked
    auto config = ConfigSystem::getConfig();
    assert(config != nullptr);
    
    std::cout << "    JSON export/import test passed" << std::endl;
}

void testCommandLineParsing() {
    std::cout << "Testing command-line parsing..." << std::endl;
    
    // Initialize with command-line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"nlm",
        (char*)"--neurons",
        (char*)"1000",
        (char*)"--regions",
        (char*)"2"
    };
    
    bool initialized = ConfigSystem::initialize("configs/default.cfg", argc, argv);
    assert(initialized);
    
    // Check that command-line values were set
    auto config = ConfigSystem::getConfig();
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    
    std::cout << "    Command-line parsing test passed" << std::endl;
}

void testHelpDisplay() {
    std::cout << "Testing help display..." << std::endl;
    
    // The help should be displayed when --help is passed
    // For this test, we just verify that the printHelp function exists
    // and can be called without crashing
    ConfigSystem::printHelp();
    
    std::cout << "    Help display test completed" << std::endl;
}

void testBackupAndRestore() {
    std::cout << "Testing backup and restore functionality..." << std::endl;
    
    // Get current configuration
    auto config = ConfigSystem::getConfig();
    assert(config != nullptr);
    
    // Create a backup
    bool backupCreated = ConfigSystem::createBackup("Test backup");
    assert(backupCreated);
    
    // Get backup list
    auto backups = ConfigSystem::getManager()->getBackups();
    assert(!backups.empty());
    
    // Restore from backup
    bool restored = ConfigSystem::restoreFromBackup(0);
    assert(restored);
    
    std::cout << "    Backup and restore test passed" << std::endl;
}

void testComprehensiveConfigOptions() {
    std::cout << "Testing comprehensive configuration options..." << std::endl;
    
    // Reset to defaults
    ConfigSystem::resetToDefaults();
    
    auto config = ConfigSystem::getConfig();
    assert(config != nullptr);
    
    // Test various configuration options from different systems
    
    // Simulation parameters
    assert(config->has("simulation_timestep"));
    assert(config->has("max_simulation_steps"));
    
    // Plasticity parameters
    assert(config->has("stdp_ltp_weight"));
    assert(config->has("stdp_ltd_weight"));
    assert(config->has("stdp_tau"));
    assert(config->has("plasticity_learning_rate"));
    
    // Neuromodulation parameters
    assert(config->has("dopamine_baseline"));
    assert(config->has("dopamine_peak"));
    assert(config->has("curiosity_level"));
    
    // Development parameters
    assert(config->has("development_stage"));
    assert(config->has("development_synaptogenesis_rate"));
    
    // Memory parameters
    assert(config->has("working_memory_capacity"));
    
    // Motor parameters
    assert(config->has("planning_depth"));
    
    // Reward parameters
    assert(config->has("reward_scaling_factor"));
    
    // Environment parameters
    assert(config->has("environment_name"));
    assert(config->has("environment_width"));
    
    // Logging parameters
    assert(config->has("log_level"));
    assert(config->has("log_to_file"));
    
    // Visualization parameters
    assert(config->has("visualization_enabled"));
    
    std::cout << "    Comprehensive configuration options test passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Testing Enhanced Configuration System ===" << std::endl;
    
    testConfigSystemInitialization();
    testConfigValidation();
    testConfigBuilder();
    testJSONExportImport();
    testCommandLineParsing();
    testHelpDisplay();
    testBackupAndRestore();
    testComprehensiveConfigOptions();
    
    std::cout << "=== All Configuration System Tests Passed ===" << std::endl;
}

} // namespace test_config_system

int main(int argc, char** argv) {
    test_config_system::runAllTests();
    return 0;
}
