// NLM Configuration Examples
// Demonstrates file format conversion capabilities

#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    
    // Load configuration in different formats
    config.loadFromFile("basic.cfg", nlm::ConfigFileFormat::PlainText);
    std::cout << "Loaded from basic.cfg" << std::endl;
    
    config.loadFromFile("advanced.json", nlm::ConfigFileFormat::JSON);
    std::cout << "Loaded from advanced.json" << std::endl;
    
    config.loadFromFile("research.yml", nlm::ConfigFileFormat::YAML);
    std::cout << "Loaded from research.yml" << std::endl;
    
    // Save in different formats
    config.saveToFile("converted_basic.cfg", nlm::ConfigFileFormat::PlainText);
    config.saveToFile("converted_basic.json", nlm::ConfigFileFormat::JSON);
    config.saveToFile("converted_basic.yml", nlm::ConfigFileFormat::YAML);
    
    return 0;
}
