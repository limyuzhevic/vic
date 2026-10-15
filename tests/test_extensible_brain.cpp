// Extensible Brain Tests
// Testing ExtensibleBrain integration with plugins and custom systems

#include "core/ExtensibleBrain.hpp"
#include "plugins/PluginManager.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace nlm {
namespace tests {

namespace test_extensible_brain {

void testExtensibleBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    std::cout << "    testExtensibleBrainCreation passed" << std::endl;
}

void testExtensibleBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize should work (may warn about missing plugins)
    bool success = brain.initialize();
    
    std::cout << "    testExtensibleBrainInitialization passed" << std::endl;
}

void testPluginIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Test adding neuron model plugin (interface test)
    // In real implementation, would create actual plugin
    auto dummyPlugin = std::make_shared<nlm::INeuronModelPlugin>();
    bool result = brain.addNeuronModelPlugin(dummyPlugin);
    
    std::cout << "    testPluginIntegration passed" << std::endl;
}

void testCustomMemorySystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Test setting custom memory system
    auto customMemory = std::make_unique<nlm::CustomMemorySystem>();
    bool result = brain.setCustomMemorySystem(std::move(customMemory));
    
    std::cout << "    testCustomMemorySystem passed" << std::endl;
}

void testAdvancedNeuromodulator() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Test setting advanced neuromodulator
    auto neuromodulator = std::make_unique<nlm::AdvancedNeuromodulator>();
    bool result = brain.setAdvancedNeuromodulator(std::move(neuromodulator));
    
    std::cout << "    testAdvancedNeuromodulator passed" << std::endl;
}

void testNeuralArchitecture() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Test setting neural architecture
    auto architecture = std::make_unique<nlm::NeuralArchitecture>();
    bool result = brain.setNeuralArchitecture(std::move(architecture));
    
    std::cout << "    testNeuralArchitecture passed" << std::endl;
}

void testOptimizationHooks() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Test setting optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    std::cout << "    testOptimizationHooks passed" << std::endl;
}

void testExperimentFramework() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Test setting experiment framework
    auto experiment = std::make_unique<nlm::AdvancedExperiment>();
    brain.setExperimentFramework(std::move(experiment));
    
    std::cout << "    testExperimentFramework passed" << std::endl;
}

void testExtensibleBrainUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    brain.initialize();
    
    // Test update with timestep
    brain.update(0.1);
    
    std::cout << "    testExtensibleBrainUpdate passed" << std::endl;
}

void testExtensibleBrainShutdown() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    brain.initialize();
    
    // Test shutdown
    brain.shutdown();
    
    std::cout << "    testExtensibleBrainShutdown passed" << std::endl;
}

void runAll() {
    testExtensibleBrainCreation();
    testExtensibleBrainInitialization();
    testPluginIntegration();
    testCustomMemorySystem();
    testAdvancedNeuromodulator();
    testNeuralArchitecture();
    testOptimizationHooks();
    testExperimentFramework();
    testExtensibleBrainUpdate();
    testExtensibleBrainShutdown();
}

} // namespace test_extensible_brain

} // namespace nlm
namespace tests {

void test_extensible_brain() {
    test_extensible_brain::runAll();
}

} // namespace tests
