#ifndef NLM_CLI_H
#define NLM_CLI_H

#include "main.cpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace nlm {

class CLI {
public:
    CLI();
    ~CLI();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Execute the requested command
    int execute();
    
    // Print help message
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
    // Get the configuration
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Get the requested command
    const std::string& getCommand() const { return command; }
    
    // Get command arguments
    const std::vector<std::string>& getArgs() const { return args; }
    
    // Get verbose mode
    bool getVerbose() const { return verbose; }
    
    // Get quiet mode
    bool getQuiet() const { return quiet; }
    
private:
    // Command types
    enum class CommandType {
        Help,
        Version,
        Init,
        Reset,
        Run,
        Save,
        Load,
        Status,
        Test,
        Config,
        Log,
        Explore
    };
    
    // Parse command line string to command type
    CommandType parseCommand(const std::string& cmd) const;
    
    // Parse boolean flag
    bool parseFlag(const std::string& flag, const std::vector<std::string>& args) const;
    
    // Parse value for a flag
    std::string parseFlagValue(const std::string& flag, const std::vector<std::string>& args) const;
    
    // Execute specific commands
    int executeHelp() const;
    int executeVersion() const;
    int executeInit();
    int executeReset();
    int executeRun();
    int executeSave();
    int executeLoad();
    int executeStatus();
    int executeTest();
    int executeConfig();
    int executeLog();
    int executeExplore();
    
    // Command execution
    int runBrainSimulation();
    int runTests();
    int manageConfig();
    int manageLogging();
    int exploreBrain();
    
    // Common functionality
    bool validateConfig();
    void setupLogging();
    void printBrainInfo();
    
    // Member variables
    std::string command;
    std::vector<std::string> args;
    std::shared_ptr<Config> config;
    bool verbose;
    bool quiet;
    bool interactive;
    std::string configFile;
    std::string outputFile;
    std::string logLevel;
    std::string logFile;
};

} // namespace nlm

#endif // NLM_CLI_H