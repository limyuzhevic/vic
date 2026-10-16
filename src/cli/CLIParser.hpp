#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>

namespace nlm {

class CLIParser {
public:
    struct Option {
        std::string name;
        char shortName;
        std::string description;
        std::string defaultValue;
        std::string type;
        bool required = false;
        std::string value;
        bool hasValue = false;
        std::function<bool(const std::string&)> validator;
        std::vector<std::string> validValues;
        std::string example;
    };
    
    struct Command {
        std::string name;
        std::string description;
        std::vector<Option> options;
        std::function<void(const CLIParser&)> action;
        bool requiresInteraction = false;
    };
    
    CLIParser();
    ~CLIParser();
    
    void addOption(const std::string& name, char shortName, const std::string& description, 
                   const std::string& defaultValue = "", const std::string& type = "string",
                   bool required = false, const std::vector<std::string>& validValues = {});
    
    void addOptionValidator(const std::string& name, std::function<bool(const std::string&)> validator);
    
    void addCommand(const std::string& name, const std::string& description, 
                   const std::vector<Option>& options = {});
    
    bool parse(int argc, char** argv);
    bool parse(const std::vector<std::string>& args);
    
    std::string getOptionValue(const std::string& name, const std::string& defaultValue = "") const;
    bool hasOption(const std::string& name) const;
    
    bool executeCommand(const std::string& name);
    
    void printHelp(const std::string& commandName = "") const;
    void printVersion() const;
    
    void setInteractive(bool interactive);
    bool isInteractive() const;
    
    void setOutputFile(const std::string& filepath);
    
    void enableCommandHistory(const std::string& historyFile = "");
    void enableTabCompletion(const std::vector<std::string>& completions = {});
    
    std::vector<std::string> getCommandHistory() const;
    void clearCommandHistory();
    
    std::string getError() const;
    
    bool saveHistory() const;
    bool loadHistory();
    
    std::vector<std::string> getTabCompletions(const std::string& partialCommand) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm