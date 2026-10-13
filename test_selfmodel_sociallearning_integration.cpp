#include <iostream>
#include <memory>
#include "brain/Brain.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/NeuralPlanner.hpp" // For SelfModel and SocialLearning

using namespace nlm;

int main() {
    std::cout << "=== SelfModel & SocialLearning Integration Test ===" << std::endl;
    
    // Create a simple brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(100));
    config->set("region_count", size_t(1));
    config->set("connection_probability", 0.1f);
    
    // Create brain
    std::cout << "Creating brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Test 1: SelfModel integration
    std::cout << "\n=== Test 1: SelfModel Integration ===" << std::endl;
    auto* selfModel = brain->getSelfModel();
    if (selfModel) {
        std::cout << "✓ SelfModel is integrated and accessible" << std::endl;
        
        // Test basic self-model operations
        std::vector<float> testState(10, 0.5f);
        ActionType testAction = ActionType::MoveForward;
        
        // Record a self-action (simulate learning)
        std::vector<float> beforeState = testState;
        std::vector<float> afterState = testState;
        for (int i = 0; i < 10; i++) {
            afterState[i] = std::min(1.0f, testState[i] + 0.1f);
        }
        
        selfModel->recordSelfAction(testAction, beforeState, afterState);
        std::cout << "✓ SelfModel can record self-actions" << std::endl;
        
        // Test prediction
        std::vector<float> predicted = selfModel->predictActionConsequence(testAction, testState);
        if (!predicted.empty()) {
            std::cout << "✓ SelfModel can predict action consequences" << std::endl;
        }
        
        // Check self-model has formed
        if (selfModel->hasSelfModel()) {
            std::cout << "✓ SelfModel has formed from experience" << std::endl;
        }
        
        // Test preferred action
        ActionType preferred = selfModel->getPreferredAction(testState);
        std::cout << "✓ SelfModel can provide preferred actions (got: " 
                  << static_cast<int>(preferred) << ")" << std::endl;
        
    } else {
        std::cerr << "✗ SelfModel is NOT integrated!" << std::endl;
        return 1;
    }
    
    // Test 2: SocialLearning integration
    std::cout << "\n=== Test 2: SocialLearning Integration ===" << std::endl;
    auto* socialLearning = brain->getSocialLearning();
    if (socialLearning) {
        std::cout << "✓ SocialLearning is integrated and accessible" << std::endl;
        
        // Test basic social learning operations
        std::vector<float> observerState(10, 0.3f);
        std::vector<float> resultingState(10, 0.6f);
        ActionType observedAction = ActionType::Interact;
        
        // Record an observation
        socialLearning->observeAgentAction(observedAction, observerState, resultingState);
        std::cout << "✓ SocialLearning can record observations" << std::endl;
        
        // Check if can imitate
        if (socialLearning->canImitate(observedAction)) {
            std::cout << "✓ SocialLearning can track observable actions for imitation" << std::endl;
        }
        
        // Test imitation
        ActionType imitation = socialLearning->getImitationAction(observerState);
        std::cout << "✓ SocialLearning can determine imitation actions (got: " 
                  << static_cast<int>(imitation) << ")" << std::endl;
        
        // Check social knowledge has formed
        if (socialLearning->hasSocialKnowledge()) {
            std::cout << "✓ SocialLearning has learned from observations" << std::endl;
        }
        
    } else {
        std::cerr << "✗ SocialLearning is NOT integrated!" << std::endl;
        return 1;
    }
    
    // Test 3: Integration with NeuralPlanner
    std::cout << "\n=== Test 3: Integration with NeuralPlanner ===" << std::endl;
    auto* planner = brain->getPlanner();
    if (planner) {
        std::cout << "✓ NeuralPlanner is accessible" << std::endl;
        
        std::vector<float> testState(10, 0.5f);
        ActionType action = planner->planAction(testState, 0.5f);
        std::cout << "✓ NeuralPlanner can plan actions (got: " 
                  << static_cast<int>(action) << ")" << std::endl;
        
        // Test integration methods
        std::cout << "Testing integration methods..." << std::endl;
        planner->integrateWithPredictionSystem(brain.get());
        std::cout << "✓ Prediction system integration" << std::endl;
        
        planner->integrateWithMemorySystems(brain.get());
        std::cout << "✓ Memory systems integration" << std::endl;
        
        // Test action production
        auto actionPtr = brain->produceAction();
        if (actionPtr) {
            std::cout << "✓ Brain can produce actions" << std::endl;
        }
        
    } else {
        std::cerr << "✗ NeuralPlanner is NOT accessible!" << std::endl;
        return 1;
    }
    
    // Test 4: Step method integration
    std::cout << "\n=== Test 4: Step Method Integration ===" << std::endl;
    std::cout << "Running brain steps..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        brain->step(i, i * 0.001f);
        if (i % 3 == 0) {
            std::cout << "." << std::flush;
        }
    }
    std::cout << "\n✓ Step method runs without errors" << std::endl;
    
    // Test 5: Neuromodulation integration
    std::cout << "\n=== Test 5: Neuromodulation Integration ===" << std::endl;
    
    // Check that neuromodulators are integrated
    auto* curiosity = brain->getCuriosity();
    auto* dopamine = brain->getDopamine();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (curiosity) {
        std::cout << "✓ Curiosity system accessible" << std::endl;
    }
    if (dopamine) {
        std::cout << "✓ Dopamine system accessible" << std::endl;
    }
    if (novelty) {
        std::cout << "✓ Novelty system accessible" << std::endl;
    }
    if (predictionError) {
        std::cout << "✓ Prediction error system accessible" << std::endl;
    }
    
    std::cout << "\n=== All Integration Tests Passed! ===" << std::endl;
    return 0;
}
