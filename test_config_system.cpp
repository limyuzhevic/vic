#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <filesystem>

#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"

using namespace nlm;

void printConfigSummary(const Config& config) {
    std::cout << config.summary() << std::endl;
}

void testBasicConfig() {
    std::cout << "=== Test 1: Basic Config Operations ===" << std::endl;
    
    // Create a default config
    Config config;
    
    // Set various types of values
    config.set("string_key", std::string("hello"));
    config.set("int_key", int(42));
    config.set("double_key", double(3.14159));
    config.set("bool_key", bool(true));
    config.set("int64_key", int64_t(1000000000));
    config.set("vector_int_key", std::vector<int>({1, 2, 3, 4, 5}));
    config.set("vector_string_key", std::vector<std::string>({"a", "b", "c"}));
    
    // Check existence
    std::cout << "Has 'string_key': " << config.has("string_key") << std::endl;
    std::cout << "Has 'nonexistent_key': " << config.has("nonexistent_key") << std::endl;
    
    // Get values
    if (auto val = config.get<std::string>("string_key")) {
        std::cout << "Got string_key: '" << *val << "'" << std::endl;
    }
    
    if (auto val = config.get<int>("int_key")) {
        std::cout << "Got int_key: " << *val << std::endl;
    }
    
    // Get with default
    std::string default_string = "default_value";
    std::string actual_string = config.getOr<std::string>("string_key", default_string);
    std::cout << "GotOr string_key: '" << actual_string << "' (default: '" << default_string << "')" << std::endl;
    
    std::cout << "" << std::endl;
}

void testJsonLoad() {
    std::cout << "=== Test 2: JSON Configuration Loading ===" << std::endl;
    
    Config config;
    
    // Load from our test JSON config
    std::string configPath = "configs/test_config.json";
    if (config.loadFromFile(configPath)) {
        std::cout << "Successfully loaded JSON config from: " << configPath << std::endl;
        std::cout << "Config summary:" << std::endl;
        printConfigSummary(config);
    } else {
        std::cout << "Failed to load JSON config from: " << configPath << std::endl;
    }
    
    std::cout << "" << std::endl;
}

void testJsonSave() {
    std::cout << "=== Test 3: JSON Configuration Saving ===" << std::endl;
    
    Config config;
    
    // Set some test values with descriptions
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("brain_neuron_count", 1000, ConfigSource::Default);
    config.set("plasticity_stdp_enable", true, ConfigSource::Default);
    config.set("neuromodulation_curiosity_enable", false, ConfigSource::Default);
    
    // Save to JSON file
    std::string outputPath = "configs/saved_config.json";
    if (config.saveToFile(outputPath)) {
        std::cout << "Successfully saved JSON config to: " << outputPath << std::endl;
        
        // Try to load it back
        Config config2;
        if (config2.loadFromFile(outputPath)) {
            std::cout << "Successfully reloaded saved config from: " << outputPath << std::endl;
            std::cout << "Config2 summary:" << std::endl;
            printConfigSummary(config2);
        } else {
            std::cout << "Failed to reload saved config from: " << outputPath << std::endl;
        }
    } else {
        std::cout << "Failed to save JSON config to: " << outputPath << std::endl;
    }
    
    std::cout << "" << std::endl;
}

void testCommandLineArgs() {
    std::cout << "=== Test 4: Command Line Argument Parsing ===" << std::endl;
    
    // Simulate command line arguments
    std::vector<std::string> args = {
        "--config", "configs/test_config.json",
        "--neuron_count", "2000",
        "-region_count", "2",
        "--stdp_ltp_weight", "0.02"
    };
    
    Config config;
    if (config.loadFromArgs(args.size(), args.data())) {
        std::cout << "Successfully parsed command line arguments" << std::endl;
        std::cout << "Config summary:" << std::endl;
        printConfigSummary(config);
    } else {
        std::cout << "Failed to parse command line arguments" << std::endl;
    }
    
    std::cout << "" << std::endl;
}

