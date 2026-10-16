#include <iostream>
#include <cassert>
#include <filesystem>
#include "core/Config/Config.hpp"

int main() {
    std::cout << "=== Testing NLM Configuration System Improvements ===" << std::endl;
    
    // Test 1: Basic config functionality
    std::cout << "\n1. Testing basic configuration operations...";
    auto config = std::make_shared<nlm::Config>();
    
    config->set("test.key1", std::string("value1"));
    config->set("test.key2", 42);
    config->set("test.key3", 3.14);
    
    assert(config->get<std::string>("test.key1").value() == "value1");
    assert(config->get<int>("test.key2").value() == 42);
    assert(config->get<double>("test.key3").value() == 3.14);
    
    std::cout << " PASSED" << std::endl;
    
    // Test 2: File save/load (simple format)
    std::cout << "\n2. Testing file save/load (simple format)...";
    std::string simpleFile = "/tmp/test_config_simple.cfg";
    assert(config->saveToSimpleFile(simpleFile));
    
    auto config2 = std::make_shared<nlm::Config>();
    assert(config2->loadFromFile(simpleFile));
    
    assert(config2->get<std::string>("test.key1").value() == "value1");
    assert(config2->get<int>("test.key2").value() == 42);
    assert(config2->get<double>("test.key3").value() == 3.14);
    
    std::cout << " PASSED" << std::endl;
    
    // Test 3: JSON save/load
    std::cout << "\n3. Testing JSON save/load...";
    std::string jsonFile = "/tmp/test_config.json";
    assert(config->saveToJSON(jsonFile));
    
    auto config3 = std::make_shared<nlm::Config>();
    assert(config3->loadFromFile(jsonFile));
    
    assert(config3->get<std::string>("test.key1").value() == "value1");
    
    std::cout << " PASSED" << std::endl;
    
    // Test 4: Config summary
    std::cout << "\n4. Testing configuration summary...";
    std::string summary = config->summary();
    assert(summary.find("Configuration (3 entries):") != std::string::npos);
    assert(summary.find("test.key1 = \"value1\"") != std::string::npos);
    
    std::cout << " PASSED" << std::endl;
    
    // Test 5: Key management
    std::cout << "\n5. Testing key management...";
    config->set("temp.key", std::string("temp_value"));
    assert(config->getKeys().size() == 4);
    config->remove("temp.key");
    assert(config->getKeys().size() == 3);
    
    std::cout << " PASSED" << std::endl;
    
    // Clean up
    std::filesystem::remove("/tmp/test_config_simple.cfg");
    std::filesystem::remove("/tmp/test_config.json");
    
    std::cout << "\n=== All Configuration System Tests PASSED ===" << std::endl;
    std::cout << "\nImprovements made:" << std::endl;
    std::cout << "  ✓ Fixed saveToFile bug (now saves actual values, not PLACEHOLDER_VALUE)" << std::endl;
    std::cout << "  ✓ Added JSON file format support" << std::endl;
    std::cout << "  ✓ Added YAML file format support (with yaml-cpp)" << std::endl;
    std::cout << "  ✓ Added saveToSimpleFile() method" << std::endl;
    std::cout << "  ✓ Added saveToJSON() method" << std::endl;
#ifdef YAML_CPP_AVAILABLE
    std::cout << "  ✓ Added saveToYAML() method" << std::endl;
#endif
    std::cout << "  ✓ Improved file format auto-detection" << std::endl;
    std::cout << "  ✓ Enhanced value preservation during save/load cycles" << std::endl;
    
    return 0;
}
