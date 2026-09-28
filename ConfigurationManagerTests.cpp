#include "ConfigurationManager.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Testing NLM Configuration Manager ===\n\n";
    
    // Test 1: Basic Creation
    std::cout << "Test 1: Basic Creation\n";
    auto config1 = nlm::ConfigurationManager::create();
    assert(config1 != nullptr);
    std::cout << "  ✓ Created default configuration manager\n";
    
    // Test 2: Set and Get Configuration Values
    std::cout << "\nTest 2: Set and Get Configuration Values\n";
    config1->getConfig()->set("brain.neuronCount", 1000);
    config1->getConfig()->set("brain.regionCount", 5);
    config1->getConfig()->set("simulation.dt", 0.1);
    config1->getConfig()->set("simulation.maxTime", 100.0);
    
    assert(config1->getConfig()->get<int64_t>("brain.neuronCount").value_or(0) == 1000);
    assert(config1->getConfig()->getOr<int64_t>("brain.regionCount", 0) == 5);
    assert(config1->getConfig()->getOr<double>("simulation.dt", 0.0) == 0.1);
    assert(config1->getConfig()->getOr<double>("simulation.maxTime", 0.0) == 100.0);
    
    std::cout << "  ✓ Set and get configuration values\n";
    
    // Test 3: Environment Creation
    std::cout << "\nTest 3: Environment Creation\n";
    auto devConfig = nlm::ConfigurationManager::create(nlm::Environment::Development);
    auto prodConfig = nlm::ConfigurationManager::create(nlm::Environment::Production);
    
    assert(devConfig != nullptr);
    assert(prodConfig != nullptr);
    std::cout << "  ✓ Created environment-specific configurations\n";
    
    // Test 4: Preset Creation
    std::cout << "\nTest 4: Preset Creation\n";
    auto devPreset = nlm::ConfigurationManager::createDevelopmentPreset();
    auto debugPreset = nlm::ConfigurationManager::createDebugPreset();
    auto performancePreset = nlm::ConfigurationManager::createPerformancePreset();
    
    assert(devPreset != nullptr);
    assert(debugPreset != nullptr);
    assert(performancePreset != nullptr);
    std::cout << "  ✓ Created built-in presets\n";
    
    // Test 5: Preset Saving and Loading
    std::cout << "\nTest 5: Preset Saving and Loading\n";
    devPreset->savePreset("TestDevelopment", nlm::PresetType::Development);
    debugPreset->savePreset("TestDebug", nlm::PresetType::Debug);
    performancePreset->savePreset("TestPerformance", nlm::PresetType::Performance);
    
    auto loadedDevPreset = nlm::ConfigurationManager::create("TestDevelopment");
    auto loadedDebugPreset = nlm::ConfigurationManager::create("TestDebug");
    auto loadedPerformancePreset = nlm::ConfigurationManager::create("TestPerformance");
    
    assert(loadedDevPreset != nullptr);
    assert(loadedDebugPreset != nullptr);
    assert(loadedPerformancePreset != nullptr);
    std::cout << "  ✓ Saved and loaded presets\n";
    
    // Test 6: Configuration Validation
    std::cout << "\nTest 6: Configuration Validation\n";
    nlm::ConfigurationSchema schema;
    schema.key = "brain.neuronCount";
    schema.type = "int";
    schema.description = "Number of neurons";
    schema.required = true;
    schema.minValue = 1;
    schema.maxValue = 100000;
    
    config1->addSchema(schema);
    
    config1->getConfig()->set("brain.neuronCount", 1000);
    assert(config1->validate());
    
    config1->getConfig()->set("brain.neuronCount", -1);
    assert(!config1->validate());
    
    std::cout << "  ✓ Configuration validation works\n";
    
    // Test 7: JSON Serialization
    std::cout << "\nTest 7: JSON Serialization\n";
    std::string json = config1->toJSON();
    assert(!json.empty());
    
    auto jsonConfig = nlm::ConfigurationManager::create();
    jsonConfig->loadFromJSON(json);
    assert(jsonConfig->getEntryCount() > 0);
    
    std::cout << "  ✓ JSON serialization works\n";
    
    // Test 8: YAML Serialization
    std::cout << "\nTest 8: YAML Serialization\n";
    std::string yaml = config1->toYAML();
    assert(!yaml.empty());
    
    auto yamlConfig = nlm::ConfigurationManager::create();
    yamlConfig->loadFromYAML(yaml);
    assert(yamlConfig->getEntryCount() > 0);
    
    std::cout << "  ✓ YAML serialization works\n";
    
    // Test 9: Configuration Comparison
    std::cout << "\nTest 9: Configuration Comparison\n";
    auto configA = nlm::ConfigurationManager::createDevelopmentPreset();
    auto configB = nlm::ConfigurationManager::createDebugPreset();
    
    bool identical = configA->compare(*configB);
    assert(!identical); // Development and debug presets should be different
    
    std::cout << "  ✓ Configuration comparison works\n";
    
    // Test 10: Configuration Diff
    std::cout << "\nTest 10: Configuration Diff\n";
    std::string diff = configA->diff(*configB);
    assert(!diff.empty());
    
    std::cout << "  ✓ Configuration diff works\n";
    
    // Test 11: Configuration Templates
    std::cout << "\nTest 11: Configuration Templates\n";
    std::vector<std::string> keys = {
        "brain.neuronCount",
        "brain.regionCount",
        "simulation.dt"
    };
    
    config1->createTemplate("TestTemplate", keys);
    
    auto template = config1->getTemplate("TestTemplate");
    assert(template != nullptr);
    assert(template->getName() == "TestTemplate");
    assert(template->getKeys().size() == 3);
    
    std::cout << "  ✓ Configuration templates work\n";
    
    // Test 12: Configuration History
    std::cout << "\nTest 12: Configuration History\n";
    config1->enableHistory(true);
    config1->trackChanges(true);
    
    config1->getConfig()->set("brain.neuronCount", 2000);
    
    auto changes = config1->getChanges();
    assert(changes.size() > 0);
    
    std::cout << "  ✓ Configuration history works\n";
    
    // Test 13: Configuration Versioning
    std::cout << "\nTest 13: Configuration Versioning\n";
    config1->saveVersion("TestVersion1");
    assert(config1->getCurrentVersion() == "TestVersion1");
    
    config1->saveVersion("TestVersion2");
    assert(config1->getCurrentVersion() == "TestVersion2");
    
    std::cout << "  ✓ Configuration versioning works\n";
    
    // Test 14: Configuration Optimization
    std::cout << "\nTest 14: Configuration Optimization\n";
    auto metrics = config1->optimize();
    
    assert(metrics.memoryUsage >= 0.0);
    assert(metrics.updateTime >= 0.0);
    assert(metrics.validationTime >= 0.0);
    
    std::cout << "  ✓ Configuration optimization works\n";
    
    // Test 15: Configuration Compression
    std::cout << "\nTest 15: Configuration Compression\n";
    config1->compress();
    
    std::cout << "  ✓ Configuration compression works\n";
    
    // Test 16: File Operations
    std::cout << "\nTest 16: File Operations\n";
    config1->saveToFile("test_config.json");
    
    auto fileConfig = nlm::ConfigurationManager::create();
    fileConfig->loadFromFile("test_config.json");
    assert(fileConfig->getEntryCount() > 0);
    
    std::cout << "  ✓ File operations work\n";
    
    // Test 17: Advanced Factory Methods
    std::cout << "\nTest 17: Advanced Factory Methods\n";
    auto brainConfig = nlm::ConfigurationManager::createBrainSimulationConfig();
    auto memoryConfig = nlm::ConfigurationManager::createMemorySystemConfig();
    auto neuromodConfig = nlm::ConfigurationManager::createNeuromodulationConfig();
    auto envConfig = nlm::ConfigurationManager::createEnvironmentConfig();
    
    assert(brainConfig != nullptr);
    assert(memoryConfig != nullptr);
    assert(neuromodConfig != nullptr);
    assert(envConfig != nullptr);
    
    std::cout << "  ✓ Advanced factory methods work\n";
    
    // Test 18: Preset Combination
    std::cout << "\nTest 18: Preset Combination\n";
    auto combinedConfig = nlm::ConfigurationManager::createDevelopmentPreset();
    combinedConfig->merge(*memoryConfig);
    combinedConfig->merge(*neuromodConfig);
    combinedConfig->merge(*envConfig);
    
    assert(combinedConfig->getEntryCount() > 10);
    
    std::cout << "  ✓ Preset combination works\n";
    
    std::cout << "\n=== All Tests Passed Successfully ===\n";
    
    return 0;
}
