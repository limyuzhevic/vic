#include "core/Config/Config.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void testEnhancedConfigFeatures() {
    std::cout << "=== Testing Enhanced Configuration System ===\n";
    
    nlm::Config config;
    
    // Test 1: Basic type inference and setting
    std::cout << "Test 1: Basic type inference\n";
    config.legacySet("string_value", "hello", nlm::ConfigSource::Runtime);
    config.legacySet("int_value", 42, nlm::ConfigSource::Runtime);
    config.legacySet("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.legacySet("bool_value", true, nlm::ConfigSource::Runtime);
    config.legacySet("bool_false", false, nlm::ConfigSource::Runtime);
    
    // Test 2: Type inference from strings
    std::cout << "Test 2: Type inference from strings\n";
    config.legacySet("auto_bool_true", "true", nlm::ConfigSource::Runtime);
    config.legacySet("auto_bool_false", "0", nlm::ConfigSource::Runtime);
    config.legacySet("auto_int", "123", nlm::ConfigSource::Runtime);
    config.legacySet("auto_double", "45.67", nlm::ConfigSource::Runtime);
    
    // Test 3: Type safety with get()
    std::cout << "Test 3: Type safety with get()\n";
    std::string str;
    if (config.legacyGet("string_value", str).isSuccess()) {
        std::cout << "  Got string: " << str << std::endl;
    }
    
    int intVal;
    if (config.legacyGet("int_value", intVal).isSuccess()) {
        std::cout << "  Got int: " << intVal << std::endl;
    }
    
    double doubleVal;
    if (config.legacyGet("double_value", doubleVal).isSuccess()) {
        std::cout << "  Got double: " << doubleVal << std::endl;
    }
    
    bool boolVal;
    if (config.legacyGet("bool_value", boolVal).isSuccess()) {
        std::cout << "  Got bool: " << boolVal << std::endl;
    }
    
    // Test 4: Thread safety
    std::cout << "Test 4: Thread safety\n";
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&config, i]() {
            config.legacySet("thread_key", "thread_value", nlm::ConfigSource::Runtime);
            std::string value = config.legacyGetOr<std::string>("thread_key", "default");
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "  Thread safety test completed\n";
    
    // Test 5: Configuration validation
    std::cout << "Test 5: Configuration validation\n";
    ConfigResult validationResult = config.validate();
    if (validationResult.isSuccess()) {
        std::cout << "  Configuration is valid\n";
    } else {
        std::cout << "  Validation error: " << validationResult.getMessage() << std::endl;
    }
    
    // Test 6: Configuration summary
    std::cout << "Test 6: Configuration summary\n";
    std::string summary = config.legacySummary();
    std::cout << "  " << summary.substr(0, std::min<size_t>(200, summary.size())) << "...\n";
    
    // Test 7: Error handling
    std::cout << "Test 7: Error handling\n";
    ConfigResult errorResult = config.legacyGet<std::string>("nonexistent_key", str);
    if (errorResult.isFailure()) {
        std::cout << "  Expected error: " << errorResult.getMessage() << std::endl;
    }
    
    // Test 8: JSON export
    std::cout << "Test 8: JSON export\n";
    ConfigResult jsonResult = config.legacySaveToFile("test_config.json");
    if (jsonResult.isSuccess()) {
        std::cout << "  JSON exported successfully\n";
    }
    
    // Cleanup
    std::remove("test_config.json");
    
    std::cout << "\n=== All tests completed ===\n";
}

int main() {
    testEnhancedConfigFeatures();
    return 0;
}