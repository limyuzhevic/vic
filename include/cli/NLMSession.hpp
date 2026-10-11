#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "PluginManager.hpp"
#include "Core/Types/Types.hpp"
#include "Core/Config/Config.hpp"
#include "Core/Logger/Logger.hpp"

using json = nlohmann::json;

namespace nlm {
namespace cli {

class CommandLineParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<bool(const std::vector<std::string>&)> callback;
        std::vector<std::string> flags;
        std::vector<std::pair<std::string, std::string>> options;
        bool requiresBrain = false;
        bool requiresSession = false;
    };
    
    CommandLineParser(std::shared_ptr<PluginManager> pluginManager,
                    std::shared_ptr<Config> config,
                    std::shared_ptr<Logger> logger);
    
    bool parse(int argc, char** argv, std::shared_ptr<NLMSession> session = nullptr);
    
    void printHelp() const;
    void printVersion() const;
    
    void registerCommand(const Command& cmd);
    bool executeCommand(const std::string& cmdName, 
                       const std::vector<std::string>& args,
                       std::shared_ptr<NLMSession> session = nullptr);
    
    std::string autoComplete(const std::string& partial) const;
    
    void setInteractiveMode(bool enabled) { interactiveMode_ = enabled; }
    bool isInteractiveMode() const { return interactiveMode_; }
    
    void logCommand(const std::string& command) const;
    
    void logStatus() const;
    
    static bool validateArguments(const Command& cmd, const std::vector<std::string>& args);
    
private:
    std::unordered_map<std::string, Command> commands_;
    std::shared_ptr<PluginManager> pluginManager_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    bool interactiveMode_ = true;
    std::vector<std::string> commandHistory_;
};

class NLMSession {
public:
    NLMSession(std::shared_ptr<Config> config,
              std::shared_ptr<Logger> logger,
              std::shared_ptr<CommandLineParser> parser);
    ~NLMSession();
    
    bool initialize(std::shared_ptr<Brain> brain = nullptr);
    void shutdown();
    
    bool runCommand(const std::string& commandLine);
    bool runBatchFile(const std::string& filename);
    
    void saveBrainState(const std::string& filename) const;
    bool loadBrainState(const std::string& filename);
    void resetBrainState();
    
    void runExperiment(const std::string& experimentName, 
                      const std::vector<std::string>& args);
    
    void setPerformanceMonitoring(bool enabled) { performanceMonitoringEnabled_ = enabled; }
    bool isPerformanceMonitoringEnabled() const { return performanceMonitoringEnabled_; }
    
    void startProfiling() { profilingStartTime_ = std::chrono::high_resolution_clock::now(); }
    void stopProfiling();
    double getProfilingTime() const;
    
    void logSessionStatus() const;
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    const std::string& getSessionId() const { return sessionId_; }
    
    void setPluginManager(std::shared_ptr<PluginManager> manager) { pluginManager_ = manager; }
    std::shared_ptr<PluginManager> getPluginManager() const { return pluginManager_; }
    
private:
    struct Impl;
    Impl* pImpl;
    std::string sessionId_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<CommandLineParser> parser_;
    std::shared_ptr<PluginManager> pluginManager_;
    std::shared_ptr<Brain> brain_;
    bool performanceMonitoringEnabled_ = false;
    std::chrono::high_resolution_clock::time_point profilingStartTime_;
    double profilingDuration_ = 0.0;
};

} // namespace cli
} // namespace nlm
