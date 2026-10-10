// Option parsing system implementation
// Handles command-line argument parsing with support for various option types

#ifndef NLM_OPTIONPARSER_HPP
#define NLM_OPTIONPARSER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <sstream>

namespace nlm {
namespace cli {

// Forward declaration
class OptionParser;

// Result type for option parsing
enum class ParseResult {
    SUCCESS,
    PARSE_ERROR,
    VALIDATION_ERROR
};

// Custom exception types for CLI errors
class CLIException : public std::runtime_error {
public:
    explicit CLIException(const std::string& message) : std::runtime_error(message) {}
};

class CLIParseError : public CLIException {
public:
    explicit CLIParseError(const std::string& message) : CLIException("Parse error: " + message) {}
};

class CLIValidationError : public CLIException {
public:
    explicit CLIValidationError(const std::string& message) : CLIException("Validation error: " + message) {}
};

// Enhanced option type with more flexibility
struct AdvancedOption {
    std::string longName;           // --option-name
    char shortName;                 // -o
    std::string description;        // Human-readable description
    std::string defaultValue;       // Default value
    bool required;                  // Must be provided
    std::string value;              // Parsed value
    bool hasValue;                  // Does this option take a value?
    std::vector<std::string> allowedValues;  // If provided, restrict valid values
    std::function<bool(const std::string&)> validator;  // Custom validator function
    
    AdvancedOption(const std::string& longName, char shortName = '\0',
                   const std::string& desc = "",
                   const std::string& defaultVal = "",
                   bool req = false)
        : longName(longName), shortName(shortName), description(desc),
          defaultValue(defaultVal), required(req), hasValue(true), validator(nullptr) {}
    
    AdvancedOption(const std::string& longName, char shortName = '\0',
                   const std::string& desc = "",
                   bool req = false)
        : longName(longName), shortName(shortName), description(desc),
          defaultValue(""), required(req), hasValue(false), validator(nullptr) {}
    
    bool isSet() const { return !value.empty() || (!hasValue && required); }
    bool isValid() const {
        if (required && !isSet()) return false;
        if (hasValue && !allowedValues.empty() && !value.empty()) {
            return std::find(allowedValues.begin(), allowedValues.end(), value) != allowedValues.end();
        }
        if (validator && value.empty()) return true;  // Validator runs only when value is provided
        if (validator && value.empty()) return validator(value);
        return true;
    }
};

// Position argument specification
struct PositionalArg {
    std::string name;
    std::string description;
    bool required;
    size_t minCount;
    size_t maxCount;
    
    PositionalArg(const std::string& name, const std::string& desc = "",
                  bool req = true, size_t min = 1, size_t max = 1)
        : name(name), description(desc), required(req), minCount(min), maxCount(max) {}
};

// Enhanced option parser with sophisticated parsing capabilities
class OptionParser {
private:
    std::vector<AdvancedOption> options;
    std::unordered_map<std::string, std::string> parsedValues;
    std::vector<std::string> positionalArgs;
    std::vector<PositionalArg> positionalSpecs;
    
    // Parsing state
    ParseResult parseResult;
    std::vector<std::string> errors;
    
public:
    OptionParser() : parseResult(ParseResult::SUCCESS) {}
    
    // Option management
    void addOption(const AdvancedOption& option);
    void addFlag(const std::string& longName, char shortName = '\0', 
                 const std::string& description = "",
                 bool defaultValue = false);
    void addOptionWithValue(const std::string& longName, char shortName = '\0',
                            const std::string& description = "",
                            const std::string& defaultValue = "",
                            bool required = false);
    
    // Positional argument management
    void addPositionalArg(const PositionalArg& arg);
    
    // Parsing methods
    ParseResult parse(int argc, char** argv);
    ParseResult parse(const std::vector<std::string>& args);
    
    // Value access
    template<typename T>
    T getValue(const std::string& optionName, const T& defaultValue) const;
    
    bool hasOption(const std::string& optionName) const;
    std::string getString(const std::string& optionName) const;
    int getInt(const std::string& optionName) const;
    double getDouble(const std::string& optionName) const;
    bool getBool(const std::string& optionName) const;
    
    // Positional arguments
    std::vector<std::string> getPositionalArgs() const { return positionalArgs; }
    std::string getPositionalArg(size_t index) const;
    size_t getPositionalArgCount() const { return positionalArgs.size(); }
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const { return errors; }
    
    // Result query
    bool isSuccessful() const { return parseResult == ParseResult::SUCCESS; }
    
    // Error handling
    void addError(const std::string& error) {
        errors.push_back(error);
        parseResult = ParseResult::PARSE_ERROR;
    }
    
    // Help generation
    void printUsage(const std::string& programName) const;
    void printHelp(const std::string& programName) const;
    
private:
    // Parsing helpers
    bool parseArgument(const std::string& arg, size_t& currentPos, 
                       const std::vector<std::string>& args);
    bool parseOption(const std::string& arg, AdvancedOption& option, size_t& currentPos,
                     const std::vector<std::string>& args);
    bool parseValue(const std::string& arg, AdvancedOption& option, size_t& currentPos,
                    const std::vector<std::string>& args);
    
    // Validation helpers
    bool validateOption(const AdvancedOption& option) const;
    bool validatePositionalArgs() const;
    
    // String utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Configuration file parser
class ConfigFileParser {
private:
    std::string configFilePath;
    std::unordered_map<std::string, std::string> configValues;
    
public:
    ConfigFileParser(const std::string& filePath) : configFilePath(filePath) {}
    
    bool parse(const std::string& filePath = "");
    bool parseFromString(const std::string& content);
    
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    
    bool hasKey(const std::string& key) const {
        return configValues.find(key) != configValues.end();
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    const std::unordered_map<std::string, std::string>& getAllValues() const { return configValues; }
    
private:
    static std::string trim(const std::string& str);
    static std::string normalizeKey(const std::string& key);
    static bool parseLine(const std::string& line, std::string& key, std::string& value);
};

// Command line argument merger
class ArgumentMerger {
private:
    std::unique_ptr<ConfigFileParser> configParser;
    std::unique_ptr<OptionParser> optionParser;
    
public:
    ArgumentMerger() {
        configParser = std::make_unique<ConfigFileParser>("configs/default.cfg");
        optionParser = std::make_unique<OptionParser>();
    }
    
    bool merge(int argc, char** argv);
    
    template<typename T>
    T getFinalValue(const std::string& optionName, const T& defaultValue) const;
    
    bool hasOption(const std::string& optionName) const;
    std::string getString(const std::string& optionName) const;
    int getInt(const std::string& optionName) const;
    double getDouble(const std::string& optionName) const;
    bool getBool(const std::string& optionName) const;
    
    std::vector<std::string> getPositionalArgs() const { return optionParser->getPositionalArgs(); }
    
    bool isSuccessful() const { return configParser->isSuccessful() && optionParser->isSuccessful(); }
    std::string getErrors() const;
};

} // namespace cli
} // namespace nlm

#endif // NLM_OPTIONPARSER_HPP
