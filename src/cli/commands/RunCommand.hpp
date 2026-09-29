#ifndef RUNCOMMAND_HPP
#define RUNCOMMAND_HPP

#include "CLICommand.hpp"
#include "NLMCLI.hpp"

namespace nlm {

class RunCommand : public CLICommand {
public:
    RunCommand(std::shared_ptr<NLMCLI> cli);
    ~RunCommand() override = default;
    
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { return "Run a simulation or experiment"; }
    bool hasSubcommands() const override { return true; }
    std::vector<std::string> getSubcommands() const override;
    int execute(const std::vector<std::string>& args) override;
    int executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) override;
    
private:
    std::shared_ptr<NLMCLI> cli;
    
    int executeBasicRun(const std::vector<std::string>& args);
    int executeQuickRun(const std::vector<std::string>& args);
    int executeLongRun(const std::vector<std::string>& args);
    int executeBatchRun(const std::vector<std::string>& args);
    int executeCheckpointRun(const std::vector<std::string>& args);
};

} // namespace nlm

#endif // RUNCOMMAND_HPP
