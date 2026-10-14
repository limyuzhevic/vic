// Test the new configuration system
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

void testConfigurationSystem() {
    std::cout << "Testing NLM Configuration System..." << std::endl;
    
    // Test 1: Basic configuration loading
    std::cout << "Test 1: Loading JSON configuration..." << std::endl;
    auto config1 = std::make_shared<nlm::Config>();
    
    // Create a temporary JSON config file
    std::string jsonContent = R"({
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "dopamine_baseline": 0.1,
        "log_level": "INFO",
        "visualization_enabled": false
    })";
    
    std::ofstream tempFile("temp_config.json");
    tempFile << jsonContent;
    tempFile.close();
    
    assert(config1->loadFromJson("temp_config.json"));
    assert(config1->getOr<int64_t>("random_seed", 0) == 42);
    assert(config1->getOr<double>("simulation_timestep", 1.0) == 0.001);
    assert(config1->getOr<int64_t>("neuron_count", 0) == 1000);
    assert(config1->getOr<double>("dopamine_baseline", 0.0) == 0.1);
    assert(config1->getOr<std::string>("log_level", "") == "INFO");
    assert(config1->getOr<bool>("visualization_enabled", true) == false);
    
    std::cout << "✓ JSON configuration loaded successfully" << std::endl;
    
    // Test 2: Configuration validation
    std::cout << "\nTest 2: Configuration validation..." << std::endl;
    auto config2 = std::make_shared<nlm::Config>();
    
    // Load from JSON string
    assert(config2->loadFromJsonString(R"({"random_seed": 123, "neuron_count": 500})"));
    
    // Test validation
    assert(config2->has("random_seed"));
    assert(config2->has("neuron_count"));
    assert(config2->getOr<int64_t>("random_seed", 0) == 123);
    assert(config2->getOr<int64_t>("neuron_count", 0) == 500);
    
    // Test invalid key
    assert(!config2->has("nonexistent_key"));
    assert(!config2->getOr<int64_t>("nonexistent_key", -1) == -1);
    
    std::cout << "✓ Configuration validation working" << std::endl;
    
    // Test 3: Configuration export/import
    std::cout << "\nTest 3: Configuration export/import..." << std::endl;
    auto config3 = std::make_shared<nlm::Config>();
    
    // Set values
    config3->set("random_seed", int64_t(999));
    config3->set("neuron_count", 2000);
    config3->set("visualization_enabled", true);
    
    // Export to JSON
    std::string exportedJson = config3->exportToJson();
    std::cout << "Exported JSON: " << exportedJson << std::endl;
    
    // Import from JSON
    auto config4 = std::make_shared<nlm::Config>();
    assert(config4->loadFromJsonString(exportedJson));
    assert(config4->getOr<int64_t>("random_seed", 0) == 999);
    assert(config4->getOr<int64_t>("neuron_count", 0) == 2000);
    assert(config4->getOr<bool>("visualization_enabled", false) == true);
    
    std::cout << "✓ Configuration export/import working" << std::endl;
    
    // Test 4: Default schema
    std::cout << "\nTest 4: Default schema validation..." << std::endl;
    auto schema = nlm::Config::getDefaultSchema();
    assert(!schema.empty());
    
    // Check that required keys exist
    bool hasRandomSeed = false;
    bool hasNeuronCount = false;
    for (const auto& entry : schema) {
        if (entry.key == "random_seed") hasRandomSeed = true;
        if (entry.key == "neuron_count") hasNeuronCount = true;
    }
    assert(hasRandomSeed);
    assert(hasNeuronCount);
    
    std::cout << "✓ Default schema working" << std::endl;
    
    // Test 5: Health score
    std::cout << "\nTest 5: Configuration health score..." << std::endl;
    auto config5 = std::make_shared<nlm::Config>();
    
    int healthScore = config5->getStats().validEntries;
    std::cout << "Health score: " << healthScore << std::endl;
    
    // Add some valid entries
    config5->set("random_seed", int64_t(42));
    config5->set("neuron_count", 1000);
    
    healthScore = config5->getHealthScore();
    std::cout << "Updated health score: " << healthScore << std::endl;
    assert(healthScore > 0);
    
    std::cout << "✓ Health score calculation working" << std::endl;
    
    // Test 6: Deprecated keys migration
    std::cout << "\nTest 6: Deprecated keys migration..." << std::endl;
    auto config6 = std::make_shared<nlm::Config>();
    
    // Try to set deprecated key (should migrate to new key)
    config6->set("dopamine_baseline", 0.5);
    config6->set("stdp_time_constant", 20.0);
    
    // Check that new keys are set
    assert(config6->getOr<double>("neuromodulation_baseline", 0.0) == 0.5);
    assert(config6->getOr<double>("stdp_time_constant", 0.0) == 20.0);
    
    std::cout << "✓ Deprecated keys migration working" << std::endl;
    
    // Test 7: Configuration compression
    std::cout << "\nTest 7: Configuration compression..." << std::endl;
    auto config7 = std::make_shared<nlm::Config>();
    
    // Add many entries for compression
    for (int i = 0; i < 20; ++i) {
        config7->set("key_" + std::to_string(i), double(i * 0.1));
    }
    
    bool compressed = config7->compressConfiguration();
    std::cout << "Compression applied: " << (compressed ? "Yes" : "No") << std::endl;
    
    // Decompress
    assert(config7->decompressConfiguration());
    
    std::cout << "✓ Configuration compression working" << std::endl;
    
    // Cleanup
    std::remove("temp_config.json");
    
    std::cout << "\n✓ All configuration system tests passed!" << std::endl;
}

int main() {
    testConfigurationSystem();
    return 0;
}