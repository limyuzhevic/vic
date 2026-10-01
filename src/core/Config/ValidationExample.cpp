// Configuration validation demonstration example
// Complete example showing how to use NLM's configuration validation system

#include "core/Config/Validation.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {
namespace config {

class ConfigurationValidationExample {
private:
    std::shared_ptr<Schema> create_brain_config_schema() {
        auto schema = std::make_shared<Schema>("brain_config", "1.0");
        
        // Neuron configuration
        auto neuron_count = validation::integer_element("neuron_count", true, 1, 100000);
        auto neuron_type = validation::enum_element("neuron_type", 
                                                   {"Excitatory", "Inhibitory", "Modulatory", "Sensory", "Motor", "Internal"},
                                                   false);
        auto neuron_threshold = validation::double_element("neuron_threshold", true, -1000.0, 1000.0);
        auto neuron_refractory_period = validation::integer_element("neuron_refractory_period", false, 0, 1000);
        
        schema->add_property("neuron_count", neuron_count);
        schema->add_property("neuron_type", neuron_type);
        schema->add_property("neuron_threshold", neuron_threshold);
        schema->add_property("neuron_refractory_period", neuron_refractory_period);
        
        return schema;
    }
    
    std::shared_ptr<Schema> create_memory_config_schema() {
        auto schema = std::make_shared<Schema>("memory_config", "1.0");
        
        // Working memory configuration
        auto working_memory_capacity = validation::integer_element("working_memory_capacity", true, 1, 10000);
        auto working_memory_decay_rate = validation::double_element("working_memory_decay_rate", false, 0.0, 1.0);
        auto working_memory_competition_enabled = validation::boolean_element("working_memory_competition_enabled", false);
        
        // Episodic memory configuration
        auto episodic_memory_capacity = validation::integer_element("episodic_memory_capacity", true, 1, 100000);
        auto episodic_memory_max_episodes = validation::integer_element("episodic_memory_max_episodes", false, 1, 1000000);
        auto episodic_memory_replay_enabled = validation::boolean_element("episodic_memory_replay_enabled", false);
        auto episodic_memory_replay_frequency = validation::double_element("episodic_memory_replay_frequency", false, 0.0, 1.0);
        
        // Associative memory configuration
        auto associative_memory_capacity = validation::integer_element("associative_memory_capacity", true, 1, 100000);
        auto associative_memory_learning_rate = validation::double_element("associative_memory_learning_rate", false, 0.0, 1.0);
        auto associative_memory_decay_rate = validation::double_element("associative_memory_decay_rate", false, 0.0, 1.0);
        
        schema->add_property("working_memory_capacity", working_memory_capacity);
        schema->add_property("working_memory_decay_rate", working_memory_decay_rate);
        schema->add_property("working_memory_competition_enabled", working_memory_competition_enabled);
        
        schema->add_property("episodic_memory_capacity", episodic_memory_capacity);
        schema->add_property("episodic_memory_max_episodes", episodic_memory_max_episodes);
        schema->add_property("episodic_memory_replay_enabled", episodic_memory_replay_enabled);
        schema->add_property("episodic_memory_replay_frequency", episodic_memory_replay_frequency);
        
        schema->add_property("associative_memory_capacity", associative_memory_capacity);
        schema->add_property("associative_memory_learning_rate", associative_memory_learning_rate);
        schema->add_property("associative_memory_decay_rate", associative_memory_decay_rate);
        
        return schema;
    }
    
    std::shared_ptr<Schema> create_development_config_schema() {
        auto schema = std::make_shared<Schema>("development_config", "1.0");
        
        // Development configuration
        auto development_enabled = validation::boolean_element("development_enabled", false);
        auto developmental_stage = validation::enum_element("developmental_stage",
                                                          {"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"},
                                                          false);
        auto critical_period_strength = validation::double_element("critical_period_strength", false, 0.0, 1.0);
        auto maturation_rate = validation::double_element("maturation_rate", false, 0.0, 1.0);
        auto pruning_strength = validation::double_element("pruning_strength", false, 0.0, 1.0);
        auto synaptogenesis_rate = validation::double_element("synaptogenesis_rate", false, 0.0, 1.0);
        
        schema->add_property("development_enabled", development_enabled);
        schema->add_property("developmental_stage", developmental_stage);
        schema->add_property("critical_period_strength", critical_period_strength);
        schema->add_property("maturation_rate", maturation_rate);
        schema->add_property("pruning_strength", pruning_strength);
        schema->add_property("synaptogenesis_rate", synaptogenesis_rate);
        
        return schema;
    }
    
