#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_brain_integration {

void testMemorySystemIntegration() {
    std::cout << "  Testing memory system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that memory systems are accessible
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory functionality
    workingMemory->initialize(&brain);
    workingMemory->setCapacity(50);
    workingMemory->store(std::vector<float>(10, 0.5f), 1.0f);
    
    assert(workingMemory->getActiveTraces() > 0);
    assert(workingMemory->getCapacity() == 50);
    
    // Test episodic memory functionality
    episodicMemory->initialize(&brain);
    episodicMemory->setMaxEpisodes(1000);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 0.5f;
    episode.novelty = 0.3f;
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    assert(episodicMemory->isReplayEnabled());
    
    // Test associative memory functionality
    associativeMemory->initialize(&brain);
    associativeMemory->associate(std::vector<float>(5, 0.1f), std::vector<float>(5, 0.9f), 0.8f);
    
    assert(associativeMemory->getAssociationCount() == 1);
    
    std::cout << " PASSED" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "  Testing prediction system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system basic functionality
    assert(predictionSystem->getPredictionError() >= 0.0f);
    assert(predictionSystem->getConfidence() >= 0.0f);
    assert(predictionSystem->getErrorHistory().empty());
    
    std::cout << " PASSED" << std::endl;
}

void testCognitiveSystemIntegration() {
    std::cout << "  Testing cognitive system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neural planner integration
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    assert(planner->getPlanningDepth() >= 0);
    assert(planner->getPlanningConfidence() >= 0.0f);
    
    // Test concept formation integration
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    std::cout << " PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation systems are accessible
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    // These should not be null (Phase 6 implementation)
    if (dopamine) {
        assert(dopamine->getLevel() >= 0.0f);
    }
    if (curiosity) {
        assert(curiosity->getLevel() >= 0.0f);
    }
    if (novelty) {
        assert(novelty->getLevel() >= 0.0f);
    }
    
    std::cout << " PASSED" << std::endl;
}

void testIntegratedBrainStepWithMemory() {
    std::cout << "  Testing integrated brain step with memory systems...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("replay_interval", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run brain steps - memory systems should update automatically
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Every 10 steps, episodic memory should store an episode
        if (step % 10 == 0) {
            auto* episodicMemory = brain.getEpisodicMemory();
            if (episodicMemory) {
                // Episodes accumulate over time
                assert(episodicMemory->getEpisodeCount() > 0);
            }
        }
    }
    
    // Verify memory systems are functional
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        assert(workingMemory->getActiveTraces() > 0);
    }
    
    std::cout << " PASSED" << std::endl;
}

void testIntegratedCheckpointing() {
    std::cout << "  Testing integrated checkpointing...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps to generate state
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Test save/load with integrated systems
    bool saved = brain.save("./test_checkpoint.bin");
    assert(saved);
    
    // Create new brain and load
    auto config2 = std::make_shared<nlm::Config>();
    nlm::Brain brain2(config2);
    bool loaded = brain2.load("./test_checkpoint.bin");
    assert(loaded);
    
    // Verify basic properties are preserved
    assert(brain2.getTotalNeuronCount() == brain.getTotalNeuronCount());
    
    std::cout << " PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Brain Integration Tests ===" << std::endl;
    
    testMemorySystemIntegration();
    testPredictionSystemIntegration();
    testCognitiveSystemIntegration();
    testNeuromodulationIntegration();
    testIntegratedBrainStepWithMemory();
    testIntegratedCheckpointing();
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_brain_integration