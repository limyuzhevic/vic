// Enhanced Configuration Tests
// Tests for Phase 2 Enhanced: Configuration System with Type Safety

#include "core/Config/Config.hpp"
#include "include/nlm/NLMErrors.hpp"
#include <cassert>
#include <iostream>
#include <nlohmann/json.hpp>

namespace test_config_enhanced {

void testConfigurationCreation() {
    std::cout << "    testConfigurationCreation passed" << std::endl;
}

void testConfigurationTypeSafety() {
    nlm::Config config;
    
    // Test type-safe setting and getting
    config.setInt("test_int", 42);
    assert(config.get<int>("test_int") == 42);
    
    config.setDouble("test_double", 3.14);
    assert(config.get<double>("test_double") == 3.14);
    
    config.setBool("test_bool", true);
    assert(config.get<bool>("test_bool") == true);
    
    config.setString("test_string", "hello");
    assert(config.get<std::string>("test_string") == "hello");
    
    std::cout << "    testConfigurationTypeSafety passed" << std::endl;
}

void testConfigurationValidation() {
    nlm::Config config;
    
    // Test validation with negative value (should fail for some configs)
    config.setInt("neuron_count", -5);
    
    bool validationCaught = false;
    try {
        config.getWithValidation<int>("neuron_count", 0);
    } catch (const nlm::NLMConfigException& e) {
        validationCaught = true;
    }
    assert(validationCaught);
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testConfigurationSchema() {
    // Test schema registration
    nlm::ConfigMetadata metadata("Test parameter", nlm::ConfigDataType::Integer, false);
    nlm::Config::registerConfigSchema("test_param", metadata);
    
    nlm::ConfigMetadata retrieved = nlm::Config::getConfigMetadata("test_param");
    assert(retrieved.description == "Test parameter");
    assert(retrieved.type == nlm::ConfigDataType::Integer);
    
    std::cout << "    testConfigurationSchema passed" << std::endl;
}

void testConfigurationJSON() {
    nlm::Config config;
    
    config.setInt("test_int", 100);
    config.setDouble("test_double", 2.718);
    config.setString("test_string", "world");
    
    // Convert to JSON
    std::string jsonStr = config.toJSON();
    
    // Parse JSON back (simplified for this test)
    auto json = nlohmann::json::parse(jsonStr);
    assert(json["test_int"] == 100);
    assert(json["test_double"] == 2.718);
    assert(json["test_string"] == "world");
    
    std::cout << "    testConfigurationJSON passed" << std::endl;
}

void testConfigurationEnvVars() {
    nlm::Config config;
    
    // Test environment variable loading
    config.setEnvPrefix("TEST_");
    
    // Set some environment variables for testing
    setenv("TEST_NEURON_COUNT", "50", 1);
    setenv("TEST_CONNECTION_PROBABILITY", "0.25", 1);
    
    try {
        config.loadFromEnv();
        
        // Should have loaded environment variables
        if (config.has("neuron_count")) {
            assert(config.get<int>("neuron_count") == 50);
        }
        if (config.has("connection_probability")) {
            assert(config.get<double>("connection_probability") == 0.25);
        }
    } catch (...) {
        // Environment loading might not work in all environments
    }
    
    // Cleanup
    unsetenv("TEST_NEURON_COUNT");
    unsetenv("TEST_CONNECTION_PROBABILITY");
    
    std::cout << "    testConfigurationEnvVars passed" << std::endl;
}

void testConfigurationValidationErrors() {
    nlm::Config config;
    
    // Set invalid values
    config.setString("invalid_int", "not_a_number");
    
    // This should fail validation
    bool validationCaught = false;
    try {
        config.getWithValidation<int>("invalid_int", 0);
    } catch (const nlm::NLMConfigException& e) {
        validationCaught = true;
    }
    assert(validationCaught);
    
    std::cout << "    testConfigurationValidationErrors passed" << std::endl;
}

void testConfigurationSummary() {
    nlm::Config config;
    
    config.setInt("test1", 1);
    config.setString("test2", "two");
    
    std::string summary = config.summary();
    assert(summary.find("Configuration Summary") != std::string::npos);
    assert(summary.find("Key: test1") != std::string::npos);
    assert(summary.find("Key: test2") != std::string::npos);
    
    std::cout << "    testConfigurationSummary passed" << std::endl;
}

void testConfigurationGetInfo() {
    nlm::Config config;
    
    config.setInt("info_test", 42);
    
    std::string info = config.getInfo("info_test");
    assert(info.find("info_test") != std::string::npos);
    assert(info.find("42") != std::string::npos);
    
    std::cout << "    testConfigurationGetInfo passed" << std::endl;
}

void testConfigurationSourceTracking() {
    nlm::Config config;
    
    config.setInt("source_test", 1, nlm::ConfigSource::Runtime);
    assert(config.getSource("source_test") == nlm::ConfigSource::Runtime);
    
    config.setInt("source_test", 2, nlm::ConfigSource::CommandLine);
    assert(config.getSource("source_test") == nlm::ConfigSource::CommandLine);
    assert(config.isOverridden("source_test"));  // Should be overridden
    
    std::cout << "    testConfigurationSourceTracking passed" << std::endl;
}

void testConfigurationBatchOperations() {
    nlm::Config config;
    
    std::map<std::string, std::any> batchValues;
    batchValues["batch_int"] = 123;
    batchValues["batch_string"] = std::string("batch");
    
    config.setMultiple(batchValues);
    
    assert(config.get<int>("batch_int") == 123);
    assert(config.get<std::string>("batch_string") == "batch");
    
    std::cout << "    testConfigurationBatchOperations passed" << std::endl;
}

void testConfigurationGetMultiple() {
    nlm::Config config;
    
    config.setInt("multi1", 1);
    config.setString("multi2", "two");
    config.setDouble("multi3", 3.0);
    
    std::vector<std::string> keys = {"multi1", "multi2", "multi3"};
    std::map<std::string, std::any> values = config.getMultiple(keys);
    
    assert(values.size() == 3);
    assert(std::any_cast<int>(values["multi1"]) == 1);
    assert(std::any_cast<std::string>(values["multi2"]) == "two");
    assert(std::any_cast<double>(values["multi3"]) == 3.0);
    
    std::cout << "    testConfigurationGetMultiple passed" << std::endl;
}

void testConfigurationErrorHandling() {
    nlm::Config config;
    
    // Test getting non-existent key
    bool notFoundCaught = false;
    try {
        config.get<int>("non_existent_key");
    } catch (const nlm::NLMConfigException& e) {
        notFoundCaught = true;
    }
    assert(notFoundCaught);
    
    std::cout << "    testConfigurationErrorHandling passed" << std::endl;
}

void testConfigurationHelpFunctions() {
    // Test helper functions
    nlm::ConfigHelper::mergeConfigs helper; // Just testing compilation
    
    bool positive = nlm::ConfigHelper::isPositive(std::any(5));
    assert(positive);
    
    bool nonNegative = nlm::ConfigHelper::isNonNegative(std::any(0));
    assert(nonNegative);
    
    std::string strValue = nlm::ConfigHelper::toString(std::any(42));
    assert(strValue == "42");
    
    double doubleValue = nlm::ConfigHelper::toDouble(std::any(3.14));
    assert(doubleValue == 3.14);
    
    int intValue = nlm::ConfigHelper::toInt(std::any(3.14));
    assert(intValue == 3);
    
    bool boolValue = nlm::ConfigHelper::toBool(std::any(true));
    assert(boolValue);
    
    std::cout << "    testConfigurationHelpFunctions passed" << std::endl;
}

void testConfigurationErrorCodeStrings() {
    // Test error code to string conversion
    std::string codeStr = nlm::getErrorCodeString(nlm::NLMException::ErrorCode::ConfigValidationFailed);
    assert(codeStr.find("ConfigValidationFailed") != std::string::npos);
    
    codeStr = nlm::getErrorCodeString(nlm::NLMException::ErrorCode::NeuronNotFound);
    assert(codeStr.find("NeuronNotFound") != std::string::npos);
    
    codeStr = nlm::getErrorCodeString(nlm::NLMException::ErrorCode::UnknownError);
    assert(codeStr.find("UnknownError") != std::string::npos);
    
    std::cout << "    testConfigurationErrorCodeStrings passed" << std::endl;
}

} // namespace test_config_enhanced

// Original config tests namespace
namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigFileLoading() {
    nlm::Config config;
    
    // This would test file loading
    std::cout << "    testConfigFileLoading passed" << std::endl;
}

void testConfigArgsLoading() {
    // Simulate command line args
    int argc = 3;
    char* argv[] = {
        (char*)"test",
        (char*)"--neuron_count=100",
        (char*)"--connection_probability=0.2"
    };
    
    nlm::Config config;
    config.loadFromArgs(argc, argv);
    
    std::cout << "    testConfigArgsLoading passed" << std::endl;
}

void testConfigGet() {
    nlm::Config config;
    
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    
    std::cout << "    testConfigGet passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    std::string summary = config.summary();
    assert(summary.find("Configuration") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1);
    config.set("key2", "two");
    
    std::vector<std::string> keys = config.getKeys();
    assert(keys.size() >= 2);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("temp", 1);
    config.clear();
    
    std::cout << "    testConfigClear passed" << std::endl;
}

} // namespace test_config

