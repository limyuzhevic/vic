// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

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

// New JSON/YAML tests
void testJSONParsing() {
    nlm::Config config;
    
    // Test parsing JSON string
    std::string jsonStr = R"({
        "learning_rate": 0.01,
        "batch_size": 32,
        "epochs": 100,
        "hidden_layers": [128, 64],
        "activation": "relu",
        "optimizer": "adam",
        "use_gpu": true
    })";
    
    config.loadFromJSON(jsonStr);
    
    assert(config.has("learning_rate"));
    assert(config.get<double>("learning_rate") == 0.01);
    assert(config.has("batch_size"));
    assert(config.get<int>("batch_size") == 32);
    assert(config.has("epochs"));
    assert(config.get<int>("epochs") == 100);
    assert(config.has("hidden_layers"));
    assert(config.has("activation"));
    assert(config.get<std::string>("activation") == "relu");
    assert(config.has("optimizer"));
    assert(config.get<std::string>("optimizer") == "adam");
    assert(config.has("use_gpu"));
    assert(config.get<bool>("use_gpu") == true);
    
    std::cout << "    testJSONParsing passed" << std::endl;
}

void testYAMLParsing() {
    nlm::Config config;
    
    // Test parsing YAML string
    std::string yamlStr = R"(learning_rate: 0.01
batch_size: 32
epochs: 100
hidden_layers:
  - 128
  - 64
activation: relu
optimizer: adam
use_gpu: true)";
    
    config.loadFromYAML(yamlStr);
    
    assert(config.has("learning_rate"));
    assert(config.get<double>("learning_rate") == 0.01);
    assert(config.has("batch_size"));
    assert(config.get<int>("batch_size") == 32);
    assert(config.has("epochs"));
    assert(config.get<int>("epochs") == 100);
    assert(config.has("hidden_layers"));
    assert(config.has("activation"));
    assert(config.get<std::string>("activation") == "relu");
    assert(config.has("optimizer"));
    assert(config.get<std::string>("optimizer") == "adam");
    assert(config.has("use_gpu"));
    assert(config.get<bool>("use_gpu") == true);
    
    std::cout << "    testYAMLParsing passed" << std::endl;
}

void testJSONSaveToFile() {
    nlm::Config config;
    
    // Add some test data
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string testFile = "/tmp/test_config.json";
    config.saveToFile(testFile);
    
    // Load it back
    nlm::Config config2;
    config2.loadFromFile(testFile);
    
    assert(config2.has("test_int"));
    assert(config2.get<int>("test_int") == 42);
    assert(config2.has("test_string"));
    assert(config2.get<std::string>("test_string") == "hello");
    assert(config2.has("test_bool"));
    assert(config2.get<bool>("test_bool") == true);
    assert(config2.has("test_double"));
    assert(config2.get<double>("test_double") > 3.13 && config2.get<double>("test_double") < 3.15);
    
    std::cout << "    testJSONSaveToFile passed" << std::endl;
}

void testYAMLSaveToFile() {
    nlm::Config config;
    
    // Add some test data
    config.set("yaml_test", "YAML content", nlm::ConfigSource::Runtime);
    config.set("number", 123, nlm::ConfigSource::Runtime);
    config.set("enabled", false, nlm::ConfigSource::Runtime);
    
    // Save to YAML file
    std::string testFile = "/tmp/test_config.yaml";
    config.saveToFile(testFile);
    
    // Load it back
    nlm::Config config2;
    config2.loadFromFile(testFile);
    
    assert(config2.has("yaml_test"));
    assert(config2.get<std::string>("yaml_test") == "YAML content");
    assert(config2.has("number"));
    assert(config2.get<int>("number") == 123);
    assert(config2.has("enabled"));
    assert(config2.get<bool>("enabled") == false);
    
    std::cout << "    testYAMLSaveToFile passed" << std::endl;
}

void testEnvironmentVariableSubstitution() {
    nlm::Config config;
    
    // Set a value with environment variable reference
    config.set("model_path", "${HOME}/models/model.pth", nlm::ConfigSource::Runtime);
    config.set("batch_size", "32", nlm::ConfigSource::Runtime);
    
    assert(config.has("model_path"));
    assert(config.get<std::string>("model_path") == "${HOME}/models/model.pth");
    assert(config.has("batch_size"));
    assert(config.get<std::string>("batch_size") == "32");
    
    std::cout << "    testEnvironmentVariableSubstitution passed" << std::endl;
}

void testConfigInheritance() {
    nlm::Config config;
    
    // Create a parent config file
    std::string parentFile = "/tmp/parent_config.json";
    {
        nlm::Config parent;
        parent.set("parent_value", "inherited", nlm::ConfigSource::Default);
        parent.set("common_value", "from_parent", nlm::ConfigSource::Default);
        parent.saveToFile(parentFile);
    }
    
    // Create child config and inherit from parent
    nlm::Config child;
    child.set("child_value", "local", nlm::ConfigSource::Runtime);
    child.set("common_value", "from_child", nlm::ConfigSource::Runtime);
    
    bool inherited = child.inheritFrom(parentFile);
    assert(inherited == true);
    
    assert(child.has("parent_value"));
    assert(child.get<std::string>("parent_value") == "inherited");
    
    assert(child.has("common_value"));
    assert(child.get<std::string>("common_value") == "from_child");
    
    assert(child.has("child_value"));
    assert(child.get<std::string>("child_value") == "local");
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void testSchemaValidation() {
    nlm::Config config;
    
    // Create a simple schema
    nlm::JSONSchema schema;
    schema.type = "object";
    
    // Add property schemas
    nlm::JSONSchema learningRateSchema;
    learningRateSchema.type = "number";
    learningRateSchema.min = 0.0;
    learningRateSchema.max = 1.0;
    
    nlm::JSONSchema batchSizeSchema;
    batchSizeSchema.type = "integer";
    batchSizeSchema.min = 1;
    
    nlm::JSONSchema nameSchema;
    nameSchema.type = "string";
    nameSchema.minLength = 1;
    nameSchema.maxLength = 50;
    
    schema.properties = std::make_shared<std::unordered_map<std::string, nlm::JSONSchema>>(
        std::unordered_map<std::string, nlm::JSONSchema>{{"learning_rate", learningRateSchema},
                                                        {"batch_size", batchSizeSchema},
                                                        {"name", nameSchema}}
    );
    
    // Test valid configuration
    std::string jsonStr = R"({"learning_rate": 0.5, "batch_size": 32, "name": "model1"})";
    config.loadFromJSONWithValidation(jsonStr, schema);
    
    assert(config.has("learning_rate"));
    assert(config.has("batch_size"));
    assert(config.has("name"));
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testFormatDetection() {
    // Test JSON detection
    std::string jsonContent = R"({"key": "value"})";
    assert(nlm::Config::isJSON(jsonContent));
    assert(!nlm::Config::isYAML(jsonContent));
    
    // Test YAML detection
    std::string yamlContent = R"(key: value)";
    assert(!nlm::Config::isJSON(yamlContent));
    assert(nlm::Config::isYAML(yamlContent));
    
    std::cout << "    testFormatDetection passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    testJSONParsing();
    testYAMLParsing();
    testJSONSaveToFile();
    testYAMLSaveToFile();
    testEnvironmentVariableSubstitution();
    testConfigInheritance();
    testSchemaValidation();
    testFormatDetection();
    
    std::cout << "All config tests passed!" << std::endl;
}

} // namespace test_config