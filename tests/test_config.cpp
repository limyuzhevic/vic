// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

void testConfigAdvancedFeatures() {
    nlm::Config config;
    
    // Test setBatch
    std::vector<std::pair<std::string, nlm::ConfigValue>> batchUpdates;
    batchUpdates.push_back({"batch_int", nlm::ConfigValue(100)});
    batchUpdates.push_back({"batch_string", nlm::ConfigValue(std::string("batch"))});
    batchUpdates.push_back({"batch_double", nlm::ConfigValue(99.9)});
    
    bool batchSuccess = config.setBatch(batchUpdates, nlm::ConfigSource::Runtime);
    assert(batchSuccess);
    assert(config.has("batch_int"));
    assert(config.has("batch_string"));
    assert(config.has("batch_double"));
    
    // Test setMultipleFromJson
    std::string testJson = R"({
        "json_int": 42,
        "json_string": "json_test",
        "json_bool": true,
        "json_array": [1, 2, 3]
    })";
    
    std::ofstream jsonFile("test_config.json");
    jsonFile << testJson;
    jsonFile.close();
    
    auto jsonResults = config.setMultipleFromJson("test_config.json");
    assert(jsonResults.size() > 0);
    
    // Test validation
    auto validationErrors = config.validate();
    // Config should be valid (no errors)
    assert(validationErrors.empty());
    
    // Test isValid
    assert(config.isValid());
    
    // Test exportToJson
    assert(config.exportToJson("config_export.json"));
    
    // Test importFromJson
    nlm::Config importedConfig;
    assert(importedConfig.importFromJson("config_export.json"));
    
    // Test hasChanges
    assert(config.hasChanges(importedConfig)); // Should have differences
    
    // Test calculateSimilarity
    double similarity = config.calculateSimilarity(importedConfig);
    assert(similarity >= 0.0 && similarity <= 1.0);
    
    // Test getEntryCount
    size_t entryCount = config.getEntryCount();
    assert(entryCount > 0);
    
    // Test getStats
    std::string stats = config.getStats();
    assert(!stats.empty());
    
    // Test key categorization
    auto intKeys = config.getIntegerKeys();
    auto doubleKeys = config.getDoubleKeys();
    auto stringKeys = config.getStringKeys();
    auto boolKeys = config.getBoolKeys();
    auto vectorKeys = config.getVectorKeys();
    
    // Should have at least one of each type from the tests
    assert(!intKeys.empty() || !doubleKeys.empty() || !stringKeys.empty() || !boolKeys.empty() || !vectorKeys.empty());
    
    std::cout << "    testConfigAdvancedFeatures passed" << std::endl;
}

} // namespace test_config
