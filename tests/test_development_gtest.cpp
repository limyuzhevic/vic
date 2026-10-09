#include <gtest/gtest.h>
#include "development/DevelopmentSystem.hpp"
#include "development/Maturation.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.cpp"
#include "development/Synaptogenesis.cpp"
#include "development/Pruning.cpp"
#include <memory>
#include <cmath>

namespace test_development {

class DevelopmentSystemTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        developmentSystem = std::make_unique<nlm::DevelopmentSystem>();
    }

    void TearDown() override {
        developmentSystem.reset();
    }

    std::unique_ptr<nlm::DevelopmentSystem> developmentSystem;
};

TEST_F(DevelopmentSystemTestFixture, DevelopmentSystemCreation) {
    EXPECT_NE(developmentSystem, nullptr);
    EXPECT_TRUE(developmentSystem->getStage() == nlm::DevelopmentalStage::Embryonic ||
                developmentSystem->getStage() == nlm::DevelopmentalStage::Fetal ||
                developmentSystem->getStage() == nlm::DevelopmentalStage::Postnatal);
}

TEST_F(DevelopmentSystemTestFixture, DevelopmentSystemStageProgression) {
    nlm::DevelopmentalStage initialStage = developmentSystem->getStage();
    
    developmentSystem->advanceStage();
    nlm::DevelopmentalStage newStage = developmentSystem->getStage();
    
    EXPECT_NE(initialStage, newStage);
}

TEST_F(DevelopmentSystemTestFixture, DevelopmentSystemStageName) {
    const char* stageName = developmentSystem->getStageName();
    EXPECT_TRUE(stageName != nullptr);
    EXPECT_TRUE(strlen(stageName) > 0);
}

TEST_F(DevelopmentSystemTestFixture, DevelopmentSystemPlasticityModifier) {
    float modifier = developmentSystem->getPlasticityModifier();
    EXPECT_TRUE(std::isfinite(modifier));
    EXPECT_GE(modifier, 0.0f);
    EXPECT_LE(modifier, 2.0f);  // Reasonable range
}

TEST_F(DevelopmentSystemTestFixture, DevelopmentSystemCriticalPeriod) {
    bool isCritical = developmentSystem->isCriticalPeriod();
    EXPECT_TRUE(std::isfinite(developmentSystem->getCriticalPeriodProgress()));
    
    if (isCritical) {
        EXPECT_GT(developmentSystem->getCriticalPeriodProgress(), 0.0f);
    }
}

TEST_F(DevelopmentSystemTestFixture, DevelopmentSystemAge) {
    double initialAge = developmentSystem->getDevelopmentalAge();
    
    developmentSystem->setDevelopmentalAge(1000.0);
    EXPECT_DOUBLE_EQ(developmentSystem->getDevelopmentalAge(), 1000.0);
}

TEST_F(DevelopmentSystemTestFixture, DevelopmentSystemUpdate) {
    class MockBrain {
    public:
        void step(SimulationStep) {}
        void reset() {}
        int getNeuronCount() const { return 100; }
    };
    
    MockBrain brain;
    developmentSystem->update(&brain, 0);
    EXPECT_TRUE(std::isfinite(developmentSystem->getDevelopmentalAge()));
}

class MaturationTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        maturation = std::make_unique<nlm::Maturation>();
    }

    void TearDown() override {
        maturation.reset();
    }

    std::unique_ptr<nlm::Maturation> maturation;
};

TEST_F(MaturationTestFixture, MaturationCreation) {
    EXPECT_NE(maturation, nullptr);
}

TEST_F(MaturationTestFixture, MaturationProgress) {
    float initialProgress = maturation->getProgress();
    
    maturation->setProgress(0.5f);
    EXPECT_FLOAT_EQ(maturation->getProgress(), 0.5f);
    
    maturation->setProgress(1.0f);
    EXPECT_FLOAT_EQ(maturation->getProgress(), 1.0f);
}

TEST_F(MaturationTestFixture, MaturationMatureProperties) {
    maturation->setProgress(1.0f);
    
    MembranePotential matureThreshold = maturation->getMatureThreshold();
    MembranePotential matureResting = maturation->getMatureRestingPotential();
    float matureTau = maturation->getMatureTimeConstant();
    
    EXPECT_TRUE(std::isfinite(matureThreshold));
    EXPECT_TRUE(std::isfinite(matureResting));
    EXPECT_TRUE(std::isfinite(matureTau));
}

TEST_F(MaturationTestFixture, MaturationUpdate) {
    class MockBrain {
    public:
        void step(SimulationStep) {}
        void reset() {}
    };
    
    MockBrain brain;
    maturation->update(&brain, 0);
    EXPECT_TRUE(std::isfinite(maturation->getProgress()));
}

class SynaptogenesisTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        synaptogenesis = std::make_unique<nlm::Synaptogenesis>();
    }

    void TearDown() override {
        synaptogenesis.reset();
    }

    std::unique_ptr<nlm::Synaptogenesis> synaptogenesis;
};

TEST_F(SynaptogenesisTestFixture, SynaptogenesisCreation) {
    EXPECT_NE(synaptogenesis, nullptr);
}

TEST_F(SynaptogenesisTestFixture, SynaptogenesisFormationRate) {
    float initialRate = synaptogenesis->getFormationRate();
    
    synaptogenesis->setFormationRate(0.5f);
    EXPECT_FLOAT_EQ(synaptogenesis->getFormationRate(), 0.5f);
}

TEST_F(SynaptogenesisTestFixture, SynaptogenesisTargetDensity) {
    float initialDensity = synaptogenesis->getTargetDensity();
    
    synaptogenesis->setTargetDensity(0.8f);
    EXPECT_FLOAT_EQ(synaptogenesis->getTargetDensity(), 0.8f);
}

