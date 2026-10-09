#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

class HelpSystem {
public:
    HelpSystem();
    ~HelpSystem();
    
    // Load help information for all commands
    void loadCommandHelp();
    
    // Show help for a specific command
    int showCommandHelp(const std::string& commandName) const;
    
    // Show general help
    void showGeneralHelp() const;
    
    // Get help text for a command
    std::string getCommandHelp(const std::string& commandName) const;
    
    // Add custom help text for a command
    void addCommandHelp(const std::string& commandName, 
                       const std::string& description,
                       const std::string& usage,
                       const std::vector<std::string>& examples = {});
    
private:
    struct CommandHelp {
        std::string description;
        std::string usage;
        std::vector<std::string> examples;
        bool loaded;
    };
    
    std::unordered_map<std::string, CommandHelp> commandHelp;
    
    // Default help text for built-in commands
    void loadDefaultHelp() const;
};

} // namespace nlm