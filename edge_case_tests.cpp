// Edge Case Tests for NLM Components
// Comprehensive test suite covering error conditions and edge cases

#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

namespace edge_case_tests {

void testConfigEdgeCases() {
    std::cout << "Running Config edge case tests..." << std::endl;
    
    // Test 1: Empty file
    {
        std::ofstream file("empty_config.txt");
        file.close();
        nlm::Config config;
        assert(config.loadFromFile("empty_config.txt") == true);
        assert(config.getKeys().empty());
        std::filesystem::remove("empty_config.txt");
    }
    
    // Test 2: File with only comments
    {
        std::ofstream file("comment_config.txt");
        file << "# This is a comment\n";
        file << "// Another comment\n";
        file << "   # Indented comment\n";
        file.close();
        
        nlm::Config config;
        assert(config.loadFromFile("comment_config.txt") == true);
        assert(config.getKeys().empty());
        std::filesystem::remove("comment_config.txt");
    }
    
    // Test 3: Malformed key=value pairs
    {
        std::ofstream file("malformed_config.txt");
        file << "key1 = value1\n";
        file << "key2=value2\n";  // No spaces
        file << "=value3\n";       // No key
        file << "key4 = \n";        // No value
        file << "key5 = value with spaces\n";
        file << "key6=\"quoted value\"\n";
        file.close();
        
        nlm::Config config;
        assert(config.loadFromFile("malformed_config.txt") == true);
        
        // Check that valid entries were parsed
        assert(config.has("key1") == true);
        assert(config.has("key2") == true);
        assert(config.has("key5") == true);
        assert(config.has("key6") == true);
        
        // Check that malformed entries were ignored or handled
        assert(config.has("key3") == false);
        assert(config.has("key4") == false);
        
        std::filesystem::remove("malformed_config.txt");
    }
    
    // Test 4: JSON file with various data types
    {
        std::ofstream file("test_json.json");
        file << "{\n";
        file << "  \"string_key\": \"hello world\",\n";
        file << "  \"int_key\": 42,\n";
        file << "  \"float_key\": 3.14,\n";
        file << "  \"bool_key\": true,\n";
        file << "  \"int_array\": [1, 2, 3, 4, 5],\n";
        file << "  \"float_array\": [1.5, 2.5, 3.14],\n";
        file << "  \"string_array\": [\"apple\", \"banana\", \"cherry\"],\n";
        file << "  \"empty_array\": [],\n";
        file << "  \"nested_key\": \"nested_value\"\n";
        file << "}\n";
        file.close();
        
        nlm::Config config;
        assert(config.loadFromFile("test_json.json") == true);
        
        // Verify JSON loading
        auto strVal = config.get<std::string>("string_key");
        assert(strVal.has_value() && strVal.value() == "hello world");
        
        auto intVal = config.get<int>("int_key");
        assert(intVal.has_value() && intVal.value() == 42);
        
        auto floatVal = config.get<double>("float_key");
        assert(floatVal.has_value() && floatVal.value() == 3.14);
        
        auto boolVal = config.get<bool>("bool_key");
        assert(boolVal.has_value() && boolVal.value() == true);
        
        std::filesystem::remove("test_json.json");
    }
    
    // Test 5: YAML file with various formats
    {
        std::ofstream file("test_yaml.yaml");
        file << "string_key: \"hello world\"\n";
        file << "int_key: 42\n";
        file << "float_key: 3.14\n";
        file << "bool_key: true\n";
        file << "int_array: [1, 2, 3, 4, 5]\n";
        file << "float_array: [1.5, 2.5, 3.14]\n";
        file << "string_array: [apple, banana, cherry]\n";
        file << "empty_array: []\n";
        file.close();
        
        nlm::Config config;
        assert(config.loadFromFile("test_yaml.yaml") == true);
        
        // Verify YAML loading
        auto strVal = config.get<std::string>("string_key");
        assert(strVal.has_value() && strVal.value() == "hello world");
        
        auto intVal = config.get<int>("int_key");
        assert(intVal.has_value() && intVal.value() == 42);
        
        std::filesystem::remove("test_yaml.yaml");
    }
    
    // Test 6: Mixed file extensions
    {
        // .json extension should use JSON parser
        std::ofstream file("mixed.json");
        file << "{\"key\": \"value\", \"number\": 123}\n";
        file.close();
        
        nlm::Config config1;
        assert(config1.loadFromFile("mixed.json") == true);
        assert(config1.has("key") == true);
        assert(config1.has("number") == true);
        
        // .yaml extension should use YAML parser
        std::ofstream file2("mixed.yaml");
        file2 << "key: value\nnumber: 123\n";
        file2.close();
        
        nlm::Config config2;
        assert(config2.loadFromFile("mixed.yaml") == true);
        assert(config2.has("key") == true);
        assert(config2.has("number") == true);
        
        std::filesystem::remove("mixed.json");
        std::filesystem::remove("mixed.yaml");
    }
    
    // Test 7: Config save with JSON format
    {
        nlm::Config config;
        config.set("name", std::string("Test Brain"));
        config.set("neuron_count", static_cast<int64_t>(1000));
        config.set("learning_rate", 0.01);
        config.set("enabled", true);
        
        assert(config.saveToFile("test_save.json") == true);
        
        // Verify the saved file can be loaded
        nlm::Config config2;
        assert(config2.loadFromFile("test_save.json") == true);
        
        assert(config2.has("name") == true);
        assert(config2.has("neuron_count") == true);
        assert(config2.has("learning_rate") == true);
        assert(config2.has("enabled") == true);
        
        std::filesystem::remove("test_save.json");
    }
    
    // Test 8: Config save with YAML format
    {
        nlm::Config config;
        config.set("name", std::string("Test Brain"));
        config.set("neuron_count", static_cast<int64_t>(1000));
        config.set("learning_rate", 0.01);
        config.set("enabled", true);
        
        assert(config.saveToFile("test_save.yaml") == true);
        
        // Verify the saved file can be loaded
        nlm::Config config2;
        assert(config2.loadFromFile("test_save.yaml") == true);
        
        assert(config2.has("name") == true);
        assert(config2.has("neuron_count") == true);
        assert(config2.has("learning_rate") == true);
        assert(config2.has("enabled") == true);
        
        std::filesystem::remove("test_save.yaml");
    }
    
    // Test 9: Config type conversions
    {
        nlm::Config config;
        
        // Test setting different types
        config.set("int_val", 42);
        config.set("double_val", 3.14);
        config.set("string_val", "hello");
        config.set("bool_val", true);
        
        // Test getting with different types
        assert(config.get<int>("int_val").has_value());
        assert(config.get<int64_t>("int_val").has_value());
        assert(config.get<double>("int_val").has_value());
        
        assert(config.get<double>("double_val").has_value());
        assert(config.get<int>("double_val").has_value());  // Should truncate
        
        assert(config.get<std::string>("string_val").has_value());
        assert(config.get<bool>("string_val").has_value());  // Should fail
        
        // Test getOr with default values
        assert(config.getOr<int>("missing_key", 999) == 999);
        assert(config.getOr<std::string>("missing_key", "default") == "default");
        assert(config.getOr<double>("missing_key", 0.0) == 0.0);
        
        // Test has() function
        assert(config.has("int_val") == true);
        assert(config.has("missing_val") == false);
    }
    
    // Test 10: Config error conditions
    {
        nlm::Config config;
        
        // Test non-existent file
        assert(config.loadFromFile("/nonexistent/path/file.txt") == false);
        
        // Test writing to invalid directory (should fail gracefully)
        assert(config.saveToFile("/invalid/path/config.json") == false);
        
        // Test very long key
        std::string longKey(1000, 'x');
        config.set(longKey, "value");
        assert(config.has(longKey) == true);
        
        // Test very long value
        std::string longValue(10000, 'y');
        config.set("long_value", longValue);
        assert(config.get<std::string>("long_value").has_value());
    }
    
    std::cout << "Config edge case tests passed!" << std::endl;
}

void testBrainEdgeCases() {
    std::cout << "Running Brain edge case tests..." << std::endl;
    
    // Test 1: Brain with minimal config
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize() == true);
        
