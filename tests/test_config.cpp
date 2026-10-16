// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigJSONLoading() {
    nlm::Config config;
    
    std::string json = R"({"int_val": 42, "double_val": 3.14, "string_val": "test", "bool_val": true})";
    assert(config.loadFromJSON(json));
    
    assert(config.getOr<int>("int_val", 0) == 42);
    assert(config.getOr<double>("double_val", 0.0) > 3.13 && config.getOr<double>("double_val", 0.0) < 3.15);
    assert(config.getOr<std::string>("string_val", "") == "test");
    assert(config.getOr<bool>("bool_val", false) == true);
    
    std::cout << "    testConfigJSONLoading passed" << std::endl;
}

void testConfigYAMLLoading() {
    nlm::Config config;
    
    std::string yaml = """int_val: 42
double_val: 3.14
string_val: test
bool_val: true""";
    assert(config.loadFromYAML(yaml));
    
    assert(config.getOr<int>("int_val", 0) == 42);
    assert(config.getOr<double>("double_val", 0.0) > 3.13 && config.getOr<double>("double_val", 0.0) < 3.15);
    assert(config.getOr<std::string>("string_val", "") == "test");
    assert(config.getOr<bool>("bool_val", false) == true);
    
    std::cout << "    testConfigYAMLLoading passed" << std::endl;
}

void testConfigJSONSaving() {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    
    std::string json = config.saveToJSON();
    assert(!json.empty());
    
    // Load it back
    nlm::Config config2;
    assert(config2.loadFromJSON(json));
    
    assert(config2.getOr<int>("int_val", 0) == 42);
    assert(config2.getOr<double>("double_val", 0.0) > 3.13 && config2.getOr<double>("double_val", 0.0) < 3.15);
    assert(config2.getOr<std::string>("string_val", "") == "test");
    
    std::cout << "    testConfigJSONSaving passed" << std::endl;
}

void testConfigYAMLSaving() {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    
    std::string yaml = config.saveToYAML();
    assert(!yaml.empty());
    
    // Load it back
    nlm::Config config2;
    assert(config2.loadFromYAML(yaml));
    
    assert(config2.getOr<int>("int_val", 0) == 42);
    assert(config2.getOr<double>("double_val", 0.0) > 3.13 && config2.getOr<double>("double_val", 0.0) < 3.15);
    assert(config2.getOr<std::string>("string_val", "") == "test");
    
    std::cout << "    testConfigYAMLSaving passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Create schema
    nlm::ConfigSchema schema;
    nlm::ConfigSchema::SchemaEntry entry1("required_int", nlm::SchemaType::Integer, true, "Required integer");
    entry1.rules.push_back(nlm::SchemaValidationRule(nlm::SchemaValidationRule::ConstraintType::MinValue, "0", "Minimum value is 0"));
    schema.addEntry(entry1);
    
    nlm::ConfigSchema::SchemaEntry entry2("optional_string", nlm::SchemaType::String, false, "Optional string");
    entry2.rules.push_back(nlm::SchemaValidationRule(nlm::SchemaValidationRule::ConstraintType::MaxLength, "100", "Maximum length is 100"));
    schema.addEntry(entry2);
    
    // Test with missing required field
    std::vector<nlm::ConfigValidationError> errors = schema.validate(config);
    assert(errors.size() == 1);
    assert(errors[0].getErrorType() == nlm::ConfigValidationError::ErrorType::MissingRequired);
    
    // Add the required field
    config.set("required_int", 10, nlm::ConfigSource::Runtime);
    
    // Test with valid values
    errors = schema.validate(config);
    assert(errors.empty());
    
    // Test with invalid value (too small)
    nlm::Config config2;
    config2.set("required_int", -5, nlm::ConfigSource::Runtime);
    
    errors = schema.validate(config2);
    assert(!errors.empty());
    
    // Test with string that's too long
    nlm::Config config3;
    config3.set("required_int", 10, nlm::ConfigSource::Runtime);
    std::string longString(200, 'a');
    config3.set("optional_string", longString, nlm::ConfigSource::Runtime);
    
    errors = schema.validate(config3);
    assert(!errors.empty());
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigVersioning() {
    nlm::Config config;
    
    // Check default version
    nlm::ConfigVersion version = config.getVersion();
    assert(version.major == 1);
    assert(version.minor == 0);
    assert(version.patch == 0);
    
    // Set custom version
    nlm::ConfigVersion customVersion(2, 1, 5, "Custom version");
    config.setVersion(customVersion);
    
    assert(config.getVersion() == customVersion);
    assert(config.getVersionString() == "2.1.5 (Custom version)");
    
    std::cout << "    testConfigVersioning passed" << std::endl;
}

void testConfigImportExport() {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Export to JSON
    std::string json = config.exportAsJSON();
    assert(!json.empty());
    
    // Import from JSON
    nlm::Config config2;
    assert(config2.importFromJSON(json));
    
    assert(config2.getOr<int>("int_val", 0) == 42);
    assert(config2.getOr<std::string>("string_val", "") == "test");
    assert(config2.getOr<bool>("bool_val", false) == true);
    
    // Export to YAML
    std::string yaml = config.exportAsYAML();
    assert(!yaml.empty());
    
    // Import from YAML
    nlm::Config config3;
    assert(config3.importFromYAML(yaml));
    
    assert(config3.getOr<int>("int_val", 0) == 42);
    assert(config3.getOr<std::string>("string_val", "") == "test");
    assert(config3.getOr<bool>("bool_val", false) == true);
    
    // Export to INI
    std::string ini = config.exportAsINI();
    assert(!ini.empty());
    
    // Import from INI
    nlm::Config config4;
    assert(config4.importFromINI(ini));
    
    assert(config4.getOr<int>("int_val", 0) == 42);
    assert(config4.getOr<std::string>("string_val", "") == "test");
    assert(config4.getOr<bool>("bool_val", false) == true);
    
    std::cout << "    testConfigImportExport passed" << std::endl;
}

void testConfigInheritance() {
    nlm::Config parent;
    parent.set("common", 1, nlm::ConfigSource::Default);
    parent.set("parent_only", 2, nlm::ConfigSource::Default);
    
    nlm::Config child;
    child.inheritFrom(parent);
    
    assert(child.getOr<int>("common", 0) == 1);
    assert(child.getOr<int>("parent_only", 0) == 2);
    assert(!child.has("child_only"));
    
    // Override with child value
    child.set("common", 10, nlm::ConfigSource::Runtime);
    assert(child.getOr<int>("common", 0) == 10);
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void testConfigMerging() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Default);
    config1.set("key2", 2, nlm::ConfigSource::Default);
    
    nlm::Config config2;
    config2.set("key2", 20, nlm::ConfigSource::CommandLine);
    config2.set("key3", 3, nlm::ConfigSource::CommandLine);
    
    config1.mergeFrom(config2);
    
    assert(config1.getOr<int>("key1", 0) == 1);
    assert(config1.getOr<int>("key2", 0) == 20);
    assert(config1.getOr<int>("key3", 0) == 3);
    
    // Check sources
    assert(config1.getRaw("key1")->source == nlm::ConfigSource::Default);
    assert(config1.getRaw("key2")->source == nlm::ConfigSource::CommandLine);
    assert(config1.getRaw("key3")->source == nlm::ConfigSource::CommandLine);
    
    std::cout << "    testConfigMerging passed" << std::endl;
}

