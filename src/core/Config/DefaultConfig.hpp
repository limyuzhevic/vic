#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>

namespace nlm {

class DefaultConfig {
public:
    static void applyPhase2Defaults(Config& config);
    static void applyPhase6Defaults(Config& config);
    static std::string getConfigSummary(const Config& config);
};

} // namespace nlm
