#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <functional>
#include <memory>
#include <optional>
#include <chrono>
#include <filesystem>

namespace nlm {

// Forward declarations
class CLIParser;
class CLICommand;

// Advanced argument types
enum class ArgumentType {
    Positional,
    Flag,
    Option,
    Enum,
    Range,
    File,
    Vector,
    Path,
    JSON,
    YAML,
    Hyperparameter
};

enum class ArgumentSource {
    CommandLine,
    EnvironmentVariable,
    ConfigFile,
    Default,
    Runtime
};

struct ArgumentValidation {
    std::string errorMessage;
    std::function<bool(const ConfigValue&)> validator;
    std::vector<std::string> suggestions;
    bool required = false;
    std::string description;
    std::string defaultValue;
    std::string envVariable;
    std::string configKey;
    std::string profile;
};

struct ArgumentInfo {
    std::string name;
    std::string description;
    ArgumentType type;
    ConfigValue defaultValue;
    ArgumentSource source = ArgumentSource::Runtime;
    ArgumentValidation validation;
    std::vector<std::string> allowedValues;
    std::optional<std::pair<int, int>> range; // min, max
    bool required = false;
    bool hidden = false;
    std::string category;
    std::string example;
};

struct ParsedArgument {
    std::string key;
    ConfigValue value;
    ArgumentSource source;
    std::string profile;
    std::chrono::system_clock::time_point timestamp;
    bool validated = false;
};

class CLIArgument {
public:
    CLIArgument(const std::string& name, ArgumentType type)
        : name(name), type(type) {}

    virtual ~CLIArgument() = default;

    virtual bool parse(const std::string& value, ParsedArgument& parsed) = 0;
    virtual bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const = 0;
    virtual std::string getUsage() const = 0;
    virtual std::string getHelp() const = 0;

    const std::string& getName() const { return name; }
    ArgumentType getType() const { return type; }

protected:
    std::string name;
    ArgumentType type;
};

class CLIParser {
public:
    CLIParser();
    ~CLIParser() = default;

    // Argument definition
    template<typename T>
    CLIParser& addArgument(const std::string& name, const std::string& description, T defaultValue = T{})

    CLIParser& addPositional(const std::string& name, const std::string& description, bool required = true);
    CLIParser& addFlag(const std::string& name, const std::string& description = "");
    CLIParser& addOption(const std::string& name, const std::string& description, const std::string& defaultValue = "");
    CLIParser& addEnum(const std::string& name, const std::string& description,
                     const std::vector<std::string>& allowedValues, const std::string& defaultValue = "");
    CLIParser& addRange(const std::string& name, const std::string& description,
                      int min, int max, int defaultValue = 0);
    CLIParser& addFile(const std::string& name, const std::string& description, bool required = false);
    CLIParser& addPath(const std::string& name, const std::string& description, bool required = false, bool mustExist = true);

    // Validation
    CLIParser& addValidator(const std::string& name, std::function<bool(const ConfigValue&)> validator);
    CLIParser& addErrorMessage(const std::string& name, const std::string& errorMessage);
    CLIParser& addSuggestions(const std::string& name, const std::vector<std::string>& suggestions);

    // Environment variable support
    CLIParser& addEnvVariable(const std::string& argName, const std::string& envVarName);

    // Configuration file integration
    CLIParser& addConfigFile(const std::string& configKey, const std::string& format = "auto");

    // Profile management
    CLIParser& addProfile(const std::string& profileName);
    CLIParser& setProfile(const std::string& profileName);

    // Command structure
    CLIParser& addSubcommand(const std::string& name, const std::string& description);
    CLIParser& addNestedOption(const std::string& parent, const std::string& name,
                             const std::string& description, const std::string& defaultValue = "");

    // Parsing
    bool parse(int argc, char** argv);
    bool parse(const std::vector<std::string>& args);

    // Configuration integration
    bool applyToConfig(std::shared_ptr<Config> config) const;

    // Output formatting
    std::string getHelpText() const;
    std::string getCommandHelp(const std::string& command) const;
    std::string getArgumentHelp(const std::string& name) const;
    std::string getErrorReport() const;

    // Completion support
    std::vector<std::string> getCommandCompletions(const std::string& partial) const;
    std::vector<std::string> getOptionCompletions(const std::string& partial) const;

    // Utility
    bool hasArgument(const std::string& name) const;
    ConfigValue getArgumentValue(const std::string& name) const;
    std::string getArgumentSource(const std::string& name) const;

    // Expert features
    void enableAutoCompletion() { autoCompletionEnabled = true; }
    void disableAutoCompletion() { autoCompletionEnabled = false; }
    void setOutputFormat(const std::string& format) { outputFormat = format; }
    void setDefaultsFile(const std::string& defaultsFile) { defaultsFilePath = defaultsFile; }

