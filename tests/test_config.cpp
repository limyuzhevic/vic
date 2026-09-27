// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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

void testConfigKeyValueFormat() {
    nlm::Config config;
    
    // Create a test key=value file
    std::ofstream file("test_keyvalue.cfg");
    file << "# Test key=value file\n";
    file << "key1 = value1\n";
    file << "key2 = 42\n";
    file << "key3 = true\n";
    file << "key4 = 3.14\n";
    file.close();
    
    // Load from file
    assert(config.loadFromFile("test_keyvalue.cfg"));
    
    // Check values
    assert(config.get<std::string>("key1").value() == "value1");
    assert(config.get<int64_t>("key2").value() == 42);
    assert(config.get<bool>("key3").value() == true);
    assert(config.get<double>("key4").value() == 3.14);
    
    // Clean up
    std::remove("test_keyvalue.cfg");
    
    std::cout << "    testConfigKeyValueFormat passed" << std::endl;
}

void testConfigJSONFormat() {
    nlm::Config config;
    
    // Create a test JSON file
    std::ofstream file("test_config.json");
    file << "{\n";
    file << "    \"json_key1\": \"json_value1\",\n";
    file << "    \"json_key2\": 123,\n";
    file << "    \"json_key3\": true,\n";
    file << "    \"json_key4\": 3.14,\n";
    file << "    \"json_key5\": [1, 2, 3]\n";
    file << "}\n";
    file.close();
    
    // Load from file
    assert(config.loadFromFile("test_config.json"));
    
    // Check values
    assert(config.get<std::string>("json_key1").value() == "json_value1");
    assert(config.get<int64_t>("json_key2").value() == 123);
    assert(config.get<bool>("json_key3").value() == true);
    assert(config.get<double>("json_key4").value() == 3.14);
    // Note: JSON array parsing may need enhancement
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigJSONFormat passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Set some test values
    config.set("save_test1", std::string("test_value"), nlm::ConfigSource::Runtime);
    config.set("save_test2", 42, nlm::ConfigSource::Runtime);
    config.set("save_test3", 3.14, nlm::ConfigSource::Runtime);
    config.set("save_test4", true, nlm::ConfigSource::Runtime);
    
    // Save to key=value file
    assert(config.saveToFile("test_save.cfg"));
    
    // Clear and reload to verify
    config.clear();
    assert(config.loadFromFile("test_save.cfg"));
    
    // Check values
    assert(config.get<std::string>("save_test1").value() == "test_value");
    assert(config.get<int64_t>("save_test2").value() == 42);
    assert(config.get<double>("save_test3").value() == 3.14);
    assert(config.get<bool>("save_test4").value() == true);
    
    // Clean up
    std::remove("test_save.cfg");
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigImportExport() {
    nlm::Config config;
    
    // Set some test values
    config.set("import_export1", std::string("test1"), nlm::ConfigSource::Runtime);
    config.set("import_export2", 100, nlm::ConfigSource::Runtime);
    config.set("import_export3", 2.718, nlm::ConfigSource::Runtime);
    
    // Export to JSON
    assert(config.exportToFile("test_import_export.json"));
    
    // Clear and import back
    config.clear();
    assert(config.import("test_import_export.json"));
    
    // Verify values
    assert(config.get<std::string>("import_export1").value() == "test1");
    assert(config.get<int64_t>("import_export2").value() == 100);
    assert(config.get<double>("import_export3").value() == 2.718);
    
    // Clean up
    std::remove("test_import_export.json");
    
    std::cout << "    testConfigImportExport passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Add some values
    config.set("val1", 1, nlm::ConfigSource::Runtime);
    config.set("val2", "test", nlm::ConfigSource::Runtime);
    
    // Test basic validation
    assert(config.validate());
    
    // Test adding required keys
    config.addRequiredKey("val1");
    config.addRequiredKey("val2");
    config.addRequiredKey("val3"); // This doesn't exist
    
    // Should fail validation because val3 is missing
    assert(!config.isValid());
    
    // Add the missing key
    config.set("val3", 3, nlm::ConfigSource::Runtime);
    
    // Should now pass validation
    assert(config.isValid());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigValidators() {
    nlm::Config config;
    
    // Add a custom validator for positive numbers
    auto isPositive = [](const nlm::ConfigValue& value) -> bool {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) > 0;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) > 0.0;
        }
        return false;
    };
    
    config.addValidator("positive_value", isPositive);
    config.set("positive_value", 10, nlm::ConfigSource::Runtime);
    
    // Test validator
    auto val = config.get<int64_t>("positive_value");
    assert(val.has_value() && val.value() > 0);
    
    std::cout << "    testConfigValidators passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    // Set values with different sources
    config.set("source_default", 1, nlm::ConfigSource::Default);
    config.set("source_file", 2, nlm::ConfigSource::File);
    config.set("source_cli", 3, nlm::ConfigSource::CommandLine);
    config.set("source_runtime", 4, nlm::ConfigSource::Runtime);
    
    // Check sources
    assert(config.getSource("source_default") == nlm::ConfigSource::Default);
    assert(config.getSource("source_file") == nlm::ConfigSource::File);
    assert(config.getSource("source_cli") == nlm::ConfigSource::CommandLine);
    assert(config.getSource("source_runtime") == nlm::ConfigSource::Runtime);
    
    // Test filtering by source
    auto defaultEntries = config.getEntriesBySource(nlm::ConfigSource::Default);
    assert(defaultEntries.size() == 1);
    assert(defaultEntries[0].key == "source_default");
    
    auto cliEntries = config.getEntriesBySource(nlm::ConfigSource::CommandLine);
    assert(cliEntries.size() == 1);
    assert(cliEntries[0].key == "source_cli");
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigSizeAndEmpty() {
    nlm::Config config;
    
    assert(config.empty());
    assert(config.size() == 0);
    
    config.set("test1", 1, nlm::ConfigSource::Runtime);
    assert(!config.empty());
    assert(config.size() == 1);
    
    config.set("test2", 2, nlm::ConfigSource::Runtime);
    assert(config.size() == 2);
    
    config.clear();
    assert(config.empty());
    assert(config.size() == 0);
    
    std::cout << "    testConfigSizeAndEmpty passed" << std::endl;
}

