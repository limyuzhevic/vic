// NLM (熙然) - Neural Learning Machine
// Advanced Command Line Interface
//
// This provides enhanced command-line interface for controlling the NLM brain,
// including debugging tools, configuration management, and advanced commands.

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    bool initialize(int argc, char** argv);
    void run();
    void stop() { running = false; }
    
    void printHelp() const;
    void printBrainStatus() const;
    void printConfig() const;
    void printMemoryStats() const;
    void printPerformanceStats() const;
    
    // Command handlers
    void handleHelp(const std::vector<std::string>& args);
    void handleLoad(const std::vector<std::string>& args);
    void handleSave(const std::vector<std::string>& args);
    void handleStep(const std::vector<std::string>& args);
    void handleRun(const std::vector<std::string>& args);
    void handleReset(const std::vector<std::string>& args);
    void handleStatus(const std::vector<std::string>& args);
    void handleInject(const std::vector<std::string>& args);
    void handleCheckpoint(const std::vector<std::string>& args);
    void handleDebug(const std::vector<std::string>& args);
    void handleStats(const std::vector<std::string>& args);
    void handleConfig(const std::vector<std::string>& args);
    void handleWorld(const std::vector<std::string>& args);
    void handleSensory(const std::vector<std::string>& args);
    void handleExit(const std::vector<std::string>& args);
    void handleQuit(const std::vector<std::string>& args);
    
    void executeCommand(const std::string& cmdLine);
    
private:
    static void signalHandler(int signal);
    static NLMCLI* instance;
    
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<SimpleWorld> world;
    bool running;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> commandHandlers;
};

} // namespace nlm