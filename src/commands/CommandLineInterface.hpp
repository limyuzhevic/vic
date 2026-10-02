#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

// Forward declarations
class CommandLineTool;

class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    bool initialize(int argc, char* argv[]);
    bool run();
    bool shutdown();
    
    bool registerTool(std::unique_ptr<CommandLineTool> tool);
    CommandLineTool* getTool(const std::string& name) const;
    
    std::vector<std::string> getArguments() const { return args_; }
    std::string getProgramName() const { return programName_; }
    
    void printUsage() const;
    void printVersion() const;
    void printHelp() const;
    
private:
    bool parseArguments(int argc, char* argv[]);
    bool processCommand() const;
    bool handleInteractiveMode() const;
    bool handleBatchMode() const;
    bool handleScriptMode(const std::string& scriptPath) const;
    
    std::string programName_;
    std::vector<std::string> args_;
    std::map<std::string, std::unique_ptr<CommandLineTool>> tools_;
    std::string currentMode_;
};

// Command-line option structure
struct CommandLineOptions {
    std::string toolName;
    std::vector<std::string> toolArgs;
    std::string scriptFile;
    bool interactive;
    bool batch;
    bool verbose;
    std::string logFile;
    std::string configFile;
};