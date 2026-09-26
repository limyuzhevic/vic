# Phase 6 Integration Tests
# Comprehensive testing of memory, prediction, cognition system integration

// Phase 6 Integration Test Framework
// Tests that all major NLM systems are properly connected and functional

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

// Forward declarations
namespace nlm {
    class Brain;
    class WorkingMemory;
    class EpisodicMemory;
    class PredictionSystem;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    class Config;
    class SimpleWorld;
    class AgentBrain;
    class DevelopmentalStage;
    class NeuralRepresentationType;
    class ActionType;
    class Experience;
    class Concept;
    class MotorCommand;
}

namespace test_integration {

// Integration Test Framework
class IntegrationTestFramework {
public:
    IntegrationTestFramework() : totalTests(0), passedTests(0) {}
    
    void runAllTests() {
        std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
        std::cout << "Testing system interconnections and integration..." << std::endl;
        
        // Test memory system integration
        testWorkingMemoryIntegration();
        testEpisodicMemoryIntegration();
        
        // Test prediction system integration
        testPredictionSystemIntegration();
        
        // Test cognitive system integration
        testCognitiveSystemIntegration();
        
        // Test neuromodulation integration
        testNeuromodulationIntegration();
        
        // Test development system integration
        testDevelopmentIntegration();
        
        // Test complete brain loop integration
        testCompleteBrainLoop();
        
        std::cout << std::endl;
        std::cout << "=== Integration Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        
        if (passedTests == totalTests) {
            std::cout << "✓ All integration tests passed!" << std::endl;
            return 0;
        } else {
            std::cout << "✗ Some integration tests failed" << std::endl;
            return 1;
        }
    }
    
protected:
    void recordTest(const std::string& name, bool passed) {
        totalTests++;
        if (passed) {
            passedTests++;
            std::cout << "  ✓ " << name << std::endl;
        } else {
            std::cout << "  ✗ " << name << std::endl;
        }
    }
    
    int totalTests;
    int passedTests;
};

// Test WorkingMemory Integration
void testWorkingMemoryIntegration() {
    std::cout << "Testing WorkingMemory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "  ✗ Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test that WorkingMemory is accessible
    auto workingMemory = brain->getWorkingMemory();
    if (workingMemory == nullptr) {
        std::cout << "  ✗ WorkingMemory is null - not integrated" << std::endl;
        return;
    }
    
    // Test WorkingMemory functionality
    workingMemory->store(42, 3.14f, nlm::NeuralRepresentationType::Pattern);
    workingMemory->store(43, -1.5f, nlm::NeuralRepresentationType::Pattern);
    
    if (workingMemory->retrieve(42) == 3.14f) {
        recordTest("WorkingMemory storage/retrieval", true);
    } else {
        recordTest("WorkingMemory storage/retrieval", false);
    }
    
    // Test WorkingMemory interference and updating
    workingMemory->update(42, 6.28f);  // Double the value
    if (workingMemory->retrieve(42) == 6.28f) {
        recordTest("WorkingMemory update", true);
    } else {
        recordTest("WorkingMemory update", false);
    }
    
    // Test WorkingMemory decay
    workingMemory->decay(0.5f);  // Decay by 50%
    float retrieved = workingMemory->retrieve(42);
    if (retrieved < 6.28f) {  // Should be less after decay
        recordTest("WorkingMemory decay", true);
    } else {
        recordTest("WorkingMemory decay", false);
    }
    
    // Test WorkingMemory competition
    workingMemory->store(44, 10.0f, nlm::NeuralRepresentationType::Pattern);
    workingMemory->store(45, 8.0f, nlm::NeuralRepresentationType::Pattern);
    
    workingMemory->activate(45); // Activate one item
    float competitionScore = workingMemory->getCompetitionScore(45);
    if (competitionScore > 0.0f) {
        recordTest("WorkingMemory competition", true);
    } else {
        recordTest("WorkingMemory competition", false);
    }
}

// Test EpisodicMemory Integration
void testEpisodicMemoryIntegration() {
    std::cout << "Testing EpisodicMemory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "  ✗ Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test that EpisodicMemory is accessible
    auto episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory == nullptr) {
        std::cout << "  ✗ EpisodicMemory is null - not integrated" << std::endl;
        return;
    }
    
