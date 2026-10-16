// BrainDevelopmentManager Comprehensive Tests
// Tests developmental stages, synaptogenesis, pruning

#include "brain/BrainDevelopmentManager.hpp"
#include "brain/BrainNeuralCore.hpp"
#include "brain/BrainPlasticityManager.hpp"
#include "core/Config/Config.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_brain_development_manager {

void testBrainDevelopmentManagerCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    
    assert(development.initialize());
    
    std::cout << "    testBrainDevelopmentManagerCreation passed" << std::endl;
}

void testDevelopmentalStageManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(development.initialize());
    
    // Test initial stage
    nlm::DevelopmentalStage initialStage = development.getStage();
    std::cout << "Initial stage: " << development.getStageName() << std::endl;
    assert(development.getStageName() != nullptr);
    
    // Test setting different stages
    development.setStage(nlm::DevelopmentalStage::Somatogenesis);
    assert(development.getStage() == nlm::DevelopmentalStage::Somatogenesis);
    assert(std::string(development.getStageName()) == "Somatogenesis");
    
    development.setStage(nlm::DevelopmentalStage::Neurulation);
    assert(development.getStage() == nlm::DevelopmentalStage::Neurulation);
    assert(std::string(development.getStageName()) == "Neurulation");
    
    development.advanceStage();
    
    std::cout << "    testDevelopmentalStageManagement passed" << std::endl;
}

void testSynaptogenesis() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    nlm::BrainPlasticityManager plasticity(config);
    plasticity.initialize();
    assert(development.initialize());
    
    // Set to synaptogenesis stage
    development.setStage(nlm::DevelopmentalStage::Synaptogenesis);
    
    // Create random generator
    nlm::RandomGenerator rng(42);
    std::unique_ptr<nlm::RandomGenerator> rngPtr = std::make_unique<nlm::RandomGenerator>(rng);
    
    // Apply developmental effects to neural core and plasticity
    development.applyEffectsToNeuralCore(&neuralCore);
    development.applyEffectsToPlasticity(&plasticity);
    
    std::cout << "    testSynaptogenesis passed" << std::endl;
}

void testPruning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    nlm::BrainPlasticityManager plasticity(config);
    plasticity.initialize();
    assert(development.initialize());
    
    // Set to pruning stage
    development.setStage(nlm::DevelopmentalStage::Pruning);
    
    // Create random generator
    nlm::RandomGenerator rng(42);
    std::unique_ptr<nlm::RandomGenerator> rngPtr = std::make_unique<nlm::RandomGenerator>(rng);
    
    // Apply developmental effects
    development.applyEffectsToNeuralCore(&neuralCore);
    development.applyEffectsToPlasticity(&plasticity);
    
    std::cout << "    testPruning passed" << std::endl;
}

void testDevelopmentalAge() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(development.initialize());
    
    double age = development.getDevelopmentalAge();
    assert(age >= 0.0);
    
    // Age should increase as stage advances
    development.advanceStage();
    development.advanceStage();
    
    double updatedAge = development.getDevelopmentalAge();
    assert(updatedAge >= age);
    
    std::cout << "    testDevelopmentalAge passed" << std::endl;
}

void testCriticalPeriod() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(development.initialize());
    
    bool isCritical = development.isCriticalPeriod();
    assert(!isCritical); // Should not be in critical period initially
    
    float progress = development.getCriticalPeriodProgress();
    assert(progress >= 0.0f && progress <= 1.0f);
    
    std::cout << "    testCriticalPeriod passed" << std::endl;
}

void testDevelopmentConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(development.initialize());
    
    // Configure development parameters
    development.setDevelopmentRate(0.5f);
    development.setCriticalPeriodStart(100.0f);
    development.setCriticalPeriodEnd(500.0f);
    
    std::cout << "    testDevelopmentConfiguration passed" << std::endl;
}

void testDevelopmentReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(development.initialize());
    
    // Advance through stages
    development.advanceStage();
    development.advanceStage();
    
    // Reset development
    development.reset();
    
    std::cout << "    testDevelopmentReset passed" << std::endl;
}

void testDevelopmentStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainDevelopmentManager development(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(development.initialize());
    
    // Test configuration access
    std::shared_ptr<const nlm::Config> cfg = development.getConfig();
    assert(cfg != nullptr);
    
    // Test status logging
    development.logStatus();
    
    std::cout << "    testDevelopmentStatus passed" << std::endl;
}

void runAll() {
    std::cout << "Running BrainDevelopmentManager tests..." << std::endl;
    
    testBrainDevelopmentManagerCreation();
    testDevelopmentalStageManagement();
    testSynaptogenesis();
    testPruning();
    testDevelopmentalAge();
    testCriticalPeriod();
    testDevelopmentConfiguration();
    testDevelopmentReset();
    testDevelopmentStatus();
    
    std::cout << "\nAll BrainDevelopmentManager tests passed!" << std::endl;
}

} // namespace test_brain_development_manager
