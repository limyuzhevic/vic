// Configuration System Tests
// Comprehensive test suite for configuration system functionality

#include "ConfigurationSystemTestSuite.h"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <random>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <fstream>
#include <sstream>

namespace test_suites {

class ConfigurationSystemTestSuite {
public:
    static void runAllTests() {
        std::cout << "=== Running Configuration System Tests ===" << std::endl;
        
        test_configuration_validation_and_error_handling();
        test_configuration_schema_validation();
        test_configuration_merging_and_versioning();
        test_configuration_health_monitoring();
        test_configuration_persistence();
        test_configuration_distribution();
        test_configuration_access_control();
        test_configuration_templates();
        test_configuration_diff();
        test_configuration_migration();
        
        std::cout << "=== Configuration System Tests Complete ===" << std::endl;
    }

private:
    static void test_configuration_validation_and_error_handling() {
        std::cout << "Testing configuration validation and error handling..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Configuration validation testing
        std::vector<std::string> validationTestCases;
        std::vector<std::string> errorTypes;
        std::vector<bool> validationResults;
        
        for (int step = 0; step < 50; ++step) {
            // Test case configuration
            std::string testCase = "validation_test_" + std::to_string(step % 5);
            validationTestCases.push_back(testCase);
            
            // Add configuration based on test case
            if (step % 2 == 0) {
                config->set("valid_key", static_cast<int64_t>(step), nlm::ConfigSource::Default);
                config->set("neuron_count", static_cast<int64_t>(100 + step), nlm::ConfigSource::Default);
                config->set("learning_rate", 0.1f * (step % 10), nlm::ConfigSource::Default);
            }
            
            if (step % 3 == 0) {
                // Invalid configuration (test error handling)
                try {
                    config->set("invalid_key", "invalid_value", nlm::ConfigSource::Default);
                } catch (...) {
                    // Expected error
                }
            }
            
            // Validate configuration
            std::string errorType = "error_type_" + std::to_string(step % 4);
            errorTypes.push_back(errorType);
            
            bool isValid = config->validate();
            validationResults.push_back(isValid);
            
            if (!isValid) {
                // Get validation errors
                auto errors = config->getValidationErrors();
                assert(!errors.empty());
            }
            
            // Test error recovery
            if (step % 10 == 0) {
                config->reset();
            }
        }
        
        // Verify configuration validation
        assert(validationTestCases.size() == 50);
        assert(errorTypes.size() == 50);
        assert(validationResults.size() == 50);
        
        // Validation should be effective
        float validationSuccessRate = 0.0f;
        for (bool result : validationResults) {
            if (result) validationSuccessRate += 100.0f;
        }
        validationSuccessRate /= validationResults.size();
        assert(validationSuccessRate >= 60.0f);  // At least 60% validation success
        
        std::cout << "    Configuration validation and error handling: PASSED" << std::endl;
    }
    
    static void test_configuration_schema_validation() {
        std::cout << "Testing configuration schema validation..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Schema validation testing
        std::vector<std::string> schemaTestCases;
        std::vector<std::string> schemaVersions;
        std::vector<bool> schemaValidationResults;
        
        // Create schema test cases
        for (int step = 0; step < 40; ++step) {
            std::string testCase = "schema_test_" + std::to_string(step % 4);
            schemaTestCases.push_back(testCase);
            
            std::string schemaVersion = "schema_v" + std::to_string(1 + (step / 10));
            schemaVersions.push_back(schemaVersion);
            
            // Set up configuration according to schema
            config->set("schema_version", schemaVersion, nlm::ConfigSource::Default);
            
            // Validate against schema
            bool isValid = config->validateAgainstSchema(schemaVersion);
            schemaValidationResults.push_back(isValid);
            
            if (!isValid) {
                // Get schema validation errors
                auto errors = config->getSchemaValidationErrors();
                assert(!errors.empty());
            }
            
            // Test schema evolution
            if (step % 20 == 0) {
                config->set("schema_evolution", "true", nlm::ConfigSource::Default);
            }
        }
        
        // Verify schema validation
        assert(schemaTestCases.size() == 40);
        assert(schemaVersions.size() == 40);
        assert(schemaValidationResults.size() == 40);
        
        // Schema validation should be effective
        float schemaValidationSuccessRate = 0.0f;
        for (bool result : schemaValidationResults) {
            if (result) schemaValidationSuccessRate += 100.0f;
        }
        schemaValidationSuccessRate /= schemaValidationResults.size();
        assert(schemaValidationSuccessRate >= 70.0f);  // At least 70% schema validation success
        
        std::cout << "    Configuration schema validation: PASSED" << std::endl;
    }
    
