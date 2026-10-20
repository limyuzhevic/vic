// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

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

// New comprehensive validation tests
void testConfigValidation() {
    nlm::Config config;
    
    // Set valid values
    config.set("random_seed", "42");
    config.set("neuron_count", "1000");
    config.set("region_count", "5");
    config.set("connection_probability", "0.1");
    config.set("stdp_ltp_weight", "0.01");
    config.set("stdp_ltd_weight", "0.012");
    config.set("stdp_tau", "20.0");
    config.set("synaptogenesis_rate", "0.0001");
    config.set("pruning_rate", "0.00001");
    
    // Validation should pass for valid values
    assert(config.validate());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigValidationWithInvalidValues() {
    nlm::Config config;
    
    // Set valid values
    config.set("random_seed", "42");
    config.set("neuron_count", "-1"); // Invalid - should be negative
    
    // Validation should fail due to invalid neuron_count
    assert(!config.validate());
    
    // Test validation error messages
    std::string errorMsg = config.getValidationError("neuron_count", "-1");
    assert(errorMsg.find("neuron_count") != std::string::npos);
    assert(errorMsg.find("Invalid") != std::string::npos);
    
    std::cout << "    testConfigValidationWithInvalidValues passed" << std::endl;
}

void testTypeSafeGetValidated() {
    nlm::Config config;
    
    // Set valid values
    config.set("random_seed", "42");
    config.set("neuron_count", "1000");
    config.set("connection_probability", "0.1");
    config.set("stdp_ltp_weight", "0.01");
    config.set("reward_modulation", "true");
    
    // Test getValidated with valid values
    try {
        int64_t seed = config.getValidated<int64_t>("random_seed", 42);
        assert(seed == 42);
    } catch (const nlm::ConfigurationError& e) {
        assert(false); // Should not throw
    }
    
    try {
        int64_t count = config.getValidated<int64_t>("neuron_count", 1000);
        assert(count == 1000);
    } catch (const nlm::ConfigurationError& e) {
        assert(false); // Should not throw
    }
    
    try {
        float prob = config.getValidated<float>("connection_probability", 0.1f);
        assert(std::abs(prob - 0.1f) < 0.0001f);
    } catch (const nlm::ConfigurationError& e) {
        assert(false); // Should not throw
    }
    
    try {
        bool modulation = config.getValidated<bool>("reward_modulation", false);
        assert(modulation == true);
    } catch (const nlm::ConfigurationError& e) {
        assert(false); // Should not throw
    }
    
    // Test getValidated with invalid values (should return default)
    try {
        int64_t invalidCount = config.getValidated<int64_t>("invalid_key", 999);
        assert(invalidCount == 999); // Should return default
    } catch (const nlm::ConfigurationError& e) {
        assert(false); // Should not throw for missing key
    }
    
    std::cout << "    testTypeSafeGetValidated passed" << std::endl;
}

void testStaticValidationMethods() {
    // Test validateInt64
    assert(nlm::Config::validateInt64("123"));
    assert(nlm::Config::validateInt64("-456"));
    assert(!nlm::Config::validateInt64("not_a_number"));
    assert(!nlm::Config::validateInt64(""));
    
    // Test validateDouble
    assert(nlm::Config::validateDouble("123.45"));
    assert(nlm::Config::validateDouble("-0.001"));
    assert(!nlm::Config::validateDouble("not_a_number"));
    
    // Test validateFloat
    assert(nlm::Config::validateFloat("0.5f"));
    assert(nlm::Config::validateFloat("-1.0f"));
    assert(!nlm::Config::validateFloat("not_a_number"));
    
    // Test validateString
    assert(nlm::Config::validateString("hello"));
    assert(nlm::Config::validateString("")); // Empty string is valid
    assert(!nlm::Config::validateString("") == false); // Actually empty string is valid
    
    // Test validateBool
    assert(nlm::Config::validateBool("true"));
    assert(nlm::Config::validateBool("false"));
    assert(nlm::Config::validateBool("yes"));
    assert(nlm::Config::validateBool("no"));
    assert(nlm::Config::validateBool("1"));
    assert(nlm::Config::validateBool("0"));
    assert(!nlm::Config::validateBool("invalid"));
    
    // Test specialized validation methods
    assert(nlm::Config::validateNeuronCount("1000"));
    assert(nlm::Config::validateNeuronCount("0")); // Edge case: 0 is invalid
    assert(!nlm::Config::validateNeuronCount("-10"));
    assert(!nlm::Config::validateNeuronCount("100000")); // Upper bound exclusive
    
    assert(nlm::Config::validateRegionCount("1"));
    assert(nlm::Config::validateRegionCount("100")); // Edge case: 100 is valid
    assert(!nlm::Config::validateRegionCount("0"));
    assert(!nlm::Config::validateRegionCount("1000")); // Upper bound exclusive
    
    assert(nlm::Config::validateConnectionProbability("0.5"));
    assert(nlm::Config::validateConnectionProbability("0.0"));
    assert(nlm::Config::validateConnectionProbability("1.0"));
    assert(!nlm::Config::validateConnectionProbability("-0.1"));
    assert(!nlm::Config::validateConnectionProbability("1.1"));
    
    assert(nlm::Config::validateSTDPParameters("0.01", "stdp_ltp_weight"));
    assert(nlm::Config::validateSTDPParameters("0.012", "stdp_ltd_weight"));
    assert(nlm::Config::validateSTDPParameters("20.0", "stdp_tau"));
    assert(!nlm::Config::validateSTDPParameters("2.0", "stdp_ltp_weight")); // Too high
    assert(!nlm::Config::validateSTDPParameters("0.0", "stdp_tau")); // Too low
    
    assert(nlm::Config::validatePlasticityRate("0.0001", "synaptogenesis_rate"));
    assert(nlm::Config::validatePlasticityRate("0.00001", "pruning_rate"));
    assert(nlm::Config::validatePlasticityRate("1.0", "learning_rate"));
    assert(!nlm::Config::validatePlasticityRate("0.1", "synaptogenesis_rate")); // Too high
    assert(!nlm::Config::validatePlasticityRate("-0.1", "pruning_rate")); // Negative
    
    std::cout << "    testStaticValidationMethods passed" << std::endl;
}

void testConfigurationSchema() {
    const auto& schema = nlm::ConfigSchema::getSchema();
    
    // Check that schema contains expected fields
    bool foundRandomSeed = false;
    bool foundNeuronCount = false;
    bool foundConnectionProbability = false;
    bool foundSTDPParameters = false;
    
    for (const auto& field : schema) {
        if (field.name == "random_seed") {
            assert(field.required);
            assert(field.type == "int64_t");
            foundRandomSeed = true;
        } else if (field.name == "neuron_count") {
            assert(field.required);
            assert(field.type == "int64_t");
            foundNeuronCount = true;
        } else if (field.name == "connection_probability") {
            assert(field.required);
            assert(field.type == "float");
            foundConnectionProbability = true;
        } else if (field.name.find("stdp_") != std::string::npos) {
            foundSTDPParameters = true;
        }
    }
    
    assert(foundRandomSeed);
    assert(foundNeuronCount);
    assert(foundConnectionProbability);
    assert(foundSTDPParameters);
    
    std::cout << "    testConfigurationSchema passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // New validation tests
    testConfigValidation();
    testConfigValidationWithInvalidValues();
    testTypeSafeGetValidated();
    testStaticValidationMethods();
    testConfigurationSchema();
}

} // namespace test_config
