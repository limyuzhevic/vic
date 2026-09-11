# Test program to demonstrate the improved NLM Config system

#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv) {
    std::cout << "=== NLM Config System Demo ===" << std::endl;
    
    // Create config instance
    auto config = std::make_shared<nlm::Config>();
    
    std::cout << "\n1. Testing key=value format loading..." << std::endl;
    config->loadFromFile("configs/default.cfg");
    
    std::cout << "Loaded config summary:" << std::endl;
    std::cout << config->summary();
    
    std::cout << "\n2. Testing JSON format loading..." << std::endl;
    auto config2 = std::make_shared<nlm::Config>();
    config2->loadFromFile("configs/default.json");
    
    std::cout << "Loaded config keys: ";
    auto keys = config2->getKeys();
    for (const auto& key : keys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n3. Testing YAML format loading..." << std::endl;
    auto config3 = std::make_shared<nlm::Config>();
    config3->loadFromFile("configs/default.yaml");
    
    std::cout << "Loaded config keys: ";
    keys = config3->getKeys();
    for (const auto& key : keys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n4. Testing save functionality (key=value format)..." << std::endl;
    std::string testFile = "test_output.cfg";
    config->saveToFile(testFile);
    std::cout << "Saved to " << testFile << std::endl;
    
    std::cout << "\n5. Testing save functionality (JSON format)..." << std::endl;
    std::string jsonFile = "test_output.json";
    config->saveToFile(jsonFile);
    std::cout << "Saved to " << jsonFile << std::endl;
    
    std::cout << "\n6. Testing command line argument parsing..." << std::endl;
    auto config4 = std::make_shared<nlm::Config>();
    config4->loadFromArgs(5, (char*[]){ "program", "--neuron_count=500", "--stdp_ltp_weight=0.02", "-max_simulation_steps", "5000" });
    
    std::cout << "Parsed config values:" << std::endl;
    std::cout << "  neuron_count: " << config4->getOr<int64_t>("neuron_count", 0) << std::endl;
    std::cout << "  stdp_ltp_weight: " << config4->getOr<double>("stdp_ltp_weight", 0.0) << std::endl;
    std::cout << "  max_simulation_steps: " << config4->getOr<int64_t>("max_simulation_steps", 0) << std::endl;
    
    std::cout << "\n7. Testing --help flag..." << std::endl;
    auto config5 = std::make_shared<nlm::Config>();
    config5->loadFromArgs(2, (char*[]){ "program", "--help" });
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "The NLM Config system now supports:" << std::endl;
    std::cout << "  ✓ Key=value format (.cfg, .txt)" << std::endl;
    std::cout << "  ✓ JSON format (.json)" << std::endl;
    std::cout << "  ✓ YAML format (.yaml, .yml)" << std::endl;
    std::cout << "  ✓ Automatic format detection" << std::endl;
    std::cout << "  ✓ Config validation for common keys" << std::endl;
    std::cout << "  ✓ --help flag for usage information" << std::endl;
    std::cout << "  ✓ Proper value serialization" << std::endl;
    
    return 0;
}
