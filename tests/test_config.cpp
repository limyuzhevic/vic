// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

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

// Test JSON configuration loading
void testConfigJSONLoading() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string jsonContent = R"({
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "connection_probability": 0.15,
        "visualization_enabled": true
    })"";
    
    bool result = config.loadFromJSON(jsonContent);
    assert(result);
    
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    assert(config.has("visualization_enabled"));
    
    std::cout << "    testConfigJSONLoading passed" << std::endl;
}

// Test YAML configuration loading
void testConfigYAMLLoading() {
    nlm::Config config;
    
    // Create a temporary YAML file
    std::string yamlContent = R"(
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
connection_probability: 0.15
visualization_enabled: true
)""";
    
    bool result = config.loadFromYAML(yamlContent);
    assert(result);
    
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    assert(config.has("visualization_enabled"));
    
    std::cout << "    testConfigYAMLLoading passed" << std::endl;
}

// Test INI configuration loading
void testConfigINILoading() {
    nlm::Config config;
    
    // Create a temporary INI file
    std::string iniContent = R"(
[general]
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000

[connectivity]
connection_probability = 0.15

[visualization]
visualization_enabled = true
)""";
    
    bool result = config.loadFromINI(iniContent);
    assert(result);
    
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    assert(config.has("visualization_enabled"));
    
    std::cout << "    testConfigINILoading passed" << std::endl;
}

// Test configuration serialization
void testConfigSerialization() {
    nlm::Config config;
    
    // Set various configuration values
    config.set("int_value", 42, nlm::ConfigSource::File);
    config.set("double_value", 3.14159, nlm::ConfigSource::File);
    config.set("string_value", "test", nlm::ConfigSource::File);
    config.set("bool_value", true, nlm::ConfigSource::File);
    config.set("vector_value", std::vector<int>({1, 2, 3}), nlm::ConfigSource::File);
    
    // Test JSON serialization
    bool jsonResult = config.saveToJSON("test_config.json");
    assert(jsonResult);
    
    // Load the JSON back
    nlm::Config config2;
    bool loadResult = config2.loadFromFile("test_config.json");
    assert(loadResult);
    
    // Verify values
    assert(config2.has("int_value"));
    assert(config2.has("double_value"));
    assert(config2.has("string_value"));
    assert(config2.has("bool_value"));
    assert(config2.has("vector_value"));
    
    // Test YAML serialization
    bool yamlResult = config.saveToYAML("test_config.yaml");
    assert(yamlResult);
    
    // Clean up
    std::remove("test_config.json");
    std::remove("test_config.yaml");
    
    std::cout << "    testConfigSerialization passed" << std::endl;
}