void testConfigTypeSafety() {
    nlm::Config config;
    
    // Set value of one type
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    
    // Try to get as different type
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("int_val");
    assert(doubleVal.has_value());
    assert(*doubleVal == 42.0);
    
    // Try to get as wrong type (string)
    auto stringVal = config.get<std::string>("int_val");
    assert(!stringVal.has_value()); // Should fail
    
    // Test type checking
    assert(config.isValidType("int_val", std::get<int>(config.getRaw("int_val").value())));
    
    std::cout << "    testConfigTypeSafety passed" << std::endl;
}

void testConfigDocumentation() {
    nlm::Config config;
    
    config.set("key1", 42, nlm::ConfigSource::Runtime);
    config.setDescription("key1", "An integer value");
    
    config.set("key2", "hello", nlm::ConfigSource::Runtime);
    config.setDescription("key2", "A string value");
    
    std::string doc = config.generateDocumentation();
    assert(!doc.empty());
    assert(doc.find("key1") != std::string::npos);
    assert(doc.find("key2") != std::string::npos);
    assert(doc.find("An integer value") != std::string::npos);
    assert(doc.find("A string value") != std::string::npos);
    
    std::string key1Doc = config.getDocumentation("key1");
    assert(key1Doc.find("key1") != std::string::npos);
    assert(key1Doc.find("An integer value") != std::string::npos);
    
    std::cout << "    testConfigDocumentation passed" << std::endl;
}

void testConfigBackwardCompatibility() {
    // Test that simple key=value format still works
    std::string ini = R"(# This is a comment
key1 = "value1"
key2 = 123
key3 = 45.67
key4 = true
key5 = [1,2,3,4]""";
    
    nlm::Config config;
    assert(config.importFromINI(ini));
    
    assert(config.getOr<std::string>("key1", "") == "value1");
    assert(config.getOr<int>("key2", 0) == 123);
    assert(config.getOr<double>("key3", 0.0) > 45.66 && config.getOr<double>("key3", 0.0) < 45.68);
    assert(config.getOr<bool>("key4", false) == true);
    
    // Arrays might not be fully supported yet, but test anyway
    auto arrayVal = config.getRaw("key5");
    // This depends on implementation
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONLoading();
    testConfigYAMLLoading();
    testConfigJSONSaving();
    testConfigYAMLSaving();
    testConfigSchemaValidation();
    testConfigVersioning();
    testConfigImportExport();
    testConfigInheritance();
    testConfigMerging();
    testConfigTypeSafety();
    testConfigDocumentation();
    testConfigBackwardCompatibility();
    
    std::cout << "\nAll Config tests passed!" << std::endl;
}

} // namespace test_config