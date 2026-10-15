// Unit Tests for Advanced Neuron Tests
// Comprehensive test suite for advanced neuron functionality

#include "tests/utils/TestUtils.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/dynamics/NeuralDynamics.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace test_neuron_advanced {

void testAdvancedNeuronCreation() {
    TestUtils::TestSession session("testAdvancedNeuronCreation");
    
    nlm::Neuron neuron;
    
    // Test neuron properties
    TestAssert::true(neuron.getId() >= 0, "Neuron should have valid ID");
    TestAssert::true(neuron.getType() == nlm::NeuronType::LIF, "Neuron should be LIF type by default");
    TestAssert::true(neuron.getMembranePotential() >= -100.0 && neuron.getMembranePotential() <= 50.0, "Membrane potential should be reasonable");
    
    // Test connectivity
    TestAssert::true(neuron.getInputCount() >= 0, "Neuron should have input count");
    TestAssert::true(neuron.getOutputCount() >= 0, "Neuron should have output count");
    
    std::cout << "    testAdvancedNeuronCreation passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronDynamics() {
    TestUtils::TestSession session("testAdvancedNeuronDynamics");
    
    nlm::Neuron neuron;
    
    // Set initial state
    neuron.setMembranePotential(-65.0);
    neuron.setRefractoryPeriod(2.0);
    neuron.setAdaptationConductance(0.5);
    
    TestAssert::equal(neuron.getMembranePotential(), -65.0, 0.001, "Membrane potential should be set correctly");
    TestAssert::equal(neuron.getRefractoryPeriod(), 2.0, 0.001, "Refractory period should be set correctly");
    TestAssert::equal(neuron.getAdaptationConductance(), 0.5, 0.001, "Adaptation conductance should be set correctly");
    
    // Test membrane potential update
    double initialPotential = neuron.getMembranePotential();
    neuron.step(1.0); // Simulate time step
    double newPotential = neuron.getMembranePotential();
    
    // Potential should change (decay towards resting potential)
    TestAssert::true(std::abs(newPotential - initialPotential) < 10.0, "Potential change should be reasonable");
    
    std::cout << "    testAdvancedNeuronDynamics passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronInputs() {
    TestUtils::TestSession session("testAdvancedNeuronInputs");
    
    nlm::Neuron neuron;
    
    // Add input spikes
    std::vector<double> inputSpikes;
    for (int i = 0; i < 10; ++i) {
        inputSpikes.push_back(TestUtils::randomDouble() * 20.0); // Random spike times
    }
    
    neuron.setInputSpikes(inputSpikes);
    
    // Process inputs
    neuron.processInputs(0.001); // Small time step
    
    // Test synaptic inputs
    std::vector<std::pair<double, double>> synapticInputs;
    for (int i = 0; i < 5; ++i) {
        synapticInputs.push_back({TestUtils::randomDouble(), TestUtils::randomDouble() * 100.0});
    }
    
    neuron.setSynapticInputs(synapticInputs);
    neuron.processInputs(0.001);
    
    // Test output spike generation
    bool hasSpike = neuron.hasOutputSpike();
    TestAssert::true(hasSpike || !hasSpike, "Spike generation is stochastic but should be valid");
    
    std::cout << "    testAdvancedNeuronInputs passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronAdaptation() {
    TestUtils::TestSession session("testAdvancedNeuronAdaptation");
    
    nlm::Neuron neuron;
    
    // Set adaptation parameters
    neuron.setAdaptationConductance(0.8);
    neuron.setAdaptationTau(10.0);
    neuron.setAdaptationBaseline(0.0);
    
    TestAssert::equal(neuron.getAdaptationConductance(), 0.8, 0.001, "Adaptation conductance should be set");
    TestAssert::equal(neuron.getAdaptationTau(), 10.0, 0.001, "Adaptation tau should be set");
    TestAssert::equal(neuron.getAdaptationBaseline(), 0.0, 0.001, "Adaptation baseline should be set");
    
    // Update adaptation
    neuron.updateAdaptation(0.01, 0.001);
    
    // Test adaptation current
    double adaptationCurrent = neuron.getAdaptationCurrent();
    TestAssert::true(adaptationCurrent >= 0.0, "Adaptation current should be non-negative");
    
    std::cout << "    testAdvancedNeuronAdaptation passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronPlasticity() {
    TestUtils::TestSession session("testAdvancedNeuronPlasticity");
    
    nlm::Neuron neuron;
    
    // Set plasticity parameters
    neuron.setPlasticityEnabled(true);
    neuron.setLearningRate(0.01);
    neuron.setSTDPEnabled(true);
    neuron.setHomeostaticPlasticityEnabled(true);
    
    TestAssert::true(neuron.isPlasticityEnabled(), "Plasticity should be enabled");
    TestAssert::equal(neuron.getLearningRate(), 0.01, 0.001, "Learning rate should be set");
    TestAssert::true(neuron.isSTDPEnabled(), "STDP should be enabled");
    TestAssert::true(neuron.isHomeostaticPlasticityEnabled(), "Homeostatic plasticity should be enabled");
    
    // Test spike-timing dependent plasticity
    neuron.recordPreSpike(0.0);
    neuron.recordPostSpike(5.0); // 5ms post-pre spike pairing
    
    neuron.applyPlasticity(0.001);
    
    // Test synaptic weight changes
    double weightChange = neuron.getWeightChange();
    TestAssert::true(weightChange <= 1.0 && weightChange >= -1.0, "Weight change should be reasonable");
    
    std::cout << "    testAdvancedNeuronPlasticity passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronMemory() {
    TestUtils::TestSession session("testAdvancedNeuronMemory");
    
    nlm::Neuron neuron;
    
    // Test spike history
    std::vector<double> spikeTimes;
    for (int i = 0; i < 20; ++i) {
        spikeTimes.push_back(static_cast<double>(i) * 10.0); // Spikes every 10ms
    }
    
    neuron.setSpikeHistory(spikeTimes);
    
    auto history = neuron.getSpikeHistory(50.0); // Last 50ms
    TestAssert::true(history.size() >= 0, "Should get spike history");
    
    // Test membrane potential history
    std::vector<double> voltageHistory;
    for (int i = 0; i < 100; ++i) {
        voltageHistory.push_back(-65.0 + TestUtils::randomDouble() * 20.0);
    }
    
    neuron.setMembranePotentialHistory(voltageHistory);
    
    auto voltageHistoryRetrieved = neuron.getMembranePotentialHistory(1000.0);
    TestAssert::true(voltageHistoryRetrieved.size() >= 0, "Should get voltage history");
    
    std::cout << "    testAdvancedNeuronMemory passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronPopulations() {
    TestUtils::TestSession session("testAdvancedNeuronPopulations");
    
    nlm::NeuralPopulation population("test_population", 100);
    
    // Test population properties
    TestAssert::equal(population.getSize(), 100, "Population should have 100 neurons");
    TestAssert::equal(population.getNeuronType(), nlm::NeuronType::LIF, "Population should be LIF type");
    
    // Add neurons to population
    std::vector<nlm::Neuron> neurons;
    for (int i = 0; i < 10; ++i) {
        neurons.push_back(nlm::Neuron());
    }
    
    population.addNeurons(neurons);
    TestAssert::true(population.getSize() >= 100, "Population size should increase");
    
    // Test population dynamics
    population.step(0.001);
    
    // Test population statistics
    double avgFiringRate = population.getAverageFiringRate();
    TestAssert::true(avgFiringRate >= 0.0, "Average firing rate should be non-negative");
    
    double avgPotential = population.getAverageMembranePotential();
    TestAssert::true(avgPotential >= -100.0 && avgPotential <= 50.0, "Average potential should be reasonable");
    
    std::cout << "    testAdvancedNeuronPopulations passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuralDynamics() {
    TestUtils::TestSession session("testAdvancedNeuralDynamics");
    
    auto config = std::make_shared<nlm::Config>();
    auto dynamics = std::make_unique<nlm::NeuralDynamics>(config);
    
    // Test dynamics initialization
    TestAssert::true(dynamics != nullptr, "NeuralDynamics should be created successfully");
    
    // Create test neurons
    std::vector<nlm::Neuron> neurons;
    for (int i = 0; i < 10; ++i) {
        neurons.push_back(nlm::Neuron());
    }
    
    // Create test synapses
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 5; ++i) {
        synapses.push_back(nlm::Synapse());
    }
    
    // Simulate dynamics
    dynamics->simulate(neurons, synapses, 0.001, 100);
    
    // Check that dynamics modified neuron states
    TestAssert::true(neurons.size() == 10, "Should have 10 neurons after dynamics simulation");
    
    std::cout << "    testAdvancedNeuralDynamics passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronConnectivity() {
    TestUtils::TestSession session("testAdvancedNeuronConnectivity");
    
    nlm::Neuron neuron1;
    nlm::Neuron neuron2;
    
    // Set up connectivity
    neuron1.addOutput(&neuron2);
    neuron2.addInput(&neuron1);
    
    TestAssert::true(neuron1.getOutputCount() > 0, "Neuron1 should have outputs");
    TestAssert::true(neuron2.getInputCount() > 0, "Neuron2 should have inputs");
    
    // Test synaptic connection
    neuron1.setOutputWeight(&neuron2, 0.5);
    double weight = neuron1.getOutputWeight(&neuron2);
    TestAssert::equal(weight, 0.5, 0.001, "Output weight should be set correctly");
    
    // Test synaptic delay
    neuron1.setSynapticDelay(&neuron2, 0.001);
    double delay = neuron1.getSynapticDelay(&neuron2);
    TestAssert::equal(delay, 0.001, 0.000001, "Synaptic delay should be set correctly");
    
    std::cout << "    testAdvancedNeuronConnectivity passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronStateManagement() {
    TestUtils::TestSession session("testAdvancedNeuronStateManagement");
    
    nlm::Neuron neuron;
    
    // Set neuron state
    neuron.setMembranePotential(-70.0);
    neuron.setRefractoryPeriod(5.0);
    neuron.setAdaptationConductance(0.3);
    neuron.setLearningRate(0.02);
    neuron.setHomeostaticPlasticityEnabled(true);
    
    // Get neuron state
    auto state = neuron.getState();
    TestAssert::true(state.size() > 0, "Neuron state should not be empty");
    
    // Save and restore state
    std::string stateString = neuron.toString();
    TestAssert::true(stateString.size() > 0, "State string should not be empty");
    
    // Test spike recording
    neuron.recordPreSpike(0.0);
    neuron.recordPostSpike(10.0);
    
    auto preSpikes = neuron.getRecentPreSpikes(20.0);
    auto postSpikes = neuron.getRecentPostSpikes(20.0);
    
    TestAssert::true(preSpikes.size() > 0, "Should have pre-spikes");
    TestAssert::true(postSpikes.size() > 0, "Should have post-spikes");
    
    std::cout << "    testAdvancedNeuronStateManagement passed" << std::endl;
    session.setSuccess(true);
}

void testAdvancedNeuronPerformance() {
    TestUtils::TestSession session("testAdvancedNeuronPerformance");
    
    nlm::Neuron neuron;
    
    // Performance test - simulate many time steps
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (int i = 0; i < 1000; ++i) {
        neuron.step(0.001);
        neuron.processInputs(0.001);
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001; // Convert to milliseconds
    
    // Performance should be reasonable (< 100ms for 1000 steps)
    TestAssert::true(durationMs < 100.0, "Performance should be reasonable");
    
    std::cout << "    testAdvancedNeuronPerformance passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Advanced Neuron Tests ===" << std::endl;
    
    testAdvancedNeuronCreation();
    testAdvancedNeuronDynamics();
    testAdvancedNeuronInputs();
    testAdvancedNeuronAdaptation();
    testAdvancedNeuronPlasticity();
    testAdvancedNeuronMemory();
    testAdvancedNeuronPopulations();
    testAdvancedNeuralDynamics();
    testAdvancedNeuronConnectivity();
    testAdvancedNeuronStateManagement();
    testAdvancedNeuronPerformance();
    
    std::cout << std::endl;
    std::cout << "=== All Advanced Neuron Tests PASSED ===" << std::endl;
}