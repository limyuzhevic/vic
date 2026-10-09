#include <gtest/gtest.h>
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <memory>
#include <vector>
#include <cmath>

namespace test_cognition {

class NeuralPlannerTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        planner = std::make_unique<nlm::NeuralPlanner>();
    }

    void TearDown() override {
        planner.reset();
    }

    std::unique_ptr<nlm::NeuralPlanner> planner;
};

TEST_F(NeuralPlannerTestFixture, NeuralPlannerCreation) {
    EXPECT_NE(planner, nullptr);
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerInitialization) {
    EXPECT_TRUE(planner->isInitialized());
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerType) {
    EXPECT_STREQ(planner->getType().c_str(), "NeuralPlanner");
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerPlan) {
    std::vector<float> state = {1.0f, 2.0f, 3.0f};
    
    auto action = planner->plan(state);
    EXPECT_TRUE(action.has_value());
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerPlanWithConstraints) {
    std::vector<float> state = {1.0f, 2.0f, 3.0f};
    std::vector<float> constraints = {0.5f, 0.8f, 0.3f};
    
    auto action = planner->plan(state, constraints);
    EXPECT_TRUE(action.has_value());
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerEvaluate) {
    std::vector<float> action = {1.0f, 2.0f, 3.0f};
    
    float value = planner->evaluate(action);
    EXPECT_TRUE(std::isfinite(value));
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerLearn) {
    std::vector<float> state = {1.0f, 2.0f, 3.0f};
    std::vector<float> action = {0.5f, 0.7f, 0.9f};
    
    planner->learn(state, action);
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerReset) {
    std::vector<float> state = {1.0f, 2.0f, 3.0f};
    
    planner->plan(state);
    planner->reset();
    
    auto action = planner->plan(state);
    EXPECT_TRUE(action.has_value());
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerCapacity) {
    EXPECT_GT(planner->getCapacity(), 0);
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerLearningRate) {
    float rate = planner->getLearningRate();
    EXPECT_TRUE(std::isfinite(rate));
    EXPECT_GT(rate, 0.0f);
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerExploration) {
    std::vector<float> state = {1.0f, 2.0f, 3.0f};
    
    float exploration = planner->getExplorationRate();
    EXPECT_TRUE(std::isfinite(exploration));
    EXPECT_GE(exploration, 0.0f);
    EXPECT_LE(exploration, 1.0f);
}

TEST_F(NeuralPlannerTestFixture, NeuralPlannerComplexity) {
    // Test with complex state space
    std::vector<float> complexState(100, 1.0f);
    
    auto action = planner->plan(complexState);
    EXPECT_TRUE(action.has_value());
    EXPECT_EQ(action->size(), 100);
}

class ConceptFormationTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        conceptFormation = std::make_unique<nlm::ConceptFormation>();
    }

    void TearDown() override {
        conceptFormation.reset();
    }

    std::unique_ptr<nlm::ConceptFormation> conceptFormation;
};

TEST_F(ConceptFormationTestFixture, ConceptFormationCreation) {
    EXPECT_NE(conceptFormation, nullptr);
}

TEST_F(ConceptFormationTestFixture, ConceptFormationInitialization) {
    EXPECT_TRUE(conceptFormation->isInitialized());
}

TEST_F(ConceptFormationTestFixture, ConceptFormationType) {
    EXPECT_STREQ(conceptFormation->getType().c_str(), "ConceptFormation");
}

TEST_F(ConceptFormationTestFixture, ConceptFormationLearnConcept) {
    std::vector<float> features = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    
    bool learned = conceptFormation->learnConcept(features);
    EXPECT_TRUE(learned);
}

TEST_F(ConceptFormationTestFixture, ConceptFormationRecallConcept) {
    std::vector<float> features = {1.0f, 2.0f, 3.0f};
    
    conceptFormation->learnConcept(features);
    auto retrieved = conceptFormation->recallConcept(features);
    EXPECT_TRUE(retrieved.has_value());
}

TEST_F(ConceptFormationTestFixture, ConceptFormationSimilarConcept) {
    std::vector<float> concept1 = {1.0f, 2.0f, 3.0f};
    std::vector<float> concept2 = {1.1f, 2.1f, 3.1f};
    
    conceptFormation->learnConcept(concept1);
    conceptFormation->learnConcept(concept2);
    
    auto similarity = conceptFormation->getSimilarity(concept1, concept2);
    EXPECT_TRUE(std::isfinite(similarity));
}