void testValidationAndDefaults() {
    std::cout << "=== Test 5: Configuration Validation and Defaults ===" << std::endl;
    
    Config config;
    
    // Load test config
    if (config.loadFromFile("configs/test_config.json")) {
        // Test getting with defaults for non-existent keys
        double timestep = config.getOr<double>("simulation.timestep", 0.01);
        int neurons = config.getOr<int>("brain.neuron_count", 500);
        bool curiosity = config.getOr<bool>("neuromodulation.curiosity.enable", true);
        
        std::cout << "timestep (from JSON or default): " << timestep << std::endl;
        std::cout << "neurons (from JSON or default): " << neurons << std::endl;
        std::cout << "curiosity (from JSON or default): " << curiosity << std::endl;
        
        // Check nested paths (JSON keys with dots)
        if (config.has("simulation.timestep")) {
            std::cout << "Has nested key 'simulation.timestep': Yes" << std::endl;
        }
        if (config.has("plasticity.stdp.enable")) {
            std::cout << "Has nested key 'plasticity.stdp.enable': Yes" << std::endl;
        }
    }
    
    std::cout << "" << std::endl;
}

void testConfigMerging() {
    std::cout << "=== Test 6: Configuration Merging ===" << std::endl;
    
    // Start with default config
    Config config1;
    config1.set("brain.neuron_count", 100, ConfigSource::Default);
    config1.set("plasticity.stdp.enable", true, ConfigSource::Default);
    
    // Load from JSON (should override defaults)
    if (config1.loadFromFile("configs/test_config.json")) {
        std::cout << "Config after JSON load:" << std::endl;
        printConfigSummary(config1);
        
        // Modify some values
        config1.set("brain.neuron_count", 3000, ConfigSource::Runtime);
        std::cout << "After runtime modification of neuron_count:" << std::endl;
        printConfigSummary(config1);
    }
    
    std::cout << "" << std::endl;
}

void testPerformance() {
    std::cout << "=== Test 7: Configuration Performance ===" << std::endl;
    
    // Create a large config with many entries
    Config config;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        config.set("key_" + std::to_string(i), i, ConfigSource::Default);
        config.has("key_" + std::to_string(i));
        config.getOr<int>("key_" + std::to_string(i), 0);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Created and accessed 1000 config entries in " << duration.count() << " ms" << std::endl;
    std::cout << "Total keys in config: " << config.getKeys().size() << std::endl;
    
    std::cout << "" << std::endl;
}

void testFileIo() {
    std::cout << "=== Test 8: File I/O Operations ===" << std::endl;
    
    Config config;
    
    // Set up test values
    config.set("test_string", "Hello World", ConfigSource::Runtime);
    config.set("test_int", 42, ConfigSource::Runtime);
    config.set("test_double", 3.14159, ConfigSource::Runtime);
    config.set("test_bool", false, ConfigSource::Runtime);
    config.set("test_vector", std::vector<int>({1, 2, 3, 4, 5}), ConfigSource::Runtime);
    
    // Save to file
    std::string testFile = "configs/test_io_config.txt";
    if (config.saveToFile(testFile)) {
        std::cout << "Saved config to: " << testFile << std::endl;
        
        // Check file exists and has content
        std::ifstream file(testFile);
        if (file.is_open()) {
            std::string line;
            size_t lineCount = 0;
            while (std::getline(file, line)) {
                lineCount++;
                if (lineCount <= 10) {  // Show first 10 lines
                    std::cout << "  File line: " << line << std::endl;
                }
            }
            std::cout << "  File has " << lineCount << " lines total" << std::endl;
        }
    }
    
    std::cout << "" << std::endl;
}

int main() {
    // Initialize logger
    auto& logger = Logger::getGlobal();
    logger.setLevel(LogLevel::Info);
    
    std::cout << "=== NLM Config System Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    testBasicConfig();
    testJsonLoad();
    testJsonSave();
    testCommandLineArgs();
    testValidationAndDefaults();
    testConfigMerging();
    testPerformance();
    testFileIo();
    
    std::cout << "=== All Tests Completed ===" << std::endl;
    
    return 0;
}