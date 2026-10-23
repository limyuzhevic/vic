#include <iostream>
#include <fstream>
#include <filesystem>
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "=== NLM Config JSON Parser Test ===" << std::endl;
    
    // Test 1: Basic save and load functionality
    std::cout << "\nTest 1: Basic save and load functionality" << std::endl;
    nlm::Config config;
    
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.1f);
    config.set("plasticity.stdp.enable", true);
    config.set("development.critical_period_start", 10.0);
    config.set("development.critical_period_end", 50.0);
    
    std::cout << "Configuration created:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test 2: Save to JSON file
    std::cout << "\nTest 2: Saving configuration to JSON file" << std::endl;
    if (config.saveToFile("test_config.json")) {
        std::cout << "✅ Configuration saved to test_config.json" << std::endl;
    } else {
        std::cout << "❌ Failed to save configuration to JSON file" << std::endl;
        return 1;
    }
    
    // Test 3: Load from JSON file
    std::cout << "\nTest 3: Loading configuration from JSON file" << std::endl;
    nlm::Config config2;
    if (config2.loadFromFile("test_config.json")) {
        std::cout << "✅ Configuration loaded successfully:" << std::endl;
        std::cout << config2.summary() << std::endl;
        
        // Verify values
        std::cout << "\nVerifying loaded values:" << std::endl;
        std::cout << "  🧠 neuron_count: " << config2.get<int>("brain.neuron_count").value_or(0) << std::endl;
        std::cout << "  🧠 synapse_density: " << config2.get<double>("brain.synapse_density").value_or(0.0) << std::endl;
        std::cout << "  🧠 stdp_enable: " << config2.get<bool>("plasticity.stdp.enable").value_or(false) << std::endl;
        std::cout << "  🧠 critical_period_start: " << config2.get<double>("development.critical_period_start").value_or(0.0) << std::endl;
        std::cout << "  🧠 critical_period_end: " << config2.get<double>("development.critical_period_end").value_or(0.0) << std::endl;
        
        // Check if all values match
        if (config2.get<int>("brain.neuron_count").value_or(0) != 1000) {
            std::cout << "❌ neuron_count mismatch" << std::endl;
            return 1;
        }
        if (config2.get<double>("brain.synapse_density").value_or(0.0) != 0.1) {
            std::cout << "❌ synapse_density mismatch" << std::endl;
            return 1;
        }
        if (config2.get<bool>("plasticity.stdp.enable").value_or(false) != true) {
            std::cout << "❌ stdp_enable mismatch" << std::endl;
            return 1;
        }
        if (config2.get<double>("development.critical_period_start").value_or(0.0) != 10.0) {
            std::cout << "❌ critical_period_start mismatch" << std::endl;
            return 1;
        }
        if (config2.get<double>("development.critical_period_end").value_or(0.0) != 50.0) {
            std::cout << "❌ critical_period_end mismatch" << std::endl;
            return 1;
        }
        
        std::cout << "✅ All values match!" << std::endl;
    } else {
        std::cout << "❌ Failed to load configuration from JSON file" << std::endl;
        return 1;
    }
    
    // Test 4: Advanced JSON parsing with nested structure
    std::cout << "\nTest 4: Advanced JSON parsing with nested structure" << std::endl;
    nlm::Config config3;
    
    // Set values with nested structure
    config3.set("brain.neuron_count", 2000);
    config3.set("brain.synapse_density", 0.15);
    config3.set("plasticity.stdp.enable", true);
    config3.set("plasticity.stdp.learning_rate", 0.01);
    config3.set("plasticity.hebbian.enable", true);
    config3.set("plasticity.hebbian.learning_rate", 0.001);
    config3.set("plasticity.structural.enable", true);
    config3.set("plasticity.structural.growth_rate", 0.02);
    config3.set("neuromod.dopamine.scale", 2.0);
    config3.set("neuromod.curiosity.enable", true);
    config3.set("neuromod.novelty.enable", true);
    config3.set("prediction.enable", true);
    config3.set("prediction.horizon", 10.0);
    config3.set("cognition.planner.enable", true);
    config3.set("cognition.planner.horizon", 5.0);
    config3.set("development.critical_period_start", 20.0);
    config3.set("development.critical_period_end", 100.0);
    config3.set("world.width", 100.0);
    config3.set("world.height", 100.0);
    config3.set("world.vision.width", 16);
    config3.set("world.vision.height", 16);
    
    std::cout << "Advanced configuration created:" << std::endl;
    std::cout << config3.summary() << std::endl;
    
    // Save advanced configuration
    if (config3.saveToFile("advanced_config.json")) {
        std::cout << "✅ Advanced configuration saved to advanced_config.json" << std::endl;
    }
    
    // Load advanced configuration
    nlm::Config config4;
    if (config4.loadFromFile("advanced_config.json")) {
        std::cout << "✅ Advanced configuration loaded successfully:" << std::endl;
        std::cout << config4.summary() << std::endl;
        
        // Verify all nested values
        std::cout << "\nVerifying nested values:" << std::endl;
        
        struct NestedTest {
            std::string key;
            std::string expectedType;
            bool(*checker)(const nlm::Config&);
        };
        
        NestedTest tests[] = {
            {"brain.neuron_count", "int", [](const nlm::Config& c) { return c.get<int>("brain.neuron_count").value_or(0) == 2000; }},
            {"brain.synapse_density", "double", [](const nlm::Config& c) { return c.get<double>("brain.synapse_density").value_or(0.0) == 0.15; }},
            {"plasticity.stdp.enable", "bool", [](const nlm::Config& c) { return c.get<bool>("plasticity.stdp.enable").value_or(false) == true; }},
            {"plasticity.stdp.learning_rate", "double", [](const nlm::Config& c) { return c.get<double>("plasticity.stdp.learning_rate").value_or(0.0) == 0.01; }},
            {"plasticity.hebbian.enable", "bool", [](const nlm::Config& c) { return c.get<bool>("plasticity.hebbian.enable").value_or(false) == true; }},
            {"neuromod.dopamine.scale", "double", [](const nlm::Config& c) { return c.get<double>("neuromod.dopamine.scale").value_or(0.0) == 2.0; }},
            {"development.critical_period_start", "double", [](const nlm::Config& c) { return c.get<double>("development.critical_period_start").value_or(0.0) == 20.0; }},
            {"development.critical_period_end", "double", [](const nlm::Config& c) { return c.get<double>("development.critical_period_end").value_or(0.0) == 100.0; }},
            {"world.width", "double", [](const nlm::Config& c) { return c.get<double>("world.width").value_or(0.0) == 100.0; }},
        };
        
        bool allPassed = true;
        for (const auto& test : tests) {
            if (test.checker(config4)) {
                std::cout << "  ✅ " << test.key << " (" << test.expectedType << ") - PASS" << std::endl;
            } else {
                std::cout << "  ❌ " << test.key << " (" << test.expectedType << ") - FAIL" << std::endl;
                allPassed = false;
            }
        }
        
        if (allPassed) {
            std::cout << "✅ All nested values verified!" << std::endl;
        } else {
            std::cout << "❌ Some nested values failed verification" << std::endl;
            return 1;
        }
    }
    
    // Test 5: Test getOr() method with JSON-loaded config
    std::cout << "\nTest 5: Testing getOr() method with JSON-loaded config" << std::endl;
    std::cout << "  🧠 neuron_count (existing): " << config4.getOr<int>("brain.neuron_count", 1000) << std::endl;
    std::cout << "  🧠 missing_key (default): " << config4.getOr<int>("missing.key", 999) << std::endl;
    std::cout << "  🧠 synapse_density (existing): " << config4.getOr<double>("brain.synapse_density", 0.05) << std::endl;
    std::cout << "  🧠 stdp_enable (existing): " << config4.getOr<bool>("plasticity.stdp.enable", false) << std::endl;
    
    // Test 6: Test has() method with JSON-loaded config
    std::cout << "\nTest 6: Testing has() method with JSON-loaded config" << std::endl;
    std::cout << "  🧠 Has brain.neuron_count: " << config4.has("brain.neuron_count") << std::endl;
    std::cout << "  🧠 Has missing.key: " << config4.has("missing.key") << std::endl;
    std::cout << "  🧠 Has plasticity.enable: " << config4.has("plasticity.enable") << std::endl;
    
    // Test 7: Test remove() method with JSON-loaded config
    std::cout << "\nTest 7: Testing remove() method with JSON-loaded config" << std::endl;
    config4.remove("neuromod.dopamine.scale");
    std::cout << "  🧠 After removing neuromod.dopamine.scale:" << std::endl;
    std::cout << "    🧠 Has neuromod.dopamine.scale: " << config4.has("neuromod.dopamine.scale") << std::endl;
    std::cout << "    🧠 Has neuromod.curiosity.enable: " << config4.has("neuromod.curiosity.enable") << std::endl;
    
    // Test 8: Test clear() method with JSON-loaded config
    std::cout << "\nTest 8: Testing clear() method with JSON-loaded config" << std::endl;
    std::cout << "  🧠 Before clear - keys: " << config4.getKeys().size() << std::endl;
    config4.clear();
    std::cout << "  🧠 After clear - keys: " << config4.getKeys().size() << std::endl;
    
    // Cleanup
    if (std::filesystem::exists("test_config.json")) {
        std::remove("test_config.json");
        std::cout << "✅ Cleaned up test_config.json" << std::endl;
    }
    
    if (std::filesystem::exists("advanced_config.json")) {
        std::remove("advanced_config.json");
        std::cout << "✅ Cleaned up advanced_config.json" << std::endl;
    }
    
    std::cout << "\n🎉 All tests passed successfully! 🎉" << std::endl;
    return 0;
}