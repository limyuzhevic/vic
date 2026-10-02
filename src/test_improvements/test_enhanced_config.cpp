#include "core/Config/ConfigManager.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void testConfigurationManagement() {
    std::cout << "=== Testing Configuration Management ===" << std::endl;
    
    nlm::ConfigManager configManager;
    
    // Test 1: Load from default config
    std::cout << "1. Loading default configuration..." << std::endl;
    bool loaded = configManager.loadConfig("configs/default.cfg");
    std::cout << "   Config loaded: " << (loaded ? "Yes" : "No") << std::endl;
    
    // Test 2: Get configuration values
    std::cout << "2. Getting configuration values..." << std::endl;
    int randomSeed = configManager.getConfig<int>("random_seed", 42);
    std::cout << "   Random seed: " << randomSeed << std::endl;
    
    double timestep = configManager.getConfig<double>("simulation_timestep", 0.001);
    std::cout << "   Simulation timestep: " << timestep << std::endl;
    
    size_t neuronCount = configManager.getConfig<size_t>("neuron_count", 1000);
    std::cout << "   Neuron count: " << neuronCount << std::endl;
    
    // Test 3: Configuration summary
    std::cout << "3. Configuration summary:" << std::endl;
    std::cout << configManager.getConfigSummary() << std::endl;
    
    // Test 4: Validation
    std::cout << "4. Validating configuration..." << std::endl;
    bool isValid = configManager.validateConfig();
    std::cout << "   Configuration valid: " << (isValid ? "Yes" : "No") << std::endl;
    
    if (!isValid) {
        auto errors = configManager.getValidationErrors();
        std::cout << "   Validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "     - " << error << std::endl;
        }
    }
    
    // Test 5: Export configuration
    std::cout << "5. Exporting configuration..." << std::endl;
    bool exported = configManager.exportConfig("test_export.cfg", true);
    std::cout << "   Configuration exported: " << (exported ? "Yes" : "No") << std::endl;
    
    // Test 6: Preset configurations
    std::cout << "6. Testing preset configurations..." << std::endl;
    std::cout << "   Available presets:" << std::endl;
    auto presets = configManager.getAvailablePresets();
    for (const auto& preset : presets) {
        std::cout << "     - " << preset << std::endl;
    }
    
    std::cout << std::endl;
}

void testErrorHandling() {
    std::cout << "=== Testing Error Handling ===" << std::endl;
    
    // Enable error reporting
    nlm::GlobalErrorHandler::setup(true, true);
    
    // Test 1: Valid operations
    std::cout << "1. Testing valid operations..." << std::endl;
    try {
        auto configManager = std::make_shared<nlm::ConfigManager>();
        std::cout << "   ConfigManager created successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "   ERROR: Failed to create ConfigManager: " << e.what() << std::endl;
    }
    
    // Test 2: Validation errors
    std::cout << "2. Testing validation errors..." << std::endl;
    
    // Create a config with invalid values for testing
    nlm::ConfigManager configManager;
    configManager.setConfig<int>("invalid_key", -999);  // Valid as int
    
    // Test validation
    bool isValid = configManager.validateConfig();
    std::cout << "   Config validation result: " << (isValid ? "Valid" : "Invalid") << std::endl;
    
    // Test 3: Error reporter
    std::cout << "3. Testing error reporter..." << std::endl;
    auto errors = nlm::ErrorReporter::getInstance().getErrors();
    std::cout << "   Total errors reported: " << errors.size() << std::endl;
    
    // Test 4: Error severity filtering
    std::cout << "4. Testing error severity filtering..." << std::endl;
    auto warningErrors = nlm::ErrorReporter::getInstance().getErrors(nlm::ErrorSeverity::Warning);
    auto errorErrors = nlm::ErrorReporter::getInstance().getErrors(nlm::ErrorSeverity::Error);
    auto criticalErrors = nlm::ErrorReporter::getInstance().getErrors(nlm::ErrorSeverity::Critical);
    
    std::cout << "   Warnings: " << warningErrors.size() << std::endl;
    std::cout << "   Errors: " << errorErrors.size() << std::endl;
    std::cout << "   Critical: " << criticalErrors.size() << std::endl;
    
    // Test 5: Global error handler
    std::cout << "5. Testing global error handler..." << std::endl;
    auto recentErrors = nlm::GlobalErrorHandler::getRecentErrors();
    std::cout << "   Recent errors: " << recentErrors.size() << std::endl;
    
    // Test 6: Validation utilities
    std::cout << "6. Testing validation utilities..." << std::endl;
    
    // Test type conversion
    auto intVal = nlm::ValidationUtils::toInt("42");
    std::cout << "   Integer conversion '42': " << (intVal ? std::to_string(*intVal) : "Failed") << std::endl;
    
    auto doubleVal = nlm::ValidationUtils::toDouble("3.14");
    std::cout << "   Double conversion '3.14': " << (doubleVal ? std::to_string(*doubleVal) : "Failed") << std::endl;
    
    auto boolVal = nlm::ValidationUtils::toBool("true");
    std::cout << "   Boolean conversion 'true': " << (boolVal ? (*boolVal ? "true" : "false") : "Failed") << std::endl;
    
    auto invalidBool = nlm::ValidationUtils::toBool("maybe");
    std::cout << "   Boolean conversion 'maybe': " << (invalidBool ? (*invalidBool ? "true" : "false") : "Failed") << std::endl;
    
    std::cout << std::endl;
}

