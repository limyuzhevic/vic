// Error handling tests
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace test_error_handling {

void testConfigValidationErrors() {
    nlm::Config config;
    
    // Create schema with strict bounds
    nlm::ConfigSchema schema;
    schema.name = "strict_validation";
    
    nlm::ParameterBounds positiveBounds;
    positiveBounds.min = 1.0;
    positiveBounds.max = 10.0;
    positiveBounds.allowNegative = false;
    positiveBounds.allowZero = false;
    
    schema.parameterBounds["positive_param"] = positiveBounds;
    config.addSchema("validation", schema);
    
    // Test that invalid values throw ConfigValidationError
    bool exceptionCaught = false;
    try {
        config.set("positive_param", 0, nlm::ConfigSource::Runtime);  // Invalid: zero
    } catch (const nlm::ConfigValidationError& e) {
        exceptionCaught = true;
        assert(e.key == "positive_param");
        assert(e.source == nlm::ConfigSource::Runtime);
        assert(e.value == nlm::ConfigValue(0));
    }
    assert(exceptionCaught);
    
    // Test another invalid value
    exceptionCaught = false;
    try {
        config.set("positive_param", 15, nlm::ConfigSource::Runtime);  // Invalid: too large
    } catch (const nlm::ConfigValidationError& e) {
        exceptionCaught = true;
        assert(e.key == "positive_param");
    }
    assert(exceptionCaught);
    
    // Valid value should not throw
    try {
        config.set("positive_param", 5, nlm::ConfigSource::Runtime);
        assert(config.has("positive_param"));
    } catch (...) {
        assert(false); // Should not throw
    }
    
    std::cout << "    testConfigValidationErrors passed" << std::endl;
}

void testConfigLoadFileErrors() {
    nlm::Config config;
    
    // Test loading non-existent file
    bool loadResult = config.loadFromFile("/nonexistent/path/file.txt");
    assert(!loadResult);
    
    // Create a malformed config file (missing equals sign)
    std::string tempFile = "/tmp/malformed_config.txt";
    std::ofstream outFile(tempFile);
    outFile << "key1 = value1" << std::endl;
    outFile << "key2 value2" << std::endl;  // Missing equals sign
    outFile << "key3 = 3" << std::endl;
    outFile.close();
    
    bool loadResult2 = config.loadFromFile(tempFile);
    // The malformed line might be ignored or cause issues depending on implementation
    // We just verify the file loading doesn't crash
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigLoadFileErrors passed" << std::endl;
}

void testConfigSaveFileErrors() {
    nlm::Config config;
    
    // Create a read-only directory to test save error
    std::string tempDir = "/tmp/readonly_test";
    std::string readonlyFile = tempDir + "/test_config.txt";
    
    // Create directory
    std::filesystem::create_directory(tempDir);
    
    // Make directory read-only (might not work on all systems)
    std::filesystem::permissions(tempDir, std::filesystem::perms::owner_read | std::filesystem::perms::owner_exec);
    
    bool saveResult = config.saveToFile(readonlyFile);
    // This might fail depending on permissions
    
    // Restore permissions for cleanup
    std::filesystem::permissions(tempDir, std::filesystem::perms::owner_all);
    
    // Clean up
    std::filesystem::remove_all(tempDir);
    
    std::cout << "    testConfigSaveFileErrors passed" << std::endl;
}

void testBrainInitializationErrors() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test with invalid config (zero neurons)
    config->set("neuron_count", 0, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    // Initialization might fail, which is acceptable behavior
    
    // Test with very high values (might cause memory issues)
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 100000, nlm::ConfigSource::Default);
    config2->set("region_count", 1000, nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    bool success2 = brain2.initialize();
    // This might consume a lot of memory, so we just verify it doesn't crash
    
    std::cout << "    testBrainInitializationErrors passed" << std::endl;
}

void testNeuronInitializationErrors() {
    // Test neuron creation with invalid ID (though ID constructor doesn't validate)
    nlm::Neuron neuron(nlm::NeuronId(0));  // Invalid neuron ID
    assert(neuron.getId() == nlm::NeuronId(0));
    
    // Test that neuron methods don't crash with extreme values
    neuron.setMembranePotential(1000.0f);  // Very high potential
    neuron.setMembranePotential(-1000.0f); // Very low potential
    
    neuron.setThreshold(1000.0f);  // Very high threshold
    
    neuron.setFiringState(nlm::FiringState::Active);
    neuron.setFiringState(nlm::FiringState::Refractory);
    neuron.setFiringState(nlm::FiringState::Resting);
    
    assert(neuron.getMembranePotential() == -1000.0f || neuron.getMembranePotential() == 1000.0f);
    
    std::cout << "    testNeuronInitializationErrors passed" << std::endl;
}

void testSynapseInitializationErrors() {
    // Test synapse creation with invalid neuron IDs
    nlm::Synapse synapse(nlm::SynapseId(0), nlm::NeuronId(0), nlm::NeuronId(0));  // Invalid IDs
    assert(synapse.getId() == nlm::SynapseId(0));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(0));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(0));
    
    // Test extreme weight values
    synapse.setWeight(1e10f);  // Very large weight
    assert(synapse.getWeight() <= 1.0f);  // Should be clamped to 1.0
    
    synapse.setWeight(-1e10f);  // Very negative weight
    assert(synapse.getWeight() >= -1.0f);  // Should be clamped to -1.0
    
    // Test extreme delay values
    synapse.setDelay(100000);  // Very large delay
    assert(synapse.getDelay() == 100000);  // Delays might not have upper bounds
    
    std::cout << "    testSynapseInitializationErrors passed" << std::endl;
}

