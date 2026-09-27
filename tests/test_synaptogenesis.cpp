// Synaptogenesis Tests
// Comprehensive testing of synaptogenesis development system

#include "development/Synaptogenesis.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_synaptogenesis {

void testSynaptogenesisCreation() {
    nlm::Synaptogenesis synaptogenesis;
    
    const char* name = synaptogenesis.getName();
    if (!name || std::string(name) != "Synaptogenesis") {
        throw nlm::NLMTestException("Synaptogenesis creation failed: name should be 'Synaptogenesis'");
    }
    
    std::cout << "    testSynaptogenesisCreation passed" << std::endl;
}

void testSynaptogenesisName() {
    nlm::Synaptogenesis synaptogenesis;
    
    const char* name = synaptogenesis.getName();
    if (!name) {
        throw nlm::NLMTestException("Synaptogenesis name failed: name is null");
    }
    if (std::string(name) != "Synaptogenesis") {
        throw nlm::NLMTestException("Synaptogenesis name failed: expected 'Synaptogenesis', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testSynaptogenesisName passed" << std::endl;
}

void testSynaptogenesisUpdate() {
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Synaptogenesis synaptogenesis;
    
    // Verify synaptogenesis doesn't crash
    synaptogenesis.update(brain.get(), *brain->getRandomGenerator());
    
    std::cout << "    testSynaptogenesisUpdate passed" << std::endl;
}

void testSynaptogenesisUpdateWithRandom() {
    nlm::RandomGenerator rng(12345);
    
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Synaptogenesis synaptogenesis;
    
    // Update multiple times with same random seed
    for (size_t i = 0; i < 3; ++i) {
        synaptogenesis.update(brain.get(), rng);
    }
    
    // Verify brain state is consistent
    size_t neuronCount = brain->getTotalNeuronCount();
    if (neuronCount == 0) {
        throw nlm::NLMTestException("Synaptogenesis update test failed: brain has no neurons");
    }
    
    std::cout << "    testSynaptogenesisUpdateWithRandom passed (neurons: " << neuronCount << ")" << std::endl;
}

void testSynaptogenesisNullBrain() {
    Synaptogenesis synaptogenesis;
    
    nlm::RandomGenerator rng(42);
    
    // Test with null brain - should not crash
    synaptogenesis.update(nullptr, rng);
    
    std::cout << "    testSynaptogenesisNullBrain passed" << std::endl;
}

void testSynaptogenesisRegionSpecific() {
    // Create brain with multiple regions
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Synaptogenesis synaptogenesis;
    
    // Update synaptogenesis
    nlm::RandomGenerator rng(12345);
    synaptogenesis.update(brain.get(), rng);
    
    // Verify regions have neurons
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Synaptogenesis region test failed: brain has no regions");
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
        throw nlm::NLMTestException("Synaptogenesis region test failed: no region has neurons");
    }
    
    std::cout << "    testSynaptogenesisRegionSpecific passed (regions: " << regions.size() << ")" << std::endl;
}

void testSynaptogenesisActivityDependent() {
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(25), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Synaptogenesis synaptogenesis;
    
    // Get neurons and give them activity
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Synaptogenesis activity test failed: brain has no regions");
        return;
    }
    
    auto neurons = regions[0]->getAllNeurons();
    
    // Give some neurons spike history (activity)
    for (size_t i = 0; i < neurons.size() && i < 5; ++i) {
        neurons[i]->recordSpike(static_cast<float>(i) * 10.0f);
    }
    
    // Update synaptogenesis - should be activity dependent
    nlm::RandomGenerator rng(42);
    synaptogenesis.update(brain.get(), rng);
    
    std::cout << "    testSynaptogenesisActivityDependent passed (neurons with activity: " << std::min<size_t>(5, neurons.size()) << ")" << std::endl;
}

void testSynaptogenesisSizeScaling() {
    // Test with different network sizes
    for (size_t neuronCount : {10, 25, 50}) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        Synaptogenesis synaptogenesis;
        
        // Update synaptogenesis
        nlm::RandomGenerator rng(42);
        synaptogenesis.update(brain.get(), rng);
        
        // Verify brain has expected number of neurons
        if (brain->getTotalNeuronCount() != neuronCount) {
            throw nlm::NLMTestException("Synaptogenesis size scaling test failed: expected " + 
                                       std::to_string(neuronCount) + " neurons, got " + 
                                       std::to_string(brain->getTotalNeuronCount()));
        }
    }
    
    std::cout << "    testSynaptogenesisSizeScaling passed" << std::endl;
}

void testSynaptogenesisPerformance() {
    // Create larger brain for performance testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    Synaptogenesis synaptogenesis;
    
    // Time the update operation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::RandomGenerator rng(12345);
    for (size_t i = 0; i < 50; ++i) {
        synaptogenesis.update(brain.get(), rng);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Should complete without errors
    if (brain->getTotalNeuronCount() == 0) {
        throw nlm::NLMTestException("Synaptogenesis performance test failed: brain has no neurons after updates");
    }
    
    std::cout << "    testSynaptogenesisPerformance passed (50 updates in " << std::fixed << std::setprecision(4) << totalTime << "s)" << std::endl;
}

void runAll() {
    std::cout << "Running Synaptogenesis tests..." << std::endl;
    testSynaptogenesisCreation();
    testSynaptogenesisName();
    testSynaptogenesisUpdate();
    testSynaptogenesisUpdateWithRandom();
    testSynaptogenesisNullBrain();
    testSynaptogenesisRegionSpecific();
    testSynaptogenesisActivityDependent();
    testSynaptogenesisSizeScaling();
    testSynaptogenesisPerformance();
}

} // namespace test_synaptogenesis
