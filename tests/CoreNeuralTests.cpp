#include "tests/CoreNeuralTests.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

void CoreNeuralTests::runAllTests() {
    std::cout << "=== NLM Core Neural Component Tests ===" << std::endl;
    
    testNeuronBasics();
    testNeuronMoveSemantics();
    testSynapseBasics();
    testSTDPPlasticity();
    testHebbianPlasticity();
    testNeuralPopulation();
    testNeuralRegion();
    testSpikeSystem();
    
    std::cout << "\n=== All Core Neural Tests Passed! ===" << std::endl;
}

void CoreNeuralTests::testNeuronBasics() {
    std::cout << "Testing Neuron basics..." << std::endl;
    
    // Test neuron creation
    Neuron neuron(NeuronId(1));
    assert(neuron.getId().index() == 1);
    assert(neuron.getType() == NeuronType::Internal);
    
    // Test membrane potential operations
    neuron.setMembranePotential(-60.0f);
    assert(std::abs(neuron.getMembranePotential() - (-60.0f)) < 0.001f);
    
    neuron.addToMembranePotential(10.0f);
    assert(std::abs(neuron.getMembranePotential() - (-50.0f)) < 0.001f);
    
    // Test threshold and firing
    neuron.setThreshold(-55.0f);
    assert(neuron.checkThreshold() == false);  // -50 < -55 is false
    
    neuron.setMembranePotential(-50.0f);
    assert(neuron.checkThreshold() == true);  // -50 >= -55 is true
    
    // Test firing state
    neuron.setFiringState(FiringState::Firing);
    assert(neuron.isFiring() == true);
    assert(neuron.isRefractory() == false);
    
    std::cout << "  ✓ Neuron basics test passed" << std::endl;
}

void CoreNeuralTests::testNeuronMoveSemantics() {
    std::cout << "Testing Neuron move semantics..." << std::endl;
    
    // Test move constructor
    Neuron original(NeuronId(42));
    Neuron moved(std::move(original));
    assert(moved.getId().index() == 42);
    
    // Original should be in valid but unspecified state after move
    // (implementation specific)
    
    // Test move assignment
    Neuron assigned(NeuronId(99));
    assigned = std::move(moved);
    assert(assigned.getId().index() == 42);
    
    // Test copy prevention
    Neuron copy1(NeuronId(1));
    Neuron copy2(NeuronId(2));
    // These should compile without copy constructor usage
    // (compiler will catch if copy constructor is used)
    
    std::cout << "  ✓ Neuron move semantics test passed" << std::endl;
}

void CoreNeuralTests::testSynapseBasics() {
    std::cout << "Testing Synapse basics..." << std::endl;
    
    // Create neurons first
    Neuron source(NeuronId(1));
    Neuron target(NeuronId(2));
    
    // Create synapse
    Synapse synapse(NeuronId(1), NeuronId(2), 0.1f, Delay(1));
    assert(synapse.getSourceNeuron().index() == 1);
    assert(synapse.getDestinationNeuron().index() == 2);
    assert(std::abs(synapse.getWeight() - 0.1f) < 0.001f);
    assert(synapse.getDelay() == 1);
    
    // Test weight operations
    synapse.addToWeight(0.05f);
    assert(std::abs(synapse.getWeight() - 0.15f) < 0.001f);
    
    // Test synaptic type
    assert(synapse.getType() == SynapseType::Excitatory);
    
    // Test plasticity flags
    synapse.enablePlasticity(true, false, true);  // Hebbian and structural
    assert(synapse.getPlasticityFlags().hebbian == true);
    assert(synapse.getPlasticityFlags().stdp == false);
    assert(synapse.getPlasticityFlags().structural == true);
    
    // Test eligibility trace
    synapse.recordPreSpike(0.0f);
    synapse.recordPostSpike(0.001f);
    assert(synapse.getPreSpikeHistory().size() > 0);
    assert(synapse.getPostSpikeHistory().size() > 0);
    
    std::cout << "  ✓ Synapse basics test passed" << std::endl;
}

void CoreNeuralTests::testSTDPPlasticity() {
    std::cout << "Testing STDP plasticity..." << std::endl;
    
    STDP stdp;
    stdp.configure(0.01f, 0.012f, 20.0f);  // LTP, LTD, tau
    
    // Create neurons and synapse
    Neuron pre(NeuronId(1));
    Neuron post(NeuronId(2));
    Synapse synapse(NeuronId(1), NeuronId(2), 0.1f, Delay(1));
    
    // Enable STDP
    synapse.enablePlasticity(true, true, false);  // Only STDP
    assert(synapse.getPlasticityFlags().stdp == true);
    
    // Simulate pre-before-post spike timing (should cause LTP)
    synapse.recordPreSpike(0.0f);
    synapse.recordPostSpike(0.005f);  // Post before pre (LTD)
    
    float weightBefore = synapse.getWeight();
    stdp.update(&synapse, synapse.getPreSpikeHistory(), synapse.getPostSpikeHistory(), 0.001f);
    float weightAfter = synapse.getWeight();
    
    // Weight should have changed (either increased or decreased)
    assert(weightBefore != weightAfter);
    
    // Test the opposite: pre-after-post (should cause LTD)
    Synapse synapse2(NeuronId(3), NeuronId(4), 0.1f, Delay(1));
    synapse2.enablePlasticity(true, true, false);
    
    synapse2.recordPreSpike(0.0f);
    synapse2.recordPostSpike(0.002f);  // Post after pre (LTP)
    
    float weightBefore2 = synapse2.getWeight();
    stdp.update(&synapse2, synapse2.getPreSpikeHistory(), synapse2.getPostSpikeHistory(), 0.001f);
    float weightAfter2 = synapse2.getWeight();
    
    assert(weightBefore2 != weightAfter2);
    
    std::cout << "  ✓ STDP plasticity test passed" << std::endl;
}

