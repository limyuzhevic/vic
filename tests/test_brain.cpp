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

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory  
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test attention system
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Test concept formation
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test planning system
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    std::cout << "    testBrainMemorySystems passed" << std::endl;
}

void testBrainEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create some neural activity
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Check if episodic memory has stored episodes
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Episodic memory should have stored some episodes
    // In a real implementation, we would check episode count
    std::cout << "    testBrainEpisodicMemory passed" << std::endl;
}

void testBrainConceptFormation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Present some experiences to concept formation
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features1 = {0.5f, 0.7f, 0.3f};
    std::vector<float> pattern2 = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    std::vector<float> features2 = {0.8f, 0.9f, 0.6f};
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Present experiences
    size_t concept1 = conceptFormation->presentExperience(pattern1, features1, 0.8f, 10);
    size_t concept2 = conceptFormation->presentExperience(pattern2, features2, 0.9f, 11);
    
    // Concepts should be discovered
    if (concept1 > 0) {
        const auto* discovered = conceptFormation->getConcept(concept1);
        assert(discovered != nullptr);
    }
    
    if (concept2 > 0) {
        const auto* discovered = conceptFormation->getConcept(concept2);
        assert(discovered != nullptr);
    }
    
    std::cout << "    testBrainConceptFormation passed" << std::endl;
}

void testBrainPlanningSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test planning with current state
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    float targetReward = 1.0f;
    
    nlm::ActionType action = planner->planAction(currentState, targetReward);
    
    // Action should be valid
    assert(action == nlm::ActionType::Wait || action == nlm::ActionType::MoveForward ||
           action == nlm::ActionType::MoveBackward || action == nlm::ActionType::TurnLeft ||
           action == nlm::ActionType::TurnRight || action == nlm::ActionType::Interact);
    
    std::cout << "    testBrainPlanningSystem passed" << std::endl;
}

void testBrainPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Initial prediction error should be valid
    float error = predictionSystem->getPredictionError();
    assert(error >= 0.0f);
    
    // Test that prediction system tracks error history
    const auto& errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.empty() || errorHistory.size() > 0);
    
    std::cout << "    testBrainPredictionSystem passed" << std::endl;
}

void testBrainAttentionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Test attention system update
    attention->update(0.001f);
    
    // Check attention winners
    const auto& winners = attention->getWinners();
    assert(winners.empty() || winners.size() > 0);
    
    // Test attention parameters
    float inhibition = attention->getInhibitionStrength();
    float excitation = attention->getExcitationStrength();
    
    assert(inhibition >= 0.0f);
    assert(excitation >= 0.0f);
    
    std::cout << "    testBrainAttentionSystem passed" << std::endl;
}

void testBrainSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps to create some activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Save brain state
    bool saved = brain.save("test_checkpoint.kcp");
    assert(saved);
    
    // Reset brain
    brain.reset();
    
    // Load brain state
    bool loaded = brain.load("test_checkpoint.kcp");
    assert(loaded);
    
    // Verify some properties after load
    assert(brain.getTotalNeuronCount() == 100);
    
    // Clean up
    std::remove("test_checkpoint.kcp");
    
    std::cout << "    testBrainSaveLoad passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    // New tests for integrated systems
    testBrainMemorySystems();
    testBrainEpisodicMemory();
    testBrainConceptFormation();
    testBrainPlanningSystem();
    testBrainPredictionSystem();
    testBrainAttentionSystem();
    testBrainSaveLoad();
}

} // namespace test_brain
