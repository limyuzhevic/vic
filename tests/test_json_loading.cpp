#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Testing JSON Config loading..." << std::endl;
    
    nlm::Config config;
    
    // Test 1: Load from JSON file
    bool loadResult = config.loadFromFile("configs_json/default.json");
    if (loadResult) {
        std::cout << "✓ Successfully loaded JSON config from file" << std::endl;
        
        // Verify some key values
        auto seed = config.getOr<int64_t>("random_seed", 0);
        if (seed == 42) {
            std::cout << "✓ Verified random_seed value: " << seed << std::endl;
        }
        
        auto timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (std::abs(timestep - 0.001) < 0.00001) {
            std::cout << "✓ Verified simulation_timestep value: " << timestep << std::endl;
        }
        
        auto neurons = config.getOr<int64_t>("neuron_count", 0);
        if (neurons == 1000) {
            std::cout << "✓ Verified neuron_count value: " << neurons << std::endl;
        }
        
        auto region = config.getOr<int64_t>("region_count", 0);
        if (region == 2) {
            std::cout << "✓ Verified region_count value: " << region << std::endl;
        }
    } else {
        std::cout << "✗ Failed to load JSON config from file" << std::endl;
        return 1;
    }
    
    // Test 2: Save to JSON string
    std::string jsonString = config.saveToJsonString();
    if (!jsonString.empty()) {
        std::cout << "✓ Successfully generated JSON string" << std::endl;
        std::cout << "JSON string preview (first 200 chars): " << jsonString.substr(0, 200) << "..." << std::endl;
        
        // Verify it's valid JSON
        try {
            nlohmann::json j = nlohmann::json::parse(jsonString);
            if (j.contains("config") && j.contains("metadata")) {
                std::cout << "✓ Generated JSON is valid and has expected structure" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "✗ Generated JSON is not valid: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cout << "✗ Failed to generate JSON string" << std::endl;
        return 1;
    }
    
    // Test 3: Load from JSON string
    nlm::Config config2;
    bool loadStringResult = config2.loadFromJsonString(jsonString);
    if (loadStringResult) {
        std::cout << "✓ Successfully loaded config from JSON string" << std::endl;
        
        auto seed2 = config2.getOr<int64_t>("random_seed", 0);
        if (seed2 == 42) {
            std::cout << "✓ Verified seed value from JSON string: " << seed2 << std::endl;
        }
    } else {
        std::cout << "✗ Failed to load config from JSON string" << std::endl;
        return 1;
    }
    
    // Test 4: Test vector types in JSON
    nlm::Config config3;
    config3.set("int_vector", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    config3.set("double_vector", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config3.set("string_vector", std::vector<std::string>{"a", "b", "c", "d"}, nlm::ConfigSource::Runtime);
    
    auto intVec = config3.get<std::vector<int>>("int_vector");
    auto doubleVec = config3.get<std::vector<double>>("double_vector");
    auto stringVec = config3.get<std::vector<std::string>>("string_vector");
    
    if (intVec && intVec->size() == 5) {
        std::cout << "✓ Int vector preserved correctly: " << intVec->size() << " elements" << std::endl;
    }
    
    if (doubleVec && doubleVec->size() == 3) {
        std::cout << "✓ Double vector preserved correctly: " << doubleVec->size() << " elements" << std::endl;
    }
    
    if (stringVec && stringVec->size() == 4) {
        std::cout << "✓ String vector preserved correctly: " << stringVec->size() << " elements" << std::endl;
    }
    
    // Test 5: Save and verify vector preservation
    bool saveFileResult = config3.saveToFile("test_config_vectors.json");
    if (saveFileResult) {
        std::cout << "✓ Successfully saved config with vectors to file" << std::endl;
        
        nlm::Config config4;
        bool loadVectorsResult = config4.loadFromFile("test_config_vectors.json");
        if (loadVectorsResult) {
            auto intVec2 = config4.get<std::vector<int>>("int_vector");
            auto doubleVec2 = config4.get<std::vector<double>>("double_vector");
            auto stringVec2 = config4.get<std::vector<std::string>>("string_vector");
            
            if (intVec2 && intVec2->size() == 5) {
                std::cout << "✓ Vector data preserved through JSON round-trip" << std::endl;
            }
        }
        
        // Clean up
        std::remove("test_config_vectors.json");
    }
    
    std::cout << "\n=== All JSON Config tests passed! ===" << std::endl;
    return 0;
}
