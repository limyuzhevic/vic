// NLM Command Line Interface Header
// Comprehensive CLI for NLM experiments and simulations

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace nlm {

/**
 * Simple argument parser for NLM CLI
 */
class SimpleArgParser {
public:
    SimpleArgParser(const std::string& programName);
    
    /**
     * Add a subcommand
     */
    class Subcommand {
    public:
        Subcommand& add_argument(const std::string& name, const std::string& help = "");
        Subcommand& default_value(const std::string& value);
        Subcommand& default_value(int value);
        Subcommand& default_value(bool value);
        Subcommand& type(const std::string& typeName);
        Subcommand& help(const std::string& helpText);
        Subcommand& set_callback(std::function<void()> callback);
        
        template<typename T>
        void setArg(const std::string& name, const T& value);
        
        template<typename T>
        T getArg(const std::string& name) const;
    
    private:
        std::string _name;
        std::string _help;
        std::map<std::string, std::string> _args;
        std::map<std::string, std::string> _defaultValues;
        std::function<void()> _callback;
        std::string _type;
    };
    
    Subcommand add_subcommand(const std::string& name, const std::string& help = "");
    
    /**
     * Add a global argument
     */
    class GlobalArgument {
    public:
        GlobalArgument& default_value(const std::string& value);
        GlobalArgument& default_value(int value);
        GlobalArgument& default_value(bool value);
        GlobalArgument& type(const std::string& typeName);
        GlobalArgument& help(const std::string& helpText);
        GlobalArgument& action(std::function<void()> action);
        
        template<typename T>
        void setArg(const std::string& name, const T& value);
        
        template<typename T>
        T getArg(const std::string& name) const;
    
    private:
        std::map<std::string, std::string> _args;
        std::map<std::string, std::string> _defaultValues;
        std::function<void()> _action;
        std::string _type;
    };
    
    GlobalArgument add_argument(const std::string& name, const std::string& help = "");
    
    /**
     * Parse command line arguments
     */
    void parse();
    
    /**
     * Print help information
     */
    void printHelp(std::ostream& out = std::cout);
    
    /**
     * Get all subcommands
     */
    const std::vector<std::string>& getSubcommands() const;
    
    /**
     * Check if a subcommand was specified
     */
    bool hasSubcommand(const std::string& name) const;

private:
    std::string _programName;
    std::vector<std::string> _subcommands;
    std::map<std::string, Subcommand> _subcommandMap;
    std::map<std::string, GlobalArgument> _globalArguments;
    std::vector<std::string> _positionalArgs;
};

/**
 * Main command line interface for NLM
 */
class NLMCommandLine {
public:
    NLMCommandLine();
    
    /**
     * Run the command line interface
     */
    void run();
    
    /**
     * Print version information
     */
    void printVersion();
    
    /**
     * Print help information
     */
    void printHelp();
    
    /**
     * Run Phase 3 experiment
     */
    void runPhase3();
    
    /**
     * Run Phase 4 experiment
     */
    void runPhase4();
    
    /**
     * Run Phase 6 integration
     */
    void runPhase6();
    
    /**
     * Profile a simulation
     */
    void profileSimulation();
    
    /**
     * Handle configuration commands
     */
    void handleConfig();
    
    /**
     * Run batch of experiments
     */
    void runBatch();
    
    /**
     * Compare results
     */
    void compareResults();

private:
    SimpleArgParser _parser;
    
    // Helper methods
    void setupCommands();
    void validateConfig(const std::string& filepath);
    void saveConfig(const std::string& filepath);
    void listConfigOptions();
    void createDefaultConfig(const std::string& filepath);
    void printResults(const ExperimentResult& result, const std::string& outputFile);
    void printPhase6Results(const Phase6IntegrationResult& result, const std::string& outputFile);
};

} // namespace nlm