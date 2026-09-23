// Development System Tests
// Testing developmental processes, synaptogenesis, pruning, and maturation

#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_development {

void testDevelopmentSystem() {
    std::cout << "    Testing development system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test initialization
    developmentSystem->initialize(brain.get());
    
    // Test developmental stage
    nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial);
    
    // Test development update
    developmentSystem->update(brain.get(), *brain->getRandomGenerator(), 1000.0f);
    
    // Test developmental stage transition
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    stage = brain->getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::CriticalPeriod);
    
    std::cout << "      ✓ Development system tests passed" << std::endl;
}

void testSynaptogenesis() {
    std::cout << "    Testing synaptogenesis..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* synaptogenesis = std::make_unique<nlm::Synaptogenesis>();
    
    // Test initialization
    synaptogenesis->initialize(brain.get());
    
    // Test synaptogenesis update
    synaptogenesis->update(brain.get(), *brain->getRandomGenerator());
    
    // Test synaptiogenesis rate
    synaptogenesis->setSynaptogenesisRate(0.001f);
    float rate = synaptogenesis->getSynaptogenesisRate();
    assert(rate == 0.001f);
    
    std::cout << "      ✓ Synaptogenesis tests passed" << std::endl;
}

void testPruning() {
    std::cout << "    Testing pruning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* pruning = std::make_unique<nlm::Pruning>();
    
    // Test initialization
    pruning->initialize(brain.get());
    
    // Test pruning update
    pruning->update(brain.get(), *brain->getRandomGenerator());
    
    // Test pruning rate
    pruning->setPruningRate(0.0001f);
    float rate = pruning->getPruningRate();
    assert(rate == 0.0001f);
    
    // Test threshold
    pruning->setPruningThreshold(0.3f);
    float threshold = pruning->getPruningThreshold();
    assert(threshold == 0.3f);
    
    std::cout << "      ✓ Pruning tests passed" << std::endl;
}

void testMaturation() {
    std::cout << "    Testing maturation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto* maturation = std::make_unique<nlm::Maturation>();
    
    // Test initialization
    maturation->initialize(brain.get());
    
    // Test maturation update
    maturation->update(brain.get(), *brain->getRandomGenerator());
    
    // Test maturation parameters
    maturation->setMaturationRate(0.5f);
    float rate = maturation->getMaturationRate();
    assert(rate == 0.5f);
    
    // Test critical period end
    maturation->setCriticalPeriodEnd(100);
    int end = maturation->getCriticalPeriodEnd();
    assert(end == 100);
    
    std::cout << "      ✓ Maturation tests passed" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "    Testing development integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test that all development systems are available
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage progression
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    brain->develop();
    
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    brain->develop();
    
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    brain->develop();
    
    std::cout << "      ✓ Development integration tests passed" << std::endl;
}

void runAll() {
    std::cout << "=== Development System Tests ===" << std::endl;
    
    testDevelopmentSystem();
    testSynaptogenesis();
    testPruning();
    testMaturation();
    testDevelopmentIntegration();
    
    std::cout << "=== All Development Tests PASSED ===" << std::endl;
}

} // namespace test_development
