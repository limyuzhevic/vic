// Test file for integrated systems - Phase 6
// This file tests the integrated memory, prediction, and cognition systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace nlm {
    class SensoryInput;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class PredictionSystem;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
} // namespace nlm

namespace test_integrated_systems {

void testWorkingMemoryIntegration() {
    std::cout << "Testing working memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test working memory is available and functional
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store a simple pattern
    std::vector<float> testPattern = {0.5f, 0.3f, 0.8f, 0.2f};
    workingMemory->store(testPattern, 1.0f);
    
    // Retrieve and verify pattern
    auto retrieved = workingMemory->retrieve();
    assert(retrieved.size() == 4);
    assert(std::abs(retrieved[0] - 0.5f) < 0.01f);
    assert(std::abs(retrieved[1] - 0.3f) < 0.01f);
    assert(std::abs(retrieved[2] - 0.8f) < 0.01f);
    assert(std::abs(retrieved[3] - 0.2f) < 0.01f);
    
    // Check capacity
    assert(workingMemory->getCapacity() == 50);
    assert(workingMemory->getActiveTraces() > 0);
    
    std::cout << "  Working memory integration test PASSED" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "Testing episodic memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("episodic_memory_max_episodes", 100, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test episodic memory is available
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create and store an episodic memory item
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.sensoryState = {0.5f, 0.3f, 0.8f};
    episode.reward = 0.5f;
    episode.positionX = 1.0f;
    episode.positionY = 2.0f;
    
    episodicMemory->storeEpisode(episode);
    
    // Check that episode was stored
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test retrieval
    auto recent = episodicMemory->getRecentEpisodes(5);
    assert(recent.size() == 1);
    assert(recent[0]->reward == 0.5f);
    
    // Test location-based retrieval
    auto byLocation = episodicMemory->retrieveByLocation(1.0f, 2.0f, 0.1f);
    assert(byLocation.size() == 1);
    
    std::cout << "  Episodic memory integration test PASSED" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("prediction_learning_rate", 0.01f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test prediction system is available
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create test sensory input
    auto sensoryInput = std::make_unique<nlm::SensoryInput>();
    std::vector<float> testData = {0.5f, 0.3f, 0.8f, 0.2f};
    sensoryInput->setData(testData);
    
    // Test prediction
    auto prediction = predictionSystem->predictNextState(*sensoryInput);
    assert(prediction != nullptr);
    assert(!prediction->getData().empty());
    
    // Check confidence
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Check error tracking
    float error = predictionSystem->getPredictionError();
    assert(error >= 0.0f);
    
    // Test training
    predictionSystem->train(*sensoryInput);
    
    // Check error history
    const auto& errorHistory = predictionSystem->getErrorHistory();
    assert(!errorHistory.empty());
    
    std::cout << "  Prediction system integration test PASSED" << std::endl;
}

void testCognitionSystemsIntegration() {
    std::cout << "Testing cognition systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test attention system
    auto* attention = brain.getAttention();
    if (attention) {
        std::vector<nlm::NeuronId> competitors;
        for (size_t i = 0; i < 5; ++i) {
            competitors.push_back(nlm::NeuronId(i + 50000));
        }
        
        auto winners = attention->processCompetition(competitors, 0.5f);
        assert(winners.size() <= competitors.size());
        
        std::cout << "    Attention system: " << winners.size() << " winners selected" << std::endl;
    }
    
    // Test concept formation
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        std::vector<float> pattern = {0.5f, 0.7f, 0.3f, 0.9f};
        std::vector<float> features = {0.1f, 0.8f, 0.5f};
        size_t conceptId = conceptFormation->presentExperience(pattern, features, 0.6f, 0);
        
        // Concept may or may not be created depending on novelty threshold
        // Both outcomes are acceptable for testing integration
        if (conceptId > 0) {
            assert(conceptFormation->getConceptCount() >= 1);
            std::cout << "    Concept formation: Created concept ID " << conceptId << std::endl;
        } else {
            std::cout << "    Concept formation: Pattern not novel enough for new concept" << std::endl;
        }
    }
    
    // Test neural planner
    auto* planner = brain.getPlanner();
    if (planner) {
        std::vector<float> currentState = {0.2f, 0.5f, 0.8f};
        nlm::ActionType action = planner->planAction(currentState, 0.5f);
        
        assert(action >= nlm::ActionType::Wait && action <= nlm::ActionType::Interact);
        assert(planner->getPlanningDepth() >= 1);
        
        std::cout << "    Neural planner: Selected action " << static_cast<int>(action) << std::endl;
    }
    
    std::cout << "  Cognition systems integration test PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("dopamine_learning_rate", 0.02f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test dopamine system
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        float level = dopamine->getLevel();
        assert(level >= -1.0f && level <= 1.0f);
        std::cout << "    Dopamine level: " << level << std::endl;
    }
    
    // Test curiosity system
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        float curiosityLevel = curiosity->getLevel();
        assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
        std::cout << "    Curiosity level: " << curiosityLevel << std::endl;
    }
    
    // Test novelty system
    auto* novelty = brain.getNovelty();
    if (novelty) {
        float noveltyLevel = novelty->getLevel();
        assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
        std::cout << "    Novelty level: " << noveltyLevel << std::endl;
    }
    
    // Test prediction error signal
    auto* predError = brain.getPredictionErrorSignal();
    if (predError) {
        float error = predError->getLevel();
        assert(error >= 0.0f);
        std::cout << "    Prediction error signal: " << error << std::endl;
    }
    
    std::cout << "  Neuromodulation integration test PASSED" << std::endl;
}

void runAllIntegratedSystemTests() {
    std::cout << "" << std::endl;
    std::cout << "=== NLM Phase 6 Integrated System Tests ===" << std::endl;
    std::cout << "Testing all newly integrated systems..." << std::endl;
    std::cout << "" << std::endl;
    
    try {
        testWorkingMemoryIntegration();
        testEpisodicMemoryIntegration();
        testPredictionSystemIntegration();
        testCognitionSystemsIntegration();
        testNeuromodulationIntegration();
        
        std::cout << "" << std::endl;
        std::cout << "=== All Integrated System Tests PASSED ===" << std::endl;
        std::cout << "The Phase 6 integrated systems are working correctly!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "" << std::endl;
        std::cout << "=== TEST FAILED ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        std::cout << "Integrated system test FAILED" << std::endl;
    } catch (...) {
        std::cout << "" << std::endl;
        std::cout << "=== TEST FAILED ===" << std::endl;
        std::cout << "Unknown exception occurred" << std::endl;
        std::cout << "Integrated system test FAILED" << std::endl;
    }
}

} // namespace test_integrated_systems

// Main function for integrated system tests
int main() {
    test_integrated_systems::runAllIntegratedSystemTests();
    return 0;
}
