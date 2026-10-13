// NLM Command Line Interface (nlmc)
// Advanced debugging, analysis, and control commands for NLM users

#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <limits>
#include <sstream>
#include <fstream>

// Forward declarations for NLM namespace
namespace nlm {
    class Config;
    class Brain;
    class RegionId;
    class NeuronId;
    class SynapseHandle;
    class NeuronType;
    class FiringState;
    class SynapseType;
}

namespace nlmc {

struct CommandResult {
    bool success;
    std::string output;
    std::string error;
    
    CommandResult(bool s = true, const std::string& out = "", const std::string& err = "")
        : success(s), output(out), error(err) {}
};

using CommandHandler = std::function<CommandResult(const std::vector<std::string>& args, const std::shared_ptr<nlm::Config>& config, const std::shared_ptr<nlm::Brain>& brain)>;

class NLMCommandLine {
private:
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
    std::map<std::string, CommandHandler> commands;
    
public:
    NLMCommandLine(const std::shared_ptr<nlm::Config>& cfg, const std::shared_ptr<nlm::Brain>& br) 
        : config(cfg), brain(br) {
        initializeCommands();
    }
    
    void initializeCommands();
    
    // Command execution methods
    CommandResult debugNeurons(const std::vector<std::string>& args);
    CommandResult debugSynapses(const std::vector<std::string>& args);
    CommandResult debugRegion(const std::vector<std::string>& args);
    CommandResult analyzeStats(const std::vector<std::string>& args);
    CommandResult analyzeLearningCurve(const std::vector<std::string>& args);
    CommandResult analyzeNetwork(const std::vector<std::string>& args);
    CommandResult saveState(const std::vector<std::string>& args);
    CommandResult loadState(const std::vector<std::string>& args);
    CommandResult resetBrain(const std::vector<std::string>& args);
    CommandResult configCommand(const std::vector<std::string>& args);
    CommandResult configSet(const std::vector<std::string>& args);
    CommandResult runExperiment(const std::vector<std::string>& args);
    CommandResult exportNeurons(const std::vector<std::string>& args);
    CommandResult exportSynapses(const std::vector<std::string>& args);
    CommandResult generateHeatmap(const std::vector<std::string>& args);
    CommandResult showHelp(const std::vector<std::string>& args);
    CommandResult showVersion(const std::vector<std::string>& args);
    
public:
    void processCommandLine(int argc, char** argv);
};

} // namespace nlmc

// Global CLI instance
extern nlmc::NLMCommandLine* g_cli;

void initializeCLI(const std::shared_ptr<nlm::Config>& config, const std::shared_ptr<nlm::Brain>& brain);
void processCommandLine(int argc, char** argv);
void cleanupCLI();