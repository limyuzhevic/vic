// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include "core/Validation/ValidationExceptions.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

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

void testValidation() {
    nlm::Config config;
    
    // Set valid brain system parameters
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("stdp_ltp_weight", 0.01, nlm::ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.012, nlm::ConfigSource::Runtime);
    config.set("stdp_tau", 20.0, nlm::ConfigSource::Runtime);
    
    // Validate against brain system schema
    auto validationResult = config.validateBrainSystem();
    assert(validationResult.isValid);
    
    std::cout << "    testValidation passed" << std::endl;
}

void testValidationErrors() {
    nlm::Config config;
    
    // Set invalid parameters
    config.set("neuron_count", 0, nlm::ConfigSource::Runtime);  // Invalid: must be > 0
    config.set("connection_probability", 1.5, nlm::ConfigSource::Runtime);  // Invalid: must be <= 1
    
    // Validate against brain system schema
    auto validationResult = config.validateBrainSystem();
    assert(!validationResult.isValid);
    assert(!validationResult.errors.empty());
    
    std::cout << "    testValidationErrors passed" << std::endl;
}

void testParameterValidation() {
    nlm::Config config;
    
    // Set a parameter that should fail validation
    config.set("neuron_count", -1, nlm::ConfigSource::Runtime);
    
    // Validate specific parameter
    auto errorOpt = config.validateParameter("neuron_count");
    assert(errorOpt.has_value());
    assert(!errorOpt.value().empty());
    
    std::cout << "    testParameterValidation passed" << std::endl;
}

void testFileLoading() {
    nlm::Config config;
    
    // Create a temporary config file
    std::string testFile = "/tmp/test_config.txt";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "neuron_count = 500\n";
    file << "connection_probability = 0.2\n";
    file << "simulation_timestep = 0.0005\n";
    file.close();
    
    // Load from file
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    // Check values
    assert(config.getOr<int>("neuron_count", 0) == 500);
    assert(config.getOr<double>("connection_probability", -1.0) == 0.2);
    assert(config.getOr<double>("simulation_timestep", -1.0) == 0.0005);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testFileLoading passed" << std::endl;
}

void testFileSaving() {
    nlm::Config config;
    
    // Set some values
    config.set("neuron_count", 2000, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.05, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.002, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string testFile = "/tmp/test_save_config.txt";
    bool saved = config.saveToFile(testFile);
    assert(saved);
    
    // Verify file exists and contains placeholder values
    std::ifstream file(testFile);
    assert(file.is_open());
    
    std::string line;
    bool foundNeuron = false;
    bool foundConnection = false;
    while (std::getline(file, line)) {
        if (line.find("neuron_count") != std::string::npos) foundNeuron = true;
        if (line.find("connection_probability") != std::string::npos) foundConnection = true;
    }
    
    file.close();
    assert(foundNeuron);
    assert(foundConnection);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testFileSaving passed" << std::endl;
}

void testConfigurationSchema() {
    nlm::Config config;
    
    // Get the brain system schema
    auto schema = config.createBrainSystemSchema();
    assert(!schema.getParameterNames().empty());
    
    // Check that key parameters exist
    assert(schema.hasParameter("neuron_count"));
    assert(schema.hasParameter("connection_probability"));
    assert(schema.hasParameter("simulation_timestep"));
    assert(schema.hasParameter("stdp_ltp_weight"));
    assert(schema.hasParameter("stdp_ltd_weight"));
    assert(schema.hasParameter("stdp_tau"));
    
    std::cout << "    testConfigurationSchema passed" << std::endl;
}

void testConfigurationGroups() {
    nlm::Config config;
    
    // Add some groups
    config.addGroup("neuron_system", "Parameters related to neurons");
    config.addGroup("plasticity_system", "Parameters related to synaptic plasticity");
    config.addGroup("memory_system", "Parameters related to memory systems");
    
    // Check groups
    auto groups = config.getGroups();
    assert(groups.size() >= 3);
    assert(std::find(groups.begin(), groups.end(), "neuron_system") != groups.end());
    assert(std::find(groups.begin(), groups.end(), "plasticity_system") != groups.end());
    assert(std::find(groups.begin(), groups.end(), "memory_system") != groups.end());
    
    std::cout << "    testConfigurationGroups passed" << std::endl;
}

void testConfigurationId() {
    nlm::Config config;
    
    // Generate config ID
    std::string id1 = config.generateConfigId();
    assert(!id1.empty());
    assert(id1.find("config_") == 0);
    
    // Generate another ID - should be different
    std::string id2 = config.generateConfigId();
    assert(id1 != id2);
    
    std::cout << "    testConfigurationId passed" << std::endl;
}

void testDocumentationGeneration() {
    nlm::Config config;
    
    // Set some values
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    // Generate documentation
    std::string doc = config.generateDocumentation("Test Documentation");
    assert(!doc.empty());
    assert(doc.find("Test Documentation") != std::string::npos);
    assert(doc.find("neuron_count") != std::string::npos);
    assert(doc.find("connection_probability") != std::string::npos);
    
    std::cout << "    testDocumentationGeneration passed" << std::endl;
}

void testBackupAndRestore() {
    nlm::Config config;
    
    // Set some values
    config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.3, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Create backup
    std::string backupFile = "/tmp/config_backup.txt";
    bool backupSuccess = config.backup(backupFile);
    assert(backupSuccess);
    
    // Create new config and restore
    nlm::Config config2;
    bool restoreSuccess = config2.restore(backupFile);
    assert(restoreSuccess);
    
    // Check that values were restored
    assert(config2.getOr<int>("neuron_count", 0) == 500);
    assert(config2.getOr<double>("connection_probability", -1.0) == 0.3);
    assert(config2.getOr<double>("simulation_timestep", -1.0) == 0.001);
    
    // Clean up
    std::remove(backupFile.c_str());
    
    std::cout << "    testBackupAndRestore passed" << std::endl;
}

void testConfigurationComparison() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set different values
    config1.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config2.set("neuron_count", 2000, nlm::ConfigSource::Runtime);
    config1.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config2.set("connection_probability", 0.2, nlm::ConfigSource::Runtime);
    
    // Compare configurations
    std::string diff = config1.compareWith(config2);
    assert(!diff.empty());
    assert(diff.find("neuron_count") != std::string::npos);
    assert(diff.find("connection_probability") != std::string::npos);
    
    std::cout << "    testConfigurationComparison passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testValidation();
    testValidationErrors();
    testParameterValidation();
    testFileLoading();
    testFileSaving();
    testConfigurationSchema();
    testConfigurationGroups();
    testConfigurationId();
    testDocumentationGeneration();
    testBackupAndRestore();
    testConfigurationComparison();
    
    std::cout << "\nAll enhanced config tests passed!" << std::endl;
}

} // namespace test_config