// Neuromodulation Integration Tests for Phase 6 NLM Brain
// Tests dopamine, curiosity, and novelty integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neuromodulation_integration {

void testDopamineIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get dopamine system
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test dopamine level updates
    float initialLevel = dopamine->getLevel();
    
    // Simulate reward prediction error
    nlm::Neuromodulator rewardSignal;
    rewardSignal.setLevel(1.5f); // High reward
    brain.applyNeuromodulation(rewardSignal);
    
    // Dopamine level should be affected
    float updatedLevel = dopamine->getLevel();
    assert(updatedLevel > initialLevel);
    
    // Test plasticity factor
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor > 0.0f);
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void testCuriosityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get curiosity system
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test curiosity updates based on novel events
    float initialCuriosity = curiosity->getLevel();
    
    // Simulate exposure to novel stimulus
    curiosity->updateNovelEvent("new_environment");
    
    // Curiosity should be affected
    float updatedCuriosity = curiosity->getLevel();
    assert(updatedCuriosity >= initialCuriosity);
    
    // Test curiosity-driven exploration
    curiosity->setExplorationBias(0.7f);
    float explorationBias = curiosity->getExplorationBias();
    assert(explorationBias == 0.7f);
    
    std::cout << "    testCuriosityIntegration passed" << std::endl;
}

void testNoveltyIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get novelty system
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test novelty detection
    float initialNovelty = novelty->getLevel();
    
    // Simulate exposure to novel stimulus
    novelty->updatePattern({0.1f, 0.9f, 0.2f, 0.8f}); // High variance = novel
    
    float updatedNovelty = novelty->getLevel();
    assert(updatedNovelty >= initialNovelty);
    
    // Test novelty threshold
    novelty->setNoveltyThreshold(0.5f);
    float threshold = novelty->getNoveltyThreshold();
    assert(threshold == 0.5f);
    
    // Test novelty reset
    novelty->reset();
    float resetNovelty = novelty->getLevel();
    assert(resetNovelty == 0.0f);
    
    std::cout << "    testNoveltyIntegration passed" << std::endl;
}

void testNeuromodulationEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Set neuromodulation levels
    nlm::Neuromodulator rewardSignal;
    rewardSignal.setLevel(1.0f);
    
    nlm::Neuromodulator explorationSignal;
    explorationSignal.setLevel(0.8f);
    
    nlm::Neuromodulator noveltySignal;
    noveltySignal.setLevel(0.6f);
    
    // Apply neuromodulation
    brain.applyNeuromodulation(rewardSignal);
    brain.applyNeuromulation(explorationSignal);
    brain.applyNeuromodulation(noveltySignal);
    
    // Check that neuromodulation systems were updated
    assert(dopamine->getLevel() > 0.0f);
    assert(curiosity->getLevel() > 0.0f);
    assert(novelty->getLevel() > 0.0f);
    
    std::cout << "    testNeuromodulationEffects passed" << std::endl;
}

} // namespace test_neuromodulation_integration
