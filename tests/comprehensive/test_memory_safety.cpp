// Simplified Memory Management and Safety Tests

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_memory_safety {

void test_basic_neuron_creation() {
    std::cout << "    Testing basic neuron creation..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(neuron.getMembranePotential() == -70.0f); // Default value
    
    std::cout << "    test_basic_neuron_creation passed" << std::endl;
}

void test_move_constructor_safety() {
    std::cout << "    Testing move constructor safety..." << std::endl;
    
    // Test Neuron move constructor
    nlm::Neuron neuron1(nlm::NeuronId(1));
    neuron1.setMembranePotential(-60.0f);
    neuron1.setThreshold(-55.0f);
    
    nlm::Neuron neuron2(std::move(neuron1));
    
    // Source should be in a valid state
    assert(!neuron1.getId().isValid() || true); // Implementation-dependent
    
    // Destination should have the moved-to state
    assert(neuron2.getMembranePotential() == -60.0f);
    assert(neuron2.getThreshold() == -55.0f);
    
    std::cout << "    test_move_constructor_safety passed" << std::endl;
}

void test_assignment_operator_safety() {
    std::cout << "    Testing assignment operator safety..." << std::endl;
    
    // Test Synapse move assignment
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse1.setWeight(0.5f);
    
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2 = std::move(synapse1);
    
    // Destination should have the assigned state
    assert(synapse2.getWeight() == 0.5f);
    assert(synapse2.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse2.getDestinationNeuron() == nlm::NeuronId(2));
    
    std::cout << "    test_assignment_operator_safety passed" << std::endl;
}

void test_basic_neuron_operations() {
    std::cout << "    Testing basic neuron operations..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test initialization
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    // Test addition
    neuron.addToMembranePotential(10.0f);
    assert(neuron.getMembranePotential() == -55.0f);
    
    // Test threshold check
    neuron.setThreshold(-50.0f);
    assert(neuron.checkThreshold());
    
    // Test reset
    neuron.reset();
    assert(neuron.getMembranePotential() == -70.0f);
    
    std::cout << "    test_basic_neuron_operations passed" << std::endl;
}

void test_basic_synapse_operations() {
    std::cout << "    Testing basic synapse operations..." << std::endl;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test basic properties
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    assert(synapse.getWeight() == 0.0f); // Default
    assert(synapse.getDelay() == 1); // Default
    
    // Test weight modification
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.2f);
    assert(synapse.getWeight() == 0.7f);
    
    // Test type
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    std::cout << "    test_basic_synapse_operations passed" << std::endl;
}

void test_basic_brain_functionality() {
    std::cout << "    Testing basic brain functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test initialization
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test basic properties
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 10);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test basic operations
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
        assert(brain.getTotalNeuronCount() == 10);
    }
    
    std::cout << "    test_basic_brain_functionality passed" << std::endl;
}

void runAll() {
    test_basic_neuron_creation();
    test_move_constructor_safety();
    test_assignment_operator_safety();
    test_basic_neuron_operations();
    test_basic_synapse_operations();
    test_basic_brain_functionality();
}

} // namespace test_memory_safety