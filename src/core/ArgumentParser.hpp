// Enhanced Argument Parser
//
// This header file provides comprehensive command-line argument parsing capabilities
// for the NLM (Neural Learning Machine) project. It supports:
// - Traditional and JSON configuration files
// - Configuration presets
// - Advanced configuration overrides
// - Comprehensive error handling
// - Detailed help system with examples

#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <unordered_map>
#include "core/Config/Config.hpp"

namespace nlm {

class ArgumentParser {
public:
    enum class OptionType {
        FLAG,
        VALUE,
        KEY_VALUE,
        KEY_OBJECT,
        VALUES
    };
    
    struct Option {
        OptionType type;
        std::string longOpt;
        std::string shortOpt;
        std::string description;
        std::string defaultValue;
        std::string example;
        std::string category;
        bool advanced;
    };
    
    ArgumentParser();
    ~ArgumentParser() = default;
    
    // Add an option to the parser
    void addOption(const std::string& longOpt, const std::string& shortOpt, const Option& opt);
    void addOption(const std::string& longOpt, const std::string& shortOpt,
                  OptionType type, const std::string& description,
                  const std::string& defaultValue, const std::string& category,
                  bool advanced = false);
    
    // Parse command-line arguments
    bool parse(int argc, char** argv, Config& config);
    
    // Display help information
    void displayHelp() const;
    
    // Display version information
    void displayVersion() const;
    
    // Configuration management
    bool applyPreset(const Config& config);
    bool loadConfigurationFiles(Config& config);
    bool applyOverrides(Config& config);
    
    // Verbosity control
    void setVerbose(bool verbose);
    void setQuiet(bool quiet);
    bool isVerbose() const;
    bool isQuiet() const;
    
    // Getter methods
    const std::map<std::string, bool>& getParsedOptions() const { return parsedOptions_; }
    const std::map<std::string, std::string>& getParsedValues() const { return parsedValues_; }
    const std::map<std::string, std::map<std::string, std::string>>& getParsedKeyValues() const { return parsedKeyValues_; }
    const std::map<std::string, std::map<std::string, std::string>>& getParsedKeyObjects() const { return parsedKeyObjects_; }
    const std::map<std::string, std::vector<std::string>>& getParsedValuesList() const { return parsedValuesList_; }
    
private:
    // Helper methods
    bool parseArgument(const std::string& arg, int& i, int argc, char** argv, Config& config);
    void initializeBuiltInOptions();
    
    // Member variables
    std::map<std::string, Option> options_;                    // Available options
    std::map<std::string, bool> parsedOptions_;               // Parsed flag options
    std::map<std::string, std::string> parsedValues_;          // Parsed value options
    std::map<std::string, std::map<std::string, std::string>> parsedKeyValues_;  // Parsed key-value options
    std::map<std::string, std::map<std::string, std::string>> parsedKeyObjects_; // Parsed key-object options
    std::map<std::string, std::vector<std::string>> parsedValuesList_; // Parsed value list options
    std::map<std::string, Option> originalValues_;           // Original option values
    
    bool verbose_;
    bool quiet_;
    
    std::map<std::string, std::string> shortOptions_;          // Map short options to long options
};

} // namespace nlm
