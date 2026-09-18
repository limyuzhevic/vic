#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Maturation.hpp"
#include "development/Pruning.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Config/Config.hpp"

namespace test_brain_advanced {

void testNeuronTypes() {
    std::cout << "Testing neuron types and properties..." << std::endl;
    
    nlm::Neuron lif_neuron(nlm::NeuronId(1), nlm::NeuronType::LIF, 0.001f, 1.0f, -70.0f, 20.0f);
    assert(lif_neuron.getType() == nlm::NeuronType::LIF);
    assert(lif_neuron.getMembraneTimeConstant() == 0.001f);
    assert(lif_neuron.getThreshold() == 1.0f);
    assert(lif_neuron.getRestPotential() == -70.0f);
    assert(lif_neuron.getMaxFiringRate() == 20.0f);
    
    nlm::Neuron izh_neuron(nlm::NeuronId(2), nlm::NeuronType::Izhikevich, 0.002f, 0.5f, 1.0f, -65.0f, -70.0f);
    assert(izh_neuron.getType() == nlm::NeuronType::Izhikevich);
    assert(izh_neuron.getA() == 0.5f);
    assert(izh_neuron.getB() == 1.0f);
    assert(izh_neuron.getC() == -65.0f);
    assert(izh_neuron.getD() == -70.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testSynapticConnections() {
    std::cout << "Testing synaptic connections..." << std::endl;
    
    nlm::Neuron pre_neuron(nlm::NeuronId(1));
    nlm::Neuron post_neuron(nlm::NeuronId(2));
    
    nlm::Synapse synapse(&pre_neuron, &post_neuron, 0.1f, 0.8f, 1.0f);
    assert(synapse.getWeight() == 0.1f);
    assert(synapse.getDelay() == 0.8f);
    assert(synapse.getMaxDelay() == 1.0f);
    assert(synapse.getPreSynapticNeuron() == &pre_neuron);
    assert(synapse.getPostSynapticNeuron() == &post_neuron);
    assert(synapse.getIsActive() == false);
    
    pre_neuron.setMembranePotential(1.5f);
    post_neuron.setMembranePotential(-60.0f);
    
    synapse.activate();
    assert(synapse.getIsActive() == true);
    
    float input = synapse.getSynapticInput();
    post_neuron.update(0.01f, input);
    assert(post_neuron.getMembranePotential() > -70.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testPlasticityMechanisms() {
    std::cout << "Testing plasticity mechanisms (STDP, Hebbian)..." << std::endl;
    
    nlm::Neuron pre_neuron(nlm::NeuronId(1));
    nlm::Neuron post_neuron(nlm::NeuronId(2));
    
    nlm::STDP stdp(0.01f, 0.02f, 5.0f, 5.0f);
    assert(stdp.getLTPTimeWindow() == 0.01f);
    assert(stdp.getLTDTimeWindow() == 0.02f);
    assert(stdp.getMaxLTP() == 5.0f);
    assert(stdp.getMaxLTD() == 5.0f);
    
    nlm::Hebbian hebbian(0.1f, 0.5f, 0.05f);
    assert(hebbian.getLearningRate() == 0.1f);
    assert(hebbian.getMaxWeight() == 0.5f);
    assert(hebbian.getMinWeight() == 0.05f);
    
    nlm::StructuralPlasticity structural(0.001f, 0.01f, 0.5f, 10.0f);
    assert(structural.getConnectionCreationRate() == 0.001f);
    assert(structural.getConnectionDeletionRate() == 0.01f);
    assert(structural.getMaxConnections() == 0.5f);
    assert(structural.getMaxBranchingFactor() == 10.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testNeuromodulationEffects() {
    std::cout << "Testing neuromodulation effects..." << std::endl;
    
    nlm::Neuromodulator dopamine(nlm::DopamineType::Excitatory, 1.0f, 0.1f, -65.0f, 1.0f);
    assert(dopamine.getType() == nlm::NeuromodulatorType::Dopamine);
    assert(dopamine.getValue() == 1.0f);
    assert(dopamine.getTimeConstant() == 0.1f);
    assert(dopamine.getBaseLevel() == -65.0f);
    assert(dopamine.getGain() == 1.0f);
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    float potential_before = neuron.getMembranePotential();
    
    dopamine.applyToNeuron(&neuron, 0.01f);
    float potential_after = neuron.getMembranePotential();
    
    assert(potential_after > potential_before);
    
    nlm::Novelty novelty_detector(0.5f, 0.8f, 0.01f, 0.05f);
    assert(novelty_detector.getThreshold() == 0.5f);
    assert(novelty_detector.getAdaptationRate() == 0.8f);
    assert(novelty_detector.getDecayRate() == 0.01f);
    assert(novelty_detector.getMaxValue() == 0.05f);
    
    nlm::Novelty::NoveltyInput input;
    input.patternId = 123;
    input.intensity = 0.9f;
    input.context = "test_pattern";
    
    float novelty_value = novelty_detector.update(input);
    assert(novelty_value > novelty_detector.getThreshold());
    
    nlm::Curiosity curiosity(0.7f, 0.3f, 0.1f, 0.5f, 1.0f);
    assert(curiosity.getExplorationRate() == 0.7f);
    assert(curiosity.getExploitationRate() == 0.3f);
    assert(curiosity.getNoveltyThreshold() == 0.1f);
    assert(curiosity.getMaxCuriosity() == 0.5f);
    assert(curiosity.getDecayRate() == 1.0f);
    
    float curiosity_value = curiosity.update(0.8f, 1.0f);
    assert(curiosity_value > 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "Testing memory integration..." << std::endl;
    
    nlm::NeuralWorkingMemory workingMem(10, 100.0f, 0.5f);
    assert(workingMem.getCapacity() == 10);
    assert(workingMem.getDecayRate() == 0.5f);
    
    nlm::NeuralEpisodicMemory episodicMem(1000, 0.3f, 0.1f);
    assert(episodicMem.getCapacity() == 1000);
    assert(episodicMem.getConsolidationRate() == 0.3f);
    assert(episodicMem.getRecallProbability() == 0.1f);
    
    nlm::MemoryItem item1;
    item1.type = nlm::MemoryItemType::Working;
    item1.content = std::vector<float>{1.0f, 2.0f, 3.0f};
    item1.timestamp = 1000.0f;
    item1.associationStrength = 0.9f;
    
    workingMem.encode(item1);
    assert(workingMem.getItemCount() == 1);
    
    nlm::MemoryItem item2;
    item2.type = nlm::MemoryItemType::Episodic;
    item2.content = std::vector<float>{4.0f, 5.0f, 6.0f};
    item2.timestamp = 1001.0f;
    item2.associationStrength = 0.8f;
    
    episodicMem.encode(item2);
    assert(episodicMem.getItemCount() == 1);
    
    nlm::MemoryItem retrieved = workingMem.retrieve(0.0f);
    assert(retrieved.type == nlm::MemoryItemType::Working);
    
    workingMem.update(1.0f);
    assert(workingMem.getItemCount() == 1);
    
    episodicMem.update(100.0f);
    assert(episodicMem.getItemCount() == 1);
    
    std::cout << "  PASSED" << std::endl;
}

void testDevelopmentStages() {
    std::cout << "Testing development stages..." << std::endl;
    
    nlm::DevelopmentSystem devSystem;
    assert(devSystem.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    devSystem.update(10.0f);
    assert(devSystem.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    nlm::Synaptogenesis synaptogenesis(0.1f, 0.5f, 0.2f, 0.8f);
    assert(synaptogenesis.getInitialRate() == 0.1f);
    assert(synaptogenesis.getPeakRate() == 0.5f);
    assert(synaptogenesis.getGrowthPeriod() == 0.2f);
    assert(synaptogenesis.getDeclinePeriod() == 0.8f);
    
    nlm::Maturation maturation(0.3f, 0.9f, 1.0f, 0.1f);
    assert(maturation.getMaturationRate() == 0.3f);
    assert(maturation.getTargetMaturity() == 0.9f);
    assert(maturation.getTimeToMaturity() == 1.0f);
    assert(maturation.getPruningThreshold() == 0.1f);
    
    nlm::Pruning pruning(0.4f, 0.6f, 0.15f, 0.8f);
    assert(pruning.getPruningRate() == 0.4f);
    assert(pruning.getRecoveryRate() == 0.6f);
    assert(pruning.getActivityThreshold() == 0.15f);
    assert(pruning.getMaxPruning() == 0.8f);
    
    assert(devSystem.getCriticalPeriodDuration() > 0.0f);
    assert(devSystem.getCriticalPeriodEnd() > devSystem.getCriticalPeriodStart());
    
    std::cout << "  PASSED" << std::endl;
}

void testAgentWorldInteraction() {
    std::cout << "Testing agent-world interaction..." << std::endl;
    
    nlm::Brain brain(10, 20);
    
    nlm::SensoryInput sensoryInput;
    assert(sensoryInput.getInputCount() == 0);
    
    sensoryInput.addInput(1, 0.5f);
    assert(sensoryInput.getInputCount() == 1);
    
    nlm::Action action;
    assert(action.getActionCount() == 0);
    
    action.addAction(nlm::MotorCommand::MOVE_FORWARD, 1.0f);
    assert(action.getActionCount() == 1);
    
    brain.executeAction(action);
    assert(action.getActionCount() == 1);
    
    brain.processSensoryInput(sensoryInput);
    assert(sensoryInput.getInputCount() == 1);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Advanced Brain Tests ===" << std::endl;
    
    testNeuronTypes();
    testSynapticConnections();
    testPlasticityMechanisms();
    testNeuromodulationEffects();
    testMemoryIntegration();
    testDevelopmentStages();
    testAgentWorldInteraction();
    
    std::cout << "=== All Advanced Brain Tests PASSED ===" << std::endl;
}

} // namespace test_brain_advanced
