#include <iostream>
#include <string>
#include "../src/core/Config/Config.hpp"

int main() {
    try {
        // Test 1: Basic Config creation and configuration
        std::cout << "=== Test 1: Basic Config Configuration ===" << std::endl;
        nlm::Config config;
        
        config.set("learning_rate", 0.01);
        config.set("batch_size", 32);
        config.set("model_type", "feedforward");
        config.set("epochs", 100);
        config.set("dropout_rate", 0.2);
        
        std::cout << "Configuration created successfully:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Test 2: Type conversion methods
        std::cout << "\n=== Test 2: Type Conversion Methods ===" << std::endl;
        
        // Get values as specific types using the enhanced getters
        auto learning_rate = config.get<double>("learning_rate");
        auto batch_size = config.get<int>("batch_size");
        auto model_type = config.get<std::string>("model_type");
        auto epochs = config.get<int64_t>("epochs");
        auto dropout_rate = config.get<double>("dropout_rate");
        
        std::cout << "Type-converted values:" << std::endl;
        std::cout << "  learning_rate (double): " << (learning_rate ? *learning_rate : 0) << std::endl;
        std::cout << "  batch_size (int): " << (batch_size ? *batch_size : 0) << std::endl;
        std::cout << "  model_type (string): " << (model_type ? *model_type : "") << std::endl;
        std::cout << "  epochs (int64_t): " << (epochs ? *epochs : 0) << std::endl;
        std::cout << "  dropout_rate (double): " << (dropout_rate ? *dropout_rate : 0) << std::endl;
        
        // Test 3: Configuration validation
        std::cout << "\n=== Test 3: Configuration Validation ===" << std::endl;
        
        nlm::ConfigSchema validationSchema;
        
        // Add validation rule for positive learning rate
        validationSchema["learning_rate"] = nlm::ConfigValidation("learning_rate", "Learning rate must be positive")
            .addRule([](const nlm::ConfigValue& value) {
                return std::visit([](const auto& val) -> bool {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, double>) {
                        return val > 0;
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        return val > 0;
                    }
                    return false;
                }, value);
            });
        
        config.addSchema(validationSchema);
        
        // Test validation with context
        std::map<std::string, nlm::ConfigValue> context;
        context["optimizer"] = "adam";
        
        if (config.validate(context)) {
            std::cout << "Configuration validation passed!" << std::endl;
        } else {
            std::cout << "Configuration validation failed!" << std::endl;
        }
        
        // Test 4: Configuration equality and merging
        std::cout << "\n=== Test 4: Configuration Equality and Merging ===" << std::endl;
        
        nlm::Config config1;
        config1.set("param1", "value1");
        config1.set("param2", 42);
        
        nlm::Config config2;
        config2.set("param1", "value1");
        config2.set("param2", 42);
        config2.set("param3", "value3");
        
        std::cout << "config1 == config2: " << (config1.diff(config2).empty() ? "true" : "false") << std::endl;
        
        // Merge config2 into config1
        config1.merge(config2);
        
        std::cout << "After merging config2 into config1:" << std::endl;
        std::cout << "  config1 keys: ";
        auto config1Keys = config1.getKeys();
        for (const auto& key : config1Keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
        
        // Test 5: Configuration diff
        std::cout << "\n=== Test 5: Configuration Diff ===" << std::endl;
        
        auto diff = config1.diff(config2);
        std::cout << "Differences between config1 and config2:" << std::endl;
        for (const auto& pair : diff) {
            std::cout << "  Key: " << pair.first << std::endl;
            std::cout << "    Old value: ";
            std::visit([](const auto& val) { std::cout << val; }, pair.second.first);
            std::cout << std::endl;
            std::cout << "    New value: ";
            std::visit([](const auto& val) { std::cout << val; }, pair.second.second);
            std::cout << std::endl;
        }
        
        // Test 6: Configuration serialization (toDict/fromDict)
        std::cout << "\n=== Test 6: Configuration Serialization ===" << std::endl;
        
        auto configDict = config1.toDict();
        std::cout << "Configuration as dictionary:" << std::endl;
        for (const auto& pair : configDict) {
            std::cout << "  " << pair.first << ": ";
            std::visit([](const auto& val) { std::cout << val; }, pair.second);
            std::cout << std::endl;
        }
        
        // Test loading from dictionary
        nlm::Config configFromDict;
        configFromDict.fromDict(configDict);
        
        std::cout << "Loaded from dictionary:")
        std::cout << configFromDict.summary() << std::endl;
        
        // Test 7: Configuration statistics
        std::cout << "\n=== Test 7: Configuration Statistics ===" << std::endl;
        
        auto stats = config1.getStats();
        std::cout << "Configuration statistics:" << std::endl;
        std::cout << "  Total entries: " << stats.totalEntries << std::endl;
        std::cout << "  Memory usage: " << stats.memoryUsage << " bytes" << std::endl;
        std::cout << "  Type counts:" << std::endl;
        for (const auto& pair : stats.typeCounts) {
            std::cout << "    " << pair.first << ": " << pair.second << std::endl;
        }
        std::cout << "  Average values:" << std::endl;
        for (const auto& pair : stats.averageValues) {
            std::cout << "    " << pair.first << ": " << pair.second << std::endl;
        }
        
        // Test 8: Configuration help and documentation
        std::cout << "\n=== Test 8: Configuration Help and Documentation ===" << std::endl;
        
        std::cout << "Configuration help:" << std::endl;
        std::cout << config1.help() << std::endl;
        
        std::cout << "Documentation for 'param1':" << std::endl;
        std::cout << config1.getKeyDocumentation("param1") << std::endl;
        
        // Test 9: Configuration validation rules
        std::cout << "\n=== Test 9: Configuration Validation Rules ===" << std::endl;
        
        auto validationRules = config1.getValidationRules();
        std::cout << "Configuration validation rules:" << std::endl;
        for (const auto& rule : validationRules) {
            std::cout << "  " << rule << std::endl;
        }
        
        // Test 10: Configuration iteration and access
        std::cout << "\n=== Test 10: Configuration Iteration and Access ===" << std::endl;
        
        std::cout << "Iterating over keys:" << std::endl;
        for (const auto& key : config1.getKeys()) {
            auto value = config1.get<nlm::ConfigValue>(key);
            std::cout << "  " << key << ": ";
            if (value) {
                std::visit([](const auto& val) { std::cout << val; }, *value);
            }
            std::cout << std::endl;
        }
        
        std::cout << "Accessing by indexing:" << std::endl;
        try {
            auto value = config1.get<nlm::ConfigValue>("param1");
            if (value) {
                std::cout << "  config['param1'] = ";
                std::visit([](const auto& val) { std::cout << val; }, *value);
                std::cout << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  Error: " << e.what() << std::endl;
        }
        
        // Test 11: Configuration size and iteration
        std::cout << "\n=== Test 11: Configuration Size and Iteration ===" << std::endl;
        
        std::cout << "Configuration size: " << config1.size() << std::endl;
        
        std::cout << "Getting items (key-value pairs):" << std::endl;
        auto items = config1.items();
        for (const auto& item : items) {
            std::cout << "  " << item.key << ": ";
            std::visit([](const auto& val) { std::cout << val; }, item.value);
            std::cout << std::endl;
        }
        
        std::cout << "Getting values:" << std::endl;
        auto values = config1.values();
        std::cout << "  Values: ";
        for (const auto& val : values) {
            std::visit([](const auto& v) { std::cout << v << " "; }, val);
        }
        std::cout << std::endl;
        
        // Test 12: Error handling
        std::cout << "\n=== Test 12: Error Handling ===" << std::endl;
        
        try {
            // Attempt to get a non-existent key
            auto nonExistent = config1.get<int>("non_existent_key");
            if (!nonExistent) {
                std::cout << "Key 'non_existent_key' not found (expected)" << std::endl;
            }
        } catch (const nlm::ConfigException& e) {
            std::cout << "ConfigException caught: " << e.what() << std::endl;
        }
        
        std::cout << "\n=== All tests completed successfully! ===" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}