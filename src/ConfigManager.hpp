#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <memory>
#include "CLI.hpp"
#include "core/Config/Config.hpp"
#include "ConfigSchema.hpp"

namespace nlm {

class ConfigManager {
public:
    static std::shared_ptr<Config> setupConfig(const CLI::Options& cliOptions);
    
private:
    static void logConfigurationSummary(const Config& config, bool verbose);
    static std::string getSourceString(ConfigSource source);
};

} // namespace nlm

#endif // CONFIG_MANAGER_HPP
