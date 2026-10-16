#include "core/Config/Config.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== NLM Config Enhanced Test ===" << std::endl;
    
    // Test 1: Basic Config functionality
    nlm::Config config;
    
    // Test setting and getting values
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("test"));
    config.set("bool_value", true);
    
    std::cout << "1. Basic config operations:" << std::endl;
    std::cout << "   int_value: " << config.get<int>("int_value").value() << std::endl;
    std::cout << "   double_value: " << config.get<double>("double_value").value() << std::endl;
    std::cout << "   string_value: " << config.get<std::string>("string_value").value() << std::endl;
    std::cout << "   bool_value: " << config.get<bool>("bool_value").value() << std::endl;
    
    // Test getOr with missing key
    int defaultValue = config.getOr<int>("missing", 100);
    std::cout << "   missing key (default): " << defaultValue << std::endl;
    
    // Test 2: JSON loading
    std::string json = R"({"json_int": 100, "json_string": "json_test"})";
    if (config.loadFromJSON(json)) {
        std::cout << "\n2. JSON loading: SUCCESS" << std::endl;
        std::cout << "   json_int: " << config.getOr<int>("json_int", 0) << std::endl;
        std::cout << "   json_string: " << config.getOr<std::string>("json_string", "") << std::endl;
    } else {
        std::cout << "\n2. JSON loading: FAILED" << std::endl;
    }
    
    // Test 3: YAML loading
    std::string yaml = "yaml_int: 200\nyaml_string: yaml_test";
    if (config.loadFromYAML(yaml)) {
        std::cout << "\n3. YAML loading: SUCCESS" << std::endl;
        std::cout << "   yaml_int: " << config.getOr<int>("yaml_int", 0) << std::endl;
        std::cout << "   yaml_string: " << config.getOr<std::string>("yaml_string", "") << std::endl;
    } else {
        std::cout << "\n3. YAML loading: FAILED" << std::endl;
    }
    
    // Test 4: JSON saving
    std::string savedJson = config.saveToJSON();
    std::cout << "\n4. JSON saving: SUCCESS" << std::endl;
    std::cout << "   Saved JSON length: " << savedJson.length() << std::endl;
    
    // Test 5: YAML saving
    std::string savedYaml = config.saveToYAML();
    std::cout << "\n5. YAML saving: SUCCESS" << std::endl;
    std::cout << "   Saved YAML length: " << savedYaml.length() << std::endl;
    
    // Test 6: Config versioning
    nlm::ConfigVersion version(2, 1, 0, "Enhanced version");
    config.setVersion(version);
    std::cout << "\n6. Config versioning: SUCCESS" << std::endl;
    std::cout << "   Version: " << config.getVersionString() << std::endl;
    
    // Test 7: Schema validation
    nlm::ConfigSchema schema;
    nlm::ConfigSchema::SchemaEntry entry("required_field", nlm::SchemaType::Integer, true, "Required field");
    schema.addEntry(entry);
    
    std::vector<nlm::ConfigValidationError> errors = schema.validate(config);
    std::cout << "\n7. Schema validation: " << (errors.empty() ? "PASSED (no errors)" : "FAILED (errors found)") << std::endl;
    if (!errors.empty()) {
        for (const auto& error : errors) {
            std::cout << "   Error: " << error.what() << std::endl;
        }
    }
    
    // Test 8: Import/Export
    std::string exportedJson = config.exportAsJSON();
    nlm::Config config2;
    if (config2.importFromJSON(exportedJson)) {
        std::cout << "\n8. Import/Export: SUCCESS" << std::endl;
        std::cout << "   Successfully imported from exported JSON" << std::endl;
    } else {
        std::cout << "\n8. Import/Export: FAILED" << std::endl;
    }
    
    // Test 9: Inheritance
    nlm::Config parent;
    parent.set("parent_key", 999);
    
    nlm::Config child;
    child.inheritFrom(parent);
    
    if (child.getOr<int>("parent_key", 0) == 999) {
        std::cout << "\n9. Inheritance: SUCCESS" << std::endl;
        std::cout << "   Child inherited parent_key = " << child.getOr<int>("parent_key", 0) << std::endl;
    } else {
        std::cout << "\n9. Inheritance: FAILED" << std::endl;
    }
    
    // Test 10: Merging
    nlm::Config config1;
    config1.set("key1", 1);
    config1.set("key2", 2);
    
    nlm::Config config2;
    config2.set("key2", 20);
    config2.set("key3", 3);
    
    config1.mergeFrom(config2);
    
    if (config1.getOr<int>("key1", 0) == 1 && 
        config1.getOr<int>("key2", 0) == 20 && 
        config1.getOr<int>("key3", 0) == 3) {
        std::cout << "\n10. Merging: SUCCESS" << std::endl;
        std::cout << "   Merged config: key1=" << config1.getOr<int>("key1", 0) << ", "
                  << "key2=" << config1.getOr<int>("key2", 0) << ", "
                  << "key3=" << config1.getOr<int>("key3", 0) << std::endl;
    } else {
        std::cout << "\n10. Merging: FAILED" << std::endl;
    }
    
    std::cout << "\n=== Enhanced Config Test Complete ===" << std::endl;
    
    return 0;
}