    // Thread safety
    void enableThreadSafety() { threadSafe = true; }
    void disableThreadSafety() { threadSafe = false; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    bool autoCompletionEnabled = false;
    std::string outputFormat = "console";
    std::string defaultsFilePath;
    bool threadSafe = true;

    std::vector<std::string> parseCommandLineArgs(int argc, char** argv) const;
    bool parseArgument(const std::string& arg, std::vector<ParsedArgument>& parsedArgs) const;
    void loadDefaultsFile();
    void loadConfigFile(const std::string& filepath);
    void resolveEnvVariables(std::vector<ParsedArgument>& parsedArgs) const;
    bool validateArguments(const std::vector<ParsedArgument>& parsedArgs) const;
    std::string generateErrorMessage(const std::string& arg, const std::string& expected) const;
};

class CLICommand {
public:
    CLICommand(const std::string& name, const std::string& description)
        : name(name), description(description) {}

    virtual ~CLICommand() = default;

    virtual bool execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) = 0;
    virtual std::string getHelp() const = 0;

    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }

protected:
    std::string name;
    std::string description;
};

// Argument implementations
class StringArgument : public CLIArgument {
public:
    StringArgument(const std::string& name, const std::string& description, const std::string& defaultValue = "")
        : CLIArgument(name, ArgumentType::Option), defaultValue(defaultValue) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    std::string defaultValue;
};

class IntArgument : public CLIArgument {
public:
    IntArgument(const std::string& name, const std::string& description, int defaultValue = 0)
        : CLIArgument(name, ArgumentType::Option), defaultValue(defaultValue) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    int defaultValue;
};

class FloatArgument : public CLIArgument {
public:
    FloatArgument(const std::string& name, const std::string& description, double defaultValue = 0.0)
        : CLIArgument(name, ArgumentType::Option), defaultValue(defaultValue) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    double defaultValue;
};

class BoolArgument : public CLIArgument {
public:
    BoolArgument(const std::string& name, const std::string& description, bool defaultValue = false)
        : CLIArgument(name, ArgumentType::Flag), defaultValue(defaultValue) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    bool defaultValue;
};

class EnumArgument : public CLIArgument {
public:
    EnumArgument(const std::string& name, const std::string& description,
                const std::vector<std::string>& allowedValues, const std::string& defaultValue = "")
        : CLIArgument(name, ArgumentType::Enum), allowedValues(allowedValues), defaultValue(defaultValue) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    std::vector<std::string> allowedValues;
    std::string defaultValue;
};

class RangeArgument : public CLIArgument {
public:
    RangeArgument(const std::string& name, const std::string& description,
                 int min, int max, int defaultValue = 0)
        : CLIArgument(name, ArgumentType::Range), min(min), max(max), defaultValue(defaultValue) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    int min;
    int max;
    int defaultValue;
};

class FileArgument : public CLIArgument {
public:
    FileArgument(const std::string& name, const std::string& description, bool required = false)
        : CLIArgument(name, ArgumentType::File), required(required) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    bool required;
};

class PathArgument : public CLIArgument {
public:
    PathArgument(const std::string& name, const std::string& description, bool required = false, bool mustExist = true)
        : CLIArgument(name, ArgumentType::Path), required(required), mustExist(mustExist) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    bool required;
    bool mustExist;
};

class VectorArgument : public CLIArgument {
public:
    VectorArgument(const std::string& name, const std::string& description, const std::string& type = "string")
        : CLIArgument(name, ArgumentType::Vector), elementType(type) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    std::string elementType;
};

class HyperparameterArgument : public CLIArgument {
public:
    HyperparameterArgument(const std::string& name, const std::string& description)
        : CLIArgument(name, ArgumentType::Hyperparameter) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;
};

class ConfigArgument : public CLIArgument {
public:
    ConfigArgument(const std::string& name, const std::string& description, const std::string& format = "auto")
        : CLIArgument(name, ArgumentType::JSON), format(format) {}

    bool parse(const std::string& value, ParsedArgument& parsed) override;
    bool validate(const ParsedArgument& parsed, const ArgumentValidation& validation) const override;
    std::string getUsage() const override;
    std::string getHelp() const override;

private:
    std::string format;
};

// Command implementations
class RunCommand : public CLICommand {
public:
    RunCommand() : CLICommand("run", "Run neural network simulation") {}

    bool execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) override;
    std::string getHelp() const override;
};

class TrainCommand : public CLICommand {
public:
    TrainCommand() : CLICommand("train", "Train neural network model") {}

    bool execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) override;
    std::string getHelp() const override;
};

class SimulateCommand : public CLICommand {
public:
    SimulateCommand() : CLICommand("simulate", "Run simulation with current brain state") {}

    bool execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) override;
    std::string getHelp() const override;
};

class ExploreCommand : public CLICommand {
public:
    ExploreCommand() : CLICommand("explore", "Explore neural network space") {}

    bool execute(const std::vector<std::string>& args, std::shared_ptr<Config> config) override;
    std::string getHelp() const override;
};

} // namespace nlm
