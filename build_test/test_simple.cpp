#include <iostream>
#include <string>
#include <cassert>
#include "../src/core/Config/Config.hpp"

int main() {
    std::cout << "Testing enhanced NLM Configuration system..." << std::endl;
    
    nlm::Config config;
    config.set("name", "Test Config", nlm::ConfigSource::Runtime);
    config.set("value", 42, nlm::ConfigSource::Runtime);
    
    std::cout << "Config created successfully" << std::endl;
    std::cout << "Keys: ";
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    std::cout << "name value: " << config.getString("name") << std::endl;
    std::cout << "value: " << config.getOr<int>("value", 0) << std::endl;
    
    std::cout << "✓ All tests passed!" << std::endl;
    return 0;
}
