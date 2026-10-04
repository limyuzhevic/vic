#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {

// CLI command base class
class CLICommand {
public:
    virtual ~CLICommand() = default;
    
    // Command metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual std::string getHelp() const = 0;
    
    // Command execution
    virtual int execute(const std::vector<std::string>& args) = 0;
    virtual bool validate(const std::vector<std::string>& args) const = 0;
    
    // Command options and parameters
    virtual const std::vector<CLIOption>& getOptions() const = 0;
    virtual const std::vector<CLIParameter>& getParameters() const = 0;
    
    // Command execution context
    virtual void setContext(const std::map<std::string, std::any>& context) = 0;
    virtual const std::map<std::string, std::any>& getContext() const = 0;
};

// Base class for simple commands that don't require complex logic
class SimpleCLICommand : public CLICommand {
protected:
    std::string name;
    std::string description;
    std::string usage;
    std::string help;
    std::vector<CLIOption> options;
    std::vector<CLIParameter> parameters;
    std::map<std::string, std::any> context;
    
public:
    SimpleCLICommand(const std::string& name, const std::string& description,
                     const std::string& usage, const std::string& help = "")
        : name(name), description(description), usage(usage), help(help) {}
    
    std::string getName() const override { return name; }
    std::string getDescription() const override { return description; }
    std::string getUsage() const override { return usage; }
    std::string getHelp() const override { return help; }
    
    virtual int execute(const std::vector<std::string>& args) override = 0;
    virtual bool validate(const std::vector<std::string>& args) const override = 0;
    
    const std::vector<CLIOption>& getOptions() const override { return options; }
    const std::vector<CLIParameter>& getParameters() const override { return parameters; }
    
    void setContext(const std::map<std::string, std::any>& ctx) override { context = ctx; }
    const std::map<std::string, std::any>& getContext() const override { return context; }
    
    // Helper methods for parameter and option parsing
protected:
    std::any getOptionValue(const std::vector<std::string>& args, const std::string& option) const;
    bool hasOption(const std::vector<std::string>& args, const std::string& option) const;
    std::any getRequiredParameter(const std::vector<std::string>& args, size_t index) const;
    std::any getOptionalParameter(const std::vector<std::string>& args, size_t index) const;
    
    // Type conversion utilities
    template<typename T>
    T convertValue(const std::string& value) const;
    
    // Validation utilities
    bool validateRange(const std::string& value, double min, double max) const;
    bool validateChoice(const std::string& value, const std::vector<std::string>& choices) const;
    bool validateFileExists(const std::string& path) const;
};

// CLI option definitions
struct CLIOption {
    std::string name;
    std::string shortName;
    std::string description;
    OptionType type;
    bool required;
    std::string defaultValue;
    std::string validationPattern;
    std::vector<std::string> choices;
    std::string valueDescription;
    bool sensitive;  // For passwords
    
    CLIOption(const std::string& name, const std::string& shortName = "",
              const std::string& description = "", OptionType type = OptionType::Value,
              bool required = false, const std::string& defaultValue = "")
        : name(name), shortName(shortName), description(description), type(type),
          required(required), defaultValue(defaultValue), sensitive(false) {}
};

// CLI parameter definitions
struct CLIParameter {
    std::string name;
    std::string description;
    bool required;
    std::string type;
    std::string defaultValue;
    std::string validationPattern;
    
    CLIParameter(const std::string& name, const std::string& description,
                 bool required = false, const std::string& type = "string",
                 const std::string& defaultValue = "")
        : name(name), description(description), required(required),
          type(type), defaultValue(defaultValue) {}
};

// Common CLI types
enum class OptionType {
    Flag,
    Value,
    RequiredValue,
    OptionalValue,
    Range,
    Choice,
    File,
    Directory,
    Number,
    Float,
    Integer,
    Boolean,
    String,
    Enum,
    Custom
};

// Common CLI formatters
class CLIFormatters {
public:
    static std::string formatValue(const std::any& value);
    static std::string formatDuration(double seconds);
    static std::string formatFileSize(size_t bytes);
    static std::string formatPercentage(double value);
    static std::string formatMemory(size_t bytes);
    static std::string formatNumber(int64_t number);
    static std::string formatList(const std::vector<std::string>& items);
    
    // Color output for terminals
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    static void printDebug(const std::string& message);
    
    // Progress indicators
    static void printProgress(const std::string& message, double progress = -1.0);
    static void printSpinner(const std::string& message, std::chrono::milliseconds delay = std::chrono::milliseconds(100));
};

} // namespace nlm
