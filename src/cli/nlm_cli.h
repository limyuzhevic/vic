// nlm_cli.h - Header for NLM CLI
#pragma once

#include <string>
#include <memory>
#include <optional>

namespace nlm {
    class Config;
    class Brain;
}

struct CLIConfig {
    std::string configFile = "configs/default.cfg";
    std::string outputDir = ".";
    bool verbose = false;
    std::string logLevel = "INFO";
    bool jsonOutput = false;
    std::string profileOutput = "profile.json";
    std::string experimentOutput = "experiment_results.json";
    int maxSteps = 1000;
    double timestep = 0.001;
};

class NLMCLI {
public:
    NLMCLI(int argc, char** argv);
    int run();
    
private:
    void parseArgs();
    void printHelp() const;
    void printVersion() const;
    int runSimulation();
    int runExperiment();
    int handleConfigCommand();
    int handleExportCommand();
    int handleImportCommand();
    int runProfile();
    int runTests();
    int runMonitor();
    int runBenchmark();
    int generateConfigFile();
    int validateConfigFile();
    void saveBrainState(const std::string& filename);
    int exportBrainData(const std::string& format);
    
    int argc;
    char** argv;
    std::shared_ptr<nlm::Config> config;
    
    // Command line options
    std::string subcommand = "run";
    bool helpRequested = false;
    bool versionRequested = false;
    bool configFileProvided = false;
    std::string configFile;
    std::optional<int64_t> neuronCount;
    std::optional<double> timestep;
    std::optional<int> maxSteps;
    bool verbose = false;
    std::string logLevel = "INFO";
    std::string outputFile;
    
    // Additional options
    std::string experimentName;
    std::string exportFormat;
    std::string importFile;
    std::shared_ptr<nlm::Brain> brain;
};