void testAdvancedFeatures() {
    std::cout << "=== Testing Advanced Features ===" << std::endl;
    
    // Test 1: Configuration monitoring
    std::cout << "1. Testing configuration monitoring..." << std::endl;
    nlm::ConfigManager configManager;
    
    configManager.startConfigMonitoring(2.0); // 2 second monitoring interval
    
    configManager.setConfig<int>("test_value", 123);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    auto changes = configManager.getConfigChanges();
    std::cout << "   Changes detected: " << changes.size() << std::endl;
    
    configManager.stopConfigMonitoring();
    
    // Test 2: Configuration diffing
    std::cout << "2. Testing configuration diffing..." << std::endl;
    nlm::ConfigManager config1;
    nlm::ConfigManager config2;
    
    config1.setConfig<int>("key1", 100);
    config1.setConfig<std::string>("key2", "value1");
    
    config2.setConfig<int>("key1", 200); // Different value
    config2.setConfig<std::string>("key3", "value3"); // Different key
    
    std::string diff = config1.diffConfigs(config2);
    std::cout << "   Configuration diff (first 200 chars): " << diff.substr(0, std::min(size_t(200), diff.size())) << "..." << std::endl;
    
    // Test 3: Configuration backup/restore
    std::cout << "3. Testing configuration backup/restore..." << std::endl;
    nlm::ConfigManager configToBackup;
    configToBackup.setConfig<int>("backup_key", 999);
    
    bool backupSuccess = configToBackup.backupConfig("./test_backups");
    std::cout << "   Backup successful: " << (backupSuccess ? "Yes" : "No") << std::endl;
    
    // Test 4: Configuration templates
    std::cout << "4. Testing configuration templates..." << std::endl;
    nlm::ConfigManager configManager2;
    
    bool presetSuccess = configManager2.loadPresetConfig("fast_debug");
    std::cout << "   Preset 'fast_debug' loaded: " << (presetSuccess ? "Yes" : "No") << std::endl;
    
    if (presetSuccess) {
        std::cout << "   Preset values:" << std::endl;
        auto keys = configManager2.getConfig<std::string>("random_seed", "0");
        std::cout << "     - random_seed: " << configManager2.getConfig<int>("random_seed", 0) << std::endl;
        std::cout << "     - neuron_count: " << configManager2.getConfig<int>("neuron_count", 0) << std::endl;
        std::cout << "     - log_level: " << configManager2.getConfig<std::string>("log_level", "") << std::endl;
    }
    
    // Test 5: Error recovery actions
    std::cout << "5. Testing error recovery actions..." << std::endl;
    
    class TestRecoveryAction : public nlm::RecoveryAction {
    public:
        bool canRecover() const override { return true; }
        bool performRecovery() override { 
            std::cout << "     Recovery action performed!" << std::endl;
            return true; 
        }
        std::string getDescription() const override { return "Test recovery action"; }
    };
    
    nlm::RecoveryManager recoveryManager;
    recoveryManager.addRecoveryAction(std::make_shared<TestRecoveryAction>());
    
    std::cout << "   Recovery actions available: " << recoveryManager.getRecoveryActions().size() << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "=== NLM Enhanced Configuration and Error Handling Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testConfigurationManagement();
        testErrorHandling();
        testAdvancedFeatures();
        
        std::cout << "=== All tests completed ===" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
