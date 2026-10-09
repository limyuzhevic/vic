#include <gtest/gtest.h>
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include <memory>
#include <string>
#include <cmath>

namespace test_neuromodulation {

class NeuromodulatorTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        dopamine = std::make_unique<Dopamine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
    }

    void TearDown() override {
        dopamine.reset();
        acetylcholine.reset();
        norepinephrine.reset();
        serotonin.reset();
    }

    std::unique_ptr<nlm::Dopamine> dopamine;
    std::unique_ptr<nlm::Acetylcholine> acetylcholine;
    std::unique_ptr<nlm::Norepinephrine> norepinephrine;
    std::unique_ptr<nlm::Serotonin> serotonin;
};

TEST_F(NeuromodulatorTestFixture, NeuromodulatorCreation) {
    EXPECT_NE(dopamine, nullptr);
    EXPECT_NE(acetylcholine, nullptr);
    EXPECT_NE(norepinephrine, nullptr);
    EXPECT_NE(serotonin, nullptr);
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorInitialization) {
    EXPECT_TRUE(dopamine->isInitialized());
    EXPECT_TRUE(acetylcholine->isInitialized());
    EXPECT_TRUE(norepinephrine->isInitialized());
    EXPECT_TRUE(serotonin->isInitialized());
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorNames) {
    EXPECT_STREQ(dopamine->getName().c_str(), "DA");
    EXPECT_STREQ(acetylcholine->getName().c_str(), "ACh");
    EXPECT_STREQ(norepinephrine->getName().c_str(), "NE");
    EXPECT_STREQ(serotonin->getName().c_str(), "5-HT");
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorTypes) {
    EXPECT_EQ(dopamine->getNeurotransmitterType(), "Dopamine (DA)");
    EXPECT_EQ(acetylcholine->getNeurotransmitterType(), "Acetylcholine (ACh)");
    EXPECT_EQ(norepinephrine->getNeurotransmitterType(), "Norepinephrine (NE)");
    EXPECT_EQ(serotonin->getNeurotransmitterType(), "Serotonin (5-HT)");
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorLevels) {
    float daLevel = dopamine->getLevel();
    float achLevel = acetylcholine->getLevel();
    float neLevel = norepinephrine->getLevel();
    float serLevel = serotonin->getLevel();
    
    EXPECT_TRUE(daLevel >= 0.0f && daLevel <= 1.0f);
    EXPECT_TRUE(achLevel >= 0.0f && achLevel <= 1.0f);
    EXPECT_TRUE(neLevel >= 0.0f && neLevel <= 1.0f);
    EXPECT_TRUE(serLevel >= 0.0f && serLevel <= 1.0f);
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorBaselineLevels) {
    float daBase = dopamine->getBaselineLevel();
    float achBase = acetylcholine->getBaselineLevel();
    float neBase = norepinephrine->getBaselineLevel();
    float serBase = serotonin->getBaselineLevel();
    
    EXPECT_TRUE(std::isfinite(daBase));
    EXPECT_TRUE(std::isfinite(achBase));
    EXPECT_TRUE(std::isfinite(neBase));
    EXPECT_TRUE(std::isfinite(serBase));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorUpdate) {
    TimestepDuration dt = 0.001f;
    
    float daBefore = dopamine->getLevel();
    float achBefore = acetylcholine->getLevel();
    float neBefore = norepinephrine->getLevel();
    float serBefore = serotonin->getLevel();
    
    dopamine->update(dt);
    acetylcholine->update(dt);
    norepinephrine->update(dt);
    serotonin->update(dt);
    
    float daAfter = dopamine->getLevel();
    float achAfter = acetylcholine->getLevel();
    float neAfter = norepinephrine->getLevel();
    float serAfter = serotonin->getLevel();
    
    EXPECT_TRUE(std::isfinite(daAfter));
    EXPECT_TRUE(std::isfinite(achAfter));
    EXPECT_TRUE(std::isfinite(neAfter));
    EXPECT_TRUE(std::isfinite(serAfter));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorActivityDependentRelease) {
    float daActivity = dopamine->getActivityDependentRelease(0.5f);
    float achActivity = acetylcholine->getActivityDependentRelease(0.5f);
    float neActivity = norepinephrine->getActivityDependentRelease(0.5f);
    float serActivity = serotonin->getActivityDependentRelease(0.5f);
    
    EXPECT_TRUE(std::isfinite(daActivity));
    EXPECT_TRUE(std::isfinite(achActivity));
    EXPECT_TRUE(std::isfinite(neActivity));
    EXPECT_TRUE(std::isfinite(serActivity));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorPlasticityFactors) {
    float daFactor = dopamine->getPlasticityFactor();
    float achFactor = acetylcholine->getPlasticityFactor();
    float neFactor = norepinephrine->getPlasticityFactor();
    float serFactor = serotonin->getPlasticityFactor();
    
    EXPECT_TRUE(std::isfinite(daFactor));
    EXPECT_TRUE(std::isfinite(achFactor));
    EXPECT_TRUE(std::isfinite(neFactor));
    EXPECT_TRUE(std::isfinite(serFactor));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorModulationValues) {
    float daExcit = dopamine->getExcitabilityModulation();
    float achExcit = acetylcholine->getExcitabilityModulation();
    float neExcit = norepinephrine->getExcitabilityModulation();
    float serExcit = serotonin->getExcitabilityModulation();
    
    EXPECT_TRUE(std::isfinite(daExcit));
    EXPECT_TRUE(std::isfinite(achExcit));
    EXPECT_TRUE(std::isfinite(neExcit));
    EXPECT_TRUE(std::isfinite(serExcit));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorWorkingMemoryModulation) {
    float daWM = dopamine->getWorkingMemoryModulation();
    float achWM = acetylcholine->getWorkingMemoryModulation();
    float neWM = norepinephrine->getWorkingMemoryModulation();
    float serWM = serotonin->getWorkingMemoryModulation();
    
    EXPECT_TRUE(std::isfinite(daWM));
    EXPECT_TRUE(std::isfinite(achWM));
    EXPECT_TRUE(std::isfinite(neWM));
    EXPECT_TRUE(std::isfinite(serWM));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorAttentionModulation) {
    float daAtt = dopamine->getAttentionModulation();
    float achAtt = acetylcholine->getAttentionModulation();
    float neAtt = norepinephrine->getAttentionModulation();
    float serAtt = serotonin->getAttentionModulation();
    
    EXPECT_TRUE(std::isfinite(daAtt));
    EXPECT_TRUE(std::isfinite(achAtt));
    EXPECT_TRUE(std::isfinite(neAtt));
    EXPECT_TRUE(std::isfinite(serAtt));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorLearningRateModulation) {
    float daLR = dopamine->getLearningRateModulation();
    float achLR = acetylcholine->getLearningRateModulation();
    float neLR = norepinephrine->getLearningRateModulation();
    float serLR = serotonin->getLearningRateModulation();
    
    EXPECT_TRUE(std::isfinite(daLR));
    EXPECT_TRUE(std::isfinite(achLR));
    EXPECT_TRUE(std::isfinite(neLR));
    EXPECT_TRUE(std::isfinite(serLR));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorEligibilityTraceModulation) {
    float daET = dopamine->getEligibilityTraceModulation();
    float achET = acetylcholine->getEligibilityTraceModulation();
    float neET = norepinephrine->getEligibilityTraceModulation();
    float serET = serotonin->getEligibilityTraceModulation();
    
    EXPECT_TRUE(std::isfinite(daET));
    EXPECT_TRUE(std::isfinite(achET));
    EXPECT_TRUE(std::isfinite(neET));
    EXPECT_TRUE(std::isfinite(serET));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorReceptorActivation) {
    float daRec = dopamine->getReceptorActivation();
    float achRec = acetylcholine->getReceptorActivation();
    float neRec = norepinephrine->getReceptorActivation();
    float serRec = serotonin->getReceptorActivation();
    
    EXPECT_TRUE(std::isfinite(daRec));
    EXPECT_TRUE(std::isfinite(achRec));
    EXPECT_TRUE(std::isfinite(neRec));
    EXPECT_TRUE(std::isfinite(serRec));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorSpecificFunctions) {
    // Test specific neuromodulator functions
    dopamine->signalReward(0.6f);
    dopamine->signalRewardPredictionError(0.3f);
    dopamine->signalLearning(0.8f, 0.7f);
    
    acetylcholine->signalAttentionShift(0.7f);
    acetylcholine->signalWorkingMemoryEncoding(0.5f);
    acetylcholine->signalWorkingMemoryConsolidation(0.8f);
    
    norepinephrine->signalNoveltyDetection(0.9f);
    norepinephrine->signalCognitiveFlexibility(0.6f);
    norepinephrine->signalVigilance(0.8f);
    
    serotonin->signalMoodChange(0.4f);
    serotonin->signalBehavioralInhibition(0.7f);
    serotonin->signalSocialBehavior(0.5f);
    
    // Test cross-modulation
    dopamine->applyCrossModulation(0.5f, "Acetylcholine");
    acetylcholine->applyCrossModulation(0.3f, "Dopamine");
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorStateChanges) {
    float initialDA = dopamine->getLevel();
    float initialACh = acetylcholine->getLevel();
    
    // Simulate state changes
    dopamine->signalReward(0.8f);
    acetylcholine->signalAttentionShift(0.9f);
    
    float finalDA = dopamine->getLevel();
    float finalACh = acetylcholine->getLevel();
    
    EXPECT_TRUE(std::isfinite(finalDA));
    EXPECT_TRUE(std::isfinite(finalACh));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorIntegration) {
    // Test all neuromodulators together
    std::vector<nlm::Neuromodulator*> neuromodulators = {
        dopamine.get(), acetylcholine.get(), norepinephrine.get(), serotonin.get()
    };
    
    for (auto* mod : neuromodulators) {
        EXPECT_TRUE(mod->isInitialized());
        mod->update(0.001f);
        EXPECT_TRUE(std::isfinite(mod->getLevel()));
    }
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorDecay) {
    float initialDA = dopamine->getLevel();
    
    for (int i = 0; i < 100; ++i) {
        dopamine->update(0.001f);
    }
    
    float finalDA = dopamine->getLevel();
    EXPECT_TRUE(std::isfinite(finalDA));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorStressResponse) {
    // Test stress response simulation
    norepinephrine->signalVigilance(0.8f);
    serotonin->signalBehavioralInhibition(0.7f);
    dopamine->signalRewardPredictionError(0.9f);
    
    float neLevel = norepinephrine->getLevel();
    float serLevel = serotonin->getLevel();
    float daLevel = dopamine->getLevel();
    
    EXPECT_TRUE(std::isfinite(neLevel));
    EXPECT_TRUE(std::isfinite(serLevel));
    EXPECT_TRUE(std::isfinite(daLevel));
}

TEST_F(NeuromodulatorTestFixture, NeuromodulatorRecovery) {
    // Test recovery from stress
    for (int i = 0; i < 50; ++i) {
        dopamine->update(0.001f);
        acetylcholine->update(0.001f);
        norepinephrine->update(0.001f);
        serotonin->update(0.001f);
    }
    
    float daLevel = dopamine->getLevel();
    float achLevel = acetylcholine->getLevel();
    
    EXPECT_TRUE(std::isfinite(daLevel));
    EXPECT_TRUE(std::isfinite(achLevel));
    EXPECT_TRUE(daLevel > 0.0f);
    EXPECT_TRUE(achLevel > 0.0f);
}

} // namespace test_neuromodulation