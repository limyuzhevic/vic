// Neuromodulation Tests - Modern Google Test Version
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class NeuromodulationTests : public NLMTestBase {
public:
    void TestNeuromodulatorBasics() {
        Neuromodulator neuromodulator;
        EXPECT_FLOAT_EQ(neuromodulator.getValue(), 0.0f);
        EXPECT_FLOAT_EQ(neuromodulator.getLearningRate(), 0.01f);
    }
    
    void TestRewardSystem() {
        Reward reward;
        EXPECT_FLOAT_EQ(reward.getValue(), 0.0f);
        EXPECT_TRUE(reward.canInhibitLearning());
        
        reward.setValue(0.5f);
        EXPECT_FLOAT_EQ(reward.getValue(), 0.5f);
        EXPECT_FALSE(reward.canInhibitLearning());
    }
    
    void TestCuriositySystem() {
        Curiosity curiosity;
        EXPECT_FLOAT_EQ(curiosity.getDrive(), 0.0f);
        EXPECT_FALSE(curiosity.canDriveLearning());
        
        curiosity.setDrive(0.3f);
        EXPECT_FLOAT_EQ(curiosity.getDrive(), 0.3f);
        EXPECT_TRUE(curiosity.canDriveLearning());
    }
    
    void TestNoveltyDetection() {
        Novelty novelty;
        EXPECT_FLOAT_EQ(novelty.getNoveltyScore(), 0.0f);
        EXPECT_FALSE(novelty.isHighlyNovel());
        
        novelty.setNoveltyScore(0.8f);
        EXPECT_FLOAT_EQ(novelty.getNoveltyScore(), 0.8f);
        EXPECT_TRUE(novelty.isHighlyNovel());
    }
    
    void TestPredictionErrorSystem() {
        PredictionError error;
        EXPECT_FLOAT_EQ(error.getError(), 0.0f);
        EXPECT_FALSE(error.shouldIncreaseLearning());
        
        error.setError(0.2f);
        EXPECT_FLOAT_EQ(error.getError(), 0.2f);
        EXPECT_TRUE(error.shouldIncreaseLearning());
    }
};

TEST_F(NeuromodulationTests, TestNeuromodulatorBasics) { TestNeuromodulatorBasics(); }
TEST_F(NeuromodulationTests, TestRewardSystem) { TestRewardSystem(); }
TEST_F(NeuromodulationTests, TestCuriositySystem) { TestCuriositySystem(); }
TEST_F(NeuromodulationTests, TestNoveltyDetection) { TestNoveltyDetection(); }
TEST_F(NeuromodulationTests, TestPredictionErrorSystem) { TestPredictionErrorSystem(); }

} // namespace test
} // namespace nlm
