#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace nlm {

class CLIParser {
public:
    struct Option {
        std::string name;
        std::string shortOpt;
        std::string description;
        std::string defaultValue;
        bool required;
        std::string type; // "string", "int", "float", "bool"
        
        Option(const std::string& n, const std::string& s, const std::string& d, 
               const std::string& def = "", bool req = false, const std::string& t = "string")
            : name(n), shortOpt(s), description(d), defaultValue(def), required(req), type(t) {}
    };
    
    CLIParser(const std::string& programName, const std::string& description)
        : programName(programName), description(description) {}
    
    void addOption(const std::string& name, const std::string& shortOpt, 
                   const std::string& description, const std::string& defaultValue = "",
                   bool required = false, const std::string& type = "string") {
        options.push_back(Option(name, shortOpt, description, defaultValue, required, type));
    }
    
    void addFlag(const std::string& name, const std::string& description, bool defaultValue = false) {
        flags[name] = FlagInfo{description, defaultValue};
    }
    
    bool parse(int argc, char** argv);
    
    std::optional<std::string> getString(const std::string& name) const;
    std::optional<int> getInt(const std::string& name) const;
    std::optional<double> getDouble(const std::string& name) const;
    std::optional<bool> getBool(const std::string& name) const;
    bool getFlag(const std::string& name) const;
    
    void printHelp() const;
    void printVersion() const;
    
    // Enhanced output formatting
    static void printBanner(const std::string& title, const std::string& subtitle = "");
    static void printSection(const std::string& title);
    static void printStatus(const std::string& label, const std::string& value, bool success = true);
    static void printProgress(const std::string& message, int current, int total);
    
    // Configuration management
    bool saveConfig(const std::string& filename) const;
    bool loadConfig(const std::string& filename);
    std::string generateConfigTemplate() const;
    
private:
    std::string programName;
    std::string description;
    std::vector<Option> options;
    std::map<std::string, FlagInfo> flags;
    std::map<std::string, std::string> stringArgs;
    std::map<std::string, int> intArgs;
    std::map<std::string, double> doubleArgs;
    std::map<std::string, bool> boolArgs;
    
    struct FlagInfo {
        std::string description;
        bool defaultValue;
    };
    
    void printError(const std::string& message) const;
    std::string getUsage() const;
    std::string formatOption(const Option& opt) const;
};

} // namespace nlm