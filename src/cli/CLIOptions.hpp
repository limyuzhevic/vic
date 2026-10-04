#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>
#include <regex>

namespace nlm {

// Option parsing result
struct ParsedOption {
    std::string name;
    std::optional<std::string> value;
    bool found;
    
    ParsedOption(const std::string& name, const std::optional<std::string>& value = std::nullopt)
        : name(name), value(value), found(true) {}
};

// Option parser with validation
class CLIOptions {
public:
    CLIOptions();
    ~CLIOptions();
    
    // Add option to parser
    void addOption(const CLIOption& option);
    
    // Parse command line arguments
    std::vector<ParsedOption> parse(int argc, char** argv) const;
    
    // Parse from string list
    std::vector<ParsedOption> parse(const std::vector<std::string>& args) const;
    
    // Get option by name
    ParsedOption getOption(const std::string& name) const;
    
    // Check if option exists
    bool hasOption(const std::string& name) const;
    
    // Get all options
    const std::vector<CLIOption>& getOptions() const { return options; }
    
    // Validate parsed options
    bool validate(const std::vector<ParsedOption>& parsedOptions) const;
    
    // Get error messages
    std::vector<std::string> getErrors() const { return errors; }
    
    // Clear errors
    void clearErrors() { errors.clear(); }
    
    // Option help generation
    std::string generateHelp() const;
    std::string generateOptionHelp(const CLIOption& option) const;
    
    // Validation utilities
    static bool validateValue(const std::string& value, const CLIOption& option);
    static std::string getValidationError(const std::string& value, const CLIOption& option);
    
    // Type conversion utilities
    static std::string convertToString(const std::any& value);
    static bool convertToBool(const std::string& value);
    static int convertToInt(const std::string& value);
    static double convertToDouble(const std::string& value);
    static std::string convertToString(const std::string& value);
    
    // File and directory utilities
    static bool validateFileExists(const std::string& path);
    static bool validateDirectoryExists(const std::string& path);
    static std::string expandPath(const std::string& path);
    static std::string getRelativePath(const std::string& path, const std::string& base = ".");
    
    // Progress and output utilities
    static void printProgress(const std::string& message, double progress = -1.0);
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    static void printDebug(const std::string& message);
    
    // String utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string capitalize(const std::string& str);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter = " ");
    static std::vector<std::string> split(const std::string& str, char delimiter = ' ');
    
    // Date and time utilities
    static std::string getCurrentTimestamp();
    static std::string formatDuration(double seconds);
    static std::string formatFileSize(size_t bytes);
    static std::string formatPercentage(double value);
    
    // Configuration utilities
    static std::string getConfigValue(const std::string& key, const std::map<std::string, std::string>& config = {});
    static bool setConfigValue(std::map<std::string, std::string>& config, const std::string& key, const std::string& value);
    
    // Environment utilities
    static std::string getEnvironmentVariable(const std::string& name, const std::string& defaultValue = "");
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
    static std::string getHomeDirectory();
    static std::string getCurrentWorkingDirectory();
    
private:
    std::vector<CLIOption> options;
    std::vector<std::string> errors;
    
    // Internal parsing methods
    bool matchOption(const std::string& arg, const CLIOption& option) const;
    std::optional<std::string> extractOptionValue(const std::string& arg) const;
    
    // Validation methods
    bool validateRequiredOption(const CLIOption& option, const std::vector<ParsedOption>& parsedOptions) const;
    bool validateOptionDependencies(const CLIOption& option, const std::vector<ParsedOption>& parsedOptions) const;
    bool validateOptionConflicts(const CLIOption& option, const std::vector<ParsedOption>& parsedOptions) const;
    
    // Error handling
    void addError(const std::string& error) { errors.push_back(error); }
    
    // Option comparison
    bool optionsEqual(const CLIOption& option1, const CLIOption& option2) const;
    size_t getOptionHash(const CLIOption& option) const;
};

// Option builder for fluent API
class OptionBuilder {
public:
    OptionBuilder(const std::string& name, const std::string& description);
    
    OptionBuilder& shortName(const std::string& shortName);
    OptionBuilder& required();
    OptionBuilder& optional();
    OptionBuilder& flag();
    OptionBuilder& value(const std::string& defaultValue = "");
    OptionBuilder& range(double min, double max);
    OptionBuilder& choices(const std::vector<std::string>& choices);
    OptionBuilder& file();
    OptionBuilder& directory();
    OptionBuilder& sensitive();
    OptionBuilder& validate(const std::string& pattern);
    OptionBuilder& type(const std::string& type);
    
    CLIOption build() const;
    
private:
    CLIOption option;
};

// Parameter parser for command-line arguments
class CLIParameterParser {
public:
    CLIParameterParser();
    ~CLIParameterParser();
    
    // Parse parameters from arguments
    std::vector<std::any> parse(const std::vector<std::string>& args,
                               const std::vector<CLIParameter>& parameters) const;
    
    // Validate parameters
    bool validate(const std::vector<std::string>& args,
                  const std::vector<CLIParameter>& parameters) const;
    
    // Get parameter count
    static size_t getRequiredParameterCount(const std::vector<CLIParameter>& parameters);
    
    // Get parameter help
    static std::string generateParameterHelp(const CLIParameter& parameter);
    
private:
    std::vector<std::string> errors;
    
    // Internal parsing methods
    bool matchParameter(const std::string& arg, const CLIParameter& parameter) const;
    std::any extractParameterValue(const std::string& arg, const CLIParameter& parameter) const;
    
    // Type conversion
    std::any convertValue(const std::string& value, const std::string& type) const;
    
    // Validation
    bool validateParameter(const std::string& value, const CLIParameter& parameter) const;
    
    // Error handling
    void addError(const std::string& error) { errors.push_back(error); }
};

} // namespace nlm
