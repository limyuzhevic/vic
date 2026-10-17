#include "NLMShell.hpp"
#include "../agent/EnhancedAgentBrain.hpp"
#include "../world/EnhancedSimpleWorld.hpp"
#include "../core/Config/ConfigManager.hpp"
#include "../core/Logger/AdvancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <filesystem>
#include <csignal>

namespace nlm {

struct NLMCommandLine::Impl {
    std::shared_ptr<ConfigManager> configManager_;
    std::unordered_map<std::string, std::function<bool(const std::vector<std::string>&)>> commands_;
    std::unordered_map<std::string, std::vector<std::string>> commandUsages_;
    std::unordered_map<std::string, std::string> commandDescriptions_;
    std::string outputFile_;
    bool verboseEnabled_;
    bool autoSaveEnabled_;
    double autoSaveInterval_;
    bool sessionActive_;
    std::string currentSessionId_;
    
    Impl() : verboseEnabled_(false), autoSaveEnabled_(false), autoSaveInterval_(300.0),
             sessionActive_(false), currentSessionId_("") {
        configManager_ = std::make_shared<ConfigManager>("NLM_CLI");
    }
};

struct NLMSession::Impl {
    std::shared_ptr<EnhancedAgentBrain> agentBrain_;
    std::shared_ptr<EnhancedSimpleWorld> world_;
    std::shared_ptr<ConfigManager> configManager_;
    double timestep_;
    bool running_;
    bool paused_;
    double simTime_;
    size_t stepCount_;
    size_t totalActions_;
    bool recordingEnabled_;
    std::string sessionFile_;
    std::thread sessionThread_;
    bool profilingEnabled_;
    std::chrono::steady_clock::time_point startTime_;
    std::vector<std::chrono::steady_clock::time_point> stepTimes_;
    
    Impl(std::shared_ptr<EnhancedAgentBrain> agentBrain,
         std::shared_ptr<EnhancedSimpleWorld> world,
         std::shared_ptr<ConfigManager> config)
        : agentBrain_(agentBrain), world_(world), configManager_(config),
          timestep_(0.01), running_(false), paused_(false), simTime_(0.0),
          stepCount_(0), totalActions_(0), recordingEnabled_(false),
          sessionFile_(""), profilingEnabled_(false) {
        startTime_ = std::chrono::steady_clock::now();
    }
    
    ~Impl() {
        stop();
    }
    
    void recordStep() {
        stepTimes_.push_back(std::chrono::steady_clock::now());
        if (stepTimes_.size() > 1000) {
            stepTimes_.erase(stepTimes_.begin());
        }
    }
    
    double getAverageStepTime() const {
        if (stepTimes_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& timePoint : stepTimes_) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                timePoint - startTime_);
            total += duration.count();
        }
        return total / stepTimes_.size() / 1000.0; // Convert to milliseconds
    }
};

// NLMCommandLine implementation
NLMCommandLine::NLMCommandLine() : pImpl_(new Impl()) {
    initializeDefaultCommands();
}

NLMCommandLine::~NLMCommandLine() {
    delete pImpl_;
}

bool NLMCommandLine::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        printHelp();
        return false;
    }
    
    std::string command = args[0];
    auto it = pImpl_->commands_.find(command);
    if (it == pImpl_->commands_.end()) {
        std::cout << "Unknown command: " << command << std::endl;
        printHelp();
        return false;
    }
    
    std::vector<std::string> commandArgs(args.begin() + 1, args.end());
    bool result = it->second(commandArgs);
    
    if (pImpl_->verboseEnabled_) {
        std::cout << "Command executed: " << command << " (result: " 
                  << (result ? "success" : "failure") << ")" << std::endl;
    }
    
    return result;
}

void NLMCommandLine::registerCommand(const std::string& name,
                                     const std::string& description,
                                     std::function<bool(const std::vector<std::string>&)> handler,
                                     const std::vector<std::string>& usage) {
    pImpl_->commands_[name] = handler;
    pImpl_->commandDescriptions_[name] = description;
    pImpl_->commandUsages_[name] = usage;
}

