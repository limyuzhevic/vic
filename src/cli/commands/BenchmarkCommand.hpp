#ifndef BENCHMARKCOMMAND_HPP
#define BENCHMARKCOMMAND_HPP

#include "CLICommand.hpp"
#include "NLMCLI.hpp"

namespace nlm {

class BenchmarkCommand : public CLICommand {
public:
    BenchmarkCommand(std::shared_ptr<NLMCLI> cli);
    ~BenchmarkCommand() override = default;
    
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { return "Performance benchmarking"; }
    bool hasSubcommands() const override { return true; }
    std::vector<std::string> getSubcommands() const override;
    int execute(const std::vector<std::string>& args) override;
    int executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) override;
    
private:
    std::shared_ptr<NLMCLI> cli;
    
    int executeBasic(const std::vector<std::string>& args);
    int executeScalability(const std::vector<std::string>& args);
    int executeMemory(const std::vector<std::string>& args);
    int executeProfile(const std::vector<std::string>& args);
    int executeReport(const std::vector<std::string>& args);
};

} // namespace nlm

#endif // BENCHMARKCOMMAND_HPP
