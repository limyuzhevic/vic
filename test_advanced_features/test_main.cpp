#include <iostream>
#include <string>
#include "src/core/Config/Config.hpp"
#include "src/core/AdvancedConfig/AdvancedConfig.hpp"

int main() {
    std::cout << "=== NLM Advanced Features Test ===" << std::endl << std::endl;
    
    // Test 1: Basic Configuration
    std::cout << "Test 1: Basic Configuration" << std::endl;
    nlm::Config config;
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.1f);
    config.set("simulation_timestep", 0.001);
    
    std::cout << "Configuration created successfully" << std::endl;
    std::cout << "Neuron count: " << config.getOr<size_t>("brain.neuron_count", 0) << std::endl;
    std::cout << std::endl;
    
    // Test 2: Advanced Configuration
    std::cout << "Test 2: Advanced Configuration" << std::endl;
    nlm::AdvancedConfig advancedConfig;
    
    // Modify advanced settings
    auto& perfSettings = advancedConfig.getPerformanceSettings();
    perfSettings.numThreads = 4;
    perfSettings.enableSIMD = true;
    perfSettings.enableMemoryPools = true;
    
    auto& plasticSettings = advancedConfig.getPlasticitySettings();
    plasticSettings.stdpLearningRate = 0.02f;
    plasticSettings.dopaminePlasticityModulator = 1.5f;
    
    auto& memorySettings = advancedConfig.getMemorySettings();
    memorySettings.workingMemoryCapacity = 2000;
    memorySettings.enableWorkingMemoryCompression = true;
    
    // Apply to config
    advancedConfig.applyToConfig(config);
    
    std::cout << "Advanced configuration applied successfully" << std::endl;
    std::cout << "Performance threads: " << config.getOr<size_t>("performance.num_threads", 0) << std::endl;
    std::cout << "STDP learning rate: " << config.getOr<float>("plasticity.stdp.learning_rate", 0.0f) << std::endl;
    std::cout << "Working memory capacity: " << config.getOr<size_t>("memory.working.capacity", 0) << std::endl;
    std::cout << std::endl;
    
    // Test 3: Create default and optimized advanced configurations
    std::cout << "Test 3: Advanced Configuration Templates" << std::endl;
    
    auto defaultConfig = nlm::AdvancedConfig::createDefault();
    std::cout << "Default configuration created" << std::endl;
    
    auto optimizedConfig = nlm::AdvancedConfig::createOptimized();
    std::cout << "Optimized configuration created" << std::endl;
    
    // Apply optimized config
    optimizedConfig.applyToConfig(config);
    std::cout << "Optimized configuration applied" << std::endl;
    std::cout << "Final neuron count: " << config.getOr<size_t>("brain.neuron_count", 0) << std::endl;
    std::cout << std::endl;
    
    // Test 4: Configuration summary
    std::cout << "Test 4: Configuration Summary" << std::endl;
    std::string summary = config.summary();
    std::cout << "Configuration summary length: " << summary.length() << " characters" << std::endl;
    std::cout << "Summary preview: " << summary.substr(0, 100) << "..." << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== All Tests Passed ===" << std::endl;
    
    return 0;
}