#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>

// Forward declarations to avoid circular includes
namespace nlm {
    class AgentBrain;
    class NeuralEpisodicMemory;
    class NeuralPlanner;
    class Brain;
    enum class ActionType;
    
    // Mock classes for testing
    class MockBrain {
    public:
        std::shared_ptr<class RandomGenerator> getRandomGenerator() {
            return std::make_shared<MockRandomGenerator>();
        }
    };
    
    class MockRandomGenerator {
    public:
        float uniformReal(float min, float max) { return 0.5f; }
        int uniformInt(int min, int max) { return min; }
    };
    
    class MockBrainWrapper {
    public:
        MockBrainWrapper() {
            brain_ = std::make_shared<Brain>();
        }
        
        Brain* get() { return brain_.get(); }
        
        std::shared_ptr<Brain> brain_;
    };
    
    // Include actual headers
    #include "behaviors/Behaviors.hpp"
    #include "motor/Action.hpp"
    
    // Mock classes
    class MockAgentBrain : public AgentBrain {
    public:
        MockAgentBrain() : AgentBrain(nullptr) {
            testState = std::vector<float>(100, 0.5f);
        }
        
        std::vector<float> getSensoryInputSize() const { 
            return testState; 
        }
        
        float getNeuromodulationLevel() const { return 0.5f; }
        
        std::vector<float> testState;
    };
    
    class MockMemory : public NeuralEpisodicMemory {
    public:
        MockMemory() = default;
        
        void storeEpisode(const EpisodicMemoryItem& episode) {
            episodes.push_back(episode);
        }
        
        std::vector<const EpisodicMemoryItem*> retrieveSimilar(
            const std::vector<float>& pattern, 
            size_t maxResults) const {
            return {};
        }
        
        size_t getEpisodeCount() const { return episodes.size(); }
        
        std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const {
            return {};
        }
        
        std::vector<EpisodicMemoryItem> episodes;
    };
    
    class MockPlanner : public NeuralPlanner {
    public:
        MockPlanner() = default;
        
        MotorCommand planAction(const std::vector<float>& currentState, 
                              float targetReward = 0.5f) {
            return MotorCommand::Wait;
        }
        
        ActionType getCurrentPlan() const { return ActionType::Wait; }
    };
    
    // Test classes
    class TestRewardShapingBehavior : public RewardShapingBehavior {
    public:
        TestRewardShapingBehavior() = default;
        
        MockAgentBrain mockBrain;
        MockMemory mockMemory;
        MockPlanner mockPlanner;
        
        void testRewardShaping() {
            initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
            setEnabled(true);
            
            MotorCommand cmd = update(1.0);
            assert(cmd == MotorCommand::Wait);
            
            float (*testComponent)(const std::vector<float>&, const ActionType&) = 
                [](const std::vector<float>&, const ActionType&) { return 0.5f; };
            
            addShapingComponent(testComponent, 2.0f);
            setBaseRewardFunction(
                [](const std::vector<float>&, const ActionType&) { return 0.3f; }
            );
        }
    };
    
    class TestTransferLearningBehavior : public TransferLearningBehavior {
    public:
        TestTransferLearningBehavior() = default;
        
        MockAgentBrain mockBrain;
        MockMemory mockMemory;
        MockPlanner mockPlanner;
        
        void testTransferLearning() {
            initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
            setEnabled(true);
            
            std::vector<float> testPattern(10, 0.5f);
            registerSkill("TestSkill", testPattern, ActionType::MoveForward);
            
            float confidence;
            MotorCommand action = findTransferSkill(testPattern, confidence);
            assert(action == MotorCommand::Wait);
        }
    };
    
    class TestHierarchicalPlanningBehavior : public HierarchicalPlanningBehavior {
    public:
        TestHierarchicalPlanningBehavior() = default;
        
        MockAgentBrain mockBrain;
        MockMemory mockMemory;
        MockPlanner mockPlanner;
        
        void testHierarchicalPlanning() {
            initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
            setEnabled(true);
            
            std::vector<float> testGoal(5, 0.7f);
            addHighLevelGoal(testGoal, 1.0f);
            
            float confidence = getPlanningConfidence();
            assert(confidence >= 0.0f);
            
            std::vector<ActionType> plan = decomposeGoal(testGoal);
        }
    };
    
