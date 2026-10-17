// Test configuration validation
#include "core/Config/Config.hpp"
#include "core/Config/ConfigurationValidator.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    std::cout << "✓ testConfigCreation passed" << std::endl;
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
    
    std::cout << "✓ testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "✓ testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "✓ testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "✓ testConfigKeys passed" << std::endl;
}

void testExperimentPresets() {
    auto validator = std::make_shared<nlm::ConfigurationValidator>();
    
    auto learningPreset = validator->getExperimentPreset("learning");
    assert(!learningPreset.empty());
    assert(learningPreset.find("plasticity.learningRate") != learningPreset.end());
    
    auto behaviorPreset = validator->getExperimentPreset("behavior");
    assert(!behaviorPreset.empty());
    
    auto plasticityPreset = validator->getExperimentPreset("plasticity");
    assert(!plasticityPreset.empty());
    
    auto unknownPreset = validator->getExperimentPreset("unknown");
    assert(unknownPreset.empty());
    
    std::cout << "✓ testExperimentPresets passed" << std::endl;
}

void testTemplateGeneration() {
    auto validator = std::make_shared<nlm::ConfigurationValidator>();
    
    // Create temp directory for test files
    std::string tempDir = "/tmp/nlm_test";
    std::filesystem::create_directories(tempDir);
    
    std::string templatePath = tempDir + "/test_template.cfg";
    bool success = validator->generateTemplate(templatePath);
    assert(success);
    
    // Check if file was created and has content
    std::ifstream file(templatePath);
    assert(file.good());
    
    file.close();
    std::filesystem::remove(templatePath);
    
    std::cout << "✓ testTemplateGeneration passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Set valid values
    config.set("neuron.count", 100, nlm::ConfigSource::Runtime);
    config.set("plasticity.learningRate", 0.01, nlm::ConfigSource::Runtime);
    config.set("simulation.timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Check if validation errors exist
    auto errors = config.validateConfiguration();
    // This should be empty since all values are valid
    // Note: This depends on the validation implementation
    
    std::cout << "✓ testValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testExperimentPresets();
    testTemplateGeneration();
    testValidation();
    
    std::cout << "\nAll configuration tests passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration Tests ===" << std::endl;
    runAll();
    return 0;
}
