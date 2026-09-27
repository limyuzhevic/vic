#ifndef NLM_CLI_BENCHMARK_COMMAND_HPP
#define NLM_CLI_BENCHMARK_COMMAND_HPP

#include "cli/CommandBase.hpp"

namespace nlm {
namespace cli {

class BenchmarkCommand : public CommandBase {
private:
    int neurons_;
    int steps_;
    std::string output_format_;
    std::string benchmark_name_;
    
public:
    BenchmarkCommand();
    
    bool execute(const std::map<std::string, std::string>& args) override;
    void printHelp() const override;
    
protected:
    void printOptions() const override;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_BENCHMARK_COMMAND_HPP
