#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

namespace NLMCLI {

    // Command line mode enumeration
    enum class Mode {
        NONE,
        DEMO,
        EXPERIMENT,
        BATCH,
        CHECKPOINT,
        VISUALIZE,
        TEST,
        CONFIG,
        HELP
    };

    // Command line arguments structure
    struct CLIArgs {
        Mode mode = Mode::NONE;
        std::string configFile = "configs/default.cfg";
        std::string experimentName;
        std::string batchFile;
        std::string checkpointCommand;
        std::string checkpointPath;
        std::string vizType;
        std::string testSuite;
        std::string configTemplate;
        int verbosity = 1;
        std::string logFile;
        bool quiet = false;
        bool autoSaveCheckpoint = false;
        std::map<std::string, std::string> extraArgs;
    };

    // Initialization and cleanup
    void initialize();
    void cleanup();
    
    // Argument parsing and configuration
    CLIArgs parseArguments(int argc, char* argv[]);
    std::shared_ptr<Config> loadConfig(const std::string& configFile);
    void applyOverrides(std::shared_ptr<Config>& config, const CLIArgs& args);
    
    // Logging setup
    void setupLogging(int verbosity, const std::string& logFile = "", bool quiet = false);
    
    // Experiment running modes
    void runBuiltInDemo(const std::shared_ptr<Config>& config);
    void runNamedExperiment(const std::shared_ptr<Config>& config, const std::string& experimentName);
    void runExperiment(const std::shared_ptr<Config>& config, const std::string& experimentName);
    void runBatch(const std::shared_ptr<Config>& config, const std::string& batchFile);
    void handleCheckpointCommand(const std::string& command, const std::shared_ptr<Config>& config, const std::string& path);
    void runVisualization(const std::string& vizType, const std::shared_ptr<Config>& config);
    void runSystemTests(const std::string& testSuite);
    void generateConfigTemplate(const std::string& templateType);
    
    // Utility functions
    void printHelp();
    void saveCheckpoint(const std::shared_ptr<Config>& config, const std::string& path);
    std::string getVersionInfo();
    std::vector<std::string> listAvailableExperiments();
    std::vector<std::string> listAvailableExperiments();
}

#endif // NLM_CLI_HPP
