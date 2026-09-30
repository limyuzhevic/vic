// Config Tests
#include "core/Config/Config.hpp"
#include "core/AdvancedConfig.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
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

// Test enhanced configuration system
void testAdvancedConfig() {
    nlm::Config config;
    
    // Test setting and getting with metadata
    config.set("host", "localhost", nlm::ConfigSource::Runtime);
    config.set("port", 8080, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("host"));
    assert(config.has("port"));
    assert(config.has("enabled"));
    
    auto host = config.get<std::string>("host");
    assert(host.has_value() && *host == "localhost");
    
    auto port = config.get<int>("port");
    assert(port.has_value() && *port == 8080);
    
    auto enabled = config.get<bool>("enabled");
    assert(enabled.has_value() && *enabled == true);
    
    // Test configuration summary
    std::string summary = config.summary();
    assert(summary.find("host") != std::string::npos);
    assert(summary.find("port") != std::string::npos);
    assert(summary.find("enabled") != std::string::npos);
    
    // Test configuration keys
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    assert(std::find(keys.begin(), keys.end(), "host") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "port") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "enabled") != keys.end());
    
    std::cout << "    testAdvancedConfig passed" << std::endl;
}

// Test group management
void testConfigGroups() {
    nlm::Config config;
    
    // Create a test group
    nlm::ConfigGroup group("server", "Server configuration", 50);
    config.addGroup(group);
    
    // Add entries to group
    config.set("host", "localhost", nlm::ConfigSource::Runtime);
    config.set("port", 8080, nlm::ConfigSource::Runtime);
    
    // Test group management
    auto groupNames = config.getGroupNames();
    assert(groupNames.size() == 1);
    assert(std::find(groupNames.begin(), groupNames.end(), "server") != groupNames.end());
    
    nlm::ConfigGroup* foundGroup = config.getGroup("server");
    assert(foundGroup != nullptr);
    assert(foundGroup->name == "server");
    
    // Test group member management
    config.addToGroup("server", "host");
    bool isInGroup = config.isInGroup("server", "host");
    assert(isInGroup);
    
    std::cout << "    testConfigGroups passed" << std::endl;
}

// Test backward compatibility
void testBackwardCompatibility() {
    nlm::Config config;
    
    // Test legacy file loading format
    std::ofstream legacyFile("legacy_config.txt");
    legacyFile << "# Legacy configuration file\n";
    legacyFile << "host = localhost\n";
    legacyFile << "port = 8080\n";
    legacyFile << "enabled = true\n";
    legacyFile.close();
    
    bool loaded = config.loadFromFile("legacy_config.txt");
    assert(loaded);
    
    // Test legacy format parsing
    assert(config.has("host"));
    assert(config.has("port"));
    assert(config.has("enabled"));
    
    auto host = config.get<std::string>("host");
    assert(host.has_value() && *host == "localhost");
    
    auto port = config.get<int>("port");
    assert(port.has_value() && *port == 8080);
    
    auto enabled = config.get<bool>("enabled");
    assert(enabled.has_value() && *enabled == true);
    
    // Clean up
    std::remove("legacy_config.txt");
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testAdvancedConfig();
    testConfigGroups();
    testBackwardCompatibility();
}

} // namespace test_config
