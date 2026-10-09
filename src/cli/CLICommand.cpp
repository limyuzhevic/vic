// CLICommand implementation
//
// Base implementation for CLI command functionality

#include "CLICommand.hpp"

namespace nlm {

// Default implementations
std::string CLICommand::getUsage() const {
    return "";
}

bool CLICommand::acceptsArguments() const {
    return false;
}

int CLICommand::getRequiredArgCount() const {
    return 0;
}

std::string CLICommand::getOptionalArgsHelp() const {
    return "";
}

} // namespace nlm