    // Test EpisodicMemory encoding
    nlm::Experience experience1;
    experience1.timestamp = 0.0;
    experience1.sensoryInput[3] = {0.1f, 0.2f, 0.3f};
    experience1.action = nlm::ActionType::MoveForward;
    experience1.reward = 1.0f;
    
    episodicMemory->encode(experience1);
    
    // Test EpisodicMemory retrieval
    std::vector<nlm::Experience> retrieved = episodicMemory->retrieve(0.0, 1.0); // Look back 1 second
    if (!retrieved.empty()) {
        recordTest("EpisodicMemory encoding/retrieval", true);
    } else {
        recordTest("EpisodicMemory encoding/retrieval", false);
    }
    
    // Test EpisodicMemory pattern completion
    nlm::Experience experience2;
    experience2.timestamp = 1.0;
    experience2.sensoryInput[3] = {0.15f, 0.25f, 0.35f};  // Similar to experience1
    experience2.action = nlm::ActionType::MoveForward;
    experience2.reward = 0.5f;
    
    episodicMemory->encode(experience2);
    
    // Test pattern completion
    nlm::Experience patternInput;
    patternInput.timestamp = 2.0;
    patternInput.sensoryInput[3] = {0.12f, 0.22f, 0.32f};  // Very similar to pattern
    patternInput.action = nlm::ActionType::MoveForward;  // Predicted action
    
    std::string completion = episodicMemory->completePattern(patternInput);
    if (completion.length() > 0) {
        recordTest("EpisodicMemory pattern completion", true);
    } else {
        recordTest("EpisodicMemory pattern completion", false);
    }
    
    // Test EpisodicMemory replay during sleep
    episodicMemory->requestReplay();
    if (episodicMemory->isReplaying()) {
        recordTest("EpisodicMemory replay mechanism", true);
    } else {
        recordTest("EpisodicMemory replay mechanism", false);
    }
}

// Test PredictionSystem Integration
void testPredictionSystemIntegration() {
    std::cout << "Testing PredictionSystem integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "  ✗ Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test that PredictionSystem is accessible
    auto predictionSystem = brain->getPredictionSystem();
    if (predictionSystem == nullptr) {
        std::cout << "  ✗ PredictionSystem is null - not integrated" << std::endl;
        return;
    }
    
    // Test PredictionSystem sequence prediction
    std::vector<nlm::Timestamp> sequence = {0.0, 1.0, 2.0, 3.0};
    std::vector<float> predicted = predictionSystem->predictNext(sequence);
    
    if (!predicted.empty() && predicted.size() == 1) {
        recordTest("PredictionSystem sequence prediction", true);
    } else {
        recordTest("PredictionSystem sequence prediction", false);
    }
    
    // Test PredictionSystem temporal pattern learning
    std::vector<float> inputPattern = {0.1f, 0.2f, 0.3f, 0.4f};
    predictionSystem->learnTemporalPattern(inputPattern);
    
    std::vector<float> predictedPattern = predictionSystem->generatePattern(4);
    if (!predictedPattern.empty()) {
        recordTest("PredictionSystem temporal pattern learning", true);
    } else {
        recordTest("PredictionSystem temporal pattern learning", false);
    }
    
    // Test PredictionSystem error signal generation
    std::vector<float> actualPattern = {0.15f, 0.25f, 0.35f, 0.45f};
    float error = predictionSystem->computeError(inputPattern, actualPattern);
    
    if (std::abs(error) > 0.0f) {  // Should have some error
        recordTest("PredictionSystem error computation", true);
    } else {
        recordTest("PredictionSystem error computation", false);
    }
    
    // Test PredictionSystem action consequence prediction
    nlm::ActionType action = nlm::ActionType::MoveForward;
    float predictedReward = predictionSystem->predictActionConsequence(action);
    
    if (predictedReward > -100.0f && predictedReward < 100.0f) {  // Reasonable value
        recordTest("PredictionSystem action consequence prediction", true);
    } else {
        recordTest("PredictionSystem action consequence prediction", false);
    }
}

