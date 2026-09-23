#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace nlm {

/**
 * Command line parser for NLM with enhanced features
 */
class CommandLineParser {
public:
    struct Option {
        std::string flags;  // Single dash or multiple -- flags
        std::string value;  // Optional value
        std::string description;  // Help text
        std::string defaultValue;  // Default if not provided
        bool required = false;  // Whether option is required
        bool hasValue = true;  // Whether option takes a value
    };

    CommandLineParser(int argc, char* argv[]);
    
    // Parse command line arguments
    bool parse();
    
    // Check if option was provided
    bool hasOption(const std::string& flag) const;
    
    // Get option value
    std::string getOption(const std::string& flag) const;
    int getOptionAsInt(const std::string& flag) const;
    float getOptionAsFloat(const std::string& flag) const;
    bool getOptionAsBool(const std::string& flag) const;
    
    // Get positional arguments
    std::vector<std::string> getArgs() const;
    
    // Help and version
    void printHelp() const;
    void printVersion() const;
    
    // Error handling
    std::string getError() const { return errorMessage; }
    
    // Option management
    void addOption(const std::string& flags, const std::string& description,
                  const std::string& defaultValue = "", bool required = false);
    
    // Built-in option definitions
    static void defineBuiltInOptions();
    
private:
    int argc;
    char** argv;
    std::vector<std::string> positionalArgs;
    std::unordered_map<std::string, std::string> optionValues;
    std::vector<Option> options;
    std::string errorMessage;
    
    // Short and long option parsing
    bool parseShortOption(const std::string& arg);
    bool parseLongOption(const std::string& arg);
    
    // Option validation
    bool validateOptions() const;
    
    // Help/usage generation
    void generateHelp() const;
};

} // namespace nlm

#endif // COMMAND_LINE_PARSER_HPP