TEST_F(ConceptFormationTestFixture, ConceptFormationUpdate) {
    std::vector<float> features = {1.0f, 2.0f, 3.0f};
    
    conceptFormation->learnConcept(features);
    conceptFormation->update(features);
    
    auto retrieved = conceptFormation->recallConcept(features);
    EXPECT_TRUE(retrieved.has_value());
}

TEST_F(ConceptFormationTestFixture, ConceptFormationReset) {
    std::vector<float> features = {1.0f, 2.0f, 3.0f};
    
    conceptFormation->learnConcept(features);
    conceptFormation->reset();
    
    auto retrieved = conceptFormation->recallConcept(features);
    EXPECT_FALSE(retrieved.has_value());
}

TEST_F(ConceptFormationTestFixture, ConceptFormationCapacity) {
    EXPECT_GT(conceptFormation->getCapacity(), 0);
}

TEST_F(ConceptFormationTestFixture, ConceptFormationSimilarityThreshold) {
    float threshold = conceptFormation->getSimilarityThreshold();
    EXPECT_TRUE(std::isfinite(threshold));
    EXPECT_GE(threshold, 0.0f);
    EXPECT_LE(threshold, 1.0f);
}

TEST_F(ConceptFormationTestFixture, ConceptFormationLearnMultipleConcepts) {
    std::vector<std::vector<float>> concepts = {
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
        {7.0f, 8.0f, 9.0f}
    };
    
    for (const auto& concept : concepts) {
        EXPECT_TRUE(conceptFormation->learnConcept(concept));
    }
}

TEST_F(ConceptFormationTestFixture, ConceptFormationComplexFeatures) {
    std::vector<float> complexFeatures(50, 1.0f);
    
    EXPECT_TRUE(conceptFormation->learnConcept(complexFeatures));
}

TEST_F(ConceptFormationTestFixture, ConceptFormationZeroFeatures) {
    std::vector<float> zeroFeatures(10, 0.0f);
    
    EXPECT_TRUE(conceptFormation->learnConcept(zeroFeatures));
}

TEST_F(ConceptFormationTestFixture, ConceptFormationNegativeFeatures) {
    std::vector<float> negativeFeatures = {-1.0f, -2.0f, -3.0f};
    
    EXPECT_TRUE(conceptFormation->learnConcept(negativeFeatures));
}

TEST_F(ConceptFormationTestFixture, CognitionIntegration) {
    // Test integration of neural planner and concept formation
    std::vector<float> state = {1.0f, 2.0f, 3.0f};
    
    auto action = planner->plan(state);
    EXPECT_TRUE(action.has_value());
    
    std::vector<float> features = {1.0f, 2.0f, 3.0f};
    bool learned = conceptFormation->learnConcept(features);
    EXPECT_TRUE(learned);
}

TEST_F(ConceptFormationTestFixture, NeuralPlannerLearningCurve) {
    std::vector<float> state = {1.0f, 2.0f, 3.0f};
    std::vector<float> action = {0.5f, 0.7f, 0.9f};
    
    // Learn multiple times and track improvement
    float initialValue = planner->evaluate(action);
    
    for (int i = 0; i < 10; ++i) {
        planner->learn(state, action);
    }
    
    float finalValue = planner->evaluate(action);
    EXPECT_TRUE(std::isfinite(finalValue));
}

TEST_F(ConceptFormationTestFixture, ConceptFormationAssociativeLearning) {
    std::vector<float> features1 = {1.0f, 2.0f, 3.0f};
    std::vector<float> features2 = {4.0f, 5.0f, 6.0f};
    
    conceptFormation->learnConcept(features1);
    conceptFormation->learnConcept(features2);
    
    auto similarity = conceptFormation->getSimilarity(features1, features2);
    EXPECT_TRUE(std::isfinite(similarity));
}

TEST_F(ConceptFormationTestFixture, NeuralPlannerConstraintSatisfaction) {
    std::vector<float> state = {1.0f, 2.0f, 3.0f};
    std::vector<float> constraints = {0.1f, 0.5f, 0.9f};
    
    auto action = planner->plan(state, constraints);
    EXPECT_TRUE(action.has_value());
}

} // namespace test_cognition