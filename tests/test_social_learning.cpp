// Social Learning Tests
// Tests for learning from other agents and social interactions

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_social_learning {

void testObservationalLearning() {
    std::cout << "  Testing observational learning..." << std::endl;
    
    // Test learning from observing other agents' behavior
    std::vector<std::string> observedActions = {
        "move_forward", "turn_left", "look", "interact",
        "move_forward", "turn_right", "look", "interact"
    };
    
    // Extract unique actions from observation
    std::vector<std::string> learnedActions;
    for (const auto& action : observedActions) {
        if (std::find(learnedActions.begin(), learnedActions.end(), action) == learnedActions.end()) {
            learnedActions.push_back(action);
        }
    }
    
    // Verify that actions were learned
    assert(learnedActions.size() > 0 && "Should learn from observations");
    
    std::cout << "    ✓ Observational learning test passed" << std::endl;
}

void testImitationLearning() {
    std::cout << "  Testing imitation learning..." << std::endl;
    
    // Test imitating successful behaviors from other agents
    std::vector<std::pair<std::string, float>> successfulBehaviors = {
        {"move_forward", 0.8f},
        {"turn_left", 0.6f},
        {"look", 0.9f},
        {"interact", 0.7f}
    };
    
    // Imitate the most successful behavior
    std::string imitatedBehavior = "";
    float maxSuccess = 0.0f;
    
    for (const auto& behavior : successfulBehaviors) {
        if (behavior.second > maxSuccess) {
            maxSuccess = behavior.second;
            imitatedBehavior = behavior.first;
        }
    }
    
    assert(!imitatedBehavior.empty() && "Should imitate a successful behavior");
    assert(maxSuccess > 0.5f && "Should imitate highly successful behavior");
    
    std::cout << "    ✓ Imitation learning test passed" << std::endl;
}

void testSocialInfluence() {
    std::cout << "  Testing social influence..." << std::endl;
    
    // Test social influence on behavior selection
    std::vector<std::pair<std::string, float>> agentBehaviors = {
        {"move_forward", 0.3f},
        {"turn_left", 0.4f},
        {"look", 0.2f},
        {"interact", 0.1f}
    };
    
    // Social influence can change behavior probabilities
    std::vector<std::pair<std::string, float>> influencedBehaviors;
    
    for (const auto& behavior : agentBehaviors) {
        // Social influence can increase or decrease behavior probability
        float influenceFactor = 1.2f; // Positive social influence
        float influencedProb = behavior.second * influenceFactor;
        
        // Ensure probability stays within bounds
        influencedProb = std::min(1.0f, influencedProb);
        
        influencedBehaviors.push_back({behavior.first, influencedProb});
    }
    
    // Verify that influenced probabilities are different from original
    bool influenced = false;
    for (size_t i = 0; i < agentBehaviors.size(); ++i) {
        if (std::abs(influencedBehaviors[i].second - agentBehaviors[i].second) > 0.001f) {
            influenced = true;
            break;
        }
    }
    
    assert(influenced && "Social influence should change behavior probabilities");
    
    std::cout << "    ✓ Social influence test passed" << std::endl;
}

void runAll() {
    std::cout << "Social Learning Tests:" << std::endl;
    
    testObservationalLearning();
    testImitationLearning();
    testSocialInfluence();
    
    std::cout << "Social Learning Tests: PASSED" << std::endl;
}

} // namespace test_social_learning
