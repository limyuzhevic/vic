#pragma once

namespace nlm {

/**
 * Command-line interface header for NLM (Neural Learning Machine)
 * 
 * Provides comprehensive command-line interface for NLM simulations including:
 * - Multiple subcommands (run, config, benchmark, profile, etc.)
 * - Configuration file management
 * - Experiment control
 * - Performance monitoring
 * - Batch processing capabilities
 * - Advanced configuration options
 */
class CLI {
public:
    CLI();
    ~CLI();
    
    /**
     * Run the command-line interface with provided arguments
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return Exit code (0 = success, non-zero = error)
     */
    int run(int argc, char** argv);
    
    /**
     * Print help information for all commands
     */
    void printHelp() const;
    
    /**
     * Print version information
     */
    void printVersion() const;
    
    /**
     * Get the current configuration
     */
    std::shared_ptr<Config> getCurrentConfig() const { return currentConfig_; }
    
    /**
     * Set the current configuration
     */
    void setCurrentConfig(const std::shared_ptr<Config>& config) { currentConfig_ = config; }
    
protected:
    // Command handler methods (protected for inheritance)
    virtual int handleRunCommand(int argc, char** argv);
    virtual int handleConfigCommand(int argc, char** argv);
    virtual int handleBenchmarkCommand(int argc, char** argv);
    virtual int handleProfileCommand(int argc, char** argv);
    virtual int handleExperimentCommand(int argc, char** argv);
    virtual int handleBatchCommand(int argc, char** argv);
    virtual int handleCheckCommand(int argc, char** argv);
    virtual int handleGenerateCommand(int argc, char** argv);
    
    // Subcommand handlers
    virtual int handleConfigLoad(int argc, char** argv);
    virtual int handleConfigSave(int argc, char** argv);
    virtual int handleConfigShow(int argc, char** argv);
    virtual int handleConfigValidate(int argc, char** argv);
    virtual int handleConfigGet(int argc, char** argv);
    virtual int handleConfigSet(int argc, char** argv);
    
    virtual int handleExperimentRun(int argc, char** argv);
    virtual int handleExperimentList(int argc, char** argv);
    virtual int handleExperimentInfo(int argc, char** argv);
    virtual int handleExperimentCreate(int argc, char** argv);
    
    virtual int handleBatchRun(int argc, char** argv);
    virtual int handleBatchCreate(int argc, char** argv);
    
    virtual int handleGenerateConfig(int argc, char** argv);
    virtual int handleGenerateDocs(int argc, char** argv);
    virtual int handleGenerateTest(int argc, char** argv);
    
private:
    // Help printing methods
    void printRunHelp() const;
    void printConfigHelp() const;
    void printBenchmarkHelp() const;
    void printProfileHelp() const;
    void printExperimentHelp() const;
    void printBatchHelp() const;
    void printCheckHelp() const;
    void printGenerateHelp() const;
    
    // Core functionality
    int runSimulation(const std::string& configFile, size_t steps, const std::string& outputFile, bool debug, bool progress);
    int runBenchmarks(const std::vector<std::pair<size_t, size_t>>& benchmarks, const std::string& outputFile, bool verbose);
    int profileSimulation(const std::string& type, const std::string& outputFile, double duration, bool continuous);
    int checkSystem(bool validateConfig, bool validateFiles, const std::string& configFile, const std::string& filePattern);
    
    // Configuration management
    std::shared_ptr<Config> currentConfig_;
};

} // namespace nlm