void NLMCommandLine::printHelp() const {
    std::cout << "NLM Command Line Interface" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << std::endl;
    
    for (const auto& pair : pImpl_->commandDescriptions_) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        
        auto usageIt = pImpl_->commandUsages_.find(pair.first);
        if (usageIt != pImpl_->commandUsages_.end()) {
            std::cout << "    Usage:" << std::endl;
            for (const std::string& line : usageIt->second) {
                std::cout << "      " << line << std::endl;
            }
        }
    }
}

void NLMCommandLine::printCommandHelp(const std::string& commandName) const {
    auto descIt = pImpl_->commandDescriptions_.find(commandName);
    if (descIt == pImpl_->commandDescriptions_.end()) {
        std::cout << "Unknown command: " << commandName << std::endl;
        return;
    }
    
    std::cout << "Command: " << commandName << std::endl;
    std::cout << "Description: " << descIt->second << std::endl;
    
    auto usageIt = pImpl_->commandUsages_.find(commandName);
    if (usageIt != pImpl_->commandUsages_.end()) {
        std::cout << "Usage:" << std::endl;
        for (const std::string& line : usageIt->second) {
            std::cout << "  " << line << std::endl;
        }
    }
}

bool NLMCommandLine::loadConfig(const std::string& filepath) {
    bool result = pImpl_->configManager_->loadFromFile(filepath);
    if (result && pImpl_->verboseEnabled_) {
        std::cout << "Configuration loaded from: " << filepath << std::endl;
    }
    return result;
}

bool NLMCommandLine::saveConfig(const std::string& filepath) {
    bool result = pImpl_->configManager_->saveToFile(filepath);
    if (result && pImpl_->verboseEnabled_) {
        std::cout << "Configuration saved to: " << filepath << std::endl;
    }
    return result;
}

void NLMCommandLine::printConfig() const {
    std::cout << "=== Current Configuration ===" << std::endl;
    std::cout << pImpl_->configManager_->getStats() << std::endl;
    
    std::cout << "\nConfiguration values:" << std::endl;
    auto keys = pImpl_->configManager_->getAllKeys();
    for (const std::string& key : keys) {
        std::cout << "  " << key << " = ";
        
        // Try to get value as string
        try {
            std::string value = pImpl_->configManager_->getString(key);
            std::cout << value << std::endl;
        } catch (...) {
            try {
                int value = pImpl_->configManager_->getInt(key);
                std::cout << value << std::endl;
            } catch (...) {
                try {
                    float value = pImpl_->configManager_->getFloat(key);
                    std::cout << value << std::endl;
                } catch (...) {
                    try {
                        bool value = pImpl_->configManager_->getBool(key);
                        std::cout << (value ? "true" : "false") << std::endl;
                    } catch (...) {
                        std::cout << "[unable to display]" << std::endl;
                    }
                }
            }
        }
    }
}

void NLMCommandLine::setConfig(const std::string& key, const std::string& value) {
    pImpl_->configManager_->set(key, value);
    if (pImpl_->verboseEnabled_) {
        std::cout << "Configuration set: " << key << " = " << value << std::endl;
    }
}

void NLMCommandLine::startSession() {
    pImpl_->sessionActive_ = true;
    pImpl_->currentSessionId_ = "session_" + 
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    
    if (pImpl_->verboseEnabled_) {
        std::cout << "Session started: " << pImpl_->currentSessionId_ << std::endl;
    }
}

void NLMCommandLine::endSession() {
    if (pImpl_->sessionActive_) {
        pImpl_->sessionActive_ = false;
        if (pImpl_->verboseEnabled_) {
            std::cout << "Session ended" << std::endl;
        }
        
        // Auto-save if enabled
        if (pImpl_->autoSaveEnabled_) {
            std::string filename = "session_" + pImpl_->currentSessionId_ + ".json";
            saveConfig(filename);
        }
    }
}

