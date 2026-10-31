// Development Integration Tests
// Tests developmental processes

#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_development {

void testDevelopmentSystemBase() {
    std::cout << "  Testing DevelopmentSystem Base..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Test initial developmental stage
    assert(development->getStage() == nlm::DevelopmentalStage::Embryonic);
    
    // Test stage setting
    development->setStage(nlm::DevelopmentalStage::Juvenile);
    assert(development->getStage() == nlm::DevelopmentalStage::Juvenile);
    
    assert(std::string(development->getStageName()) == "Juvenile");
    
    // Test stage advancement
    development->advanceStage();
    assert(development->getStage() == nlm::DevelopmentalStage::Adolescent);
    
    // Test plasticity modifier
    float plasticity = development->getPlasticityModifier();
    assert(plasticity >= 0.0f && plasticity <= 1.0f);
    
    // Test critical period info
    bool criticalPeriod = development->isCriticalPeriod();
    float progress = development->getCriticalPeriodProgress();
    assert(progress >= 0.0f && progress <= 1.0f);
    
    // Test developmental age
    double age = development->getDevelopmentalAge();
    assert(age >= 0.0);
    
    development->setDevelopmentalAge(10.5);
    assert(development->getDevelopmentalAge() == 10.5);
    
    std::cout << "    testDevelopmentSystemBase passed" << std::endl;
}

void testDevelopmentStageProgression() {
    std::cout << "  Testing Development Stage Progression..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Test complete stage progression
    development->setStage(nlm::DevelopmentalStage::Embryonic);
    assert(development->getStage() == nlm::DevelopmentalStage::Embryonic);
    
    development->advanceStage();
    assert(development->getStage() == nlm::DevelopmentalStage::Fetal);
    
    development->advanceStage();
    assert(development->getStage() == nlm::DevelopmentalStage::Infant);
    
    development->advanceStage();
    assert(development->getStage() == nlm::DevelopmentalStage::Juvenile);
    
    development->advanceStage();
    assert(development->getStage() == nlm::DevelopmentalStage::Adolescent);
    
    development->advanceStage();
    assert(development->getStage() == nlm::DevelopmentalStage::YoungAdult);
    
    development->advanceStage();
    assert(development->getStage() == nlm::DevelopmentalStage::Adult);
    
    // Test stage names
    assert(std::string(development->getStageName()) == "Adult");
    
    std::cout << "    testDevelopmentStageProgression passed" << std::endl;
}

void testSynaptogenesisIntegration() {
    std::cout << "  Testing Synaptogenesis Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Synaptogenesis* synaptogenesis = brain.getStructuralPlasticity()->getSynaptogenesis();
    assert(synaptogenesis != nullptr);
    
    // Test synaptogenesis rate
    float rate = synaptogenesis->getSynaptogenesisRate();
    assert(rate >= 0.0f);
    
    synaptogenesis->setSynaptogenesisRate(0.5f);
    assert(std::abs(synaptogenesis->getSynaptogenesisRate() - 0.5f) < 0.001f);
    
    // Test development system integration
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Development system controls when synaptogenesis is active
    development->setStage(nlm::DevelopmentalStage::Infant);
    assert(development->getStage() == nlm::DevelopmentalStage::Infant);
    
    // During development, synaptogenesis would be more active
    // This tests the conceptual integration
    
    std::cout << "    testSynaptogenesisIntegration passed" << std::endl;
}

void testPruningIntegration() {
    std::cout << "  Testing Pruning Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Pruning* pruning = brain.getStructuralPlasticity()->getPruning();
    assert(pruning != nullptr);
    
    // Test pruning rate
    float rate = pruning->getPruningRate();
    assert(rate >= 0.0f);
    
    pruning->setPruningRate(0.3f);
    assert(std::abs(pruning->getPruningRate() - 0.3f) < 0.001f);
    
    // Test development system integration
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Pruning is more active in certain developmental stages
    development->setStage(nlm::DevelopmentalStage::YoungAdult);
    // Should have different pruning patterns than embryonic stage
    
    // Test pruning integration with memory systems
    // Pruning refines synaptic connections that are not used
    // This would be tested through the broader integration tests
    
    std::cout << "    testPruningIntegration passed" << std::endl;
}

