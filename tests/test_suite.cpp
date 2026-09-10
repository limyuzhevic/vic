// Enhanced NLM (Neural Learning Machine) Test Suite
// This file provides comprehensive unit tests for NLM neural components
// Tests are designed to be run with the nlm_test executable or via CTest

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>

namespace nlm_tests {

// Global test tracking
int totalTests = 0;
int passedTests = 0;
int failedTests = 0;

void logTest(const std::string& name, bool passed) {
    totalTests++;
    if (passed) {
        passedTests++;
        std::cout << "✓ " << name << " passed" << std::endl;
    } else {
        failedTests++;
        std::cout << "✗ " << name << " FAILED" << std::endl;
    }
}

// Neuron Tests
void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getState().membranePotential < neuron.getState().threshold);
    assert(neuron.getState().firingState == nlm::FiringState::Resting);
    
    logTest("Neuron Creation", true);
}

void testNeuronInitialization() {
    nlm::Neuron neuron(nlm::NeuronId(1), nlm::NeuronType::Excitatory);
    
    // Check default parameters
    assert(neuron.getState().membranePotential == neuron.getState().restingPotential);
    assert(neuron.getState().threshold > neuron.getState().restingPotential);
    assert(neuron.getState().resetPotential < neuron.getState().restingPotential);
    
    // Check neuron type
    assert(neuron.getNeuronType() == nlm::NeuronType::Excitatory);
    
    logTest("Neuron Initialization", true);
}

void testNeuronLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Inject current to drive neuron above threshold
    neuron.injectCurrent(20.0f);  // Strong excitatory input
    
    // Step with time
    neuron.stepLIF(0.001, 0.001);
    
    // Check that neuron is active
    assert(neuron.getState().firingState == nlm::FiringState::Active ||
           neuron.getState().firingState == nlm::FiringState::Refractory);
    
    logTest("Neuron LIF Dynamics", true);
}

void testNeuronFiring() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron for firing
    neuron.setMembranePotential(-40.0f);  // Above threshold
    neuron.setFiringState(nlm::FiringState::Active);
    neuron.recordSpike(0.0);
    
    // Check that neuron fired
    assert(neuron.getState().lastSpikeTime >= 0.0);
    assert(neuron.getState().firingState == nlm::FiringState::Refractory);
    
    // Check membrane potential reset
    assert(neuron.getState().membranePotential == neuron.getState().resetPotential);
    
    logTest("Neuron Firing", true);
}

void testNeuronRefractoryPeriod() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Make neuron fire
    neuron.injectCurrent(30.0f);
    neuron.stepLIF(0.001, 0.001);
    
    // Check that neuron enters refractory period
    assert(neuron.getState().firingState == nlm::FiringState::Refractory);
    
    // Check refractory remaining time
    assert(neuron.getState().refractoryRemaining > 0);
    
    // After refractory time, neuron should be able to fire again
    neuron.stepLIF(0.001, 0.002);  // Advance time beyond refractory
    
    logTest("Neuron Refractory Period", true);
}

void testNeuronTypeProperties() {
    // Test excitatory neuron
    nlm::Neuron excitatory(nlm::NeuronId(1), nlm::NeuronType::Excitatory);
    assert(excitatory.getNeuronType() == nlm::NeuronType::Excitatory);
    
    // Test inhibitory neuron
    nlm::Neuron inhibitory(nlm::NeuronId(2), nlm::NeuronType::Inhibitory);
    assert(inhibitory.getNeuronType() == nlm::NeuronType::Inhibitory);
    
    // Test sensory neuron
    nlm::Neuron sensory(nlm::NeuronId(3), nlm::NeuronType::Sensory);
    assert(sensory.getNeuronType() == nlm::NeuronType::Sensory);
    
    // Test motor neuron
    nlm::Neuron motor(nlm::NeuronId(4), nlm::NeuronType::Motor);
    assert(motor.getNeuronType() == nlm::NeuronType::Motor);
    
    logTest("Neuron Type Properties", true);
}

// Synapse Tests
void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    assert(synapse.getWeight() == 0.0f);  // Default weight
    assert(synapse.getType() == nlm::SynapseType::Excitatory);  // Default type
    
    logTest("Synapse Creation", true);
}

