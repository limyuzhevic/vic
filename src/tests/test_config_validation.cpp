# Simple configuration validation test
void testConfigurationValidation() {
    NLM_LOG_INFO("=== Testing Configuration Validation ===");
    
    // Create a config object
    Config config;
    
    // Test 1: Load default config
    auto defaults = Config::getDefaultConfig();
    NLM_LOG_INFO("Default config has " + std::to_string(defaults.size()) + " keys");
    
    // Test 2: Set some values
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "Hello World");
    config.set("test_bool", true);
    
    // Test 3: Get values
    auto intVal = config.get<int64_t>("test_int");
    auto doubleVal = config.get<double>("test_double");
    auto stringVal = config.get<std::string>("test_string");
    auto boolVal = config.get<bool>("test_bool");
    
    if (intVal.has_value()) NLM_LOG_INFO("Retrieved int: " + std::to_string(intVal.value()));
    if (doubleVal.has_value()) NLM_LOG_INFO("Retrieved double: " + std::to_string(doubleVal.value()));
    if (stringVal.has_value()) NLM_LOG_INFO("Retrieved string: " + stringVal.value());
    if (boolVal.has_value()) NLM_LOG_INFO("Retrieved bool: " + std::to_string(boolVal.value()));
    
    // Test 4: Validation (currently has many required keys)
    bool isValid = config.validate();
    NLM_LOG_INFO("Configuration validation: " + std::string(isValid ? "PASSED" : "FAILED"));
    
    // Test 5: Test file loading
    config.clear();
    bool loaded = config.loadFromFile("configs/default.cfg");
    NLM_LOG_INFO("Loaded config file: " + std::string(loaded ? "SUCCESS" : "FAILED"));
    
    // Test 6: Test file saving
    if (loaded) {
        config.saveToJSONFile("test_output.json");
        NLM_LOG_INFO("Saved config to JSON file");
    }
    
    NLM_LOG_INFO("=== Configuration Validation Tests Complete ===");
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Configuration Validation Test...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run configuration validation test
    testConfigurationValidation();
    
    NLM_LOG_INFO("\nConfiguration validation testing complete!");
    
    return 0;
}