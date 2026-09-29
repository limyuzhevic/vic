#ifndef ANALYSISCOMMAND_HPP
#define ANALYSISCOMMAND_HPP

#include "CLICommand.hpp"
#include "NLMCLI.hpp"

namespace nlm {

class AnalysisCommand : public CLICommand {
public:
    AnalysisCommand(std::shared_ptr<NLMCLI> cli);
    ~AnalysisCommand() override = default;
    
    std::string getName() const override { return "analyze"; }
    std::string getDescription() const override { return "Data analysis and reporting"; }
    bool hasSubcommands() const override { return true; }
    std::vector<std::string> getSubcommands() const override;
    int execute(const std::vector<std::string>& args) override;
    int executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) override;
    
private:
    std::shared_ptr<NLMCLI> cli;
    
    int executeResults(const std::vector<std::string>& args);
    int executePatterns(const std::vector<std::string>& args);
    int executeTrends(const std::vector<std::string>& args);
    int executeCompare(const std::vector<std::string>& args);
    int executeReport(const std::vector<std::string>& args);
};

} // namespace nlm

#endif // ANALYSISCOMMAND_HPP
