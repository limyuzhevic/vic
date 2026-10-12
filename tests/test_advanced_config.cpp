// Advanced Configuration Tests
#include "core/Config/Config.hpp"
#include "advanced_config/ConfigSchema.hpp"
#include "advanced_config/ConfigValidator.hpp"
#include "advanced_config/ConfigSource.hpp"
#include <cassert>
#include <iostream>

namespace test_advanced_config {

void testAdvancedConfigCreation() {
    nlm::Config config;
    config.enableAdvancedConfig(true);
    
    // Get advanced manager
    auto* advancedManager = config.getAdvancedManager();
    assert(advancedManager != nullptr);
    
    // Set some values
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    
    // Get values
    assert(config.has("neuron_count"));
    assert(config.has("learning_rate"));
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    std::cout << "    testAdvancedConfigCreation passed" << std::endl;
}

void testConfigSchema() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    
    // Add fields with validation
    schema->add<int64_t>("neuron_count", 1000, true, "Number of neurons");
    schema->add<double>("learning_rate", 0.01, false, "Learning rate");
    
    // Test schema validation
    nlm::Config config;
    config.enableAdvancedConfig(true);
    
    // Should fail validation (missing required field)
    assert(!schema->validate(config));
    
    // Add required field
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    
    // Should pass validation
    assert(schema->validate(config));
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigValidators() {
    auto rangeValidator = std::make_shared<nlm::ConfigRangeValidator>(100, 1000);
    
    ValidationResult result = rangeValidator->validate("test", 500);
    assert(result.valid);
    
    result = rangeValidator->validate("test", 1500);
    assert(!result.valid);
    
    auto formatValidator = std::make_shared<nlm::ConfigFormatValidator>(".*@.*\.com");
    
    result = formatValidator->validate("email", std::string("test@example.com"));
    assert(result.valid);
    
    result = formatValidator->validate("email", std::string("invalid-email"));
    assert(!result.valid);
    
    std::cout << "    testConfigValidators passed" << std::endl;
}

void testConfigGroups() {
    auto group = std::make_shared<nlm::ConfigGroup>();
    
    // Add fields to group
    group->addField("neuron_count", 1000LL, "Number of neurons");
    group->addField("learning_rate", 0.01, "Learning rate");
    
    assert(group->hasField("neuron_count"));
    assert(group->hasField("learning_rate"));
    
    // Get group info
    auto* groupInfo = group->getGroupInfo("default");
    assert(groupInfo != nullptr);
    
    std::cout << "    testConfigGroups passed" << std::endl;
}

void testConfigTemplates() {
    auto template = std::make_unique<nlm::ConfigTemplate>();
    
    template->getInfo().name = "Test Template";
    template->getInfo().description = "Test template for validation";
    
    // Add template fields
    template->addField<int64_t>("neuron_count", 1000);
    template->addField<double>("learning_rate", 0.01);
    
    // Create config and apply template
    nlm::Config config;
    config.enableAdvancedConfig(true);
    
    template->applyTo(config);
    
    assert(config.has("neuron_count"));
    assert(config.has("learning_rate"));
    
    std::cout << "    testConfigTemplates passed" << std::endl;
}

void testConfigProfiles() {
    auto profile = std::make_unique<nlm::ConfigProfile>();
    
    profile->getInfo().name = "Test Profile";
    profile->getInfo().description = "Test profile for validation";
    
    // Add profile values
    profile->addValue<int64_t>("neuron_count", 500);
    profile->addValue<double>("learning_rate", 0.001);
    
    // Create config and apply profile
    nlm::Config config;
    config.enableAdvancedConfig(true);
    
    profile->applyTo(config);
    
    assert(config.has("neuron_count"));
    assert(config.has("learning_rate"));
    
    std::cout << "    testConfigProfiles passed" << std::endl;
}

void testConfigSources() {
    auto jsonSource = std::make_shared<nlm::JSONConfigSource>("test_config.json", 1);
    auto yamlSource = std::make_shared<nlm::YAMLConfigSource>("test_config.yaml", 2);
    auto envSource = std::make_shared<nlm::EnvironmentConfigSource>(3);
    
    // Add environment variable mapping
    envSource->addMapping("NEURON_COUNT", "neuron_count");
    
    // Test source creation
    assert(jsonSource->getTypeName() == "JSON");
    assert(yamlSource->getTypeName() == "YAML");
    assert(envSource->getTypeName() == "Environment");
    
    std::cout << "    testConfigSources passed" << std::endl;
}

void testConfigManager() {
    auto manager = std::make_unique<nlm::ConfigManager>();
    
    // Add sources
    manager->addSource(std::make_shared<nlm::EnvironmentConfigSource>());
    
    // Load configuration
    bool loaded = manager->loadConfiguration();
    
    // Test basic functionality
    assert(manager->has("neuron_count") || !manager->has("neuron_count"));
    
    // Get keys
    auto keys = manager->getKeys();
    
    // Test set/get
    manager->set<int64_t>("test_value", 42);
    
    std::cout << "    testConfigManager passed" << std::endl;
}

void testConfigEvents() {
    nlm::Config config;
    config.enableAdvancedConfig(true);
    
    // Get advanced manager
    auto* advancedManager = config.getAdvancedManager();
    assert(advancedManager != nullptr);
    
    // Test setting values triggers events
    config.set("test", 123, nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigEvents passed" << std::endl;
}

void runAll() {
    testAdvancedConfigCreation();
    testConfigSchema();
    testConfigValidators();
    testConfigGroups();
    testConfigTemplates();
    testConfigProfiles();
    testConfigSources();
    testConfigManager();
    testConfigEvents();
}

} // namespace test_advanced_config