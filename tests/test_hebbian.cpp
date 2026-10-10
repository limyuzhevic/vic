// Hebbian Plasticity Tests
// Hebbian learning and synaptic plasticity

#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeightChange() == 1.0f);
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianLearningRate() {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    assert(hebbian.getLearningRate() == 0.05f);
    
    hebbian.setLearningRate(0.1f);
    assert(hebbian.getLearningRate() == 0.1f);
    
    std::cout << "    testHebbianLearningRate passed" << std::endl;
}

void testHebbianMaxWeightChange() {
    nlm::Hebbian hebbian;
    
    hebbian.setMaxWeightChange(2.0f);
    assert(hebbian.getMaxWeightChange() == 2.0f);
    
    hebbian.setMaxWeightChange(5.0f);
    assert(hebbian.getMaxWeightChange() == 5.0f);
    
    std::cout << "    testHebbianMaxWeightChange passed" << std::endl;
}

void testHebbianPlasticityUpdate() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    nlm::Neuron preNeuron(nlm::NeuronId(1));
    nlm::Neuron postNeuron(nlm::NeuronId(2));
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    preNeuron.recordSpike(0.0);
    postNeuron.recordSpike(10.0);
    
    hebbian.update(&synapse, preNeuron, postNeuron, 0.001);
    
    assert(synapse.getWeight() > 0.5f);
    
    std::cout << "    testHebbianPlasticityUpdate passed" << std::endl;
}

void testHebbianNoPlasticityForWrongOrder() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    nlm::Neuron preNeuron(nlm::NeuronId(1));
    nlm::Neuron postNeuron(nlm::NeuronId(2));
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    postNeuron.recordSpike(0.0);
    preNeuron.recordSpike(10.0);
    
    hebbian.update(&synapse, preNeuron, postNeuron, 0.001);
    
    assert(synapse.getWeight() <= 0.5f);
    
    std::cout << "    testHebbianNoPlasticityForWrongOrder passed" << std::endl;
}

void testHebbianWeightNormalization() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 5; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.3f + i * 0.1f);
        synapses.push_back(synapse);
    }
    
    nlm::Neuron preNeuron(nlm::NeuronId(100));
    nlm::Neuron postNeuron(nlm::NeuronId(101));
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        preNeuron.recordSpike(0.0);
        postNeuron.recordSpike(10.0);
        hebbian.update(&synapses[i], preNeuron, postNeuron, 0.001);
    }
    
    for (const auto& synapse : synapses) {
        assert(synapse.getWeight() >= -1.0f);
        assert(synapse.getWeight() <= 1.0f);
    }
    
    std::cout << "    testHebbianWeightNormalization passed" << std::endl;
}

void testHebbianName() {
    nlm::Hebbian hebbian;
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "    testHebbianName passed" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianLearningRate();
    testHebbianMaxWeightChange();
    testHebbianPlasticityUpdate();
    testHebbianNoPlasticityForWrongOrder();
    testHebbianWeightNormalization();
    testHebbianName();
}

} // namespace test_hebbian