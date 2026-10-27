#pragma once

#include <string>
#include <vector>
#include "core/Config/Config.hpp"

namespace nlm {

class ConfigurationValidator {
public:
    ConfigurationValidator() = default;
    ~ConfigurationValidator() = default;
    
    // Validate a configuration entry
    std::vector<std::string> validate(const ConfigEntry& entry) const;
    
    // Validate specific values
    bool validateNeuronCount(int64_t count) const;
    bool validateRegionCount(int64_t count) const;
    bool validateConnectionProbability(float prob) const;
    bool validateTimestep(double timestep) const;
};

} // namespace nlm