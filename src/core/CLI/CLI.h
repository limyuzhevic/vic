#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include "CommonArgs.h"

namespace nlm {

/**
 * @brief Advanced command-line interface for NLM programs
 * 
 * Provides comprehensive command-line parsing with help, version, validation,
 * and configuration options for all NLM demo programs.
 */
class CLI {
public:
    /**
     * @brief Command-line argument structure
     */
    struct Argument {
        std::string name;          // Long option name (e.g., "--episodes")
        char shortName;            // Short option name (e.g., 'e')
        std::string description;   // Human-readable description
        std::string defaultValue;  // Default value if not provided
        bool hasValue;             // Whether this argument takes a value
        std::function<void(const std::string&)> callback; // Optional callback
        
        Argument(const std::string& n, char s, const std::string& d, 
                 const std::string& def = "", bool hv = false,
                 const std::function<void(const std::string&)>& cb = nullptr)
            : name(n), shortName(s), description(d), defaultValue(def),
              hasValue(hv), callback(cb) {}
    };
    
    /**
     * @brief Command categories for organization
     */
    enum class CommandCategory {
        BASIC,           // Basic run parameters
        CONFIG,          // Configuration options
        OUTPUT,          // Output and logging options
        DEBUG,           // Debug and development options
        ADVANCED         // Advanced/experimental options
    };
    
    /**
     * @brief Program metadata
     */
    struct ProgramInfo {
        std::string name;           // Program name
        std::string version;        // Version string
        std::string description;    // Program description
        std::string author;         // Author/credit
        std::string website;        // Website/project URL
        
        ProgramInfo(const std::string& n, const std::string& v,
                   const std::string& d, const std::string& a,
                   const std::string& w = "")
            : name(n), version(v), description(d), author(a), website(w) {}
    };
    
    CLI();
    virtual ~CLI() = default;
    
    /**
     * @brief Set program metadata
     */
    void setProgramInfo(const ProgramInfo& info);
    
    /**
     * @brief Add a command-line argument
     */
    void addArgument(const std::string& name, char shortName, 
                     const std::string& description, const std::string& defaultValue = "",
                     bool hasValue = false, const std::function<void(const std::string&)>& callback = nullptr,
                     CommandCategory category = CommandCategory::BASIC);
    
    /**
     * @brief Parse command-line arguments
     * @param argc Argument count
     * @param argv Argument vector
     * @return true if parsing succeeded, false if help/version was requested or error occurred
     */
    bool parse(int argc, char* argv[]);
    
    /**
     * @brief Get argument value as string
     */
    std::string getString(const std::string& name, const std::string& defaultValue = "") const;
    
    /**
     * @brief Get argument value as integer
     */
    int getInt(const std::string& name, int defaultValue = 0) const;
    
    /**
     * @brief Get argument value as float
     */
    float getFloat(const std::string& name, float defaultValue = 0.0f) const;
    
    /**
     * @brief Get argument value as boolean (presence only)
     */
    bool getBool(const std::string& name) const;
    
    /**
     * @brief Check if an argument was provided
     */
    bool hasArgument(const std::string& name) const;
    
    /**
     * @brief Get all unrecognized arguments (for future extensibility)
     */
    const std::vector<std::string>& getExtraArgs() const;
    
    /**
     * @brief Print help information
     */
    void printHelp() const;
    
    /**
     * @brief Print version information
     */
    void printVersion() const;
    
    /**
     * @brief Print configuration summary
     */
    void printConfig() const;
    
    /**
     * @brief Check if help was requested
     */
    bool isHelpRequested() const;
    
    /**
     * @brief Check if version was requested
     */
    bool isVersionRequested() const;
    
    /**
     * @brief Get the command-line argument string
     */
    const std::string& getArgString() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void setupDefaultArguments();
    bool parseArgument(const std::string& arg, std::map<std::string, std::string>& parsedArgs);
    void printArgumentHelp(CommandCategory category) const;
};

/**
 * @brief Convenience function to create common CLI setups for NLM programs
 */
namespace cli {
    
    /**
     * @brief Create a basic NLM demo CLI with common arguments
     * @param programName Program name
     * @param programVersion Program version
     * @return Configured CLI instance
     */
    CLI createDemoCLI(const std::string& programName, const std::string& programVersion);
    
    /**
     * @brief Create an advanced NLM demo CLI with all available options
     * @param programName Program name
     * @param programVersion Program version
     * @return Configured CLI instance
     */
    CLI createAdvancedDemoCLI(const std::string& programName, const std::string& programVersion);
    
    /**
     * @brief Create experiment metrics CSV output
     */
    std::string createMetricsCSV(const std::string& filename, 
                                 const std::vector<std::map<std::string, std::string>>& metrics);
    
    /**
     * @brief Validate configuration parameters
     */
    bool validateConfig(const std::map<std::string, std::string>& config);
    
    /**
     * @brief Print program banner with metadata
     */
    void printBanner(const std::string& programName, const std::string& version,
                    const std::string& description = "");
}

} // namespace nlm

#endif // NLM_CLI_H

