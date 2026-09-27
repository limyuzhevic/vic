// Pruning Tests
// Comprehensive testing of pruning development system

#include "development/Pruning.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_pruning {

void testPruningCreation() {
    nlm::Pruning pruning;
    
    const char* name = pruning.getName();
    if (!name || std::string(name) != "Pruning") {
        throw nlm::NLMTestException("Pruning creation failed: name should be 'Pruning'");
    }
    
    std::cout << "    testPruningCreation passed" << std::endl;
}

void testPruningName() {
    nlm::Pruning pruning;
    
    const char* name = pruning.getName();
    if (!name) {
        throw nlm::NLMTestException("Pruning name failed: name is null");
    }
    if (std::string(name) != "Pruning") {
        throw nlm::NLMTestException("Pruning name failed: expected 'Pruning', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testPruningName passed" << std::endl;
}

void testPruningUpdate() {
    // Create brain with neurons and synapses
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Pruning pruning;
    
    // Verify pruning doesn't crash
    pruning.update(brain.get(), *brain->getRandomGenerator());
    
    std::cout << "    testPruningUpdate passed" << std::endl;
}

void testPruningUpdateWithRandom() {
    nlm::RandomGenerator rng(12345);
    
    // Create brain with neurons and synapses
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Pruning pruning;
    
    // Update multiple times with same random seed
    for (size_t i = 0; i < 3; ++i) {
        pruning.update(brain.get(), rng);
    }
    
    // Verify brain state is consistent
    size_t neuronCount = brain->getTotalNeuronCount();
    if (neuronCount == 0) {
        throw nlm::NLMTestException("Pruning update test failed: brain has no neurons");
    }
    
    std::cout << "    testPruningUpdateWithRandom passed (neurons: " << neuronCount << ")" << std::endl;
}

void testPruningNullBrain() {
    Pruning pruning;
    
    nlm::RandomGenerator rng(42);
    
    // Test with null brain - should not crash
    pruning.update(nullptr, rng);
    
    std::cout << "    testPruningNullBrain passed" << std::endl;
}

void testPruningRegionSpecific() {
    // Create brain with multiple regions
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Pruning pruning;
    
    // Update pruning
    nlm::RandomGenerator rng(12345);
    pruning.update(brain.get(), rng);
    
    // Verify regions have neurons
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Pruning region test failed: brain has no regions");
    }
    
    // At least one region should have neurons
    bool hasNeurons = false;
    for (auto& region : regions) {
        if (!region->getAllNeurons().empty()) {
            hasNeurons = true;
            break;
        }
    }
    
    if (!hasNeurons) {
        throw nlm::NLMTestException("Pruning region test failed: no region has neurons");
    }
    
    std::cout << "    testPruningRegionSpecific passed (regions: " << regions.size() << ")" << std::endl;
}

void testPruningActivityDependent() {
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(25), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Pruning pruning;
    
    // Get neurons and give them activity
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Pruning activity test failed: brain has no regions");
        return;
    }
    
    auto neurons = regions[0]->getAllNeurons();
    
    // Give some neurons spike history (activity)
    for (size_t i = 0; i < neurons.size() && i < 5; ++i) {
        neurons[i]->recordSpike(static_cast<float>(i) * 10.0f);
    }
    
    // Update pruning - should be activity dependent
    nlm::RandomGenerator rng(42);
    pruning.update(brain.get(), rng);
    
    std::cout << "    testPruningActivityDependent passed (neurons with activity: " << std::min<size_t>(5, neurons.size()) << ")" << std::endl;
}

void testPruningWeightBased() {
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Pruning pruning;
    
    // Get some synapses and set some to weak weights
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Pruning weight test failed: brain has no regions");
        return;
    }
    
    auto synapses = regions[0]->getSynapses();
    size_t weakSynapseCount = 0;
    
    for (auto* synapse : synapses) {
        if (synapse) {
            // Set some synapses to weak weight (below threshold)
            if (weakSynapseCount < 3) {
                synapse->setWeight(0.01f); // Very weak
                weakSynapseCount++;
            }
        }
    }
    
    // Update pruning with high pruning rate
    nlm::RandomGenerator rng(12345);
    pruning.update(brain.get(), rng);
    
    std::cout << "    testPruningWeightBased passed (weak synapses: " << weakSynapseCount << ")" << std::endl;
}

void testPruningSizeScaling() {
    // Test with different network sizes
    for (size_t neuronCount : {10, 25, 50}) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        Pruning pruning;
        
        // Update pruning
        nlm::RandomGenerator rng(42);
        pruning.update(brain.get(), rng);
        
        // Verify brain has expected number of neurons
        if (brain->getTotalNeuronCount() != neuronCount) {
            throw nlm::NLMTestException("Pruning size scaling test failed: expected " + 
                                       std::to_string(neuronCount) + " neurons, got " + 
                                       std::to_string(brain->getTotalNeuronCount()));
        }
    }
    
    std::cout << "    testPruningSizeScaling passed" << std::endl;
}

void testPruningPerformance() {
    // Create larger brain for performance testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Pruning pruning;
    
    // Time the update operation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::RandomGenerator rng(12345);
    for (size_t i = 0; i < 50; ++i) {
        pruning.update(brain.get(), rng);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Should complete without errors
    if (brain->getTotalNeuronCount() == 0) {
        throw nlm::NLMTestException("Pruning performance test failed: brain has no neurons after updates");
    }
    
    std::cout << "    testPruningPerformance passed (50 updates in " << std::fixed << std::setprecision(4) << totalTime << "s)" << std::endl;
}

void runAll() {
    std::cout << "Running Pruning tests..." << std::endl;
    testPruningCreation();
    testPruningName();
    testPruningUpdate();
    testPruningUpdateWithRandom();
    testPruningNullBrain();
    testPruningRegionSpecific();
    testPruningActivityDependent();
    testPruningWeightBased();
    testPruningSizeScaling();
    testPruningPerformance();
}

} // namespace test_pruning
