#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    // Disable copying, enable moving
    CommandLineInterface(const CommandLineInterface&) = delete;
    CommandLineInterface& operator=(const CommandLineInterface&) = delete;
    CommandLineInterface(CommandLineInterface&&) noexcept;
    CommandLineInterface& operator=(CommandLineInterface&&) noexcept;
    
    // Main CLI entry point
    int run(int argc, char** argv);
    
    // Print help information
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
    // Cleanup
    void cleanup();
    
    // Static helper methods for experiments
    static void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    static void runPlasticityLearningExperiment(std::shared_ptr<Brain> brain);
    static void runStdpVerificationTest(std::shared_ptr<Brain> brain);
    static void runDevelopmentalStages(std::shared_ptr<Brain> brain);
    static void runCuriosityExploration(std::shared_ptr<Brain> brain);
    static void runRewardLearning(std::shared_ptr<Brain> brain);
    
    // Get singleton instance (for testing/utilities)
    static CommandLineInterface& getInstance() {
        static CommandLineInterface instance;
        return instance;
    }
    
private:
    // Command processing
    bool parseArguments(int argc, char** argv);
    int processCommands();
    int processCommand();
    
    // Configuration handling
    void setupDefaultConfig();
    bool loadConfigurationFiles();
    void applyConfigurationOverrides();
    void applyConfigOverride(const std::string& key, const std::string& value);
    void applyConfigAdd(const std::string& key, const std::string& value);
    int validateConfiguration();
    void showConfigurationSummary();
    
    // Command implementations
    int commandInit();
    int commandReset();
    int commandStart();
    int commandStop();
    int commandStep(const std::string& steps);
    int commandStatus();
    int commandSave(const std::string& filepath);
    int commandLoad(const std::string& filepath);
    int commandExperiment(const std::string& experimentName);
    int commandExplore();
    int commandExploit();
    int commandMonitor();
    int commandStats();
    int commandSpikes();
    int commandPlasticity();
    int commandPlugins();
    
    // Helper methods
    Environment* createEnvironment();
    int runExperiment(const std::string& experimentName);
    
    // State display
    void showStatus();
    void showStatistics();
    void showSpikeStatistics();
    void showPlasticityStatistics();
    
    // Logging helpers
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    void logSuccess(const std::string& message);
    
    // Type checking helpers
    bool isInteger(const std::string& str) const;
    bool isFloat(const std::string& str) const;
    bool isBoolean(const std::string& str) const;
    
    // Internal state
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    Environment* environment_;
    
    // Command line state
    std::string currentCommand_;
    std::string commandArgs_;
    bool showHelp_ = false;
    bool showVersion_ = false;
    bool verbose_ = false;
    bool quiet_ = false;
    bool validateOnly_ = false;
    bool showConfigSummary_ = false;
    bool force_ = false;
    
    // Configuration files
    std::string configFile_;
    std::string jsonConfigFile_;
    std::string yamlConfigFile_;
    std::string exportConfigFile_;
    std::string importConfigFile_;
    
    // Configuration overrides
    std::map<std::string, std::string> configOverrides_;
    std::map<std::string, std::string> configAdds_;
    std::vector<std::string> configRemoves_;
    
    // Mode settings
    bool explorationEnabled_ = false;
    bool exploitationEnabled_ = false;
    bool simulationRunning_ = false;
    bool monitoringActive_ = false;
    bool error_ = false;
};

} // namespace nlm
