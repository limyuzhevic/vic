// NLM CLI Framework
// Command-line interface for advanced NLM operations

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

class CLICommand {
public:
    virtual ~CLICommand() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool execute(const std::vector<std::string>& args) = 0;
};

class CLI {
private:
    std::vector<std::unique_ptr<CLICommand>> commands;
    
public:
    void addCommand(std::unique_ptr<CLICommand> command);
    
    bool executeCommand(const std::string& commandName, 
                        const std::vector<std::string>& args);
    
    void printHelp() const;
    
    static CLI& getInstance();
    
private:
    CLI() = default;
    CLI(const CLI&) = delete;
    CLI& operator=(const CLI&) = delete;
};

// Command argument parsing utilities
class CLIArgumentParser {
public:
    struct Argument {
        std::string name;
        std::string value;
        bool hasValue;
    };
    
    static std::vector<Argument> parseArguments(const std::vector<std::string>& args);
    static bool hasFlag(const std::vector<Argument>& args, const std::string& flag);
    static std::string getValue(const std::vector<Argument>& args, 
                               const std::string& flag, 
                               const std::string& defaultValue = "");
    static int getIntValue(const std::vector<Argument>& args,
                          const std::string& flag,
                          int defaultValue = 0);
    static float getFloatValue(const std::vector<Argument>& args,
                              const std::string& flag,
                              float defaultValue = 0.0f);
    static double getDoubleValue(const std::vector<Argument>& args,
                                const std::string& flag,
                                double defaultValue = 0.0);
};

} // namespace nlm