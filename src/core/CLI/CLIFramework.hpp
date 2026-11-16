#ifndef NLM_CLI_FRAMEWORK_HPP
#define NLM_CLI_FRAMEWORK_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>
#include <sstream>

namespace nlm {

// CLI Framework Types
using CLIOptionValue = std::variant<std::string, int, double, bool>;
using CLICommandHandler = std::function<int(int argc, char** argv)>;

class CLICommand;
class CLIOption;

class CLIApplication {
public:
    CLIApplication(const std::string& name, const std::string& description);
    virtual ~CLIApplication();
    
    // Register subcommand
    void addCommand(const std::string& name, const std::string& description,
                    CLICommandHandler handler);
    
    // Register global option
    void addOption(char shortOpt, const std::string& longOpt,
                   const std::string& description, CLIOptionValue defaultValue);
    
    // Parse arguments
    int run(int argc, char** argv);
    
    // Get help text
    std::string getHelp() const;
    
    // Version information
    void setVersion(const std::string& version) { version_ = version; }
    const std::string& getVersion() const { return version_; }
    
protected:
    // Command execution
    int executeCommand(const std::string& commandName, int argc, char** argv);
    
    // Option parsing
    bool parseOptions(int argc, char** argv, std::unordered_map<std::string, CLIOptionValue>& parsedOptions);
    
    // Utility functions
    template<typename T>
    static T getOptionValue(const std::unordered_map<std::string, CLIOptionValue>& options,
                           const std::string& key, const T& defaultValue);
    
    std::string name_;
    std::string description_;
    std::string version_;
    std::unordered_map<std::string, CLICommandHandler> commands_;
    std::unordered_map<char, CLIOptionValue> shortOptions_;
    std::unordered_map<std::string, CLIOptionValue> longOptions_;
    std::unordered_map<std::string, std::string> optionDescriptions_;
};

class CLICommand {
public:
    CLICommand(const std::string& name, const std::string& description,
               CLICommandHandler handler);
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    int execute(int argc, char** argv) const { return handler_(argc, argv); }
    
private:
    std::string name_;
    std::string description_;
    CLICommandHandler handler_;
};

class CLIOption {
public:
    CLIOption(char shortOpt, const std::string& longOpt,
              const std::string& description, CLIOptionValue defaultValue);
    
    char getShortOpt() const { return shortOpt_; }
    const std::string& getLongOpt() const { return longOpt_; }
    const std::string& getDescription() const { return description_; }
    CLIOptionValue getDefaultValue() const { return defaultValue_; }
    
private:
    char shortOpt_;
    std::string longOpt_;
    std::string description_;
    CLIOptionValue defaultValue_;
};

// Template implementation

template<typename T>
T CLIApplication::getOptionValue(const std::unordered_map<std::string, CLIOptionValue>& options,
                                const std::string& key, const T& defaultValue) {
    auto it = options.find(key);
    if (it == options.end()) {
        return defaultValue;
    }
    
    const CLIOptionValue& value = it->second;
    try {
        return std::get<T>(value);
    } catch (const std::bad_variant_access&) {
        // Type conversion could be enhanced here
        return defaultValue;
    }
}

// Helper functions for common option types
inline std::string getStringOption(const std::unordered_map<std::string, CLIOptionValue>& options,
                                   const std::string& key, const std::string& defaultValue = "") {
    return getOptionValue<std::string>(options, key, defaultValue);
}

inline int getIntOption(const std::unordered_map<std::string, CLIOptionValue>& options,
                        const std::string& key, int defaultValue = 0) {
    return getOptionValue<int>(options, key, defaultValue);
}

inline double getDoubleOption(const std::unordered_map<std::string, CLIOptionValue>& options,
                              const std::string& key, double defaultValue = 0.0) {
    return getOptionValue<double>(options, key, defaultValue);
}

inline bool getBoolOption(const std::unordered_map<std::string, CLIOptionValue>& options,
                          const std::string& key, bool defaultValue = false) {
    return getOptionValue<bool>(options, key, defaultValue);
}

// Standard error handling
inline void printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

inline void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " <command> [options]" << std::endl;
    std::cout << "Use \"" << programName << " help\" for command list." << std::endl;
}

} // namespace nlm

#endif // NLM_CLI_FRAMEWORK_HPP
