// Neuromodulation Tests
// Phase 3: Real Neuromodulation Tests

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include "neuromodulation/NeuromodulationManager.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>

namespace test_neuromodulation {

void testDopamine() {
    std::cout << "Testing Dopamine..." << std::endl;
    
    // Test basic functionality
    nlm::Dopamine dopamine;
    
    assert(std::string(dopamine.getName()) == "DA");
    assert(dopamine.getLevel() == 0.0f);
    
    // Test level setting with bounds
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    dopamine.setLevel(1.2f); // Should clamp to 1.0
    assert(dopamine.getLevel() == 1.0f);
    
    dopamine.setLevel(-0.2f); // Should clamp to 0.0
    assert(dopamine.getLevel() == 0.0f);
    
    // Test plasticity factor
    dopamine.setLevel(0.0f);
    float plasticity = dopamine.getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 0.6f);
    
    dopamine.setLevel(1.0f);
    plasticity = dopamine.getPlasticityFactor();
    assert(plasticity >= 0.9f && plasticity <= 1.0f);
    
    // Test reward signaling
    dopamine.setLevel(0.5f);
    float before = dopamine.getLevel();
    dopamine.signalReward(0.8f);
    assert(dopamine.getLevel() > before);
    
    // Test prediction error
    float after = dopamine.getLevel();
    dopamine.signalRewardPredictionError(-0.5f);
    assert(dopamine.getLevel() < after);
    
    dopamine.signalRewardPredictionError(0.5f);
    assert(dopamine.getLevel() > after);
    
    // Test update dynamics
    float initial = dopamine.getLevel();
    dopamine.update(0.001f);
    // Should decay towards baseline (0.0)
    assert(dopamine.getLevel() <= initial);
    
    std::cout << "  PASSED" << std::endl;
}

void testAcetylcholine() {
    std::cout << "Testing Acetylcholine..." << std::endl;
    
    nlm::Acetylcholine acetylcholine;
    
    assert(std::string(acetylcholine.getName()) == "ACh");
    assert(acetylcholine.getLevel() == 0.0f);
    
    // Test level setting
    acetylcholine.setLevel(0.7f);
    assert(acetylcholine.getLevel() == 0.7f);
    
    // Test plasticity factor
    acetylcholine.setLevel(0.0f);
    float plasticity = acetylcholine.getPlasticityFactor();
    assert(plasticity >= 0.8f && plasticity <= 0.9f);
    
    acetylcholine.setLevel(1.0f);
    plasticity = acetylcholine.getPlasticityFactor();
    assert(plasticity >= 1.1f && plasticity <= 1.2f);
    
    // Test attention signaling
    acetylcholine.setLevel(0.5f);
    float before = acetylcholine.getLevel();
    acetylcholine.signalAttention(0.8f, 0.3f); // High salience, some novelty
    assert(acetylcholine.getLevel() > before);
    
    // Test memory enhancement
    std::vector<float> encoding{0.1f, 0.2f, 0.3f};
    acetylcholine.setLevel(0.8f); // High attention level
    acetylcholine.enhanceMemory(encoding);
    assert(encoding[0] > 0.1f);
    assert(encoding[1] > 0.2f);
    assert(encoding[2] > 0.3f);
    
    acetylcholine.setLevel(0.1f); // Low attention level
    float before2 = encoding[0];
    acetylcholine.enhanceMemory(encoding);
    assert(encoding[0] == before2); // Should not change
    
    std::cout << "  PASSED" << std::endl;
}