    std::shared_ptr<Schema> create_neuromodulation_config_schema() {
        auto schema = std::make_shared<Schema>("neuromodulation_config", "1.0");
        
        // Neuromodulation configuration
        auto dopamine_enabled = validation::boolean_element("dopamine_enabled", false);
        auto dopamine_sensitivity = validation::double_element("dopamine_sensitivity", false, 0.0, 10.0);
        auto dopamine_decay_rate = validation::double_element("dopamine_decay_rate", false, 0.0, 1.0);
        
        auto curiosity_enabled = validation::boolean_element("curiosity_enabled", false);
        auto curiosity_sensitivity = validation::double_element("curiosity_sensitivity", false, 0.0, 10.0);
        auto curiosity_decay_rate = validation::double_element("curiosity_decay_rate", false, 0.0, 1.0);
        
        auto novelty_enabled = validation::boolean_element("novelty_enabled", false);
        auto novelty_sensitivity = validation::double_element("novelty_sensitivity", false, 0.0, 10.0);
        auto novelty_decay_rate = validation::double_element("novelty_decay_rate", false, 0.0, 1.0);
        
        auto reward_enabled = validation::boolean_element("reward_enabled", false);
        auto reward_strength = validation::double_element("reward_strength", false, 0.0, 10.0);
        auto reward_decay_rate = validation::double_element("reward_decay_rate", false, 0.0, 1.0);
        
        schema->add_property("dopamine_enabled", dopamine_enabled);
        schema->add_property("dopamine_sensitivity", dopamine_sensitivity);
        schema->add_property("dopamine_decay_rate", dopamine_decay_rate);
        
        schema->add_property("curiosity_enabled", curiosity_enabled);
        schema->add_property("curiosity_sensitivity", curiosity_sensitivity);
        schema->add_property("curiosity_decay_rate", curiosity_decay_rate);
        
        schema->add_property("novelty_enabled", novelty_enabled);
        schema->add_property("novelty_sensitivity", novelty_sensitivity);
        schema->add_property("novelty_decay_rate", novelty_decay_rate);
        
        schema->add_property("reward_enabled", reward_enabled);
        schema->add_property("reward_strength", reward_strength);
        schema->add_property("reward_decay_rate", reward_decay_rate);
        
        return schema;
    }
    
public:
    void demonstrate_validation_system() {
        std::cout << "=== NLM Configuration Validation Demonstration ===\n" << std::endl;
        
        // Create validator instance
        ConfigurationValidator validator;
        
        // Register schemas
        validator.register_schema("brain_config", create_brain_config_schema());
        validator.register_schema("memory_config", create_memory_config_schema());
        validator.register_schema("development_config", create_development_config_schema());
        validator.register_schema("neuromodulation_config", create_neuromodulation_config_schema());
        
        // Enable auto-fix
        validator.enable_auto_fix(true);
        
        // Example 1: Valid configuration
        std::cout << "1. Testing valid configuration...\n";
        std::map<std::string, ConfigValue> valid_config = {
            {"neuron_count", ConfigValue(1000)},
            {"neuron_type", ConfigValue(std::string("Excitatory"))},
            {"neuron_threshold", ConfigValue(-55.0)},
            {"neuron_refractory_period", ConfigValue(5)},
            {"working_memory_capacity", ConfigValue(100)},
            {"working_memory_decay_rate", ConfigValue(0.01)},
            {"working_memory_competition_enabled", ConfigValue(true)},
            {"episodic_memory_capacity", ConfigValue(1000)},
            {"episodic_memory_replay_enabled", ConfigValue(true)},
            {"associative_memory_capacity", ConfigValue(5000)},
            {"development_enabled", ConfigValue(true)},
            {"developmental_stage", ConfigValue(std::string("Adult"))},
            {"dopamine_enabled", ConfigValue(true)},
            {"curiosity_enabled", ConfigValue(true)},
            {"reward_enabled", ConfigValue(true)}
        };
        
        auto result = validator.validate(valid_config, "brain_config");
        std::cout << "   Result: " << result.get_summary() << std::endl;
        if (!result.valid) {
            std::cout << "   Errors: " << result.get_error_string();
        }
        
        // Example 2: Invalid configuration
        std::cout << "\n2. Testing invalid configuration...\n";
        std::map<std::string, ConfigValue> invalid_config = {
            {"neuron_count", ConfigValue(-100)},  // Negative value
            {"neuron_threshold", ConfigValue(2000.0)},  // Value too high
            {"working_memory_capacity", ConfigValue(1000000)},  // Too large
            {"episodic_memory_replay_frequency", ConfigValue(1.5)},  // Value > 1.0
            {"associative_memory_decay_rate", ConfigValue(-0.1)},  // Negative decay
            {"development_enabled", ConfigValue("yes")},  // Wrong type
            {"dopamine_sensitivity", ConfigValue("high")},  // Wrong type for enum
            {"unknown_param", ConfigValue(42)}  // Unknown parameter
        };
        
        result = validator.validate(invalid_config, "brain_config");
        std::cout << "   Result: " << result.get_summary() << std::endl;
        if (result.has_errors()) {
            std::cout << "   Validation errors:\n";
            for (const auto& error : result.errors) {
                std::cout << "     - " << error.to_string() << std::endl;
            }
        }
        
        // Example 3: Auto-fix demonstration
        std::cout << "\n3. Testing auto-fix...\n";
        auto fixed_config = invalid_config;
        bool auto_fixed = validator.auto_fix(invalid_config, "brain_config", fixed_config);
        std::cout << "   Auto-fix " << (auto_fixed ? "succeeded" : "failed") << std::endl;
        
        if (auto_fixed) {
            std::cout << "   Fixed configuration:\n";
            for (const auto& pair : fixed_config) {
                std::cout << "     - " << pair.first << ": " << validation::config_value_to_string(pair.second) << std::endl;
            }
            
            // Validate again
            auto final_result = validator.validate(fixed_config, "brain_config");
            std::cout << "   After fix validation: " << final_result.get_summary() << std::endl;
        }
        
        // Example 4: Schema comparison
        std::cout << "\n4. Testing schema operations...\n";
        auto brain_schema = validator.get_schema("brain_config");
        auto memory_schema = validator.get_schema("memory_config");
        
        std::cout << "   Brain schema: " << brain_schema->get_name() << " v" << brain_schema->get_version() << std::endl;
        std::cout << "   Memory schema: " << memory_schema->get_name() << " v" << memory_schema->get_version() << std::endl;
        
        // Example 5: Custom validator
        std::cout << "\n5. Testing custom validator...\n";
        auto custom_validator = validation::enum_validator({"Production", "Development", "Testing"});
        std::map<std::string, ConfigValue> custom_config = {
            {"environment", ConfigValue(std::string("Production"))}
        };
        
        std::vector<ValidationError> custom_errors;
        bool custom_valid = custom_validator->validate(custom_config["environment"], custom_errors, "environment");
        std::cout << "   Custom validator result: " << (custom_valid ? "PASS" : "FAIL") << std::endl;
        if (!custom_valid) {
            std::cout << "   Custom validator errors: " << custom_errors.size() << std::endl;
        }
        
        // Example 6: Configuration file validation
        std::cout << "\n6. Testing configuration file validation...\n";
        std::string config_file = "config_validation_example.json";
        bool file_valid = validate_config_file(config_file, validator);
        std::cout << "   Configuration file validation: " << (file_valid ? "PASS" : "FAIL") << std::endl;
        
        // Example 7: Performance and memory validation
        std::cout << "\n7. Testing performance validation...\n";
        std::map<std::string, ConfigValue> performance_config = {
            {"neuron_count", ConfigValue(10000)},
            {"working_memory_capacity", ConfigValue(1000)},
            {"episodic_memory_capacity", ConfigValue(50000)},
            {"associative_memory_capacity", ConfigValue(100000)}
        };
        
        result = validator.validate(performance_config, "brain_config");
        std::cout << "   Performance configuration validation: " << (result.valid ? "PASS" : "FAIL") << std::endl;
        
        // Example 8: Advanced schema features
        std::cout << "\n8. Testing advanced schema features...\n";
        demonstrate_advanced_features(validator);
        
        std::cout << "\n=== Demonstration Complete ===" << std::endl;
    }
    
