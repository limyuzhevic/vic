// TestConfig.cpp - Test the improved configuration system

#include "Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing improved NLM configuration system..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    
    // Test type-safe setters
    config->setInt("neuron_count", 1000);
    config->setDouble("simulation_timestep", 0.001);
    config->setString("checkpoint_dir", "./checkpoints");
    config->setBool("enable_audio", true);
    
    // Test type-safe getters
    auto neuronCount = config->getInt("neuron_count");
    auto timestep = config->getDouble("simulation_timestep");
    auto checkpointDir = config->getString("checkpoint_dir");
    auto enableAudio = config->getBool("enable_audio");
    
    std::cout << "Neuron count: " << (neuronCount ? std::to_string(*neuronCount) : "NOT SET") << std::endl;
    std::cout << "Simulation timestep: " << (timestep ? std::to_string(*timestep) : "NOT SET") << std::endl;
    std::cout << "Checkpoint dir: " << (checkpointDir ? *checkpointDir : "NOT SET") << std::endl;
    std::cout << "Enable audio: " << (enableAudio ? (*enableAudio ? "true" : "false") : "NOT SET") << std::endl;
    
    // Test getOr methods
    int neurons = config->getOrInt("neuron_count", 500);
    double timestepDefault = config->getOrDouble("unknown_timestep", 0.01);
    std::string dir = config->getOrString("unknown_dir", "/tmp");
    
    std::cout << "Neuron count (with default): " << neurons << std::endl;
    std::cout << "Default timestep: " << timestepDefault << std::endl;
    std::cout << "Default dir: " << dir << std::endl;
    
    // Test summary
    std::cout << "\nConfiguration Summary:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Test validation
    if (!config->validateAll()) {
        std::cout << "Configuration has validation errors!" << std::endl;
    } else {
        std::cout << "All configuration entries are valid!" << std::endl;
    }
    
    std::cout << "\nConfiguration system test completed successfully!" << std::endl;
    return 0;
}
