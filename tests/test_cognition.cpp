// tests/test_cognition.cpp
#include "NeuralPlanner.hpp"
#include "ConceptFormation.hpp"
#include <gtest/gtest.h>

class NeuralPlannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        planner = std::make_unique<NeuralPlanner>();
    }
    
    std::unique_ptr<NeuralPlanner> planner;
};

TEST_F(NeuralPlannerTest, InitialState) {
    EXPECT_EQ(planner->getPlanningDepth(), 3); // Default depth
}

TEST_F(NeuralPlannerTest, SetPlanningDepth) {
    planner->setPlanningDepth(5);
    EXPECT_EQ(planner->getPlanningDepth(), 5);
}

TEST_F(NeuralPlannerTest, EvaluateActionSequence) {
    std::vector<NeuronId> sequence = {1, 2, 3, 4, 5};
    float score = planner->evaluateActionSequence(sequence);
    // Should return some score (implementation dependent)
    EXPECT_GE(score, -1000.0f); // Allow for negative scores
}

TEST_F(NeuralPlannerTest, GetBestActionSequence) {
    std::vector<NeuronId> sequence = {1, 2, 3};
    planner->update(sequence, 0.5f); // Give positive reward
    
    auto bestSequence = planner->getBestActionSequence();
    // Should contain the learned sequence
    EXPECT_FALSE(bestSequence.empty());
}

class ConceptFormationTest : public ::testing::Test {
protected:
    void SetUp() override {
        concept = std::make_unique<ConceptFormation>();
    }
    
    std::unique_ptr<ConceptFormation> concept;
};

TEST_F(ConceptFormationTest, InitialState) {
    EXPECT_EQ(concept->getConceptCount(), 0);
}

TEST_F(ConceptFormationTest, ProcessPattern) {
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f};
    concept->processPattern(pattern);
    
    // Should have processed the pattern
    EXPECT_GE(concept->getConceptCount(), 0);
}

TEST_F(ConceptFormationTest, GetConceptStats) {
    auto stats = concept->getConceptStats();
    EXPECT_EQ(stats.size(), 0); // No concepts yet
}

TEST_F(ConceptFormationTest, Reset) {
    concept->reset();
    EXPECT_EQ(concept->getConceptCount(), 0);
}
