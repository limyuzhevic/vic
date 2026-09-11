#include <iostream>
#include <cassert>
#include "core/Config/Config.hpp"

int main() {
    std::cout << "Testing Config file parser..." << std::endl;
    
    nlm::Config config;
    
    // Test JSON parsing
    std::cout << "Testing JSON file..." << std::endl;
    assert(config.loadFromFile("test_config.json"));
    assert(config.has("int_value"));
    assert(*config.get<int64_t>("int_value") == 42);
    assert(*config.get<double>("double_value") > 3.13 && *config.get<double>("double_value") < 3.15);
    assert(*config.get<std::string>("string_value") == "hello");
    assert(*config.get<bool>("bool_value") == true);
    std::cout << "  JSON parsing: PASS" << std::endl;
    
    // Test YAML parsing
    std::cout << "Testing YAML file..." << std::endl;
    assert(config.loadFromFile("test_config.yaml"));
    assert(config.has("int_value"));
    assert(*config.get<int64_t>("int_value") == 100);
    assert(*config.get<double>("double_value") > 2.717 && *config.get<double>("double_value") < 2.719);
    assert(*config.get<std::string>("string_value") == "world");
    assert(*config.get<bool>("bool_value") == false);
    std::cout << "  YAML parsing: PASS" << std::endl;
    
    // Test key=value parsing (backward compatibility)
    std::cout << "Testing key=value file..." << std::endl;
    assert(config.loadFromFile("test_config.cfg"));
    assert(config.has("int_value"));
    assert(*config.get<int64_t>("int_value") == 999);
    assert(*config.get<double>("double_value") > 1.413 && *config.get<double>("double_value") < 1.415);
    assert(*config.get<std::string>("string_value") == "test");
    assert(*config.get<bool>("bool_value") == true);
    std::cout << "  key=value parsing: PASS" << std::endl;
    
    // Test file type detection
    std::cout << "Testing file type detection..." << std::endl;
    nlm::Config config2;
    
    // Test .json extension
    assert(config2.loadFromFile("test_config.json"));
    assert(config2.has("int_value"));
    assert(*config2.get<int64_t>("int_value") == 42);
    std::cout << "  .json detection: PASS" << std::endl;
    
    // Test .yaml extension
    nlm::Config config3;
    assert(config3.loadFromFile("test_config.yaml"));
    assert(config3.has("int_value"));
    assert(*config3.get<int64_t>("int_value") == 100);
    std::cout << "  .yaml detection: PASS" << std::endl;
    
    // Test .yml extension
    std::ofstream yamlFile("test_yml.yml");
    yamlFile << "key: value";
    yamlFile.close();
    nlm::Config config4;
    assert(config4.loadFromFile("test_yml.yml"));
    assert(config4.has("key"));
    assert(*config4.get<std::string>("key") == "value");
    std::remove("test_yml.yml");
    std::cout << "  .yml detection: PASS" << std::endl;
    
    // Test .cfg extension
    nlm::Config config5;
    assert(config5.loadFromFile("test_config.cfg"));
    assert(config5.has("int_value"));
    assert(*config5.get<int64_t>("int_value") == 999);
    std::cout << "  .cfg detection: PASS" << std::endl;
    
    // Test .txt extension (should fallback to key=value)
    std::ofstream txtFile("test_config.txt");
    txtFile << "test_key = test_value";
    txtFile.close();
    nlm::Config config6;
    assert(config6.loadFromFile("test_config.txt"));
    assert(config6.has("test_key"));
    assert(*config6.get<std::string>("test_key") == "test_value");
    std::remove("test_config.txt");
    std::cout << "  .txt fallback: PASS" << std::endl;
    
    std::cout << std::endl << "All tests passed! Config file parser implementation successful." << std::endl;
    
    return 0;
}