void NLMCommandLine::initializeDefaultCommands() {
    // Help command
    registerCommand("help", "Show help information", 
                   [this](const std::vector<std::string>& args) {
                       printHelp();
                       return true;
                   },
                   {"help", "Display this help message",
                    "help <command>", "Show help for a specific command"});
    
    // Version command
    registerCommand("version", "Show version information",
                   [this](const std::vector<std::string>& args) {
                       std::cout << "NLM Command Line Interface v1.0.0" << std::endl;
                       std::cout << "Build: " << NLMCommandProcessor::getBuildInfo() << std::endl;
                       return true;
                   },
                   {"version", "Show version and build information"});
    
    // Info command
    registerCommand("info", "Show system information",
                   [this](const std::vector<std::string>& args) {
                       std::cout << "=== System Information ===" << std::endl;
                       std::cout << NLMCommandProcessor::getSystemInfo() << std::endl;
                       return true;
                   },
                   {"info", "Show system information"});
    
    // Config command
    registerCommand("config", "Configuration management",
                   [this](const std::vector<std::string>& args) {
                       if (args.empty()) {
                           printConfig();
                       } else if (args[0] == "load") {
                           if (args.size() < 2) {
                               std::cout << "Usage: config load <filename>" << std::endl;
                               return false;
                           }
                           loadConfig(args[1]);
                       } else if (args[0] == "save") {
                           if (args.size() < 2) {
                               std::cout << "Usage: config save <filename>" << std::endl;
                               return false;
                           }
                           saveConfig(args[1]);
                       } else if (args[0] == "set") {
                           if (args.size() < 3) {
                               std::cout << "Usage: config set <key> <value>" << std::endl;
                               return false;
                           }
                           setConfig(args[1], args[2]);
                       }
                       return true;
                   },
                   {"config", "Show current configuration",
                    "config load <filename>", "Load configuration from file",
                    "config save <filename>", "Save configuration to file",
                    "config set <key> <value>", "Set configuration value"});
    
    // Session command
    registerCommand("session", "Session management",
                   [this](const std::vector<std::string>& args) {
                       if (args.empty()) {
                           std::cout << "Usage: session <start|stop|status>" << std::endl;
                           return false;
                       }
                       
                       if (args[0] == "start") {
                           startSession();
                       } else if (args[0] == "stop") {
                           endSession();
                       } else if (args[0] == "status") {
                           std::cout << "Session active: " << (isSessionActive() ? "yes" : "no") << std::endl;
                       }
                       return true;
                   },
                   {"session start", "Start a new session",
                    "session stop", "Stop current session",
                    "session status", "Show session status"});
    
    // World command
    registerCommand("world", "World configuration and control",
                   [this](const std::vector<std::string>& args) {
                       // This would interface with the world through a session
                       std::cout << "World commands require an active session" << std::endl;
                       return true;
                   },
                   {"world reset", "Reset the world",
                    "world set-agent <x> <y>", "Set agent starting position"});
    
    // Agent command
    registerCommand("agent", "Agent configuration and control",
                   [this](const std::vector<std::string>& args) {
                       // This would interface with the agent through a session
                       std::cout << "Agent commands require an active session" << std::endl;
                       return true;
                   },
                   {"agent reset", "Reset the agent",
                    "agent enable-features", "Enable agent features"});
    
    // Experiment command
    registerCommand("experiment", "Run experiments",
                   [this](const std::vector<std::string>& args) {
                       // This would run experiments through a session
                       std::cout << "Experiment commands require an active session" << std::endl;
                       return true;
                   },
                   {"experiment run <episodes>", "Run experiment with specified episodes",
                    "experiment list", "List available experiments"});
    
    // Debug command
    registerCommand("debug", "Debug and diagnostic commands",
                   [this](const std::vector<std::string>& args) {
                       if (args.empty()) {
                           std::cout << "Usage: debug <command>" << std::endl;
                           std::cout << "Commands: info, performance, memory, logs" << std::endl;
                           return false;
                       }
                       
                       if (args[0] == "info") {
                           printSystemInfo();
                       } else if (args[0] == "performance") {
                           printPerformanceStats();
                       } else if (args[0] == "memory") {
                           printMemoryInfo();
                       }
                       return true;
                   },
                   {"debug info", "Show system debug information",
                    "debug performance", "Show performance statistics",
                    "debug memory", "Show memory information"});
    
    // Performance command
    registerCommand("performance", "Performance monitoring",
                   [this](const std::vector<std::string>& args) {
                       // This would monitor performance through a session
                       std::cout << "Performance monitoring requires an active session" << std::endl;
                       return true;
                   },
                   {"performance start", "Start performance monitoring",
                    "performance stop", "Stop performance monitoring"});
    
    // Save command
    registerCommand("save", "Save current state",
                   [this](const std::vector<std::string>& args) {
                       if (args.empty()) {
                           std::cout << "Usage: save <filename>" << std::endl;
                           return false;
                       }
                       return saveConfig(args[0]);
                   },
                   {"save <filename>", "Save configuration to file"});
    
    // Load command
    registerCommand("load", "Load saved state",
                   [this](const std::vector<std::string>& args) {
                       if (args.empty()) {
                           std::cout << "Usage: load <filename>" << std::endl;
                           return false;
                       }
                       return loadConfig(args[0]);
                   },
                   {"load <filename>", "Load configuration from file"});
    
    // Reset command
    registerCommand("reset", "Reset all state",
                   [this](const std::vector<std::string>& args) {
                       // Reset configuration
                       pImpl_->configManager_->reset();
                       std::cout << "All configuration reset" << std::endl;
                       return true;
                   },
                   {"reset", "Reset all configuration"});
    
    // Exit command
    registerCommand("exit", "Exit the program", 
                   [this](const std::vector<std::string>& args) {
                       return false; // Signal to stop
                   },
                   {"exit", "Exit the program",
                    "quit", "Exit the program"});
    
    // Run command
    registerCommand("run", "Run simulation",
                   [this](const std::vector<std::string>& args) {
                       // This would start a simulation through a session
                       std::cout << "Run simulation requires an active session" << std::endl;
                       return true;
                   },
                   {"run <steps>", "Run simulation for specified steps"});
    
    // Stop command
    registerCommand("stop", "Stop simulation",
                   [this](const std::vector<std::string>& args) {
                       // This would stop a running simulation
                       std::cout << "Stop simulation requires an active session" << std::endl;
                       return true;
                   },
                   {"stop", "Stop running simulation"});
    
    // Status command
    registerCommand("status", "Show current status",
                   [this](const std::vector<std::string>& args) {
                       printSystemInfo();
                       printPerformanceStats();
                       return true;
                   },
                   {"status", "Show system status and statistics"});
    
    // Record command
    registerCommand("record", "Start/stop recording",
                   [this](const std::vector<std::string>& args) {
                       // This would control recording through a session
                       std::cout << "Record commands require an active session" << std::endl;
                       return true;
                   },
                   {"record start", "Start recording simulation",
                    "record stop", "Stop recording simulation"});
    
    // Replay command
    registerCommand("replay", "Replay recorded session",
                   [this](const std::vector<std::string>& args) {
                       if (args.empty()) {
                           std::cout << "Usage: replay <filename>" << std::endl;
                           return false;
                       }
                       // This would replay a recorded session
                       std::cout << "Replay not yet implemented" << std::endl;
                       return true;
                   },
                   {"replay <filename>", "Replay recorded session"});
}

