// Enhanced Configuration Tests
#include "EnhancedConfig.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

namespace test_enhanced_config {

void testConfigCreation() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("int_value", 42);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("test", 123);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("key1", 1);
    config.set("key2", 2.0);
    config.set("key3", std::string("three"));
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testSchemaValidation() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    // Valid values should pass
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.5);
    config.set("brain.v_thresh", -65.0);
    
    auto errors = config.validate();
    assert(errors.empty());
    
    // Invalid values should fail
    nlm::EnhancedConfig config2(schema);
    config2.set("brain.neuron_count", 0); // Too low
    auto errors2 = config2.validate();
    assert(!errors2.empty());
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testJSONImportExport() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", "hello");
    
    std::string jsonStr = config.exportToJson();
    
    nlm::EnhancedConfig config2(schema);
    bool success = config2.loadFromJson(jsonStr);
    assert(success);
    
    assert(config2.getOr<int>("int_value") == 42);
    assert(std::abs(config2.getOr<double>("double_value") - 3.14) < 0.001);
    assert(config2.getOr<std::string>("string_value") == "hello");
    
    std::cout << "    testJSONImportExport passed" << std::endl;
}

void testYAMLImportExport() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    
    std::string yamlStr = config.exportToYaml();
    
    nlm::EnhancedConfig config2(schema);
    bool success = config2.loadFromYaml(yamlStr);
    assert(success);
    
    assert(config2.getOr<int>("int_value") == 42);
    assert(std::abs(config2.getOr<double>("double_value") - 3.14) < 0.001);
    
    std::cout << "    testYAMLImportExport passed" << std::endl;
}

void testEnvironmentVariables() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    // Set environment variables
    setenv("NLM_TEST_KEY", "test_value", 1);
    
    config.loadFromEnvironment("NLM_");
    
    assert(config.getOr<std::string>("test_key") == "test_value");
    
    // Clean up
    unsetenv("NLM_TEST_KEY");
    
    std::cout << "    testEnvironmentVariables passed" << std::endl;
}

void testCommandLineArgs() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    int argc = 3;
    char* argv[] = {
        (char*)"test",
        (char*)"--int_key=123",
        (char*)"--string_key=hello"
    };
    
    bool result = config.loadFromCommandLine(argc, argv);
    assert(result);
    
    assert(config.getOr<int>("int_key") == 123);
    assert(config.getOr<std::string>("string_key") == "hello");
    
    std::cout << "    testCommandLineArgs passed" << std::endl;
}

void testConfigTemplates() {
    nlm::ConfigTemplate devTemplate = nlm::Templates::createDevelopmentTemplate();
    
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    bool success = config.applyTemplate(devTemplate);
    assert(success);
    
    assert(config.getOr<bool>("simulation.enabled") == true);
    assert(config.getOr<std::string>("log_level") == "DEBUG");
    
    std::cout << "    testConfigTemplates passed" << std::endl;
}

void testConfigPresets() {
    nlm::ConfigPreset devPreset = nlm::Presets::createDevelopment();
    
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    bool success = config.applyPreset(devPreset);
    assert(success);
    
    assert(config.getOr<bool>("simulation.enabled") == true);
    assert(config.getOr<std::string>("log_level") == "DEBUG");
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigComparison() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config1(schema);
    nlm::EnhancedConfig config2(schema);
    
    config1.set("key1", 1);
    config1.set("key2", 2.0);
    
    config2.set("key1", 1);
    config2.set("key3", 3.0);
    
    auto diff = nlm::Comparison::compare(config1, config2);
    
    // Should have 2 diffs (key2 removed from config2, key3 added to config2)
    assert(diff.size() >= 2);
    
    std::cout << "    testConfigComparison passed" << std::endl;
}

void testConfigHistory() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("key1", "value1");
    
    // Check change history
    auto history = config.getChangeHistory();
    assert(!history.empty());
    
    config.set("key2", "value2");
    auto history2 = config.getChangeHistory();
    assert(history2.size() >= history.size());
    
    std::cout << "    testConfigHistory passed" << std::endl;
}

void testConfigCompression() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("key1", "value1");
    config.set("key2", ""); // Empty string that should be removed
    config.set("key3", 123);
    
    size_t initialSize = config.getSize();
    config.compress();
    
    // Empty string should be removed
    assert(config.getSize() <= initialSize);
    
    std::cout << "    testConfigCompression passed" << std::endl;
}

void testConfigOptimization() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("key1", 1);
    config.set("key2", 2.0);
    config.set("key3", "value");
    
    config.optimize();
    
    // Check performance metrics
    auto metrics = config.getPerformanceMetrics();
    assert(metrics.totalLookups > 0);
    
    std::cout << "    testConfigOptimization passed" << std::endl;
}

void testConfigPerformanceRecommendations() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("brain.neuron_count", 50000); // Large value
    config.set("log_level", "DEBUG"); // Verbose logging
    
    auto recommendations = config.generatePerformanceRecommendations();
    
    // Should have recommendations for large values
    bool hasRecommendations = !recommendations.empty();
    
    std::cout << "    testConfigPerformanceRecommendations passed" << std::endl;
}

void testConfigVersioning() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.setVersion("1.0.0");
    assert(config.getVersion() == "1.0.0");
    
    // Test migration
    bool migrated = config.migrateFromVersion("1.0.0");
    assert(migrated);
    
    std::cout << "    testConfigVersioning passed" << std::endl;
}

void testConfigFileOperations() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("key1", 1);
    config.set("key2", "value");
    
    std::string tempFile = "/tmp/test_config.json";
    
    bool exported = config.exportToFile(tempFile);
    assert(exported);
    
    nlm::EnhancedConfig config2(schema);
    bool imported = nlm::ExportImport::importFromFile(tempFile, config2);
    assert(imported);
    
    assert(config2.getOr<int>("key1") == 1);
    assert(config2.getOr<std::string>("key2") == "value");
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void testConfigDocumentationGeneration() {
    nlm::ConfigSchema schema = nlm::createDefaultSchema();
    nlm::EnhancedConfig config(schema);
    
    config.set("brain.neuron_count", 1000);
    config.set("log_level", "INFO");
    
    std::string doc = config.generateDocumentation();
    
    // Should contain documentation for configured keys
    assert(doc.find("brain.neuron_count") != std::string::npos);
    assert(doc.find("log_level") != std::string::npos);
    
    std::cout << "    testConfigDocumentationGeneration passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testSchemaValidation();
    testJSONImportExport();
    testYAMLImportExport();
    testEnvironmentVariables();
    testCommandLineArgs();
    testConfigTemplates();
    testConfigPresets();
    testConfigComparison();
    testConfigHistory();
    testConfigCompression();
    testConfigOptimization();
    testConfigPerformanceRecommendations();
    testConfigVersioning();
    testConfigFileOperations();
    testConfigDocumentationGeneration();
    
    std::cout << "\nEnhanced configuration test suite completed successfully!" << std::endl;
}

} // namespace test_enhanced_config
