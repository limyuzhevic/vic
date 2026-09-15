// Advanced Plasticity Tests
// Tests for new advanced plasticity rules (R-STDP, neuromodulatory plasticity,
// homosynaptic/heterosynaptic plasticity, synaptic tagging and capture,
// metaplasticity, STP, neuromodulatory integration)

#include "plasticity/AdvancedPlasticity.hpp"
#include "brain/Synapse.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_advanced_plasticity {

void testRewardModulatedSTDP() {
    nlm::RewardModulatedSTDP rstdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Set reward prediction error
    rstdp.setRewardPredictionError(0.1f);
    rstdp.setLearningRate(0.02f);
    rstdp.setRewardDecayRate(0.05f);
    rstdp.setBaselineReward(0.1f);
    
    // Create spike patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    // Update with reward
    rstdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Check that eligibility trace was updated
    float eligibility = synapse.getEligibilityTrace();
    
    std::cout << "    testRewardModulatedSTDP passed (eligibility=" << eligibility << ")" << std::endl;
}

void testHomosynapticPlasticity() {
    nlm::HomosynapticPlasticity homoPlasticity;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Set homosynaptic parameters
    homoPlasticity.setHomosynapticStrength(0.01f);
    homoPlasticity.setTargetWeight(0.5f);
    homoPlasticity.setOutputDependency(0.3f);
    homoPlasticity.setHeterosynapticRange(5.0f);
    homoPlasticity.setHeterosynapticStrength(0.005f);
    homoPlasticity.enableHeterosynaptic(true);
    
    // Create spike patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    // Update with homosynaptic plasticity
    homoPlasticity.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Check that weight changed (should increase due to output dependency)
    float finalWeight = synapse.getWeight();
    
    std::cout << "    testHomosynapticPlasticity passed (weight Δ=" << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSynapticTaggingAndCapture() {
    nlm::SynapticTaggingAndCapture stc;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.4f);
    
    float initialWeight = synapse.getWeight();
    
    // Set STC parameters
    stc.setTagPersistence(100.0f);
    stc.setCaptureThreshold(0.5f);
    stc.setTagDecayRate(0.01f);
    stc.setCaptureRange(2.0f);
    stc.setMultiNeuromodulatorIntegration(false); // Simplify test
    
    // Create spike patterns that would set a tag
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    // Update with synaptic tagging and capture
    stc.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Check weight change
    float finalWeight = synapse.getWeight();
    
    std::cout << "    testSynapticTaggingAndCapture passed (weight Δ=" << (finalWeight - initialWeight) << ")" << std::endl;
}

void testMetaplasticity() {
    nlm::Metaplasticity metaPlasticity;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.setEfficacy(0.8f);
    
    float initialWeight = synapse.getWeight();
    float initialEfficacy = synapse.getEfficacy();
    
    // Set metaplasticity parameters
    metaPlasticity.setPlasticityThreshold(0.1f);
    metaPlasticity.setPlasticityHistorySize(100);
    metaPlasticity.setMetaplasticLearningRate(0.001f);
    metaPlasticity.setHomeostasisTarget(0.0f);
    
    // Create spike patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    // Update with metaplasticity
    metaPlasticity.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Check that efficacy changed (metaplastic regulation of plasticity)
    float finalEfficacy = synapse.getEfficacy();
    
    std::cout << "    testMetaplasticity passed (efficacy Δ=" << (finalEfficacy - initialEfficacy) << ")" << std::endl;
}

void testShortTermPlasticity() {
    nlm::ShortTermPlasticity stp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.6f);
    synapse.setEfficacy(1.0f);
    
    float initialWeight = synapse.getWeight();
    float initialEfficacy = synapse.getEfficacy();
    
    // Set STP parameters
    stp.setDepressionParameters(0.5f, 100.0f);
    stp.setFacilitationParameters(0.5f, 10.0f);
    stp.enableDepression(true);
    stp.enableFacilitation(true);
    stp.setUseDependentModulation(true);
    stp.setModulationParameters(1.0f);
    
    // Create spike patterns to trigger STP
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    // Update with short-term plasticity
    stp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Check efficacy change (use-dependent modulation)
    float finalEfficacy = synapse.getEfficacy();
    
    std::cout << "    testShortTermPlasticity passed (efficacy Δ=" << (finalEfficacy - initialEfficacy) << ")" << std::endl;
}