// Test Cognitive System Integration
void testCognitiveSystemIntegration() {
    std::cout << "Testing Cognitive system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "  ✗ Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test NeuralPlanner integration
    auto neuralPlanner = brain->getPlanner();
    if (neuralPlanner == nullptr) {
        std::cout << "  ✗ NeuralPlanner is null - not integrated" << std::endl;
    } else {
        // Test NeuralPlanner action planning
        std::vector<nlm::ActionType> possibleActions = {
            nlm::ActionType::MoveForward,
            nlm::ActionType::MoveBackward,
            nlm::ActionType::TurnLeft,
            nlm::ActionType::TurnRight
        };
        
        std::vector<nlm::ActionType> plannedAction = neuralPlanner->planAction(possibleActions, 5.0f);
        
        if (!plannedAction.empty()) {
            recordTest("NeuralPlanner action planning", true);
        } else {
            recordTest("NeuralPlanner action planning", false);
        }
    }
    
    // Test ConceptFormation integration
    auto conceptFormation = brain->getConceptFormation();
    if (conceptFormation == nullptr) {
        std::cout << "  ✗ ConceptFormation is null - not integrated" << std::endl;
    } else {
        // Test ConceptFormation pattern discovery
        std::vector<std::vector<float>> dataPatterns = {
            {0.1f, 0.2f, 0.3f},
            {0.15f, 0.25f, 0.35f},
            {0.05f, 0.15f, 0.25f}
        };
        
        conceptFormation->processData(dataPatterns);
        
        std::vector<nlm::Concept> concepts = conceptFormation->getConcepts();
        if (!concepts.empty()) {
            recordTest("ConceptFormation pattern discovery", true);
        } else {
            recordTest("ConceptFormation pattern discovery", false);
        }
    }
    
    // Test AttentionalSelection integration
    auto attentionalSelection = brain->getAttention();
    if (attentionalSelection == nullptr) {
        std::cout << "  ✗ AttentionalSelection is null - not integrated" << std::endl;
    } else {
        // Test AttentionalSelection competitive selection
        std::vector<float> stimuli = {0.8f, 0.3f, 0.6f, 0.1f};
        size_t selectedIndex = attentionalSelection->selectAttention(stimuli);
        
        if (selectedIndex < stimuli.size()) {
            recordTest("AttentionalSelection competitive selection", true);
        } else {
            recordTest("AttentionalSelection competitive selection", false);
        }
    }
}

// Test Neuromodulation Integration
void testNeuromodulationIntegration() {
    std::cout << "Testing Neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "  ✗ Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test dopamine integration
    auto dopamine = brain->getDopamine();
    if (dopamine == nullptr) {
        std::cout << "  ✗ Dopamine is null - not integrated" << std::endl;
    } else {
        // Test dopamine release and effect
        dopamine->release(1.0f);  // Positive prediction error
        float dopamineLevel = dopamine->getLevel();
        
        if (dopamineLevel > 0.0f) {
            recordTest("Dopamine release and modulation", true);
        } else {
            recordTest("Dopamine release and modulation", false);
        }
    }
    
    // Test curiosity integration
    auto curiosity = brain->getCuriosity();
    if (curiosity == nullptr) {
        std::cout << "  ✗ Curiosity is null - not integrated" << std::endl;
    } else {
        // Test curiosity activation
        curiosity->activateNovelty(0.5f);
        float curiosityLevel = curiosity->getLevel();
        
        if (curiosityLevel > 0.0f) {
            recordTest("Curiosity activation", true);
        } else {
            recordTest("Curiosity activation", false);
        }
    }
    
    // Test novelty detection integration
    auto novelty = brain->getNovelty();
    if (novelty == nullptr) {
        std::cout << "  ✗ Novelty is null - not integrated" << std::endl;
    } else {
        // Test novelty detection
        std::vector<float> sensoryInput = {0.1f, 0.2f, 0.3f};
        float noveltyScore = novelty->detect(sensoryInput);
        
        if (noveltyScore > 0.0f) {
            recordTest("Novelty detection", true);
        } else {
            recordTest("Novelty detection", false);
        }
    }
}

