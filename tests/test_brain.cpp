// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainSensoryInputIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a sensory input with some data
    nlm::SensoryInput sensoryInput;
    std::vector<float> data = {1.0f, 0.5f, -0.3f, 0.8f, 0.2f, -0.1f, 0.9f, 0.4f};
    sensoryInput.setData(data);
    
    // Get initial working memory state
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Initial working memory should be empty or have low activity
    std::vector<float> initialPattern = workingMemory->retrieve();
    
    // Receive sensory input - this should integrate with working memory
    brain.receiveSensoryInput(sensoryInput);
    
    // Working memory should now contain some pattern based on sensory input
    std::vector<float> afterSensoryPattern = workingMemory->retrieve();
    
    // At least some neurons should be active in working memory
    bool memoryUpdated = false;
    for (float activation : afterSensoryPattern) {
        if (activation > 0.01f) {
            memoryUpdated = true;
            break;
        }
    }
    
    // Working memory should have been updated by sensory input
    assert(memoryUpdated);
    
    std::cout << "    testBrainSensoryInputIntegration passed" << std::endl;
}

void testBrainWorkingMemoryCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store multiple patterns to test competition
    std::vector<float> pattern1 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    std::vector<float> pattern2 = {0.3f, 0.4f, 0.2f, 0.1f, 0.0f};
    std::vector<float> pattern3 = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    
    workingMemory->store(pattern1, 1.0f);
    workingMemory->store(pattern2, 1.0f);
    workingMemory->store(pattern3, 1.0f);
    
    // Run a step to allow competition to occur
    brain.step(0);
    
    // Working memory should have winners based on competition
    std::vector<nlm::NeuronId> winners = workingMemory->getMemoryNeurons();
    
    // At least some patterns should remain after competition
    assert(!winners.empty() || workingMemory->getMemoryActivity() > 0.0f);
    
    std::cout << "    testBrainWorkingMemoryCompetition passed" << std::endl;
}

void testBrainNeuromodulationEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    // Create sensory input
    nlm::SensoryInput sensoryInput;
    std::vector<float> data = {0.7f, 0.6f, 0.5f, 0.4f, 0.3f};
    sensoryInput.setData(data);
    
    // Get initial working memory state
    std::vector<float> initialPattern = workingMemory->retrieve();
    
    // Set neuromodulators to high levels
    if (dopamine) dopamine->setLevel(0.8f);
    if (curiosity) curiosity->setLevel(0.7f);
    if (novelty) novelty->setLevel(0.6f);
    
    // Receive sensory input with neuromodulators active
    brain.receiveSensoryInput(sensoryInput);
    
    // Working memory should be enhanced by neuromodulators
    std::vector<float> enhancedPattern = workingMemory->retrieve();
    
    // Memory should be enhanced (activations should be higher than initial)
    bool neuromodulationApplied = false;
    for (size_t i = 0; i < enhancedPattern.size() && i < initialPattern.size(); ++i) {
        if (enhancedPattern[i] > initialPattern[i] * 1.1f) {  // Allow 10% enhancement
            neuromodulationApplied = true;
            break;
        }
    }
    
    // Neuromodulation should have enhanced working memory
    assert(neuromodulationApplied);
    
    std::cout << "    testBrainNeuromodulationEffects passed" << std::endl;
}

void testBrainAttentionalSelection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Set up attention system
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    // Create sensory input
    nlm::SensoryInput sensoryInput;
    std::vector<float> data = {0.8f, 0.5f, 0.3f, 0.9f, 0.2f};
    sensoryInput.setData(data);
    
    // Get initial attention state
    auto initialAttended = attention->getWinners();
    
    // Process competition with attention system
    std::vector<nlm::NeuronId> competitors;
    for (size_t i = 0; i < data.size() && i < 5; ++i) {
        // Create mock neuron IDs for competition
        competitors.push_back(nlm::NeuronId(i));
    }
    
    // Process competition
    attention->processCompetition(competitors, 0.5f);
    
    // Attention should select some winners
    auto winners = attention->getWinners();
    
    // Should have some attended neurons
    assert(!winners.empty() || !initialAttended.empty());
    
    std::cout << "    testBrainAttentionalSelection passed" << std::endl;
}

} // namespace test_brain
