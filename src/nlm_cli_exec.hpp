#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Config;
class Brain;
class SimpleWorld;
class AgentBrain;
class SensoryPercept;
class MotorCommand;
class RegionId;

class NLM_CLI_EXEC {
public:
    NLM_CLI_EXEC();
    ~NLM_CLI_EXEC();
    
    // Command execution
    bool executeCommand(const std::string& cmd, const std::vector<std::string>& args);
    
    // Command handlers
    bool createBrain(const std::vector<std::string>& args);
    bool runSimulation(const std::vector<std::string>& args);
    bool showStatus();
    bool saveBrain(const std::vector<std::string>& args);
    bool loadBrain(const std::vector<std::string>& args);
    bool exportData(const std::vector<std::string>& args);
    
    // Helper functions
    void showHelp();
    void showVersion();
    void cleanup();
    
private:
    std::shared_ptr<Brain> currentBrain_;
    std::string configFile_;
    uint64_t currentStep_;
};

} // namespace nlm
