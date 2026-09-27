#ifndef NLM_CLI_COMMAND_RUNNER_HPP
#define NLM_CLI_COMMAND_RUNNER_HPP

#include <string>
#include <map>
#include <memory>
#include "cli/CommandBase.hpp"

namespace nlm {
namespace cli {

class CommandRunner {
private:
    std::map<std::string, CommandBase*> commands_;
    
public:
    CommandRunner();
    ~CommandRunner();
    
    void registerCommand(CommandBase* command);
    
    bool executeCommand(const std::string& name, const std::map<std::string, std::string>& args);
    void printHelp(const std::string& name) const;
    void printAllHelp() const;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_COMMAND_RUNNER_HPP
