// CommandLineParser.hpp - Handles command line argument parsing
#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <iostream>

namespace nlm {

class CommandLineParser {
public:
    struct Options {
        bool help = false;
        bool version = false;
        std::string verbose;
        std::string quiet;
        std::string debug;
        std::string experiment;
        std::string configFile;
        bool dryRun = false;
        bool validateOnly = false;
        bool gpuEnabled = false;
        std::vector<std::string> customScripts;
        std::string outputFormat;
        std::string logFile;
        bool profilingEnabled = false;
        bool memoryTrackingEnabled = false;
        int profilingGranularity = 1000;
        std::string profilingOutput;
    };

    CommandLineParser(int argc, char** argv);
    ~CommandLineParser() = default;
    
    // Parse command line arguments
    bool parse();
    
    // Get parsed options
    const Options& getOptions() const { return options_; }
    
    // Show help information
    static void showHelp(const std::string& programName);
    
    // Show version information
    static void showVersion();
    
    // Get program name
    static std::string getProgramName(const std::string& argv0);
    
    // Check if specific option was provided
    bool hasOption(const std::string& option) const;
    
    // Get specific option value
    std::string getOptionValue(const std::string& option) const;

private:
    int argc_;
    char** argv_;
    Options options_;
    std::string programName_;
    
    // Internal parsing methods
    void parseArguments();
    void processFlag(const std::string& arg, size_t& i);
    void processOption(const std::string& arg, size_t& i);
    void normalizeOptions();
};

} // namespace nlm
