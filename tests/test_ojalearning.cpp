#include "tests/TestOjaLearning.h"
#include "src/plasticity/OjaLearning.hpp"
#include "src/brain/Synapse.hpp"
#include <cassert>
#include <iostream>

void testOjaLearning_basic() {
    std::cout << "Testing Oja Learning basic functionality..." << std::endl;
    
    OjaLearning oja(0.01f, 1.0f);
    
    // Create a mock synapse
    auto synapse = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
    synapse->setType(SynapseType::Excitatory);
    synapse->setWeight(0.1f);
    
    // Enable Oja learning
    synapse->enablePlasticity(false, false, false); // Custom flag for Oja
    
    // Create spike patterns
    std::vector<Timestamp> preSpikes;
    std::vector<Timestamp> postSpikes;
    
    // Multiple correlated spikes
    for (int i = 0; i < 10; ++i) {
        preSpikes.push_back(static_cast<Timestamp>(i * 10));
        postSpikes.push_back(static_cast<Timestamp>(i * 10 + 5));
    }
    
    // Apply Oja learning
    oja.update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    // Weight should have changed (Oja normalizes the update)
    float finalWeight = synapse->getWeight();
    assert(finalWeight != 0.1f); // Should have changed
    
    // Test weight limits
    synapse->setWeight(1.5f); // Exceed limit
    oja.applyWeightChange(synapse.get(), -0.1f); // Should clamp
    assert(synapse->getWeight() <= 1.0f); // Should be clamped to limit
    
    synapse->setWeight(-1.5f); // Exceed negative limit
    oja.applyWeightChange(synapse.get(), 0.1f); // Should clamp
    assert(synapse->getWeight() >= -1.0f); // Should be clamped to min
    
    std::cout << "  ✓ Oja Learning basic tests passed" << std::endl;
}

void testOjaLearning_parameterConfiguration() {
    std::cout << "Testing Oja Learning parameter configuration..." << std::endl;
    
    OjaLearning oja(0.02f, 2.0f);
    
    assert(oja.getLearningRate() == 0.02f);
    assert(oja.getMaxWeight() == 2.0f);
    assert(oja.getMinWeight() == -2.0f);
    
    // Test parameter updates
    oja.setLearningRate(0.05f);
    assert(oja.getLearningRate() == 0.05f);
    
    oja.setMaxWeight(5.0f);
    assert(oja.getMaxWeight() == 5.0f);
    assert(oja.getMinWeight() == -5.0f);
    
    oja.setMinWeight(-3.0f);
    assert(oja.getMinWeight() == -3.0f);
    assert(oja.getMaxWeight() == 3.0f);
    
    std::cout << "  ✓ Oja Learning parameter tests passed" << std::endl;
}

void testOjaLearning_normalization() {
    std::cout << "Testing Oja Learning normalization behavior..." << std::endl;
    
    OjaLearning oja(0.01f, 1.0f);
    
    // Create a synapse with high weight
    auto synapse = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
    synapse->setType(SynapseType::Excitatory);
    synapse->setWeight(0.9f); // High weight
    
    // Create correlated spike patterns
    std::vector<Timestamp> preSpikes;
    std::vector<Timestamp> postSpikes;
    
    for (int i = 0; i < 20; ++i) {
        preSpikes.push_back(static_cast<Timestamp>(i * 10));
        postSpikes.push_back(static_cast<Timestamp>(i * 10 + 5));
    }
    
    // Apply Oja learning many times
    float previousWeight = synapse->getWeight();
    for (int step = 0; step < 5; ++step) {
        oja.update(synapse.get(), preSpikes, postSpikes, 0.001f);
        float currentWeight = synapse->getWeight();
        
        // Oja should prevent weight from exploding
        assert(currentWeight <= 1.0f); // Should stay within bounds
        assert(currentWeight >= -1.0f); // Should stay within bounds
    }
    
    // Weight should converge rather than explode
    // (exact behavior depends on learning rate and input statistics)
    
    std::cout << "  ✓ Oja Learning normalization tests passed" << std::endl;
}

int main() {
    std::cout << "=== Oja Learning Unit Tests ===" << std::endl << std::endl;
    
    try {
        testOjaLearning_basic();
        std::cout << std::endl;
        
        testOjaLearning_parameterConfiguration();
        std::cout << std::endl;
        
        testOjaLearning_normalization();
        std::cout << std::endl;
        
        std::cout << "=== All Oja Learning tests passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}