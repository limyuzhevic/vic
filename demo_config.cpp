// Simple demonstration of enhanced Config system
// This file can be compiled and run to demonstrate the new features

#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Enhanced Configuration System Demo ===" << std::endl;
    std::cout << std::endl;
    
    // 1. Basic Configuration
    std::cout << "1. Basic Configuration:" << std::endl;
    nlm::Config config;
    
    config.set("learning_rate", 0.01f);
    config.set("max_iterations", 1000);
    config.set("enable_logging", true);
    config.set("model_name", "neural_network");
    
    std::cout << "   Set values:" << std::endl;
    std::cout << "     - learning_rate: " << config.getOr<float>("learning_rate", 0.001f) << std::endl;
    std::cout << "     - max_iterations: " << config.getOr<int>("max_iterations", 100) << std::endl;
    std::cout << "     - enable_logging: " << (config.getOr<bool>("enable_logging", false) ? "true" : "false") << std::endl;
    std::cout << "     - model_name: " << config.getOr<std::string>("model_name", "default") << std::endl;
    std::cout << std::endl;
    
    // 2. Configuration Validation
    std::cout << "2. Configuration Validation:" << std::endl;
    
    nlm::ConfigSchema schema;
    schema.addField("learning_rate", 0.01f, "Learning rate", [](const nlm::ConfigValue& v) {
        return std::get<float>(v) > 0.0f && std::get<float>(v) < 1.0f;
    }, true);
    
    schema.addField("max_iterations", 1000, "Max iterations", [](const nlm::ConfigValue& v) {
        return std::get<int>(v) > 0;
    }, true);
    
    schema.addField("batch_size", 32, "Batch size (power of 2)", [](const nlm::ConfigValue& v) {
        int size = std::get<int>(v);
        return size > 0 && (size & (size - 1)) == 0;
    }, true);
    
    std::cout << "   Schema fields:" << std::endl;
    for (const auto& name : schema.getFieldNames()) {
        const auto* field = schema.getField(name);
        if (field) {
            std::cout << "     - " << field->name << " (required: " << (field->required ? "yes" : "no") << ")" << std::endl;
        }
    }
    
    config.set("batch_size", 64);
    
    if (config.validate(schema)) {
        std::cout << "   ✓ Configuration is valid!" << std::endl;
    } else {
        std::cout << "   ✗ Configuration validation failed!" << std::endl;
    }
    std::cout << std::endl;
    
    // 3. Type Conversion
    std::cout << "3. Type Conversion and Error Handling:" << std::endl;
    
    config.set("string_value", "123");
    
    std::cout << "   Set string_value to: \"123\"" << std::endl;
    
    auto stringVal = config.get<std::string>("string_value");
    std::cout << "   Retrieved as string: \"" << stringVal.value() << "\"" << std::endl;
    
    auto intVal = config.get<int>("string_value");
    if (intVal.has_value()) {
        std::cout << "   Retrieved as int: " << intVal.value() << std::endl;
    } else {
        std::cout << "   ✗ Cannot retrieve string as int (expected)" << std::endl;
    }
    
    std::cout << std::endl;
    
    // 4. Configuration Summary
    std::cout << "4. Configuration Summary:" << std::endl;
    std::cout << "   " << config.summary() << std::endl;
    std::cout << std::endl;
    
    // 5. Configuration Operations
    std::cout << "5. Configuration Operations:" << std::endl;
    
    std::cout << "   Has 'learning_rate': " << (config.has("learning_rate") ? "yes" : "no") << std::endl;
    
    std::cout << "   Keys: ";
    for (const auto& key : config.getKeys()) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    config.set("new_key", "new_value");
    std::cout << "   After adding 'new_key': " << (config.has("new_key") ? "has key" : "no key") << std::endl;
    
    config.remove("new_key");
    std::cout << "   After removing 'new_key': " << (config.has("new_key") ? "has key" : "no key") << std::endl;
    std::cout << std::endl;
    
    // 6. Performance Test
    std::cout << "6. Performance Test:" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        config.set("perf_key_" + std::to_string(i), i);
        auto val = config.getOr<int>("perf_key_" + std::to_string(i), -1);
        if (val != i) {
            std::cerr << "Error: Performance test failed!" << std::endl;
            return 1;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "   1000 operations in " << duration.count() << " ms" << std::endl;
    if (duration.count() > 0) {
        double opsPerSec = (1000.0 * 1000.0) / duration.count();
        std::cout << "   Performance: " << opsPerSec << " ops/sec" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "=== Demo completed successfully! ===" << std::endl;
    return 0;
}
