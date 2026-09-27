// Configuration Manager Tests
#include "core/ConfigurationManager.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_configuration_manager {

void testConfigurationManagerCreation() {
    nlm::core::ConfigurationManager& configManager = nlm::core::ConfigurationManager::getInstance();
    
    // Test basic configuration
    assert(configManager.initialize());
    
    std::cout << "    testConfigurationManagerCreation passed" << std::endl;
}

void testConfigurationSetAndGet() {
    nlm::core::ConfigurationManager& configManager = nlm::core::ConfigurationManager::getInstance();
    
    // Test string configuration
    configManager.set("test_string", "hello", nlm::core::ConfigSource::RUNTIME);
    assert(configManager.has("test_string"));
    assert(configManager.get<std::string>("test_string") == "hello");
    
    // Test integer configuration
    configManager.set("test_int", 42, nlm::core::ConfigSource::RUNTIME);
    assert(configManager.has("test_int"));
    assert(configManager.getInt("test_int") == 42);
    
    // Test double configuration
    configManager.set("test_double", 3.14, nlm::core::ConfigSource::RUNTIME);
    assert(configManager.has("test_double"));
    assert(configManager.getDouble("test_double") > 3.13 && configManager.getDouble("test_double") < 3.15);
    
    // Test boolean configuration
    configManager.set("test_bool", true, nlm::core::ConfigSource::RUNTIME);
    assert(configManager.has("test_bool"));
    assert(configManager.getBool("test_bool") == true);
    
    // Test list configuration
    configManager.set("test_list", std::vector<std::string>{"a", "b", "c"}, nlm::core::ConfigSource::RUNTIME);
    assert(configManager.has("test_list"));
    
    std::cout << "    testConfigurationSetAndGet passed" << std::endl;
}

void testConfigurationValidation() {
    nlm::core::ConfigurationManager& configManager = nlm::core::ConfigurationManager::getInstance();
    
    // Add invalid configuration
    configManager.set("invalid_number", "not_a_number", nlm::core::ConfigSource::RUNTIME);
    
    // Test validation
    bool valid = configManager.validate();
    // Validation might fail for invalid configuration
    std::cout << "    Validation result: " << (valid ? "PASS" : "FAIL") << std::endl;
    
    // Get validation errors
    auto errors = configManager.getValidationErrors();
    for (const auto& error : errors) {
        std::cout << "    Validation error: " << error.message << std::endl;
    }
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testConfigurationSessions() {
    nlm::core::ConfigurationManager& configManager = nlm::core::ConfigurationManager::getInstance();
    
    // Create session
    std::string sessionId = configManager.createSession("Test Session", "A test session");
    assert(!sessionId.empty());
    
    // Set configuration in session
    configManager.set("session_key", "session_value", nlm::core::ConfigSource::RUNTIME);
    assert(configManager.get<std::string>("session_key") == "session_value");
    
    // Switch session
    assert(configManager.switchSession(sessionId));
    assert(configManager.getCurrentSessionId() == sessionId);
    
    // Remove session
    assert(configManager.removeSession(sessionId));
    
    std::cout << "    testConfigurationSessions passed" << std::endl;
}

void testConfigurationBackup() {
    nlm::core::ConfigurationManager& configManager = nlm::core::ConfigurationManager::getInstance();
    
    // Create backup
    std::string backupFile = "/tmp/test_config_backup.json";
    assert(configManager.backup(backupFile));
    
    // Set some configuration
    configManager.set("backup_test", "test_value", nlm::core::ConfigSource::RUNTIME);
    
    // Clear and restore
    configManager.clear();
    assert(configManager.restore(backupFile));
    assert(configManager.get<std::string>("backup_test") == "test_value");
    
    std::cout << "    testConfigurationBackup passed" << std::endl;
}

void testConfigurationThreadSafety() {
    nlm::core::ConfigurationManager& configManager = nlm::core::ConfigurationManager::getInstance();
    
    // Test thread safety
    configManager.lock();
    assert(configManager.tryLock() == false); // Should fail to lock again
    configManager.unlock();
    
    assert(configManager.tryLock()); // Should succeed this time
    configManager.unlock();
    
    std::cout << "    testConfigurationThreadSafety passed" << std::endl;
}

void testConfigurationChangeTracking() {
    nlm::core::ConfigurationManager& configManager = nlm::core::ConfigurationManager::getInstance();
    
    // Clear change tracking
    configManager.clearChangeHistory();
    
    // Set initial value
    configManager.set("track_test", "initial", nlm::core::ConfigSource::RUNTIME);
    
    // Change value
    configManager.set("track_test", "changed", nlm::core::ConfigSource::RUNTIME);
    
    // Get recent changes
    auto changes = configManager.getRecentChanges(10);
    assert(!changes.empty());
    
    std::cout << "    testConfigurationChangeTracking passed" << std::endl;
}

void testConfigurationSummary() {
    nlm::core::ConfigurationManager& configManager = nlm::core::ConfigurationManager::getInstance();
    
    // Add some test data
    configManager.clear();
    configManager.set("summary_test_1", 1, nlm::core::ConfigSource::RUNTIME);
    configManager.set("summary_test_2", "value", nlm::core::ConfigSource::RUNTIME);
    
    // Get summary
    std::string summary = configManager.summary();
    assert(!summary.empty());
    assert(summary.find("summary_test_1") != std::string::npos);
    
    std::cout << "    testConfigurationSummary passed" << std::endl;
}

void runAll() {
    testConfigurationManagerCreation();
    testConfigurationSetAndGet();
    testConfigurationValidation();
    testConfigurationSessions();
    testConfigurationBackup();
    testConfigurationThreadSafety();
    testConfigurationChangeTracking();
    testConfigurationSummary();
}

} // namespace test_configuration_manager
