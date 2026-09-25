// Comprehensive Config Tests for the new modern Config system

#include "core/Config/Config.hpp"
#include "core/Config/ConfigParser.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

namespace test_config_advanced {

void testConfigValueTypes() {
    nlm::ConfigValue cv1 = nlm::ConfigValue::create(42);
    assert(cv1.getTypeName() == "int");
    assert(cv1.as<int>() == 42);
    
    nlm::ConfigValue cv2 = nlm::ConfigValue::create(3.14);
    assert(cv2.getTypeName() == "double");
    assert(cv2.as<double>() == 3.14);
    
    nlm::ConfigValue cv3 = nlm::ConfigValue::create(std::string("test"));
    assert(cv3.getTypeName() == "string");
    assert(cv3.as<std::string>() == "test");
    
    nlm::ConfigValue cv4 = nlm::ConfigValue::create(true);
    assert(cv4.getTypeName() == "bool");
    assert(cv4.as<bool>() == true);
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void testConfigCreationWithFactory() {
    auto config = nlm::Config::create();
    assert(config != nullptr);
    
    auto config2 = nlm::ConfigFactory::createDefaultConfig();
    assert(config2 != nullptr);
    
    std::cout << "    testConfigCreationWithFactory passed" << std::endl;
}

void testConfigJsonParsing() {
    // Create a JSON string
    nlohmann::json json;
    json["int_key"] = 42;
    json["string_key"] = "hello";
    json["double_key"] = 3.14;
    json["bool_key"] = true;
    json["array_key"] = nlohmann::json::array({1, 2, 3});
    
    std::string jsonStr = json.dump();
    
    auto config = nlm::ConfigParser::parseString(jsonStr);
    assert(config != nullptr);
    
    assert(config->has("int_key"));
    assert(config->getOr<int>("int_key", 0) == 42);
    
    assert(config->has("string_key"));
    assert(config->getOr<std::string>("string_key", "") == "hello");
    
    assert(config->has("double_key"));
    assert(config->getOr<double>("double_key", 0.0) == 3.14);
    
    assert(config->has("bool_key"));
    assert(config->getOr<bool>("bool_key", false) == true);
    
    std::cout << "    testConfigJsonParsing passed" << std::endl;
}

void testConfigYamlParsing() {
    // Create a YAML string
    std::string yamlStr = R"(
    int_key: 42
    string_key: hello
    double_key: 3.14
    bool_key: true
    array_key:
      - 1
      - 2
      - 3
    )";
    
    auto config = nlm::ConfigParser::parseString(yamlStr, "yaml");
    assert(config != nullptr);
    
    assert(config->has("int_key"));
    assert(config->getOr<int>("int_key", 0) == 42);
    
    assert(config->has("string_key"));
    assert(config->getOr<std::string>("string_key", "") == "hello");
    
    std::cout << "    testConfigYamlParsing passed" << std::endl;
}

void testConfigSimpleParsing() {
    // Create a simple format string
    std::string simpleStr = R"(
    int_key = 42
    string_key = hello
    double_key = 3.14
    bool_key = true
    array_key = [1, 2, 3]
    )";
    
    auto config = nlm::ConfigParser::parseString(simpleStr);
    assert(config != nullptr);
    
    assert(config->has("int_key"));
    assert(config->getOr<int>("int_key", 0) == 42);
    
    assert(config->has("string_key"));
    assert(config->getOr<std::string>("string_key", "") == "hello");
    
    std::cout << "    testConfigSimpleParsing passed" << std::endl;
}