    static void test_configuration_merging_and_versioning() {
        std::cout << "Testing configuration merging and versioning..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Configuration merging and versioning testing
        std::vector<std::string> mergeTestCases;
        std::vector<std::string> versionNumbers;
        std::vector<bool> mergeResults;
        
        for (int step = 0; step < 60; ++step) {
            std::string mergeTestCase = "merge_test_" + std::to_string(step % 6);
            mergeTestCases.push_back(mergeTestCase);
            
            std::string version = "v" + std::to_string(1 + (step / 15));
            versionNumbers.push_back(version);
            
            // Create base configuration
            config->set("base_config", "true", nlm::ConfigSource::Default);
            config->set("version", version, nlm::ConfigSource::Default);
            config->set("neuron_count", 100 + step, nlm::ConfigSource::Default);
            config->set("learning_rate", 0.01f * (step % 5 + 1), nlm::ConfigSource::Default);
            
            // Merge with another configuration
            bool mergeSuccess = config->merge(
                std::map<std::string, std::string>{{"override_1", "value_1"},
                                                  {"override_2", "value_2"}},
                nlm::MergeStrategy::Override
            );
            mergeResults.push_back(mergeSuccess);
            
            if (mergeSuccess) {
                // Verify merge
                assert(config->has("override_1"));
                assert(config->has("override_2"));
            }
        }
        
        // Verify configuration merging
        assert(mergeTestCases.size() == 60);
        assert(versionNumbers.size() == 60);
        assert(mergeResults.size() == 60);
        
        // Merge operations should be successful
        float mergeSuccessRate = 0.0f;
        for (bool result : mergeResults) {
            if (result) mergeSuccessRate += 100.0f;
        }
        mergeSuccessRate /= mergeResults.size();
        assert(mergeSuccessRate >= 80.0f);  // At least 80% merge success
        
        std::cout << "    Configuration merging and versioning: PASSED" << std::endl;
    }
    
    static void test_configuration_health_monitoring() {
        std::cout << "Testing configuration health monitoring..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Configuration health monitoring testing
        std::vector<std::string> healthTestCases;
        std::vector<float> healthScores;
        std::vector<bool> healthCheckResults;
        
        for (int step = 0; step < 50; ++step) {
            std::string healthTestCase = "health_test_" + std::to_string(step % 5);
            healthTestCases.push_back(healthTestCase);
            
            // Configure health monitoring
            config->enableHealthMonitoring(true);
            
            // Add various configuration entries
            config->set("health_check_1", step, nlm::ConfigSource::Default);
            config->set("health_check_2", 0.5f * (step % 10), nlm::ConfigSource::Default);
            config->set("health_check_3", "health_status_" + std::to_string(step % 3), nlm::ConfigSource::Default);
            
            // Check configuration health
            bool isHealthy = config->isConfigurationHealthy();
            healthCheckResults.push_back(isHealthy);
            
            // Get health score
            float healthScore = config->getConfigurationHealth().score;
            healthScores.push_back(healthScore);
            
            if (!isHealthy) {
                // Get health issues
                auto issues = config->getConfigurationHealth().issues;
                assert(!issues.empty());
            }
            
            // Test health repair
            if (step % 15 == 0) {
                bool repaired = config->repairConfiguration();
                assert(repaired || !isHealthy);
            }
        }
        
        // Verify configuration health monitoring
        assert(healthTestCases.size() == 50);
        assert(healthScores.size() == 50);
        assert(healthCheckResults.size() == 50);
        
        // Health monitoring should be effective
        float avgHealthScore = std::accumulate(healthScores.begin(), healthScores.end(), 0.0f) / healthScores.size();
        assert(avgHealthScore >= 0.5f && avgHealthScore <= 1.0f);
        
        float healthSuccessRate = 0.0f;
        for (bool result : healthCheckResults) {
            if (result) healthSuccessRate += 100.0f;
        }
        healthSuccessRate /= healthCheckResults.size();
        assert(healthSuccessRate >= 70.0f);  // At least 70% health monitoring success
        
        std::cout << "    Configuration health monitoring: PASSED" << std::endl;
    }
    
