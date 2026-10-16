// NLM CLI Parser
// Modern argument parser with comprehensive feature support

#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>
#include <set>
#include <functional>
#include <memory>
#include <chrono>
#include <algorithm>

namespace nlm {

// Argument types
enum class ArgType {
    FLAG,
    VALUE,
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    ENUM,
    VECTOR,
    FILE_PATH,
    DIRECTORY_PATH,
    OUTPUT_FORMAT,
    LOG_LEVEL,
    RANGE
};

// Argument definition for help and validation
struct ArgumentDef {
    std::string name;
    std::string description;
    ArgType type;
    std::string defaultValue;
    std::string helpText;
    std::vector<std::string> enumValues;
    std::pair<double, double> range;
    bool required;
    bool hidden;
    std::function<bool(const std::string&)> validator;
};

// Command definition for subcommand support
struct CommandDef {
    std::string name;
    std::string description;
    std::vector<ArgumentDef> arguments;
    std::function<bool(const std::map<std::string, std::string>&)> handler;
    bool hidden;
};

// Main argument parser class
class ArgumentParser {
private:
    int argc;
    char** argv;
    
    // Parsed arguments
    std::map<std::string, std::string> args;
    std::map<std::string, std::vector<std::string>> vectorArgs;
    std::map<std::string, bool> flagArgs;
    
    // Configuration
    std::map<std::string, ArgumentDef> argumentDefs;
    std::map<std::string, CommandDef> commandDefs;
    
    // Parsed values with type conversion
    std::map<std::string, std::variant<std::string, int, float, bool, std::vector<std::string>>> parsedValues;
    
    // Parser state
    std::string currentCommand;
    bool helpRequested;
    bool versionRequested;
    bool validationMode;
    
    // Error handling
    std::vector<std::string> errors;
    
public:
    ArgumentParser(int argc, char** argv);
    ~ArgumentParser() = default;
    
    // Argument registration
    void addArgument(const std::string& name, const std::string& description, 
                    ArgType type = ArgType::STRING, const std::string& defaultValue = "",
                    bool required = false, const std::vector<std::string>& enumValues = {});
    
    void addFlag(const std::string& name, const std::string& description, bool defaultValue = false);
    void addOption(const std::string& name, const std::string& description, ArgType type,
                  const std::string& defaultValue = "", bool required = false);
    
    void addCommand(const std::string& name, const std::string& description,
                   const std::vector<ArgumentDef>& arguments, bool hidden = false);
    
    // Argument parsing
    bool parse();
    bool parse(const std::string& inputString); // For script mode
    
    // Argument access
    bool hasArg(const std::string& name) const;
    std::optional<std::string> getArg(const std::string& name) const;
    std::optional<int> getInt(const std::string& name) const;
    std::optional<float> getFloat(const std::string& name) const;
    std::optional<bool> getBool(const std::string& name) const;
    std::optional<std::vector<std::string>> getVector(const std::string& name) const;
    
    std::string getArg(const std::string& name, const std::string& defaultValue) const;
    int getInt(const std::string& name, int defaultValue) const;
    float getFloat(const std::string& name, float defaultValue) const;
    bool getBool(const std::string& name, bool defaultValue) const;
    
    // Command handling
    std::optional<std::string> getCommand() const { return currentCommand.empty() ? std::nullopt : std::make_optional(currentCommand); }
    std::map<std::string, std::string> getCommandArgs() const;
    
    // Validation and error handling
    bool validate() const;
    const std::vector<std::string>& getErrors() const { return errors; }
    
    // Help and documentation
    void printHelp() const;
    void printUsage(const std::string& command = "") const;
    void printArgumentHelp(const std::string& name) const;
    void printVersion() const;
    
    // Parser state
    bool isHelpRequested() const { return helpRequested; }
    bool isVersionRequested() const { return versionRequested; }
    bool isValidationMode() const { return validationMode; }
    bool hasErrors() const { return !errors.empty(); }
    
    // Convenience methods
    bool hasShortOption(char c) const;
    bool hasLongOption(const std::string& name) const;
    std::string getPositionalArgument(size_t index) const;
    
    // Script mode support
    bool loadScript(const std::string& scriptPath);
    bool executeScript(const std::string& scriptPath);
    
    // Configuration file support
    bool loadConfigFile(const std::string& configPath);
    bool saveConfigFile(const std::string& configPath) const;
    
    // Progress tracking
    void setProgressCallback(std::function<void(double)> callback);
    
    // All arguments
    const std::map<std::string, std::string>& getAllArgs() const { return args; }
    
private:
    void parseArgs(int argc, char** argv);
    void parseEqualsFormat(const std::string& arg);
    void parseSpaceFormat(const std::string& arg);
    void processCommandLineArgs();
    bool validateArgument(const ArgumentDef& def, const std::string& value) const;
    std::string trim(const std::string& str) const;
    std::string ltrim(const std::string& str) const;
    std::string rtrim(const std::string& str) const;
};

} // namespace nlm

#endif // CLI_PARSER_HPP