    bool validate_config_file(const std::string& filename, ConfigurationValidator& validator) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "   Error: Could not open configuration file: " << filename << std::endl;
            return false;
        }
        
        try {
            json json_config;
            file >> json_config;
            
            std::map<std::string, ConfigValue> config;
            for (const auto& item : json_config.items()) {
                config[item.key()] = json_to_config_value(item.value());
            }
            
            auto result = validator.validate(config, "brain_config");
            if (!result.valid) {
                std::cout << "   Configuration file validation errors:\n";
                for (const auto& error : result.errors) {
                    std::cout << "     - " << error.to_string() << std::endl;
                }
                return false;
            }
            
            return true;
        } catch (const json::exception& e) {
            std::cerr << "   Error: JSON parsing failed: " << e.what() << std::endl;
            return false;
        }
    }
    
    ConfigValue json_to_config_value(const json& j) {
        if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_number_integer()) {
            return j.get<int64_t>();
        } else if (j.is_number_unsigned()) {
            return j.get<uint64_t>();
        } else if (j.is_number()) {
            return j.get<double>();
        } else if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_array()) {
            std::vector<ConfigValue> array;
            for (const auto& item : j) {
                array.push_back(json_to_config_value(item));
            }
            return array;
        } else {
            return ConfigValue(std::string("[null]"));
        }
    }
    
    void demonstrate_advanced_features(ConfigurationValidator& validator) {
        // Create advanced schema with custom validators
        auto advanced_schema = std::make_shared<Schema>("advanced_config", "1.0");
        
        // Custom validator for positive odd integers
        auto custom_odd_validator = std::make_shared<Validator>();
        // Implementation would be provided
        
        // Add custom property with validator
        auto odd_number = validation::integer_element("odd_number", true, 1, 1000000);
        odd_number->custom_validator = [custom_odd_validator](const ConfigValue& value) {
            // Custom validation logic would go here
            return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) % 2 == 1;
        };
        
        advanced_schema->add_property("odd_number", odd_number);
        validator.register_schema("advanced_config", advanced_schema);
        
        std::cout << "   Advanced schema created with custom validator" << std::endl;
        
        // Demonstrate schema inheritance
        auto base_schema = std::make_shared<Schema>("base_config", "1.0");
        auto derived_schema = std::make_shared<Schema>("derived_config", "1.0");
        derived_schema->parent_schema_ = base_schema;
        
        std::cout << "   Schema inheritance demonstrated" << std::endl;
        
        // Demonstrate schema comparison
        auto schema1 = validator.get_schema("brain_config");
        auto schema2 = validator.get_schema("memory_config");
        
        std::cout << "   Schema comparison: " << (schema1->get_version() != schema2->get_version() ? "DIFFERENT VERSIONS" : "SAME VERSION") << std::endl;
    }
};

// Entry point
int main() {
    ConfigurationValidationExample example;
    example.demonstrate_validation_system();
    return 0;
}

// Example configuration file
/*
{
    "neuron_count": 1000,
    "neuron_type": "Excitatory",
    "neuron_threshold": -55.0,
    "working_memory_capacity": 100,
    "episodic_memory_replay_enabled": true,
    "associative_memory_capacity": 5000
}
*/

// Note: This example requires the nlohmann/json library
// To compile: g++ -std=c++20 -I/usr/include/json example.cpp -lnlohmann_json
// Or with CMake: add_executable(config_validation_example example.cpp)
// Add dependency: find_package(nlohmann_json REQUIRED)
// target_link_libraries(config_validation_example PRIVATE nlohmann_json::nlohmann_json)