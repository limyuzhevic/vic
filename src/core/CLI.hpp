#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "../core/Config/Config.hpp"

namespace nlm {

class CLI {
public:
    CLI();
    ~CLI();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Get help text
    std::string getHelp() const;
    
    // Get version information
    std::string getVersion() const;
    
    // Get command line arguments
    const std::vector<std::string>& getArgs() const { return args_; }
    
    // Register CLI commands
    void registerCommand(const std::string& name, 
                         const std::string& description,
                         std::function<bool(const std::vector<std::string>&)> handler);
    
    // Execute a registered command
    bool executeCommand(const std::string& name, const std::vector<std::string>& args);
    
    // Print usage
    void printUsage(const std::string& programName) const;
    
private:
    // Parse configuration from command line
    bool parseConfigFromArgs(int argc, char** argv);
    
    // Parse configuration from file
    bool parseConfigFromFile(const std::string& filepath);
    
    // Parse configuration from key=value pairs
    bool parseConfigFromKeyValue(const std::string& key, const std::string& value);
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Command line arguments
    std::vector<std::string> args_;
    
    // Registered commands
    std::map<std::string, 
             std::pair<std::string, std::function<bool(const std::vector<std::string>&)>>> commands_;
    
    // Help text
    std::string helpText_;
    
    // Version string
    std::string version_;
};

} // namespace nlm