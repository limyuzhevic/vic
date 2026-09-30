// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Configuration validation and error handling
 */
class ConfigurationValidator {
public:
    static void validateConfigParameters(const std::shared_ptr<Config>& config);
    static void validateNeuronParameters(const std::map<std::string, double>& params);
    static void validateSynapseParameters(const std::map<std::string, double>& params);
    static void validateMemoryParameters(const std::map<std::string, size_t>& params);
    static void validatePlasticityParameters(const std::map<std::string, double>& params);
    static void validateNeuromodulationParameters(const std::map<std::string, double>& params);
};

} // namespace nlm