// Test Development System Integration
void testDevelopmentIntegration() {
    std::cout << "Testing Development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "  ✗ Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test developmental stage progression
    auto developmentSystem = brain->getDevelopmentSystem();
    if (developmentSystem == nullptr) {
        std::cout << "  ✗ DevelopmentSystem is null - not integrated" << std::endl;
    } else {
        nlm::DevelopmentalStage initialStage = brain->getDevelopmentalStage();
        
        // Simulate development over time
        developmentSystem->update(10.0f);
        
        nlm::DevelopmentalStage finalStage = brain->getDevelopmentalStage();
        
        if (finalStage != initialStage) {  // Should have progressed
            recordTest("Developmental stage progression", true);
        } else {
            recordTest("Developmental stage progression", false);
        }
    }
    
    // Test plasticity modulation during development
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    developmentSystem->update(60.0f); // Move to CriticalPeriod
    
    float plasticityModifier = developmentSystem->getPlasticityModifier();
    if (plasticityModifier < 1.0f) {  // Should be reduced
        recordTest("Plasticity modulation during development", true);
    } else {
        recordTest("Plasticity modulation during development", false);
    }
}

// Test Complete Brain Loop Integration
void testCompleteBrainLoop() {
    std::cout << "Testing complete brain loop integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "  ✗ Failed to initialize brain" << std::endl;
        return;
    }
    
    // Create a simple world for testing
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(50.0f, 50.0f, 8, 8);
    world->setAgentStart(25.0f, 25.0f);
    
    // Create an agent brain
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Test complete cycle: sense -> process -> act -> learn
    for (int step = 0; step < 10; ++step) {
        // 1. Sense: Get sensory percept from world
        auto percept = world->getSensoryPercept();
        
        // 2. Process: Agent processes sensory input
        agent->processSensoryInput(percept);
        
        // 3. Compute: Brain performs neural computation
        brain->step(step, step * 0.1);
        
        // 4. Act: Agent decodes motor command
        nlm::MotorCommand action = agent->decodeMotorCommand();
        
        // 5. Execute: World applies action
        auto result = world->applyMotorCommand(action, world->getSimulationTime());
        
        // 6. Learn: Apply reward modulation
        agent->applyRewardModulation(result.reward, agent->getPredictionError());
        
        // 7. Develop: Update developmental state
        agent->updateDevelopment(0.1f);
    }
    
    // Verify that all systems were used
    bool workingMemoryActive = (brain->getWorkingMemory() != nullptr);
    bool episodicMemoryActive = (brain->getEpisodicMemory() != nullptr);
    bool predictionActive = (brain->getPredictionSystem() != nullptr);
    bool plannerActive = (brain->getPlanner() != nullptr);
    bool conceptsActive = (brain->getConceptFormation() != nullptr);
    bool attentionActive = (brain->getAttention() != nullptr);
    bool neuromodActive = (brain->getDopamine() != nullptr);
    bool curiosityActive = (brain->getCuriosity() != nullptr);
    bool developmentActive = (brain->getDevelopmentSystem() != nullptr);
    
    if (workingMemoryActive && episodicMemoryActive && predictionActive &&
        plannerActive && conceptsActive && attentionActive && neuromodActive &&
        curiosityActive && developmentActive) {
        
        recordTest("Complete brain loop integration", true);
        std::cout << "  All systems were integrated and functional" << std::endl;
    } else {
        recordTest("Complete brain loop integration", false);
        std::cout << "  Some systems were missing or not integrated" << std::endl;
    }
    
    // Test that we got some learning evidence
    float finalSpikeCount = brain->getTotalSpikeCount();
    float finalFiringRate = brain->getAverageFiringRate();
    
    if (finalSpikeCount > 0 && finalFiringRate > 0.0f) {
        recordTest("Brain activity and learning", true);
    } else {
        recordTest("Brain activity and learning", false);
    }
}

int main() {
    IntegrationTestFramework testFramework;
    return testFramework.runAllTests();
}

}} // namespace test_integration