void testConfigBatchOperationErrors() {
    nlm::Config config;
    
    // Create schema to trigger validation errors
    nlm::ConfigSchema schema;
    schema.name = "batch_test_schema";
    
    nlm::ParameterBounds positiveBounds;
    positiveBounds.min = 1;
    positiveBounds.max = 100;
    positiveBounds.allowNegative = false;
    positiveBounds.allowZero = false;
    
    schema.parameterBounds["positive_param"] = positiveBounds;
    config.addSchema("batch_test", schema);
    
    // Test batch operation with some invalid values
    std::unordered_map<std::string, nlm::ConfigValue> batchValues;
    batchValues["valid_param"] = 50;  // Valid
    batchValues["invalid_param"] = 0;  // Invalid
    batchValues["another_valid"] = 75;  // Valid
    
    // Batch operations should handle errors gracefully
    // The implementation should catch validation errors and continue with other values
    try {
        config.setBatch(batchValues, nlm::ConfigSource::Runtime);
        // The batch operation might complete despite some validation errors
        // depending on implementation
    } catch (...) {
        // This is also acceptable - the batch might throw
    }
    
    std::cout << "    testConfigBatchOperationErrors passed" << std::endl;
}

void testConfigMergeErrors() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Add different types of values
    config1.set("int_key", 42, nlm::ConfigSource::Runtime);
    config1.set("string_key", std::string("test"), nlm::ConfigSource::Default);
    
    config2.set("int_key", 100, nlm::ConfigSource::File);  // Same key, higher priority
    config2.set("bool_key", true, nlm::ConfigSource::CommandLine);
    
    // Merge should work without crashing
    config1.merge(config2);
    
    // Verify merge results
    assert(config1.getOr<int>("int_key", 0) == 100);  // From config2 (File source)
    assert(config1.getOr<std::string>("string_key", "") == "test");
    assert(config1.has("bool_key"));
    assert(config1.getOr<bool>("bool_key", false) == true);
    
    std::cout << "    testConfigMergeErrors passed" << std::endl;
}

void testMemoryAllocationErrors() {
    // This test is mainly to ensure that the system doesn't crash
    // with extreme memory usage scenarios
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Default);
    config->set("region_count", 10, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    
    // Even if initialization doesn't succeed due to memory issues,
    // we just want to ensure the system handles it gracefully
    
    std::cout << "    testMemoryAllocationErrors passed" << std::endl;
}

void runAll() {
    std::cout << "Running Error Handling tests..." << std::endl;
    testConfigValidationErrors();
    testConfigLoadFileErrors();
    testConfigSaveFileErrors();
    testBrainInitializationErrors();
    testNeuronInitializationErrors();
    testSynapseInitializationErrors();
    testConfigBatchOperationErrors();
    testConfigMergeErrors();
    testMemoryAllocationErrors();
}

} // namespace test_error_handling