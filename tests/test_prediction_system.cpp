#include <gtest/gtest.h>
#include "../prediction/PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"

class PredictionSystemTest : public ::testing::Test {
protected:
    nlm::PredictionSystem predictionSystem;
    nlm::Vision vision;
    nlm::Vision actual;
};

TEST_F(PredictionSystemTest, InitialState) {
    EXPECT_FLOAT_EQ(predictionSystem.getPredictionError(), 0.0f);
    EXPECT_FLOAT_EQ(predictionSystem.getConfidence(), 0.5f);
    EXPECT_EQ(predictionSystem.getErrorHistory().size(), 0U);
}

TEST_F(PredictionSystemTest, PredictNextState) {
    // Set up some test data
    std::vector<float> testData = {1.0f, 2.0f, 3.0f};
    vision.setData(testData);
    
    auto prediction = predictionSystem.predictNextState(vision);
    ASSERT_NE(prediction, nullptr);
    
    // Check that prediction is a Vision type
    const auto& predData = prediction->getData();
    EXPECT_EQ(predData.size(), testData.size());
}

TEST_F(PredictionSystemTest, UpdatePredictions) {
    std::vector<float> predData = {1.0f, 2.0f, 3.0f};
    std::vector<float> actualData = {1.1f, 2.1f, 3.1f};
    
    vision.setData(predData);
    actual.setData(actualData);
    
    predictionSystem.updatePredictions(vision, actual);
    
    EXPECT_GT(predictionSystem.getPredictionError(), 0.0f);
    EXPECT_EQ(predictionSystem.getErrorHistory().size(), 1U);
}

TEST_F(PredictionSystemTest, GetConfidence) {
    EXPECT_FLOAT_EQ(predictionSystem.getConfidence(), 0.5f);
    
    // Update predictions to improve confidence
    std::vector<float> predData = {1.0f, 2.0f};
    std::vector<float> actualData = {1.0f, 2.0f};
    
    vision.setData(predData);
    actual.setData(actualData);
    
    predictionSystem.updatePredictions(vision, actual);
    EXPECT_LE(predictionSystem.getPredictionError(), 0.1f); // Should be very low
    EXPECT_GT(predictionSystem.getConfidence(), 0.5f);
}

TEST_F(PredictionSystemTest, ClearHistory) {
    predictionSystem.clearHistory();
    EXPECT_EQ(predictionSystem.getErrorHistory().size(), 0U);
    EXPECT_FLOAT_EQ(predictionSystem.getPredictionError(), 0.0f);
}