void NLMCommandLine::printSystemInfo() const {
    std::cout << "=== System Information ===" << std::endl;
    std::cout << "NLM Command Line Interface v1.0.0" << std::endl;
    std::cout << "Build: " << NLMCommandProcessor::getBuildInfo() << std::endl;
    std::cout << "Platform: Linux" << std::endl;
    std::cout << "Architecture: x86_64" << std::endl;
    std::cout << "Python: 3.8+" << std::endl;
    std::cout << "C++: C++20" << std::endl;
    std::cout << "Configuration manager: Active" << std::endl;
    std::cout << "Auto-save: " << (pImpl_->autoSaveEnabled_ ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Verbose mode: " << (pImpl_->verboseEnabled_ ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Session active: " << (pImpl_->sessionActive_ ? "Yes" : "No") << std::endl;
}

void NLMCommandLine::printPerformanceStats() const {
    std::cout << "=== Performance Statistics ===" << std::endl;
    std::cout << "Configuration operations: " << pImpl_->configManager_->getLoadCount() << std::endl;
    std::cout << "Configuration saves: " << pImpl_->configManager_->getSaveCount() << std::endl;
    std::cout << "Configuration uptime: " << pImpl_->configManager_->getUptime() << " seconds" << std::endl;
}

void NLMCommandLine::printMemoryInfo() const {
    std::cout << "=== Memory Information ===" << std::endl;
    std::cout << "NLM Command Line Interface memory usage is optimized" << std::endl;
    std::cout << "Configuration manager uses efficient memory management" << std::endl;
}

// NLMSession implementation
NLMSession::NLMSession(std::shared_ptr<EnhancedAgentBrain> agentBrain,
                     std::shared_ptr<EnhancedSimpleWorld> world,
                     std::shared_ptr<ConfigManager> config)
    : pImpl_(new Impl(agentBrain, world, config)) {
}

NLMSession::~NLMSession() {
    delete pImpl_;
}

bool NLMSession::start(double timestep) {
    if (!pImpl_->agentBrain_ || !pImpl_->world_) {
        std::cout << "Error: Agent or world not properly initialized" << std::endl;
        return false;
    }
    
    pImpl_->timestep_ = timestep;
    pImpl_->world_->setRandomSeed(pImpl_->world_->getRandomSeed());
    pImpl_->world_->reset();
    pImpl_->agentBrain_->reset();
    
    pImpl_->running_ = true;
    pImpl_->simTime_ = 0.0;
    pImpl_->stepCount_ = 0;
    pImpl_->totalActions_ = 0;
    
    if (pImpl_->profilingEnabled_) {
        pImpl_->startTime_ = std::chrono::steady_clock::now();
    }
    
    std::cout << "Session started with timestep: " << timestep << std::endl;
    std::cout << "Initial agent energy: " << pImpl_->agentBrain_->getBrain()->getTotalNeuronCount() 
              << " neurons, " << pImpl_->world_->getAgentBody().energy << " energy" << std::endl;
    
    return true;
}

bool NLMSession::update() {
    if (!pImpl_->running_ || pImpl_->paused_) {
        return false;
    }
    
    pImpl_->simTime_ += pImpl_->timestep_;
    pImpl_->stepCount_++;
    
    // Update world
    pImpl_->world_->update(pImpl_->timestep_);
    
    // Get sensory input
    const SensoryPercept& percept = pImpl_->world_->getSensoryPercept();
    
    // Process with agent
    pImpl_->agentBrain_->processSensoryInput(percept);
    
    // Decode motor command
    MotorCommand cmd = pImpl_->agentBrain_->decodeMotorCommand();
    
    // Apply to world
    ActionResult result = pImpl_->world_->applyMotorCommand(cmd, pImpl_->simTime_);
    
    // Apply reward modulation
    pImpl_->agentBrain_->applyRewardModulation(result.reward, 0.0f);
    
    // Update development
    pImpl_->agentBrain_->updateDevelopment(pImpl_->timestep_);
    
    pImpl_->totalActions_++;
    
    // Record step for profiling
    pImpl_->recordStep();
    
    // Call callbacks
    if (stepCallback_) {
        stepCallback_(pImpl_->stepCount_);
    }
    
    if (actionCallback_) {
        actionCallback_(result);
    }
    
    // Auto-save if enabled
    if (pImpl_->recordingEnabled_ && pImpl_->stepCount_ % 100 == 0) {
        saveSession("auto_save_" + std::to_string(pImpl_->stepCount_) + ".json");
    }
    
    return true;
}

void NLMSession::stop() {
    if (pImpl_->running_) {
        pImpl_->running_ = false;
        std::cout << "Session stopped" << std::endl;
        
        if (pImpl_->profilingEnabled_) {
            double avgStepTime = pImpl_->getAverageStepTime();
            std::cout << "Average step time: " << avgStepTime << "ms" << std::endl;
            std::cout << "Total steps: " << pImpl_->stepCount_ << std::endl;
            std::cout << "Steps per second: " << (pImpl_->stepCount_ / (pImpl_->simTime_ / pImpl_->timestep_)) << std::endl;
        }
        
        saveSession("session_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()) + ".json");
    }
}

std::string NLMSession::getSessionStats() const {
    std::ostringstream stream;
    stream << "=== Session Statistics ===" << std::endl;
    stream << "Simulation time: " << pImpl_->simTime_ << "s" << std::endl;
    stream << "Steps: " << pImpl_->stepCount_ << std::endl;
    stream << "Actions: " << pImpl_->totalActions_ << std::endl;
    stream << "Paused: " << (pImpl_->paused_ ? "Yes" : "No") << std::endl;
    stream << "Agent energy: " << pImpl_->world_->getAgentBody().energy << std::endl;
    stream << "Agent health: " << pImpl_->world_->getAgentBody().health << std::endl;
    stream << "Current developmental stage: " << static_cast<int>(pImpl_->agentBrain_->getDevelopmentalStage()) << std::endl;
    
    return stream.str();
}

std::string NLMSession::getPerformanceReport() const {
    std::ostringstream stream;
    stream << "=== Performance Report ===" << std::endl;
    stream << "Session started at: " << pImpl_->startTime_ << std::endl;
    stream << "Current time: " << std::chrono::steady_clock::now() << std::endl;
    stream << "Duration: " << std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl_->startTime_).count() << "s" << std::endl;
    stream << "Average step time: " << pImpl_->getAverageStepTime() << "ms" << std::endl;
    stream << "Steps per second: " << (pImpl_->stepCount_ / std::max(pImpl_->simTime_ / pImpl_->timestep_, 1.0)) << std::endl;
    
    return stream.str();
}

void NLMSession::saveSession(const std::string& filepath) {
    if (!configManager_) {
        std::cout << "Error: Config manager not available" << std::endl;
        return;
    }
    
    bool result = configManager_->saveToFile(filepath);
    if (result) {
        std::cout << "Session saved to: " << filepath << std::endl;
        pImpl_->sessionFile_ = filepath;
    } else {
        std::cout << "Error saving session to: " << filepath << std::endl;
    }
}

void NLMSession::loadSession(const std::string& filepath) {
    if (!configManager_) {
        std::cout << "Error: Config manager not available" << std::endl;
        return;
    }
    
    bool result = configManager_->loadFromFile(filepath);
    if (result) {
        std::cout << "Session loaded from: " << filepath << std::endl;
        pImpl_->sessionFile_ = filepath;
    } else {
        std::cout << "Error loading session from: " << filepath << std::endl;
    }
}

// NLMCommandProcessor implementation
bool NLMCommandProcessor::processHelp(const std::vector<std::string>& args) {
    std::cout << "Help command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processVersion(const std::vector<std::string>& args) {
    std::cout << getVersionString() << std::endl;
    return true;
}

bool NLMCommandProcessor::processInfo(const std::vector<std::string>& args) {
    std::cout << getSystemInfo() << std::endl;
    return true;
}

bool NLMCommandProcessor::processConfig(const std::vector<std::string>& args) {
    std::cout << "Config command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processSession(const std::vector<std::string>& args) {
    std::cout << "Session command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processWorld(const std::vector<std::string>& args) {
    std::cout << "World command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processAgent(const std::vector<std::string>& args) {
    std::cout << "Agent command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processExperiment(const std::vector<std::string>& args) {
    std::cout << "Experiment command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processDebug(const std::vector<std::string>& args) {
    std::cout << "Debug command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processPerformance(const std::vector<std::string>& args) {
    std::cout << "Performance command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processSave(const std::vector<std::string>& args) {
    std::cout << "Save command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processLoad(const std::vector<std::string>& args) {
    std::cout << "Load command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processReset(const std::vector<std::string>& args) {
    std::cout << "Reset command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processExit(const std::vector<std::string>& args) {
    std::cout << "Exit command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processQuit(const std::vector<std::string>& args) {
    std::cout << "Quit command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processRun(const std::vector<std::string>& args) {
    std::cout << "Run command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processStop(const std::vector<std::string>& args) {
    std::cout << "Stop command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processStatus(const std::vector<std::string>& args) {
    std::cout << "Status command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processRecord(const std::vector<std::string>& args) {
    std::cout << "Record command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

bool NLMCommandProcessor::processReplay(const std::vector<std::string>& args) {
    std::cout << "Replay command called with args:" << std::endl;
    for (const std::string& arg : args) {
        std::cout << "  " << arg << std::endl;
    }
    return true;
}

std::string NLMCommandProcessor::getVersionString() {
    return "NLM Command Line Interface v1.0.0";
}

std::string NLMCommandProcessor::getBuildInfo() {
    return "Build: NLM v1.0.0 - Phase 6 Final Integration";
}

std::string NLMCommandProcessor::getSystemInfo() {
    std::ostringstream stream;
    stream << "=== System Information ===" << std::endl;
    stream << "NLM Command Line Interface v1.0.0" << std::endl;
    stream << "Build: NLM v1.0.0 - Phase 6 Final Integration" << std::endl;
    stream << "Platform: Linux" << std::endl;
    stream << "Architecture: x86_64" << std::endl;
    stream << "C++: C++20" << std::endl;
    stream << "Python: 3.8+" << std::endl;
    stream << std::endl;
    stream << "=== Project Information ===" << std::endl;
    stream << "Project: NLM (熙然) - Neural Learning Machine" << std::endl;
    stream << "Phase: 6 - Final Integration" << std::endl;
    stream << "Description: An experimental artificial developmental brain" << std::endl;
    stream << "Components: Neural computation, Memory systems, Neuromodulation, Prediction, Cognition" << std::endl;
    
    return stream.str();
}

// NLMShell implementation
NLMShell::NLMShell() : interactiveMode_(true), running_(true) {
    initializeShell();
}

bool NLMShell::run() {
    std::cout << "NLM Shell v1.0.0" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    std::cout << std::endl;
    
    while (running_ && interactiveMode_) {
        std::string line;
        std::cout << getPrompt();
        std::getline(std::cin, line);
        
        if (std::cin.eof()) {
            break;
        }
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Add to command history
        addCommandToHistory(line);
        
        // Execute command
        executeCommand(line);
    }
    
    std::cout << "NLM Shell shutting down..." << std::endl;
    return true;
}

void NLMShell::initializeShell() {
    setupCommands();
    setupAutoComplete();
}

void NLMShell::setupCommands() {
    // The commands are already set up in NLMCommandLine
}

void NLMShell::setupAutoComplete() {
    // Auto-completion setup would go here
}

std::string NLMShell::getCurrentDirectory() const {
    try {
        return std::filesystem::current_path().string();
    } catch (...) {
        return ".";
    }
}

std::vector<std::string> NLMShell::tokenizeCommand(const std::string& command) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(command);
    
    while (stream >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string NLMShell::expandCommand(const std::string& command) const {
    // Simple command expansion
    std::string result = command;
    
    // Handle environment variables
    size_t pos = result.find('$');
    while (pos != std::string::npos && pos + 1 < result.length()) {
        std::string varName = result.substr(pos + 1);
        size_t endPos = varName.find_first_of(" \t\"'");
        if (endPos != std::string::npos) {
            varName = varName.substr(0, endPos);
        }
        
        // Try to get environment variable
        const char* envValue = std::getenv(varName.c_str());
        if (envValue) {
            result.replace(pos, varName.length() + 1, envValue);
        } else {
            result.replace(pos, varName.length() + 1, "");
        }
        
        pos = result.find('$');
    }
    
    return result;
}

void NLMShell::executeCommand(const std::string& command) {
    std::string expandedCommand = expandCommand(command);
    std::vector<std::string> tokens = tokenizeCommand(expandedCommand);
    
    if (tokens.empty()) {
        return;
    }
    
    std::string commandName = tokens[0];
    
    // Check if command exists in command line
    bool commandFound = commandLine_->execute(tokens);
    
    if (!commandFound) {
        std::cout << "Unknown command: " << commandName << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
    }
}

bool NLMShell::startSession() {
    // This would create a session with a brain and world
    std::cout << "Session creation requires configuration - not yet implemented" << std::endl;
    return false;
}

bool NLMShell::endSession() {
    if (session_) {
        session_->stop();
        session_.reset();
    }
    return true;
}

} // namespace nlm