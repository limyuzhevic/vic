// CLICommand.hpp - Base class for all CLI tools
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "core/Config/Config.hpp"

namespace nlm {

class CLICommand {
public:
    enum class CommandType {
        PROFILING,
        DEBUGGING,
        INSTRUMENTATION,
        ANALYSIS,
        REPORT,
        HEALTH,
        OPTIMIZATION
    };
    
    enum class ExecutionResult {
        SUCCESS,
        FAILURE,
        VALIDATION_ERROR,
        FILE_ERROR,
        NOT_IMPLEMENTED
    };
    
    CLICommand() = default;
    virtual ~CLICommand() = default;
    
    // Command information
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual CommandType getType() const = 0;
    virtual std::string getCategory() const = 0;
    
    // Command execution
    virtual ExecutionResult execute(const std::vector<std::string>& args, Config& config) = 0;
    
    // Help and documentation
    virtual void displayHelp() const {
        std::cout << "=== " << getName() << " Help ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Description: " << getDescription() << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << getUsage() << std::endl;
        std::cout << std::endl;
        std::cout << "Category: " << getCategory() << std::endl;
    }
    
    // Validation
    virtual bool validateArgs(const std::vector<std::string>& args) const {
        return true;
    }
};

} // namespace nlm
