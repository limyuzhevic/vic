#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <string>
#include <map>

namespace nlm {

class Brain;
class Config;
class Logger;
struct Phase6Config;

// Command line options structure
struct CLIOptions {
    std::string config_file = "configs/default.cfg";
    std::string output_file = "";
    std::string checkpoint_file = "";
    std::string log_file = "";
    std::string mode = "standard"; // standard, debug, benchmark, experiment
    int steps = 1000;
    double timestep = 0.001;
    int neurons = 0; // 0 = use config default
    int regions = 0;
    bool enable_plasticity = true;
    bool enable_development = false;
    bool enable_curiosity = false;
    bool enable_checkpointing = false;
    bool enable_replay = false;
    bool continuous = false;
    bool quiet = false;
    bool verbose = false;
    std::string experiment_type = "standard"; // standard, learning, stmep, plasticity
    std::map<std::string, std::string> custom_params;
    bool help = false;
    bool version = false;
    bool list_configs = false;
};

class NLMCLI {
private:
    CLIOptions options;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    size_t total_spikes_at_start = 0;
    size_t total_steps_completed = 0;
    bool running = false;
    
public:
    NLMCLI(int argc, char** argv);
    ~NLMCLI() = default;
    
    void run();
    void stop();
    
    // Configuration and setup
    void parseArguments(int argc, char** argv);
    void setupLogging();
    void initializeBrain();
    
    // Execution modes
    void runStandardMode();
    void runDebugMode();
    void runBenchmarkMode();
    void runExperimentMode();
    
    // Helper methods
    void logStepProgress(size_t step);
    void logBrainState();
    void logRegionActivity(size_t step);
    void logLearningEvents(size_t step);
    bool checkTerminationCondition(size_t step);
    
    // Output and reporting
    void printHelp();
    void printVersion();
    void listAvailableConfigs();
    void printFinalStats(std::chrono::seconds duration);
    
    // File I/O
    void writeResultsToFile();
    void writeBenchmarkResults(const std::string& filename, double steps_per_second, double spikes_per_step);
    void writeExperimentResults(const std::string& filename, const Phase6Config& config);
    
private:
    // Internal helper methods
    void setupBrainConfiguration();
    void applyCommandLineOptions();
    void captureInitialState();
    void cleanupOnExit();
};

} // namespace nlm

#endif // NLM_CLI_H