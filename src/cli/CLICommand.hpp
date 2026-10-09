#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class CLICommand {
public:
    virtual ~CLICommand() = default;
    
    // Get command name
    virtual std::string getName() const = 0;
    
    // Get command description
    virtual std::string getDescription() const = 0;
    
    // Get command usage
    virtual std::string getUsage() const = 0;
    
    // Execute command with arguments
    virtual int execute(const std::vector<std::string>& args) = 0;
    
    // Check if command accepts arguments
    virtual bool acceptsArguments() const = 0;
    
    // Get required argument count
    virtual int getRequiredArgCount() const = 0;
    
    // Get optional arguments help
    virtual std::string getOptionalArgsHelp() const = 0;
};

} // namespace nlm