    static void test_configuration_persistence() {
        std::cout << "Testing configuration persistence..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Configuration persistence testing
        std::vector<std::string> persistenceTestCases;
        std::vector<std::string> filePaths;
        std::vector<bool> persistenceResults;
        
        for (int step = 0; step < 40; ++step) {
            std::string testCase = "persist_test_" + std::to_string(step % 4);
            persistenceTestCases.push_back(testCase);
            
            // Create test configuration
            config->set("persistent_key_1", "persistent_value_1", nlm::ConfigSource::Default);
            config->set("persistent_key_2", step, nlm::ConfigSource::Default);
            config->set("persistent_key_3", 0.5f + step * 0.01f, nlm::ConfigSource::Default);
            
            // Generate file path
            std::string filePath = "/tmp/test_config_" + std::to_string(step) + ".json";
            filePaths.push_back(filePath);
            
            // Save configuration
            bool saved = config->saveToFile(filePath);
            persistenceResults.push_back(saved);
            
            if (saved) {
                // Load configuration
                auto loadedConfig = std::make_shared<nlm::Config>();
                bool loaded = loadedConfig->loadFromFile(filePath);
                assert(loaded);
                
                // Verify loaded configuration
                assert(loadedConfig->has("persistent_key_1"));
                assert(loadedConfig->has("persistent_key_2"));
                assert(loadedConfig->has("persistent_key_3"));
            }
            
            // Clean up
            std::remove(filePath.c_str());
        }
        
        // Verify configuration persistence
        assert(persistenceTestCases.size() == 40);
        assert(filePaths.size() == 40);
        assert(persistenceResults.size() == 40);
        
        // Persistence operations should be successful
        float persistenceSuccessRate = 0.0f;
        for (bool result : persistenceResults) {
            if (result) persistenceSuccessRate += 100.0f;
        }
        persistenceSuccessRate /= persistenceResults.size();
        assert(persistenceSuccessRate >= 85.0f);  // At least 85% persistence success
        
        std::cout << "    Configuration persistence: PASSED" << std::endl;
    }
    
    static void test_configuration_distribution() {
        std::cout << "Testing configuration distribution..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Configuration distribution testing
        std::vector<std::string> distributionTestCases;
        std::vector<std::string> distributionTargets;
        std::vector<bool> distributionResults;
        
        for (int step = 0; step < 50; ++step) {
            std::string testCase = "dist_test_" + std::to_string(step % 5);
            distributionTestCases.push_back(testCase);
            
            // Configure for distribution
            config->set("distribution_source", "local", nlm::ConfigSource::Default);
            config->set("distribution_target_1", "target_1", nlm::ConfigSource::Default);
            config->set("distribution_target_2", "target_2", nlm::ConfigSource::Default);
            
            std::string target = "dist_target_" + std::to_string(step % 3);
            distributionTargets.push_back(target);
            
            // Distribute configuration
            bool distributed = config->distributeToTarget(target);
            distributionResults.push_back(distributed);
            
            if (distributed) {
                // Verify distribution
                auto status = config->getDistributionStatus(target);
                assert(status == nlm::DistributionStatus::Distributed);
            }
            
            if (step % 10 == 0) {
                // Test distribution rollback
                config->rollbackDistribution(target);
            }
        }
        
        // Verify configuration distribution
        assert(distributionTestCases.size() == 50);
        assert(distributionTargets.size() == 50);
        assert(distributionResults.size() == 50);
        
        // Distribution operations should be successful
        float distributionSuccessRate = 0.0f;
        for (bool result : distributionResults) {
            if (result) distributionSuccessRate += 100.0f;
        }
        distributionSuccessRate /= distributionResults.size();
        assert(distributionSuccessRate >= 75.0f);  // At least 75% distribution success
        
        std::cout << "    Configuration distribution: PASSED" << std::endl;
    }
    
