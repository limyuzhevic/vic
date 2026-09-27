#ifndef NLM_CLI_CONFIG_COMMAND_HPP
#define NLM_CLI_CONFIG_COMMAND_HPP

#include "cli/CommandBase.hpp"

namespace nlm {
namespace cli {

class ConfigCommand : public CommandBase {
private:
    std::string action_;
    std::string output_file_;
    bool validate_only_;
    
public:
    ConfigCommand();
    
    bool execute(const std::map<std::string, std::string>& args) override;
    void printHelp() const override;
    
protected:
    void printOptions() const override;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_CONFIG_COMMAND_HPP
