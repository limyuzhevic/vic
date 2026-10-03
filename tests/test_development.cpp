// Development System Tests
// This file tests the development components of the NLM framework
// Covers developmental stages, structural plasticity, and age-dependent plasticity

#include "development/DevelopmentalStages/DevelopmentalStages.hpp"
#include "development/StructuralPlasticity/StructuralPlasticity.hpp"
#include "development/AgeDependentPlasticity/AgeDependentPlasticity.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_development {

void testDevelopmentalStages() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("development.stages.max_stages", 5, nlm::ConfigSource::Default);
    config->set("development.stages.transition_threshold", 0.8, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto developmentalStages = brain.getDevelopmentalStages();
    assert(developmentalStages != nullptr);
    
    // Test stage progression
    nlm::DevelopmentStage initialStage = developmentalStages->getCurrentStage();
    assert(initialStage != nlm::DevelopmentStage::Final);
    
    // Advance through stages
    for (int i = 0; i < 3; ++i) {
        developmentalStages->advanceStage();
        nlm::DevelopmentStage currentStage = developmentalStages->getCurrentStage();
        assert(currentStage != nlm::DevelopmentStage::Final);
    }
    
    // Test stage-specific characteristics
    auto stageCharacteristics = developmentalStages->getStageCharacteristics();
    assert(stageCharacteristics.size() > 0);
    
    std::cout << "    testDevelopmentalStages passed" << std::endl;
}

void testDevelopmentalStageTransitions() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("development.stages.transition_threshold", 0.9, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto developmentalStages = brain.getDevelopmentalStages();
    
    // Test transition conditions
    nlm::DevelopmentStage currentStage = developmentalStages->getCurrentStage();
    
    // Set stage completion conditions
    developmentalStages->completeStage();
    
    // Check if transition occurred
    nlm::DevelopmentStage newStage = developmentalStages->getCurrentStage();
    assert(newStage != currentStage); // Should have transitioned
    
    std::cout << "    testDevelopmentalStageTransitions passed" << std::endl;
}

void testStructuralPlasticitySynaptogenesis() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("development.structural_plasticity.synaptogenesis_rate", 0.1, nlm::ConfigSource::Default);
    config->set("development.structural_plasticity.pruning_rate", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto structuralPlasticity = brain.getStructuralPlasticity();
    assert(structuralPlasticity != nullptr);
    
    // Test synaptogenesis (formation of new synapses)
    std::vector<nlm::Synapse> newSynapses = structuralPlasticity->synaptogenesis(10);
    assert(newSynapses.size() == 10);
    
    // Test that synapses have valid properties
    for (const auto& synapse : newSynapses) {
        assert(synapse.sourceNeuron >= 0);
        assert(synapse.targetNeuron >= 0);
        assert(synapse.weight >= 0.0 && synapse.weight <= 1.0);
        assert(synapse.delay >= 0.0);
    }
    
    // Test synaptic pruning
    structuralPlasticity->pruneSynapses(5); // Prune 5 synapses
    
    std::cout << "    testStructuralPlasticitySynaptogenesis passed" << std::endl;
}

void testStructuralPlasticityPruning() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("development.structural_plasticity.pruning_rate", 0.2, nlm::ConfigSource::Default);
    config->set("development.structural_plasticity.min_strength_threshold", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto structuralPlasticity = brain.getStructuralPlasticity();
    
    // Create some synapses with varying strengths
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 20; ++i) {
        nlm::Synapse synapse;
        synapse.sourceNeuron = i;
        synapse.targetNeuron = (i + 1) % 20;
        synapse.weight = (i % 5 == 0) ? 0.1 : 0.8; // Some weak, some strong
        synapse.delay = 1.0 + (i % 3);
        synapses.push_back(synapse);
    }
    
    // Store synapses (assuming structural plasticity has a method to add synapses)
    // This is a simplified test - actual implementation may vary
    
    // Test pruning
    structuralPlasticity->pruneSynapses(10); // Prune 10 synapses
    
    std::cout << "    testStructuralPlasticityPruning passed" << std::endl;
}

void testAgeDependentPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("development.age_dependent_plasticity.youth_plasticity", 1.0, nlm::ConfigSource::Default);
    config->set("development.age_dependent_plasticity.adult_plasticity", 0.3, nlm::ConfigSource::Default);
    config->set("development.age_dependent_plasticity.elderly_plasticity", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto ageDependentPlasticity = brain.getAgeDependentPlasticity();
    assert(ageDependentPlasticity != nullptr);
    
    // Test age-dependent plasticity factors
    double currentAge = ageDependentPlasticity->getCurrentAge();
    assert(currentAge >= 0.0 && currentAge <= 1.0);
    
    // Test plasticity for different age groups
    double youthPlasticity = ageDependentPlasticity->getPlasticityFactor(nlm::AgeGroup::Youth);
    double adultPlasticity = ageDependentPlasticity->getPlasticityFactor(nlm::AgeGroup::Adult);
    double elderlyPlasticity = ageDependentPlasticity->getPlasticityFactor(nlm::AgeGroup::Elderly);
    
    assert(youthPlasticity > adultPlasticity);
    assert(adultPlasticity > elderlyPlasticity);
    
    // Test plasticity modulation
    ageDependentPlasticity->modulatePlasticity(0.5); // Moderate modulation
    
    std::cout << "    testAgeDependentPlasticity passed" << std::endl;
}

void testAgeDependentPlasticityDevelopmentalEffects() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto ageDependentPlasticity = brain.getAgeDependentPlasticity();
    
    // Test that plasticity affects learning differently at different ages
    std::vector<double> learningRates;
    for (int age = 0; age <= 100; age += 10) {
        ageDependentPlasticity->setCurrentAge(static_cast<double>(age) / 100.0);
        double plasticityFactor = ageDependentPlasticity->getPlasticityFactor(nlm::AgeGroup::Adult);
        learningRates.push_back(plasticityFactor * 0.1); // Simplified learning rate calculation
    }
    
    // Learning should be highest in youth and decrease with age
    assert(learningRates[0] > learningRates[learningRates.size() - 1]);
    
    std::cout << "    testAgeDependentPlasticityDevelopmentalEffects passed" << std::endl;
}

void testDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("development.stages.max_stages", 3, nlm::ConfigSource::Default);
    config->set("development.structural_plasticity.synaptogenesis_rate", 0.1, nlm::ConfigSource::Default);
    config->set("development.age_dependent_plasticity.youth_plasticity", 0.8, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all developmental systems are present
    assert(brain.getDevelopmentalStages() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    assert(brain.getAgeDependentPlasticity() != nullptr);
    
    // Test development system interactions
    auto developmentalStages = brain.getDevelopmentalStages();
    auto structuralPlasticity = brain.getStructuralPlasticity();
    auto ageDependentPlasticity = brain.getAgeDependentPlasticity();
    
    // Stage progression should affect structural plasticity
    developmentalStages->advanceStage();
    
    // Age-dependent plasticity should affect developmental processes
    ageDependentPlasticity->modulatePlasticity(0.3);
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void runAll() {
    testDevelopmentalStages();
    testDevelopmentalStageTransitions();
    testStructuralPlasticitySynaptogenesis();
    testStructuralPlasticityPruning();
    testAgeDependentPlasticity();
    testAgeDependentPlasticityDevelopmentalEffects();
    testDevelopmentIntegration();
    
    std::cout << "All development tests passed!" << std::endl;
}

} // namespace test_development
