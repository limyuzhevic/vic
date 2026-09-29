#ifndef EXPERIMENTCOMMAND_HPP
#define EXPERIMENTCOMMAND_HPP

#include "CLICommand.hpp"
#include "NLMCLI.hpp"

namespace nlm {

class ExperimentCommand : public CLICommand {
public:
    ExperimentCommand(std::shared_ptr<NLMCLI> cli);
    ~ExperimentCommand() override = default;
    
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { return "Experiment management and analysis"; }
    bool hasSubcommands() const override { return true; }
    std::vector<std::string> getSubcommands() const override;
    int execute(const std::vector<std::string>& args) override;
    int executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) override;
    
private:
    std::shared_ptr<NLMCLI> cli;
    
    int executeCreate(const std::vector<std::string>& args);
    int executeRun(const std::vector<std::string>& args);
    int executeList(const std::vector<std::string>& args);
    int executeStatus(const std::vector<std::string>& args);
    int executeAnalyze(const std::vector<std::string>& args);
    int executeExport(const std::vector<std::string>& args);
};

} // namespace nlm

#endif // EXPERIMENTCOMMAND_HPP