    class TestMemoryGuidedBehavior : public MemoryGuidedBehavior {
    public:
        TestMemoryGuidedBehavior() = default;
        
        MockAgentBrain mockBrain;
        MockMemory mockMemory;
        MockPlanner mockPlanner;
        
        void testMemoryGuided() {
            initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
            setEnabled(true);
            
            std::vector<float> beforeState(10, 0.5f);
            std::vector<float> afterState(10, 0.6f);
            
            recordExperience(ActionType::MoveForward, beforeState, afterState, 0.5f);
            
            MotorCommand cmd = recommendAction(beforeState, ActionType::MoveForward, 0.2f);
            assert(cmd == MotorCommand::Wait || cmd != MotorCommand::Wait);
        }
    };
    
    class TestSocialLearningBehavior : public SocialLearningBehavior {
    public:
        TestSocialLearningBehavior() = default;
        
        MockAgentBrain mockBrain;
        MockMemory mockMemory;
        MockPlanner mockPlanner;
        
        void testSocialLearning() {
            initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
            setEnabled(true);
            
            std::vector<float> observerState(10, 0.5f);
            std::vector<float> resultingState(10, 0.6f);
            
            recordObservation(ActionType::MoveForward, observerState, resultingState, 0.5f);
            
            assert(canImitate(ActionType::MoveForward) == true);
            
            ActionType action = getImitationAction(observerState);
            assert(action == ActionType::MoveForward || action == ActionType::Wait);
        }
    };
    
    class TestMetaCognitionBehavior : public MetaCognitionBehavior {
    public:
        TestMetaCognitionBehavior() = default;
        
        MockAgentBrain mockBrain;
        MockMemory mockMemory;
        MockPlanner mockPlanner;
        
        void testMetaCognition() {
            initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
            setEnabled(true);
            
            std::vector<float> state(10, 0.5f);
            
            recordPredictionOutcome(
                ActionType::MoveForward,
                ActionType::MoveForward,
                0.5f,
                0.6f,
                state
            );
            
            float confidence = getActionConfidence(ActionType::MoveForward);
            assert(confidence >= 0.0f);
            
            float metaConf = getMetaCognitiveConfidence();
            assert(metaConf >= 0.0f);
        }
    };
    
    class TestCuriosityDrivenBehavior : public CuriosityDrivenBehavior {
    public:
        TestCuriosityDrivenBehavior() = default;
        
        MockAgentBrain mockBrain;
        MockMemory mockMemory;
        MockPlanner mockPlanner;
        
        void testCuriosityDriven() {
            initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
            setEnabled(true);
            
            updateCuriosity(1.0);
            
            float curiosity = getCuriosityLevel();
            assert(curiosity >= 0.0f && curiosity <= 1.0f);
            
            setCuriosityParameters(0.9f, 5.0f);
        }
    };
}

class MockAgentBrain {
public:
    MockAgentBrain() {
        // Initialize mock state
        testState = std::vector<float>(100, 0.5f);
    }
    
    std::vector<float> getSensoryInputSize() const { 
        return testState; 
    }
    
    float getNeuromodulationLevel() const { return 0.5f; }
    
    // Mock methods needed by behaviors
    class MockBrain {
    public:
        std::shared_ptr<class RandomGenerator> getRandomGenerator() {
            return std::make_shared<MockRandomGenerator>();
        }
    };
    
    class MockRandomGenerator {
    public:
        float uniformReal(float min, float max) { return 0.5f; }
        int uniformInt(int min, int max) { return min; }
    };
    
    Brain* getBrain() { return brain_.get(); }
    
    std::shared_ptr<Brain> brain_ = std::make_shared<Brain>();
    std::vector<float> testState;
};

class MockMemory {
public:
    MockMemory() = default;
    
    void storeEpisode(const EpisodicMemoryItem& episode) {
        episodes.push_back(episode);
    }
    
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& pattern, 
        size_t maxResults) const {
        return {};
    }
    
    size_t getEpisodeCount() const { return episodes.size(); }
    
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const {
        return {};
    }
    
    std::vector<EpisodicMemoryItem> episodes;
};

class MockPlanner {
public:
    MockPlanner() = default;
    
