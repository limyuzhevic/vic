#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/Curiosity.hpp"
#include <cmath>

class NeuromodulatorTestSuite : public TestSuite {
public:
    NeuromodulatorTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Neuromodulator Test Suite..." << std::endl;
        
        testNeuromodulatorCreation();
        testNeuromodulatorLearning();
        testNeuromodulatorRelease();
        testNeuromodulatorModulation();
        
        std::cout << "Neuromodulator Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "NeuromodulatorTestSuite";
    }
    
private:
    void testNeuromodulatorCreation() {
        nlm::Brain brain("test_brain");
        nlm::Neuromodulator neuromodulator;
        
        neuromodulator.initialize(&brain);
        ASSERT_TRUE(neuromodulator.getModulationLevel() >= 0.0f);
        
        std::cout << "    testNeuromodulatorCreation passed" << std::endl;
    }
    
    void testNeuromodulatorLearning() {
        nlm::Brain brain("test_brain");
        nlm::Neuromodulator neuromodulator;
        
        neuromodulator.initialize(&brain);
        
        neuromodulator.recordExperience(0.8f, 0.2f, 1.0f);
        neuromodulator.updateLearning(0.5f);
        
        ASSERT_TRUE(neuromodulator.getModulationLevel() >= 0.0f);
        
        std::cout << "    testNeuromodulatorLearning passed" << std::endl;
    }
    
    void testNeuromodulatorRelease() {
        nlm::Brain brain("test_brain");
        nlm::Neuromodulator neuromodulator;
        
        neuromodulator.initialize(&brain);
        
        neuromodulator.triggerRelease(0.6f, 0.3f);
        
        ASSERT_TRUE(neuromodulator.getModulationLevel() >= 0.0f);
        
        std::cout << "    testNeuromodulatorRelease passed" << std::endl;
    }
    
    void testNeuromodulatorModulation() {
        nlm::Brain brain("test_brain");
        nlm::Neuromodulator neuromodulator;
        
        neuromodulator.initialize(&brain);
        
        neuromodulator.applyModulation(nlm::ModulationType::Dopamine, 0.7f);
        neuromodulator.applyModulation(nlm::ModulationType::Curiosity, 0.5f);
        
        ASSERT_TRUE(neuromodulator.getModulationLevel() >= 0.0f);
        
        std::cout << "    testNeuromodulatorModulation passed" << std::endl;
    }
};

class RewardTestSuite : public TestSuite {
public:
    RewardTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Reward Test Suite..." << std::endl;
        
        testRewardCreation();
        testRewardCalculation();
        testRewardLearning();
        testRewardIntegration();
        
        std::cout << "Reward Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "RewardTestSuite";
    }
    
private:
    void testRewardCreation() {
        nlm::Brain brain("test_brain");
        nlm::Reward reward;
        
        reward.initialize(&brain);
        ASSERT_TRUE(reward.getRewardSignal() >= 0.0f);
        
        std::cout << "    testRewardCreation passed" << std::endl;
    }
    
    void testRewardCalculation() {
        nlm::Brain brain("test_brain");
        nlm::Reward reward;
        
        reward.initialize(&brain);
        
        reward.recordOutcome(1.0f, 0.9f);
        ASSERT_TRUE(reward.getRewardSignal() > 0.0f);
        
        std::cout << "    testRewardCalculation passed" << std::endl;
    }
    
    void testRewardLearning() {
        nlm::Brain brain("test_brain");
        nlm::Reward reward;
        
        reward.initialize(&brain);
        
        reward.updateValueFunction(0.8f, 1.0f);
        ASSERT_TRUE(reward.getValueEstimate() >= 0.0f);
        
        std::cout << "    testRewardLearning passed" << std::endl;
    }
    
    void testRewardIntegration() {
        nlm::Brain brain("test_brain");
        nlm::Reward reward;
        
        reward.initialize(&brain);
        
        reward.recordOutcome(0.7f, 0.8f);
        float integration = reward.getIntegratedReward();
        ASSERT_TRUE(integration >= 0.0f);
        
        std::cout << "    testRewardIntegration passed" << std::endl;
    }
};

class PredictionErrorTestSuite : public TestSuite {
public:
    PredictionErrorTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Prediction Error Test Suite..." << std::endl;
        
        testPredictionErrorCreation();
        testPredictionErrorCalculation();
        testPredictionErrorLearning();
        
        std::cout << "Prediction Error Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "PredictionErrorTestSuite";
    }
    
