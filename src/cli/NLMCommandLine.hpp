#pragma once

#include <string>
#include <memory>

namespace nlm {

class NLMCommandLine {
public:
    NLMCommandLine();
    ~NLMCommandLine();
    
    // Run command line interface
    int run(int argc, char** argv);
    
    // Help methods
    void printHelp() const;
    void printConfigureHelp() const;
    void printTestHelp() const;
    void printRunHelp() const;
    void printPluginHelp() const;
    void printCheckpointHelp() const;
    void printBenchmarkHelp() const;
    
    // Version and info
    void printVersion() const;
    
private:
    // Command handlers
    int handleConfigureCommand(int argc, char** argv);
    int handleTestCommand(int argc, char** argv);
    int handleRunCommand(int argc, char** argv);
    int handlePluginCommand(int argc, char** argv);
    int handleCheckpointCommand(int argc, char** argv);
    int handleBenchmarkCommand(int argc, char** argv);
    
    // Helper methods
    bool validateConfiguration() const;
    void printDefaultConfiguration() const;
    
    // Test execution
    int runUnitTests(bool verbose, const std::string& outputFile);
    int runIntegrationTests(bool verbose, const std::string& outputFile);
    int runPerformanceTests(bool verbose, const std::string& outputFile);
    int runBenchmarkTests(bool verbose, const std::string& outputFile);
    
    // Plugin management
    int handlePluginList();
    int handlePluginLoad(int argc, char** argv);
    int handlePluginUnload(int argc, char** argv);
    int handlePluginConfigure(int argc, char** argv);
    
    std::shared_ptr<class Config> config_;
};

} // namespace nlm