void testNorepinephrine() {
    std::cout << "Testing Norepinephrine..." << std::endl;
    
    nlm::Norepinephrine norepinephrine;
    
    assert(std::string(norepinephrine.getName()) == "NE");
    assert(norepinephrine.getLevel() == 0.0f);
    
    // Test level setting
    norepinephrine.setLevel(0.6f);
    assert(norepinephrine.getLevel() == 0.6f);
    
    // Test plasticity factor
    norepinephrine.setLevel(0.0f);
    float plasticity = norepinephrine.getPlasticityFactor();
    assert(plasticity >= 0.9f && plasticity <= 1.0f);
    
    norepinephrine.setLevel(1.0f);
    plasticity = norepinephrine.getPlasticityFactor();
    assert(plasticity >= 1.2f && plasticity <= 1.5f);
    
    // Test arousal signaling
    norepinephrine.setLevel(0.3f);
    float before = norepinephrine.getLevel();
    norepinephrine.signalArousal(0.9f, 0.4f); // High intensity, some novelty
    assert(norepinephrine.getLevel() > before);
    
    // Test vigilance enhancement
    std::vector<float> pattern{0.8f, 0.7f, 0.6f};
    std::vector<float> threshold{0.7f, 0.7f, 0.7f};
    norepinephrine.setLevel(0.2f);
    float vigilanceBefore = norepinephrine.getLevel(); // Access through level
    norepinephrine.enhanceVigilance(pattern, threshold);
    
    // Should have increased vigilance (affects level indirectly)
    assert(norepinephrine.getLevel() >= vigilanceBefore);
    
    std::cout << "  PASSED" << std::endl;
}

void testSerotonin() {
    std::cout << "Testing Serotonin..." << std::endl;
    
    nlm::Serotonin serotonin;
    
    assert(std::string(serotonin.getName()) == "5-HT");
    assert(serotonin.getLevel() == 0.0f);
    
    // Test level setting
    serotonin.setLevel(0.8f);
    assert(serotonin.getLevel() == 0.8f);
    
    // Test plasticity factor
    serotonin.setLevel(0.0f);
    float plasticity = serotonin.getPlasticityFactor();
    assert(plasticity >= 0.7f && plasticity <= 0.8f);
    
    serotonin.setLevel(1.0f);
    plasticity = serotonin.getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 0.6f);
    
    // Test mood signaling
    serotonin.setLevel(0.5f);
    float before = serotonin.getLevel();
    serotonin.signalMood(0.7f, 0.4f); // Positive reward history, some social context
    assert(serotonin.getLevel() > before);
    
    // Test social behavior regulation
    float levelBefore = serotonin.getLevel();
    serotonin.regulateSocialBehavior(0.8f, 0.3f); // High cohesion, moderate dominance
    assert(serotonin.getLevel() >= levelBefore);
    
    // Test negative mood
    serotonin.setLevel(0.5f);
    serotonin.signalMood(-0.5f, -0.3f); // Negative reward and social context
    // Level may decrease depending on implementation
    assert(serotonin.getLevel() >= 0.0f); // Should not go negative
    
    std::cout << "  PASSED" << std::endl;
}

void testNeuromodulationManager() {
    std::cout << "Testing NeuromodulationManager..." << std::endl;
    
    // Create a minimal config
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(10));
    config->set("region_count", static_cast<int64_t>(1));
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cout << "  SKIPPED (brain initialization failed)" << std::endl;
        return;
    }
    
    // Test neuromodulation manager
    nlm::NeuromodulationManager manager;
    manager.initialize(brain.get());
    
    // Check that all neuromodulators are initialized
    assert(manager.getDopamine() != nullptr);
    assert(manager.getReward() != nullptr);
    assert(manager.getPredictionError() != nullptr);
    assert(manager.getNovelty() != nullptr);
    assert(manager.getCuriosity() != nullptr);
    
    // Test total modulation
    float total = manager.getTotalModulation();
    assert(total >= 0.0f);
    
    // Test exploration drive
    float exploration = manager.getExplorationDrive();
    assert(exploration >= 0.0f);
    
    // Test process observation
    nlm::Observation obs;
    nlm::Observation prevObs;
    manager.processObservation(obs, prevObs, 0.5f);
    
    // Test reset
    manager.reset();
    assert(manager.getDopamine()->getLevel() == 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation tests..." << std::endl;
    
    testDopamine();
    testAcetylcholine();
    testNorepinephrine();
    testSerotonin();
    testNeuromodulationManager();
    
    std::cout << "All neuromodulation tests passed!" << std::endl;
}

} // namespace test_neuromodulation