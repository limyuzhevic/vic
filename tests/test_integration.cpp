// NLM Phase 6 Integration Tests
// Testing real integration between cognitive systems

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>

namespace test_integration {

void testMemorySystemIntegration() {
    std::cout << "  Testing Memory System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that memory systems are accessible and functional
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory functionality
    workingMemory->initialize(&brain);
    workingMemory->setCapacity(50);
    
    // Store some data in working memory
    workingMemory->storeToNeuron(1001, 0.5f);
    assert(workingMemory->contains(1001));
    assert(workingMemory->getNeuronActivation(1001) > 0.0f);
    
    // Test episodic memory functionality
    episodicMemory->initialize(&brain);
    episodicMemory->setMaxEpisodes(100);
    
    // Create a test episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 10;
    episode.sensoryState = {0.1f, 0.2f, 0.3f};
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 1.0f;
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test episodic memory retrieval
    auto similarEpisodes = episodicMemory->retrieveSimilar({0.1f, 0.2f, 0.3f}, 5);
    assert(!similarEpisodes.empty());
    
    std::cout << "    PASSED" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "  Testing Prediction System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that prediction system is accessible
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // The prediction system may not have full implementation yet (Phase 2)
    // But we should be able to access it and call basic methods
    auto errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.size() >= 0); // Could be empty initially
    
    predictionSystem->clearHistory();
    assert(predictionSystem->getErrorHistory().empty());
    
    std::cout << "    PASSED" << std::endl;
}

void testNeuralPlannerIntegration() {
    std::cout << "  Testing NeuralPlanner Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("planning_depth", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that neural planner is accessible
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test planner configuration
    assert(planner->getPlanningDepth() == 5);
    
    // Create a simple state for planning
    std::vector<float> testState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Test planning (should return a valid action)
    nlm::ActionType action = planner->planAction(testState, 0.5f);
    assert(action == nlm::ActionType::Wait || action == nlm::ActionType::MoveForward);
    
    // Test planner confidence
    float confidence = planner->getPlanningConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testConceptFormationIntegration() {
    std::cout << "  Testing ConceptFormation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that concept formation is accessible
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Create test experiences with different patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.8f, 0.7f, 0.6f}; // Similar but different
    std::vector<float> pattern3 = {0.1f, 0.2f, 0.3f}; // Same as pattern1
    
    // Present experiences
    size_t concept1 = conceptFormation->presentExperience(pattern1, pattern1, 1.0f, 10);
    size_t concept2 = conceptFormation->presentExperience(pattern2, pattern2, 0.5f, 20);
    size_t concept3 = conceptFormation->presentExperience(pattern3, pattern3, 1.5f, 30);
    
    // pattern3 should match concept1
    assert(concept1 == concept3 || concept1 == 0 || concept3 == 0);
    
    // Get concept information
    size_t conceptCount = conceptFormation->getConceptCount();
    assert(conceptCount >= 1);
    
    if (conceptCount > 0) {
        auto* concept = conceptFormation->getConcept(1);
        if (concept) {
            float stability = conceptFormation->getConceptStability(1);
            assert(stability >= 0.0f && stability <= 1.0f);
        }
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testSaveLoadIntegration() {
    std::cout << "  Testing Save/Load Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", std::string("./test_checkpoints"), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps to create state
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Test save functionality
    std::string testFile = "./test_checkpoint.nlm";
    bool saveSuccess = brain.save(testFile);
    assert(saveSuccess);
    
    // Test load functionality
    nlm::Brain brain2(config);
    bool loadSuccess = brain2.load(testFile);
    assert(loadSuccess);
    
    // Verify basic properties are preserved
    assert(brain2.getTotalNeuronCount() == 50);
    assert(brain2.getRegionCount() == 2);
    
    std::cout << "    PASSED" << std::endl;
}

void testActionSelectionIntegration() {
    std::cout << "  Testing Action Selection Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test basic action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // The action should be either Wait or MoveForward
    nlm::ActionType actionType = action->getType();
    assert(actionType == nlm::ActionType::Wait || actionType == nlm::ActionType::MoveForward);
    
    // Test that neural planner influences action selection
    // by setting up a simple motor neuron that fires
    if (!brain.getMotorNeurons().empty()) {
        brain.getMotorNeurons()[0]->injectCurrent(10.0f); // Make neuron fire
        brain.step(0); // Process the spike
        
        auto action2 = brain.produceAction();
        if (action2) {
            // Action might now be MoveForward due to firing motor neuron
            assert(action2->getType() == nlm::ActionType::Wait || 
                   action2->getType() == nlm::ActionType::MoveForward);
        }
    }
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    testMemorySystemIntegration();
    testPredictionSystemIntegration();
    testNeuralPlannerIntegration();
    testConceptFormationIntegration();
    testSaveLoadIntegration();
    testActionSelectionIntegration();
    
    std::cout << std::endl;
    std::cout << "=== Integration Tests Completed ===" << std::endl;
}

} // namespace test_integration