// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Plasticity system validation and error handling
 */
class PlasticityValidator {
public:
    static void validatePlasticityRules(const PlasticityFlags& flags);
    static void validatePlasticityParameters(const std::map<std::string, double>& params);
    static void validateLearningRules(const std::vector<std::string>& rules);
    static void validateWeightChanges(const Synapse& synapse, double delta);
    static void handlePlasticityError(const std::exception& e);
};

} // namespace nlm