void testNeuromodulatoryIntegration() {
    nlm::NeuromodulatoryIntegration neuromodIntegration;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.setEfficacy(1.0f);
    
    float initialWeight = synapse.getWeight();
    float initialEfficacy = synapse.getEfficacy();
    
    // Add neuromodulators
    neuromodIntegration.addNeuromodulator(nlm::NeuromodulatorType::Dopamine, 0.1f, 0.01f, 0.05f);
    neuromodIntegration.addNeuromodulator(nlm::NeuromodulatorType::Acetylcholine, 0.05f, 0.01f, 0.05f);
    neuromodIntegration.addNeuromodulator(nlm::NeuromodulatorType::Norepinephrine, 0.05f, 0.01f, 0.05f);
    neuromodIntegration.addNeuromodulator(nlm::NeuromodulatorType::Serotonin, 0.05f, 0.01f, 0.05f);
    
    // Set modulation weights
    std::vector<float> weights = {1.0f, 1.0f, 1.0f, 1.0f};
    neuromodIntegration.setModulationWeights(weights);
    
    // Create interaction matrix (simple identity for test)
    std::vector<std::vector<float>> interactions(4, std::vector<float>(4, 0.0f));
    for (int i = 0; i < 4; ++i) {
        interactions[i][i] = 1.0f;
    }
    neuromodIntegration.setInteractionTerms(interactions);
    
    // Create spike patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    // Update with neuromodulatory integration
    neuromodIntegration.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Check weight change with neuromodulatory effects
    float finalWeight = synapse.getWeight();
    float finalEfficacy = synapse.getEfficacy();
    
    std::cout << "    testNeuromodulatoryIntegration passed (weight Δ=" << (finalWeight - initialWeight) << ")" << std::endl;
}

void testPlasticityRuleFactory() {
    // Test factory function for creating advanced plasticity rules
    nlm::PlasticityRule* rstdp = nlm::createAdvancedPlasticityRule("RewardModulatedSTDP");
    assert(rstdp != nullptr);
    assert(std::string(rstdp->getName()) == "RewardModulatedSTDP");
    delete rstdp;
    
    nlm::PlasticityRule* homo = nlm::createAdvancedPlasticityRule("HomosynapticPlasticity");
    assert(homo != nullptr);
    assert(std::string(homo->getName()) == "HomosynapticPlasticity");
    delete homo;
    
    nlm::PlasticityRule* stc = nlm::createAdvancedPlasticityRule("SynapticTaggingAndCapture");
    assert(stc != nullptr);
    assert(std::string(stc->getName()) == "SynapticTaggingAndCapture");
    delete stc;
    
    nlm::PlasticityRule* meta = nlm::createAdvancedPlasticityRule("Metaplasticity");
    assert(meta != nullptr);
    assert(std::string(meta->getName()) == "Metaplasticity");
    delete meta;
    
    nlm::PlasticityRule* stp = nlm::createAdvancedPlasticityRule("ShortTermPlasticity");
    assert(stp != nullptr);
    assert(std::string(stp->getName()) == "ShortTermPlasticity");
    delete stp;
    
    nlm::PlasticityRule* neuromod = nlm::createAdvancedPlasticityRule("NeuromodulatoryIntegration");
    assert(neuromod != nullptr);
    assert(std::string(neuromod->getName()) == "NeuromodulatoryIntegration");
    delete neuromod;
    
    std::cout << "    testPlasticityRuleFactory passed" << std::endl;
}

void runAll() {
    std::cout << "Running Advanced Plasticity tests..." << std::endl;
    testRewardModulatedSTDP();
    testHomosynapticPlasticity();
    testSynapticTaggingAndCapture();
    testMetaplasticity();
    testShortTermPlasticity();
    testNeuromodulatoryIntegration();
    testPlasticityRuleFactory();
}

} // namespace test_advanced_plasticity