private:
    void testPredictionErrorCreation() {
        nlm::Brain brain("test_brain");
        nlm::PredictionError predError;
        
        predError.initialize(&brain);
        ASSERT_TRUE(predError.getPredictionError() == 0.0f);
        
        std::cout << "    testPredictionErrorCreation passed" << std::endl;
    }
    
    void testPredictionErrorCalculation() {
        nlm::Brain brain("test_brain");
        nlm::PredictionError predError;
        
        predError.initialize(&brain);
        
        predError.updatePrediction(0.8f, 1.0f);
        float error = predError.getPredictionError();
        ASSERT_TRUE(std::abs(error) > 0.0f);
        
        std::cout << "    testPredictionErrorCalculation passed" << std::endl;
    }
    
    void testPredictionErrorLearning() {
        nlm::Brain brain("test_brain");
        nlm::PredictionError predError;
        
        predError.initialize(&brain);
        
        predError.updatePrediction(0.9f, 1.0f);
        predError.updateLearningRate(0.1f);
        
        ASSERT_TRUE(predError.getLearningRate() > 0.0f);
        
        std::cout << "    testPredictionErrorLearning passed" << std::endl;
    }
};

class NoveltyTestSuite : public TestSuite {
public:
    NoveltyTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Novelty Test Suite..." << std::endl;
        
        testNoveltyCreation();
        testNoveltyDetection();
        testNoveltyLearning();
        
        std::cout << "Novelty Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "NoveltyTestSuite";
    }
    
private:
    void testNoveltyCreation() {
        nlm::Brain brain("test_brain");
        nlm::Novelty novelty;
        
        novelty.initialize(&brain);
        ASSERT_TRUE(novelty.getNoveltyScore() >= 0.0f);
        
        std::cout << "    testNoveltyCreation passed" << std::endl;
    }
    
    void testNoveltyDetection() {
        nlm::Brain brain("test_brain");
        nlm::Novelty novelty;
        
        novelty.initialize(&brain);
        
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        novelty.detectNovelty(pattern);
        ASSERT_TRUE(novelty.getNoveltyScore() >= 0.0f);
        
        std::cout << "    testNoveltyDetection passed" << std::endl;
    }
    
    void testNoveltyLearning() {
        nlm::Brain brain("test_brain");
        nlm::Novelty novelty;
        
        novelty.initialize(&brain);
        
        novelty.updateNoveltyThreshold(0.5f);
        ASSERT_TRUE(novelty.getNoveltyThreshold() > 0.0f);
        
        std::cout << "    testNoveltyLearning passed" << std::endl;
    }
};

class CuriosityTestSuite : public TestSuite {
public:
    CuriosityTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Curiosity Test Suite..." << std::endl;
        
        testCuriosityCreation();
        testCuriosityActivation();
        testCuriosityExploration();
        
        std::cout << "Curiosity Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "CuriosityTestSuite";
    }
    
private:
    void testCuriosityCreation() {
        nlm::Brain brain("test_brain");
        nlm::Curiosity curiosity;
        
        curiosity.initialize(&brain);
        ASSERT_TRUE(curiosity.getCuriosityLevel() >= 0.0f);
        
        std::cout << "    testCuriosityCreation passed" << std::endl;
    }
    
    void testCuriosityActivation() {
        nlm::Brain brain("test_brain");
        nlm::Curiosity curiosity;
        
        curiosity.initialize(&brain);
        
        std::vector<float> state = {0.1f, 0.2f, 0.3f};
        curiosity.triggerExploration(state);
        
        ASSERT_TRUE(curiosity.getCuriosityLevel() >= 0.0f);
        
        std::cout << "    testCuriosityActivation passed" << std::endl;
    }
    
    void testCuriosityExploration() {
        nlm::Brain brain("test_brain");
        nlm::Curiosity curiosity;
        
        curiosity.initialize(&brain);
        
        curiosity.setExplorationThreshold(0.3f);
        ASSERT_TRUE(curiosity.getExplorationThreshold() > 0.0f);
        
        std::cout << "    testCuriosityExploration passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createNeuromodulatorTestSuite() {
        return new NeuromodulatorTestSuite();
    }
    
    TestSuite* createRewardTestSuite() {
        return new RewardTestSuite();
    }
    
    TestSuite* createPredictionErrorTestSuite() {
        return new PredictionErrorTestSuite();
    }
    
    TestSuite* createNoveltyTestSuite() {
        return new NoveltyTestSuite();
    }
    
    TestSuite* createCuriosityTestSuite() {
        return new CuriosityTestSuite();
    }
}