#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>
#include <csignal>
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "CLICommand.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

/**
 * Main NLM CLI class - handles command-line interface for NLM (熙然)
 * 
 * Provides a comprehensive command-line interface for NLM with commands for:
 * - Basic brain operations (run, brain, config, experiment)
 * - Phase 6 integration commands (demo, integration, verify)
 * - Advanced user features (benchmark, compare, analyze)
 * - Configuration management (--config, --param, --output)
 */
class NLMCLI {
public:
    NLMCLI(int argc, char** argv);
    ~NLMCLI();
    
    int run();
    
private:
    int argc_;
    char** argv_;
    bool verbose_;
    bool helpRequested_;
    bool versionRequested_;
    std::string outputDir_;
    std::string configFile_;
    std::map<std::string, std::string> paramOverrides_;
    bool dryRun_;
    bool background_;
    
    // Command execution methods
    void executeRunCommand(const std::map<std::string, std::string>& params);
    void executeBrainCommand(const std::map<std::string, std::string>& params);
    void executeConfigCommand(const std::map<std::string, std::string>& params);
    void executeExperimentCommand(const std::map<std::string, std::string>& params);
    void executeDemoCommand(const std::map<std::string, std::string>& params);
    void executeIntegrationCommand(const std::map<std::string, std::string>& params);
    void executeVerifyCommand(const std::map<std::string, std::string>& params);
    void executeBenchmarkCommand(const std::map<std::string, std::string>& params);
    void executeCompareCommand(const std::map<std::string, std::string>& params);
    void executeAnalyzeCommand(const std::map<std::string, std::string>& params);
    
    // Helper methods
    void parseArguments();
    void setupLogging();
    void setupConfig();
    void setupExperimentRunner();
    void printHelp(const std::string& programName) const;
    void printVersion() const;
    void signalHandler(int signal);
    
    std::string parseOptionValue(const std::string& arg, const std::string& option);
    void loadParameters(const std::string& paramFile);
    std::string trim(const std::string& str);
};

} // namespace nlm
