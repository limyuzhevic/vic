#ifndef NLM_CLI_EXPERIMENT_COMMAND_HPP
#define NLM_CLI_EXPERIMENT_COMMAND_HPP

#include "cli/CommandBase.hpp"

namespace nlm {
namespace cli {

class ExperimentCommand : public CommandBase {
private:
    std::string experiment_name_;
    int phase_;
    std::string param_file_;
    bool parallel_;
    
public:
    ExperimentCommand();
    
    bool execute(const std::map<std::string, std::string>& args) override;
    void printHelp() const override;
    
protected:
    void printOptions() const override;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_EXPERIMENT_COMMAND_HPP
