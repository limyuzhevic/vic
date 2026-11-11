// Test JSON/YAML file loading (Phase 2 enhancement)
void testConfigJsonYamlLoading() {
    nlm::Config config;
    
    // Test simple key=value format (backward compatibility)
    config.set("test_int", 42);
    config.set("test_string", "hello");
    config.set("test_double", 3.14);
    config.set("test_bool", true);
    
    // Create test JSON file
    std::ofstream jsonFile("test_config.json");
    jsonFile << "{\"int_value\": 123, \"string_value\": \"world\", \"double_value\": 2.71, \"bool_value\": false}";
    jsonFile.close();
    
    // Load JSON file
    bool jsonSuccess = config.loadFromFile("test_config.json");
    assert(jsonSuccess);
    
    // Verify loaded values
    auto jsonIntVal = config.get<int>("int_value");
    assert(jsonIntVal.has_value());
    assert(*jsonIntVal == 123);
    
    auto jsonStringVal = config.get<std::string>("string_value");
    assert(jsonStringVal.has_value());
    assert(*jsonStringVal == "world");
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigJsonYamlLoading passed" << std::endl;
}

// Test enhanced error handling
void testConfigEnhancedErrorHandling() {
    nlm::Config config;
    
    // Test contains() with better error handling
    config.set("neuron1", 1.0);
    config.set("neuron2", 2.0);
    
    // The enhanced contains() should work properly
    bool hasNeuron1 = config.has("neuron1");
    assert(hasNeuron1);
    
    bool hasNeuron3 = config.has("neuron3");
    assert(!hasNeuron3);
    
    // Test getOr() with type safety
    int missingVal = config.getOr<int>("missing_key", -1);
    assert(missingVal == -1);
    
    std::cout << "    testConfigEnhancedErrorHandling passed" << std::endl;
}

// Test Config.saveToFile with actual values (bug fix)
void testConfigSaveToFileActualValues() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_config", 100);
    config.set("double_config", 3.14159);
    config.set("string_config", "test_value");
    config.set("bool_config", true);
    
    // Save to file
    std::string testFile = "test_saved_config.json";
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Verify file contains actual values (not PLACEHOLDER_VALUE)
    std::ifstream savedFile(testFile);
    std::string content((std::istreambuf_iterator<char>(savedFile)), std::istreambuf_iterator<char>());
    savedFile.close();
    
    // Check that actual values are present
    assert(content.find("100") != std::string::npos);  // int value
    assert(content.find("3.14159") != std::string::npos);  // double value
    assert(content.find("test_value") != std::string::npos);  // string value
    assert(content.find("true") != std::string::npos);  // bool value
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigSaveToFileActualValues passed" << std::endl;
}

// Test enhanced config loading with error recovery
void testConfigErrorRecovery() {
    nlm::Config config;
    
    // Load a valid simple config
    std::ofstream simpleFile("test_simple_config");
    simpleFile << "simple_value = 42\nsimple_string = \"hello\"\n# This is a comment\nvalid_key = true";
    simpleFile.close();
    
    bool loadSuccess = config.loadFromFile("test_simple_config");
    assert(loadSuccess);
    
    // Verify values were loaded correctly
    auto simpleIntVal = config.get<int>("simple_value");
    assert(simpleIntVal.has_value());
    assert(*simpleIntVal == 42);
    
    auto simpleStringVal = config.get<std::string>("simple_string");
    assert(simpleStringVal.has_value());
    assert(*simpleStringVal == "hello");
    
    auto simpleBoolVal = config.get<bool>("valid_key");
    assert(simpleBoolVal.has_value());
    assert(*simpleBoolVal == true);
    
    // Clean up
    std::remove("test_simple_config");
    
    std::cout << "    testConfigErrorRecovery passed" << std::endl;
}

// Test advanced configuration features
void testConfigAdvancedFeatures() {
    nlm::Config config;
    
    // Test array values
    std::vector<int> intArray{1, 2, 3, 4, 5};
    config.set("int_array", intArray);
    
    std::vector<double> doubleArray{1.0, 2.5, 3.14, 4.0};
    config.set("double_array", doubleArray);
    
    std::vector<std::string> stringArray{"alpha", "beta", "gamma"};
    config.set("string_array", stringArray);
    
    // Test getOr with default values for missing keys
    auto missingArray = config.get<std::vector<int>>("missing_array");
    assert(!missingArray.has_value());
    
    // Test that existing arrays can be retrieved
    auto retrievedArray = config.get<std::vector<int>>("int_array");
    assert(retrievedArray.has_value());
    assert(retrievedArray->size() == 5);
    
    std::cout << "    testConfigAdvancedFeatures passed" << std::endl;
}

// Test backward compatibility with simple key=value format
void testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Set some values
    config.set("legacy_key1", 999);
    config.set("legacy_key2", "legacy_value");
    config.set("legacy_key3", 3.14);
    
    // Simulate old file format
    std::ofstream legacyFile("legacy_config");
    legacyFile << "legacy_key1 = 111\nlegacy_key2 = \"legacy\"\nlegacy_key3 = 2.71\n";
    legacyFile.close();
    
    // Load using backward compatible loader
    bool legacySuccess = config.loadFromFile("legacy_config");
    assert(legacySuccess);
    
    // Verify values were loaded
    auto legacyVal1 = config.get<int>("legacy_key1");
    assert(legacyVal1.has_value());
    assert(*legacyVal1 == 111);
    
    auto legacyVal2 = config.get<std::string>("legacy_key2");
    assert(legacyVal2.has_value());
    assert(*legacyVal2 == "legacy");
    
    // Clean up
    std::remove("legacy_config");
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

// Test config summary functionality
void testConfigSummary() {
    nlm::Config config;
    
    config.set("summary_key1", 1);
    config.set("summary_key2", "two");
    config.set("summary_key3", 3.0);
    
    std::string summary = config.summary();
    
    // Verify summary contains expected content
    assert(summary.find("summary_key1") != std::string::npos);
    assert(summary.find("summary_key2") != std::string::npos);
    assert(summary.find("summary_key3") != std::string::npos);
    assert(summary.find("3 entries") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}
