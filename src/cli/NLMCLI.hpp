// NLM Command Line Interface
// Provides user-friendly interface for NLM (Neural Learning Machine)
// Supports multiple modes: demo, experiment, benchmark, analysis

#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <memory>
#include <map>
#include <functional>

namespace nlm {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    // Main entry point
    int run(int argc, char** argv);
    
    // Command registration
    void registerCommand(const std::string& name, 
                        const std::string& description,
                        std::function<int(const std::vector<std::string>&)> handler);
    
    // Help and usage
    void printHelp() const;
    void printVersion() const;
    
    // Configuration management
    bool loadConfiguration(const std::string& configFile = "configs/default.cfg");
    bool saveConfiguration(const std::string& configFile = "configs/custom.cfg");
    
    // Experiment management
    bool runExperiment(const std::string& experimentName, 
                      const std::map<std::string, std::string>& parameters = {});
    bool listExperiments() const;
    bool saveExperiment(const std::string& experimentName);
    
    // Brain and simulation control
    std::shared_ptr<Brain> createBrain(const Config& config) const;
    bool runSimulation(std::shared_ptr<Brain> brain, int steps = 1000);
    bool runBrainDiagnostics(std::shared_ptr<Brain> brain);
    
    // Output and visualization
    void printBrainStatus(std::shared_ptr<Brain> brain) const;
    bool exportBrainState(const std::string& filename, std::shared_ptr<Brain> brain) const;
    bool generateReports(const std::string& outputDir, std::shared_ptr<Brain> brain) const;
    
    // Configuration helpers
    bool setConfigValue(const std::string& key, const std::string& value);
    bool getConfigValue(const std::string& key, std::string& value) const;
    std::map<std::string, std::string> getAllConfigValues() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Command line parsing
    bool parseArguments(int argc, char** argv);
    bool executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Helper methods
    std::string getTimestamp() const;
    std::string sanitizeFilename(const std::string& filename) const;
    bool ensureDirectoryExists(const std::string& path) const;
};

// Convenience functions
void initializeNLM();
void cleanupNLM();

// Command line shortcuts
int nlmDemo(const std::vector<std::string>& args);
int nlmExperiment(const std::vector<std::string>& args);
int nlmBenchmark(const std::vector<std::string>& args);
int nlmConfig(const std::vector<std::string>& args);
int nlmAnalyze(const std::vector<std::string>& args);

} // namespace nlm

#endif // NLM_CLI_HPP