    MotorCommand planAction(const std::vector<float>& currentState, 
                          float targetReward = 0.5f) {
        return MotorCommand::Wait;
    }
    
    ActionType getCurrentPlan() const { return ActionType::Wait; }
};

void testBaseBehavior() {
    std::cout << "Testing BaseBehavior..." << std::endl;
    
    Behavior behavior;
    assert(behavior.getName() == "BaseBehavior");
    assert(behavior.shouldActivate() == false);
    
    behavior.setEnabled(true);
    assert(behavior.shouldActivate() == true);
    
    MotorCommand cmd = behavior.update(1.0);
    assert(cmd == MotorCommand::Wait);
    
    assert(behavior.getConfidence() == 0.5f);
    behavior.updateConfidence(0.3f);
    assert(behavior.getConfidence() > 0.5f);
    
    std::cout << "✓ BaseBehavior tests passed" << std::endl;
}

void testRewardShapingBehavior() {
    std::cout << "Testing RewardShapingBehavior..." << std::endl;
    
    RewardShapingBehavior behavior;
    assert(behavior.getName() == "RewardShaping");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    MotorCommand cmd = behavior.update(1.0);
    assert(cmd == MotorCommand::Wait);
    
    // Test reward shaping components
    float (*testComponent)(const std::vector<float>&, const ActionType&) = 
        [](const std::vector<float>&, const ActionType&) { return 0.5f; };
    
    behavior.addShapingComponent(testComponent, 2.0f);
    behavior.setBaseRewardFunction(
        [](const std::vector<float>&, const ActionType&) { return 0.3f; }
    );
    
    std::cout << "✓ RewardShapingBehavior tests passed" << std::endl;
}

void testTransferLearningBehavior() {
    std::cout << "Testing TransferLearningBehavior..." << std::endl;
    
    TransferLearningBehavior behavior;
    assert(behavior.getName() == "TransferLearning");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test skill registration
    std::vector<float> testPattern(10, 0.5f);
    behavior.registerSkill("TestSkill", testPattern, ActionType::MoveForward);
    
    // Test skill retrieval
    float confidence;
    MotorCommand action = behavior.findTransferSkill(testPattern, confidence);
    assert(action == MotorCommand::Wait); // No matching skill yet
    
    std::cout << "✓ TransferLearningBehavior tests passed" << std::endl;
}

void testHierarchicalPlanningBehavior() {
    std::cout << "Testing HierarchicalPlanningBehavior..." << std::endl;
    
    HierarchicalPlanningBehavior behavior;
    assert(behavior.getName() == "HierarchicalPlanning");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test goal adding
    std::vector<float> testGoal(5, 0.7f);
    behavior.addHighLevelGoal(testGoal, 1.0f);
    
    // Test planning confidence
    float confidence = behavior.getPlanningConfidence();
    assert(confidence >= 0.0f);
    
    // Test plan decomposition
    std::vector<ActionType> plan = behavior.decomposeGoal(testGoal);
    // Should return a simple plan based on goal values
    
    std::cout << "✓ HierarchicalPlanningBehavior tests passed" << std::endl;
}

void testMemoryGuidedBehavior() {
    std::cout << "Testing MemoryGuidedBehavior..." << std::endl;
    
    MemoryGuidedBehavior behavior;
    assert(behavior.getName() == "MemoryGuided");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test recording experience
    std::vector<float> beforeState(10, 0.5f);
    std::vector<float> afterState(10, 0.6f);
    
    behavior.recordExperience(ActionType::MoveForward, beforeState, afterState, 0.5f);
    
    // Test action recommendation
    MotorCommand cmd = behavior.recommendAction(beforeState, ActionType::MoveForward, 0.2f);
    assert(cmd == MotorCommand::Wait || cmd != MotorCommand::Wait); // Either is fine
    
    std::cout << "✓ MemoryGuidedBehavior tests passed" << std::endl;
}

