#include "PredictionValidator.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Types/Types.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void PredictionValidator::validatePredictionSystem(const Brain* brain) {
    if (!brain) {
        throw BrainInitializationException("Brain object is null", "PredictionValidator");
    }
    
    // Validate that prediction system is properly initialized
    if (!brain->getPredictionSystem()) {
        throw BrainInitializationException("Prediction system is not initialized", "PredictionValidator");
    }
    
    // Validate prediction parameters
    if (auto predictionAccuracy = brain->getConfig()->get<double>("prediction_accuracy")) {
        Validator::validateRange(*predictionAccuracy, 0.0, 1.0, "prediction_accuracy");
    }
}

void PredictionValidator::validatePredictionAccuracy(const std::vector<double>& predictions) {
    // Validate prediction accuracy vector
    if (predictions.empty()) {
        throw BrainRuntimeException(
            "Prediction accuracy vector is empty",
            {},
            "PredictionValidator"
        );
    }
    
    // Validate prediction accuracy values
    for (size_t i = 0; i < predictions.size(); ++i) {
        if (predictions[i] < 0.0 || predictions[i] > 1.0) {
            throw BrainRuntimeException(
                "Prediction accuracy value out of valid range",
                {{ "index", std::to_string(i) },
                 { "value", std::to_string(predictions[i]) },
                 { "valid_range", "[0.0, 1.0]"}},
                "PredictionValidator"
            );
        }
    }
}

void PredictionValidator::validatePredictionParameters(const std::map<std::string, double>& params) {
    // Validate prediction parameters
    if (params.find("prediction_horizon") != params.end()) {
        double horizon = params.at("prediction_horizon");
        Validator::validateRange(horizon, 0.0, 1e6, "prediction_horizon");
    }
    
    if (params.find("prediction_confidence") != params.end()) {
        double confidence = params.at("prediction_confidence");
        Validator::validateRange(confidence, 0.0, 1.0, "prediction_confidence");
    }
}

void PredictionValidator::handlePredictionError(const std::exception& e) {
    try {
        throw;
    } catch (const BrainException& be) {
        LOG_ERROR("Prediction system error: " + std::string(be.whatWithDetails()));
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected prediction system error: " + std::string(e.what()));
        throw BrainRuntimeException(
            "Unexpected error in prediction system",
            {{ "error", std::string(e.what()) }},
            "PredictionValidator"
        );
    }
}

} // namespace nlm
