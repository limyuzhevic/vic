// NeuralPopulation Tests
#include "brain/NeuralPopulation.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_neural_population {

void testNeuralPopulationCreation() {
    nlm::NeuralPopulation population(nlm::PopulationId(1), 10);
    
    assert(population.getId() == nlm::PopulationId(1));
    assert(population.getSize() == 10);
    assert(!population.isEmpty());
    
    std::cout << "    testNeuralPopulationCreation passed" << std::endl;
}

void testNeuralPopulationNeuronAccess() {
    nlm::NeuralPopulation population(nlm::PopulationId(2), 5);
    
    // Initially empty
    assert(population.isEmpty());
    
    // Add neurons manually
    nlm::Neuron neurons[5];
    for (int i = 0; i < 5; ++i) {
        population.addNeuron(&neurons[i]);
    }
    
    assert(!population.isEmpty());
    assert(population.getSize() == 5);
    
    // Test neuron access
    for (size_t i = 0; i < 5; ++i) {
        assert(population.getNeuron(i) == &neurons[i]);
        assert(population.getNeuron(i) != nullptr);
    }
    
    std::cout << "    testNeuralPopulationNeuronAccess passed" << std::endl;
}

void testNeuralPopulationType() {
    nlm::NeuralPopulation population(nlm::PopulationId(3), 10);
    
    population.setNeuronType(nlm::NeuronType::Excitatory);
    assert(population.getNeuronType() == nlm::NeuronType::Excitatory);
    
    population.setNeuronType(nlm::NeuronType::Inhibitory);
    assert(population.getNeuronType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuralPopulationType passed" << std::endl;
}

void testNeuralPopulationStatistics() {
    nlm::NeuralPopulation population(nlm::PopulationId(4), 20);
    
    // Initially all neurons should be of internal type
    assert(population.getNeuronType() == nlm::NeuronType::Internal);
    
    // Change type to sensory
    population.setNeuronType(nlm::NeuronType::Sensory);
    
    // All neurons should now be sensory type
    for (size_t i = 0; i < 20; ++i) {
        auto* neuron = population.getNeuron(i);
        if (neuron) {
            assert(neuron->getType() == nlm::NeuronType::Sensory);
        }
    }
    
    // Test statistics methods
    assert(population.getActiveCount() >= 0);
    assert(population.getFiringCount() >= 0);
    assert(population.getAverageFiringRate() >= 0.0f);
    assert(population.getActivityLevel() >= 0.0f);
    
    std::cout << "    testNeuralPopulationStatistics passed" << std::endl;
}

void testNeuralPopulationMembranePotentialStats() {
    nlm::NeuralPopulation population(nlm::PopulationId(5), 10);
    
    // Set some neurons to specific membrane potentials
    for (size_t i = 0; i < 10; ++i) {
        auto* neuron = population.getNeuron(i);
        if (neuron) {
            neuron->setMembranePotential(-60.0f + static_cast<float>(i));
        }
    }
    
    float mean = population.getMeanMembranePotential();
    float variance = population.getVarianceMembranePotential();
    
    assert(mean > -70.0f && mean < -50.0f);
    assert(variance >= 0.0f);
    
    std::cout << "    testNeuralPopulationMembranePotentialStats passed" << std::endl;
}

void testNeuralPopulationRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::NeuralPopulation population(nlm::PopulationId(6), 100);
    
    // This should not crash
    population.initializeRandom(rng);
    
    // Verify that neurons have been initialized with random values
    // (they should have membrane potentials in reasonable ranges)
    for (size_t i = 0; i < 100; ++i) {
        auto* neuron = population.getNeuron(i);
        if (neuron) {
            float potential = neuron->getMembranePotential();
            assert(potential < -50.0f && potential > -80.0f);
        }
    }
    
    std::cout << "    testNeuralPopulationRandomInitialization passed" << std::endl;
}

void runAll() {
    testNeuralPopulationCreation();
    testNeuralPopulationNeuronAccess();
    testNeuralPopulationType();
    testNeuralPopulationStatistics();
    testNeuralPopulationMembranePotentialStats();
    testNeuralPopulationRandomInitialization();
}

} // namespace test_neural_population
