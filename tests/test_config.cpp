// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <any>

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

// Enhanced tests
void testEnhancedSchema() {
    nlm::Config config;
    
    // Test schema is initialized
    assert(config.getSchema() != nullptr);
    
    // Test getting groups
    auto groups = config.getGroups();
    assert(!groups.empty());
    assert(std::find(groups.begin(), groups.end(), "brain") != groups.end());
    assert(std::find(groups.begin(), groups.end(), "plasticity") != groups.end());
    assert(std::find(groups.begin(), groups.end(), "neuromodulation") != groups.end());
    assert(std::find(groups.begin(), groups.end(), "development") != groups.end());
    
    // Test getting parameters in group
    auto brainParams = config.getParametersInGroup("brain");
    assert(!brainParams.empty());
    assert(std::find(brainParams.begin(), brainParams.end(), "brain.region_count") != brainParams.end());
    
    std::cout << "    testEnhancedSchema passed" << std::endl;
}

void testParameterGroups() {
    nlm::Config config;
    
    // Set parameters in different groups
    config.set("brain.region_count", 4);
    config.set("plasticity.stdp.learning_rate", 0.01);
    config.set("neuromodulation.reward.strength", 1.0);
    config.set("development.maturation.rate", 0.1);
    
    // Verify they are in correct groups
    auto brainParams = config.getParametersInGroup("brain");
    assert(std::find(brainParams.begin(), brainParams.end(), "brain.region_count") != brainParams.end());
    
    auto plasticityParams = config.getParametersInGroup("plasticity");
    assert(std::find(plasticityParams.begin(), plasticityParams.end(), "plasticity.stdp.learning_rate") != plasticityParams.end());
    
    auto neuromodulationParams = config.getParametersInGroup("neuromodulation");
    assert(std::find(neuromodulationParams.begin(), neuromodulationParams.end(), "neuromodulation.reward.strength") != neuromodulationParams.end());
    
    auto developmentParams = config.getParametersInGroup("development");
    assert(std::find(developmentParams.begin(), developmentParams.end(), "development.maturation.rate") != developmentParams.end());
    
    std::cout << "    testParameterGroups passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Set valid values
    config.set("brain.region_count", 4);
    config.set("brain.inhibitory_ratio", 0.2);
    
    // Test validation
    assert(config.isValid("brain.region_count"));
    assert(config.isValid("brain.inhibitory_ratio"));
    
    // Test with invalid value (if schema supports validation)
    // Note: The simple schema in test may not have range validation for all parameters
    
    std::cout << "    testValidation passed" << std::endl;
}

void testTemplateLoading() {
    nlm::Config config;
    
    // Test simple brain template
    assert(config.loadFromTemplate("simple_brain"));
    assert(config.has("brain.region_count"));
    assert(config.getOr<int>("brain.region_count", 0) == 4);
    assert(config.has("plasticity.stdp.learning_rate"));
    assert(config.getOr<double>("plasticity.stdp.learning_rate", 0.0) > 0.0);
    
    // Clear and test agent learning template
    config.clear();
    assert(config.loadFromTemplate("agent_learning"));
    assert(config.has("brain.region_count"));
    assert(config.getOr<int>("brain.region_count", 0) == 6);
    assert(config.has("plasticity.hebbian.rate"));
    
    // Clear and test research template
    config.clear();
    assert(config.loadFromTemplate("research"));
    assert(config.has("neuromodulation.novelty.sensitivity"));
    
    // Clear and test production template
    config.clear();
    assert(config.loadFromTemplate("production"));
    assert(config.has("performance.optimization_level"));
    assert(config.getOr<bool>("performance.use_vectorization", false));
    
    std::cout << "    testTemplateLoading passed" << std::endl;
}

void testAvailableTemplates() {
    std::vector<std::string> templates = nlm::Config::getAvailableTemplates();
    
    assert(std::find(templates.begin(), templates.end(), "simple_brain") != templates.end());
    assert(std::find(templates.begin(), templates.end(), "agent_learning") != templates.end());
    assert(std::find(templates.begin(), templates.end(), "research") != templates.end());
    assert(std::find(templates.begin(), templates.end(), "production") != templates.end());
    
    assert(templates.size() >= 4);
    
    std::cout << "    testAvailableTemplates passed" << std::endl;
}

void testParameterDescription() {
    nlm::Config config;
    
    // Get description for a known parameter
    std::string desc = config.getParameterDescription("brain.region_count");
    assert(!desc.empty());
    assert(desc.find("brain.region_count") != std::string::npos);
    
    // Get description for unknown parameter
    std::string unknownDesc = config.getParameterDescription("unknown.parameter");
    // Should return empty or default message
    
    std::cout << "    testParameterDescription passed" << std::endl;
}

void testEnhancedConfigValues() {
    nlm::Config config;
    
    // Test setting values using std::any
    config.set("test_int", std::any(42));
    assert(config.getOr<int>("test_int", 0) == 42);
    
    config.set("test_double", std::any(3.14));
    assert(config.getOr<double>("test_double", 0.0) == 3.14);
    
    config.set("test_string", std::any(std::string("hello")));
    assert(config.getOr<std::string>("test_string", "") == "hello");
    
    config.set("test_bool", std::any(true));
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Test vector values
    std::vector<int> intVector = {1, 2, 3, 4, 5};
    config.set("test_vector", std::any(intVector));
    
    auto retrievedVector = config.get<std::vector<int>>("test_vector");
    assert(retrievedVector.has_value());
    assert(retrievedVector.value() == intVector);
    
    std::cout << "    testEnhancedConfigValues passed" << std::endl;
}

void testValidationResult() {
    nlm::Config config;
    
    // Set some valid values
    config.set("brain.region_count", 4);
    
    // Get validation results
    auto validationResult = config.validate();
    assert(validationResult.isValid);
    
    // Test validation for specific key
    auto keyValidation = config.validate("brain.region_count");
    assert(keyValidation.isValid);
    
    // Get error messages
    std::string errors = config.getValidationErrors("brain.region_count");
    std::string warnings = config.getValidationWarnings("brain.region_count");
    
    std::cout << "    testValidationResult passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // Enhanced tests
    testEnhancedSchema();
    testParameterGroups();
    testValidation();
    testTemplateLoading();
    testAvailableTemplates();
    testParameterDescription();
    testEnhancedConfigValues();
    testValidationResult();
}

} // namespace test_config
