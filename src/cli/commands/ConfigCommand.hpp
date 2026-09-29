#ifndef CONFIGCOMMAND_HPP
#define CONFIGCOMMAND_HPP

#include "CLICommand.hpp"
#include "NLMCLI.hpp"
#include <string>

namespace nlm {

class ConfigCommand : public CLICommand {
public:
    ConfigCommand(std::shared_ptr<NLMCLI> cli);
    ~ConfigCommand() override = default;
    
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { return "Configuration management"; }
    bool hasSubcommands() const override { return true; }
    std::vector<std::string> getSubcommands() const override;
    int execute(const std::vector<std::string>& args) override;
    int executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) override;
    
private:
    std::shared_ptr<NLMCLI> cli;
    
    int executeGenerate(const std::vector<std::string>& args);
    int executeValidate(const std::vector<std::string>& args);
    int executeMerge(const std::vector<std::string>& args);
    int executeExport(const std::vector<std::string>& args);
    int executeImport(const std::vector<std::string>& args);
    int executeShow(const std::vector<std::string>& args);
};

} // namespace nlm

#endif // CONFIGCOMMAND_HPP
