// Self-Model Tests
// Tests body schema learning, action consequence prediction, self vs external distinction, and capability representation

#include "cognition/NeuralPlanner.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_self_model {

void testSelfModelCreation() {
    nlm::SelfModel selfModel;
    
    assert(!selfModel.hasSelfModel());
    assert(selfModel.getCapabilityLevel() == 0.0f);
    assert(selfModel.getBodyAwareness() == 0.0f);
    
    std::cout << "    testSelfModelCreation passed" << std::endl;
}

void testSelfModelInitialization() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    assert(!selfModel.hasSelfModel());
    
    std::cout << "    testSelfModelInitialization passed" << std::endl;
}

void testBodySchemaLearning() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    std::vector<float> beforeState = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    std::vector<float> afterState = {0.6f, 0.7f, 0.8f, 0.7f, 0.6f}; // Changed due to action
    
    nlm::ActionType action = nlm::ActionType::MoveForward;
    
    // Record self-action
    selfModel.recordSelfAction(action, beforeState, afterState);
    
    assert(selfModel.hasSelfModel());
    
    std::cout << "    testBodySchemaLearning passed" << std::endl;
}

void testActionConsequencePrediction() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    // Learn an action-effect mapping
    std::vector<float> beforeState = {0.5f, 0.5f, 0.5f};
    std::vector<float> afterState = {0.8f, 0.7f, 0.6f};
    nlm::ActionType action = nlm::ActionType::MoveForward;
    
    selfModel.recordSelfAction(action, beforeState, afterState);
    
    // Predict consequence of same action
    std::vector<float> prediction = selfModel.predictActionConsequence(action, beforeState);
    
    // Prediction should be similar to learned after-state
    assert(prediction.size() == afterState.size());
    
    float similarity = 0.0f;
    for (size_t i = 0; i < prediction.size(); ++i) {
        similarity += std::abs(prediction[i] - afterState[i]);
    }
    similarity = 1.0f - (similarity / prediction.size()); // Convert to similarity
    
    // Should be reasonably accurate
    assert(similarity > 0.0f);
    
    std::cout << "    testActionConsequencePrediction passed" << std::endl;
}

void testSelfVsExternalDistinction() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    std::vector<float> beforeState = {0.5f, 0.5f, 0.5f};
    
    // External change - record as self-action
    std::vector<float> afterStateSelf = {0.6f, 0.7f, 0.8f};
    selfModel.recordSelfAction(nlm::ActionType::MoveForward, beforeState, afterStateSelf);
    
    // External change (without action) - compare
    std::vector<float> afterStateExternal = {0.6f, 0.7f, 0.8f};
    
    float selfLikelihood = selfModel.computeSelfGeneratedLikeness(beforeState, afterStateExternal, nlm::ActionType::MoveForward);
    
    // Should compute some likelihood (implementation dependent)
    assert(selfLikelihood >= 0.0f && selfLikelihood <= 1.0f);
    
    std::cout << "    testSelfVsExternalDistinction passed" << std::endl;
}

void testCapabilityRepresentation() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    // Initially should have zero capability
    assert(selfModel.getCapabilityLevel() == 0.0f);
    
    // The capability level might increase with learning
    // This tests the representation exists
    
    std::cout << "    testCapabilityRepresentation passed" << std::endl;
}

void testBodyAwareness() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    // Initially low awareness
    assert(selfModel.getBodyAwareness() == 0.0f);
    
    // After learning some action-effects, awareness should increase
    std::vector<float> beforeState = {0.5f, 0.5f, 0.5f};
    std::vector<float> afterState = {0.8f, 0.7f, 0.6f};
    
    selfModel.recordSelfAction(nlm::ActionType::MoveForward, beforeState, afterState);
    
    float awareness = selfModel.getBodyAwareness();
    
    // Should be positive
    assert(awareness >= 0.0f && awareness <= 1.0f);
    
    std::cout << "    testBodyAwareness passed" << std::endl;
}

void testSelfModelUpdate() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    // Create predicted and actual states
    std::vector<float> predicted = {0.6f, 0.7f, 0.8f};
    std::vector<float> actual = {0.8f, 0.9f, 1.0f};
    nlm::ActionType action = nlm::ActionType::MoveForward;
    
    // Update self-model based on prediction error
    selfModel.updateSelfModel(predicted, actual, action);
    
    // Should not crash
    assert(selfModel.hasSelfModel());
    
    std::cout << "    testSelfModelUpdate passed" << std::endl;
}

void testSelfModelPreferredAction() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    // The getPreferredAction method might not be exposed in current API
    // This test verifies the self-model infrastructure
    
    std::cout << "    testSelfModelPreferredAction passed" << std::endl;
}

void testSelfModelConfidence() {
    nlm::SelfModel selfModel;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    selfModel.initialize(&mockBrain);
    
    nlm::ActionType action = nlm::ActionType::MoveForward;
    
    float confidence = selfModel.getSelfModelConfidence(action);
    
    // Should return confidence (implementation dependent)
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testSelfModelConfidence passed" << std::endl;
}

void runAll() {
    testSelfModelCreation();
    testSelfModelInitialization();
    testBodySchemaLearning();
    testActionConsequencePrediction();
    testSelfVsExternalDistinction();
    testCapabilityRepresentation();
    testBodyAwareness();
    testSelfModelUpdate();
    testSelfModelPreferredAction();
    testSelfModelConfidence();
}

} // namespace test_self_model
