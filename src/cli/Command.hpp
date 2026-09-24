#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "../core/Config/Config.hpp"

namespace nlm {

class Command {
public:
    virtual ~Command() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual std::string getHelp() const = 0;
    
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual bool execute(const Config& config) = 0;
    
    virtual bool isAdvanced() const { return false; }
    
    // For help system
    virtual const std::vector<std::string>& getExamples() const { static std::vector<std::string> empty; return empty; }
    virtual const std::unordered_map<std::string, std::string>& getParameters() const { static std::unordered_map<std::string, std::string> empty; return empty; }
};

} // namespace nlm