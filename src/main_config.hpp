#pragma once

#include <memory>
#include <string>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class Config;
class ConfigSource;

class ConfigManager {
public:
    static std::shared_ptr<Config> createConfig(int argc, char** argv);
    static void logConfigurationSummary(const Config& config);
};

} // namespace nlm