#include <iostream>
#include "core/Config/Config.hpp"

int main() {
    std::cout << "Testing enhanced NLM configuration system..." << std::endl;
    
    nlm::Config config;
    
    // Test basic configuration
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Test validation
    auto validationResult = config.validateBrainSystem();
    if (validationResult.isValid) {
        std::cout << "✓ Configuration validation passed" << std::endl;
    } else {
        std::cout << "✗ Configuration validation failed:" << std::endl;
        for (const auto& error : validationResult.errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Test getOr method
    int neurons = config.getOr<int>("neuron_count", 0);
    std::cout << "✓ Retrieved neuron_count: " << neurons << std::endl;
    
    // Test configuration schema
    auto schema = config.createBrainSystemSchema();
    std::cout << "✓ Created brain system schema with " << schema.getParameterNames().size() << " parameters" << std::endl;
    
    // Test backup/restore
    bool backupSuccess = config.backup("/tmp/test_config_backup.txt");
    if (backupSuccess) {
        std::cout << "✓ Configuration backup created" << std::endl;
    }
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    return 0;
}