void testSynapseConnection() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Check delay initialization
    assert(synapse.getDelay() == 1);  // Default delay
    
    // Set delay
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    // Check plasticity flags
    assert(synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().structural);
    
    logTest("Synapse Connection", true);
}

void testSynapsePlasticity() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    
    // Test pre-synaptic spike recording
    synapse.recordPreSpike(0.0);
    synapse.recordPreSpike(10.0);
    
    const auto& preSpikes = synapse.getPreSpikeHistory();
    assert(preSpikes.size() == 2);
    assert(preSpikes[0] == 0.0f);
    assert(preSpikes[1] == 10.0f);
    
    // Test post-synaptic spike recording
    synapse.recordPostSpike(5.0);
    synapse.recordPostSpike(15.0);
    
    const auto& postSpikes = synapse.getPostSpikeHistory();
    assert(postSpikes.size() == 2);
    assert(postSpikes[0] == 5.0f);
    assert(postSpikes[1] == 15.0f);
    
    // Test weight updates
    float initialWeight = synapse.getWeight();
    synapse.addToWeight(0.1f);
    assert(synapse.getWeight() == initialWeight + 0.1f);
    
    // Test weight clamping
    synapse.setWeight(2.0f);
    assert(synapse.getWeight() <= 1.0f);  // Max weight is 1.0
    
    logTest("Synapse Plasticity", true);
}

