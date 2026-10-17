#pragma once

#include "../agent/EnhancedAgentBrain.hpp"
#include "../world/EnhancedSimpleWorld.hpp"
#include "../core/Config/ConfigManager.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>

namespace nlm {

// Command-line interface for NLM
class NLMCommandLine {
public:
    NLMCommandLine() : configManager_(std::make_shared<ConfigManager>("NLM_CLI")) {
        initializeDefaultCommands();
    }
    
    ~NLMCommandLine() = default;
    
    // Main command processing
    bool execute(const std::vector<std::string>& args);
    
    // Command registration
    void registerCommand(const std::string& name, 
                         const std::string& description,
                         std::function<bool(const std::vector<std::string>&)> handler,
                         const std::vector<std::string>& usage = {});
    
    // Help and usage
    void printHelp() const;
    void printCommandHelp(const std::string& commandName) const;
    
    // Configuration
    bool loadConfig(const std::string& filepath);
    bool saveConfig(const std::string& filepath);
    void printConfig() const;
    void setConfig(const std::string& key, const std::string& value);
    
    // Session management
    void startSession();
    void endSession();
    bool isSessionActive() const { return sessionActive_; }
    
    // Output and logging
    void setOutputFile(const std::string& filename);
    void enableVerbose(bool enable) { verboseEnabled_ = enable; }
    bool isVerboseEnabled() const { return verboseEnabled_; }
    
    // Advanced features
    void enableAutoSave(bool enable) { autoSaveEnabled_ = enable; }
    void setAutoSaveInterval(double interval) { autoSaveInterval_ = interval; }
    
    // Debug and diagnostics
    void printSystemInfo() const;
    void printPerformanceStats() const;
    void printMemoryInfo() const;
    
    // Command completion and history
    std::vector<std::string> getCommandHistory() const { return commandHistory_; }
    void clearHistory() { commandHistory_.clear(); }
    
private:
    struct Impl;
    Impl* pImpl_;
};

// Session management for NLM
class NLMSession {
public:
    NLMSession(std::shared_ptr<EnhancedAgentBrain> agentBrain,
               std::shared_ptr<EnhancedSimpleWorld> world,
               std::shared_ptr<ConfigManager> config);
    ~NLMSession();
    
    // Session lifecycle
    bool start(double timestep = 0.01);
    bool update();
    void stop();
    bool isRunning() const { return running_; }
    
    // Session control
    void pause() { paused_ = true; }
    void resume() { paused_ = false; }
    bool isPaused() const { return paused_; }
    
    // Session data
    double getSimulationTime() const { return simTime_; }
    size_t getStepCount() const { return stepCount_; }
    size_t getTotalActions() const { return totalActions_; }
    
    // Agent control
    void resetAgent() { if (agentBrain_) agentBrain_->reset(); }
    void applyReward(float reward, float predictedReward = 0.0f) {
        if (agentBrain_) agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    // World control
    void resetWorld() { if (world_) world_->reset(); }
    void setAgentPosition(float x, float y) { if (world_) world_->setAgentStart(x, y); }
    
    // Configuration
    void setConfig(const std::string& key, const std::string& value) {
        if (configManager_) configManager_->set(key, value);
    }
    
    // Data recording
    void enableRecording(bool enable) { recordingEnabled_ = enable; }
    bool isRecordingEnabled() const { return recordingEnabled_; }
    void saveSession(const std::string& filepath);
    void loadSession(const std::string& filepath);
    
    // Event handling
    void setStepCallback(std::function<void(size_t)> callback) { stepCallback_ = callback; }
    void setActionCallback(std::function<void(const ActionResult&)> callback) { actionCallback_ = callback; }
    
    // Advanced features
    void enableLogging(bool enable) { if (agentBrain_) agentBrain_->enableDebugLogging(enable); }
    void enableProfiling(bool enable) { profilingEnabled_ = enable; }
    
    // Statistics
    std::string getSessionStats() const;
    std::string getPerformanceReport() const;
    
private:
    struct Impl;
    Impl* pImpl_;
};

// NLM command processors
class NLMCommandProcessor {
public:
    static bool processHelp(const std::vector<std::string>& args);
    static bool processVersion(const std::vector<std::string>& args);
    static bool processInfo(const std::vector<std::string>& args);
    static bool processConfig(const std::vector<std::string>& args);
    static bool processSession(const std::vector<std::string>& args);
    static bool processWorld(const std::vector<std::string>& args);
    static bool processAgent(const std::vector<std::string>& args);
    static bool processExperiment(const std::vector<std::string>& args);
    static bool processDebug(const std::vector<std::string>& args);
    static bool processPerformance(const std::vector<std::string>& args);
    static bool processSave(const std::vector<std::string>& args);
    static bool processLoad(const std::vector<std::string>& args);
    static bool processReset(const std::vector<std::string>& args);
    static bool processExit(const std::vector<std::string>& args);
    static bool processQuit(const std::vector<std::string>& args);
    static bool processRun(const std::vector<std::string>& args);
    static bool processStop(const std::vector<std::string>& args);
    static bool processStatus(const std::vector<std::string>& args);
    static bool processRecord(const std::vector<std::string>& args);
    static bool processReplay(const std::vector<std::string>& args);
    
    static std::string getVersionString();
    static std::string getBuildInfo();
    static std::string getSystemInfo();
};

// Advanced NLM shell
class NLMShell {
public:
    NLMShell() : commandLine_(std::make_shared<NLMCommandLine>()), 
                session_(nullptr), interactiveMode_(true) {
        initializeShell();
    }
    
    ~NLMShell() = default;
    
    // Shell execution
    bool run();
    void stop() { running_ = false; }
    bool isRunning() const { return running_; }
    
    // Shell setup
    void initializeShell();
    void setupCommands();
    void setupAutoComplete();
    
    // Interactive features
    void enableInteractiveMode(bool enable) { interactiveMode_ = enable; }
    bool isInteractiveMode() const { return interactiveMode_; }
    
    // Command history and completion
    void addCommandToHistory(const std::string& command) {
        commandLine_->getCommandHistory().push_back(command);
    }
    
    // Session management
    bool startSession();
    bool endSession();
    bool isSessionActive() const { return session_ && session_->isRunning(); }
    
    // Configuration
    void loadConfigFile(const std::string& filepath) {
        commandLine_->loadConfig(filepath);
    }
    
    void saveConfigFile(const std::string& filepath) {
        commandLine_->saveConfig(filepath);
    }
    
    // Shell state
    std::string getPrompt() const {
        if (session_ && session_->isRunning()) {
            return "[NLM-SESSION] ";
        } else {
            return "[NLM] ";
        }
    }
    
private:
    std::shared_ptr<NLMCommandLine> commandLine_;
    std::shared_ptr<NLMSession> session_;
    bool interactiveMode_;
    bool running_;
    std::string currentWorkingDirectory_;
    
    // Shell utilities
    std::string getCurrentDirectory() const;
    std::vector<std::string> tokenizeCommand(const std::string& command) const;
    std::string expandCommand(const std::string& command) const;
    void executeCommand(const std::string& command);
};

} // namespace nlm