#include <gtest/gtest.h>
#include "Neuromodulator.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "Neuromodulator.cpp"
#include "Acetylcholine.cpp"
#include "Norepinephrine.cpp"
#include "Serotonin.cpp"
#include "Dopamine.cpp"

namespace nlm {

class NeuromodulatorIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize neuromodulators
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        dopamine = std::make_unique<Dopamine>();
    }

    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Dopamine> dopamine;
};

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveValidNames) {
    EXPECT_STREQ(acetylcholine->getName(), "ACh");
    EXPECT_STREQ(norepinephrine->getName(), "NE");
    EXPECT_STREQ(serotonin->getName(), "5-HT");
    EXPECT_STREQ(dopamine->getName(), "DA");
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveDifferentTypes) {
    EXPECT_EQ(acetylcholine->getNeurotransmitterType(), "Acetylcholine (ACh)");
    EXPECT_EQ(norepinephrine->getNeurotransmitterType(), "Norepinephrine (NE)");
    EXPECT_EQ(serotonin->getNeurotransmitterType(), "Serotonin (5-HT)");
    EXPECT_EQ(dopamine->getNeurotransmitterType(), "Dopamine (DA)");
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsUpdateMethods) {
    TimestepDuration dt = 0.001f;
    
    // Test update doesn't crash
    acetylcholine->update(dt);
    norepinephrine->update(dt);
    serotonin->update(dt);
    dopamine->update(dt);
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsSignalEvents) {
    // Test event signaling
    acetylcholine->signalEvent(0.5f, "attention_shift");
    norepinephrine->signalEvent(0.8f, "novelty");
    serotonin->signalEvent(0.3f, "social_reward");
    dopamine->signalEvent(0.7f, "reward_prediction_error");
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHavePlasiticityFactors) {
    float achFactor = acetylcholine->getPlasticityFactor();
    float neFactor = norepinephrine->getPlasticityFactor();
    float serFactor = serotonin->getPlasticityFactor();
    float daFactor = dopamine->getPlasticityFactor();
    
    // All should return float values (not crash)
    EXPECT_TRUE(std::isfinite(achFactor));
    EXPECT_TRUE(std::isfinite(neFactor));
    EXPECT_TRUE(std::isfinite(serFactor));
    EXPECT_TRUE(std::isfinite(daFactor));
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveLevels) {
    float achLevel = acetylcholine->getLevel();
    float neLevel = norepinephrine->getLevel();
    float serLevel = serotonin->getLevel();
    float daLevel = dopamine->getLevel();
    
    EXPECT_TRUE(achLevel >= 0.0f && achLevel <= 1.0f);
    EXPECT_TRUE(neLevel >= 0.0f && neLevel <= 1.0f);
    EXPECT_TRUE(serLevel >= 0.0f && serLevel <= 1.0f);
    EXPECT_TRUE(daLevel >= 0.0f && daLevel <= 1.0f);
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveExcitabilityModulation) {
    float achExcit = acetylcholine->getExcitabilityModulation();
    float neExcit = norepinephrine->getExcitabilityModulation();
    float serExcit = serotonin->getExcitabilityModulation();
    float daExcit = dopamine->getExcitabilityModulation();
    
    EXPECT_TRUE(std::isfinite(achExcit));
    EXPECT_TRUE(std::isfinite(neExcit));
    EXPECT_TRUE(std::isfinite(serExcit));
    EXPECT_TRUE(std::isfinite(daExcit));
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveWorkingMemoryModulation) {
    float achWM = acetylcholine->getWorkingMemoryModulation();
    float neWM = norepinephrine->getWorkingMemoryModulation();
    float serWM = serotonin->getWorkingMemoryModulation();
    float daWM = dopamine->getWorkingMemoryModulation();
    
    EXPECT_TRUE(std::isfinite(achWM));
    EXPECT_TRUE(std::isfinite(neWM));
    EXPECT_TRUE(std::isfinite(serWM));
    EXPECT_TRUE(std::isfinite(daWM));
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveAttentionModulation) {
    float achAtt = acetylcholine->getAttentionModulation();
    float neAtt = norepinephrine->getAttentionModulation();
    float serAtt = serotonin->getAttentionModulation();
    float daAtt = dopamine->getAttentionModulation();
    
    EXPECT_TRUE(std::isfinite(achAtt));
    EXPECT_TRUE(std::isfinite(neAtt));
    EXPECT_TRUE(std::isfinite(serAtt));
    EXPECT_TRUE(std::isfinite(daAtt));
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveLearningRateModulation) {
    float achLR = acetylcholine->getLearningRateModulation();
    float neLR = norepinephrine->getLearningRateModulation();
    float serLR = serotonin->getLearningRateModulation();
    float daLR = dopamine->getLearningRateModulation();
    
    EXPECT_TRUE(std::isfinite(achLR));
    EXPECT_TRUE(std::isfinite(neLR));
    EXPECT_TRUE(std::isfinite(serLR));
    EXPECT_TRUE(std::isfinite(daLR));
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveEligibilityTraceModulation) {
    float achET = acetylcholine->getEligibilityTraceModulation();
    float neET = norepinephrine->getEligibilityTraceModulation();
    float serET = serotonin->getEligibilityTraceModulation();
    float daET = dopamine->getEligibilityTraceModulation();
    
    EXPECT_TRUE(std::isfinite(achET));
    EXPECT_TRUE(std::isfinite(neET));
    EXPECT_TRUE(std::isfinite(serET));
    EXPECT_TRUE(std::isfinite(daET));
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveBaselineLevels) {
    float achBase = acetylcholine->getBaselineLevel();
    float neBase = norepinephrine->getBaselineLevel();
    float serBase = serotonin->getBaselineLevel();
    float daBase = dopamine->getBaselineLevel();
    
    EXPECT_TRUE(std::isfinite(achBase));
    EXPECT_TRUE(std::isfinite(neBase));
    EXPECT_TRUE(std::isfinite(serBase));
    EXPECT_TRUE(std::isfinite(daBase));
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorsHaveReceptorActivation) {
    float achRec = acetylcholine->getReceptorActivation();
    float neRec = norepinephrine->getReceptorActivation();
    float serRec = serotonin->getReceptorActivation();
    float daRec = dopamine->getReceptorActivation();
    
    EXPECT_TRUE(std::isfinite(achRec));
    EXPECT_TRUE(std::isfinite(neRec));
    EXPECT_TRUE(std::isfinite(serRec));
    EXPECT_TRUE(std::isfinite(daRec));
}

