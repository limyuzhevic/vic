// NLM CLI Tool
// Command-line interface for NLM brain simulation with configuration support

#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>

// Command-line argument parsing
class CLIParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::vector<std::string> args;
    };
    
    CLIParser(int argc, char** argv);
    
    bool hasCommand(const std::string& cmd) const;
    
    std::string getCommand(const std::string& cmd) const;
    
    std::string getArg(const std::string& cmd, const std::string& arg) const;
    
    bool hasArg(const std::string& cmd, const std::string& arg) const;
    
    const std::map<std::string, Command>& getAllCommands() const;
    
    std::string getValue(const std::string& key) const;
    
    bool hasValue(const std::string& key) const;
    
    void printHelp() const;
    
private:
    void parseArgs(int argc, char** argv);
    
    bool showHelp = false;
    std::map<std::string, Command> commands;
    std::map<std::string, std::string> values;
};

// NLM CLI Application
class NLMCLI {
public:
    NLMCLI(int argc, char** argv);
    
    bool run();
    
private:
    bool runCreateCommand();
    bool runStepCommand();
    bool runSaveCommand();
    bool runLoadCommand();
    bool runResetCommand();
    bool runDemoCommand();
    bool runExperimentCommand();
    bool runConfigCommand();
    bool runImportCommand();
    bool runExportCommand();
    
    bool runBasicDemo();
    void runPlasticityDemo(std::shared_ptr<Brain> brain);
    
    CLIParser parser;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};
