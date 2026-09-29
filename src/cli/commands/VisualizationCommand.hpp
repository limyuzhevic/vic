#ifndef VISUALIZATIONCOMMAND_HPP
#define VISUALIZATIONCOMMAND_HPP

#include "CLICommand.hpp"
#include "NLMCLI.hpp"

namespace nlm {

class VisualizationCommand : public CLICommand {
public:
    VisualizationCommand(std::shared_ptr<NLMCLI> cli);
    ~VisualizationCommand() override = default;
    
    std::string getName() const override { return "visualize"; }
    std::string getDescription() const override { return "Visualization and analysis"; }
    bool hasSubcommands() const override { return true; }
    std::vector<std::string> getSubcommands() const override;
    int execute(const std::vector<std::string>& args) override;
    int executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) override;
    
private:
    std::shared_ptr<NLMCLI> cli;
    
    int executeMetrics(const std::vector<std::string>& args);
    int executeMemory(const std::vector<std::string>& args);
    int executeLearning(const std::vector<std::string>& args);
    int executeNetwork(const std::vector<std::string>& args);
    int executeResults(const std::vector<std::string>& args);
};

} // namespace nlm

#endif // VISUALIZATIONCOMMAND_HPP
