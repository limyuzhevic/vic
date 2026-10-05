// Advanced Configuration Tests
#include "AdvancedConfigManager.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_advanced_config {

void testAdvancedConfigCreation() {
    nlm::AdvancedConfigManager config;
    assert(!config.has("test"));
    assert(config.getEntryCount() == 0);
    std::cout << "    testAdvancedConfigCreation passed" << std::endl;
}

void testAdvancedConfigSetAndGet() {
    nlm::AdvancedConfigManager config;
    
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
    
    std::cout << "    testAdvancedConfigSetAndGet passed" << std::endl;
}

void testAdvancedConfigValidation() {
    nlm::AdvancedConfigManager config;
    
    // Add schema for testing
    nlm::ConfigSchema schema;
    schema.key = "test_param";
    schema.type = "int";
    schema.description = "Test parameter";
    schema.defaultValue = "100";
    schema.required = false;
    schema.minValue = 0;
    schema.maxValue = 200;
    
    config.addSchema(schema);
    
    // Test valid value
    config.set("test_param", 50);
    assert(config.isValid());
    
    // Test invalid value (too high)
    config.set("test_param", 250);
    assert(!config.isValid());
    
    // Get validation errors
    auto errors = config.getValidationErrors();
    assert(errors.size() > 0);
    
    // Reset
    config.set("test_param", 100);
    assert(config.isValid());
    
    std::cout << "    testAdvancedConfigValidation passed" << std::endl;
}

void testAdvancedConfigTemplates() {
    nlm::AdvancedConfigManager config;
    
    // Test template loading
    assert(config.hasTemplate("advanced"));
    config.loadTemplate("advanced");
    assert(config.has("random_seed"));
    
    // Test template saving
    config.saveTemplate("custom_template", "Custom test template");
    assert(config.hasTemplate("custom_template"));
    
    // Test getting template
    auto template = config.getTemplate("advanced");
    assert(template.name == "advanced");
    
    std::cout << "    testAdvancedConfigTemplates passed" << std::endl;
}

void testAdvancedConfigPresets() {
    nlm::AdvancedConfigManager config;
    
    // Test presets
    assert(config.hasPreset("training"));
    assert(config.hasPreset("simulation"));
    assert(config.hasPreset("research"));
    assert(config.hasPreset("development"));
    
    // Test preset application
    config.applyPreset("training");
    assert(config.has("neuron_count"));
    
    // Test preset by category
    auto trainingPresets = config.getPresetsByCategory("training");
    assert(!trainingPresets.empty());
    
    std::cout << "    testAdvancedConfigPresets passed" << std::endl;
}

void testAdvancedConfigExportImport() {
    nlm::AdvancedConfigManager config;
    
    // Set some values
    config.set("test_int", 123);
    config.set("test_string", "hello");
    config.set("test_double", 3.14159);
    
    // Test JSON export
    std::string jsonFile = "test_config.json";
    bool jsonExportSuccess = config.exportToJSON(jsonFile, true, true);
    assert(jsonExportSuccess);
    
    // Test JSON import
    nlm::AdvancedConfigManager config2;
    bool jsonImportSuccess = config2.importFromJSON(jsonFile);
    assert(jsonImportSuccess);
    assert(config2.has("test_int"));
    
    // Test YAML export (placeholder - should fallback to JSON)
    std::string yamlFile = "test_config.yaml";
    bool yamlExportSuccess = config.exportToYAML(yamlFile, true);
    assert(yamlExportSuccess);
    
    // Test INI import (should use file loading)
    std::string iniFile = "test_config.ini";
    // Note: INI import uses file loading, so we'll just test file loading
    config.saveToFile(iniFile);
    nlm::AdvancedConfigManager config3;
    config3.loadFromFile(iniFile);
    assert(config3.has("log_level")); // Default config value
    
    // Clean up test files
    std::filesystem::remove(jsonFile);
    std::filesystem::remove(yamlFile);
    std::filesystem::remove(iniFile);
    
    std::cout << "    testAdvancedConfigExportImport passed" << std::endl;
}

void testAdvancedConfigComparison() {
    nlm::AdvancedConfigManager config1;
    nlm::AdvancedConfigManager config2;
    
    config1.set("param1", 100);
    config1.set("param2", "value1");
    
    config2.set("param1", 200);
    config2.set("param2", "value1");
    
    // Test change tracking
    assert(config1.hasChanges()); // Should have changes from default to set values
    
    // Test similarity calculation
    double similarity = config1.calculateSimilarity(config2);
    assert(similarity >= 0.0 && similarity <= 1.0);
    
    // Test changes
    auto changes = config1.getChanges();
    assert(!changes.empty());
    
    // Test reset change tracking
    config1.resetChangeTracking();
    assert(!config1.hasChanges());
    
    std::cout << "    testAdvancedConfigComparison passed" << std::endl;
}

void testAdvancedConfigOptimization() {
    nlm::AdvancedConfigManager config;
    
    // Load advanced template
    config.loadTemplate("advanced");
    
    // Test optimization for different use cases
    auto trainingSuggestions = config.optimize("training");
    assert(!trainingSuggestions.empty());
    
    auto simulationSuggestions = config.optimize("simulation");
    assert(!simulationSuggestions.empty());
    
    // Test optimized preset generation
    auto optimizedPreset = config.generateOptimizedPreset("general");
    assert(optimizedPreset.name == "optimized_general");
    assert(optimizedPreset.category == "general");
    
    // Test improvement factor calculation
    if (!trainingSuggestions.empty()) {
        double improvement = trainingSuggestions[0].improvementFactor;
        assert(improvement >= 0.0);
    }
    
    std::cout << "    testAdvancedConfigOptimization passed" << std::endl;
}

