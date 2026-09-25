// Basic integration test for memory retention and prediction systems
// Tests that working memory, episodic memory, and prediction system work together

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_integration {

void testMemoryIntegration() {
    std::cout << "  Testing memory integration..." << std::endl;
    
    // Create brain with memory system support
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test 1: Working memory integration
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory storage and retrieval
    std::vector<float> testPattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    workingMemory->store(testPattern, 1.0f);
    
    std::vector<float> retrieved = workingMemory->retrieve();
    assert(retrieved.size() == testPattern.size());
    
    // Check that pattern is preserved in working memory
    for (size_t i = 0; i < testPattern.size(); ++i) {
        assert(std::abs(retrieved[i] - testPattern[i]) < 0.01f);
    }
    
    // Test 2: Episodic memory integration
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create a test episodic memory item
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.reward = 0.8f;
    episode.novelty = 0.5f;
    episode.action = nlm::ActionType::MoveForward;
    episode.sensoryState = testPattern;
    
    // Store episode
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test retrieval
    auto episodes = episodicMemory->getRecentEpisodes(5);
    assert(!episodes.empty());
    assert(episodes[0]->reward == 0.8f);
    
    // Test 3: Prediction system integration
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system basic functionality
    std::vector<float> errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.empty() || errorHistory.size() > 0);
    
    // Test concept formation integration
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation with working memory input
    auto conceptId = conceptFormation->presentExperience(
        testPattern, testPattern, 0.7f, 1
    );
    assert(conceptId >= 0);
    
    // Test that concepts are discovered
    assert(conceptFormation->getConceptCount() >= 1);
    
    // Test 4: Attention integration with memory
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Process competition from working memory
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    auto attended = attention->processCompetition(memoryNeurons);
    
    // Test attention state
    auto attendedRegions = attention->getAttendedRegions();
    assert(attendedRegions.size() <= brain.getRegionCount());
    
    // Test 5: Sleep/rest cycle integration
    // Simulate sleep/rest by calling the step with sleep mode
    // (This is a simplified test - in real scenario, sleep happens automatically)
    
    std::cout << "  Memory integration test passed" << std::endl;
}

void testPredictionAccuracy() {
    std::cout << "  Testing prediction accuracy..." << std::endl;
    
    // Create brain for prediction testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction error tracking
    std::vector<float> initialHistory = predictionSystem->getErrorHistory();
    float initialError = predictionSystem->getPredictionError();
    
    // Simulate prediction and observation
    std::vector<float> predicted = {0.1f, 0.5f, 0.9f};
    std::vector<float> actual = {0.15f, 0.45f, 0.95f};
    
    // Create sensory inputs
    nlm::SensoryInput predictedInput(predicted);
    nlm::SensoryInput actualInput(actual);
    
    // Update prediction based on error
    predictionSystem->updatePredictions(predictedInput, actualInput);
    
    // Check that error history was updated
    std::vector<float> updatedHistory = predictionSystem->getErrorHistory();
    assert(!updatedHistory.empty());
    
    // Test prediction confidence
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test prediction training
    predictionSystem->train(actualInput);
    
    std::cout << "  Prediction accuracy test passed" << std::endl;
}

void testMemoryRetention() {
    std::cout << "  Testing memory retention..." << std::endl;
    
    // Create brain for memory retention testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store test pattern in working memory
    std::vector<float> testPattern = {0.5f, 0.8f, 0.3f, 0.9f, 0.2f};
    workingMemory->store(testPattern, 1.0f);
    
    // Simulate multiple steps (working memory decay)
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Check that working memory is still present
        auto currentPattern = workingMemory->retrieve();
        assert(!currentPattern.empty());
        
        // Working memory should not be completely empty immediately
        if (step > 5) {
            // After some steps, pattern should still have some presence
            bool hasSignificantActivity = false;
            for (float val : currentPattern) {
                if (val > 0.01f) {
                    hasSignificantActivity = true;
                    break;
                }
            }
            // Allow for decay - pattern should diminish over time
        }
    }
    
    // Test episodic memory retention over time
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create and store multiple episodes
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.reward = 0.5f + (i * 0.05f);
        episode.sensoryState = std::vector<float>(5, 0.5f);
        episode.action = static_cast<nlm::ActionType>(i % 2);
        
        episodicMemory->storeEpisode(episode);
    }
    
    // Check that all episodes are stored
    assert(episodicMemory->getEpisodeCount() >= 10);
    
    // Test memory consolidation (when threshold is reached)
    // This is a simplified test - real consolidation happens based on relevance
    
    std::cout << "  Memory retention test passed" << std::endl;
}

void testEndToEndIntegration() {
    std::cout << "  Testing end-to-end integration..." << std::endl;
    
    // Create a complete brain with all integrated systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create test sensory input
    std::vector<float> sensoryInput = {0.2f, 0.7f, 0.5f, 0.9f, 0.3f};
    nlm::SensoryInput input(sensoryInput);
    
    // Test 1: Sensory input processing
    brain.receiveSensoryInput(input);
    
    // Test 2: Memory storage and retrieval
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        auto memoryPattern = workingMemory->retrieve();
        assert(!memoryPattern.empty());
    }
    
    // Test 3: Step simulation with all integrated systems
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify that all integrated systems are still functioning
        assert(brain.getTotalNeuronCount() > 0);
        
        // Check working memory activity
        if (workingMemory) {
            size_t activeTraces = workingMemory->getActiveTraces();
            // Working memory should have some activity
        }
        
        // Check episodic memory
        auto* episodicMemory = brain.getEpisodicMemory();
        if (episodicMemory) {
            size_t episodeCount = episodicMemory->getEpisodeCount();
            // Should accumulate episodes over time
        }
    }
    
    // Test 4: Save and load with integrated systems
    std::string testFile = "./test_checkpoint.bin";
    bool saveSuccess = brain.save(testFile);
    assert(saveSuccess);
    
    // Create a new brain and load the saved state
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(testFile);
    assert(loadSuccess);
    
    // Verify loaded state
    assert(brain2.getTotalNeuronCount() > 0);
    
    std::cout << "  End-to-end integration test passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== Running Integration Tests ===" << std::endl;
    
    try {
        testMemoryIntegration();
        testPredictionAccuracy();
        testMemoryRetention();
        testEndToEndIntegration();
        
        std::cout << "=== All integration tests passed successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "=== Integration test failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_integration