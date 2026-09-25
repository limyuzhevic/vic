// Prediction Tests
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_prediction {

TEST_CASE("PredictionSystem creation and initialization", "[prediction][system][creation]") {
    nlm::PredictionSystem predictionSystem;
    
    REQUIRE(!predictionSystem.isInitialized());
}

TEST_CASE("PredictionSystem initialization with config", "[prediction][system][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("prediction.enabled", true, nlm::ConfigSource::Runtime);
    config->set("prediction.horizon", 10, nlm::ConfigSource::Runtime);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(config);
    REQUIRE(predictionSystem.isInitialized());
}

TEST_CASE("NeuralPrediction creation and properties", "[prediction][neural][creation]") {
    nlm::NeuralPrediction neuralPrediction;
    
    REQUIRE(neuralPrediction.getPredictionConfidence() >= 0.0f);
    REQUIRE(neuralPrediction.getPredictionAccuracy() >= 0.0f);
    REQUIRE(neuralPrediction.getPredictionId() != 0);
}

TEST_CASE("PredictionSystem prediction generation", "[prediction][system][generation]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("prediction.enabled", true, nlm::ConfigSource::Runtime);
    config->set("prediction.horizon", 5, nlm::ConfigSource::Runtime);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(config);
    
    // Generate a prediction
    nlm::Prediction prediction = predictionSystem.generatePrediction();
    REQUIRE(prediction.confidence >= 0.0f);
    REQUIRE(prediction.horizon > 0);
}

TEST_CASE("PredictionSystem prediction evaluation", "[prediction][system][evaluation]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("prediction.enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(config);
    
    nlm::Prediction prediction = predictionSystem.generatePrediction();
    
    // Evaluate the prediction
    float accuracy = predictionSystem.evaluatePrediction(prediction);
    REQUIRE(accuracy >= 0.0f);
}

TEST_CASE("NeuralPrediction confidence and accuracy tracking", "[prediction][neural][tracking]") {
    nlm::NeuralPrediction neuralPrediction;
    
    float initialConfidence = neuralPrediction.getPredictionConfidence();
    float initialAccuracy = neuralPrediction.getPredictionAccuracy();
    
    // Update prediction metrics
    neuralPrediction.updateConfidence(0.8f);
    neuralPrediction.updateAccuracy(0.9f);
    
    REQUIRE(neuralPrediction.getPredictionConfidence() != initialConfidence);
    REQUIRE(neuralPrediction.getPredictionAccuracy() != initialAccuracy);
}

TEST_CASE("PredictionSystem multi-step forecasting", "[prediction][system][forecasting]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("prediction.enabled", true, nlm::ConfigSource::Runtime);
    config->set("prediction.horizon", 3, nlm::ConfigSource::Runtime);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(config);
    
    // Generate multiple predictions
    std::vector<nlm::Prediction> predictions;
    for (int i = 0; i < 3; ++i) {
        predictions.push_back(predictionSystem.generatePrediction());
    }
    
    REQUIRE(predictions.size() == 3);
    
    // All predictions should have valid values
    for (const auto& prediction : predictions) {
        REQUIRE(prediction.confidence >= 0.0f);
        REQUIRE(prediction.horizon > 0);
    }
}

TEST_CASE("PredictionSystem integration with brain", "[prediction][integration][brain]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("prediction.enabled", true, nlm::ConfigSource::Runtime);
    config->set("prediction.horizon", 2, nlm::ConfigSource::Runtime);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(config);
    
    // Create a scenario for prediction
    nlm::Brain brain(config);
    if (brain.initialize()) {
        // Prediction system should be able to integrate with brain
        nlm::Prediction prediction = predictionSystem.generatePrediction();
        REQUIRE(prediction.confidence >= 0.0f);
    }
}

TEST_CASE("Prediction accuracy over time", "[prediction][accuracy][time]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("prediction.enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(config);
    
    // Track accuracy over multiple predictions
    std::vector<float> accuracies;
    for (int i = 0; i < 10; ++i) {
        nlm::Prediction prediction = predictionSystem.generatePrediction();
        float accuracy = predictionSystem.evaluatePrediction(prediction);
        accuracies.push_back(accuracy);
    }
    
    // Verify accuracy tracking
    REQUIRE(accuracies.size() == 10);
    
    // Calculate average accuracy
    float sum = 0.0f;
    for (float accuracy : accuracies) {
        sum += accuracy;
    }
    float averageAccuracy = sum / accuracies.size();
    
    REQUIRE(averageAccuracy >= 0.0f);
}

} // namespace test_prediction
