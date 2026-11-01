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

void testConfigTypeChecking() {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Test numeric type checking
    assert(config.isNumericType("int_val"));
    assert(config.isNumericType("double_val"));
    assert(!config.isNumericType("string_val"));
    assert(!config.isNumericType("bool_val"));
    assert(!config.isNumericType("int_array"));
    assert(!config.isNumericType("double_array"));
    assert(!config.isNumericType("string_array"));
    
    // Test string type checking
    assert(!config.isStringType("int_val"));
    assert(!config.isStringType("double_val"));
    assert(config.isStringType("string_val"));
    assert(!config.isStringType("bool_val"));
    assert(!config.isStringType("int_array"));
    assert(!config.isStringType("double_array"));
    assert(!config.isStringType("string_array"));
    
    // Test vector type checking
    assert(!config.isVectorType("int_val"));
    assert(!config.isVectorType("double_val"));
    assert(!config.isVectorType("string_val"));
    assert(!config.isVectorType("bool_val"));
    assert(config.isVectorType("int_array"));
    assert(config.isVectorType("double_array"));
    assert(config.isVectorType("string_array"));
    
    // Test vector size checking
    assert(config.getVectorSize("int_array") == 3);
    assert(config.getVectorSize("double_array") == 3);
    assert(config.getVectorSize("string_array") == 3);
    assert(config.getVectorSize("int_val") == 0);
    
    std::cout << "    testConfigTypeChecking passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", 100, nlm::ConfigSource::Runtime);
    config1.set("key2", "value1", nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", "value2", nlm::ConfigSource::File);
    config2.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    
    config1.merge(config2);
    
    // key1 should still exist with its original value
    assert(config1.get<int>("key1").has_value());
    assert(config1.get<int>("key1").value() == 100);
    
    // key2 should be overridden by config2 (File source)
    assert(config1.get<std::string>("key2").has_value());
    assert(config1.get<std::string>("key2").value() == "value2");
    
    // key3 should be added
    assert(config1.get<double>("key3").has_value());
    assert(config1.get<double>("key3").value() == 3.14);
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigHasType() {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", "hello", nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Test type-specific checking
    assert(config.hasType<int>("int_val"));
    assert(!config.hasType<double>("int_val"));
    assert(config.hasType<double>("double_val"));
    assert(config.hasType<std::string>("string_val"));
    assert(config.hasType<std::vector<int>>("int_array"));
    assert(!config.hasType<int>("int_array"));
    
    // Test non-existent keys
    assert(!config.hasType<int>("non_existent"));
    
    std::cout << "    testConfigHasType passed" << std::endl;
}

void testConfigStatistics() {
    nlm::Config config;
    
    config.set("int1", 1, nlm::ConfigSource::Default);
    config.set("int2", 2, nlm::ConfigSource::File);
    config.set("int3", 3, nlm::ConfigSource::CommandLine);
    config.set("int4", 4, nlm::ConfigSource::Runtime);
    config.set("str1", "hello", nlm::ConfigSource::Default);
    config.set("str2", "world", nlm::ConfigSource::File);
    config.set("bool1", true, nlm::ConfigSource::Runtime);
    config.set("vec1", std::vector<int>({1, 2, 3}), nlm::ConfigSource::CommandLine);
    config.set("vec2", std::vector<double>({1.1, 2.2}), nlm::ConfigSource::Runtime);
    
    auto stats = config.getStats();
    
    // Verify statistics
    assert(stats.totalEntries == 9);
    assert(stats.entriesBySource[0] == 3); // Default
    assert(stats.entriesBySource[1] == 2); // File
    assert(stats.entriesBySource[2] == 2); // CommandLine
    assert(stats.entriesBySource[3] == 2); // Runtime
    
    assert(stats.numericEntries == 7); // int1, int2, int3, int4, bool1, vec1, vec2
    assert(stats.stringEntries == 2);  // str1, str2
    assert(stats.vectorEntries == 2);  // vec1, vec2
    
    std::cout << "    testConfigStatistics passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello world", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Test save to file
    const std::string testFile = "/tmp/nlm_config_test.txt";
    bool saved = config.saveToFile(testFile);
    assert(saved);
    
    // Verify file content
    std::ifstream file(testFile);
    assert(file.is_open());
    
    std::string line;
    std::getline(file, line); // Skip description line
    std::getline(file, line); // Read int_value line
    assert(line.find("int_value = 42") != std::string::npos);
    
    std::getline(file, line); // Skip empty line
    std::getline(file, line); // Read double_value line
    assert(line.find("double_value = 3.14") != std::string::npos);
    
    std::getline(file, line); // Skip empty line
    std::getline(file, line); // Read string_value line
    assert(line.find("string_value = \"hello world\"") != std::string::npos);
    
    std::getline(file, line); // Skip empty line
    std::getline(file, line); // Read bool_value line
    assert(line.find("bool_value = true") != std::string::npos);
    
    std::getline(file, line); // Skip empty line
    std::getline(file, line); // Read int_array line
    assert(line.find("int_array = [1, 2, 3]") != std::string::npos);
    
    std::getline(file, line); // Skip empty line
    std::getline(file, line); // Read string_array line
    assert(line.find("string_array = [\"a\", \"b\", \"c\"]") != std::string::npos);
    
    file.close();
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigTypeChecking();
    testConfigMerge();
    testConfigHasType();
    testConfigStatistics();
    testConfigSaveToFile();
}

} // namespace test_config