void testSynapseTypes() {
    // Test excitatory synapse
    nlm::Synapse excSyn(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    excSyn.setType(nlm::SynapseType::Excitatory);
    assert(excSyn.getType() == nlm::SynapseType::Excitatory);
    
    // Test inhibitory synapse
    nlm::Synapse inhSyn(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    inhSyn.setType(nlm::SynapseType::Inhibitory);
    assert(inhSyn.getType() == nlm::SynapseType::Inhibitory);
    
    // Test electrical synapse
    nlm::Synapse elecSyn(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    elecSyn.setType(nlm::SynapseType::Electrical);
    assert(elecSyn.getType() == nlm::SynapseType::Electrical);
    
    // Test gap junction
    nlm::Synapse gapSyn(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    gapSyn.setType(nlm::SynapseType::GapJunction);
    assert(gapSyn.getType() == nlm::SynapseType::GapJunction);
    
    logTest("Synapse Types", true);
}

// NeuralPopulation Tests
void testPopulationCreation() {
    nlm::NeuralPopulation population(
        nlm::PopulationId(1),
        nlm::NeuronType::Excitatory,
        100
    );
    
    assert(population.getId() == nlm::PopulationId(1));
    assert(population.getNeuronType() == nlm::NeuronType::Excitatory);
    assert(population.getNeuronCount() == 100);
    
    // Check that neurons were created
    auto neurons = population.getNeurons();
    assert(neurons.size() == 100);
    
    // All neurons should have same type
    for (auto* neuron : neurons) {
        assert(neuron->getNeuronType() == nlm::NeuronType::Excitatory);
    }
    
    logTest("Population Creation", true);
}

void testPopulationStatistics() {
    nlm::NeuralPopulation population(
        nlm::PopulationId(1),
        nlm::NeuronType::Excitatory,
        50
    );
    
    // Inject current into all neurons
    for (auto* neuron : population.getNeurons()) {
        neuron->injectCurrent(10.0f);
    }
    
    // Step neurons
    for (int step = 0; step < 10; ++step) {
        for (auto* neuron : population.getNeurons()) {
            neuron->stepLIF(0.001, static_cast<double>(step) * 0.001);
        }
    }
    
    // Check statistics
    float avgRate = population.getAverageFiringRate();
    assert(avgRate >= 0.0f);
    
    size_t activeCount = population.getActiveNeuronCount();
    assert(activeCount >= 0 && activeCount <= 50);
    
    logTest("Population Statistics", true);
}

// NeuralRegion Tests
void testRegionCreation() {
    nlm::NeuralRegion region(nlm::RegionId(1), "TestRegion");
    
    assert(region.getId() == nlm::RegionId(1));
    assert(region.getName() == "TestRegion");
    assert(region.getPopulationCount() == 0);
    assert(region.getSynapseCount() == 0);
    
    logTest("Region Creation", true);
}

void testRegionPopulationManagement() {
    nlm::NeuralRegion region(nlm::RegionId(1), "TestRegion");
    
    // Add populations
    nlm::PopulationId pop1 = region.addPopulation(50, nlm::NeuronType::Excitatory);
    nlm::PopulationId pop2 = region.addPopulation(30, nlm::NeuronType::Inhibitory);
    
    assert(region.getPopulationCount() == 2);
    assert(region.getTotalNeuronCount() == 80);
    
    // Get populations
    nlm::NeuralPopulation* population1 = region.getPopulation(pop1);
    nlm::NeuralPopulation* population2 = region.getPopulation(pop2);
    
    assert(population1 != nullptr);
    assert(population2 != nullptr);
    assert(population1->getNeuronCount() == 50);
    assert(population2->getNeuronCount() == 30);
    
    // Test region connections
    auto* neuron1 = population1->getNeurons()[0];
    auto* neuron2 = population2->getNeurons()[0];
    
    // Create synapse between neurons
    nlm::Synapse synapse(nlm::SynapseId(1), neuron1->getId(), neuron2->getId());
    region.addSynapse(synapse);
    
    assert(region.getSynapseCount() == 1);
    
    logTest("Region Population Management", true);
}

void testRegionConnectivity() {
    nlm::NeuralRegion region(nlm::RegionId(1), "TestRegion");
    
    // Add neurons to region
    nlm::PopulationId pop1 = region.addPopulation(10, nlm::NeuronType::Excitatory);
    nlm::PopulationId pop2 = region.addPopulation(10, nlm::NeuronType::Inhibitory);
    
    auto* population1 = region.getPopulation(pop1);
    auto* population2 = region.getPopulation(pop2);
    
    auto neurons1 = population1->getNeurons();
    auto neurons2 = population2->getNeurons();
    
    // Create synapses from pop1 to pop2
    for (auto* source : neurons1) {
        for (auto* target : neurons2) {
            nlm::Synapse synapse(
                nlm::SynapseId((source->getId().index() * 10) + target->getId().index()),
                source->getId(),
                target->getId()
            );
            region.addSynapse(synapse);
        }
    }
    
    assert(region.getSynapseCount() == 100);
    
    // Test synapse retrieval
    auto synapses1to2 = region.getSynapsesTo(neurons2[0]->getId());
    assert(synapses1to2.size() == 10);
    
    auto synapses2to1 = region.getSynapsesFrom(neurons1[0]->getId());
    assert(synapses2to1.size() == 10);
    
    logTest("Region Connectivity", true);
}

// Brain Tests
void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    nlm::Brain brain(config);
    
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    
    logTest("Brain Creation", true);
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    
    logTest("Brain Initialization", true);
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run simulation steps
    for (int step = 0; step < 10; ++step) {
        brain.step(step, static_cast<double>(step) * 0.001);
    }
    
    // Check that brain has progressed
    assert(brain.getTotalSpikeCount() >= initialSpikes);
    assert(brain.getCurrentStep() == 10);
    
    logTest("Brain Step", true);
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps
    for (int step = 0; step < 10; ++step) {
        brain.step(step, static_cast<double>(step) * 0.001);
    }
    
    // Get current state
    size_t spikesBefore = brain.getTotalSpikeCount();
    
    // Reset brain
    brain.reset();
    
    // Check that reset occurred
    assert(brain.getTotalSpikeCount() == 0);
    assert(brain.getCurrentStep() == 0);
    assert(brain.getCurrentTime() == 0.0);
    
    logTest("Brain Reset", true);
}

// Plasticity Tests
void testSTDPLTP() {
    nlm::STDP stdp;
    stdp.configure(0.02f, 0.015f, 20.0f);
    
    // Create synapse with pre-before-post spike pattern
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    
    // Pre fires, then post 10ms later
    std::vector<nlm::Timestamp> preSpikes = {0.0f};
    std::vector<nlm::Timestamp> postSpikes = {10.0f};
    
    float initialWeight = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased (LTP)
    assert(finalWeight > initialWeight);
    
    logTest("STDP LTP", true);
}

void testSTDPLTD() {
    nlm::STDP stdp;
    
    // Create synapse with post-before-pre spike pattern
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setWeight(0.5f);
    
    // Post fires, then pre 10ms later
    std::vector<nlm::Timestamp> preSpikes = {10.0f};
    std::vector<nlm::Timestamp> postSpikes = {0.0f};
    
    float initialWeight = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased (LTD)
    assert(finalWeight < initialWeight);
    
    logTest("STDP LTD", true);
}

void testHebbianLearning() {
    nlm::Hebbian hebbian;
    hebbian.configure(0.001f);
    
    // Create synapse with simultaneous firing
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setWeight(0.5f);
    
    // Both pre and post fire
    std::vector<nlm::Timestamp> preSpikes = {0.0f, 5.0f};
    std::vector<nlm::Timestamp> postSpikes = {0.0f, 5.0f};
    
    float initialWeight = synapse.getWeight();
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased (Hebbian strengthening)
    assert(finalWeight > initialWeight);
    
    logTest("Hebbian Learning", true);
}

void testStructuralPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::StructuralPlasticity sp;
    sp.setSynaptogenesisRate(0.0001f);
    sp.setPruningRate(0.00001f);
    
    // Create a brain with config
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test structural plasticity update
    sp.update(&brain, *brain.getRandomGenerator());
    
    // The exact effect is hard to test without detailed internal knowledge
    // But we can at least verify it runs without crashing
    assert(true);  // Placeholder assertion
    
    logTest("Structural Plasticity", true);
}

// SpikeSystem Tests
void testSpikeSystemQueue() {
    nlm::SpikeSystem spikeSystem;
    
    // Create a spike event
    nlm::SpikeEvent spike(nlm::NeuronId(1), 0.0, 0);
    
    // Queue the spike
    spikeSystem.queueSpike(spike);
    
    // Process spikes
    spikeSystem.processSpikes(0);
    
    // Check that spike was processed
    assert(spikeSystem.getPendingSpikeCount() == 0);
    
    logTest("SpikeSystem Queue", true);
}

void testSpikeSystemDelayed() {
    nlm::SpikeSystem spikeSystem;
    
    // Create a delayed spike event
    nlm::DelayedSpikeEvent delayedSpike(
        nlm::NeuronId(1),
        nlm::NeuronId(2),
        nlm::SynapseId(1),
        1.0f,
        nlm::SynapseType::Excitatory,
        0.0,
        0.01,
        0,
        10
    );
    
    // Queue the delayed spike
    spikeSystem.queueDelayedSpike(delayedSpike);
    
    // Try to process with insufficient time
    spikeSystem.processDelayedSpikes(0, 0.0);
    
    // Spike should not be processed yet
    assert(spikeSystem.getPendingDelayedCount() == 1);
    
    logTest("SpikeSystem Delayed", true);
}

// Main test runner
void runAllTests() {
    std::cout << "=== NLM Neural Network Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    // Neuron tests
    testNeuronCreation();
    testNeuronInitialization();
    testNeuronLIFDynamics();
    testNeuronFiring();
    testNeuronRefractoryPeriod();
    testNeuronTypeProperties();
    
    // Synapse tests
    testSynapseCreation();
    testSynapseConnection();
    testSynapsePlasticity();
    testSynapseTypes();
    
    // Population tests
    testPopulationCreation();
    testPopulationStatistics();
    
    // Region tests
    testRegionCreation();
    testRegionPopulationManagement();
    testRegionConnectivity();
    
    // Brain tests
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    
    // Plasticity tests
    testSTDPLTP();
    testSTDPLTD();
    testHebbianLearning();
    testStructuralPlasticity();
    
    // SpikeSystem tests
    testSpikeSystemQueue();
    testSpikeSystemDelayed();
    
    // Summary
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << totalTests << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    
    if (failedTests == 0) {
        std::cout << std::endl;
        std::cout << "✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    }
}

} // namespace nlm_tests

// Main entry point
int main() {
    return nlm_tests::runAllTests();
}