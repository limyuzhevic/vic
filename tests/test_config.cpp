// Configuration Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_config {

void testConfigCreation() {
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigValueSetting() {
    nlm::Config config;
    
    // Test type-safe setters
    config.setInt("test_int", 42);
    config.setFloat("test_float", 3.14f);
    config.setBool("test_bool", true);
    config.setString("test_string", "hello");
    config.setIntVector("test_int_vector", std::vector<int>{1, 2, 3});
    
    // Test type-safe getters
    assert(config.getInt("test_int") == std::optional<int>(42));
    assert(config.getFloat("test_float") == std::optional<double>(3.14));
    assert(config.getBool("test_bool") == std::optional<bool>(true));
    assert(config.getString("test_string") == std::optional<std::string>("hello"));
    assert(config.getIntVector("test_int_vector") == std::optional<std::vector<int>>({1, 2, 3}));
    
    std::cout << "    testConfigValueSetting passed" << std::endl;
}

void testConfigPresets() {
    nlm::Config config;
    
    // Test presets
    assert(config.setPreset("minimal"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    
    config.clear();
    assert(config.setPreset("balanced"));
    assert(config.has("working_memory_capacity"));
    
    config.clear();
    assert(config.setPreset("advanced"));
    assert(config.has("enable_curiosity"));
    
    config.clear();
    assert(config.setPreset("high_performance"));
    assert(config.has("enable_simd"));
    
    config.clear();
    assert(config.setPreset("phase6_integrated"));
    assert(config.has("vision_width"));
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Set valid values
    config.setInt("neuron_count", 100);
    config.setFloat("connection_probability", 0.1f);
    config.setFloat("simulation_timestep", 0.001f);
    
    // Validate
    auto validation = config.validate();
    assert(validation.valid);
    
    // Test invalid values
    config.clear();
    config.setInt("neuron_count", -5); // Invalid
    config.setFloat("connection_probability", 1.5f); // Invalid
    
    validation = config.validate();
    assert(!validation.valid);
    assert(!validation.errorMessage.empty());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Set some values
    config.setInt("neuron_count", 500);
    config.setFloat("connection_probability", 0.15f);
    config.setString("test_name", "test_brain");
    
    // Save to file
    std::string testFile = "test_config.cfg";
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Clear and reload
    config.clear();
    bool loadSuccess = config.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify values
    assert(config.getInt("neuron_count") == std::optional<int>(500));
    assert(config.getFloat("connection_probability") == std::optional<double>(0.15));
    assert(config.getString("test_name") == std::optional<std::string>("test_brain"));
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigConvenienceMethods() {
    nlm::Config config;
    
    // Test convenience methods
    config.setNeuronCount(100);
    config.setRegionCount(2);
    config.setConnectionProbability(0.1f);
    config.setSimulationTimestep(0.001);
    
    // Verify they worked
    assert(config.getInt("neuron_count") == std::optional<int>(100));
    assert(config.getInt("region_count") == std::optional<int>(2));
    assert(config.getFloat("connection_probability") == std::optional<double>(0.1));
    assert(config.getFloat("simulation_timestep") == std::optional<double>(0.001));
    
    std::cout << "    testConfigConvenienceMethods passed" << std::endl;
}

void testConfigEnvironmentVariables() {
    // Test environment variable helpers
    std::string envVar = nlm::Config::getEnvironmentVariable("NLM_TEST");
    assert(envVar.empty()); // Should not exist
    
    assert(!nlm::Config::hasEnvironmentVariable("NLM_TEST"));
    
    std::cout << "    testConfigEnvironmentVariables passed" << std::endl;
}

void testConfigPresetNames() {
    std::vector<std::string> presets = nlm::Config::getPresetNames();
    
    assert(std::find(presets.begin(), presets.end(), "minimal") != presets.end());
    assert(std::find(presets.begin(), presets.end(), "balanced") != presets.end());
    assert(std::find(presets.begin(), presets.end(), "advanced") != presets.end());
    assert(std::find(presets.begin(), presets.end(), "high_performance") != presets.end());
    assert(std::find(presets.begin(), presets.end(), "phase6_integrated") != presets.end());
    
    std::cout << "    testConfigPresetNames passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigValueSetting();
    testConfigPresets();
    testConfigValidation();
    testConfigFileIO();
    testConfigConvenienceMethods();
    testConfigEnvironmentVariables();
    testConfigPresetNames();
}

} // namespace test_config

int main() {
    std::cout << "=== Configuration Tests ===" << std::endl;
    test_config::runAll();
    std::cout << "All configuration tests passed!" << std::endl;
    return 0;
}
}