// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Developmental system validation and error handling
 */
class DevelopmentalValidator {
public:
    static void validateDevelopmentalStage(const DevelopmentalStage& stage);
    static void validateStageTransition(const DevelopmentalStage& from, const DevelopmentalStage& to);
    static void validateCriticalPeriod(const std::map<std::string, double>& period);
    static void handleDevelopmentalError(const std::exception& e);
};

} // namespace nlm
