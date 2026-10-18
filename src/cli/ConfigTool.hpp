// ConfigTool.hpp - Base configuration tool
#pragma once

#include "CLICommand.hpp"
#include <string>

namespace nlm {

class ConfigTool : public CLICommand {
public:
    ConfigTool() = default;
    virtual ~ConfigTool() = default;
    
    CommandType getType() const override {
        return CommandType::PROFILING;
    }
    
    std::string getCategory() const override {
        return "Configuration Management";
    }
    
protected:
    void printInfo(const std::string& message) const {
        std::cout << "[INFO] " << message << std::endl;
    }
    
    void printSuccess(const std::string& message) const {
        std::cout << "[SUCCESS] " << message << std::endl;
    }
    
    void printWarning(const std::string& message) const {
        std::cout << "[WARNING] " << message << std::endl;
    }
    
    void printError(const std::string& message) const {
        std::cout << "[ERROR] " << message << std::endl;
    }
};

} // namespace nlm
