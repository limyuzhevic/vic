#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <optional>

namespace nlm {

class CommandLineParser {
public:
    // Command line option definition
    struct Option {
        std::string longName;
        char shortName;
        std::string description;
        std::string defaultValue;
        std::string type; // "string", "int", "float", "bool"
        bool required;
        std::function<void(const std::string&)> callback;
        
        Option(const std::string& longOpt, char shortOpt, const std::string& desc,
               const std::string& defaultVal = "", const std::string& optType = "string",
               bool req = false, const std::function<void(const std::string&)>& cb = nullptr)
            : longName(longOpt), shortName(shortOpt), description(desc),
              defaultValue(defaultVal), type(optType), required(req), callback(cb) {}
    };
    
    CommandLineParser(const std::string& programName, const std::string& description);
    ~CommandLineParser();
    
    // Disable copying, enable moving
    CommandLineParser(const CommandLineParser&) = delete;
    CommandLineParser& operator=(const CommandLineParser&) = delete;
    CommandLineParser(CommandLineParser&&) noexcept;
    CommandLineParser& operator=(CommandLineParser&&) noexcept;
    
    // Add an option
    void addOption(const std::string& longName, char shortName, const std::string& description,
                   const std::string& defaultValue = "", const std::string& type = "string",
                   bool required = false, const std::function<void(const std::string&)>& callback = nullptr);
    
    // Add a flag (boolean option)
    void addFlag(const std::string& longName, char shortName, const std::string& description,
                 bool defaultValue = false);
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed values
    std::string getString(const std::string& option) const;
    int getInt(const std::string& option) const;
    float getFloat(const std::string& option) const;
    bool getBool(const std::string& option) const;
    bool hasOption(const std::string& option) const;
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const;
    
    // Print help/usage information
    void printHelp() const;
    
    // Check if help was requested
    bool wasHelpRequested() const;
    
    // Check if version was requested
    bool wasVersionRequested() const;
    
    // Get program name
    const std::string& getProgramName() const;
    
    // Get description
    const std::string& getDescription() const;
    
    // Get version
    static const std::string& getVersion();
    
    // Get build information
    static const std::string& getBuildInfo();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void setupDefaultOptions();
    void processArgument(const std::string& arg, size_t index, int argc, char** argv);
    bool validateOptions() const;
    void printError(const std::string& message) const;
    std::string trim(const std::string& str) const;
};

} // namespace nlm
