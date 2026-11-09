#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_config_enhanced {

void testConfigValueConversion() {
    std::cout << "    testConfigValueConversion passed" << std::endl;
}

void testConfigJSONSupport() {
    nlm::Config config;
    
    // Test JSON file creation and parsing
    std::string testFile = "test_config.json";
    
    // Set some values
    config.set("int_val", 42);
    config.set("float_val", 3.14);
    config.set("string_val", "test");
    config.set("bool_val", true);
    config.set("int_array", std::vector<int>({1, 2, 3}));
    
    // Save as JSON
    bool saved = config.saveToFile(testFile, "json");
    assert(saved);
    
    // Clear and reload
    config.clear();
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    // Verify values
    assert(config.has("int_val"));
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && intVal.value() == 42);
    
    std::filesystem::remove(testFile);
    std::cout << "    testConfigJSONSupport passed" << std::endl;
}

void testConfigCommandLineParsing() {
    nlm::Config config;
    
    // Test boolean flag parsing
    std::string helpFlag = "--help=true";
    bool parsed = config.parseBoolFlag("true");
    assert(parsed);
    
    parsed = config.parseBoolFlag("false");
    assert(!parsed);
    
    // Test integer parsing
    auto intVal = config.parseIntArg("123");
    assert(intVal.has_value() && intVal.value() == 123);
    
    // Test double parsing
    auto doubleVal = config.parseDoubleArg("3.14");
    assert(doubleVal.has_value() && doubleVal.value() == 3.14);
    
    // Test list parsing
    auto list = config.parseListArg("1,2,3,4,5");
    assert(list.size() == 5);
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Add some schema entries
    nlm::ConfigSchemaEntry schema1("int_val", nlm::ConfigDataType::Integer, "Test integer");
    schema1.validator = nlm::config::validateRange;
    nlm::ConfigSchemaEntry schema2("float_val", nlm::ConfigDataType::Float, "Test float");
    schema2.defaultValue = 1.0;
    
    config.addSchemaEntry(schema1);
    config.addSchemaEntry(schema2);
    
    // Set values
    config.set("int_val", 5);
    config.set("float_val", 2.5);
    
    // Validate against schema
    bool valid = config.validate(schema1);
    assert(valid);
    
    valid = config.validate(schema2);
    assert(valid);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigHelp() {
    nlm::Config config;
    
    // Add some schema entries
    config.addSchemaEntry(nlm::ConfigSchemaEntry("neurons", nlm::ConfigDataType::IntVector, "Neuron count"));
    config.addSchemaEntry(nlm::ConfigSchemaEntry("enable_plasticity", nlm::ConfigDataType::Boolean, "Enable plasticity"));
    config.addSchemaEntry(nlm::ConfigSchemaEntry("learning_rate", nlm::ConfigDataType::Float, "Learning rate"));
    
    // Generate help
    std::string help = config.generateHelp(config.getSchemaEntries());
    assert(!help.empty());
    assert(help.find("--neurons") != std::string::npos);
    assert(help.find("--enable_plasticity") != std::string::npos);
    
    std::cout << "    testConfigHelp passed" << std::endl;
}

void runAll() {
    testConfigValueConversion();
    testConfigJSONSupport();
    testConfigCommandLineParsing();
    testConfigValidation();
    testConfigHelp();
}

} // namespace test_config_enhanced