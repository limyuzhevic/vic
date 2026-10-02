#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

// Forward declarations
class Brain;
class Config;
class Command;

class CommandLineTool {
public:
    virtual ~CommandLineTool() = default;
    
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual void printHelp() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

class BrainManager {
public:
    BrainManager();
    ~BrainManager();
    
    bool createBrain(const std::string& configPath = "");
    bool loadBrain(const std::string& checkpointPath);
    bool saveBrain(const std::string& checkpointPath);
    bool runSimulation(int steps = -1, double timeStep = 0.001);
    bool stepSimulation();
    bool resetBrain();
    bool pauseBrain();
    bool resumeBrain();
    bool exportBrain(const std::string& outputPath);
    bool analyzeBrain(const std::string& analysisType = "full");
    bool debugBrain(const std::string& debugLevel = "normal");
    
    Brain* getBrain() const { return brain_.get(); }
    Config* getConfig() const { return config_.get(); }
    bool isRunning() const { return isRunning_; }
    bool isPaused() const { return isPaused_; }
    
    std::string getStatusReport() const;
    std::string getPerformanceStats() const;
    std::vector<std::string> getAvailableCommands() const;
    
private:
    std::unique_ptr<Brain> brain_;
    std::unique_ptr<Config> config_;
    mutable std::mutex mutex_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> isPaused_;
    int stepCount_;
    double totalTime_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

class Command {
public:
    using CommandFunc = std::function<bool(const std::vector<std::string>&)>;
    
    Command(const std::string& name, const std::string& description, 
            CommandFunc func, const std::string& category = "General",
            const std::string& syntax = "<arguments>")
        : name_(name), description_(description), 
          func_(func), category_(category), syntax_(syntax) {}
    
    bool execute(const std::vector<std::string>& args) const {
        return func_(args);
    }
    
    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }
    std::string getCategory() const { return category_; }
    std::string getSyntax() const { return syntax_; }
    
    bool operator<(const Command& other) const {
        return name_ < other.name_;
    }
    
private:
    std::string name_;
    std::string description_;
    CommandFunc func_;
    std::string category_;
    std::string syntax_;
};

class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }
    
    void registerCommand(std::unique_ptr<Command> command) {
        commands_.insert(std::move(command));
    }
    
    std::vector<const Command*> getCommands() const {
        std::vector<const Command*> result;
        for (const auto& cmd : commands_) {
            result.push_back(cmd.get());
        }
        return result;
    }
    
    const Command* findCommand(const std::string& name) const {
        for (const auto& cmd : commands_) {
            if (cmd->getName() == name) {
                return cmd.get();
            }
        }
        return nullptr;
    }
    
    std::vector<const Command*> getCommandsByCategory(const std::string& category) const {
        std::vector<const Command*> result;
        for (const auto& cmd : commands_) {
            if (cmd->getCategory() == category) {
                result.push_back(cmd.get());
            }
        }
        return result;
    }
    
private:
    CommandRegistry() = default;
    std::set<std::unique_ptr<Command>> commands_;
};

// Command categories
namespace Categories {
    const std::string BRAIN_CONTROL = "Brain Control";
    const std::string SIMULATION = "Simulation";
    const std::string ANALYSIS = "Analysis";
    const std::string DEBUG = "Debug";
    const std::string CONFIG = "Configuration";
    const std::string EXPORT = "Export";
    const std::string UTILITIES = "Utilities";
}