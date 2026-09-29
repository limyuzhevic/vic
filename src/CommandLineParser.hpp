#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <regex>
#include <algorithm>
#include <sstream>

namespace nlm {

// Command line argument parsing and validation
class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser();
    
    // Disable copying
    CommandLineParser(const CommandLineParser&) = delete;
    CommandLineParser& operator=(const CommandLineParser&) = delete;
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed arguments
    const std::map<std::string, std::string>& getArguments() const;
    const std::vector<std::string>& getPositionalArguments() const;
    
    // Check if argument exists
    bool hasArgument(const std::string& name) const;
    
    // Get argument value
    std::string getArgument(const std::string& name) const;
    int getArgumentInt(const std::string& name) const;
    double getArgumentDouble(const std::string& name) const;
    bool getArgumentBool(const std::string& name) const;
    
    // Get positional argument
    std::string getPositionalArgument(size_t index) const;
    
    // Argument validation
    bool validateArguments() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Command help
    std::string getUsage() const;
    std::string getHelp() const;
    
    // Argument configuration
    void addArgument(const std::string& name, const std::string& defaultValue = "",
                     const std::string& help = "",
                     const std::string& type = "string");
    void addRequiredArgument(const std::string& name, const std::string& help = "");
    void addOptionalArgument(const std::string& name, const std::string& defaultValue = "",
                           const std::string& help = "");
    void addFlag(const std::string& name, const std::string& help = "");
    
    // Argument type validation
    struct ArgumentConfig {
        std::string name;
        std::string type;  // string, int, double, bool, float
        std::string defaultValue;
        std::string help;
        bool required;
        std::string validationPattern;
        std::string validationMessage;
        
        ArgumentConfig() : required(false), validationPattern() {}
        ArgumentConfig(const std::string& n, const std::string& t, const std::string& def,
                      const std::string& h, bool req = false)
            : name(n), type(t), defaultValue(def), help(h), required(req) {}
    };
    
    // Argument parsing patterns
    struct ArgumentPatterns {
        static const std::string LONG_OPTION;      // --name
        static const std::string SHORT_OPTION;     // -n
        static const std::string EQUALS;           // =
        static const std::string SPACE;            // space
        static const std::string NO_VALUE;         // flag
    };
    
    // Error types
    enum class ErrorType {
        INVALID_FORMAT,
        MISSING_REQUIRED_ARGUMENT,
        INVALID_VALUE,
        DUPLICATE_ARGUMENT,
        UNKNOWN_ARGUMENT,
        VALIDATION_FAILED
    };
    
    struct Error {
        ErrorType type;
        std::string message;
        std::string argumentName;
        
        Error(ErrorType t, const std::string& msg, const std::string& arg = "")
            : type(t), message(msg), argumentName(arg) {}
    };
    
    // Statistics
    struct Statistics {
        size_t totalArguments;
        size_t parsedArguments;
        size_t validationErrors;
        size_t unknownArguments;
        std::map<std::string, size_t> argumentCounts;
        
        Statistics() : totalArguments(0), parsedArguments(0), validationErrors(0),
                      unknownArguments(0) {}
    };
    
    // Get statistics
    Statistics getStatistics() const;
    
    // Reset parser
    void reset();
    
    // Argument value conversion
    static std::string trim(const std::string& str);
    static bool stringToBool(const std::string& str);
    static int stringToInt(const std::string& str);
    static double stringToDouble(const std::string& str);
    static bool validatePattern(const std::string& value, const std::string& pattern);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Argument parsing helpers
    bool parseArgument(const std::string& arg);
    bool parseLongOption(const std::string& arg);
    bool parseShortOption(const std::string& arg);
    bool parseEqualFormat(const std::string& arg);
    bool parseSpaceFormat(const std::string& arg, const std::string& nextArg);
    
    // Argument validation
    bool validateArgument(const std::string& name, const std::string& value) const;
    std::vector<std::string> validateAllArguments() const;
    
    // Help generation
    std::string generateUsage() const;
    std::string generateHelp() const;
    std::string generateArgumentHelp(const ArgumentConfig& config) const;
    
    // Statistics helpers
    void updateStatistics();
    
    // Argument parsing state
    enum class ParseState {
        NORMAL,
        EXPECTING_VALUE,
        EXPECTING_LONG_VALUE,
        EXPECTING_SHORT_VALUE
    };
    
    ParseState parseState;
    std::string currentArgumentName;
    
    // Configuration loading
    void loadDefaultConfiguration();
    void loadHelpConfiguration();
    
    // Error handling
    void addError(ErrorType type, const std::string& message,
                  const std::string& argumentName = "");
    
    // Argument type checking
    static bool isBooleanType(const std::string& type);
    static bool isNumericType(const std::string& type);
    static bool isStringType(const std::string& type);
    
    // Argument pattern matching
    static bool matchesLongOptionPattern(const std::string& arg);
    static bool matchesShortOptionPattern(const std::string& arg);
    static bool matchesEqualPattern(const std::string& arg);
    
    // Help text constants
    static const std::string DEFAULT_USAGE_LINE;
    static const std::string DEFAULT_HELP_HEADER;
    static const std::string DEFAULT_HELP_FOOTER;
    static const std::string ARGUMENT_SEPARATOR;
    static const std::string EQUALS_SEPARATOR;
}; // namespace nlm

// Global command line parser instance
CommandLineParser& getCommandLineParser();