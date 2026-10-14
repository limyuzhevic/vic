#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <regex>
#include "Command.hpp"

namespace nlm {

// CommandToken: Represents a token in a command string
struct CommandToken {
    enum class Type { COMMAND, PARAMETER, OPERATOR, SEPARATOR, VARIABLE, STRING, NUMBER, IDENTIFIER };
    
    Type type;
    std::string value;
    size_t position;
    
    CommandToken(Type t, const std::string& v, size_t p) : type(t), value(v), position(p) {}
};

// CommandParseResult: Result of parsing a command string
struct CommandParseResult {
    bool success;
    std::vector<CommandToken> tokens;
    std::string commandName;
    std::vector<std::string> parameters;
    std::string errorMessage;
    std::string originalInput;
    
    CommandParseResult() : success(false) {}
    CommandParseResult(const std::string& input) : success(false), originalInput(input) {}
};

// CommandParser: Parses command strings into executable commands
class CommandParser {
public:
    CommandParser();
    ~CommandParser() = default;
    
    // Parse a command string
    CommandParseResult parse(const std::string& input);
    
    // Parse a command string with context
    CommandParseResult parse(const std::string& input, CommandContext& context);
    
    // Parse a macro
    CommandParseResult parseMacro(const std::string& macroName,
                                 const std::unordered_map<std::string, std::string>& parameters);
    
    // Parse a script (multiple commands)
    std::vector<CommandParseResult> parseScript(const std::string& script);
    
    // Validate command syntax
    bool validateSyntax(const std::vector<CommandToken>& tokens) const;
    
    // Get command help from registry
    std::string getCommandHelp(const std::string& commandName) const;
    
    // Get all available commands
    std::vector<std::string> getAvailableCommands() const;
    
    // Set command registry (for command lookup)
    void setCommandRegistry(CommandRegistry* registry) { registry_ = registry; }
    
    // Set brain context (for command execution)
    void setBrain(Brain* brain) { brain_ = brain; }
    
    // Set command invoker (for command execution)
    void setCommandInvoker(CommandInvoker* invoker) { invoker_ = invoker; }
    
    // Enable/disable variable substitution
    void setVariableSubstitutionEnabled(bool enabled) { variableSubstitutionEnabled_ = enabled; }
    
    // Enable/disable expression evaluation
    void setExpressionEvaluationEnabled(bool enabled) { expressionEvaluationEnabled_ = enabled; }
    
private:
    // Tokenize command input
    std::vector<CommandToken> tokenize(const std::string& input) const;
    
    // Token types for regex patterns
    static const std::regex commandPattern;
    static const std::regex parameterPattern;
    static const std::regex variablePattern;
    static const std::regex numberPattern;
    static const std::regex stringPattern;
    static const std::regex operatorPattern;
    static const std::regex separatorPattern;
    
    // Parse command tokens into components
    void parseTokens(const std::vector<CommandToken>& tokens,
                    CommandParseResult& result) const;
    
    // Validate command parameters
    bool validateParameters(const std::string& commandName,
                           const std::vector<std::string>& parameters) const;
    
    // Substitute variables in parameters
    std::vector<std::string> substituteVariables(const std::vector<std::string>& parameters,
                                                 CommandContext& context) const;
    
    // Evaluate expressions in parameters
    std::vector<std::string> evaluateExpressions(const std::vector<std::string>& parameters,
                                                CommandContext& context) const;
    
    // Build command from parsed result
    std::unique_ptr<Command> buildCommand(const CommandParseResult& result) const;
    
    CommandRegistry* registry_;
    Brain* brain_;
    CommandInvoker* invoker_;
    bool variableSubstitutionEnabled_;
    bool expressionEvaluationEnabled_;
};

} // namespace nlm