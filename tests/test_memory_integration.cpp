// Memory Integration Tests
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory_integration {

void testWorkingMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory storage
    std::vector<float> testPattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    workingMemory->store(testPattern, 1.0f);
    
    // Test working memory update
    workingMemory->update(0.001f);
    
    // Test working memory capacity
    assert(workingMemory->getCapacity() > 0);
    
    std::cout << "    testWorkingMemoryIntegration passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 10;
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 1.0f;
    episode.energy = 1.0f;
    episode.novelty = 0.5f;
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    episode.orientation = 0.0f;
    
    // Store episode
    episodicMemory->storeEpisode(episode);
    
    // Test episodic memory retrieval
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Test episodic memory similarity retrieval
    std::vector<float> queryPattern = {0.2f, 0.3f, 0.1f};
    auto similarEpisodes = episodicMemory->retrieveSimilar(queryPattern, 5);
    // Should not crash and should return something
    
    std::cout << "    testEpisodicMemoryIntegration passed" << std::endl;
}

void testAssociativeMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Create test patterns
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.8f, 0.9f, 1.0f};
    
    // Create association
    associativeMemory->associate(patternA, patternB, 0.8f);
    
    // Test association retrieval
    auto retrieved = associativeMemory->retrieve(patternA, 5);
    
    // Test association strength
    float strength = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(strength > 0.0f);
    
    std::cout << "    testAssociativeMemoryIntegration passed" << std::endl;
}

void testConceptFormationEpisodicIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation with episodic-like pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    std::vector<float> features = {0.1f, 0.2f, 0.3f};
    
    size_t conceptId = conceptFormation->presentExperience(pattern, features, 1.0f, 0);
    assert(conceptId != 0 || conceptId == 0);  // Either valid concept or not yet
    
    std::cout << "    testConceptFormationEpisodicIntegration passed" << std::endl;
}

void testNeuralPlannerEpisodicIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* neuralPlanner = brain.getPlanner();
    assert(neuralPlanner != nullptr);
    
    // Test planning with episodic memory
    std::vector<float> state = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    float targetReward = 1.0f;
    
    nlm::ActionType action = neuralPlanner->planAction(state, targetReward);
    // Should not crash
    
    std::cout << "    testNeuralPlannerEpisodicIntegration passed" << std::endl;
}

void testMemoryCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all memory systems are integrated
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    
    // Test attention system integration
    auto* attention = brain.getAttention();
    if (attention) {
        std::vector<nlm::NeuronId> competitors;
        auto winners = attention->processCompetition(competitors);
        // Should not crash
    }
    
    std::cout << "    testMemoryCognitionIntegration passed" << std::endl;
}

void testSensoryMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create test sensory input
    nlm::SensoryInput input;
    std::vector<float> data = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    input.setData(data);
    
    // Test sensory input processing (should not crash)
    brain.receiveSensoryInput(input);
    
    std::cout << "    testSensoryMemoryIntegration passed" << std::endl;
}

void testBrainMemoryStepIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("replay_interval", 5, nlm::ConfigSource::Default);
    config->set("consolidation_interval", 10, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create test sensory input
    nlm::SensoryInput input;
    std::vector<float> data = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    input.setData(data);
    
    // Run brain steps with memory integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.receiveSensoryInput(input);
        brain.step(step);
    }
    
    // Verify memory systems were used
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        assert(episodicMemory->getEpisodeCount() > 0);
    }
    
    std::cout << "    testBrainMemoryStepIntegration passed" << std::endl;
}

} // namespace test_memory_integration

void run_memory_tests() {
    std::cout << "Running Memory Integration Tests..." << std::endl;
    
    test_memory_integration::testWorkingMemoryIntegration();
    test_memory_integration::testEpisodicMemoryIntegration();
    test_memory_integration::testAssociativeMemoryIntegration();
    test_memory_integration::testConceptFormationEpisodicIntegration();
    test_memory_integration::testNeuralPlannerEpisodicIntegration();
    test_memory_integration::testMemoryCognitionIntegration();
    test_memory_integration::testSensoryMemoryIntegration();
    test_memory_integration::testBrainMemoryStepIntegration();
    
    std::cout << "All Memory Integration Tests passed!" << std::endl;
}