#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace nlm {

class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser() = default;
    
    // Add options
    void addOption(const std::string& shortOpt, const std::string& longOpt, 
                   const std::string& description, const std::string& defaultValue = "");
    void addOption(const std::string& longOpt, const std::string& description,
                   const std::string& defaultValue = "");
    
    // Add subcommand
    void addSubcommand(const std::string& name, const std::string& description);
    
    // Parse command line
    bool parse(int argc, char* argv[]);
    
    // Check if option exists
    bool hasOption(const std::string& opt) const;
    
    // Get option values
    std::string getOptionAsString(const std::string& opt) const;
    size_t getOptionAs<size_t>(const std::string& opt) const;
    int getOptionAs<int>(const std::string& opt) const;
    float getOptionAs<float>(const std::string& opt) const;
    double getOptionAs<double>(const std::string& opt) const;
    bool getOptionAs<bool>(const std::string& opt) const;
    
    // Get subcommand
    std::string getSubcommand() const { return currentSubcommand_; }
    
    // Get help text
    std::string getHelpText() const;
    
private:
    // Option definition
    struct Option {
        std::string shortOpt;
        std::string longOpt;
        std::string description;
        std::string defaultValue;
        bool hasValue;
        bool exists;
        std::string value;
    };
    
    // Subcommand definition
    struct Subcommand {
        std::string name;
        std::string description;
        bool isCurrent;
    };
    
    std::vector<Option> options_;
    std::vector<Subcommand> subcommands_;
    std::string currentSubcommand_;
    std::string helpText_;
};

} // namespace nlm