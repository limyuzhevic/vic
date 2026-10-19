#include <iostream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include "core/Config/Config.hpp"

void testJsonLoading() {
    std::cout << "Testing JSON configuration loading..." << std::endl;
    
    nlm::Config config;
    
    // Test loading JSON file
    if (config.loadFromFile("examples/configs/default.json")) {
        std::cout << "✓ JSON file loaded successfully" << std::endl;
        
        // Test nested key access
        assert(config.has("brain.neuron_count"));
        auto neurons = config.get<int>("brain.neuron_count");
        assert(neurons.has_value());
        assert(neurons.value() == 1000);
        std::cout << "✓ Nested key access works: brain.neuron_count = " << neurons.value() << std::endl;
        
        // Test array access
        assert(config.has("brain.neuron_types"));
        auto types = config.get<std::vector<std::string>>("brain.neuron_types");
        assert(types.has_value());
        std::cout << "✓ Array access works: brain.neuron_types = [";
        for (const auto& t : types.value()) {
            std::cout << t << ", ";
        }
        std::cout << "]" << std::endl;
        
        // Test float access
        assert(config.has("brain.initial_weight_mean"));
        auto weightMean = config.get<double>("brain.initial_weight_mean");
        assert(weightMean.has_value());
        assert(std::abs(weightMean.value() - 0.5) < 0.001);
        std::cout << "✓ Float access works: brain.initial_weight_mean = " << weightMean.value() << std::endl;
        
        // Test boolean access
        assert(config.has("brain.plasticity.enable_stdp"));
        auto enableStdp = config.get<bool>("brain.plasticity.enable_stdp");
        assert(enableStdp.has_value());
        assert(enableStdp.value() == true);
        std::cout << "✓ Boolean access works: brain.plasticity.enable_stdp = " << enableStdp.value() << std::endl;
        
    } else {
        std::cout << "✗ Failed to load JSON file" << std::endl;
        assert(false);
    }
}

void testBackwardCompatibility() {
    std::cout << "\nTesting backward compatibility with key=value format..." << std::endl;
    
    // Create a simple key=value config file
    std::ofstream cfgFile("test_backward.cfg");
    cfgFile << "# Test key=value config file\n";
    cfgFile << "old_key = 42\n";
    cfgFile << "name = \"test\"\n";
    cfgFile << "enabled = true\n";
    cfgFile << "rate = 3.14\n";
    cfgFile.close();
    
    nlm::Config config;
    if (config.loadFromFile("test_backward.cfg")) {
        std::cout << "✓ Backward compatibility file loaded successfully" << std::endl;
        
        // Test key=value parsing
        assert(config.has("old_key"));
        auto oldKey = config.get<int>("old_key");
        assert(oldKey.has_value());
        assert(oldKey.value() == 42);
        std::cout << "✓ Integer parsing works: old_key = " << oldKey.value() << std::endl;
        
        assert(config.has("name"));
        auto name = config.get<std::string>("name");
        assert(name.has_value());
        assert(name.value() == "test");
        std::cout << "✓ String parsing works: name = " << name.value() << std::endl;
        
        assert(config.has("enabled"));
        auto enabled = config.get<bool>("enabled");
        assert(enabled.has_value());
        assert(enabled.value() == true);
        std::cout << "✓ Boolean parsing works: enabled = " << enabled.value() << std::endl;
        
        assert(config.has("rate"));
        auto rate = config.get<double>("rate");
        assert(rate.has_value());
        assert(std::abs(rate.value() - 3.14) < 0.001);
        std::cout << "✓ Float parsing works: rate = " << rate.value() << std::endl;
        
    } else {
        std::cout << "✗ Failed to load backward compatibility file" << std::endl;
        assert(false);
    }
    
    // Clean up
    std::filesystem::remove("test_backward.cfg");
}

void testCommandLineLoading() {
    std::cout << "\nTesting command-line argument loading..." << std::endl;
    
    // Simulate command-line arguments
    int argc = 5;
    const char* argv[] = {
        "test_program",
        "--brain.neuron_count=2000",
        "--simulation.timestep=0.002",
        "--plasticity.enable_stdp=true",
        "--name=TestBrain"
    };
    
    nlm::Config config;
    if (config.loadFromArgs(argc, const_cast<char**>(argv))) {
        std::cout << "✓ Command-line arguments loaded successfully" << std::endl;
        
        // Test command-line set values
        assert(config.has("brain.neuron_count"));
        auto neurons = config.get<int>("brain.neuron_count");
        assert(neurons.has_value());
        assert(neurons.value() == 2000);
        std::cout << "✓ Command-line integer works: brain.neuron_count = " << neurons.value() << std::endl;
        
        assert(config.has("simulation.timestep"));
        auto timestep = config.get<double>("simulation.timestep");
        assert(timestep.has_value());
        assert(std::abs(timestep.value() - 0.002) < 0.0001);
        std::cout << "✓ Command-line float works: simulation.timestep = " << timestep.value() << std::endl;
        
        assert(config.has("plasticity.enable_stdp"));
        auto enableStdp = config.get<bool>("plasticity.enable_stdp");
        assert(enableStdp.has_value());
        assert(enableStdp.value() == true);
        std::cout << "✓ Command-line boolean works: plasticity.enable_stdp = " << enableStdp.value() << std::endl;
        
        assert(config.has("name"));
        auto name = config.get<std::string>("name");
        assert(name.has_value());
        assert(name.value() == "TestBrain");
        std::cout << "✓ Command-line string works: name = " << name.value() << std::endl;
        
    } else {
        std::cout << "✗ Failed to load command-line arguments" << std::endl;
        assert(false);
    }
}