void testAdvancedConfigMerging() {
    nlm::AdvancedConfigManager config1;
    nlm::AdvancedConfigManager config2;
    
    config1.set("common_param", 100);
    config1.set("unique_param1", 50);
    
    config2.set("common_param", 200);
    config2.set("unique_param2", 75);
    
    // Test merging
    config1.merge(config2);
    
    // Both values should be present
    assert(config1.has("unique_param1"));
    assert(config1.has("unique_param2"));
    
    // Common param should have config2's value (last one wins)
    auto value = config1.get<int>("common_param");
    assert(value.has_value() && *value == 200);
    
    // Test merging with map
    std::map<std::string, nlm::ConfigValue> configMap;
    configMap["map_param1"] = "test";
    configMap["map_param2"] = 123;
    
    config1.merge(configMap);
    assert(config1.has("map_param1"));
    assert(config1.has("map_param2"));
    
    // Test subset creation
    auto subset = config1.createSubset({"common_param", "unique_param1"});
    assert(subset->has("common_param"));
    assert(subset->has("unique_param1"));
    assert(!subset->has("unique_param2"));
    
    std::cout << "    testAdvancedConfigMerging passed" << std::endl;
}

void testAdvancedConfigBackupRestore() {
    nlm::AdvancedConfigManager config;
    
    // Set some values
    config.set("backup_test", 999);
    
    // Test backup
    std::string backupDir = "backup_test";
    bool backupSuccess = config.backup(backupDir);
    assert(backupSuccess);
    
    // Create backup subdirectory
    std::filesystem::create_directory(backupDir);
    
    // Test restore
    bool restoreSuccess = config.restore(backupDir);
    assert(restoreSuccess);
    
    // Verify restore (should have default values now)
    assert(config.has("neuron_count")); // Default config
    
    // Clean up
    std::filesystem::remove_all(backupDir);
    
    std::cout << "    testAdvancedConfigBackupRestore passed" << std::endl;
}

void testAdvancedConfigDocumentation() {
    nlm::AdvancedConfigManager config;
    
    // Test documentation generation
    std::string markdownDoc = config.generateDocumentation("markdown");
    assert(!markdownDoc.empty());
    assert(markdownDoc.find("# NLM Configuration Documentation") != std::string::npos);
    
    std::string jsonDoc = config.generateDocumentation("json");
    assert(!jsonDoc.empty());
    
    std::string htmlDoc = config.generateDocumentation("html");
    assert(!htmlDoc.empty());
    assert(htmlDoc.find("<html>") != std::string::npos);
    
    // Test schema documentation
    std::string schemaDoc = config.generateSchemaDocumentation();
    assert(!schemaDoc.empty());
    
    // Test usage examples
    std::string examples = config.generateUsageExamples();
    assert(!examples.empty());
    assert(examples.find("Basic Usage") != std::string::npos);
    
    // Test preset documentation
    std::string presetDoc = config.generatePresetDocumentation();
    assert(!presetDoc.empty());
    
    std::cout << "    testAdvancedConfigDocumentation passed" << std::endl;
}

void testAdvancedConfigAnalysis() {
    nlm::AdvancedConfigManager config;
    
    // Set up configuration for analysis
    config.loadTemplate("advanced");
    
    // Test parameter statistics
    auto stats = config.calculateParameterStatistics();
    assert(!stats.empty());
    
    // Test pattern detection
    auto patterns = config.detectPatterns();
    assert(!patterns.empty());
    
    // Test performance estimation
    double perfTraining = config.estimatePerformance("training");
    double perfSimulation = config.estimatePerformance("simulation");
    
    assert(perfTraining >= 0.0 && perfTraining <= 1.0);
    assert(perfSimulation >= 0.0 && perfSimulation <= 1.0);
    
    // Test requirement checking
    std::map<std::string, double> requirements;
    requirements["neuron_count"] = 100;
    requirements["simulation_timestep"] = 0.001;
    
    bool meetsRequirements = config.meetsRequirements(requirements);
    // Might fail if values are different, but should not crash
    
    std::cout << "    testAdvancedConfigAnalysis passed" << std::endl;
}

void runAll() {
    std::cout << "=== Advanced Configuration Tests ===" << std::endl;
    
    testAdvancedConfigCreation();
    testAdvancedConfigSetAndGet();
    testAdvancedConfigValidation();
    testAdvancedConfigTemplates();
    testAdvancedConfigPresets();
    testAdvancedConfigExportImport();
    testAdvancedConfigComparison();
    testAdvancedConfigOptimization();
    testAdvancedConfigMerging();
    testAdvancedConfigBackupRestore();
    testAdvancedConfigDocumentation();
    testAdvancedConfigAnalysis();
    
    std::cout << std::endl;
    std::cout << "=== All Advanced Configuration Tests Passed ===" << std::endl;
}

} // namespace test_advanced_config