TEST_F(NeuromodulatorIntegrationTest, NeuromodulatorSpecificFunctions) {
    // Test specific functionality for each neuromodulator
    
    // Acetylcholine attention functions
    acetylcholine->signalAttentionShift(0.7f);
    acetylcholine->signalWorkingMemoryEncoding(0.5f);
    acetylcholine->signalWorkingMemoryConsolidation(0.8f);
    
    // Norepinephrine cognitive functions
    norepinephrine->signalNoveltyDetection(0.9f);
    norepinephrine->signalCognitiveFlexibility(0.6f);
    norepinephrine->signalVigilance(0.8f);
    
    // Serotonin behavioral functions
    serotonin->signalMoodChange(0.4f);
    serotonin->signalBehavioralInhibition(0.7f);
    serotonin->signalSocialBehavior(0.5f);
    
    // Dopamine reward functions
    dopamine->signalReward(0.6f);
    dopamine->signalRewardPredictionError(0.3f);
    dopamine->signalLearning(0.8f, 0.7f);
    dopamine->signalWorkingMemoryUpdate(0.5f);
}

TEST_F(NeuromodulatorIntegrationTest, CrossModulationMethods) {
    // Test cross-modulation methods
    acetylcholine->applyCrossModulation(0.5f, "Dopamine");
    norepinephrine->applyCrossModulation(0.3f, "Acetylcholine");
    serotonin->applyCrossModulation(0.7f, "Norepinephrine");
    dopamine->applyCrossModulation(0.4f, "Serotonin");
}

TEST_F(NeuromodulatorIntegrationTest, ActivityDependentRelease) {
    // Test activity-dependent release
    float achActivity = acetylcholine->getActivityDependentRelease(0.5f);
    float neActivity = norepinephrine->getActivityDependentRelease(0.8f);
    float serActivity = serotonin->getActivityDependentRelease(0.3f);
    float daActivity = dopamine->getActivityDependentRelease(0.7f);
    
    EXPECT_TRUE(std::isfinite(achActivity));
    EXPECT_TRUE(std::isfinite(neActivity));
    EXPECT_TRUE(std::isfinite(serActivity));
    EXPECT_TRUE(std::isfinite(daActivity));
}

} // namespace nlm
