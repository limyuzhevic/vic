// NLM Command Line Argument Parser
// Provides a clean abstraction for command-line argument handling
// Replaces embedded parsing in main.cpp with a reusable framework

#ifndef NLM_ARGUMENT_PARSER_HPP
#define NLM_ARGUMENT_PARSER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <functional>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cctype>

namespace nlm {

/**
 * @class ArgumentParser
 * @brief Abstract command-line argument parser with validation support
 * 
 * This class provides a clean abstraction for parsing command-line arguments,
 * supporting both short and long options, value requirements, and validation.
 * It replaces the embedded argument parsing in main.cpp with a reusable
 * framework that's easier to test and extend.
 */
class ArgumentParser {
public:
    /**
     * @struct Option
     * @brief Defines a command-line option
     */
    struct Option {
        std::string name;              // Option name (e.g., "--config")
        std::string shortName;        // Short option name (e.g., "-c")
        std::string description;      // Human-readable description
        bool requiresValue;            // Whether option needs a value
        std::optional<std::string> defaultValue;  // Default value if not provided
        std::function<bool(const std::string&)> validator; // Optional validator function
    };

    /**
     * @struct ParsedArguments
     * @brief Container for parsed command-line arguments
     */
    struct ParsedArguments {
        std::unordered_map<std::string, std::string> options;  // Name -> value mapping
        std::vector<std::string> positionalArgs;  // Non-option arguments
        bool helpRequested;  // Whether --help/-h was requested
        bool versionRequested;  // Whether --version/-v was requested
    };

    /**
     * @brief Constructs an ArgumentParser
     * @param programName The name of the program (for help output)
     * @param versionStr The version string (for help output)
     */
    explicit ArgumentParser(const std::string& programName = "nlm", 
                          const std::string& versionStr = "0.1.0");

    /**
     * @brief Adds a new command-line option
     * @param name Long option name (e.g., "--config")
     * @param shortName Short option name (e.g., "-c")
     * @param description Description for help output
     * @param requiresValue Whether this option requires a value
     * @param defaultValue Default value (if any)
     * @param validator Optional validation function
     * @return Reference to this parser for method chaining
     */
    ArgumentParser& addOption(const std::string& name, 
                           const std::string& shortName = "",
                           const std::string& description = "",
                           bool requiresValue = false,
                           const std::optional<std::string>& defaultValue = std::nullopt,
                           std::function<bool(const std::string&)> validator = nullptr);

    /**
     * @brief Adds help and version options
     * @return Reference to this parser for method chaining
     */
    ArgumentParser& addHelpOption(const std::string& helpFlag = "--help",
                                const std::string& shortHelpFlag = "-h");
    ArgumentParser& addVersionOption(const std::string& versionFlag = "--version",
                                   const std::string& shortVersionFlag = "-v");

    /**
     * @brief Parses command-line arguments
     * @param argc Number of arguments
     * @param argv Argument array
     * @return ParsedArguments containing the parsed results
     */
    ParsedArguments parse(int argc, char* argv[]);

    /**
     * @brief Prints help information to the given stream
     * @param out Output stream (default: std::cout)
     */
    void printHelp(std::ostream& out = std::cout) const;

    /**
     * @brief Validates parsed arguments against option definitions
     * @param args The parsed arguments to validate
     * @return true if all validations pass, false otherwise
     */
    bool validate(const ParsedArguments& args) const;

    /**
     * @brief Gets the value for an option
     * @param args Parsed arguments
     * advancement to existing code via wrapper
     */
    ParsedArguments parse(int argc, char** argv);

private:
    std::string programName_;
    std::string versionStr_;
    std::vector<Option> options_;
    bool helpAdded_ = false;
    bool versionAdded_ = false;
    
    // Helper methods for parsing
    void parseArguments(int argc, char* argv[], ParsedArguments& result) const;
    bool findOption(const std::string& arg, size_t& argIndex) const;
    void processPositionalArgs(const std::vector<std::string>& args, 
                              ParsedArguments& result) const;
    std::string extractOptionName(const std::string& arg) const;
    std::string extractOptionValue(const std::string& arg) const;
};

} // namespace nlm

#endif // NLM_ARGUMENT_PARSER_HPP