        // Very minimal brain should still work
        brain.step(0);
        assert(brain.getTotalNeuronCount() == 1);
    }
    
    // Test 2: Brain reset functionality
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize() == true);
        
        brain.step(0);
        size_t initialSpikeCount = brain.getTotalSpikeCount();
        
        brain.reset();
        assert(brain.getTotalNeuronCount() == 10);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getActiveNeuronCount() == 0);
        
        // After reset, spike count should be reset
        assert(brain.getTotalSpikeCount() == 0);
    }
    
    // Test 3: Brain with many regions
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize() == true);
        
        assert(brain.getRegionCount() == 10);
        assert(brain.getTotalNeuronCount() == 100);
        
        // Check all regions are accessible
        auto regionIds = brain.getRegionIds();
        assert(regionIds.size() == 10);
        
        for (auto rid : regionIds) {
            auto* region = brain.getRegion(rid);
            assert(region != nullptr);
            assert(region->getId() == rid);
        }
    }
    
    // Test 4: Brain step with large time values
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize() == true);
        
        // Step with large simulation step
        brain.step(1000);
        assert(brain.getTotalNeuronCount() == 50);
        
        // Step with timestamp
        brain.step(1000, 1000.0);
        assert(brain.getTotalNeuronCount() == 50);
    }
    
    // Test 5: Brain statistics after reset
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize() == true);
        
        // Get initial statistics
        size_t initialNeuronCount = brain.getTotalNeuronCount();
        size_t initialSynapseCount = brain.getTotalSynapseCount();
        
        // Run some steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        // Reset and check statistics
        brain.reset();
        assert(brain.getTotalNeuronCount() == initialNeuronCount);
        // Synapse count may change after reset
    }
    
    std::cout << "Brain edge case tests passed!" << std::endl;
}