    static void test_configuration_access_control() {
        std::cout << "Testing configuration access control..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Configuration access control testing
        std::vector<std::string> accessTestCases;
        std::vector<std::string> userRoles;
        std::vector<bool> accessResults;
        
        for (int step = 0; step < 45; ++step) {
            std::string testCase = "access_test_" + std::to_string(step % 5);
            accessTestCases.push_back(testCase);
            
            // Set up user roles
            config->set("access_control_enabled", "true", nlm::ConfigSource::Default);
            config->set("user_role", "user_" + std::to_string(step % 5), nlm::ConfigSource::Default);
            config->set("permissions", "read_write", nlm::ConfigSource::Default);
            
            std::string userRole = "role_" + std::to_string(step % 4);
            userRoles.push_back(userRole);
            
            // Test access control
            bool accessGranted = config->checkAccessControl(userRole);
            accessResults.push_back(accessGranted);
            
            if (accessGranted) {
                // User can access configuration
                bool canRead = config->canRead(userRole);
                bool canWrite = config->canWrite(userRole);
                assert(canRead && canWrite);
            } else {
                // User cannot access configuration
                bool canRead = config->canRead(userRole);
                bool canWrite = config->canWrite(userRole);
                assert(!canRead && !canWrite);
            }
            
            if (step % 12 == 0) {
                // Test role-based access control
                config->set("rbac_enabled", "true", nlm::ConfigSource::Default);
            }
        }
        
        // Verify configuration access control
        assert(accessTestCases.size() == 45);
        assert(userRoles.size() == 45);
        assert(accessResults.size() == 45);
        
        // Access control should be effective
        float accessControlSuccessRate = 0.0f;
        for (bool result : accessResults) {
            if (result) accessControlSuccessRate += 100.0f;
        }
        accessControlSuccessRate /= accessResults.size();
        assert(accessControlSuccessRate >= 70.0f);  // At least 70% access control success
        
        std::cout << "    Configuration access control: PASSED" << std::endl;
    }
    
    static void test_configuration_templates() {
        std::cout << "Testing configuration templates..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Configuration templates testing
        std::vector<std::string> templateTestCases;
        std::vector<std::string> templateTypes;
        std::vector<bool> templateResults;
        
        for (int step = 0; step < 40; ++step) {
            std::string testCase = "template_test_" + std::to_string(step % 4);
            templateTestCases.push_back(testCase);
            
            // Set up template system
            config->set("template_system_enabled", "true", nlm::ConfigSource::Default);
            
            std::string templateType = "template_type_" + std::to_string(step % 3);
            templateTypes.push_back(templateType);
            
            // Apply template
            bool templateApplied = config->applyTemplate(templateType);
            templateResults.push_back(templateApplied);
            
            if (templateApplied) {
                // Get template parameters
                auto params = config->getTemplateParameters(templateType);
                assert(!params.empty());
            }
            
            if (step % 10 == 0) {
                // Test template customization
                config->customizeTemplate(templateType, "custom_param", "custom_value");
            }
        }
        
        // Verify configuration templates
        assert(templateTestCases.size() == 40);
        assert(templateTypes.size() == 40);
        assert(templateResults.size() == 40);
        
        // Template operations should be successful
        float templateSuccessRate = 0.0f;
        for (bool result : templateResults) {
            if (result) templateSuccessRate += 100.0f;
        }
        templateSuccessRate /= templateResults.size();
        assert(templateSuccessRate >= 80.0f);  // At least 80% template success
        
        std::cout << "    Configuration templates: PASSED" << std::endl;
    }
    
