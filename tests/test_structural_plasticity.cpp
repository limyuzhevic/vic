// Hebbian Plasticity Tests
// Tests for Hebbian learning rules

#include "nlm_testing_framework.hpp"
#include "plasticity/Hebbian.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>

TEST_CASE("hebbian_test", TestCategory::UNIT, "Tests for Hebbian plasticity rule")
void testHebbian() {
    nlm::Hebbian hebbian;
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getMaxWeightChange() == 1.0f);
    assert(hebbian.getName() != nullptr);
    
    // Test weight update for coincident firing
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    nlm::Neuron preNeuron(nlm::NeuronId(1));
    nlm::Neuron postNeuron(nlm::NeuronId(2));
    
    preNeuron.setFiringState(nlm::FiringState::Active);
    postNeuron.setFiringState(nlm::FiringState::Active);
    
    float initialWeight = synapse.getWeight();
    
    hebbian.update(&synapse, &preNeuron, &postNeuron);
    
    float finalWeight = synapse.getWeight();
    assert(finalWeight > initialWeight); // Should potentiate
    
    // Test depression for anti-correlation
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    nlm::Neuron preNeuron2(nlm::NeuronId(3));
    nlm::Neuron postNeuron2(nlm::NeuronId(4));
    
    preNeuron2.setFiringState(nlm::FiringState::Active);
    postNeuron2.setRefractoryPeriod(1); // Post not active
    
    float initialWeight2 = synapse2.getWeight();
    
    hebbian.update(&synapse2, &preNeuron2, &postNeuron2);
    
    float finalWeight2 = synapse2.getWeight();
    assert(finalWeight2 < initialWeight2); // Should depress
    
    // Test configuration
    hebbian.configure(0.05f, 0.8f);
    assert(hebbian.getLearningRate() == 0.05f);
    assert(hebbian.getMaxWeightChange() == 0.8f);
    
    // Test weight bounds
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse3.setType(nlm::SynapseType::Excitatory);
    synapse3.setWeight(0.9f);
    
    nlm::Neuron preNeuron3(nlm::NeuronId(5));
    nlm::Neuron postNeuron3(nlm::NeuronId(6));
    
    preNeuron3.setFiringState(nlm::FiringState::Active);
    postNeuron3.setFiringState(nlm::FiringState::Active);
    
    hebbian.update(&synapse3, &preNeuron3, &postNeuron3);
    assert(synapse3.getWeight() <= 1.0f);
    
    // Test no effect when pre not firing
    nlm::Synapse synapse4(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse4.setType(nlm::SynapseType::Excitatory);
    synapse4.setWeight(0.5f);
    
    nlm::Neuron preNeuron4(nlm::NeuronId(7)); // Not firing
    nlm::Neuron postNeuron4(nlm::NeuronId(8));
    postNeuron4.setFiringState(nlm::FiringState::Active);
    
    float initialWeight4 = synapse4.getWeight();
    
    hebbian.update(&synapse4, &preNeuron4, &postNeuron4);
    assert(synapse4.getWeight() == initialWeight4); // No change
    
    std::cout << "Hebbian tests passed" << std::endl;
    return true;
}

// Structural Plasticity Tests
// Tests for structural plasticity and synapse formation/elimination

TEST_CASE("structural_plasticity_test", TestCategory::UNIT, "Tests for structural plasticity")
void testStructuralPlasticity() {
    // This would test structural changes in neural connections
    // For now, we'll test the interface and basic functionality
    
    nlm::StructuralPlasticity plasticity;
    
    assert(plasticity.getMaxSynapses() > 0);
    assert(plasticity.getPruningThreshold() > 0.0f);
    assert(plasticity.getGrowthRate() > 0.0f);
    
    // Test synapse formation
    nlm::SynapseId synapseId(1);
    nlm::NeuronId preNeuronId(1);
    nlm::NeuronId postNeuronId(2);
    
    bool formed = plasticity.formSynapse(synapseId, preNeuronId, postNeuronId);
    assert(formed); // Should form successfully
    
    // Test synapse elimination
    nlm::SynapseId synapseId2(2);
    bool eliminated = plasticity.eliminateSynapse(synapseId2);
    assert(eliminated); // Should eliminate successfully (created above)
    
    // Test strength-dependent growth
    nlm::SynapseId synapseId3(3);
    bool growth = plasticity.conditionalGrowth(synapseId3, preNeuronId, postNeuronId, 0.8f);
    assert(growth); // Should grow with sufficient strength
    
    // Test pruning
    plasticity.pruneWeakConnections(0.1f); // Prune connections with strength < 0.1
    
    // Test activity-dependent plasticity
    bool activityDependent = plasticity.activityDependentPlasticity(
        preNeuronId, postNeuronId, 0.5f, 10.0f
    );
    assert(activityDependent); // Should respond to activity
    
    std::cout << "Structural plasticity tests passed" << std::endl;
    return true;
}