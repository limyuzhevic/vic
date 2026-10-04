#pragma once

#include <string>
#include <vector>

namespace nlm {

/**
 * CLI argument parser with help system
 */
class CLIParser {
public:
    struct CommandLineOptions {
        bool showHelp = false;
        bool showVersion = false;
        bool verbose = false;
        bool quiet = false;
        bool interactive = false;
        bool exportResults = false;
        bool importResults = false;
        std::string configFile;
        std::string outputFile;
        std::string logFile;
        std::string educationMode;
        std::string powerUserMode;
        std::vector<std::string> customArgs;
        bool enableColors = true;
        bool enableProgress = true;
        bool enableLogging = true;
        int maxSteps = 2000;
        int maxEpisodes = 5;
        float timeout = 0.0f;
        std::string exportFormat;
        std::string importFormat;
    };
    
    CLIParser() = default;
    ~CLIParser() = default;
    
    /**
     * Parse command line arguments
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return Parsed options
     */
    CommandLineOptions parse(int argc, char* argv[]);
    
    /**
     * Display help information
     */
    void showHelp() const;
    
    /**
     * Display version information
     */
    void showVersion() const;
    
    /**
     * Check if argument matches pattern
     * 
     * @param arg Argument to check
     * @param pattern Pattern to match
     * @param isLongOption Long option prefix
     * @return true if matches
     */
    bool matches(const std::string& arg, const std::string& pattern, bool isLongOption = false) const;
    
    /**
     * Extract value from argument
     * 
     * @param arg Argument with value
     * @param hasPrefix Whether argument has prefix (--)
     * @return Extracted value
     */
    std::string extractValue(const std::string& arg, bool hasPrefix) const;
};

} // namespace nlm
