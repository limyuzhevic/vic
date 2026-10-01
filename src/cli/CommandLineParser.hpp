#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

class CommandLineParser {
public:
    struct Option {
        std::string longName;
        char shortName;
        std::string description;
        std::string defaultValue;
        bool required;
        bool hasValue;
        
        Option(const std::string& longN = "", char shortN = '\0', 
               const std::string& desc = "", const std::string& def = "",
               bool req = false, bool val = true)
            : longName(longN), shortName(shortN), description(desc), 
              defaultValue(def), required(req), hasValue(val) {}
    };
    
    CommandLineParser(const std::string& programName, const std::string& description) 
        : programName_(programName), description_(description) {}
    
    void addOption(const std::string& longName, char shortName, 
                   const std::string& description, const std::string& defaultValue = "",
                   bool required = false, bool hasValue = true);
    
    bool parse(int argc, char** argv);
    bool parse(const std::vector<std::string>& args);
    
    // Get value methods
    std::string getString(const std::string& option) const;
    int getInt(const std::string& option) const;
    double getDouble(const std::string& option) const;
    bool getBool(const std::string& option) const;
    bool hasOption(const std::string& option) const;
    
    // Validation methods
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Help methods
    void printHelp() const;
    void printUsage() const;
    void printVersion() const;
    
    // Configuration file methods
    bool loadConfigFile(const std::string& filepath);
    bool saveConfigFile(const std::string& filepath) const;
    
    // Configuration file parsing (simple key=value format)
    static bool parseConfigFile(const std::string& filepath, 
                               std::map<std::string, std::string>& config);
    
    // Get parsed values
    const std::map<std::string, std::string>& getValues() const { return values_; }
    
private:
    std::vector<Option> options_;
    std::map<std::string, std::string> values_;
    std::map<std::string, std::string> defaultValues_;
    std::string programName_;
    std::string description_;
    
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    std::string getOptionName(const Option& option) const;
}; 

// Convenience functions for adding common option types
inline void addOption(CommandLineParser& parser, const std::string& longName, char shortName,
                     const std::string& description, const std::string& defaultValue = "") {
    parser.addOption(longName, shortName, description, defaultValue, false);
}

inline void addOption(CommandLineParser& parser, const std::string& longName, char shortName,
                     const std::string& description, int defaultValue) {
    std::ostringstream oss;
    oss << defaultValue;
    parser.addOption(longName, shortName, description, oss.str(), false);
}

inline void addOption(CommandLineParser& parser, const std::string& longName, char shortName,
                     const std::string& description, double defaultValue) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << defaultValue;
    parser.addOption(longName, shortName, description, oss.str(), false);
}

inline void addOption(CommandLineParser& parser, const std::string& longName, char shortName,
                     const std::string& description, bool defaultValue) {
    std::string defaultStr = defaultValue ? "true" : "false";
    parser.addOption(longName, shortName, description, defaultStr, false);
}

inline void addFlag(CommandLineParser& parser, const std::string& longName, char shortName,
                   const std::string& description) {
    parser.addOption(longName, shortName, description, "false", false, false);
}

// Common NLM options
inline void addNLMCommonOptions(CommandLineParser& parser) {
    addOption(parser, "help", 'h', "Show help information and exit");
    addOption(parser, "version", 'v', "Show version information and exit");
    addOption(parser, "config", 'c', "Configuration file to load", "configs/default.cfg");
    addOption(parser, "output", 'o', "Output directory", "output");
    addOption(parser, "verbose", 'V', "Enable verbose logging", "false");
    addOption(parser, "quiet", 'q', "Suppress output messages", "false");
    addOption(parser, "steps", 's', "Number of simulation steps", "1000");
    addOption(parser, "timestep", 't', "Simulation time step size", "0.001");
    addOption(parser, "seed", 'S', "Random seed for reproducibility", "42");
    addFlag(parser, "save", 'w', "Save final brain state to file");
    addFlag(parser, "load", 'r', "Load brain state from file");
}

} // namespace nlm
