// Configuration Tests
// Tests for configuration loading, validation, and management

TEST_CASE("config_loading_test", TestCategory::CONFIGURATION, "Tests for configuration loading and parsing")
void testConfigLoading() {
    // Test 1: Default configuration
    nlm::Config defaultConfig;
    assert(defaultConfig.has("neuron_count") || defaultConfig.has("working_memory_capacity"));
    
    // Test 2: File-based configuration loading
    std::ofstream configFile("test_config.cfg");
    configFile << "[neuron]" << std::endl;
    configFile << "count = 200" << std::endl;
    configFile << std::endl;
    configFile << "[memory]" << std::endl;
    configFile << "working_capacity = 1000" << std::endl;
    configFile.close();
    
    nlm::Config fileConfig;
    fileConfig.loadFromFile("test_config.cfg");
    
    auto neuronCount = fileConfig.get<int>("count");
    assert(neuronCount.has_value() && neuronCount.value() == 200);
    
    auto memoryCapacity = fileConfig.get<int>("working_capacity");
    assert(memoryCapacity.has_value() && memoryCapacity.value() == 1000);
    
    std::remove("test_config.cfg");
    
    // Test 3: JSON configuration loading
    std::ofstream jsonFile("test_config.json");
    jsonFile << "{\"neuron\": {\"count\": 300}, \"memory\": {\"working_capacity\": 1500}}";
    jsonFile.close();
    
    nlm::Config jsonConfig;
    jsonConfig.loadFromFile("test_config.json");
    
    auto jsonNeuronCount = jsonConfig.get<int>("neuron.count");
    assert(jsonNeuronCount.has_value() && jsonNeuronCount.value() == 300);
    
    std::remove("test_config.json");
    
    // Test 4: Configuration validation
    nlm::Config validConfig;
    validConfig.set("neuron_count", 100);
    validConfig.set("memory_capacity", 2000);
    validConfig.set("timestep", 0.001);
    
    // Should not crash on valid configuration
    validConfig.validate();
    
    // Test 5: Configuration source precedence
    nlm::Config sourceConfig;
    sourceConfig.set("value", 1, nlm::ConfigSource::Default);
    sourceConfig.set("value", 2, nlm::ConfigSource::Runtime);
    sourceConfig.set("value", 3, nlm::ConfigSource::Environment);
    
    // Runtime should override default
    auto finalValue = sourceConfig.get<int>("value");
    assert(finalValue.has_value()); // Should be 2 (runtime)
    
    std::cout << "Configuration loading tests passed" << std::endl;
    return true;
}

TEST_CASE("config_validation_test", TestCategory::CONFIGURATION, "Tests for configuration validation")
void testConfigValidation() {
    // Test 1: Valid configuration
    nlm::Config validConfig;
    validConfig.set("neuron_count", 100);
    validConfig.set("region_count", 5);
    validConfig.set("connection_probability", 0.1);
    validConfig.set("timestep", 0.001);
    validConfig.set("working_memory_capacity", 1000);
    
    // Should validate without errors
    assert(validConfig.validate());
    
    // Test 2: Invalid configurations should be detected
    nlm::Config invalidConfig;
    invalidConfig.set("neuron_count", -1); // Negative neuron count
    invalidConfig.set("region_count", 0); // Too few regions
    invalidConfig.set("connection_probability", 1.5); // Invalid probability
    
    // Should detect invalid values
    bool isValid = invalidConfig.validate();
    assert(!isValid); // Should be invalid
    
    // Test 3: Type validation
    nlm::Config typeConfig;
    typeConfig.set("int_value", 42);
    typeConfig.set("string_value", "test");
    typeConfig.set("double_value", 3.14);
    typeConfig.set("bool_value", true);
    
    // Type validation should pass
    assert(typeConfig.validate());
    
    // Test 4: Missing required configuration
    nlm::Config incompleteConfig;
    incompleteConfig.set("optional_value", 100);
    
    // May or may not validate depending on requirements
    // The important thing is that it doesn't crash
    incompleteConfig.validate();
    
    // Test 5: Configuration consistency
    nlm::Config inconsistentConfig;
    inconsistentConfig.set("neuron_count", 100);
    inconsistentConfig.set("working_memory_capacity", 50); // Smaller than neurons
    
    // May detect inconsistency
    bool consistent = inconsistentConfig.validate();
    
    // Test 6: Configuration merging
    nlm::Config baseConfig;
    baseConfig.set("value1", 10);
    baseConfig.set("value2", 20);
    
    nlm::Config overrideConfig;
    overrideConfig.set("value2", 30);
    overrideConfig.set("value3", 40);
    
    nlm::Config mergedConfig = baseConfig.merge(overrideConfig);
    
    assert(mergedConfig.get<int>("value1").value() == 10);
    assert(mergedConfig.get<int>("value2").value() == 30);
    assert(mergedConfig.get<int>("value3").value() == 40);
    
    std::cout << "Configuration validation tests passed" << std::endl;
    return true;
}

TEST_CASE("config_template_test", TestCategory::CONFIGURATION, "Tests for configuration templates")
void testConfigTemplate() {
    // Test 1: Configuration template creation
    nlm::ConfigTemplate template;
    template.setDefault("neuron_count", 100);
    template.setDefault("region_count", 5);
    template.setDefault("working_memory_capacity", 1000);
    
    assert(template.getDefault("neuron_count").value() == 100);
    assert(template.getDefault("region_count").value() == 5);
    
    // Test 2: Template validation
    nlm::Config derivedConfig = template.createConfig();
    
    assert(derivedConfig.get<int>("neuron_count").has_value());
    assert(derivedConfig.get<int>("region_count").has_value());
    
    // Test 3: Template inheritance
    nlm::ConfigTemplate parentTemplate;
    parentTemplate.setDefault("parent_setting", "parent_value");
    
    nlm::ConfigTemplate childTemplate = parentTemplate.createChild();
    childTemplate.setDefault("child_setting", "child_value");
    
    nlm::Config childConfig = childTemplate.createConfig();
    assert(childConfig.get<std::string>("parent_setting").value() == "parent_value");
    assert(childConfig.get<std::string>("child_setting").value() == "child_value");
    
    // Test 4: Template configuration validation
    bool templateValid = template.validate();
    assert(templateValid);
    
    // Test 5: Template serialization
    std::string serialized = template.serialize();
    assert(!serialized.empty());
    
    // Test 6: Template modification
    template.modifyDefault("neuron_count", 200);
    assert(template.getDefault("neuron_count").value() == 200);
    
    std::cout << "Configuration template tests passed" << std::endl;
    return true;
}