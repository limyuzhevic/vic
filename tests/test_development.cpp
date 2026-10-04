// Development System Tests
// Tests DevelopmentSystem and developmental processes

#include "development/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_development {

void testDevelopmentSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    auto* devSys = brain->getDevelopmentSystem();
    assert(devSys != nullptr);
    
    // Test developmental stage
    nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial);
    
    // Test development update
    devSys->update(brain.get(), *brain->getRandomGenerator(), 1000.0f); // 1000 ms
    
    // Test setting developmental stage
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain->getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testDevelopmentSystem passed" << std::endl;
}

void testDevelopmentStages() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Test each developmental stage
    std::vector<nlm::DevelopmentalStage> stages = {
        nlm::DevelopmentalStage::Initial,
        nlm::DevelopmentalStage::CriticalPeriod,
        nlm::DevelopmentalStage::Maturation,
        nlm::DevelopmentalStage::Adult,
        nlm::DevelopmentalStage::Aging
    };
    
    for (auto stage : stages) {
        brain->setDevelopmentalStage(stage);
        assert(brain->getDevelopmentalStage() == stage);
    }
    
    std::cout << "    testDevelopmentStages passed" << std::endl;
}

void testDevelopmentPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    assert(structuralPlasticity != nullptr);
    
    // Test development effects on plasticity
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    assert(brain->getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Run some steps with development enabled
    for (size_t step = 0; step < 1000; step += 100) {
        brain->step(step, step * 0.001);
    }
    
    std::cout << "    testDevelopmentPlasticity passed" << std::endl;
}

void runAll() {
    std::cout << "Running Development tests..." << std::endl;
    testDevelopmentSystem();
    testDevelopmentStages();
    testDevelopmentPlasticity();
}

} // namespace test_development