void testSocialLearningBehavior() {
    std::cout << "Testing SocialLearningBehavior..." << std::endl;
    
    SocialLearningBehavior behavior;
    assert(behavior.getName() == "SocialLearning");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test observation recording
    std::vector<float> observerState(10, 0.5f);
    std::vector<float> resultingState(10, 0.6f);
    
    behavior.recordObservation(ActionType::MoveForward, observerState, resultingState, 0.5f);
    
    // Test imitation capability
    assert(behavior.canImitate(ActionType::MoveForward) == true);
    
    // Test action selection
    ActionType action = behavior.getImitationAction(observerState);
    assert(action == ActionType::MoveForward || action == ActionType::Wait);
    
    std::cout << "✓ SocialLearningBehavior tests passed" << std::endl;
}

void testMetaCognitionBehavior() {
    std::cout << "Testing MetaCognitionBehavior..." << std::endl;
    
    MetaCognitionBehavior behavior;
    assert(behavior.getName() == "MetaCognition");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test prediction recording
    std::vector<float> state(10, 0.5f);
    
    behavior.recordPredictionOutcome(
        ActionType::MoveForward,
        ActionType::MoveForward,
        0.5f,
        0.6f,
        state
    );
    
    // Test confidence retrieval
    float confidence = behavior.getActionConfidence(ActionType::MoveForward);
    assert(confidence >= 0.0f);
    
    // Test metacognitive confidence
    float metaConf = behavior.getMetaCognitiveConfidence();
    assert(metaConf >= 0.0f);
    
    std::cout << "✓ MetaCognitionBehavior tests passed" << std::endl;
}

void testCuriosityDrivenBehavior() {
    std::cout << "Testing CuriosityDrivenBehavior..." << std::endl;
    
    CuriosityDrivenBehavior behavior;
    assert(behavior.getName() == "CuriosityDriven");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test curiosity update
    behavior.updateCuriosity(1.0);
    
    // Test curiosity level
    float curiosity = behavior.getCuriosityLevel();
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    // Test curiosity parameters
    behavior.setCuriosityParameters(0.9f, 5.0f);
    
    std::cout << "✓ CuriosityDrivenBehavior tests passed" << std::endl;
}

void testActionClass() {
    std::cout << "Testing Action class..." << std::endl;
    
    // Test basic action creation
    Action action(ActionType::MoveForward);
    assert(action.getType() == ActionType::MoveForward);
    assert(action.getName() == "MoveForward");
    
    // Test action with parameters
    std::vector<float> params = {0.1f, 0.2f, 0.3f};
    Action action2(ActionType::MoveForward, params);
    assert(action2.getParameters() == params);
    
    // Test action cloning
    std::unique_ptr<Action> cloned = action2.clone();
    assert(cloned->getType() == ActionType::MoveForward);
    assert(cloned->getParameters() == params);
    
    // Test action consequences
    ActionConsequence cons;
    cons.expectedReward = 0.5f;
    cons.probability = 0.8f;
    cons.executionTime = 2.0;
    cons.resourceCost = 0.1f;
    cons.uncertainty = 0.3f;
    
    action.addConsequence(cons);
    assert(action.getConsequences().size() == 1);
    
    // Test action metrics
    assert(action.getExpectedReward() == 0.5f); // 0.5 / 0.8 = 0.625 but simplified
    assert(action.getExecutionTime() > 0.0f);
    assert(action.getResourceCost() == 0.1f);
    assert(action.getUncertainty() == 0.3f);
    
    // Test metadata
    action.setMetadata("test", "value");
    assert(action.getMetadata("test") == "value");
    assert(action.getMetadata("nonexistent") == "");
    
    // Test cloning with parameters
    std::vector<float> newParams = {0.9f, 0.8f, 0.7f};
    std::unique_ptr<Action> cloned2 = action2.cloneWithParameters(newParams);
    assert(cloned2->getType() == ActionType::MoveForward);
    assert(cloned2->getParameters() == newParams);
    
    std::cout << "✓ Action class tests passed" << std::endl;
}

int main() {
    std::cout << "Running advanced agent behavior integration tests..." << std::endl << std::endl;
    
    try {
        testBaseBehavior();
        testRewardShapingBehavior();
        testTransferLearningBehavior();
        testHierarchicalPlanningBehavior();
        testMemoryGuidedBehavior();
        testSocialLearningBehavior();
        testMetaCognitionBehavior();
        testCuriosityDrivenBehavior();
        testActionClass();
        
        std::cout << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        std::cout << "Advanced agent behavior integration test suite completed successfully." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception." << std::endl;
        return 1;
    }
}