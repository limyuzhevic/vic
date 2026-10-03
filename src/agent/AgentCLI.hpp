#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

enum class CommandType {
    HELP,
    STATUS,
    RESET,
    CONFIG,
    INTERACTIVE,
    ENABLE_REWARD_MODULATION,
    ENABLE_STRUCTURAL_PLASTICITY,
    ENABLE_DEVELOPMENT,
    ENABLE_CURIOSITY,
    GET_NEUROMODULATION_LEVEL,
    GET_CURIOUSITY_LEVEL,
    GET_NOVELTY_LEVEL,
    GET_PREDICTION_ERROR,
    GET_DEVELOPMENTAL_STAGE,
    SAVE_STATE,
    LOAD_STATE,
    EXPORT_METRICS,
    COMPARE_STATES,
    RUN_EXPERIMENT,
    TRAIN_AGENT,
    LIST_CONFIGS,
    CREATE_CONFIG,
    VALIDATE_CONFIG,
    SET_LOG_LEVEL,
    ENABLE_LOGGING,
    DISABLE_LOGGING,
    BENCHMARK,
    PROFILE
};

struct Command {
    CommandType type;
    std::string name;
    std::string description;
    std::vector<std::string> args;
    std::string argHelp;
};

class AgentCLI {
public:
    AgentCLI(std::shared_ptr<AgentBrain> agentBrain, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~AgentCLI();
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Execute the parsed command
    int executeCommand();
    
    // Show help
    void showHelp() const;
    
    // Show status
    void showStatus() const;
    
    // Set configuration
    bool setConfiguration(const std::string& filepath);
    
    // Interactive mode
    void runInteractiveMode();
    
    // System control
    bool enableRewardModulation(bool enable);
    bool enableStructuralPlasticity(bool enable);
    bool enableDevelopment(bool enable);
    bool enableCuriosity(bool enable);
    
    // State inspection
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    std::string getDevelopmentalStage() const;
    
    // State management
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    bool exportMetrics(const std::string& filepath) const;
    bool compareStates(const std::string& filepath1, const std::string& filepath2) const;
    
    // Batch processing
    bool runExperiment(size_t steps);
    bool trainAgent(size_t episodes, size_t maxSteps);
    
    // Configuration management
    std::vector<std::string> listConfigs() const;
    bool createConfig(const std::string& name);
    bool validateConfig(const std::string& filepath) const;
    
    // Debug and monitoring
    bool setLogLevel(const std::string& level);
    bool enableFileLogging(const std::string& filepath);
    bool disableFileLogging();
    
    // Performance
    bool runBenchmark(size_t neurons, size_t steps);
    void profile();
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    
    // Parsed command data
    CommandType commandType_;
    std::string commandArg1_;
    std::string commandArg2_;
    std::string commandArg3_;
    
    // Available commands
    static std::vector<Command> getAvailableCommands();
    
    // Command parsing helpers
    bool parseCommand(const std::string& commandStr);
    bool parseFlags(int argc, char** argv);
    
    // Validation helpers
    bool validateArguments(const Command& cmd) const;
    bool parseBoolean(const std::string& str, bool& value) const;
    
    // Logging helpers
    void logCommand(const Command& cmd) const;
    
    // Error handling
    void printError(const std::string& message) const;
    
    // Help text generation
    static std::string getCommandHelp(const Command& cmd);
};

} // namespace nlm
