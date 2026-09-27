// Base command class for NLM CLI
#pragma once

#include "CLIParser.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>

namespace nlm {
namespace cli {

class BaseCommand : public Subcommand {
protected:
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    
public:
    BaseCommand(const std::string& name, const std::string& description)
        : Subcommand(name, description) {}
    
    virtual ~BaseCommand() = default;
    
    void setConfig(std::shared_ptr<Config> cfg) { config = cfg; }
    void setLogger(std::shared_ptr<Logger> log) { logger = log; }
    
    virtual CommandResult execute() override {
        // Default implementation - must be overridden
        return CommandResult(1, "Command not implemented");
    }
    
    // Helper methods
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "") {
        if (config && config->has(key)) {
            auto val = config->get<std::string>(key);
            return val ? *val : defaultValue;
        }
        return defaultValue;
    }
    
    int getConfigValue(const std::string& key, int defaultValue) {
        if (config && config->has(key)) {
            auto val = config->get<int>(key);
            return val ? *val : defaultValue;
        }
        return defaultValue;
    }
    
    double getConfigValue(const std::string& key, double defaultValue) {
        if (config && config->has(key)) {
            auto val = config->get<double>(key);
            return val ? *val : defaultValue;
        }
        return defaultValue;
    }
    
    bool getConfigValue(const std::string& key, bool defaultValue) {
        if (config && config->has(key)) {
            auto val = config->get<bool>(key);
            return val ? *val : defaultValue;
        }
        return defaultValue;
    }
};

} // namespace cli
} // namespace nlm
