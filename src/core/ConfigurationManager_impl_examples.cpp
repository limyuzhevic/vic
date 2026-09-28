#include "ConfigurationManager.hpp"
#include <iostream>

namespace nlm {

// Example usage of ConfigurationManager
void exampleBasicUsage() {
    // Create a configuration manager
    auto configManager = ConfigurationManager::create();
    
    // Set some configuration values
    configManager->getConfig()->set("brain.neuronCount", 1000);
    configManager->getConfig()->set("brain.regionCount", 5);
    configManager->getConfig()->set("simulation.dt", 0.1);
    configManager->getConfig()->set("simulation.maxTime", 100.0);
    
    // Load from file
    configManager->loadFromFile("config.json");
    
    // Save to file
    configManager->saveToFile("config.json");
    
    // Get configuration value
    auto neuronCount = configManager->getConfig()->get<int64_t>("brain.neuronCount");
    
    std::cout << "Neuron count: " << neuronCount.value_or(0) << std::endl;
}

void exampleEnvironmentManagement() {
    // Create configuration managers for different environments
    auto devConfig = ConfigurationManager::create(Environment::Development);
    auto prodConfig = ConfigurationManager::create(Environment::Production);
    
    // Set different values for different environments
    devConfig->getConfig()->set("brain.neuronCount", 100);
    prodConfig->getConfig()->set("brain.neuronCount", 5000);
    
    devConfig->getConfig()->set("environment.debug", true);
    prodConfig->getConfig()->set("environment.debug", false);
    
    // Merge configurations
    devConfig->merge(*prodConfig);
    
    std::cout << "Development neuron count: " << devConfig->getConfig()->getOr<int64_t>("brain.neuronCount", 0) << std::endl;
    std::cout << "Development debug: " << devConfig->getConfig()->getOr<bool>("environment.debug", false) << std::endl;
}

void examplePresets() {
    // Create preset configurations
    auto devPreset = ConfigurationManager::createDevelopmentPreset();
    auto debugPreset = ConfigurationManager::createDebugPreset();
    auto performancePreset = ConfigurationManager::createPerformancePreset();
    
    // Save presets
    devPreset->savePreset("MyDevelopment", PresetType::Development);
    debugPreset->savePreset("MyDebug", PresetType::Debug);
    performancePreset->savePreset("MyPerformance", PresetType::Performance);
    
    // Load preset
    auto myDevConfig = ConfigurationManager::create("MyDevelopment");
    if (myDevConfig) {
        std::cout << "Loaded development preset with " << myDevConfig->getEntryCount() << " entries" << std::endl;
    }
}

void exampleValidation() {
    auto configManager = ConfigurationManager::create();
    
    // Add schema for validation
    ConfigurationSchema schema;
    schema.key = "brain.neuronCount";
    schema.type = "int";
    schema.description = "Number of neurons in the brain";
    schema.required = true;
    schema.minValue = 1;
    schema.maxValue = 100000;
    
    configManager->addSchema(schema);
    
    // Set valid value
    configManager->getConfig()->set("brain.neuronCount", 1000);
    
    // Validate configuration
    if (configManager->validate()) {
        std::cout << "Configuration is valid" << std::endl;
    } else {
        std::cout << "Configuration validation failed" << std::endl;
    }
}

void exampleAdvancedFeatures() {
    auto configManager = ConfigurationManager::create();
    
    // Create template
    std::vector<std::string> keys = {"brain.neuronCount", "brain.regionCount", "simulation.dt"};
    configManager->createTemplate("BrainSimulation", keys);
    
    // Generate from template
    configManager->generateFromTemplate("BrainSimulation");
    
    // Enable history tracking
    configManager->enableHistory(true);
    configManager->trackChanges(true);
    
    // Make some changes
    configManager->getConfig()->set("brain.neuronCount", 2000);
    configManager->getConfig()->set("brain.regionCount", 10);
    
    // Get changes
    auto changes = configManager->getChanges();
    std::cout << "Made " << changes.size() << " configuration changes" << std::endl;
    
    // Save version
    configManager->saveVersion("Version1");
    
    // Compare with original
    auto originalConfig = ConfigurationManager::create("Version1");
    std::cout << "Configuration comparison: " << (configManager->compare(*originalConfig) ? "identical" : "different") << std::endl;
}

void examplePresetsWithSettings() {
    // Create development preset with specific brain configuration
    auto brainConfig = ConfigurationManager::createBrainSimulationConfig();
    brainConfig->savePreset("OptimizedBrainSimulation");
    
    // Create memory system configuration
    auto memoryConfig = ConfigurationManager::createMemorySystemConfig();
    memoryConfig->savePreset("OptimizedMemorySystem");
    
    // Create neuromodulation configuration
    auto neuromodConfig = ConfigurationManager::createNeuromodulationConfig();
    neuromodConfig->savePreset("OptimizedNeuromodulation");
    
    // Create environment configuration
    auto envConfig = ConfigurationManager::createEnvironmentConfig();
    envConfig->savePreset("OptimizedEnvironment");
    
    // Load all configurations
    auto fullConfig = ConfigurationManager::create("OptimizedBrainSimulation");
    fullConfig->merge(*memoryConfig);
    fullConfig->merge(*neuromodConfig);
    fullConfig->merge(*envConfig);
    
    std::cout << "Full configuration has " << fullConfig->getEntryCount() << " entries" << std::endl;
}

} // namespace nlm
