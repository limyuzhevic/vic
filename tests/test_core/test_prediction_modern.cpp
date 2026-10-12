#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include <cmath>

class PredictionSystemTestSuite : public TestSuite {
public:
    PredictionSystemTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Prediction System Test Suite..." << std::endl;
        
        testPredictionSystemCreation();
        testPredictionSystemLearning();
        testPredictionSystemConfidence();
        testPredictionSystemUpdates();
        
        std::cout << "Prediction System Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "PredictionSystemTestSuite";
    }
    
private:
    void testPredictionSystemCreation() {
        nlm::Brain brain("test_brain");
        nlm::PredictionSystem predictor;
        
        predictor.initialize(&brain);
        ASSERT_TRUE(predictor.getPredictionCount() == 0);
        
        std::cout << "    testPredictionSystemCreation passed" << std::endl;
    }
    
    void testPredictionSystemLearning() {
        nlm::Brain brain("test_brain");
        nlm::PredictionSystem predictor;
        
        predictor.initialize(&brain);
        
        std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        predictor.recordObservation(state);
        
        ASSERT_TRUE(predictor.getPredictionCount() > 0);
        
        std::cout << "    testPredictionSystemLearning passed" << std::endl;
    }
    
    void testPredictionSystemConfidence() {
        nlm::Brain brain("test_brain");
        nlm::PredictionSystem predictor;
        
        predictor.initialize(&brain);
        
        std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        predictor.makePrediction(state, nlm::ActionType::Move);
        
        float confidence = predictor.getPredictionConfidence();
        ASSERT_TRUE(confidence >= 0.0f && confidence <= 1.0f);
        
        std::cout << "    testPredictionSystemConfidence passed" << std::endl;
    }
    
    void testPredictionSystemUpdates() {
        nlm::Brain brain("test_brain");
        nlm::PredictionSystem predictor;
        
        predictor.initialize(&brain);
        
        std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        predictor.updatePrediction(state, 1.0f);
        
        float prediction = predictor.getCurrentPrediction();
        ASSERT_TRUE(prediction >= 0.0f);
        
        std::cout << "    testPredictionSystemUpdates passed" << std::endl;
    }
};

class NeuralPredictionTestSuite : public TestSuite {
public:
    NeuralPredictionTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Neural Prediction Test Suite..." << std::endl;
        
        testNeuralPredictionCreation();
        testNeuralPredictionActivation();
        testNeuralPredictionDecay();
        testNeuralPredictionUpdate();
        
        std::cout << "Neural Prediction Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "NeuralPredictionTestSuite";
    }
    
private:
    void testNeuralPredictionCreation() {
        nlm::Brain brain("test_brain");
        nlm::NeuralPrediction prediction;
        
        prediction.initialize(&brain);
        ASSERT_TRUE(prediction.getPredictionStrength() == 0.0f);
        
        std::cout << "    testNeuralPredictionCreation passed" << std::endl;
    }
    
    void testNeuralPredictionActivation() {
        nlm::Brain brain("test_brain");
        nlm::NeuralPrediction prediction;
        
        prediction.initialize(&brain);
        
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        prediction.activatePattern(pattern);
        
        float strength = prediction.getPredictionStrength();
        ASSERT_TRUE(strength >= 0.0f);
        
        std::cout << "    testNeuralPredictionActivation passed" << std::endl;
    }
    
    void testNeuralPredictionDecay() {
        nlm::Brain brain("test_brain");
        nlm::NeuralPrediction prediction;
        
        prediction.initialize(&brain);
        
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        prediction.activatePattern(pattern);
        
        prediction.updateDecay(0.1f);
        
        float strength = prediction.getPredictionStrength();
        ASSERT_TRUE(strength >= 0.0f);
        
        std::cout << "    testNeuralPredictionDecay passed" << std::endl;
    }
    
    void testNeuralPredictionUpdate() {
        nlm::Brain brain("test_brain");
        nlm::NeuralPrediction prediction;
        
        prediction.initialize(&brain);
        
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        prediction.updatePattern(pattern, 0.8f);
        
        float strength = prediction.getPredictionStrength();
        ASSERT_TRUE(strength >= 0.0f);
        
        std::cout << "    testNeuralPredictionUpdate passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createPredictionSystemTestSuite() {
        return new PredictionSystemTestSuite();
    }
    
    TestSuite* createNeuralPredictionTestSuite() {
        return new NeuralPredictionTestSuite();
    }
}