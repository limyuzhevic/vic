// Profile command definition
// This is the CLI command for performance profiling and benchmarking

#ifndef NLM_PROFILE_COMMAND_HPP
#define NLM_PROFILE_COMMAND_HPP

#include "Command.hpp"
#include "ProfileCommandOptions.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {
namespace cli {

// Profile command declaration
class ProfileCommand : public Command {
private:
    std::shared_ptr<ProfileCommandOptions> options;

public:
    ProfileCommand();
    ~ProfileCommand() = default;
    
    // Command interface implementation
    std::string getName() const override;
    std::string getDescription() const override;
    CommandCategory getCategory() const override;
    bool execute(const std::vector<std::string>& args) override;
    void showHelp() const override;
    std::string getUsage() const override;
    std::vector<std::string> getExamples() const override;
};

} // namespace cli
} // namespace nlm

#endif // NLM_PROFILE_COMMAND_HPP