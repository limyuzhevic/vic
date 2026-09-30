// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Prediction system validation and error handling
 */
class PredictionValidator {
public:
    static void validatePredictionSystem(const Brain* brain);
    static void validatePredictionAccuracy(const std::vector<double>& predictions);
    static void validatePredictionParameters(const std::map<std::string, double>& params);
    static void handlePredictionError(const std::exception& e);
};

} // namespace nlm
