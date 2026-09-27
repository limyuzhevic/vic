#ifndef NLM_CLI_RUN_COMMAND_HPP
#define NLM_CLI_RUN_COMMAND_HPP

#include "cli/CommandBase.hpp"

namespace nlm {
namespace cli {

class RunCommand : public CommandBase {
private:
    std::string config_file_;
    int steps_;
    double duration_;
    bool parallel_;
    int threads_;
    
public:
    RunCommand();
    
    bool execute(const std::map<std::string, std::string>& args) override;
    void printHelp() const override;
    
protected:
    void printOptions() const override;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_RUN_COMMAND_HPP