void testConfigGetAllEntries() {
    nlm::Config config;
    
    config.set("entry1", 1, nlm::ConfigSource::Runtime);
    config.set("entry2", "test", nlm::ConfigSource::File);
    config.set("entry3", 3.14, nlm::ConfigSource::CommandLine);
    
    auto allEntries = config.getAllEntries();
    assert(allEntries.size() == 3);
    
    // Check that all entries are present
    bool hasEntry1 = false, hasEntry2 = false, hasEntry3 = false;
    for (const auto& entry : allEntries) {
        if (entry.key == "entry1") hasEntry1 = true;
        if (entry.key == "entry2") hasEntry2 = true;
        if (entry.key == "entry3") hasEntry3 = true;
    }
    
    assert(hasEntry1 && hasEntry2 && hasEntry3);
    
    std::cout << "    testConfigGetAllEntries passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("common_key", 1, nlm::ConfigSource::Runtime);
    config1.set("key1", "value1", nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("common_key", 2, nlm::ConfigSource::File); // Should overwrite
    config2.set("key2", "value2", nlm::ConfigSource::File);
    
    // Merge config2 into config1
    config1.merge(config2, nlm::ConfigSource::File);
    
    // Check merged values
    assert(config1.get<int64_t>("common_key").value() == 2); // From config2
    assert(config1.get<std::string>("key1").value() == "value1"); // From config1
    assert(config1.get<std::string>("key2").value() == "value2"); // From config2
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigGetFileExtension() {
    // Test file extension detection
    assert(nlm::Config::getFileExtension("config.json") == ".json");
    assert(nlm::Config::getFileExtension("config.yaml") == ".yaml");
    assert(nlm::Config::getFileExtension("config.yml") == ".yml");
    assert(nlm::Config::getFileExtension("config.cfg") == ".cfg");
    assert(nlm::Config::getFileExtension("config.txt") == ".txt");
    assert(nlm::Config::getFileExtension("config.unknown") == ".unknown");
    
    std::cout << "    testConfigGetFileExtension passed" << std::endl;
}

void testConfigSupportedExtensions() {
    auto extensions = nlm::Config::getSupportedExtensions();
    assert(std::find(extensions.begin(), extensions.end(), ".json") != extensions.end());
    assert(std::find(extensions.begin(), extensions.end(), ".yaml") != extensions.end());
    assert(std::find(extensions.begin(), extensions.end(), ".yml") != extensions.end());
    assert(std::find(extensions.begin(), extensions.end(), ".cfg") != extensions.end());
    assert(std::find(extensions.begin(), extensions.end(), ".txt") != extensions.end());
    
    std::cout << "    testConfigSupportedExtensions passed" << std::endl;
}

void testConfigDescription() {
    nlm::Config config;
    
    config.set("desc_test", 1, nlm::ConfigSource::Runtime);
    config.setDescription("desc_test", "Test description for verification");
    
    assert(config.getDescription("desc_test") == "Test description for verification");
    
    std::cout << "    testConfigDescription passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigKeyValueFormat();
    testConfigJSONFormat();
    testConfigSaveToFile();
    testConfigImportExport();
    testConfigValidation();
    testConfigValidators();
    testConfigSourceTracking();
    testConfigSizeAndEmpty();
    testConfigGetAllEntries();
    testConfigMerge();
    testConfigGetFileExtension();
    testConfigSupportedExtensions();
    testConfigDescription();
    
    std::cout << "\nAll config tests passed!" << std::endl;
}

} // namespace test_config
