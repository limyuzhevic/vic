// CLI Parser for NLM (Neural Learning Machine)
// Enhanced command-line interface with comprehensive option support
//
// This file provides:
// - Option registration and help system
// - Argument parsing with getopt_long
// - Validation and error handling
// - Configuration integration
// - Help generation with category grouping

#ifndef NLM_CLI_PARSER_HPP
#define NLM_CLI_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <getopt.h>
#include <cstdint>

namespace nlm {

class CLIParser {
public:
    // Option structure for CLI argument registration
    struct Option {
        std::string longName;           // Long option name (e.g., "help")
        char shortName;                 // Short option name (e.g., 'h')
        std::string description;        // Human-readable description
        std::string defaultValue;       // Default value if not specified
        std::string category;           // Category for help organization (Basic, Advanced, Debug, etc.)
        std::string type;              // Option type (flag, file, num, float, choice, bool, count, kv, key)
        std::string example;           // Usage example
        bool required;                  // Whether option is required
        std::vector<std::string> validValues;  // Allowed values for choice types
        std::function<bool(const std::string&)> validator; // Custom validation function
    };
    
    // Parsed argument results
    struct ParsedArgs {
        std::map<std::string, std::string> values;      // Key-value options (--key=value)
        std::set<std::string> flags;                   // Flag options (--flag)
        std::map<std::string, int> counts;             // Count options (-vv)
        std::vector<std::string> positionalArgs;       // Positional arguments
        bool helpRequested = false;                    // --help or -h was specified
        bool versionRequested = false;                 // --version or -v was specified
        bool error = false;                            // Error occurred during parsing
        std::string errorMessage;                      // Error message if error occurred
    };
    
    CLIParser();
    ~CLIParser() = default();
    
    // Disable copying and moving
    CLIParser(const CLIParser&) = delete;
    CLIParser& operator=(const CLIParser&) = delete;
    CLIParser(CLIParser&&) = delete;
    CLIParser& operator=(CLIParser&&) = delete;
    
    // Initialize all CLI options
    void initializeOptions();
    
    // Register a new CLI option
    void addOption(const std::string& longName, char shortName, const std::string& description,
                   const std::string& defaultValue, const std::string& category,
                   const std::string& type, const std::string& example,
                   bool required = false, const std::vector<std::string>& validValues = {},
                   std::function<bool(const std::string&)> validator = nullptr);
    
    // Display help information with categorized options
    void printHelp(const std::string& programName) const;
    
    // Get version string
    std::string getVersion() const;
    
    // Parse command line arguments
    ParsedArgs parseArgs(int argc, char** argv) const;
    
    // Validate parsed arguments against option definitions
    bool validateParsedArgs(const ParsedArgs& parsed) const;
    
    // Display configuration summary
    void printConfig(const Config& config) const;
    
    // Validate configuration values
    bool validateConfig(const Config& config) const;
    
    // Get option by name
    std::vector<Option>::const_iterator findOptionByName(const std::string& name) const;
    
    // Get option by short character
    std::vector<Option>::const_iterator findOptionByChar(char c) const;
    
private:
    std::vector<Option> options;                     // Registered options
    
    // Helper methods for parsing and validation
    std::string getShortOptions() const;
    
    // Type validation helpers
    bool validateType(const std::string& type, const std::string& value) const;
    
    // Option processing helpers
    void processSetOption(std::map<std::string, std::string>& values,
                         const std::string& key, const std::string& value) const;
    
    void processUnsetOption(std::set<std::string>& flags,
                           const std::string& key) const;
}; // class CLIParser

// Global CLI parser instance
inline CLIParser cliParser;

} // namespace nlm

#endif // NLM_CLI_PARSER_HPP