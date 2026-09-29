#ifndef CLICOMMAND_HPP
#define CLICOMMAND_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "NLMCLI.hpp"

namespace nlm {

class CLICommand {
public:
    virtual ~CLICommand() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool hasSubcommands() const { return false; }
    virtual std::vector<std::string> getSubcommands() const { return {}; }
    virtual int execute(const std::vector<std::string>& args) = 0;
    virtual int executeSubcommand(const std::string& subcommand, const std::vector<std::string>& args) { return 1; }
    
protected:
    void printUsage(const std::string& command) const {
        std::cout << "Usage: " << command << " [OPTIONS]" << std::endl;
    }
    
    void printError(const std::string& message) const {
        std::cerr << "Error: " << message << std::endl;
    }
    
    void printInfo(const std::string& message) const {
        std::cout << "Info: " << message << std::endl;
    }
    
    void printSuccess(const std::string& message) const {
        std::cout << "Success: " << message << std::endl;
    }
    
    void printWarning(const std::string& message) const {
        std::cerr << "Warning: " << message << std::endl;
    }
};

} // namespace nlm

#endif // CLICOMMAND_HPP
