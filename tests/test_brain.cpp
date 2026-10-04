// Unit test for brain functionality
#include <iostream>
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"

int test_unit_brain() {
    std::cout << "Unit Brain Test: Testing neural components..." << std::endl;
    
    // Test neuron creation
    nlm::Neuron test_neuron;
    test_neuron.setMembranePotential(-70.0f);
    test_neuron.setRefractoryPeriod(2.0f);
    
    std::cout << "Neuron created with membrane potential: " << test_neuron.getMembranePotential() << " mV" << std::endl;
    
    // Test synapse creation
    nlm::Synapse test_synapse(0.1f, 0.9f);
    
    std::cout << "Synapse created with weight: " << test_synapse.getWeight() << std::endl;
    
    return 0;
}

int main() {
    return test_unit_brain();
}
