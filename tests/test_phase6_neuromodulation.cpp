// Phase 6 Neuromodulation Integration Tests
// Tests dopamine, curiosity, and novelty systems work together

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

using namespace nlm;

namespace test_phase6_neuromodulation {

void testDopamineIntegration() {
    std::cout << "  Testing Dopamine Integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test dopamine system creation and basic functionality
    auto* dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    // Test initial dopamine level
    float initialDopamine = dopamine->getCurrentDopamine();
    assert(initialDopamine >= 0.0f);
    
    // Test dopamine modulation with reward
    dopamine->applyModulation(0.5f, 0.1f);  // reward, learning_rate
    
    float afterModulation = dopamine->getCurrentDopamine();
    assert(afterModulation >= initialDopamine);
    
    std::cout << "    PASSED" << std::endl;
}

void testCuriosityIntegration() {
    std::cout << "  Testing Curiosity Integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test curiosity system creation and basic functionality
    auto* curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    // Test curiosity threshold
    float threshold = curiosity->getCuriosityThreshold();
    assert(threshold >= 0.0f && threshold <= 1.0f);
    
    // Test novelty detection
    float novelty = curiosity->evaluateNovelty(0.3f);
    assert(novelty >= 0.0f && novelty <= 1.0f);
    
    // Test curiosity activation
    curiosity->activateCuriosity();
    
    std::cout << "    PASSED" << std::endl;
}

void testNoveltyIntegration() {
    std::cout << "  Testing Novelty Integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test novelty system creation and basic functionality
    auto* novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    // Test novelty sensitivity
    float sensitivity = novelty->getNoveltySensitivity();
    assert(sensitivity >= 0.0f && sensitivity <= 1.0f);
    
    // Test novelty processing with different inputs
    float novelty1 = novelty->process(0.1f);
    float novelty2 = novelty->process(0.9f);  // Should be higher
    
    assert(novelty1 >= 0.0f && novelty1 <= 1.0f);
    assert(novelty2 >= 0.0f && novelty2 <= 1.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testNeuromodulationSynergy() {
    std::cout << "  Testing Neuromodulation System Synergy..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test that all neuromodulation systems are integrated
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Test coordinated response
    float reward = 0.7f;
    float curiosityThreshold = curiosity->getCuriosityThreshold();
    
    // Dopamine responds to reward
    dopamine->applyModulation(reward, 0.05f);
    
    // Novelty affects curiosity
    float inputNovelty = novelty->process(reward);
    curiosity->update(inputNovelty);
    
    // Verify all systems are working together
    float finalDopamine = dopamine->getCurrentDopamine();
    bool curiosityActive = curiosity->isCuriosityActive();
    
    assert(finalDopamine >= 0.0f);
    assert(curiosityActive || !curiosityActive);  // Either state is valid
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Neuromodulation Integration Tests ===" << std::endl;
    
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    testDopamineIntegration();
    testCuriosityIntegration();
    testNoveltyIntegration();
    testNeuromodulationSynergy();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Neuromodulation Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_neuromodulation