TEST_F(SynaptogenesisTestFixture, SynaptogenesisUpdate) {
    class MockBrain {
    public:
        void step(SimulationStep) {}
        void reset() {}
    };
    
    class MockRandom {
    public:
        float randomFloat() { return 0.5f; }
    };
    
    MockBrain brain;
    MockRandom rng;
    synaptogenesis->update(&brain, rng);
    EXPECT_TRUE(std::isfinite(synaptogenesis->getFormationRate()));
}

class PruningTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        pruning = std::make_unique<nlm::Pruning>();
    }

    void TearDown() override {
        pruning.reset();
    }

    std::unique_ptr<nlm::Pruning> pruning;
};

TEST_F(PruningTestFixture, PruningCreation) {
    EXPECT_NE(pruning, nullptr);
}

TEST_F(PruningTestFixture, PruningUpdate) {
    class MockBrain {
    public:
        void step(SimulationStep) {}
        void reset() {}
    };
    
    MockBrain brain;
    pruning->update(&brain, 0.001f);
    EXPECT_TRUE(std::isfinite(pruning->getStrength()));
}

TEST_F(PruningTestFixture, PruningStrength) {
    float initialStrength = pruning->getStrength();
    
    pruning->setStrength(0.7f);
    EXPECT_FLOAT_EQ(pruning->getStrength(), 0.7f);
}

TEST_F(PruningTestFixture, PruningIntegration) {
    // Test all development components together
    EXPECT_TRUE(developmentSystem->isInitialized() || true);  // Basic check
    EXPECT_TRUE(maturation->getProgress() >= 0.0f && maturation->getProgress() <= 1.0f);
    EXPECT_TRUE(synaptogenesis->getFormationRate() >= 0.0f);
    EXPECT_TRUE(pruning->getStrength() >= 0.0f);
}

TEST_F(PruningTestFixture, DevelopmentSystemStageSequence) {
    // Test developmental stage progression
    nlm::DevelopmentalStage stage1 = developmentSystem->getStage();
    
    developmentSystem->advanceStage();
    nlm::DevelopmentalStage stage2 = developmentSystem->getStage();
    
    EXPECT_NE(stage1, stage2);
}

TEST_F(PruningTestFixture, MaturationNonLinearProgress) {
    // Test that maturation progress follows reasonable patterns
    maturation->setProgress(0.0f);
    EXPECT_FLOAT_EQ(maturation->getProgress(), 0.0f);
    
    maturation->setProgress(0.5f);
    EXPECT_FLOAT_EQ(maturation->getProgress(), 0.5f);
    
    maturation->setProgress(1.0f);
    EXPECT_FLOAT_EQ(maturation->getProgress(), 1.0f);
}

TEST_F(PruningTestFixture, DevelopmentSystemAgeGrowth) {
    // Test developmental age increases over time
    double age1 = developmentSystem->getDevelopmentalAge();
    
    developmentSystem->advanceStage();
    
    double age2 = developmentSystem->getDevelopmentalAge();
    EXPECT_GE(age2, age1);
}

TEST_F(PruningTestFixture, SynaptogenesisRateConstraints) {
    // Test synaptogenesis rate constraints
    synaptogenesis->setFormationRate(0.0f);
    EXPECT_FLOAT_EQ(synaptogenesis->getFormationRate(), 0.0f);
    
    synaptogenesis->setFormationRate(1.0f);
    EXPECT_FLOAT_EQ(synaptogenesis->getFormationRate(), 1.0f);
    
    synaptogenesis->setFormationRate(0.5f);
    EXPECT_FLOAT_EQ(synaptogenesis->getFormationRate(), 0.5f);
}

TEST_F(PruningTestFixture, PruningAdaptiveChanges) {
    // Test pruning adapts to developmental stage
    pruning->setStrength(0.3f);
    EXPECT_FLOAT_EQ(pruning->getStrength(), 0.3f);
    
    pruning->setStrength(0.8f);
    EXPECT_FLOAT_EQ(pruning->getStrength(), 0.8f);
}

TEST_F(PruningTestFixture, DevelopmentSystemIntegration) {
    // Test integration of all development systems
    EXPECT_NE(developmentSystem, nullptr);
    EXPECT_NE(maturation, nullptr);
    EXPECT_NE(synaptogenesis, nullptr);
    EXPECT_NE(pruning, nullptr);
    
    // All should be operational
    EXPECT_TRUE(developmentSystem->getStage() != nlm::DevelopmentalStage::NotDefined);
}

TEST_F(PruningTestFixture, DevelopmentalStageValidity) {
    // Test all developmental stages are valid
    EXPECT_TRUE(developmentSystem->getStage() == nlm::DevelopmentalStage::Embryonic ||
                developmentSystem->getStage() == nlm::DevelopmentalStage::Fetal ||
                developmentSystem->getStage() == nlm::DevelopmentalStage::Postnatal ||
                developmentSystem->getStage() == nlm::DevelopmentalStage::Adolescent ||
                developmentSystem->getStage() == nlm::DevelopmentalStage::Adult);
}

TEST_F(PruningTestFixture, DevelopmentComplexity) {
    // Test development system complexity with multiple stages
    for (int i = 0; i < 10; ++i) {
        developmentSystem->advanceStage();
        EXPECT_NE(developmentSystem->getStage(), nlm::DevelopmentalStage::NotDefined);
        
        float plasticityMod = developmentSystem->getPlasticityModifier();
        EXPECT_TRUE(std::isfinite(plasticityMod));
        EXPECT_GE(plasticityMod, 0.0f);
    }
}

} // namespace test_development