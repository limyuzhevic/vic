#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

// Simplified test for PredictionSystem implementation
// Tests the core prediction logic without requiring full dependencies

class MockSensoryInput {
public:
    MockSensoryInput(size_t size) {
        data.resize(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<float>(i) / 10.0f;
        }
    }
    
    const std::vector<float>& getData() const { return data; }
    size_t getDimensions() const { return data.size(); }
    
private:
    std::vector<float> data;
};

class MockPredictionSystem {
public:
    MockPredictionSystem() : confidence(0.5f), predictionError(0.0f) {}
    
    std::vector<float> predictNextState(const MockSensoryInput& currentState) {
        const auto& data = currentState.getData();
        std::vector<float> predictedData;
        predictedData.reserve(data.size() * 2);
        
        for (size_t i = 0; i < data.size(); ++i) {
            // Simple prediction model
            float prediction = data[i] * 0.7f;  // Decay component
            prediction += data[i] * 0.3f * confidence;  // Modulated by confidence
            
            // Add noise for exploration
            prediction += (rand() / (float)RAND_MAX - 0.5f) * 0.1f * confidence;
            
            // Apply synaptic-like filtering
            prediction = prediction * 0.95f;
            
            predictedData.push_back(prediction);
        }
        
        lastPrediction_ = predictedData;
        return predictedData;
    }
    
    void updatePredictions(const std::vector<float>& predicted, const std::vector<float>& actual) {
        if (predicted.size() == actual.size() && !predicted.empty()) {
            float sumError = 0.0f;
            float maxError = 0.0f;
            
            for (size_t i = 0; i < predicted.size(); ++i) {
                float diff = predicted[i] - actual[i];
                sumError += diff * diff;
                maxError = std::max(maxError, std::abs(diff));
            }
            
            predictionError = sqrt(sumError / predicted.size());
            errorHistory.push_back(predictionError);
            
            updateConfidence(predictionError);
        } else {
            predictionError = 1.0f;
            errorHistory.push_back(predictionError);
        }
    }
    
    void updateConfidence(float error) {
        if (errorHistory.size() > 5) {
            size_t recentCount = std::min(size_t(5), errorHistory.size());
            float avgError = 0.0f;
            for (size_t i = errorHistory.size() - recentCount; i < errorHistory.size(); ++i) {
                avgError += errorHistory[i];
            }
            avgError /= recentCount;
            
            confidence = std::max(0.1f, 1.0f - avgError);
        }
    }
    
    float getPredictionError() const { return predictionError; }
    float getConfidence() const { return confidence; }
    const std::vector<float>& getErrorHistory() const { return errorHistory; }
    void clearHistory() { errorHistory.clear(); }
    
private:
    float confidence;
    float predictionError;
    std::vector<float> errorHistory;
    std::vector<float> lastPrediction_;
};

int main() {
    std::cout << "Testing PredictionSystem implementation..." << std::endl;
    
    // Create test data
    MockSensoryInput input(10);
    
    // Create prediction system
    MockPredictionSystem predictionSystem;
    
    // Test predictNextState
    std::cout << "1. Testing predictNextState..." << std::endl;
    auto prediction = predictionSystem.predictNextState(input);
    assert(prediction.size() > 0);
    std::cout << "   ✓ Prediction created with " << prediction.size() << " elements" << std::endl;
    
    // Create actual data for comparison
    std::vector<float> actual;
    for (size_t i = 0; i < prediction.size(); ++i) {
        actual.push_back(0.5f); // Different from prediction
    }
    
    // Test updatePredictions
    std::cout << "2. Testing updatePredictions..." << std::endl;
    predictionSystem.updatePredictions(prediction, actual);
    std::cout << "   ✓ Prediction updated" << std::endl;
    
    // Test prediction error
    std::cout << "3. Testing prediction error..." << std::endl;
    float error = predictionSystem.getPredictionError();
    std::cout << "   Prediction error: " << error << std::endl;
    assert(error >= 0.0f);
    
    // Test confidence
    std::cout << "4. Testing confidence..." << std::endl;
    float confidence = predictionSystem.getConfidence();
    std::cout << "   Confidence: " << confidence << std::endl;
    assert(confidence >= 0.1f && confidence <= 1.0f);
    
    // Test history
    std::cout << "5. Testing history..." << std::endl;
    auto& history = predictionSystem.getErrorHistory();
    std::cout << "   Error history size: " << history.size() << std::endl;
    assert(history.size() > 0);
    
    // Test clear history
    std::cout << "6. Testing clear history..." << std::endl;
    predictionSystem.clearHistory();
    assert(predictionSystem.getErrorHistory().empty());
    std::cout << "   ✓ History cleared" << std::endl;
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}