#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <optional>
#include <variant>

namespace nlm {
namespace cli {

// Forward declarations
class Command;
class Subcommand;
class Result;

// CLI Result Types
class CLIError : public std::runtime_error {
public:
    enum class ErrorCode {
        Success = 0,
        InvalidArguments = 1,
        FileNotFound = 2,
        ConfigurationError = 3,
        ExecutionError = 4,
        ValidationError = 5,
        MemoryError = 6,
        PermissionError = 7,
        TimeoutError = 8
    };

    CLIError(ErrorCode code, const std::string& message, 
             const std::string& context = "", 
             const std::string& suggestion = "");
    
    ErrorCode getCode() const { return code_; }
    const std::string& getContext() const { return context_; }
    const std::string& getSuggestion() const { return suggestion_; }
    
private:
    ErrorCode code_;
    std::string context_;
    std::string suggestion_;
};

// Command execution result
class Result {
public:
    Result() : success_(true), exitCode_(0) {}
    Result(bool success, int exitCode = 0) 
        : success_(success), exitCode_(exitCode) {}
    Result(const CLIError& error) 
        : success_(false), exitCode_(static_cast<int>(error.getCode())),
          error_(error) {}
    
    bool isSuccess() const { return success_; }
    bool isFailure() const { return !success_; }
    int getExitCode() const { return exitCode_; }
    const std::string& getMessage() const { return message_; }
    const CLIError* getError() const { return error_ ? error_.get() : nullptr; }
    
    Result& setMessage(const std::string& msg) { message_ = msg; return *this; }
    Result& setExitCode(int code) { exitCode_ = code; return *this; }
    
    static Result success(const std::string& message = "") {
        return Result(true, 0).setMessage(message);
    }
    
    static Result failure(int exitCode, const std::string& message) {
        return Result(false, exitCode).setMessage(message);
    }
    
    static Result error(const CLIError& error) {
        return Result(error);
    }
    
private:
    bool success_;
    int exitCode_;
    std::string message_;
    std::unique_ptr<CLIError> error_;
};

// Base command class
class Command {
public:
    virtual ~Command() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool isHidden() const { return false; }
    
    virtual Result execute(const std::vector<std::string>& args) = 0;
    virtual Result executeWithConfig(const std::vector<std::string>& args, 
                                   const std::string& configFile) = 0;
};

// Subcommand base class
class Subcommand : public Command {
public:
    virtual std::vector<std::string> getSubcommandNames() const = 0;
    virtual std::unique_ptr<Command> createSubcommand(const std::string& name) = 0;
    
    virtual Result execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            return Result::failure(1, "Subcommand required");
        }
        
        auto subcommand = createSubcommand(args[0]);
        if (!subcommand) {
            return Result::failure(1, "Unknown subcommand: " + args[0]);
        }
        
        return subcommand->execute(std::vector<std::string>(args.begin() + 1, args.end()));
    }
    
    virtual Result executeWithConfig(const std::vector<std::string>& args, 
                                   const std::string& configFile) override {
        if (args.empty()) {
            return Result::failure(1, "Subcommand required");
        }
        
        auto subcommand = createSubcommand(args[0]);
        if (!subcommand) {
            return Result::failure(1, "Unknown subcommand: " + args[0]);
        }
        
        return subcommand->executeWithConfig(
            std::vector<std::string>(args.begin() + 1, args.end()), 
            configFile);
    }
};

// Argument parser
class ArgumentParser {
public:
    struct Option {
        std::string name;
        char shortName;
        std::string description;
        bool required;
        std::variant<std::string, int, bool> defaultValue;
        bool hasValue;
    };
    
    struct ParsedArgs {
        std::vector<std::string> positionalArgs;
        std::map<std::string, std::string> options;
        std::map<char, std::string> shortOptions;
        std::string configFile;
        bool help;
        bool version;
        bool verbose;
    };
    
    ArgumentParser(const std::string& programName);
    
    void addOption(const std::string& name, char shortName, 
                   const std::string& description, bool required = false,
                   const std::string& defaultValue = "", bool hasValue = true);
    
    void addSubcommand(const std::string& name, const std::string& description);
    
    ParsedArgs parse(const std::vector<std::string>& args);
    
    std::string getHelp() const;
    std::string getVersion() const;
    
private:
    std::string programName_;
    std::vector<Option> options_;
    std::vector<std::pair<std::string, std::string>> subcommands_;
    
    void printHelp(std::ostream& out) const;
    bool parseOption(const std::string& arg, ParsedArgs& parsed, 
                    size_t& i, const std::vector<std::string>& args);
    std::string trim(const std::string& str) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
};

// Main CLI class
class CLI {
public:
    CLI();
    ~CLI() = default;
    
    void registerCommand(std::unique_ptr<Command> command);
    void registerSubcommand(std::unique_ptr<Subcommand> subcommand);
    
    Result execute(const std::vector<std::string>& args);
    
    std::string getHelp() const;
    std::string getVersion() const;
    
private:
    std::string programName_;
    std::map<std::string, std::unique_ptr<Command>> commands_;
    std::map<std::string, std::unique_ptr<Subcommand>> subcommands_;
    ArgumentParser parser_;
};

// Default CLI implementation
class DefaultCLI : public CLI {
public:
    DefaultCLI();
    ~DefaultCLI() = default;
};

// CLI utilities
namespace utils {
    std::string formatHelp(const std::string& command, const std::string& description,
                          const std::string& usage, const std::string& examples = "");
    
    std::string formatError(const CLIError& error);
    
    std::string loadFile(const std::string& path);
    bool saveFile(const std::string& path, const std::string& content);
    
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    
    std::string getCurrentDirectory();
    std::vector<std::string> listFiles(const std::string& directory, 
                                      const std::string& pattern = "*");
    
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    std::string trim(const std::string& str);
    
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    
    std::string join(const std::vector<std::string>& strings, 
                    const std::string& delimiter);
    
    std::string generateId();
    std::string generateTimestamp();
}

// Public API
inline CLI& getCLI() {
    static DefaultCLI cli;
    return cli;
}

inline Result executeCLI(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return getCLI().execute(args);
}

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_HPP