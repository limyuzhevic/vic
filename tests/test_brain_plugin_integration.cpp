// Brain Plugin Integration Tests
// Testing brain with custom plugins integration

#include "brain/Brain.hpp"
#include "plugins/PluginManager.hpp"
#include "core/ExtensibleBrain.hpp"
#include <cassert>
#include <iostream>

namespace nlm {
namespace tests {

namespace test_brain_plugin_integration {

void testBrainPluginIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Test brain initialization
    bool success = brain.initialize();
    
    // Test that brain has plugin manager
    auto* extBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    assert(extBrain != nullptr);
    
    std::cout << "    testBrainPluginIntegration passed" << std::endl;
}

void testBrainWithNeuronModelPlugin() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test basic brain functionality
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 1);
    
    std::cout << "    testBrainWithNeuronModelPlugin passed" << std::endl;
}

void testBrainWithPlasticityPlugin() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test brain with plasticity capabilities
    brain.step(0);
    
    std::cout << "    testBrainWithPlasticityPlugin passed" << std::endl;
}

void testBrainWithCustomMemorySystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set custom memory system
    auto customMemory = std::make_unique<nlm::CustomMemorySystem>();
    brain.setCustomMemorySystem(std::move(customMemory));
    
    // Initialize brain
    brain.initialize();
    
    std::cout << "    testBrainWithCustomMemorySystem passed" << std::endl;
}

void testBrainWithAdvancedNeuromodulator() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set advanced neuromodulator
    auto neuromodulator = std::make_unique<nlm::AdvancedNeuromodulator>();
    brain.setAdvancedNeuromodulator(std::move(neuromodulator));
    
    // Initialize brain
    brain.initialize();
    
    std::cout << "    testBrainWithAdvancedNeuromodulator passed" << std::endl;
}

void testBrainWithNeuralArchitecture() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(90), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set neural architecture
    auto architecture = std::make_unique<nlm::NeuralArchitecture>();
    brain.setNeuralArchitecture(std::move(architecture));
    
    // Initialize brain
    brain.initialize();
    
    std::cout << "    testBrainWithNeuralArchitecture passed" << std::endl;
}

void testBrainWithOptimizationHooks() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set optimization hooks
    auto hooks = std::make_unique<nlm::OptimizationHooks>();
    brain.setOptimizationHooks(std::move(hooks));
    
    // Initialize brain
    brain.initialize();
    
    // Test brain update with hooks
    brain.update(0.1);
    
    std::cout << "    testBrainWithOptimizationHooks passed" << std::endl;
}

void testBrainWithExperimentFramework() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(70), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::ExtensibleBrain brain(config);
    
    // Set experiment framework
    auto experiment = std::make_unique<nlm::AdvancedExperiment>();
    brain.setExperimentFramework(std::move(experiment));
    
    // Initialize brain
    brain.initialize();
    
    std::cout << "    testBrainWithExperimentFramework passed" << std::endl;
}

void runAll() {
    testBrainPluginIntegration();
    testBrainWithNeuronModelPlugin();
    testBrainWithPlasticityPlugin();
    testBrainWithCustomMemorySystem();
    testBrainWithAdvancedNeuromodulator();
    testBrainWithNeuralArchitecture();
    testBrainWithOptimizationHooks();
    testBrainWithExperimentFramework();
}

} // namespace test_brain_plugin_integration

} // namespace nlm
namespace tests {

void test_brain_plugin_integration() {
    test_brain_plugin_integration::runAll();
}

} // namespace tests
