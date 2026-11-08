// NLM (熙然) - Neural Learning Machine
// CommandLineParser implementation

#include "CommandLineParser.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <map>
#include <string>
#include <vector>

using namespace std;

CommandLineParser::CommandLineParser(int argc, char** argv) {
    parseArguments(argc, argv);
    validateArguments();
}

void CommandLineParser::parseArguments(int argc, char** argv) {
    // Standard flags
    registerFlag("--help", hasHelpFlag_);
    registerFlag("--verbose", verbose_, "-v");
    
    // Configuration file
    registerStringFlag("--config", configFile_, "configs/default.cfg", "-c");
    
    // Test selection
    registerStringFlag("--tests", testNamesStr_, "");
    
    // Output file
    registerStringFlag("--output", outputFile_, "");
    
    // Seed
    registerIntFlag("--seed", seed_, "");
    
    // Steps override
    registerIntFlag("--steps", stepsOverride_, "");
    
    // Current override
    registerFloatFlag("--current", currentOverride_, "");
    
    // Advanced options
    registerStringFlag("--experiment", experimentType_, "");
    registerFlag("--stats-analysis", enableStatsAnalysis_);
    registerFlag("--benchmark", enableBenchmark_);
    registerFlag("--profile", enableProfile_);
    
    // Parse all arguments
    vector<string> remainingArgs;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        bool matched = false;
        
        // Check if it's a flag that takes a value
        if (arg.find("=") != string::npos) {
            // Format: --flag=value or --flag = value
            string flag = arg.substr(0, arg.find("="));
            string value = arg.substr(arg.find("=") + 1);
            
            if (flag == "--config" || flag == "-c") {
                configFile_ = value;
                matched = true;
            } else if (flag == "--tests") {
                testNamesStr_ = value;
                parseTestNames(testNamesStr_);
                matched = true;
            } else if (flag == "--output") {
                outputFile_ = value;
                matched = true;
            } else if (flag == "--seed") {
                try { seed_ = stoi(value); }
                catch (...) {}
                hasSeed_ = true;
                matched = true;
            } else if (flag == "--steps") {
                try { stepsOverride_ = stoi(value); }
                catch (...) {}
                hasStepsOverride_ = true;
                matched = true;
            } else if (flag == "--current") {
                try { currentOverride_ = stof(value); }
                catch (...) {}
                hasCurrentOverride_ = true;
                matched = true;
            } else if (flag == "--experiment") {
                experimentType_ = value;
                matched = true;
            }
        } else {
            // Check for short flags
            if (arg == "-v") {
                verbose_ = true;
                matched = true;
            } else if (arg == "-c" && i + 1 < argc) {
                configFile_ = argv[++i];
                matched = true;
            }
        }
        
        if (!matched) {
            remainingArgs.push_back(arg);
        }
    }
    
    // Parse remaining arguments
    for (const auto& arg : remainingArgs) {
        if (arg == "--help") {
            hasHelpFlag_ = true;
        } else if (arg == "--verbose") {
            verbose_ = true;
        } else if (arg == "--stats-analysis") {
            enableStatsAnalysis_ = true;
        } else if (arg == "--benchmark") {
            enableBenchmark_ = true;
        } else if (arg == "--profile") {
            enableProfile_ = true;
        }
    }
    
    // Parse test names if specified
    if (!testNamesStr_.empty()) {
        parseTestNames(testNamesStr_);
    }
}

void CommandLineParser::validateArguments() {
    // Ensure output file has .json extension if provided
    if (!outputFile_.empty()) {
        string ext = outputFile_.substr(outputFile_.find_last_of('.') + 1);
        if (ext != "json") {
            outputFile_ = outputFile_ + ".json";
        }
    }
}

void CommandLineParser::registerFlag(const string& flag, bool& flagVar, const string& shortForm) {
    // Placeholder - actual flag matching done in parseArguments
}

void CommandLineParser::registerStringFlag(const string& flag, string& flagVar, const string& defaultValue, const string& shortForm) {
    // Placeholder - actual flag matching done in parseArguments
}

void CommandLineParser::registerIntFlag(const string& flag, int64_t& flagVar, const string& shortForm) {
    // Placeholder - actual flag matching done in parseArguments
}

void CommandLineParser::registerFloatFlag(const string& flag, float& flagVar, const string& shortForm) {
    // Placeholder - actual flag matching done in parseArguments
}

void CommandLineParser::parseTestNames(const string& testNamesStr) {
    stringstream ss(testNamesStr);
    string testName;
    
    while (getline(ss, testName, ',')) {
        // Trim whitespace
        size_t start = testName.find_first_not_of(" \t\n\r");
        size_t end = testName.find_last_not_of(" \t\n\r");
        
        if (start != string::npos && end != string::npos) {
            testName = testName.substr(start, end - start + 1);
            testNames_.push_back(testName);
            testSet_.insert(testName);
        }
    }
    
    // Check if "all" was specified
    runAllTests_ = (testSet_.find("all") != testSet_.end());
}

void CommandLineParser::parseTestNamesShort(const string& testNamesStr) {
    // Alternative parsing method for --tests flag
    parseTestNames(testNamesStr);
}

string CommandLineParser::extractFlagValue(const string& arg) const {
    size_t pos = arg.find('=');
    if (pos != string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

bool CommandLineParser::extractFlag(const string& arg) const {
    // Check if argument is a flag
    return arg.find("--") == 0 || (arg.find("-") == 0 && arg.size() > 1);
}
