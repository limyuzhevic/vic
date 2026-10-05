// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neuron {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getMembranePotential() == -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getTotalCurrent() == 0.0f);
    
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    assert(neuron.getTotalCurrent() == -3.0f);
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    assert(neuron.getSpikeHistory().size() == 3);
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

void testBiologicallyBasedInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test initialization for different neuron types
    neuron.initializeBiologically(&neuron, nlm::NeuronType::Excitatory, rng);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    assert(neuron.getRestingPotential() < -50.0f);  // Excitatory should be higher than -70
    
    nlm::Neuron neuron2(nlm::NeuronId(2));
    neuron2.initializeBiologically(&neuron2, nlm::NeuronType::Inhibitory, rng);
    assert(neuron2.getType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testBiologicallyBasedInitialization passed" << std::endl;
}

void testDevelopmentStageInitialization() {
    nlm::RandomGenerator rng(43);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test initialization for different development stages
    neuron.initializeByDevelopmentStage(&neuron, nlm::DevelopmentalStage::Adult, rng);
    
    neuron.initializeByDevelopmentStage(&neuron, nlm::DevelopmentalStage::CriticalPeriod, rng);
    
    neuron.initializeByDevelopmentStage(&neuron, nlm::DevelopmentalStage::Maturation, rng);
    
    std::cout << "    testDevelopmentStageInitialization passed" << std::endl;
}

void testPopulationBasedInitialization() {
    nlm::RandomGenerator rng(44);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up population statistics
    nlm::PopulationStats stats;
    stats.avgRestingPotential = -70.0f;
    stats.stdDevRestingPotential = 2.0f;
    stats.avgThreshold = -55.0f;
    stats.stdDevThreshold = 2.0f;
    stats.avgResetPotential = -70.0f;
    stats.stdDevResetPotential = 2.0f;
    stats.firingRate = 10.0f;
    stats.energyConsumption = 1.0f;
    stats.synapticDensity = 1.0f;
    
    nlm::Neuron::setPopulationStats(nlm::PopulationId(1), stats);
    
    neuron.initializeForPopulation(&neuron, nlm::PopulationId(1), rng);
    assert(neuron.getPopulationId() == nlm::PopulationId(1));
    
    std::cout << "    testPopulationBasedInitialization passed" << std::endl;
}

void testSynapticContextInitialization() {
    nlm::RandomGenerator rng(45);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test with different connection counts
    neuron.initializeWithSynapticContext(&neuron, 0, rng);
    
    neuron.initializeWithSynapticContext(&neuron, 50, rng);
    
    neuron.initializeWithSynapticContext(&neuron, 150, rng);
    
    std::cout << "    testSynapticContextInitialization passed" << std::endl;
}

void testParameterValidation() {
    nlm::RandomGenerator rng(46);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // First initialize with valid parameters
    neuron.initializeRandom(rng);
    
    // Validate should pass
    assert(neuron.validateInitializedParameters(&neuron));
    
    // Manually set invalid parameters and test
    neuron.setRestingPotential(200.0f);  // Invalid value
    assert(!neuron.validateInitializedParameters(&neuron));
    
    // Reset to valid
    neuron.setRestingPotential(-70.0f);
    assert(neuron.validateInitializedParameters(&neuron));
    
    std::cout << "    testParameterValidation passed" << std::endl;
}

void testEnergyBudgetInitialization() {
    nlm::RandomGenerator rng(47);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test with different energy budgets
    neuron.initializeWithEnergyBudget(&neuron, 5.0f, rng);
    
    neuron.initializeWithEnergyBudget(&neuron, 15.0f, rng);
    
    neuron.initializeWithEnergyBudget(&neuron, 25.0f, rng);
    
    std::cout << "    testEnergyBudgetInitialization passed" << std::endl;
}

void testTemplateBasedInitialization() {
    nlm::RandomGenerator rng(48);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Create a template
    nlm::NeuronTemplate template;
    template.type = nlm::NeuronType::Excitatory;
    template.restingPotential = -68.0f;
    template.threshold = -53.0f;
    template.resetPotential = -68.0f;
    template.leakConductance = 15.0f;
    template.refractoryPeriod = 8;
    template.hasPlasticity = true;
    
    neuron.initializeFromTemplate(&neuron, template, rng);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    assert(neuron.getRestingPotential() > -70.0f && neuron.getRestingPotential() < -60.0f);
    
    std::cout << "    testTemplateBasedInitialization passed" << std::endl;
}

void testBatchAdvancedInitialization() {
    std::vector<nlm::Neuron*> neurons;
    nlm::RandomGenerator rng(49);
    
    // Create neurons
    for (int i = 0; i < 5; ++i) {
        neurons.push_back(new nlm::Neuron(nlm::NeuronId(i)));
    }
    
    // Test with different strategies
    nlm::Neuron::batchInitializeAdvanced(neurons, nlm::InitializationStrategy::TypeSpecific, rng);
    
    // Cleanup
    for (nlm::Neuron* neuron : neurons) {
        delete neuron;
    }
    
    // Create neurons again
    neurons.clear();
    for (int i = 0; i < 5; ++i) {
        neurons.push_back(new nlm::Neuron(nlm::NeuronId(i)));
    }
    
    nlm::Neuron::batchInitializeAdvanced(neurons, nlm::InitializationStrategy::EnergyBudget, rng);
    
    // Cleanup
    for (nlm::Neuron* neuron : neurons) {
        delete neuron;
    }
    
    std::cout << "    testBatchAdvancedInitialization passed" << std::endl;
}

void runAll() {
    testNeuronCreation();
    testNeuronType();
    testMembranePotential();
    testFiringState();
    testRefractoryDecrement();
    testCurrentInjection();
    testSynapticInput();
    testSpikeHistory();
    testRandomInitialization();
    testReset();
    testBatchLIFStep();
    testBatchInjectCurrent();
    testBatchReset();
    testBatchInitializeRandom();
    testBatchMetrics();
    
    // Test new advanced initialization methods
    testBiologicallyBasedInitialization();
    testDevelopmentStageInitialization();
    testPopulationBasedInitialization();
    testSynapticContextInitialization();
    testParameterValidation();
    testEnergyBudgetInitialization();
    testTemplateBasedInitialization();
    testBatchAdvancedInitialization();
    testPopulationStatsManagement();
}

} // namespace test_neuron

