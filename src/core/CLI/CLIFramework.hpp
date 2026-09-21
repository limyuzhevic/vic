#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <fstream>

// Forward declarations
class ProgressBar;
class OutputFormatter;
class HelpSystem;

enum class ExperienceLevel {
    BEGINNER,
    INTERMEDIATE,
    ADVANCED
};

struct CLIOptions {
    bool help = false;
    bool version = false;
    bool testMode = false;
    bool verbose = false;
    bool quiet = false;
    bool plasticity = false;
    bool logToFile = false;
    std::string logFilename;
    double timestep = 0.001;
    int neurons = 500;
    int timeout = 0;
    std::string configFile;
    std::string outputFile;
    std::string outputFormat = "console"; // console, json, summary
    std::vector<std::string> commands;
    std::unordered_map<std::string, std::string> customParams;
};

class ProgressBar {
public:
    ProgressBar(int total, const std::string& label = "Progress", bool showPercentage = true);
    
    void update(int current, const std::string& status = "");
    void finish();
    void setTotal(int total);
    
private:
    int total;
    int current;
    std::string label;
    bool showPercentage;
    auto startTime = std::chrono::high_resolution_clock::now();
};

class OutputFormatter {
public:
    OutputFormatter(bool useColors = true, bool verbose = false);
    
    void setLevel(ExperienceLevel level);
    
    void printBanner() const;
    void printHelp(const CLIOptions& options, ExperienceLevel level) const;
    void printSection(const std::string& title, const std::string& content) const;
    void printError(const std::string& message) const;
    void printWarning(const std::string& message) const;
    void printSuccess(const std::string& message) const;
    void printInfo(const std::string& message) const;
    void printDebug(const std::string& message) const;
    void printCommand(const std::string& command) const;
    
    std::string formatProgress(const std::string& label, int current, int total) const;
    std::string formatParameter(const std::string& key, const std::string& value) const;
    
    void setOutputFile(const std::string& filename);
    void closeOutputFile();
    
    void printJSON(const std::string& key, const std::string& value) const;
    void printJSON(const std::string& key, int value) const;
    void printJSON(const std::string& key, double value) const;
    void printJSON(const std::string& key, bool value) const;
    
private:
    bool useColors;
    bool verbose;
    ExperienceLevel level;
    std::ofstream outputFile;
    bool fileOutputActive = false;
};

class HelpSystem {
public:
    HelpSystem(const OutputFormatter& formatter);
    
    void showHelp(ExperienceLevel level, const CLIOptions& options) const;
    void showExamples(ExperienceLevel level) const;
    void showParameterInfo(const std::string& param) const;
    
private:
    const OutputFormatter& formatter;
    
    void showBeginnerHelp() const;
    void showIntermediateHelp() const;
    void showAdvancedHelp() const;
};

class CLIParser {
public:
    CLIParser(HelpSystem& helpSystem);
    
    CLIOptions parse(int argc, char** argv);
    ExperienceLevel detectExperienceLevel(const CLIOptions& options) const;
    
private:
    HelpSystem& helpSystem;
    
    CLIOptions parseArguments(int argc, char** argv);
    void handleSpecialArguments(CLIOptions& options, int argc, char** argv);
    bool isValidParameter(const std::string& param) const;
    std::string getParameterValue(int argc, char** argv, int& index) const;
    void validateOptions(CLIOptions& options) const;
};

// Global instances (for easy access)
inline std::unique_ptr<ProgressBar> globalProgressBar;
inline std::unique_ptr<OutputFormatter> globalFormatter;
inline std::unique_ptr<HelpSystem> globalHelpSystem;
inline std::unique_ptr<CLIParser> globalParser;

// Utility functions
namespace CLIUtils {
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string currentTimestamp();
    void sleep(int milliseconds);
    bool fileExists(const std::string& filename);
    
    // Progress monitoring utilities
    std::string formatTime(double seconds);
    double getElapsedTime(const auto& startTime);
    
    // Output formatting utilities
    std::string colorize(const std::string& text, const std::string& color);
    std::string bold(const std::string& text);
    std::string dim(const std::string& text);
    
    // Experience level detection
    ExperienceLevel detectExperienceLevel(const CLIOptions& options);
    
    // Parameter validation
    bool isValidNeuronCount(int neurons);
    bool isValidTimestep(double timestep);
    bool isValidTimeout(int timeout);
}
