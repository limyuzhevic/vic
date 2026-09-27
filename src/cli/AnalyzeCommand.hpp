#ifndef NLM_CLI_ANALYZE_COMMAND_HPP
#define NLM_CLI_ANALYZE_COMMAND_HPP

#include "cli/CommandBase.hpp"

namespace nlm {
namespace cli {

class AnalyzeCommand : public CommandBase {
private:
    std::string input_file_;
    std::string output_file_;
    std::string analysis_type_;
    
public:
    AnalyzeCommand();
    
    bool execute(const std::map<std::string, std::string>& args) override;
    void printHelp() const override;
    
protected:
    void printOptions() const override;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_ANALYZE_COMMAND_HPP
