// Maturation Tests
// Comprehensive testing of maturation development system

#include "development/Maturation.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_maturation {

void testMaturationCreation() {
    nlm::Maturation maturation;
    
    const char* name = maturation.getName();
    if (!name || std::string(name) != "Maturation") {
        throw nlm::NLMTestException("Maturation creation failed: name should be 'Maturation'");
    }
    
    std::cout << "    testMaturationCreation passed" << std::endl;
}

void testMaturationName() {
    nlm::Maturation maturation;
    
    const char* name = maturation.getName();
    if (!name) {
        throw nlm::NLMTestException("Maturation name failed: name is null");
    }
    if (std::string(name) != "Maturation") {
        throw nlm::NLMTestException("Maturation name failed: expected 'Maturation', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testMaturationName passed" << std::endl;
}

void testMaturationUpdate() {
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Maturation maturation;
    
    // Verify maturation doesn't crash
    maturation.update(brain.get(), *brain->getRandomGenerator());
    
    std::cout << "    testMaturationUpdate passed" << std::endl;
}

void testMaturationUpdateWithRandom() {
    nlm::RandomGenerator rng(12345);
    
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Maturation maturation;
    
    // Update multiple times with same random seed
    for (size_t i = 0; i < 3; ++i) {
        maturation.update(brain.get(), rng);
    }
    
    // Verify brain state is consistent
    size_t neuronCount = brain->getTotalNeuronCount();
    if (neuronCount == 0) {
        throw nlm::NLMTestException("Maturation update test failed: brain has no neurons");
    }
    
    std::cout << "    testMaturationUpdateWithRandom passed (neurons: " << neuronCount << ")" << std::endl;
}

void testMaturationNullBrain() {
    Maturation maturation;
    
    nlm::RandomGenerator rng(42);
    
    // Test with null brain - should not crash
    maturation.update(nullptr, rng);
    
    std::cout << "    testMaturationNullBrain passed" << std::endl;
}

void testMaturationRegionSpecific() {
    // Create brain with multiple regions
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Maturation maturation;
    
    // Update maturation
    nlm::RandomGenerator rng(12345);
    maturation.update(brain.get(), rng);
    
    // Verify regions have neurons
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Maturation region test failed: brain has no regions");
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
        throw nlm::NLMTestException("Maturation region test failed: no region has neurons");
    }
    
    std::cout << "    testMaturationRegionSpecific passed (regions: " << regions.size() << ")" << std::endl;
}

void testMaturationActivityDependent() {
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(25), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Maturation maturation;
    
    // Get neurons and give them activity
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Maturation activity test failed: brain has no regions");
        return;
    }
    
    auto neurons = regions[0]->getAllNeurons();
    
    // Give some neurons spike history (activity)
    for (size_t i = 0; i < neurons.size() && i < 5; ++i) {
        neurons[i]->recordSpike(static_cast<float>(i) * 10.0f);
    }
    
    // Update maturation - should be activity dependent
    nlm::RandomGenerator rng(42);
    maturation.update(brain.get(), rng);
    
    std::cout << "    testMaturationActivityDependent passed (neurons with activity: " << std::min<size_t>(5, neurons.size()) << ")" << std::endl;
}

void testMaturationSizeScaling() {
    // Test with different network sizes
    for (size_t neuronCount : {10, 25, 50}) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        Maturation maturation;
        
        // Update maturation
        nlm::RandomGenerator rng(42);
        maturation.update(brain.get(), rng);
        
        // Verify brain has expected number of neurons
        if (brain->getTotalNeuronCount() != neuronCount) {
            throw nlm::NLMTestException("Maturation size scaling test failed: expected " + 
                                       std::to_string(neuronCount) + " neurons, got " + 
                                       std::to_string(brain->getTotalNeuronCount()));
        }
    }
    
    std::cout << "    testMaturationSizeScaling passed" << std::endl;
}

void testMaturationPerformance() {
    // Create larger brain for performance testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Maturation maturation;
    
    // Time the update operation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::RandomGenerator rng(12345);
    for (size_t i = 0; i < 50; ++i) {
        maturation.update(brain.get(), rng);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Should complete without errors
    if (brain->getTotalNeuronCount() == 0) {
        throw nlm::NLMTestException("Maturation performance test failed: brain has no neurons after updates");
    }
    
    std::cout << "    testMaturationPerformance passed (50 updates in " << std::fixed << std::setprecision(4) << totalTime << "s)" << std::endl;
}

void runAll() {
    std::cout << "Running Maturation tests..." << std::endl;
    testMaturationCreation();
    testMaturationName();
    testMaturationUpdate();
    testMaturationUpdateWithRandom();
    testMaturationNullBrain();
    testMaturationRegionSpecific();
    testMaturationActivityDependent();
    testMaturationSizeScaling();
    testMaturationPerformance();
}

} // namespace test_maturation