void testJsonSaving() {
    std::cout << "\nTesting JSON file saving..." << std::endl;
    
    nlm::Config config;
    
    // Add some test values
    config.set("brain.neuron_count", 1500, nlm::ConfigSource::Runtime);
    config.set("simulation.timestep", 0.0005, nlm::ConfigSource::Runtime);
    config.set("plasticity.enable_stdp", true, nlm::ConfigSource::Runtime);
    config.set("test_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    if (config.saveToFile("test_output.json")) {
        std::cout << "✓ JSON file saved successfully" << std::endl;
        
        // Verify the saved file can be loaded
        nlm::Config config2;
        if (config2.loadFromFile("test_output.json")) {
            std::cout << "✓ Saved JSON file can be reloaded" << std::endl;
            
            // Verify values
            assert(config2.has("brain.neuron_count"));
            auto neurons = config2.get<int>("brain.neuron_count");
            assert(neurons.has_value());
            assert(neurons.value() == 1500);
            std::cout << "✓ Saved neuron_count = " << neurons.value() << std::endl;
            
            assert(config2.has("simulation.timestep"));
            auto timestep = config2.get<double>("simulation.timestep");
            assert(timestep.has_value());
            assert(std::abs(timestep.value() - 0.0005) < 0.0001);
            std::cout << "✓ Saved timestep = " << timestep.value() << std::endl;
            
        } else {
            std::cout << "✗ Failed to reload saved JSON file" << std::endl;
            assert(false);
        }
        
    } else {
        std::cout << "✗ Failed to save JSON file" << std::endl;
        assert(false);
    }
    
    // Clean up
    std::filesystem::remove("test_output.json");
}

void testValidation() {
    std::cout << "\nTesting configuration validation..." << std" << std::endl;
    
    nlm::Config config;
    
    // Add test values
    config.set("valid.int", 42, nlm::ConfigSource::Runtime);
    config.set("valid.bool", true, nlm::ConfigSource::Runtime);
    config.set("valid.string", "test", nlm::ConfigSource::Runtime);
    config.set("valid.array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Check validation
    bool isValid = config.validate();
    std::cout << "✓ Validation check completed: " << (isValid ? "PASS" : "FAIL") << std::endl;
    
    auto warnings = config.getValidationWarnings();
    if (!warnings.empty()) {
        std::cout << "Warnings:" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "  - " << warning << std::endl;
        }
    } else {
        std::cout << "✓ No validation warnings" << std::endl;
    }
}

void testGetOrWithDefault() {
    std::cout << "\nTesting getOr() with default values..." << std::endl;
    
    nlm::Config config;
    
    // Add some values
    config.set("existing", 100, nlm::ConfigSource::Runtime);
    config.set("name", std::string("test"), nlm::ConfigSource::Runtime);
    
    // Test with existing keys
    assert(config.getOr<int>("existing", 999) == 100);
    std::cout << "✓ getOr() with existing int key: 100" << std::endl;
    
    assert(config.getOr<std::string>("name", "default") == "test");
    std::cout << "✓ getOr() with existing string key: test" << std::endl;
    
    // Test with non-existing keys
    assert(config.getOr<int>("nonexistent", 999) == 999);
    std::cout << "✓ getOr() with default for non-existing int key: 999" << std::endl;
    
    assert(config.getOr<std::string>("unknown", "default") == "default");
    std::cout << "✓ getOr() with default for non-existing string key: default" << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration System Tests ===" << std::endl;
    
    try {
        testJsonLoading();
        testBackwardCompatibility();
        testCommandLineLoading();
        testJsonSaving();
        testValidation();
        testGetOrWithDefault();
        
        std::cout << "\n=== All tests passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "\n=== Test failed with exception: " << e.what() << " ===" << std::endl;
        return 1;
    } catch (...) {
        std::cout << "\n=== Test failed with unknown exception ===" << std::endl;
        return 1;
    }
}
