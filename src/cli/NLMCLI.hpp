#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"

namespace nlm {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Execute the parsed command
    int execute();
    
    // Display help for a specific command
    void showHelp(const std::string& command = "") const;
    
    // Display configuration validation report
    void showValidationReport(const std::vector<ValidationError>& errors) const;
    
    // Display configuration summary
    void showConfigSummary(const Config& config) const;

private:
    // Command parsing
    struct Command {
        std::string name;
        std::string description;
        std::function<int(const std::vector<std::string>&)> handler;
    };
    
    std::string programName;
    std::string currentConfigFile;
    std::shared_ptr<Config> config;
    std::map<std::string, Command> commands;
    
    // Command handlers
    int handleLoadCommand(const std::vector<std::string>& args);
    int handleSaveCommand(const std::vector<std::string>& args);
    int handleValidateCommand(const std::vector<std::string>& args);
    int handleExportCommand(const std::vector<std::string>& args);
    int handleImportCommand(const std::vector<std::string>& args);
    int handleSetCommand(const std::vector<std::string>& args);
    int handleGetCommand(const std::vector<std::string>& args);
    int handleListCommand(const std::vector<std::string>& args);
    int handleLogConfigCommand(const std::vector<std::string>& args);
    int handleExperimentCommand(const std::vector<std::string>& args);
    int handleHistoryCommand(const std::vector<std::string>& args);
    int handleCompletionCommand(const std::vector<std::string>& args);
    
    // Helper methods
    bool parseCommandLine(int argc, char** argv);
    void registerCommands();
    std::vector<std::string> splitArgs(const std::string& args) const;
    bool resolveConfigPath(const std::string& configPath);
};

} // namespace nlm

#endif // NLM_CLI_HPP
