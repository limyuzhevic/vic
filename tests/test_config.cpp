// Config Tests - Enhanced with validation and advanced features
#include "core/Config/Config.hpp"
#include "core/Config/AdvancedConfig.hpp"
#include "core/Config/NetworkConfig.hpp"
#include "core/Config/TrainingConfig.hpp"
#include "core/Config/SystemConfig.hpp"
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

void testConfigValidation() {
    nlm::Config config;
    
    // Test valid configuration
    config.set("valid_int", 42, nlm::ConfigSource::Runtime, "Valid integer", nlm::Config::NEURAL_NETWORK_CATEGORY, nlm::Config::TYPE_INT, 0, 100);
    config.set("valid_string", "test", nlm::ConfigSource::Runtime, "Valid string", nlm::Config::TRAINING_CATEGORY, nlm::Config::TYPE_STRING);
    
    // Validation should pass
    config.validate();
    
    // Test invalid configuration - value outside range
    config.set("invalid_int", 200, nlm::ConfigSource::Runtime, "Invalid integer", nlm::Config::NEURAL_NETWORK_CATEGORY, nlm::Config::TYPE_INT, 0, 100);
    
    // Validation should fail
    try {
        config.validate();
        assert(false); // Should have thrown
    } catch (const nlm::ValidationError& e) {
        std::cout << "    Caught expected validation error: " << e.what() << std::endl;
    }
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigPresets() {
    nlm::Config config;
    
    // Create and save a preset
    nlm::PresetConfig preset("test_preset", "Test preset configuration");
    preset.values["test_param"] = 42;
    preset.values["another_param"] = std::string("test_value");
    
    assert(nlm::Config::savePreset(preset));
    
    // Load the preset
    auto loadedPreset = nlm::Config::loadPreset("test_preset");
    assert(loadedPreset.has_value());
    assert(loadedPreset->name == "test_preset");
    assert(loadedPreset->values.size() == 2);
    
    // Apply the preset
    config.applyPreset(*loadedPreset);
    assert(config.has("test_param"));
    assert(config.getOr<int>("test_param", 0) == 42);
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigInheritance() {
    nlm::Config baseConfig;
    baseConfig.set("common_param", 100, nlm::ConfigSource::Default);
    baseConfig.set("unique_param", 200, nlm::ConfigSource::Default);
    
    nlm::Config childConfig;
    childConfig.set("child_param", 300, nlm::ConfigSource::Default);
    
    // Merge child into parent
    bool changed = childConfig.mergeFrom(baseConfig);
    assert(changed);
    assert(childConfig.has("common_param"));
    assert(childConfig.has("unique_param"));
    assert(childConfig.has("child_param"));
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void testConfigTemplates() {
    nlm::Config config;
    
    // Create a template
    assert(config.createTemplate("test_template", "Test template configuration"));
    
    // Apply the template
    assert(config.applyTemplate("test_template"));
    
    // Check that template markers were set
    assert(config.has("_template_name"));
    assert(config.has("_template_desc"));
    
    std::cout << "    testConfigTemplates passed" << std::endl;
}

void testConfigSearch() {
    nlm::Config config;
    
    config.set("nn_layer_count", 3, nlm::ConfigSource::Runtime);
    config.set("nn_neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("tr_learning_rate", 0.001, nlm::ConfigSource::Runtime);
    config.set("sys_log_level", "info", nlm::ConfigSource::Runtime);
    
    // Search for "nn" pattern
    auto nnKeys = config.searchKeys("nn");
    assert(nnKeys.size() == 2);
    assert(std::find(nnKeys.begin(), nnKeys.end(), "nn_layer_count") != nnKeys.end());
    assert(std::find(nnKeys.begin(), nnKeys.end(), "nn_neuron_count") != nnKeys.end());
    
    // Search for "log" pattern
    auto logKeys = config.searchKeys("log");
    assert(logKeys.size() == 1);
    assert(logKeys[0] == "sys_log_level");
    
    std::cout << "    testConfigSearch passed" << std::endl;
}

void testAdvancedConfig() {
    nlm::AdvancedConfig config;
    
    // Test advanced features
    config.setVersion("2.0.0");
    config.setAuthor("Test Author");
    config.setDescription("Test advanced configuration");
    
    config.set("test_param", 123, nlm::ConfigSource::Runtime, "Test parameter", nlm::Config::SYSTEM_CATEGORY, nlm::Config::TYPE_INT, 0, 1000);
    
    // Test validation report
    std::string report = config.validateReport();
    assert(report.find("Configuration is valid") != std::string::npos);
    
    // Test benchmark
    double score = config.benchmark();
    assert(score >= 0.0);
    
    std::cout << "    testAdvancedConfig passed" << std::endl;
}

void testNetworkConfig() {
    nlm::NetworkConfig config;
    
    config.setLayerCount(5);
    config.setNeuronCount(200);
    config.setInputSize(20);
    config.setOutputSize(5);
    config.setConnectionProbability(0.15);
    config.setRecurrentConnections(true);
    
    assert(config.getOr<int64_t>("nn_layer_count", 0) == 5);
    assert(config.getOr<int64_t>("nn_neuron_count", 0) == 200);
    assert(config.getOr<int64_t>("nn_input_size", 0) == 20);
    assert(config.getOr<int64_t>("nn_output_size", 0) == 5);
    assert(config.getOr<double>("nn_connection_prob", 0.0) == 0.15);
    assert(config.getOr<bool>("nn_recurrent_connections", false) == true);
    
    std::string summary = config.getNetworkSummary();
    assert(summary.find("Neural Network Configuration") != std::string::npos);
    
    std::cout << "    testNetworkConfig passed" << std::endl;
}

void testTrainingConfig() {
    nlm::TrainingConfig config;
    
    config.setLearningRate(0.01);
    config.setBatchSize(64);
    config.setEpochs(50);
    config.setOptimizerType("sgd");
    config.setL1Regularization(0.001);
    config.setDropoutRate(0.2);
    
    assert(config.getOr<double>("tr_learning_rate", 0.0) == 0.01);
    assert(config.getOr<int64_t>("tr_batch_size", 0) == 64);
    assert(config.getOr<int64_t>("tr_epochs", 0) == 50);
    assert(config.getOr<std::string>("tr_optimizer_type", "") == "sgd");
    assert(config.getOr<double>("tr_l1_regularization", 0.0) == 0.001);
    assert(config.getOr<double>("tr_dropout_rate", 0.0) == 0.2);
    
    std::string summary = config.getTrainingSummary();
    assert(summary.find("Training Configuration") != std::string::npos);
    
    std::cout << "    testTrainingConfig passed" << std::endl;
}

void testSystemConfig() {
    nlm::SystemConfig config;
    
    config.setThreadCount(8);
    config.setMemoryLimit(4 * 1024 * 1024 * 1024); // 4GB
    config.setTimeStep(0.01);
    config.setMaxTime(1000.0);
    config.setLogLevel("debug");
    config.setLogFile("/tmp/nlm_test.log");
    config.setPerformanceMonitoring(true);
    config.setProfilingEnabled(true);
    
    assert(config.getOr<int64_t>("sys_thread_count", 0) == 8);
    assert(config.getOr<int64_t>("sys_memory_limit", 0) == 4 * 1024 * 1024 * 1024);
    assert(config.getOr<double>("sys_time_step", 0.0) == 0.01);
    assert(config.getOr<double>("sys_max_time", 0.0) == 1000.0);
    assert(config.getOr<std::string>("sys_log_level", "") == "debug");
    assert(config.getOr<std::string>("sys_log_file", "") == "/tmp/nlm_test.log");
    assert(config.getOr<bool>("sys_performance_monitoring", false) == true);
    assert(config.getOr<bool>("sys_profiling_enabled", false) == true);
    
    std::string summary = config.getSystemSummary();
    assert(summary.find("System Configuration") != std::string::npos);
    
    std::cout << "    testSystemConfig passed" << std::endl;
}

void testConfigExportImport() {
    nlm::Config config;
    
    config.set("test_param", 42, nlm::ConfigSource::Runtime, "Test parameter");
    config.set("another_param", std::string("value"), nlm::ConfigSource::Runtime, "Another parameter");
    
    // Export to file
    std::string filepath = "/tmp/test_config_export.cfg";
    assert(nlm::ConfigUtils::exportConfig(config, filepath));
    
    // Import from file
    auto importedConfig = nlm::ConfigUtils::importConfig(filepath);
    assert(importedConfig.has_value());
    assert(importedConfig->has("test_param"));
    assert(importedConfig->has("another_param"));
    
    // Cleanup
    std::filesystem::remove(filepath);
    
    std::cout << "    testConfigExportImport passed" << std::endl;
}

void testConfigUtils() {
    nlm::Config config1, config2;
    
    config1.set("param1", 100, nlm::ConfigSource::Runtime);
    config1.set("param2", 200, nlm::ConfigSource::Runtime);
    
    config2.set("param1", 100, nlm::ConfigSource::Runtime);
    config2.set("param2", 300, nlm::ConfigSource::Runtime); // Different value
    
    // Test comparison
    assert(!nlm::ConfigUtils::configsEqual(config1, config2));
    
    // Test diff
    std::string diff = nlm::ConfigUtils::compareConfigs(config1, config2);
    assert(diff.find("param2") != std::string::npos);
    
    // Test search
    std::vector<std::string> searchResults = nlm::ConfigUtils::searchConfigs(config1, "param");
    assert(searchResults.size() == 2);
    
    std::cout << "    testConfigUtils passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigPresets();
    testConfigInheritance();
    testConfigTemplates();
    testConfigSearch();
    testAdvancedConfig();
    testNetworkConfig();
    testTrainingConfig();
    testSystemConfig();
    testConfigExportImport();
    testConfigUtils();
    
    std::cout << "\nAll enhanced config tests passed!" << std::endl;
}

} // namespace test_config