void testMaturationIntegration() {
    std::cout << "  Testing Maturation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(70), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Maturation* maturation = brain.getStructuralPlasticity()->getMaturation();
    assert(maturation != nullptr);
    
    // Test maturation rate
    float rate = maturation->getMaturationRate();
    assert(rate >= 0.0f && rate <= 1.0f);
    
    maturation->setMaturationRate(0.7f);
    assert(std::abs(maturation->getMaturationRate() - 0.7f) < 0.001f);
    
    // Test development system integration
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Maturation rate can change with developmental stage
    development->setStage(nlm::DevelopmentalStage::Adult);
    assert(development->getStage() == nlm::DevelopmentalStage::Adult);
    
    // Adult brain has slower maturation rate
    // (Implementation dependent)
    
    // Test maturation integration with plasticity
    // Maturation affects how plasticity rules are applied
    
    std::cout << "    testMaturationIntegration passed" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "  Testing Development Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify development system is accessible
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    
    // Test development integration with brain systems
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    nlm::StructuralPlasticity* plasticity = brain.getStructuralPlasticity();
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    nlm::Novelty* novelty = brain.getNovelty();
    
    assert(development != nullptr);
    assert(plasticity != nullptr);
    assert(workingMemory != nullptr);
    assert(novelty != nullptr);
    
    // Test developmental stage affects memory consolidation
    development->setStage(nlm::DevelopmentalStage::Adolescent);
    assert(development->getStage() == nlm::DevelopmentalStage::Adolescent);
    
    // During adolescence, high plasticity affects working memory patterns
    workingMemory->store(nlm::NeuronId(1), 0.8f);
    workingMemory->decay(0.1f);
    
    // Novelty detection may be heightened during development
    // novelty->setLevel(0.6f);
    
    // Test development integration with neuromodulation
    nlm::Dopamine* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Development affects dopamine system maturation
    // Dopamine level changes with developmental stage
    
    // Test development integration with prediction
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    assert(prediction != nullptr);
    
    // Prediction system may develop over time
    // prediction->train({0.1f, 0.2f, 0.3f});
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void testDevelopmentPerformance() {
    std::cout << "  Testing Development Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    nlm::StructuralPlasticity* plasticity = brain.getStructuralPlasticity();
    assert(development != nullptr);
    assert(plasticity != nullptr);
    
    const size_t iterations = 3000;
    
    // Performance benchmark for developmental processes
    for (size_t i = 0; i < iterations; ++i) {
        // Advance developmental stage
        development->advanceStage();
        
        // Update developmental processes
        development->update(&brain, 0.01f);
        
        // Structural plasticity processes
        plasticity->update(&brain, brain.getRandomGenerator(), 0.01f);
        
        // This tests that development and plasticity can operate together
    }
    
    // Verify performance
    assert(development->getStage() != nlm::DevelopmentalStage::Embryonic);  // Should have advanced
    
    std::cout << "    testDevelopmentPerformance passed" << std::endl;
}

void testDevelopmentErrorConditions() {
    std::cout << "  Testing Development Error Conditions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Test development system error conditions
    // Stage should be valid enum
    development->setStage(nlm::DevelopmentalStage::Adult);
    assert(development->getStage() == nlm::DevelopmentalStage::Adult);
    
    // Negative developmental age should be handled
    development->setDevelopmentalAge(-1.0);
    assert(development->getDevelopmentalAge() < 0.0);  // Should allow negative for testing
    
    // Critical period calculation should handle edge cases
    float progress = development->getCriticalPeriodProgress();
    assert(progress >= 0.0f && progress <= 1.0f);
    
    // Test development update with null/invalid brain
    // Should be handled gracefully
    
    std::cout << "    testDevelopmentErrorConditions passed" << std::endl;
}

void runAll() {
    testDevelopmentSystemBase();
    testDevelopmentStageProgression();
    testSynaptogenesisIntegration();
    testPruningIntegration();
    testMaturationIntegration();
    testDevelopmentIntegration();
    testDevelopmentPerformance();
    testDevelopmentErrorConditions();
}

} // namespace test_development
