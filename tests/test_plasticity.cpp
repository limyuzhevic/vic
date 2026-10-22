#include "tests/TestPlasticity.h"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void testSTDP_basic() {
    std::cout << "Testing STDP basic functionality..." << std::endl;
    
    STDP stdp;
    
    // Create a mock synapse
    auto synapse = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
    synapse->setType(SynapseType::Excitatory);
    synapse->setWeight(0.1f);
    
    // Enable STDP
    synapse->enablePlasticity(false, true, false);  // Only STDP
    
    // Create spike timing data (pre before post = LTP)
    std::vector<Timestamp> preSpikes;
    std::vector<Timestamp> postSpikes;
    
    preSpikes.push_back(0.0);   // pre at 0ms
    postSpikes.push_back(20.0); // post at 20ms (pre before post)
    
    // Apply STDP
    stdp.update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    // Weight should have increased (LTP)
    float finalWeight = synapse->getWeight();
    assert(finalWeight > 0.1f);  // Should have potentiated
    
    // Test depression (post before pre)
    synapse->setWeight(0.1f);
    preSpikes.clear();
    postSpikes.clear();
    
    preSpikes.push_back(20.0);  // pre at 20ms
    postSpikes.push_back(0.0);  // post at 0ms (post before pre)
    
    stdp.update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    // Weight should have decreased (LTD)
    float finalWeight2 = synapse->getWeight();
    assert(finalWeight2 < 0.1f);  // Should have depressed
    
    std::cout << "  ✓ STDP basic tests passed" << std::endl;
}

void testHebbian_basic() {
    std::cout << "Testing Hebbian basic functionality..." << std::endl;
    
    Hebbian hebbian;
    
    // Create a mock synapse
    auto synapse = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
    synapse->setType(SynapseType::Excitatory);
    synapse->setWeight(0.1f);
    
    // Enable Hebbian plasticity
    synapse->enablePlasticity(true, false, false);  // Only Hebbian
    
    // Create correlated spike patterns
    std::vector<Timestamp> preSpikes;
    std::vector<Timestamp> postSpikes;
    
    // Multiple correlated spikes
    for (int i = 0; i < 5; ++i) {
        preSpikes.push_back(static_cast<Timestamp>(i * 10));
        postSpikes.push_back(static_cast<Timestamp>(i * 10 + 5));  // post shortly after pre
    }
    
    // Apply Hebbian learning
    hebbian.update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    // Weight should have increased due to correlation
    float finalWeight = synapse->getWeight();
    assert(finalWeight > 0.1f);  // Should have potentiated
    
    std::cout << "  ✓ Hebbian basic tests passed" << std::endl;
}

void testPlasticity_parameters() {
    std::cout << "Testing plasticity parameter configuration..." << std::endl;
    
    STDP stdp;
    Hebbian hebbian;
    
    // Test STDP parameter configuration
    stdp.configure(0.02f, 0.015f, 20.0f);  // ltp, ltd, tau
    assert(stdp.getLTPWeight() == 0.02f);
    assert(stdp.getLTDWeight() == 0.015f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    // Test Hebbian parameter configuration
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    hebbian.setMinWeight(-0.5f);
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeight() == 2.0f);
    assert(hebbian.getMinWeight() == -0.5f);
    
    std::cout << "  ✓ Plasticity parameter tests passed" << std::endl;
}

int main() {
    std::cout << "=== Plasticity Unit Tests ===" << std::endl << std::endl;
    
    try {
        testSTDP_basic();
        std::cout << std::endl;
        
        testHebbian_basic();
        std::cout << std::endl;
        
        testPlasticity_parameters();
        std::cout << std::endl;
        
        std::cout << "=== All Plasticity tests passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}