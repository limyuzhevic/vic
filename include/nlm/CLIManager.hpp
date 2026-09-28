#ifndef NLM_CLI_MANAGER_H
#define NLM_CLI_MANAGER_H

#include "core/Config/Config.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

/**
 * Command-Line Interface Manager for NLM
 * 
 * Provides a comprehensive CLI for:
 * - Running neural simulations
 * - Managing experiments
 * - Configuration management
 * - Performance monitoring
 * - Advanced debugging tools
 * 
 * Features:
 * - Subcommand-based architecture
 * - YAML/JSON configuration support
 * - Real-time output formatting
 * - Progress reporting
 * - Result export
 * 
 * Designed for both casual users and advanced researchers.
 */
class CLIManager {
public:
    CLIManager();
    ~CLIManager() = default;
    
    // Disable copying
    CLIManager(const CLIManager&) = delete;
    CLIManager& operator=(const CLIManager&) = delete;
    
    // Enable moving
    CLIManager(CLIManager&&) noexcept = default;
    CLIManager& operator=(CLIManager&&) noexcept = default;
    
    /**
     * Run CLI with provided arguments
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return Exit code (0 for success)
     */
    int run(int argc, char* argv[]);
    
    /**
     * Register a subcommand
     * 
     * @param name Subcommand name
     * @param description Subcommand description
     * @param handler Handler function
     */
    void registerSubcommand(const std::string& name,
                           const std::string& description,
                           std::function<int(const std::vector<std::string>&)> handler);
    
    /**
     * Print help for all subcommands
     */
    void printHelp() const;
    
    /**
     * Print help for a specific subcommand
     * 
     * @param name Subcommand name
     */
    void printSubcommandHelp(const std::string& name) const;
    
    /**
     * Set output format
     * 
     * @param format Output format (json, yaml, table, verbose)
     */
    void setOutputFormat(const std::string& format) { outputFormat_ = format; }
    
    /**
     * Set log level
     * 
     * @param level Log level (debug, info, warn, error)
     */
    void setLogLevel(const std::string& level) { logLevel_ = level; }
    
    /**
     * Enable verbose output
     */
    void enableVerbose() { verbose_ = true; }

private:
    struct Subcommand {
        std::string name;
        std::string description;
        std::function<int(const std::vector<std::string>&)> handler;
    };
    
    std::vector<Subcommand> subcommands_;
    std::string outputFormat_;
    std::string logLevel_;
    bool verbose_;
    
    // Subcommand handlers
    int handleRun(const std::vector<std::string>& args);
    int handleConfig(const std::vector<std::string>& args);
    int handleExperiment(const std::vector<std::string>& args);
    int handleBenchmark(const std::vector<std::string>& args);
    int handleDebug(const std::vector<std::string>& args);
    int handleHelp(const std::vector<std::string>& args);
    
    // Helper methods
    void parseArguments(int argc, char* argv[], 
                       std::string& command,
                       std::vector<std::string>& args) const;
    
    Phase6Config parsePhase6Config(const std::vector<std::string>& args) const;
    
    std::shared_ptr<Config> parseConfigFile(const std::string& filepath) const;
    
    void printConfig(const Phase6Config& config) const;
    
    void printExperimentResult(const Phase6IntegrationResult& result) const;
    
    void exportResult(const Phase6IntegrationResult& result,
                     const std::string& format,
                     const std::string& filename) const;
    
    std::string formatOutput(const std::string& format,
                           const std::map<std::string, std::string>& data) const;
};

} // namespace nlm

#endif // NLM_CLI_MANAGER_H
