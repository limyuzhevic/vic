#include "core/Config/Config.hpp"
#include "core/AdvancedConfig.hpp"
#include <iostream>
#include <cassert>
#include <fstream>

void testBasicFunctionality() {
    nlm::Config config;
    
    // Test basic set/get
    config.set("host", "localhost", nlm::ConfigSource::Runtime);
    config.set("port", 8080, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("host"));
    assert(config.has("port"));
    assert(config.has("enabled"));
    
    auto host = config.get<std::string>("host");
    auto port = config.get<int>("port");
    auto enabled = config.get<bool>("enabled");
    
    assert(host && *host == "localhost");
    assert(port && *port == 8080);
    assert(enabled && *enabled == true);
    
    std::cout << "✓ Basic functionality test passed" << std::endl;
}

void testGetOr() {
    nlm::Config config;
    
    // Test missing key returns default
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    // Test existing key returns actual value
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "✓ GetOr test passed" << std::endl;
}

void testBackwardCompatibility() {
    nlm::Config config;
    
    // Test legacy file loading format
    std::ofstream legacyFile("test_legacy.cfg");
    legacyFile << "# Legacy configuration file\n";
    legacyFile << "host = localhost\n";
    legacyFile << "port = 8080\n";
    legacyFile << "enabled = true\n";
    legacyFile.close();
    
    bool loaded = config.loadFromFile("test_legacy.cfg");
    assert(loaded);
    
    // Test legacy format parsing
    assert(config.has("host"));
    assert(config.has("port"));
    assert(config.has("enabled"));
    
    auto host = config.get<std::string>("host");
    auto port = config.get<int>("port");
    auto enabled = config.get<bool>("enabled");
    
    assert(host && *host == "localhost");
    assert(port && *port == 8080);
    assert(enabled && *enabled == true);
    
    // Clean up
    std::remove("test_legacy.cfg");
    
    std::cout << "✓ Backward compatibility test passed" << std::endl;
}

void testAdvancedConfig() {
    nlm::Config config;
    
    // Test groups
    nlm::ConfigGroup group("server", "Server configuration", 50);
    config.addGroup(group);
    
    config.set("host", "localhost", nlm::ConfigSource::Runtime);
    config.addToGroup("server", "host");
    
    auto foundGroup = config.getGroup("server");
    assert(foundGroup != nullptr);
    assert(foundGroup->name == "server");
    assert(foundGroup->description == "Server configuration");
    assert(foundGroup->priority == 50);
    
    // Test inheritance
    auto parent = std::make_shared<nlm::Config>();
    parent->set("default", "value", nlm::ConfigSource::Default);
    config.setParentConfig(parent);
    
    assert(config.getParentConfig() == parent);
    
    std::cout << "✓ Advanced config test passed" << std::endl;
}

int main() {
    try {
        testBasicFunctionality();
        testGetOr();
        testBackwardCompatibility();
        testAdvancedConfig();
        
        std::cout << "\n✅ All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}