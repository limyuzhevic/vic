// NLM Command Line Interface (CLI) - Header
// Advanced command-line interface for controlling NLM simulations
// Provides comprehensive control over brain initialization, simulation, and analysis

#pragma once

#include <string>
#include <vector>
#include <map>

namespace nlm {

// Forward declarations
class Brain;
class SimpleWorld;
class AgentBrain;
class Config;
class Phase6Config;
class SensoryPercept;
class MotorCommand;
class ActionResult;
class Action;

// CLI Command types
enum class CLICommandType {
    HELP,
    VERSION,
    INIT,
    CONFIG,
    RUN,
    SAVE,
    LOAD,
    STATUS,
    STATS,
    MONITOR,
    EXPERIMENT,
    TEMPLATE,
    PLUGIN,
    EXIT,
    UNKNOWN
};

// CLI Command structure
struct CLICommand {
    CLICommandType type;
    std::string description;
    std::vector<std::string> args;
};

// Phase 6 experiment configuration
struct Phase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin") {}
};

// Main CLI class
class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    // Main CLI loop
    void run();
    
    // Static global instance
    static NLMCLI& getInstance();
    
    // Convenience function for simple CLI usage
    static void startNLM(const std::vector<std::string>& args);
    
private:
    // Command processing
    void setupCommands();
    void setupTemplates();
    
    CLICommand parseCommand(const std::string& input);
    bool executeCommand(const CLICommand& cmd);
    
    // Command implementations
    void showHelp(const std::vector<std::string>& args);
    void showVersion();
    void initializeBrain(const std::vector<std::string>& args);
    void configureBrain(const std::vector<std::string>& args);
    void runSimulation(const std::vector<std::string>& args);
    void saveState(const std::vector<std::string>& args);
    void loadState(const std::vector<std::string>& args);
    void showStatus();
    void showStats();
    void startMonitoring(const std::vector<std::string>& args);
    void runExperiment(const std::vector<std::string>& args);
    void manageTemplate(const std::vector<std::string>& args);
    void managePlugins(const std::vector<std::string>& args);
    
    // Helper methods
    std::string getCommandName(CLICommandType type);
    std::string getTemplateList();
    std::string getTemplateDescription(const std::string& name);
    
    // Template configurations
    Phase6Config getDemoTemplate();
    Phase6Config getResearchTemplate();
    Phase6Config getFastTestTemplate();
    Phase6Config getFullIntegrationTemplate();
    Phase6Config getCuriosityTemplate();
    Phase6Config getDevelopmentTemplate();
    
    // Experiment runners
    void runPhase6Experiment(uint64_t steps, const std::string& configFile);
    void runMemoryExperiment(uint64_t steps, const std::string& configFile);
    void runNeuromodulationExperiment(uint64_t steps, const std::string& configFile);
    void runPredictionExperiment(uint64_t steps, const std::string& configFile);
    void runCognitionExperiment(uint64_t steps, const std::string& configFile);
    void runDevelopmentExperiment(uint64_t steps, const std::string& configFile);
    
    // Utility methods
    void printWelcomeBanner();
    void cleanup();
    
    // State management
    void showTemplate(const std::string& name);
    void createTemplate(const std::string& name, const std::string& baseTemplate);
    void deleteTemplate(const std::string& name);
    
    // Display methods
    void showTemplate(const std::string& name);
    
public:
    // Getter methods
    bool isMonitoring() const { return isMonitoring_; }
    bool isSimulationRunning() const { return simulationRunning_; }
    
private:
    // Current system state
    std::shared_ptr<Brain> currentBrain_;
    std::unique_ptr<SimpleWorld> currentWorld_;
    std::unique_ptr<AgentBrain> currentAgent_;
    
    // CLI state
    bool isMonitoring_;
    bool simulationRunning_;
    
    // Command and template definitions
    std::map<CLICommandType, std::string> commandDescriptions_;
    std::map<std::string, Phase6Config> templates_;
};

} // namespace nlm