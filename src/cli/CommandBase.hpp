#ifndef NLM_CLI_COMMAND_BASE_HPP
#define NLM_CLI_COMMAND_BASE_HPP

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace nlm {
namespace cli {

class CommandBase {
protected:
    std::string name_;
    std::string description_;
    
public:
    CommandBase(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    virtual ~CommandBase() = default;
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    virtual bool execute(const std::map<std::string, std::string>& args) = 0;
    virtual void printHelp() const {
        std::cout << "Usage: nlm-cli " << name_ << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << description_ << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        printOptions();
    }
    
protected:
    virtual void printOptions() const {
        std::cout << "  [command options]" << std::endl;
    }
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_COMMAND_BASE_HPP
