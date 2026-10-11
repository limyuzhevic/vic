// NLM Unit Testing Framework
// Cognitive Tests

#include "framework/TestFramework.hpp"
#include "brain/CognitiveSystem.hpp"
#include "brain/Attention.hpp"
#include "brain/MemorySystem.hpp"
#include <cassert>
#include <cmath>

class TestCognition : public TestCase {
public:
    TestCognition() : TestCase("Cognition") {}
    
    void testAttentionMechanism() {
        Attention attention;
        
        // Test feature-based attention
        std::vector<double> features = {0.8, 0.3, 0.9, 0.1, 0.5};
        double attention_weight = attention.computeAttention(features);
        
        assert(attention_weight >= 0.0 && attention_weight <= 1.0);
        assert(attention_weight > features[0]); // Should boost most salient feature
    }
    
    void testWorkingMemory() {
        CognitiveSystem cognition;
        
        // Test working memory capacity
        WorkingMemory& wm = cognition.getWorkingMemory();
        
        // Store items in working memory
        for (int i = 0; i < 7; ++i) { // Miller's limit ~7 items
            wm.store(std::to_string(i), i * 0.1);
        }
        
        assert(wm.getCapacity() >= 7);
        
        // Test memory decay in working memory
        double strength_before = wm.getStrength(std::to_string(0));
        wm.advanceTime(1.0);
        double strength_after = wm.getStrength(std::to_string(0));
        
        assert(strength_after < strength_before); // Decay over time
    }
    
    void testDecisionMaking() {
        CognitiveSystem cognition;
        
        // Test value-based decision making
        std::vector<std::pair<std::string, double>> options = {
            {"option1", 0.3},
            {"option2", 0.7},
            {"option3", 0.5}
        };
        
        std::string chosen = cognition.chooseAction(options);
        assert(!chosen.empty());
        
        // Should choose option with highest value
        assert(chosen == "option2"); // Highest value
    }
    
    void testLearningAndAdaptation() {
        CognitiveSystem cognition;
        
        // Test reinforcement learning
        double reward = 1.0;
        double value_estimate_before = cognition.getValueEstimate("action");
        
        cognition.updateValue("action", reward, 0.1); // Learning rate
        
        double value_estimate_after = cognition.getValueEstimate("action");
        assert(value_estimate_after > value_estimate_before);
        
        // Test policy improvement
        Policy old_policy = cognition.getPolicy("state");
        cognition.improvePolicy("state", "action", 0.05);
        Policy new_policy = cognition.getPolicy("state");
        
        assert(new_policy.action_value >= old_policy.action_value);
    }
    
    void testCognitiveLoad() {
        CognitiveSystem cognition;
        
        // Test cognitive load management
        double load_before = cognition.getCognitiveLoad();
        
        // Perform multiple cognitive tasks
        cognition.processInformation(0.5);
        cognition.makeDecision(0.3);
        cognitionlearn(0.8);
        
        double load_after = cognition.getCognitiveLoad();
        assert(load_after >= load_before);
        
        // Test load recovery
        cognition.rest(0.5);
        double load_recovered = cognition.getCognitiveLoad();
        assert(load_recovered <= load_after);
    }
};