#ifndef NLM_CLI_APPLICATION_HPP
#define NLM_CLI_APPLICATION_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Experiment.hpp"

using namespace nlm;

namespace nlm_cli {

// CLI Command Handler Interface
class CLICommand {
public:
    virtual ~CLICommand() = default;
    virtual bool execute(const std::map<std::string, std::string>& args) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void printHelp() const = 0;
};

// Main NLM CLI Application
class NLMCLIApplication {
private:
    std::map<std::string, std::unique_ptr<CLICommand>> commands_;
    std::string config_file_;
    std::string output_dir_;
    bool quiet_mode_;
    bool verbose_mode_;
    
public:
    NLMCLIApplication();
    ~NLMCLIApplication() = default;
    
    void initializeCommands();
    bool parseArguments(int argc, char** argv);
    int executeCommand();
    void printCommandHelp(const std::string& command_name);
    void printGeneralHelp() const;
    
private:
    void registerCommands();
    bool handleRunCommand(const std::map<std::string, std::string>& args);
    bool handleExperimentCommand(const std::map<std::string, std::string>& args);
    bool handleBenchmarkCommand(const std::map<std::string, std::string>& args);
    bool handleConfigCommand(const std::map<std::string, std::string>& args);
    bool handleVisualizeCommand(const std::map<std::string, std::string>& args);
    bool handleAnalyzeCommand(const std::map<std::string, std::string>& args);
    void printBanner(const std::string& title) const;
    std::string getTimestamp() const;
};

} // namespace nlm_cli

#endif // NLM_CLI_APPLICATION_HPP
