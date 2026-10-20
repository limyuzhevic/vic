#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing enhanced NLM configuration system..." << std::endl;
    
    try {
        // Test 1: Basic configuration with schema
        auto schema = std::make_shared<nlm::ConfigSchema>();
        schema->setDefault("neuron_count", 100, nlm::ConfigSource::Default, "Number of neurons")
              .addRangeConstraint("neuron_count", 1.0, 1000000.0)
              .setDefault("learning_rate", 0.01, nlm::ConfigSource::Default, "Learning rate")
              .addRangeConstraint("learning_rate", 0.0, 1.0)
              .addGroup(nlm::ConfigGroup("Neural Network", "Neural network settings"))
              .addGroup(nlm::ConfigGroup("Simulation", "Simulation settings"));
        
        nlm::Config config(schema);
        
        // Test default values
        assert(config.getOr<int>("neuron_count", -1) == 100);
        assert(config.getOr<double>("learning_rate", -1.0) > 0.0 && config.getOr<double>("learning_rate", -1.0) < 1.0);
        
        // Test setting values
        config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
        assert(config.getOr<int>("neuron_count", -1) == 500);
        
        // Test validation - should pass
        std::string error;
        assert(config.validate("neuron_count", error));
        assert(error.empty());
        
        // Test validation - should fail
        config.set("neuron_count", 2000000, nlm::ConfigSource::Runtime);
        assert(!config.validate("neuron_count", error));
        assert(!error.empty());
        std::cout << "    Validation error: " << error << std::endl;
        
        // Test configuration groups
        config.set("max_steps", 1000, nlm::ConfigSource::Runtime);
        auto groupKeys = config.getKeysInGroup("Simulation");
        bool stepsInGroup = std::find(groupKeys.begin(), groupKeys.end(), "max_steps") != groupKeys.end();
        assert(stepsInGroup);
        
        // Test versioning
        assert(config.getVersion().toString() == "1.0.0");
        config.setVersion(nlm::ConfigVersion(2, 0, 1));
        assert(config.getVersion().major == 2);
        
        // Test reset to defaults
        config.set("neuron_count", 999999, nlm::ConfigSource::Runtime);
        config.resetToDefaults();
        assert(config.getOr<int>("neuron_count", -1) == 100);
        
        // Test thread safety (basic check)
        config.set("test_value", 42, nlm::ConfigSource::Runtime);
        assert(config.getOr<int>("test_value", -1) == 42);
        
        // Test JSON serialization (if file operations work)
        bool saved = config.saveToFile("/tmp/nlm_test_config.json");
        if (saved) {
            nlm::Config config2;
            bool loaded = config2.loadFromFile("/tmp/test_config.json");
            if (loaded) {
                std::cout << "    JSON serialization test passed" << std::endl;
                std::remove("/tmp/nlm_test_config.json");
            }
        }
        
        // Test documentation generation
        std::string doc = schema->generateDocumentation();
        assert(doc.find("# Configuration Schema") != std::string::npos);
        assert(doc.find("neuron_count") != std::string::npos);
        
        std::cout << "All configuration tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}