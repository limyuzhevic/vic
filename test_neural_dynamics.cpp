#include "src/dynamics/NeuralDynamics.hpp"
#include "src/brain/Neuron.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NeuralDynamics implementation..." << std::endl;
    
    // Create neurons
    nlm::Neuron neuron1(nlm::NeuronId(1));
    neuron1.setType(nlm::NeuronType::Excitatory);
    
    nlm::Neuron neuron2(nlm::NeuronId(2));
    neuron2.setType(nlm::NeuronType::Inhibitory);
    
    // Create a simple integrate-and-fire dynamics system
    nlm::IntegrateAndFireDynamics lifDynamics;
    
    // Set some parameters
    lifDynamics.setTemperature(310.15f);  // 37°C
    lifDynamics.setNoiseLevel(0.01f);
    lifDynamics.setRandomSeed(42);
    lifDynamics.setMembraneCapacitance(100.0f);
    
    // Test neuron updates
    for (int i = 0; i < 10; ++i) {
        neuron1.injectCurrent(10.0f);
        
        // Update neuron using LIF dynamics
        lifDynamics.updateNeuron(&neuron1, 0.001f);  // 1ms timestep
        
        // Check if neuron fired
        if (lifDynamics.shouldFire(&neuron1)) {
            std::cout << "Neuron 1 fired at step " << i << std::endl;
        }
    }
    
    // Test synaptic dynamics
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Apply spike input
    lifDynamics.applySpikeInput(&neuron2, &synapse);
    
    // Test HH dynamics
    nlm::HodgkinHuxleyDynamics hhDynamics;
    hhDynamics.setTemperature(310.15f);
    hhDynamics.setNoiseLevel(0.01f);
    
    // Test FHN dynamics
    nlm::FitzHughNagumoDynamics fhnDynamics;
    fhnDynamics.setTemperature(310.15f);
    fhnDynamics.setNoiseLevel(0.01f);
    
    std::cout << "All NeuralDynamics tests passed!" << std::endl;
    return 0;
}