// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

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

void testConfigLegacyFileFormat() {
    nlm::Config config;
    
    // Create a test config file
    std::string testFile = "/tmp/test_config.cfg";
    std::ofstream file(testFile);
    file << "# Test config file\n";
    file << "# Setting 1\n";
    file << "learning_rate = 0.001\n";
    file << "# Setting 2\n";
    file << "batch_size = 32\n";
    file << "# Setting 3\n";
    file << "epochs = 100\n";
    file.close();
    
    // Load the config file
    bool success = config.loadFromFile(testFile);
    assert(success);
    
    // Check values
    auto learningRate = config.get<double>("learning_rate");
    assert(learningRate.has_value());
    assert(*learningRate == 0.001);
    
    auto batchSize = config.get<int>("batch_size");
    assert(batchSize.has_value());
    assert(*batchSize == 32);
    
    auto epochs = config.get<int>("epochs");
    assert(epochs.has_value());
    assert(*epochs == 100);
    
    // Save back to file
    std::string outputFile = "/tmp/test_config_output.cfg";
    bool saved = config.saveToFile(outputFile);
    assert(saved);
    
    // Read back and verify
    std::ifstream readFile(outputFile);
    std::string line;
    int count = 0;
    while (std::getline(readFile, line)) {
        if (line.find("learning_rate = ") != std::string::npos) {
            count++;
        }
    }
    assert(count == 1);
    
    std::cout << "    testConfigLegacyFileFormat passed" << std::endl;
}

void testConfigCommandLineParsing() {
    nlm::Config config;
    
    // Simulate command line arguments
    const int argc = 5;
    const char* argv[] = {
        "test",
        "--learning_rate=0.01",
        "--batch_size=64",
        "-epochs", "200",
        "-verbose=true"
    };
    
    bool success = config.loadFromArgs(argc, const_cast<char**>(argv));
    assert(success);
    
    // Check values
    auto learningRate = config.get<double>("learning_rate");
    assert(learningRate.has_value());
    assert(*learningRate == 0.01);
    
    auto batchSize = config.get<int>("batch_size");
    assert(batchSize.has_value());
    assert(*batchSize == 64);
    
    auto epochs = config.get<int>("epochs");
    assert(epochs.has_value());
    assert(*epochs == 200);
    
    auto verbose = config.get<bool>("verbose");
    assert(verbose.has_value());
    assert(*verbose == true);
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

void testConfigInheritance() {
    nlm::Config config;
    
    // Set some values in parent config
    config.set("parent_setting1", "value1", nlm::ConfigSource::File);
    config.set("parent_setting2", 42, nlm::ConfigSource::File);
    
    // Create child config and inherit
    nlm::Config childConfig;
    childConfig.inheritFrom(std::make_shared<nlm::Config>(config));
    
    // Child should inherit parent values
    auto setting1 = childConfig.get<std::string>("parent_setting1");
    assert(setting1.has_value());
    assert(*setting1 == "value1");
    
    auto setting2 = childConfig.get<int>("parent_setting2");
    assert(setting2.has_value());
    assert(*setting2 == 42);
    
    // Child can override inherited values
    childConfig.set("parent_setting1", "child_value", nlm::ConfigSource::CommandLine);
    auto childSetting1 = childConfig.get<std::string>("parent_setting1");
    assert(childSetting1.has_value());
    assert(*childSetting1 == "child_value");
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Add schema for validation
    config.addSchema("learning_rate", typeid(double), true, "Learning rate for training");
    config.addSchema("batch_size", typeid(int64_t), true, "Batch size for training");
    config.addSchema("optimizer", typeid(std::string), false, "Optimization algorithm");
    
    // Set values that match schema
    config.set("learning_rate", 0.001, nlm::ConfigSource::File);
    config.set("batch_size", 32, nlm::ConfigSource::File);
    
    // Validation should pass
    bool valid = config.validate();
    assert(valid);
    
    // Set incorrect type
    config.set("learning_rate", std::string("invalid"), nlm::ConfigSource::File);
    
    // Validation should fail
    valid = config.validate();
    assert(!valid);
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigEnvVarExpansion() {
    nlm::Config config;
    
    // Set environment variable
    setenv("TEST_CONFIG_VALUE", "42", 1);
    
    // Set config with env var reference
    config.set("learning_rate", "${TEST_CONFIG_VALUE}", nlm::ConfigSource::File);
    
    // Value should be expanded
    auto value = config.get<std::string>("learning_rate");
    assert(value.has_value());
    assert(*value == "42");
    
    std::cout << "    testConfigEnvVarExpansion passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("setting1", "value1", nlm::ConfigSource::File);
    config1.set("setting2", 10, nlm::ConfigSource::CommandLine);
    
    nlm::Config config2;
    config2.set("setting2", 20, nlm::ConfigSource::File); // Should override command line
    config2.set("setting3", "value3", nlm::ConfigSource::Runtime);
    
    // Merge config2 into config1
    bool changed = config1.mergeWith(config2);
    assert(changed);
    
    // setting1 should remain
    auto setting1 = config1.get<std::string>("setting1");
    assert(setting1.has_value());
    assert(*setting1 == "value1");
    
    // setting2 should be from config2 (File overrides CommandLine)
    auto setting2 = config1.get<int>("setting2");
    assert(setting2.has_value());
    assert(*setting2 == 20);
    
    // setting3 should be added
    auto setting3 = config1.get<std::string>("setting3");
    assert(setting3.has_value());
    assert(*setting3 == "value3");
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigDocumentation() {
    nlm::Config config;
    
    config.set("learning_rate", 0.001, nlm::ConfigSource::File, "Learning rate for the model");
    config.set("batch_size", 32, nlm::ConfigSource::CommandLine, "Batch size for training");
    
    // Generate documentation
    std::string doc = config.generateDocumentation();
    
    // Check documentation contains expected content
    assert(doc.find("# Configuration Documentation") != std::string::npos);
    assert(doc.find("## learning_rate") != std::string::npos);
    assert(doc.find("## batch_size") != std::string::npos);
    assert(doc.find("Type: double") != std::string::npos);
    assert(doc.find("Type: int64") != std::string::npos);
    
    std::cout << "    testConfigDocumentation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLegacyFileFormat();
    testConfigCommandLineParsing();
    testConfigInheritance();
    testConfigSchemaValidation();
    testConfigEnvVarExpansion();
    testConfigMerge();
    testConfigDocumentation();
}

} // namespace test_config