    static void test_configuration_diff() {
        std::cout << "Testing configuration diff..." << std::endl;
        auto config1 = std::make_shared<nlm::Config>();
        auto config2 = std::make_shared<nlm::Config>();
        
        // Configuration diff testing
        std::vector<std::string> diffTestCases;
        std::vector<bool> diffResults;
        
        for (int step = 0; step < 35; ++step) {
            std::string testCase = "diff_test_" + std::to_string(step % 4);
            diffTestCases.push_back(testCase);
            
            // Create test configurations
            config1->set("config_key_1", "value_1", nlm::ConfigSource::Default);
            config1->set("config_key_2", 100 + step, nlm::ConfigSource::Default);
            config1->set("config_key_3", 0.5f * (step % 10), nlm::ConfigSource::Default);
            
            config2->set("config_key_1", "value_2", nlm::ConfigSource::Default);  // Different
            config2->set("config_key_2", 100 + step + 10, nlm::ConfigSource::Default);  // Different
            config2->set("config_key_4", "new_value", nlm::ConfigSource::Default);  // New
            
            // Calculate diff
            auto diff = config1->calculateDiff(config2);
            bool hasDiff = !diff.empty();
            diffResults.push_back(hasDiff);
            
            if (hasDiff) {
                // Apply diff
                bool applied = config1->applyDiff(diff);
                assert(applied);
            }
        }
        
        // Verify configuration diff
        assert(diffTestCases.size() == 35);
        assert(diffResults.size() == 35);
        
        // Diff should be detectable
        float diffDetectionRate = 0.0f;
        for (bool result : diffResults) {
            if (result) diffDetectionRate += 100.0f;
        }
        diffDetectionRate /= diffResults.size();
        assert(diffDetectionRate >= 90.0f);  // At least 90% diff detection rate
        
        std::cout << "    Configuration diff: PASSED" << std::endl;
    }
    
    static void test_configuration_migration() {
        std::cout << "Testing configuration migration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        
        // Configuration migration testing
        std::vector<std::string> migrationTestCases;
        std::vector<std::string> sourceVersions;
        std::vector<bool> migrationResults;
        
        for (int step = 0; step < 30; ++step) {
            std::string testCase = "migration_test_" + std::to_string(step % 4);
            migrationTestCases.push_back(testCase);
            
            // Set up migration
            std::string sourceVersion = "source_v" + std::to_string(1 + (step % 3));
            sourceVersions.push_back(sourceVersion);
            
            config->set("migration_source_version", sourceVersion, nlm::ConfigSource::Default);
            config->set("migration_target_version", "target_v1", nlm::ConfigSource::Default);
            
            // Perform migration
            bool migrated = config->migrateConfiguration(sourceVersion, "target_v1");
            migrationResults.push_back(migrated);
            
            if (migrated) {
                // Verify migration
                assert(config->has("migrated_config"));
            }
            
            if (step % 10 == 0) {
                // Test rollback
                config->rollbackMigration("target_v1");
            }
        }
        
        // Verify configuration migration
        assert(migrationTestCases.size() == 30);
        assert(sourceVersions.size() == 30);
        assert(migrationResults.size() == 30);
        
        // Migration operations should be successful
        float migrationSuccessRate = 0.0f;
        for (bool result : migrationResults) {
            if (result) migrationSuccessRate += 100.0f;
        }
        migrationSuccessRate /= migrationResults.size();
        assert(migrationSuccessRate >= 80.0f);  // At least 80% migration success
        
        std::cout << "    Configuration migration: PASSED" << std::endl;
    }
};

} // namespace test_suites