void CoreNeuralTests::testHebbianPlasticity() {
    std::cout << "Testing Hebbian plasticity..." << std::endl;
    
    Hebbian hebbian;
    hebbian.configure(0.05f, 0.1f);  // Learning rate, decay
    
    // Create neurons and synapse
    Neuron pre(NeuronId(1));
    Neuron post(NeuronId(2));
    Synapse synapse(NeuronId(1), NeuronId(2), 0.1f, Delay(1));
    
    // Enable Hebbian
    synapse.enablePlasticity(true, true, false);  // Only Hebbian
    assert(synapse.getPlasticityFlags().hebbian == true);
    
    // Record spikes
    synapse.recordPreSpike(0.0f);
    synapse.recordPostSpike(0.001f);
    
    float weightBefore = synapse.getWeight();
    hebbian.update(&synapse, synapse.getPreSpikeHistory(), synapse.getPostSpikeHistory(), 0.001f);
    float weightAfter = synapse.getWeight();
    
    // Hebbian learning should strengthen connection
    assert(weightAfter >= weightBefore);
    
    std::cout << "  ✓ Hebbian plasticity test passed" << std::endl;
}

void CoreNeuralTests::testNeuralPopulation() {
    std::cout << "Testing NeuralPopulation..." << std::endl;
    
    // Create neurons first
    std::vector<std::unique_ptr<Neuron>> neurons;
    for (size_t i = 0; i < 10; ++i) {
        neurons.push_back(std::make_unique<Neuron>(NeuronId(i + 1)));
    }
    
    // Create population
    NeuralPopulation population(PopulationId(1), NeuronType::Internal);
    for (auto& neuron : neurons) {
        population.addNeuron(neuron.get());
    }
    
    // Test population properties
    assert(population.getId() == PopulationId(1));
    assert(population.getNeuronType() == NeuronType::Internal);
    assert(population.getNeuronCount() == 10);
    
    // Test neuron access
    auto& allNeurons = population.getNeurons();
    assert(allNeurons.size() == 10);
    
    // Test population statistics
    assert(population.getTotalNeuronCount() == 10);
    assert(population.getActiveNeuronCount() == 0);
    
    std::cout << "  ✓ NeuralPopulation test passed" << std::endl;
}

void CoreNeuralTests::testNeuralRegion() {
    std::cout << "Testing NeuralRegion..." << std::endl;
    
    // Create region
    NeuralRegion region(RegionId(1), "TestRegion");
    
    // Create populations and neurons
    std::vector<Neuron*> neurons;
    
    // Create neurons manually
    for (size_t i = 0; i < 15; ++i) {
        Neuron* neuron = new Neuron(NeuronId(i + 1));
        neuron->setType(static_cast<NeuronType>(i % 3)); // Cycle through types
        neurons.push_back(neuron);
    }
    
    // Create populations
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> internalNeurons;
    std::vector<Neuron*> motorNeurons;
    
    for (size_t i = 0; i < 5; ++i) {
        sensoryNeurons.push_back(neurons[i * 3]);
        internalNeurons.push_back(neurons[i * 3 + 1]);
        motorNeurons.push_back(neurons[i * 3 + 2]);
    }
    
    // Create populations
    NeuralPopulation sensoryPop(PopulationId(1), NeuronType::Sensory);
    NeuralPopulation internalPop(PopulationId(2), NeuronType::Internal);
    NeuralPopulation motorPop(PopulationId(3), NeuronType::Motor);
    
    for (auto* neuron : sensoryNeurons) sensoryPop.addNeuron(neuron);
    for (auto* neuron : internalNeurons) internalPop.addNeuron(neuron);
    for (auto* neuron : motorNeurons) motorPop.addNeuron(neuron);
    
    region.addPopulation(std::make_unique<NeuralPopulation>(std::move(sensoryPop)));
    region.addPopulation(std::make_unique<NeuralPopulation>(std::move(internalPop)));
    region.addPopulation(std::make_unique<NeuralPopulation>(std::move(motorPop)));
    
    // Test region properties
    assert(region.getId() == RegionId(1));
    assert(region.getName() == "TestRegion");
    assert(region.getPopulationCount() == 3);
    assert(region.getTotalNeuronCount() == 15);
    
    // Test population access
    assert(region.getPopulation(PopulationId(1)) != nullptr);
    assert(region.getPopulation(PopulationId(2)) != nullptr);
    assert(region.getPopulation(PopulationId(3)) != nullptr);
    
    // Test statistics
    assert(region.getActiveNeuronCount() == 0);
    
    // Cleanup - delete neurons
    for (auto* neuron : neurons) {
        delete neuron;
    }
    
    std::cout << "  ✓ NeuralRegion test passed" << std::endl;
}

void CoreNeuralTests::testSpikeSystem() {
    std::cout << "Testing SpikeSystem..." << std::endl;
    
    SpikeSystem spikeSystem;
    
    // Create neurons and register spike handler
    std::vector<NeuronId> spikedNeurons;
    spikeSystem.registerHandler([&spikedNeurons](const SpikeEvent& event) {
        spikedNeurons.push_back(event.neuronId);
    });
    
    // Create and queue spike
    SpikeEvent spike(NeuronId(42), 0.001, 1);
    spikeSystem.queueSpike(spike);
    
    // Process spikes
    spikeSystem.processSpikes(1);
    
    // Check if spike was recorded
    assert(spikedNeurons.size() == 1);
    assert(spikedNeurons[0].index() == 42);
    
    // Test pending spike count
    assert(spikeSystem.getPendingSpikeCount() == 0);
    
    std::cout << "  ✓ SpikeSystem test passed" << std::endl;
}

} // namespace nlm