void testConfigSerialization() {
    auto config = nlm::ConfigFactory::createDefaultConfig();
    
    config->set("int_key", 42, nlm::ConfigSource::Runtime);
    config->set("string_key", std::string("hello"), nlm::ConfigSource::Runtime);
    config->set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config->set("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Test JSON serialization
    std::string jsonStr = config->toJson();
    assert(!jsonStr.empty());
    
    // Test YAML serialization
    std::string yamlStr = config->toYaml();
    assert(!yamlStr.empty());
    
    // Test simple serialization
    std::string simpleStr = config->toSimple();
    assert(!simpleStr.empty());
    
    std::cout << "    testConfigSerialization passed" << std::endl;
}

void testConfigValidation() {
    auto config = nlm::ConfigFactory::createDefaultConfig();
    
    // Add some values
    config->set("int_key", 42, nlm::ConfigSource::Runtime);
    config->set("string_key", std::string("hello"), nlm::ConfigSource::Runtime);
    
    // Test validation (should pass)
    bool isValid = config->validate();
    assert(isValid);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigProfiles() {
    nlm::ConfigProfile profile;
    profile.name = "test_profile";
    profile.description = "Test profile for configuration";
    profile.requiredKeys = {"int_key", "string_key"};
    
    profile.defaults["int_key"] = nlm::ConfigValue::create(100);
    profile.defaults["string_key"] = nlm::ConfigValue::create(std::string("default"));
    
    auto config = nlm::ConfigFactory::createDefaultConfig();
    bool success = config->applyProfile(profile);
    assert(success);
    
    assert(config->has("int_key"));
    assert(config->getOr<int>("int_key", 0) == 100);
    
    assert(config->has("string_key"));
    assert(config->getOr<std::string>("string_key", "") == "default");
    
    std::cout << "    testConfigProfiles passed" << std::endl;
}

void testConfigInheritance() {
    auto parent = nlm::ConfigFactory::createDefaultConfig();
    parent->set("parent_key", 100, nlm::ConfigSource::Default);
    
    auto child = nlm::ConfigFactory::createDefaultConfig();
    child->set("child_key", 200, nlm::ConfigSource::Runtime);
    
    bool success = child->inheritFrom(*parent, {"parent_key"});
    assert(success);
    
    assert(child->has("parent_key"));
    assert(child->has("child_key"));
    assert(child->getOr<int>("parent_key", 0) == 100);
    assert(child->getOr<int>("child_key", 0) == 200);
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void testConfigSnapshotRestore() {
    auto config = nlm::ConfigFactory::createDefaultConfig();
    config->set("test_key", 42, nlm::ConfigSource::Runtime);
    config->set("string_key", std::string("hello"), nlm::ConfigSource::Runtime);
    
    // Save snapshot
    std::string snapshotFile = "test_snapshot.json";
    bool saveSuccess = config->saveSnapshot(snapshotFile);
    assert(saveSuccess);
    
    // Create new config and restore snapshot
    auto newConfig = nlm::ConfigFactory::createDefaultConfig();
    bool restoreSuccess = newConfig->restoreSnapshot(snapshotFile);
    assert(restoreSuccess);
    
    assert(newConfig->has("test_key"));
    assert(newConfig->has("string_key"));
    assert(newConfig->getOr<int>("test_key", 0) == 42);
    assert(newConfig->getOr<std::string>("string_key", "") == "hello");
    
    // Clean up
    std::remove(snapshotFile.c_str());
    
    std::cout << "    testConfigSnapshotRestore passed" << std::endl;
}

void testConfigChangeListener() {
    class TestListener : public nlm::ConfigChangeListener {
    public:
        std::vector<std::pair<std::string, nlm::ConfigValue>> changes;
        
        void onConfigChanged(const std::string& key, const nlm::ConfigValue& newValue, nlm::ConfigSource source) override {
            changes.emplace_back(key, newValue);
        }
    };
    
    auto listener = std::make_shared<TestListener>();
    
    auto config = nlm::ConfigFactory::createDefaultConfig();
    config->addListener(listener);
    
    config->set("test_key", 42, nlm::ConfigSource::Runtime);
    config->set("another_key", std::string("hello"), nlm::ConfigSource::Runtime);
    
    assert(listener->changes.size() == 2);
    assert(listener->changes[0].first == "test_key");
    assert(listener->changes[1].first == "another_key");
    
    config->removeListener(listener);
    
    std::cout << "    testConfigChangeListener passed" << std::endl;
}

void testConfigThreadSafety() {
    auto config = nlm::ConfigFactory::createDefaultConfig();
    
    config->set("thread_test_key", 42, nlm::ConfigSource::Runtime);
    
    // Test locking
    config->lock();
    assert(config->has("thread_test_key"));
    config->unlock();
    
    // Test try_lock
    bool lockSuccess = config->tryLock();
    assert(lockSuccess);
    config->unlock();
    
    std::cout << "    testConfigThreadSafety passed" << std::endl;
}

void testConfigSizeOperations() {
    auto config = nlm::ConfigFactory::createDefaultConfig();
    
    assert(config->getSize() == 0);
    
    config->set("key1", 1, nlm::ConfigSource::Runtime);
    config->set("key2", 2, nlm::ConfigSource::Runtime);
    config->set("key3", 3, nlm::ConfigSource::Runtime);
    
    assert(config->getSize() == 3);
    
    config->reserve(100);
    
    std::cout << "    testConfigSizeOperations passed" << std::endl;
}

void testConfigCommandLineParsing() {
    // Test parsing command line arguments
    const int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--config_file=test.cfg",
        (char*)"--int_param=42",
        (char*)"--string_param=hello",
        (char*)"--bool_param=true"
    };
    
    auto config = nlm::ConfigParser::parseArgs(argc, argv);
    assert(config != nullptr);
    
    assert(config->has("config_file"));
    assert(config->getOr<std::string>("config_file", "") == "test.cfg");
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

void runAll() {
    testConfigValueTypes();
    testConfigCreationWithFactory();
    testConfigJsonParsing();
    testConfigYamlParsing();
    testConfigSimpleParsing();
    testConfigSerialization();
    testConfigValidation();
    testConfigProfiles();
    testConfigInheritance();
    testConfigSnapshotRestore();
    testConfigChangeListener();
    testConfigThreadSafety();
    testConfigSizeOperations();
    testConfigCommandLineParsing();
    
    std::cout << std::endl;
    std::cout << "All advanced config tests passed!" << std::endl;
}

} // namespace test_config_advanced