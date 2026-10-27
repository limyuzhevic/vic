#pragma once

#include <string>
#include <vector>

namespace nlm {

class CommandLineParser {
public:
    CommandLineParser(int argc, char** argv);
    ~CommandLineParser() = default;
    
    // Parse command line arguments
    bool parse();
    
    // Get parsed values
    std::string getConfigFilePath() const;
    std::string getLogLevel() const;
    std::string getLogFile() const;
    bool isHelpRequested() const;
    bool isVersionRequested() const;
    bool isBatchMode() const;
    bool isVerboseMode() const;
    
    // Get unrecognized arguments for extensibility
    std::vector<std::string> getUnrecognizedArgs() const;
    
    // Display help information
    void displayHelp() const;
    void displayVersion() const;
    
    // Validate parsed arguments
    std::vector<std::string> validateArguments() const;
    
private:
    int argc_;
    char** argv_;
    
    // Parsed values
    std::string configFilePath_;
    std::string logLevel_ = "info";
    std::string logFile_ = "";
    bool helpRequested_ = false;
    bool versionRequested_ = false;
    bool batchMode_ = false;
    bool verboseMode_ = false;
    std::vector<std::string> unrecognizedArgs_;
    
    // Argument parsing helpers
    bool parseArgument(const std::string& arg, size_t& currentArg);
    bool parseConfigArgument(const std::string& arg);
    bool parseLogArgument(const std::string& arg);
};

} // namespace nlm