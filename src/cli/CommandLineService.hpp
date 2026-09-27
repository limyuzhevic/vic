// NLM Command-Line Completion and Help System
//
// Enhanced command-line interface with completion, help, and error handling
// Provides tab completion, context-sensitive help, and improved user experience
// Maintains backward compatibility with enhanced features

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <set>
#include <chrono>
#include <ctime>
#include <functional>

namespace nlm {

// Command-line options
struct CommandLineOption {
    std::string name;
    std::string shortName;
    std::string description;
    std::string type;
    std::string defaultValue;
    std::vector<std::string> choices;
    
    CommandLineOption(const std::string& n, const std::string& desc, const std::string& t = "string",
                     const std::string& def = "")
        : name(n), description(desc), type(t), defaultValue(def) {}
};

// Command-line command
struct Command {
    std::string name;
    std::string description;
    std::string usage;
    std::vector<CommandLineOption> options;
    std::vector<std::string> subcommands;
    
    Command(const std::string& n, const std::string& desc, const std::string& usage = "")
        : name(n), description(desc), usage(usage) {}
};

// Completion result
struct CompletionResult {
    std::string word;
    std::string replacement;
    std::string description;
    int priority;
    
    CompletionResult(const std::string& w, const std::string& r, const std::string& desc, int p = 0)
        : word(w), replacement(r), description(desc), priority(p) {}
    
    bool operator<(const CompletionResult& other) const {
        return priority > other.priority; // Higher priority first
    }
};

// Command-line completion system
class CommandLineCompleter {
public:
    CommandLineCompleter();
    ~CommandLineCompleter();
    
    // Completion functionality
    std::vector<CompletionResult> complete(const std::string& line, size_t cursorPos) const;
    std::string getCompletion(const std::string& line, size_t cursorPos) const;
    bool hasCompletion(const std::string& line, size_t cursorPos) const;
    std::vector<std::string> getSuggestions(const std::string& partialWord) const;
    
    // Word completion
    std::vector<std::string> completeWord(const std::string& partial) const;
    std::vector<std::string> completeCommand(const std::string& partial) const;
    std::vector<std::string> completeOption(const std::string& partial) const;
    
    // Completion registration
    void registerCommand(const Command& command);
    void unregisterCommand(const std::string& name);
    void registerOption(const CommandLineOption& option);
    void unregisterOption(const std::string& optionName);
    void registerCompletion(const std::string& prefix, const std::vector<std::string>& completions);
    
    // Statistics
    size_t getTotalCompletions() const;
    size_t getCompletedWords() const;
    size_t getCompletedCommands() const;
    size_t getCompletedOptions() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced help system
class HelpSystem {
public:
    HelpSystem();
    ~HelpSystem();
    
    // Help display
    void showHelp(const std::string& topic = "") const;
    void showHelp(const Command& command) const;
    void showHelp(const CommandLineOption& option, const std::string& command = "") const;
    
    // Context-sensitive help
    std::string getHelp(const std::string& topic) const;
    
    // Help search
    std::vector<std::string> searchHelp(const std::string& query) const;
    
    // Help configuration
    void setHelpFormat(const std::string& format);
    void setHelpWidth(size_t width);
};

// Interactive command-line interface
class InteractiveCLI {
public:
    InteractiveCLI();
    ~InteractiveCLI();
    
    // Interactive session
    void startSession();
    void stopSession();
    bool isSessionActive() const;
    
    // Command history
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory() const;
    
    // Interactive input
    std::string getInput(const std::string& prompt = "> ") const;
    bool getConfirmation(const std::string& prompt = "Confirm? (y/n): ") const;
    double getNumericInput(const std::string& prompt, double min = 0.0, double max = 1.0) const;
    
    // Configuration
    void setPrompt(const std::string& prompt);
    void setHistorySize(size_t size);
    
    // Statistics
    size_t getSessionDuration() const;
    size_t getCommandCount() const;
    size_t getCharacterCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Command-line argument parser
class AdvancedArgumentParser {
public:
    AdvancedArgumentParser();
    ~AdvancedArgumentParser();
    
    // Argument parsing
    std::map<std::string, std::string> parseArguments(int argc, char** argv);
    std::vector<std::string> parsePositionalArguments(int argc, char** argv);
    
    // Argument validation
    std::vector<std::string> validateArguments(const std::map<std::string, std::string>& args,
                                              const std::vector<Command>& commands) const;
    
    // Statistics
    size_t getTotalParsedArgs() const;
    size_t getFailedArgumentValidations() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Command-line interface service
class CommandLineService {
public:
    static CommandLineService& getInstance();
    static void initialize();
    static void shutdown();
    
    // Completion system
    static void setCompleter(std::unique_ptr<CommandLineCompleter> completer);
    static std::unique_ptr<CommandLineCompleter>& getCompleter();
    static std::vector<CompletionResult> complete(const std::string& line, size_t cursorPos);
    static std::string getCompletion(const std::string& line, size_t cursorPos);
    
    // Help system
    static void setHelpSystem(std::unique_ptr<HelpSystem> helpSystem);
    static std::unique_ptr<HelpSystem>& getHelpSystem();
    static void showHelp(const std::string& topic = "");
    static std::string getHelp(const std::string& topic);
    
    // Interactive interface
    static void setInteractiveCLI(std::unique_ptr<InteractiveCLI> cli);
    static std::unique_ptr<InteractiveCLI>& getInteractiveCLI();
    static void startSession();
    static void stopSession();
    static bool isSessionActive();
    
    // Argument parsing
    static void setArgumentParser(std::unique_ptr<AdvancedArgumentParser> parser);
    static std::unique_ptr<AdvancedArgumentParser>& getArgumentParser();
    static std::map<std::string, std::string> parseArguments(int argc, char** argv);
    
    // Registration
    static void registerCommand(const Command& command);
    static void unregisterCommand(const std::string& name);
    static void registerOption(const CommandLineOption& option);
    static void unregisterOption(const std::string& optionName);
    
    // Statistics
    static std::map<std::string, size_t> getSystemStatistics();
    static void resetStatistics();
    
private:
    CommandLineService();
    ~CommandLineService();
    CommandLineService(const CommandLineService&) = delete;
    CommandLineService& operator=(const CommandLineService&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions
namespace CLIUtils {
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static bool contains(const std::string& str, const std::string& substr);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    static bool isNumber(const std::string& str);
    static std::string getTimestamp();
    
    // String formatting
    static std::string wrapText(const std::string& text, size_t width);
    static std::string escapeMarkdown(const std::string& text);
    static std::string escapeHTML(const std::string& text);
    
    // Validation
    static bool isValidEmail(const std::string& email);
    static bool isValidURL(const std::string& url);
    static bool isValidIP(const std::string& ip);
    static bool isValidPath(const std::string& path);
    static bool isReadable(const std::string& path);
    static bool isWritable(const std::string& path);
    
    // Random
    static std::string generateRandomString(size_t length);
    static std::string generateRandomID(size_t length = 8);
    static int generateRandomInt(int min, int max);
    static double generateRandomDouble(double min, double max);
    static bool generateRandomBool();
};

// Enum for completion type
enum class CompletionType {
    Fuzzy,
    Exact,
    Prefix
};

} // namespace nlm