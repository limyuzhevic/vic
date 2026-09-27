#ifndef NLM_CLI_HELP_COMMAND_HPP
#define NLM_CLI_HELP_COMMAND_HPP

#include "cli/CommandBase.hpp"

namespace nlm {
namespace cli {

class HelpCommand : public CommandBase {
public:
    HelpCommand();
    
    bool execute(const std::map<std::string, std::string>& args) override;
    void printHelp() const override;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_HELP_COMMAND_HPP
