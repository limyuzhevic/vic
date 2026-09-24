#ifndef CLI_HPP
#define CLI_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

// CLI argument parsing exceptions
class CLIError : public std::runtime_error {
public:
    CLIError(const std::string& message) : std::runtime_error(message) {}
};

class InvalidArgumentError : public CLIError {
public:
    InvalidArgumentError(const std::string& arg, const std::string& expected)
        : CLIError("Invalid argument '" + arg + "'. Expected: " + expected) {}
};

class MissingArgumentError : public CLIError {
public:
    MissingArgumentError(const std::string& arg)
        : CLIError("Missing value for argument '" + arg + "'. Use '--arg value' or '--arg=value'") {}
};

class CLIValidationError : public CLIError {
public:
    CLIValidationError(const std::string& message)
        : CLIError("Validation error: " + message) {}
};

// CLI argument definition structure
struct CLIArgument {
    std::string name;
    char shortOpt;
    std::string longOpt;
    std::string description;
    std::string type;
    std::string defaultValue;
    bool required;
    std::vector<std::string> allowedValues;
    std::function<bool(const std::string&)> validator;
    double minValue;
    double maxValue;
    bool hasMinMax;
    
    CLIArgument(const std::string& name, char shortOpt = 0, 
                const std::string& longOpt = "", 
                const std::string& description = "",
                const std::string& type = "string",
                const std::string& defaultValue = "",
                bool required = false)
        : name(name), shortOpt(shortOpt), longOpt(longOpt), 
          description(description), type(type), defaultValue(defaultValue),
          required(required), allowedValues(), validator(nullptr),
          minValue(0.0), maxValue(0.0), hasMinMax(false) {}
    
    CLIArgument(const std::string& name, char shortOpt,
                const std::string& longOpt,
                const std::string& description,
                const std::string& type,
                const std::string& defaultValue,
                bool required,
                const std::vector<std::string>& allowedValues)
        : name(name), shortOpt(shortOpt), longOpt(longOpt), 
          description(description), type(type), defaultValue(defaultValue),
          required(required), allowedValues(allowedValues), validator(nullptr),
          minValue(0.0), maxValue(0.0), hasMinMax(false) {}
    
    CLIArgument(const std::string& name, char shortOpt,
                const std::string& longOpt,
                const std::string& description,
                const std::string& type,
                const std::string& defaultValue,
                bool required,
                const std::vector<std::string>& allowedValues,
                double minValue, double maxValue, bool hasMinMax)
        : name(name), shortOpt(shortOpt), longOpt(longOpt), 
          description(description), type(type), defaultValue(defaultValue),
          required(required), allowedValues(allowedValues), validator(nullptr),
          minValue(minValue), maxValue(maxValue), hasMinMax(hasMinMax) {}
};

struct CLI {
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string logLevel = "info";
        std::string logFile;
        std::string outputFormat = "text";
        int debugLevel = 0;
        bool verbose = false;
        bool help = false;
        bool version = false;
        bool quiet = false;
        std::vector<std::string> customArgs;
        std::string customValue;
    };

    // Enhanced argument parsing with validation
    static Options parseArgs(int argc, char** argv);
    
    // Validate and process argument
    static void validateArgument(const std::string& arg, const std::string& value,
                                std::unordered_map<std::string, std::string>& parsedArgs);
    
    // Get argument help text
    static void printHelp();
    
    // Get error message suggestions
    static std::string getErrorSuggestion(const std::string& arg);
    
    // Check if argument is known
    static bool isValidArgument(const std::string& arg);
    
    // Get argument definition
    static CLIArgument getArgumentDefinition(const std::string& arg);
};

} // namespace nlm

#endif // CLI_HPP
