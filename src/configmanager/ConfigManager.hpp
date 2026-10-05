#pragma once

#include "core/Config/Config.hpp"
#include "cli/CLI.hpp"
#include <string>
#include <map>

namespace nlm {

class ConfigManager {
public:
    static bool loadConfigFromFile(std::shared_ptr<Config> config, const std::string& configFile);
    static void applyDefaults(std::shared_ptr<Config> config);
    static void applyOptions(std::shared_ptr<Config> config, const CLI::Options& options);
    static void printConfigSummary(const std::shared_ptr<Config>& config);
    static std::map<std::string, std::string> parseAdvancedParams(const std::string& paramStr);
};

} // namespace nlm