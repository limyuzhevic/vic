// NeuralRegion Tests
// Neural region management and connectivity

#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_neural_region {

void testNeuralRegionCreation() {
    nlm::NeuralRegion region(5);
    assert(region.getId() == nlm::RegionId(5));
    assert(region.getMaxPopulationSize() == 5);
    std::cout << "    testNeuralRegionCreation passed" << std::endl;
}

void testNeuralRegionAddPopulation() {
    nlm::NeuralRegion region(10);
    nlm::NeuralPopulation population(5);
    
    region.addPopulation(population);
    assert(region.getPopulationCount() == 1);
    
    std::cout << "    testNeuralRegionAddPopulation passed" << std::endl;
}

void testNeuralRegionGetPopulation() {
    nlm::NeuralRegion region(10);
    nlm::NeuralPopulation testPopulation(3);
    region.addPopulation(testPopulation);
    
    auto retrievedPopulation = region.getPopulation(0);
    assert(retrievedPopulation != nullptr);
    assert(retrievedPopulation->getSize() == 3);
    
    std::cout << "    testNeuralRegionGetPopulation passed" << std::endl;
}

void testNeuralRegionRemovePopulation() {
    nlm::NeuralRegion region(10);
    nlm::NeuralPopulation populationToRemove(5);
    region.addPopulation(populationToRemove);
    
    assert(region.getPopulationCount() == 1);
    
    region.removePopulation(0);
    assert(region.getPopulationCount() == 0);
    assert(region.getPopulation(0) == nullptr);
    
    std::cout << "    testNeuralRegionRemovePopulation passed" << std::endl;
}

void testNeuralRegionGetPopulations() {
    nlm::NeuralRegion region(10);
    
    std::vector<nlm::NeuralPopulation> populations;
    for (int i = 0; i < 3; ++i) {
        nlm::NeuralPopulation population(i + 1);
        region.addPopulation(population);
        populations.push_back(population);
    }
    
    auto retrievedPopulations = region.getPopulations();
    assert(retrievedPopulations.size() == 3);
    
    for (size_t i = 0; i < populations.size(); ++i) {
        assert(retrievedPopulations[i]->getSize() == populations[i].getSize());
    }
    
    std::cout << "    testNeuralRegionGetPopulations passed" << std::endl;
}

void testNeuralRegionAddSynapse() {
    nlm::NeuralRegion region(10);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    region.addSynapse(synapse);
    assert(region.getSynapseCount() == 1);
    
    std::cout << "    testNeuralRegionAddSynapse passed" << std::endl;
}

void testNeuralRegionGetSynapse() {
    nlm::NeuralRegion region(10);
    nlm::Synapse testSynapse(nlm::SynapseId(100), nlm::NeuronId(10), nlm::NeuronId(20));
    region.addSynapse(testSynapse);
    
    auto retrievedSynapse = region.getSynapse(nlm::SynapseId(100));
    assert(retrievedSynapse != nullptr);
    assert(retrievedSynapse->getId() == nlm::SynapseId(100));
    assert(retrievedSynapse->getSourceNeuron() == nlm::NeuronId(10));
    assert(retrievedSynapse->getDestinationNeuron() == nlm::NeuronId(20));
    
    std::cout << "    testNeuralRegionGetSynapse passed" << std::endl;
}

void testNeuralRegionRemoveSynapse() {
    nlm::NeuralRegion region(10);
    nlm::Synapse synapseToRemove(nlm::SynapseId(200), nlm::NeuronId(30), nlm::NeuronId(40));
    region.addSynapse(synapseToRemove);
    
    assert(region.getSynapseCount() == 1);
    
    region.removeSynapse(nlm::SynapseId(200));
    assert(region.getSynapseCount() == 0);
    assert(region.getSynapse(nlm::SynapseId(200)) == nullptr);
    
    std::cout << "    testNeuralRegionRemoveSynapse passed" << std::endl;
}

void testNeuralRegionGetSynapses() {
    nlm::NeuralRegion region(10);
    
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 5; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i * 10), nlm::NeuronId(i * 10 + 1));
        region.addSynapse(synapse);
        synapses.push_back(synapse);
    }
    
    auto retrievedSynapses = region.getSynapses();
    assert(retrievedSynapses.size() == 5);
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        assert(retrievedSynapses[i]->getId() == synapses[i].getId());
    }
    
    std::cout << "    testNeuralRegionGetSynapses passed" << std::endl;
}

void testNeuralRegionClear() {
    nlm::NeuralRegion region(10);
    
    nlm::NeuralPopulation population(3);
    region.addPopulation(population);
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    region.addSynapse(synapse);
    
    assert(region.getPopulationCount() == 1);
    assert(region.getSynapseCount() == 1);
    
    region.clear();
    assert(region.getPopulationCount() == 0);
    assert(region.getSynapseCount() == 0);
    
    std::cout << "    testNeuralRegionClear passed" << std::endl;
}

void testNeuralRegionUpdate() {
    nlm::NeuralRegion region(10);
    nlm::RandomGenerator rng(42);
    
    // Add a population with neurons
    nlm::NeuralPopulation population(5);
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        neuron.initializeRandom(rng);
        population.addNeuron(neuron);
    }
    region.addPopulation(population);
    
    // Add synapses
    for (int i = 0; i < 3; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i + 1), nlm::NeuronId(i + 2));
        region.addSynapse(synapse);
    }
    
    // Update region
    region.update(0.001, rng);
    
    // Verify populations still exist
    assert(region.getPopulationCount() == 1);
    assert(region.getSynapseCount() == 3);
    
    std::cout << "    testNeuralRegionUpdate passed" << std::endl;
}

void testNeuralRegionGetId() {
    nlm::NeuralRegion region1(nlm::RegionId(1));
    nlm::NeuralRegion region2(nlm::RegionId(2));
    
    assert(region1.getId() == nlm::RegionId(1));
    assert(region2.getId() == nlm::RegionId(2));
    
    std::cout << "    testNeuralRegionGetId passed" << std::endl;
}

void testNeuralRegionSizeManagement() {
    nlm::NeuralRegion region(5);
    assert(region.getMaxPopulationSize() == 5);
    
    // Add many populations
    for (int i = 0; i < 10; ++i) {
        nlm::NeuralPopulation population(i + 1);
        region.addPopulation(population);
    }
    
    assert(region.getPopulationCount() == 10);
    
    // Add many synapses
    for (int i = 0; i < 20; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        region.addSynapse(synapse);
    }
    
    assert(region.getSynapseCount() == 20);
    
    std::cout << "    testNeuralRegionSizeManagement passed" << std::endl;
}

void runAll() {
    std::cout << "Running NeuralRegion tests..." << std::endl;
    testNeuralRegionCreation();
    testNeuralRegionAddPopulation();
    testNeuralRegionGetPopulation();
    testNeuralRegionRemovePopulation();
    testNeuralRegionGetPopulations();
    testNeuralRegionAddSynapse();
    testNeuralRegionGetSynapse();
    testNeuralRegionRemoveSynapse();
    testNeuralRegionGetSynapses();
    testNeuralRegionClear();
    testNeuralRegionUpdate();
    testNeuralRegionGetId();
    testNeuralRegionSizeManagement();
}

} // namespace test_neural_region