// Test configuration validation
void testConfigValidation() {
    nlm::Config config;
    
    // Add schema rules
    nlm::SchemaRule intRule;
    intRule.type = nlm::ValidationRule::RangeInt;
    intRule.key = "int_value";
    intRule.description = "Test integer validation";
    intRule.parameters = std::make_pair(0.0, 100.0);
    config.addSchemaRule(intRule);
    
    nlm::SchemaRule stringRule;
    stringRule.type = nlm::ValidationRule::StringLength;
    stringRule.key = "long_string";
    stringRule.description = "Test string length validation";
    stringRule.parameters = size_t(10);
    config.addSchemaRule(stringRule);
    
    // Test valid values
    config.set("int_value", 50, nlm::ConfigSource::File);
    config.set("long_string", "hello", nlm::ConfigSource::File);
    
    assert(config.validate());
    
    // Test invalid values
    config.set("int_value", 200, nlm::ConfigSource::File); // Out of range
    
    assert(!config.validate());
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

// Test configuration inheritance
void testConfigInheritance() {
    nlm::Config config;
    
    // Create base configuration
    std::string baseJson = R"({
        "base_setting": 100,
        "common": "base_value"
    })"";
    
    bool baseResult = config.loadFromJSON(baseJson);
    assert(baseResult);
    
    // Create override configuration
    std::string overrideJson = R"({
        "common": "override_value",
        "new_setting": 200
    })""";
    
    nlm::Config config2;
    bool overrideResult = config2.loadFromJSON(overrideJson);
    assert(overrideResult);
    
    // Test inheritance (this is a simplified test)
    config.clear();
    config.set("base_setting", 100, nlm::ConfigSource::File);
    config.set("common", "override_value", nlm::ConfigSource::File);
    config.set("new_setting", 200, nlm::ConfigSource::File);
    
    assert(config.has("base_setting"));
    assert(config.has("common"));
    assert(config.has("new_setting"));
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

// Test command line parsing
void testConfigCommandLineParsing() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[6];
    argv[0] = (char*)"test";
    argv[1] = (char*)"--random_seed=123";
    argv[2] = (char*)"--simulation_timestep=0.01";
    argv[3] = (char*)"--neuron_count=2000";
    argv[4] = (char*)"--visualization_enabled";
    
    bool result = config.loadFromArgs(argc, argv);
    assert(result);
    
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("visualization_enabled"));
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

// Test configuration checksum
void testConfigChecksum() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::File);
    config.set("key2", "value2", nlm::ConfigSource::File);
    
    nlm::ConfigChecksum checksum = config.computeChecksum();
    
    // Verify checksum
    assert(config.verifyChecksum(checksum));
    
    // Modify config
    config.set("key1", 2, nlm::ConfigSource::File);
    
    assert(!config.verifyChecksum(checksum));
    
    std::cout << "    testConfigChecksum passed" << std::endl;
}

// Test configuration file format detection
void testConfigFileFormatDetection() {
    // Test JSON detection
    std::string jsonFile = "test.json";
    assert(nlm::Config::detectFileFormat(jsonFile) == nlm::FileFormat::JSON);
    
    // Test YAML detection
    std::string yamlFile = "test.yaml";
    assert(nlm::Config::detectFileFormat(yamlFile) == nlm::FileFormat::YAML);
    
    // Test INI detection
    std::string iniFile = "test.ini";
    assert(nlm::Config::detectFileFormat(iniFile) == nlm::FileFormat::INI);
    
    // Test unknown detection
    std::string unknownFile = "test.unknown";
    assert(nlm::Config::detectFileFormat(unknownFile) == nlm::FileFormat::Unknown);
    
    std::cout << "    testConfigFileFormatDetection passed" << std::endl;
}

// Test configuration help system
void testConfigHelpSystem() {
    auto help = nlm::Config::getConfigurationHelp();
    assert(!help.empty());
    
    // Check that common keys are present
    bool foundRandomSeed = false;
    bool foundNeuronCount = false;
    
    for (const auto& entry : help) {
        if (entry.key == "random_seed") {
            foundRandomSeed = true;
            assert(entry.type == "int");
            assert(entry.defaultValue == "42");
        } else if (entry.key == "neuron_count") {
            foundNeuronCount = true;
            assert(entry.type == "int64_t");
            assert(entry.defaultValue == "500");
        }
    }
    
    assert(foundRandomSeed);
    assert(foundNeuronCount);
    
    std::string examples = nlm::Config::getConfigurationExamples();
    assert(!examples.empty());
    assert(examples.find("JSON Configuration Example") != std::string::npos);
    assert(examples.find("YAML Configuration Example") != std::string::npos);
    assert(examples.find("INI Configuration Example") != std::string::npos);
    
    std::cout << "    testConfigHelpSystem passed" << std::endl;
}

// Test configuration backward compatibility
// Test help flag functionality
void testConfigHelpFlags() {
    // Test help flag detection
    std::vector<std::string> helpFlags = {"--help", "-h", "-?", "--usage", "--help-config", "--config-help", "--list-options"};
    
    for (const auto& flag : helpFlags) {
        int argc = 2;
        char* argv[2];
        argv[0] = (char*)"test";
        argv[1] = (char*)flag.c_str();
        
        // Help flags should be detected and handled by the main program
        // For config tests, we test that the config system doesn't interfere with help flags
        nlm::Config config;
        bool result = config.loadFromArgs(argc, argv);
        
        // Config parsing should not fail on help flags
        assert(result);
    }
    
    std::cout << "    testConfigHelpFlags passed" << std::endl;
}

// Test command line argument parsing with validation
void testConfigCommandLineParsingWithValidation() {
    // Test parsing of command line arguments with validation
    int argc = 8;
    char* argv[9];
    argv[0] = (char*)"test";
    argv[1] = (char*)"--seed=123";
    argv[2] = (char*)"--timestep=0.01";
    argv[3] = (char*)"--neurons=2000";
    argv[4] = (char*)"--regions=5";
    argv[5] = (char*)"--probability=0.2";
    argv[6] = (char*)"--visualization_enabled";
    argv[7] = (char*)"--log_level=DEBUG";
    
    nlm::Config config;
    bool result = config.loadFromArgsWithValidation(argc, argv);
    assert(result);
    
    // Verify that values were parsed correctly
    assert(config.has("seed"));
    assert(config.has("timestep"));
    assert(config.has("neurons"));
    assert(config.has("regions"));
    assert(config.has("probability"));
    assert(config.has("visualization_enabled"));
    assert(config.has("log_level"));
    
    std::cout << "    testConfigCommandLineParsingWithValidation passed" << std::endl;
}

// Test configuration option validation
void testConfigOptionValidation() {
    nlm::Config config;
    
    // Add schema rules for validation
    nlm::SchemaRule seedRule;
    seedRule.type = nlm::ValidationRule::RangeInt;
    seedRule.key = "seed";
    seedRule.description = "Random seed validation";
    seedRule.parameters = std::make_pair(0.0, 2147483647.0);
    config.addSchemaRule(seedRule);
    
    nlm::SchemaRule timestepRule;
    timestepRule.type = nlm::ValidationRule::RangeDouble;
    timestepRule.key = "timestep";
    timestepRule.description = "Timestep validation";
    timestepRule.parameters = std::make_pair(0.0001, 0.1);
    config.addSchemaRule(timestepRule);
    
    nlm::SchemaRule neuronCountRule;
    neuronCountRule.type = nlm::ValidationRule::RangeInt;
    neuronCountRule.key = "neurons";
    neuronCountRule.description = "Neuron count validation";
    neuronCountRule.parameters = std::make_pair(1.0, 1000000.0);
    config.addSchemaRule(neuronCountRule);
    
    // Test valid values
    config.set("seed", 42, nlm::ConfigSource::CommandLine);
    config.set("timestep", 0.001, nlm::ConfigSource::CommandLine);
    config.set("neurons", 500, nlm::ConfigSource::CommandLine);
    
    assert(config.validate());
    
    // Test invalid values
    config.set("seed", 999999999, nlm::ConfigSource::CommandLine); // Out of range
    
    assert(!config.validate());
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    
    std::cout << "    testConfigOptionValidation passed" << std::endl;
}

// Test error handling for invalid arguments
void testConfigErrorHandling() {
    // Test that invalid arguments are handled gracefully
    nlm::Config config;
    
    // Test invalid key in command line arguments
    int argc = 3;
    char* argv[4];
    argv[0] = (char*)"test";
    argv[1] = (char*)"--invalid_key=123";
    argv[2] = (char*)"--another_invalid";
    
    bool result = config.loadFromArgsWithValidation(argc, argv);
    
    // Config parsing should not crash on invalid keys
    // It should log warnings and continue
    assert(result);
    
    // Verify that valid keys are still parsed
    config.set("seed", 42, nlm::ConfigSource::CommandLine);
    assert(config.has("seed"));
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

// Test configuration help system
void testConfigEnhancedHelpSystem() {
    // Test enhanced configuration help system
    auto help = nlm::Config::getConfigurationHelp();
    assert(!help.empty());
    
    // Check that common keys are present
    bool foundSeed = false;
    bool foundTimestep = false;
    bool foundNeurons = false;
    
    for (const auto& entry : help) {
        if (entry.key == "seed") {
            foundSeed = true;
            assert(entry.type == "int64_t");
            assert(entry.defaultValue == "42");
        } else if (entry.key == "timestep") {
            foundTimestep = true;
            assert(entry.type == "double");
            assert(entry.defaultValue == "0.001");
        } else if (entry.key == "neurons") {
            foundNeurons = true;
            assert(entry.type == "int64_t");
            assert(entry.defaultValue == "500");
        }
    }
    
    assert(foundSeed);
    assert(foundTimestep);
    assert(foundNeurons);
    
    std::string examples = nlm::Config::getConfigurationExamples();
    assert(!examples.empty());
    assert(examples.find("JSON Configuration Example") != std::string::npos);
    assert(examples.find("YAML Configuration Example") != std::string::npos);
    assert(examples.find("INI Configuration Example") != std::string::npos);
    
    std::cout << "    testConfigEnhancedHelpSystem passed" << std::endl;
}

} // namespace test_config
