/*! 
 * NLM Neural Network Simulator v0.4.0 CLI Framework
 * 
 * Main CLI interface and command structure for the NLM neural network simulation framework.
 * Provides comprehensive command-line interface for running simulations, configuring experiments,
 * and analyzing results.
 */

#ifndef NLMCLI_HPP
#define NLMCLI_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>
#include "CLICommand.hpp"

namespace nlm {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    /**
     * Main CLI entry point
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return Exit code (0 for success, non-zero for error)
     */
    int run(int argc, char* argv[]);
    
    /**
     * Register a command with the CLI
     * 
     * @param name Command name
     * @param command Command implementation
     */
    void registerCommand(const std::string& name, std::unique_ptr<CLICommand> command);
    
    /**
     * Print help information
     */
    void printHelp() const;
    
    /**
     * Print version information
     */
    void printVersion() const;
    
    /**
     * Parse command line arguments
     * 
     * @param argc Argument count
     * @param argv Argument values
     */
    void parseArguments(int argc, char* argv[]);
    
    /**
     * Get global options
     */
    struct GlobalOptions {
        bool verbose = false;
        bool quiet = false;
        std::string configFile;
        std::string outputDir;
        std::string logLevel;
        int threadCount = 1;
        std::string simulationFile;
        std::string experimentConfig;
    };
    
    const GlobalOptions& getGlobalOptions() const { return globalOptions; }
    
    /**
     * Set global options
     */
    void setGlobalOptions(const GlobalOptions& options) { globalOptions = options; }
    
private:
    GlobalOptions globalOptions;
    std::map<std::string, std::unique_ptr<CLICommand>> commands;
    std::string version;
    
    /**
     * Process command line options
     */
    void processOptions(std::vector<std::string>& args);
    
    /**
     * Execute registered command
     */
    int executeCommand(const std::string& commandName, const std::vector<std::string>& args);
};

} // namespace nlm

#endif // NLMCLI_HPP
