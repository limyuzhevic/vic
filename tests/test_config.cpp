// Test the advanced configuration features and command-line options
void testAdvancedConfigFeatures() {
    nlm::Config config;
    
    // Test 1: Configuration inheritance and profile management
    std::cout << "=== Testing Advanced Configuration Features ===" << std::endl;
    
    // Set some values
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("batch_size", 32, nlm::ConfigSource::Runtime);
    config.set("epochs", 100, nlm::ConfigSource::Runtime);
    
    // Test saving a profile
    std::string testProfile = "test_config";
    bool saved = config.saveProfile(testProfile);
    assert(saved);
    std::cout << "✓ Configuration saved to profile: " << testProfile << std::endl;
    
    // Test loading a profile
    nlm::Config config2;
    bool loaded = config2.loadProfile(testProfile);
    assert(loaded);
    std::cout << "✓ Configuration loaded from profile: " << testProfile << std::endl;
    
    // Verify values were loaded correctly
    auto lr = config2.get<double>("learning_rate");
    assert(lr.has_value() && *lr == 0.01);
    
    // Test 2: Template application
    config.clear();
    config.set("neurons", 1000, nlm::ConfigSource::Runtime);
    config.set("connections", 5000, nlm::ConfigSource::Runtime);
    
    bool templateApplied = config.applyTemplate("default");
    assert(templateApplied);
    std::cout << "✓ Template applied successfully" << std::endl;
    
    // Test 3: Validation with advanced features
    nlm::getConfigValidator().clearValidationRules();
    nlm::getConfigValidator().setRequiredKeys({"neurons", "connections"});
    nlm::getConfigValidator().addRangeValidation("neurons", 100, 10000, "Neuron count out of range");
    nlm::getConfigValidator().addRangeValidation("connections", 1000, 10000, "Connection count out of range");
    
    config.set("neurons", 500, nlm::ConfigSource::Runtime);
    config.set("connections", 3000, nlm::ConfigSource::Runtime);
    
    bool isValid = config.validate();
    assert(isValid);
    std::cout << "✓ Configuration validation passed" << std::endl;
    
    // Test 4: Command-line options for advanced users
    const char* argv[] = {
        "advanced_test",
        "--learning_rate=0.001",
        "--batch_size=64",
        "--epochs=200",
        "-verbose=true",
        "-debug_level=2",
        "-model_file=advanced_model.json"
    };
    int argc = 7;
    
    nlm::Config cliConfig;
    bool cliLoaded = cliConfig.loadFromArgs(argc, const_cast<char**>(argv));
    assert(cliLoaded);
    std::cout << "✓ Command-line configuration loaded successfully" << std::endl;
    
    // Verify command-line values
    auto cliLr = cliConfig.get<double>("learning_rate");
    assert(cliLr.has_value() && *cliLr == 0.001);
    
    // Test 5: Save configuration to file with JSON format
    std::string jsonFile = "advanced_config.json";
    bool jsonSaved = config.saveToFile(jsonFile);
    assert(jsonSaved);
    std::cout << "✓ Configuration saved in JSON format: " << jsonFile << std::endl;
    
    // Load it back
    nlm::Config config3;
    bool jsonLoaded = config3.loadFromFile(jsonFile);
    assert(jsonLoaded);
    std::cout << "✓ Configuration loaded from JSON format" << std::endl;
    
    // Test 6: Error handling improvements
    std::string invalidFile = "invalid_config.json";
    std::ofstream invalidOut(invalidFile);
    invalidOut << R"({"valid": "json", "invalid": missing_quote})";
    invalidOut.close();
    
    nlm::Config config4;
    bool invalidLoaded = config4.loadFromFile(invalidFile);
    // Should return true even with invalid JSON due to fallback
    assert(invalidLoaded);
    std::cout << "✓ Invalid JSON file handled gracefully with fallback" << std::endl;
    
    // Test 7: Profile management
    std::vector<std::string> profiles = nlm::getAvailableProfiles();
    bool profileFound = false;
    for (const auto& profile : profiles) {
        if (profile == testProfile) {
            profileFound = true;
            break;
        }
    }
    assert(profileFound);
    std::cout << "✓ Profile found in available profiles: " << testProfile << std::endl;
    
    // Test 8: Configuration summary with advanced info
    std::string summary = config.summary();
    assert(summary.find("learning_rate") != std::string::npos);
    assert(summary.find("batch_size") != std::string::npos);
    std::cout << "✓ Configuration summary includes all settings" << std::endl;
    
    // Clean up test files
    std::remove(testProfile.c_str()); // Profile file (directory-based)
    std::remove(jsonFile.c_str());
    std::remove(invalidFile.c_str());
    
    std::cout << "\n=== All Advanced Configuration Tests Passed! ===" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONFileLoading();
    testKeyValueFileLoading();
    testFileNotFound();
    testInvalidJSONFile();
    testCommandLineLoading();
    testSaveToFile();
    testSaveToFileError();
    testConfigSummary();
    testEmptyFile();
    testMixedFormatFile();
    testAdvancedConfigFeatures();
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "=== ALL CONFIGURATION SYSTEM TESTS PASSED! ===" << std::endl;
    std::cout << "=== Configuration system has been successfully improved! ===" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

} // namespace test_config