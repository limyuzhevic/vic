// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Neuromodulation system validation and error handling
 */
class NeuromodulationValidator {
public:
    static void validateNeuromodulatorLevels(const Neuromodulator& signal);
    static void validateNeuromodulationParameters(const std::map<std::string, double>& params);
    static void validateNeuromodulationTiming(const std::map<std::string, double>& timing);
    static void handleNeuromodulationError(const std::exception& e);
};

} // namespace nlm