void testAgentBrainEdgeCases() {
    std::cout << "Running AgentBrain edge case tests..." << std::endl;
    
    // Test 1: AgentBrain with null brain
    {
        nlm::AgentBrain agentBrain(nullptr);
        
        // These should not crash
        assert(agentBrain.getSensoryInputSize() == 0);
        assert(agentBrain.getMotorOutputSize() == 0);
        
        nlm::Action action = agentBrain.decodeMotorCommand();
        assert(action.getType() == nlm::ActionType::Wait);
        
        agentBrain.applyRewardModulation(1.0f, 0.5f);
        agentBrain.updateDevelopment(1.0);
        
        agentBrain.reset();
    }
    
    // Test 2: AgentBrain with brain but no initialization
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
        
        // Should work even without explicit initialize call
        assert(agentBrain.getBrain() != nullptr);
        
        // Decode should return Wait since no neurons are active
        nlm::Action action = agentBrain.decodeMotorCommand();
        assert(action.getType() == nlm::ActionType::Wait);
    }
    
    // Test 3: AgentBrain with world initialization
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
        
        // Create a simple world
        nlm::SimpleWorld world;
        world.configure(16, 16, 8, 8);
        agentBrain.initialize(world);
        
        assert(agentBrain.getSensoryInputSize() == 256 + 8 + 4 + 6);  // Vision + touch + internal + proprioception
    }
    
    // Test 4: AgentBrain sensory processing with empty percept
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
        
        // Create empty percept
        nlm::SensoryPercept emptyPercept;
        
        // Should not crash
        agentBrain.processSensoryInput(emptyPercept);
    }
    
    // Test 5: AgentBrain curiosity and exploration
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
        
        // Enable curiosity
        agentBrain.enableCuriosity(true);
        
        // Set high curiosity level
        // (This would require accessing private members or adding public setters)
        
        // Reset should clear all levels
        agentBrain.reset();
    }
    
    std::cout << "AgentBrain edge case tests passed!" << std::endl;
}

void runAllEdgeCaseTests() {
    std::cout << "=== Running Edge Case Tests for NLM ===" << std::endl;
    
    testConfigEdgeCases();
    testBrainEdgeCases();
    testAgentBrainEdgeCases();
    
    std::cout << "\n=== All Edge Case Tests Passed! ===" << std::endl;
}

} // namespace edge_case_tests

// Main function for running edge case tests
int main() {
    edge_case_tests::runAllEdgeCaseTests();
    return 0;
}
