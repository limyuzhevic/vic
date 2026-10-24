#ifndef NLM_MAIN_CLI_HPP
#define NLM_MAIN_CLI_HPP

#include "cli/CLI.hpp"
#include "cli/RunCommand.hpp"
#include "cli/ConfigCommand.hpp"
#include "cli/ExperimentCommand.hpp"
#include "cli/UtilsCommand.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <iostream>
#include <string>

namespace nlm {
namespace cli {

class NLMMainCLI : public CLI {
public:
    NLMMainCLI();
    ~NLMMainCLI() = default;
    
    void setupCommands();
    Result initialize() const;
    Result cleanup() const;
    
    // Configuration management
    bool loadConfiguration(const std::string& configFile = "");
    bool saveConfiguration(const std::string& configFile) const;
    
    // Utility functions
    static std::string getDefaultConfigPath();
    static std::string getDefaultLogPath();
    static std::string getDefaultExperimentPath();
    
private:
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Config> config_;
    std::string configFile_;
    
    // Command instances
    std::unique_ptr<RunCommand> runCommand_;
    std::unique_ptr<ConfigCommand> configCommand_;
    std::unique_ptr<ExperimentCommand> experimentCommand_;
    std::unique_ptr<UtilsCommand> utilsCommand_;
    
    // Internal setup
    void setupLogger();
    void setupDefaultConfiguration();
    void setupCommandHandlers();
    
    // Error handling
    static void handleFatalError(const std::string& message);
    static void handleError(const CLIError& error);
};

} // namespace cli
} // namespace nlm

#endif // NLM_MAIN_CLI_HPP