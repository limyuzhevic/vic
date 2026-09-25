// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testAdvancedConfigSetup() {
    nlm::Config config;
    
    // Test memory system configuration
    config.setupMemoryConfig(200.0, 0.2, 0.98, 0.99);
    assert(config.getOr<double>("memory.workingMemoryCapacity", 0.0) == 200.0);
    assert(config.getOr<double>("memory.episodicConsolidationRate", 0.0) == 0.2);
    
    // Test neuromodulation configuration
    config.setupNeuromodulationConfig(0.6, 0.15, 0.75, 0.85, 0.35);
    assert(config.getOr<double>("neuromodulation.dopamineBaseline", 0.0) == 0.6);
    assert(config.getOr<double>("neuromodulation.curiosityLevel", 0.0) == 0.85);
    
    // Test cognitive configuration
    config.setupCognitiveConfig(3, 0.7, 0.98, 0.97, 0.05);
    assert(config.getOr<int>("cognitive.planningDepth", 0) == 3);
    
    // Test prediction configuration
    config.setupPredictionConfig(0.9, 0.05, 0.99, 0.03);
    assert(config.getOr<double>("prediction.confidenceThreshold", 0.0) == 0.9);
    
    // Test development configuration
    config.setupDevelopmentConfig(0.4, 0.8, 150.0, 2000.0, true);
    assert(config.getOr<double>("development.plasticityRate", 0.0) == 0.4);
    
    // Test integration configuration
    config.setupIntegrationConfig(0.8, 0.7, 0.85, 1.2, 0.06);
    assert(config.getOr<double>("integration.corticalCorticalCoupling", 0.0) == 0.8);
    
    std::cout << "    testAdvancedConfigSetup passed" << std::endl;
}

void testConfigurationTemplates() {
    nlm::Config config;
    
    // Test development template
    config.applyTemplate(config.createDevelopmentTemplate());
    assert(config.has("debug.enabled"));
    assert(config.getOr<bool>("debug.enabled", false) == true);
    
    // Test research template
    config.clear();
    config.applyTemplate(config.createResearchTemplate());
    assert(config.getOr<double>("neuromodulation.curiosityLevel", 0.0) == 1.0);
    
    // Test performance template
    config.clear();
    config.applyTemplate(config.createPerformanceTemplate());
    assert(config.getOr<bool>("debug.enabled", true) == false);
    assert(config.getOr<double>("memory.workingMemoryCapacity", 0.0) == 200.0);
    
    // Test robotics template
    config.clear();
    config.applyTemplate(config.createRoboticsTemplate());
    assert(config.getOr<bool>("performance.realtimeConstraint", false) == true);
    assert(config.getOr<double>("prediction.confidenceThreshold", 0.0) == 0.9);
    
    // Test exploration template
    config.clear();
    config.applyTemplate(config.createExplorationTemplate());
    assert(config.getOr<double>("neuromodulation.explorationRate", 0.0) == 0.7);
    
    // Test debug template
    config.clear();
    config.applyTemplate(config.createDebugTemplate());
    assert(config.getOr<bool>("debug.enabled", false) == true);
    assert(config.getOr<std::string>("debug.level", "") == "verbose");
    
    std::cout << "    testConfigurationTemplates passed" << std::endl;
}

void testConfigurationComparison() {
    nlm::Config config1;
    nlm::Config config2;
    
    config1.set("key1", 42, nlm::ConfigSource::Runtime);
    config1.set("key2", "hello", nlm::ConfigSource::File);
    config1.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    
    config2.set("key1", 42, nlm::ConfigSource::Runtime);
    config2.set("key2", "hello", nlm::ConfigSource::File);
    config2.set("key4", "new", nlm::ConfigSource::Runtime);
    
    auto diff = config1.getDiff(config2);
    
    // Should have one diff for key4 (added) and one for key3 (removed)
    assert(diff.size() == 2);
    
    // Test similarity calculation
    config2.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    double similarity = config1.getSimilar(config2)[0].oldValue.get<double>();
    assert(similarity > 0.5); // Should be highly similar
    
    std::cout << "    testConfigurationComparison passed" << std::endl;
}

void testConfigurationExportImport() {
    nlm::Config config;
    
    config.set("int_value", 123, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::File);
    
    // Export to file
    std::string testFile = "test_config_export.json";
    assert(config.exportToFile(testFile));
    
    // Create new config and import
    nlm::Config config2;
    assert(config2.importFromFile(testFile, true));
    
    // Verify imported values
    assert(config2.getOr<int>("int_value", 0) == 123);
    assert(config2.getOr<std::string>("string_value", "") == "test");
    
    // Cleanup
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigurationExportImport passed" << std::endl;
}

void testConfigurationFileIO() {
    nlm::Config config;
    
    // Set up test values
    config.set("test_int", 100, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14159, nlm::ConfigSource::File);
    config.set("test_string", "hello world", nlm::ConfigSource::CommandLine);
    config.set("test_bool", true, nlm::ConfigSource::Default);
    
    // Save to file
    std::string testFile = "test_config.cfg";
    assert(config.saveToFile(testFile, false));
    
    // Create new config and load
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    // Verify loaded values
    assert(config2.getOr<int>("test_int", 0) == 100);
    assert(config2.getOr<double>("test_double", 0.0) == 3.14159);
    assert(config2.getOr<std::string>("test_string", "") == "hello world");
    assert(config2.getOr<bool>("test_bool", false) == true);
    
    // Cleanup
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigurationFileIO passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testAdvancedConfigSetup();
    testConfigurationTemplates();
    testConfigurationComparison();
    testConfigurationExportImport();
    testConfigurationFileIO();
}

} // namespace test_config
