#ifndef NLMCLI_HPP
#define NLMCLI_HPP

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <ctime>

namespace nlm {
namespace cli {

class NLMCLI {
public:
    NLMCLI() = default;
    ~NLMCLI() = default;
    
    // Run the CLI with given arguments
    int run(int argc, char* argv[]);
    
    // Display help information
    static void printHelp(const std::string& programName);
    
    // Display version information
    static void printVersion();
    
    // Display advanced help information
    static void printAdvancedHelp();
    
private:
    // Parse command line arguments
    struct CLIConfig {
        std::string mode = "demo";  // demo, experiment, explore, benchmark
        std::string configFile = "configs/default.cfg";
        std::string checkpointFile;
        bool verbose = false;
        bool quiet = false;
        int steps = 100;
        int neurons = 500;
        std::string outputFile;
        bool enableCheckpointing = false;
        bool enableReplay = false;
        bool enableDevelopment = true;
        std::string logLevel = "info";
        std::string experimentMode = "quick";  // quick, thorough, custom
    };
    
    CLIConfig parseArguments(int argc, char* argv[]);
    
    // Command execution methods
    int executeDemoMode(const CLIConfig& config);
    int executeExperimentMode(const CLIConfig& config);
    int executeExploreMode(const CLIConfig& config);
    int executeBenchmarkMode(const CLIConfig& config);
    
    // Helper methods
    void setupConfiguration(std::shared_ptr<Config> config, const CLIConfig& cliConfig);
    void setupLogging(const std::string& level);
    void printBrainStatus(const std::shared_ptr<Brain>& brain);
    void printExperimentResults(const Phase6IntegrationResult& result);
    
    // Utility methods
    std::string getCurrentTimestamp();
    void log(const std::string& message, const std::string& level = "INFO");
    void error(const std::string& message);
    
    // Configuration file utilities
    bool validateConfigFile(const std::string& filepath);
    void createDefaultConfig(const std::string& filepath);
    
    // Output utilities
    void saveResultsToFile(const std::string& filename, const std::string& data);
    std::string formatResults(const Phase6IntegrationResult& result);
};

// Command-line entry point
inline int main(int argc, char* argv[]) {
    NLMCLI cli;
    return cli.run(argc, argv);
}

} // namespace cli
} // namespace nlm

#endif // NLMCLI_HPP
