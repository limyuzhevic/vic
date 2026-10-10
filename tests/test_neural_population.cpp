// NeuralPopulation Tests
// Neural population management and operations

#include "brain/NeuralPopulation.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_neural_population {

void testNeuralPopulationCreation() {
    nlm::NeuralPopulation population(10);
    assert(population.getSize() == 10);
    assert(population.getTotalNeurons() == 10);
    std::cout << "    testNeuralPopulationCreation passed" << std::endl;
}

void testNeuralPopulationAddNeuron() {
    nlm::NeuralPopulation population(5);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    population.addNeuron(neuron);
    assert(population.getSize() == 6);
    assert(population.getTotalNeurons() == 6);
    
    std::cout << "    testNeuralPopulationAddNeuron passed" << std::endl;
}

void testNeuralPopulationGetNeuron() {
    nlm::NeuralPopulation population(5);
    nlm::Neuron testNeuron(nlm::NeuronId(100));
    population.addNeuron(testNeuron);
    
    auto retrievedNeuron = population.getNeuron(nlm::NeuronId(100));
    assert(retrievedNeuron != nullptr);
    assert(retrievedNeuron->getId() == nlm::NeuronId(100));
    
    std::cout << "    testNeuralPopulationGetNeuron passed" << std::endl;
}

void testNeuralPopulationRemoveNeuron() {
    nlm::NeuralPopulation population(5);
    nlm::Neuron neuronToRemove(nlm::NeuronId(200));
    population.addNeuron(neuronToRemove);
    
    assert(population.getSize() == 6);
    
    population.removeNeuron(nlm::NeuronId(200));
    assert(population.getSize() == 5);
    assert(population.getNeuron(nlm::NeuronId(200)) == nullptr);
    
    std::cout << "    testNeuralPopulationRemoveNeuron passed" << std::endl;
}

void testNeuralPopulationGetNeurons() {
    nlm::NeuralPopulation population(5);
    
    std::vector<nlm::NeuronId> neuronIds;
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        population.addNeuron(neuron);
        neuronIds.push_back(nlm::NeuronId(i + 1));
    }
    
    auto neurons = population.getNeurons();
    assert(neurons.size() == 5);
    
    for (const auto& id : neuronIds) {
        bool found = false;
        for (const auto& neuron : neurons) {
            if (neuron.getId() == id) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    std::cout << "    testNeuralPopulationGetNeurons passed" << std::endl;
}

void testNeuralPopulationClear() {
    nlm::NeuralPopulation population(5);
    
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        population.addNeuron(neuron);
    }
    
    assert(population.getSize() == 5);
    
    population.clear();
    assert(population.getSize() == 0);
    assert(population.getNeurons().empty());
    
    std::cout << "    testNeuralPopulationClear passed" << std::endl;
}

void testNeuralPopulationActivate() {
    nlm::NeuralPopulation population(5);
    
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        neuron.setFiringState(nlm::FiringState::Active);
        population.addNeuron(neuron);
    }
    
    population.activate();
    
    auto neurons = population.getNeurons();
    for (const auto& neuron : neurons) {
        assert(neuron.isFiring());
    }
    
    std::cout << "    testNeuralPopulationActivate passed" << std::endl;
}

void testNeuralPopulationUpdate() {
    nlm::NeuralPopulation population(5);
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        neuron.initializeRandom(rng);
        population.addNeuron(neuron);
    }
    
    population.update(0.001, rng);
    
    assert(population.getSize() == 5);
    
    std::cout << "    testNeuralPopulationUpdate passed" << std::endl;
}

void testNeuralPopulationSizeManagement() {
    nlm::NeuralPopulation population(10);
    assert(population.getSize() == 10);
    
    for (int i = 0; i < 20; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 100));
        population.addNeuron(neuron);
    }
    
    assert(population.getSize() == 30);
    assert(population.getTotalNeurons() == 30);
    
    for (int i = 0; i < 10; ++i) {
        population.removeNeuron(nlm::NeuronId(i + 1));
    }
    
    assert(population.getSize() == 20);
    assert(population.getTotalNeurons() == 20);
    
    std::cout << "    testNeuralPopulationSizeManagement passed" << std::endl;
}

void runAll() {
    std::cout << "Running NeuralPopulation tests..." << std::endl;
    testNeuralPopulationCreation();
    testNeuralPopulationAddNeuron();
    testNeuralPopulationGetNeuron();
    testNeuralPopulationRemoveNeuron();
    testNeuralPopulationGetNeurons();
    testNeuralPopulationClear();
    testNeuralPopulationActivate();
    testNeuralPopulationUpdate();
    testNeuralPopulationSizeManagement();
}

} // namespace test_neural_population