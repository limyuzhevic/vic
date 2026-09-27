#ifndef NLM_CLI_VISUALIZE_COMMAND_HPP
#define NLM_CLI_VISUALIZE_COMMAND_HPP

#include "cli/CommandBase.hpp"

namespace nlm {
namespace cli {

class VisualizeCommand : public CommandBase {
private:
    std::string data_file_;
    std::string output_format_;
    std::string visualization_type_;
    
public:
    VisualizeCommand();
    
    bool execute(const std::map<std::string, std::string>& args) override;
    void printHelp() const override;
    
protected:
    void printOptions() const override;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_VISUALIZE_COMMAND_HPP
