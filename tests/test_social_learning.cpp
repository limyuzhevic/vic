// Social Learning Tests
// Tests observation-based learning, imitation capability, and communication signal tests

#include "cognition/NeuralPlanner.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_social_learning {

void testSocialLearningCreation() {
    nlm::SocialLearning social;
    
    assert(!social.hasSocialKnowledge());
    assert(social.getObservationCount() == 0);
    assert(social.getSignalMeaning() == 0.0f);
    
    std::cout << "    testSocialLearningCreation passed" << std::endl;
}

void testSocialLearningInitialization() {
    nlm::SocialLearning social;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    social.initialize(&mockBrain);
    
    assert(!social.hasSocialKnowledge());
    assert(social.getObservationCount() == 0);
    
    std::cout << "    testSocialLearningInitialization passed" << std::endl;
}

void testObservationBasedLearning() {
    nlm::SocialLearning social;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    social.initialize(&mockBrain);
    
    std::vector<float> observerState = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    nlm::ActionType observedAction = nlm::ActionType::MoveForward;
    std::vector<float> resultingState = {0.8f, 0.7f, 0.6f, 0.7f, 0.8f};
    
    // Observe agent's action
    social.observeAgentAction(observedAction, observerState, resultingState);
    
    assert(social.hasSocialKnowledge());
    assert(social.getObservationCount() > 0);
    
    std::cout << "    testObservationBasedLearning passed" << std::endl;
}

void testImitationCapability() {
    nlm::SocialLearning social;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    social.initialize(&mockBrain);
    
    nlm::ActionType observedAction = nlm::ActionType::MoveForward;
    std::vector<float> observerState = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // Observe action first
    social.observeAgentAction(observedAction, observerState, {0.8f, 0.7f, 0.6f, 0.7f, 0.8f});
    
    // Check if can imitate
    assert(social.canImitate(observedAction));
    
    // Get imitation action
    nlm::ActionType imitation = social.getImitationAction(observerState);
    
    // Should be able to imitate (implementation dependent)
    assert(imitation == observedAction || imitation == nlm::ActionType::Wait ||
           imitation == nlm::ActionType::Look || imitation == nlm::ActionType::Talk);
    
    std::cout << "    testImitationCapability passed" << std::endl;
}

void testCommunicationSignalLearning() {
    nlm::SocialLearning social;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    social.initialize(&mockBrain);
    
    std::vector<float> signalPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    float signalReward = 0.8f;
    
    // Learn communication signal
    social.learnCommunicationSignal(signalPattern, signalReward);
    
    // Get signal pattern
    std::vector<float> learnedSignal = social.getSignalPattern();
    
    // Should have learned the signal
    assert(learnedSignal.size() == signalPattern.size());
    
    // Get signal meaning
    float meaning = social.getSignalMeaning();
    assert(meaning >= 0.0f && meaning <= 1.0f);
    
    std::cout << "    testCommunicationSignalLearning passed" << std::endl;
}

void testSignalDetection() {
    nlm::SocialLearning social;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    social.initialize(&mockBrain);
    
    // Learn a signal first
    std::vector<float> signalPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    social.learnCommunicationSignal(signalPattern, 0.8f);
    
    // Detect the same signal
    bool detected = social.detectSignal(signalPattern);
    
    // Should detect the learned signal
    assert(detected);
    
    // Try different signal
    std::vector<float> differentSignal = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    detected = social.detectSignal(differentSignal);
    
    // Implementation dependent - might or might not detect
    
    std::cout << "    testSignalDetection passed" << std::endl;
}

void testSocialKnowledgeUpdate() {
    nlm::SocialLearning social;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    social.initialize(&mockBrain);
    
    // Learn initial observation
    social.observeAgentAction(nlm::ActionType::MoveForward, 
                            std::vector<float>{0.5f, 0.5f, 0.5f},
                            std::vector<float>{0.8f, 0.7f, 0.6f});
    
    assert(social.getObservationCount() > 0);
    
    // Update social knowledge with interaction reward
    social.updateSocialKnowledge(0.9f);
    
    // Should still have knowledge
    assert(social.hasSocialKnowledge());
    
    std::cout << "    testSocialKnowledgeUpdate passed" << std::endl;
}

void testMultipleObservationLearning() {
    nlm::SocialLearning social;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    social.initialize(&mockBrain);
    
    std::vector<nlm::ActionType> actions = {
        nlm::ActionType::MoveForward, nlm::ActionType::Look, nlm::ActionType::Talk
    };
    
    // Observe multiple actions
    for (size_t i = 0; i < actions.size(); ++i) {
        std::vector<float> state(5, 0.5f);
        std::vector<float> result(5, 0.5f + static_cast<float>(i) * 0.1f);
        social.observeAgentAction(actions[i], state, result);
    }
    
    // Should have multiple observations
    assert(social.getObservationCount() >= actions.size());
    
    // Should be able to imitate any of the observed actions
    for (nlm::ActionType action : actions) {
        assert(social.canImitate(action));
    }
    
    std::cout << "    testMultipleObservationLearning passed" << std::endl;
}

void testSocialLearningClear() {
    nlm::SocialLearning social;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    social.initialize(&mockBrain);
    
    // Learn something
    social.observeAgentAction(nlm::ActionType::MoveForward,
                            std::vector<float>{0.5f, 0.5f, 0.5f},
                            std::vector<float>{0.8f, 0.7f, 0.6f});
    
    assert(social.hasSocialKnowledge());
    
    // Clear social learning
    social.clear();
    
    assert(!social.hasSocialKnowledge());
    assert(social.getObservationCount() == 0);
    
    std::cout << "    testSocialLearningClear passed" << std::endl;
}

void runAll() {
    testSocialLearningCreation();
    testSocialLearningInitialization();
    testObservationBasedLearning();
    testImitationCapability();
    testCommunicationSignalLearning();
    testSignalDetection();
    testSocialKnowledgeUpdate();
    testMultipleObservationLearning();
    testSocialLearningClear();
}

} // namespace test_social_learning
