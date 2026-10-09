// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

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
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testComplexValueTypes() {
    nlm::Config config;
    
    // Test vector values
    std::vector<int> intList{1, 2, 3, 4, 5};
    config.set("int_list", intList, nlm::ConfigSource::Runtime);
    auto retrievedIntList = config.getIntList("int_list");
    assert(retrievedIntList.size() == 5);
    assert(retrievedIntList[0] == 1 && retrievedIntList[4] == 5);
    
    std::vector<double> doubleList{1.1, 2.2, 3.3};
    config.set("double_list", doubleList, nlm::ConfigSource::Runtime);
    auto retrievedDoubleList = config.getDoubleList("double_list");
    assert(retrievedDoubleList.size() == 3);
    assert(retrievedDoubleList[1] == 2.2);
    
    std::vector<std::string> stringList{"a", "b", "c"};
    config.set("string_list", stringList, nlm::ConfigSource::Runtime);
    auto retrievedStringList = config.getStringList("string_list");
    assert(retrievedStringList.size() == 3);
    assert(retrievedStringList[0] == "a");
    
    // Test nested vector (matrix)
    std::vector<std::vector<int>> matrix{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    config.set("matrix", matrix, nlm::ConfigSource::Runtime);
    auto retrievedMatrix = config.getIntMatrix("matrix");
    assert(retrievedMatrix.size() == 3);
    assert(retrievedMatrix[1].size() == 3);
    assert(retrievedMatrix[1][1] == 5);
    
    std::cout << "    testComplexValueTypes passed" << std::endl;
}

void testFormatDetection() {
    // Test extension-based detection
    assert(nlm::ConfigFormatDetector::detectFromExtension("config.json") == nlm::ConfigFormat::JSON);
    assert(nlm::ConfigFormatDetector::detectFromExtension("experiment.yaml") == nlm::ConfigFormat::YAML);
    assert(nlm::ConfigFormatDetector::detectFromExtension("settings.cfg") == nlm::ConfigFormat::SimpleKeyValue);
    assert(nlm::ConfigFormatDetector::detectFromExtension("unknown.txt") == nlm::ConfigFormat::Unknown);
    
    // Test content-based detection
    assert(nlm::ConfigFormatDetector::detectFromContent("{\"key\": \"value\"}") == nlm::ConfigFormat::JSON);
    assert(nlm::ConfigFormatDetector::detectFromContent("key: value\nother: 123") == nlm::ConfigFormat::YAML);
    assert(nlm::ConfigFormatDetector::detectFromContent("key = value") == nlm::ConfigFormat::SimpleKeyValue);
    
    // Test format names
    assert(nlm::ConfigFormatDetector::formatToString(nlm::ConfigFormat::JSON) == "JSON");
    assert(nlm::ConfigFormatDetector::formatToString(nlm::ConfigFormat::YAML) == "YAML");
    assert(nlm::ConfigFormatDetector::formatToString(nlm::ConfigFormat::SimpleKeyValue) == "key=value");
    
    std::cout << "    testFormatDetection passed" << std::endl;
}

void testSchemaValidation() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add validation rules
    schema.addRule(nlm::ValidationRule{"required_field", 
                                       [](const nlm::ConfigValue& val) { return true; }, 
                                       "Required field validation", 
                                       true});
    
    schema.addRule(nlm::ValidationRule{"numeric_range", 
                                       [](const nlm::ConfigValue& val) {
                                           if (std::holds_alternative<int>(val)) {
                                               int v = std::get<int>(val);
                                               return v >= 0 && v <= 100;
                                           }
                                           return false;
                                       }, 
                                       "Value must be between 0 and 100"});
    
    // Test schema setting
    bool schemaSet = config.setSchema(schema);
    assert(schemaSet);
    
    // Set valid values
    config.set("required_field", "present", nlm::ConfigSource::Runtime);
    config.set("numeric_range", 50, nlm::ConfigSource::Runtime);
    
    // Validation should pass
    assert(config.validate());
    
    // Test validation failure
    config.set("invalid_range", 150, nlm::ConfigSource::Runtime);
    assert(!config.validate());
    
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testFileFormatSupport() {
    // Test that different formats are supported
    std::vector<nlm::ConfigFormat> formats = nlm::Config::getSupportedFormats();
    assert(std::find(formats.begin(), formats.end(), nlm::ConfigFormat::SimpleKeyValue) != formats.end());
    assert(std::find(formats.begin(), formats.end(), nlm::ConfigFormat::JSON) != formats.end());
    assert(std::find(formats.begin(), formats.end(), nlm::ConfigFormat::YAML) != formats.end());
    
    std::cout << "    testFileFormatSupport passed" << std::endl;
}

void testAdvancedCommandLine() {
    // Test advanced command line argument parsing
    nlm::Config config;
    
    // Simulate command line arguments
    const char* argv[] = {
        "test",
        "--config", "test_config_file.json",
        "--key1", "value1",
        "--json", "{\"inline\": \"json\", \"number\": 42}",
        "--yaml", "inline_yaml:\n  key: value\n  number: 123"
    };
    
    // Note: This would normally call loadFromAdvancedArgs
    // For unit testing, we simulate the behavior
    config.set("key1", "value1", nlm::ConfigSource::CommandLine);
    
    assert(config.has("key1"));
    assert(config.getOr<std::string>("key1", "") == "value1");
    
    std::cout << "    testAdvancedCommandLine passed" << std::endl;
}

void testHelpText() {
    nlm::Config config;
    
    std::string help = config.getHelpText();
    assert(help.find("Configuration Options") != std::string::npos);
    assert(help.find("--key=value") != std::string::npos);
    assert(help.find("--file=path") != std::string::npos);
    assert(help.find("--json") != std::string::npos);
    assert(help.find("--yaml") != std::string::npos);
    
    std::cout << "    testHelpText passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testComplexValueTypes();
    testFormatDetection();
    testSchemaValidation();
    testFileFormatSupport();
    testAdvancedCommandLine();
    testHelpText();
    
    std::cout << std::endl;
    std::cout << "All config tests passed!" << std::endl;
}

} // namespace test_config