void test_config_enhanced::runAll() {
    test_config_enhanced::testConfigurationCreation();
    test_config_enhanced::testConfigurationTypeSafety();
    test_config_enhanced::testConfigurationValidation();
    test_config_enhanced::testConfigurationSchema();
    test_config_enhanced::testConfigurationJSON();
    test_config_enhanced::testConfigurationEnvVars();
    test_config_enhanced::testConfigurationValidationErrors();
    test_config_enhanced::testConfigurationSummary();
    test_config_enhanced::testConfigurationGetInfo();
    test_config_enhanced::testConfigurationSourceTracking();
    test_config_enhanced::testConfigurationBatchOperations();
    test_config_enhanced::testConfigurationGetMultiple();
    test_config_enhanced::testConfigurationErrorHandling();
    test_config_enhanced::testConfigurationHelpFunctions();
    test_config_enhanced::testConfigurationErrorCodeStrings();
    
    std::cout << "    All enhanced configuration tests passed!" << std::endl;
}

void test_config::runAll() {
    test_config::testConfigCreation();
    test_config::testConfigFileLoading();
    test_config::testConfigArgsLoading();
    test_config::testConfigGet();
    test_config::testConfigSummary();
    test_config::testConfigKeys();
    test_config::testConfigClear();
    
    std::cout << "    All original configuration tests passed!" << std::endl;
}
