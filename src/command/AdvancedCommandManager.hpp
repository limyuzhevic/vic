#pragma once

#include "CommandSystem.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <algorithm>
#include <map>
#include <unordered_set>

namespace nlm {

class AdvancedCommandManager {
public:
    AdvancedCommandManager(std::shared_ptr<Brain> brain,
                          std::shared_ptr<CheckpointManager> checkpointManager,
                          std::shared_ptr<Config> config,
                          std::shared_ptr<ILogger> logger)
        : brain_(brain), checkpointManager_(checkpointManager), config_(config), logger_(logger) {
        
        // Initialize command manager
        commandManager_ = std::make_shared<CommandManager>(brain, checkpointManager, config, logger);
        
        // Register all advanced commands
        registerAllCommands();
        
        // Set up scripting support
        scriptingEngine_ = std::make_shared<ScriptingEngine>(logger);
        scriptInterpreter_ = std::make_shared<ScriptInterpreter>(scriptingEngine_);
    }
    
    // Register all advanced commands
    void registerAllCommands() {
        // Batch processing commands
        commandManager_->registerCommand(std::make_unique<RunBatchCommand>(brain_, checkpointManager_, logger_));
        commandManager_->registerCommand(std::make_unique<RunUntilCommand>(brain_, logger_));
        commandManager_->registerCommand(std::make_unique<ParallelRunCommand>(brain_, logger_));
        
        // Checkpoint commands
        commandManager_->registerCommand(std::make_unique<SaveCheckpointCommand>(brain_, checkpointManager_, logger_));
        commandManager_->registerCommand(std::make_unique<LoadCheckpointCommand>(brain_, checkpointManager_, logger_));
        commandManager_->registerCommand(std::make_unique<ListCheckpointsCommand>(checkpointManager_, logger_));
        commandManager_->registerCommand(std::make_unique<DeleteCheckpointCommand>(checkpointManager_, logger_));
        commandManager_->registerCommand(std::make_unique<CheckpointInfoCommand>(checkpointManager_, logger_));
        
        // Performance monitoring commands
        commandManager_->registerCommand(std::make_unique<MonitorStartCommand>(brain_, checkpointManager_, logger_));
        commandManager_->registerCommand(std::make_unique<MonitorStopCommand>(logger_));
        commandManager_->registerCommand(std::make_unique<PerformanceStatsCommand>(brain_, logger_));
        
        // Configuration management commands
        commandManager_->registerCommand(ConfigCommands::createSaveConfigCommand(config_, logger_));
        commandManager_->registerCommand(ConfigCommands::createLoadConfigCommand(config_, logger_));
        
        // Plugin extension commands
        commandManager_->registerCommand(PluginCommands::createRegisterPluginCommand(logger_));
        commandManager_->registerCommand(PluginCommands::createListPluginsCommand(logger_));
        
        // Scripting commands (global commands)
        commandManager_->registerCommand(std::make_unique<ScriptCommand>(scriptingEngine_, scriptInterpreter_));
        commandManager_->registerCommand(std::make_unique<ReplayCommand>(scriptingEngine_));
        commandManager_->registerCommand(std::make_unique<HistoryCommand>(scriptingEngine_));
    }
    
    // Execute any advanced command (global or requires brain)
    CommandResult executeAdvancedCommand(const std::string& commandName,
                                         const std::vector<std::string>& args,
                                         bool forceGlobal = false) {
        return commandManager_->executeCommand(commandName, args);
    }
    
    // Execute global command (no brain required)
    CommandResult executeGlobalCommand(const std::string& commandName,
                                       const std::vector<std::string>& args) {
        return commandManager_->executeGlobalCommand(commandName, args);
    }
    
    // Get all advanced command information
    std::vector<CommandManager::CommandInfo> getAdvancedCommandInfo() const {
        return commandManager_->getCommandInfo();
    }
    
    // Get help for a specific command
    std::string getCommandHelp(const std::string& commandName) const {
        return commandManager_->getCommandHelp(commandName);
    }
    
    // Get available commands list
    std::vector<std::string> getAvailableCommands() const {
        return commandManager_->getAvailableCommands();
    }
    
    // Get scripting engine
    std::shared_ptr<ScriptingEngine> getScriptingEngine() const {
        return scriptingEngine_;
    }
    
    // Get script interpreter
    std::shared_ptr<ScriptInterpreter> getScriptInterpreter() const {
        return scriptInterpreter_;
    }
    
    // Check if a command exists
    bool commandExists(const std::string& commandName) const {
        return commandManager_->getAvailableCommands().end() != 
               std::find(commandManager_->getAvailableCommands().begin(),
                        commandManager_->getAvailableCommands().end(),
                        commandName);
    }
    
private:
    // Helper command classes for scripting
    
    class ScriptCommand : public ICommand {
    public:
        ScriptCommand(std::shared_ptr<ScriptingEngine> engine,
                     std::shared_ptr<ScriptInterpreter> interpreter)
            : engine_(engine), interpreter_(interpreter) {}
        
        const std::string& getName() const override { static const std::string name = "script"; return name; }
        const std::string& getDescription() const override { 
            static const std::string desc = "Execute a script or script file"; 
            return desc; 
        }
        const std::vector<std::string>& getAliases() const override { 
            static const std::vector<std::string> aliases = {"execute-script", "run-script"}; 
            return aliases; 
        }
        const std::string& getUsage() const override { 
            static const std::string usage = "script <script_file> [--interactive] [--replay] [--history]"; 
            return usage; 
        }
        bool requiresBrain() const override { return true; }
        bool isGlobal() const override { return false; }
        
        CommandResult execute(const std::vector<std::string>& args) override {
            if (args.empty()) {
                return CommandResult(false, "", "script command requires at least a file path", 
                                    std::chrono::milliseconds(0));
            }
            
            std::string filepath = args[0];
            bool interactive = false;
            bool replay = false;
            bool history = false;
            
            // Parse flags
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i] == "--interactive") {
                    interactive = true;
                } else if (args[i] == "--replay") {
                    replay = true;
                } else if (args[i] == "--history") {
                    history = true;
                }
            }
            
            if (interactive) {
                return interpreter_->interactiveMode(brain_, checkpointManager_, config_, commandManager_.get());
            } else if (replay) {
                return engine_->replayCommands(-1, brain_, checkpointManager_, config_, commandManager_.get());
            } else if (history) {
                std::vector<std::string> commandHistory = engine_->getHistory();
                std::stringstream output;
                output << "Command History (" << commandHistory.size() << " commands):\n\n";
                for (size_t i = 0; i < commandHistory.size(); ++i) {
                    output << (i + 1) << ". " << commandHistory[i] << "\n";
                }
                return CommandResult(true, output.str(), "", std::chrono::milliseconds(0));
            } else {
                return interpreter_->interpretFile(filepath, brain_, checkpointManager_, config_, commandManager_.get());
            }
        }
        
    private:
        std::shared_ptr<ScriptingEngine> engine_;
        std::shared_ptr<ScriptInterpreter> interpreter_;
    };
    
    class ReplayCommand : public ICommand {
    public:
        ReplayCommand(std::shared_ptr<ScriptingEngine> engine)
            : engine_(engine) {}
        
        const std::string& getName() const override { static const std::string name = "replay"; return name; }
        const std::string& getDescription() const override { 
            static const std::string desc = "Replay last N commands from history"; 
            return desc; 
        }
        const std::vector<std::string>& getAliases() const override { 
            static const std::vector<std::string> aliases = {"redo", "repeat"}; 
            return aliases; 
        }
        const std::string& getUsage() const override { 
            static const std::string usage = "replay <count> [--all]"; 
            return usage; 
        }
        bool requiresBrain() const override { return true; }
        bool isGlobal() const override { return false; }
        
        CommandResult execute(const std::vector<std::string>& args) override {
            if (args.empty()) {
                return CommandResult(false, "", "replay command requires count", 
                                    std::chrono::milliseconds(0));
            }
            
            try {
                int count = std::stoi(args[0]);
                bool all = false;
                
                if (args.size() > 1 && args[1] == "--all") {
                    all = true;
                }
                
                return engine_->replayCommands(count, brain_, checkpointManager_, config_, commandManager_.get());
            } catch (const std::exception& e) {
                return CommandResult(false, "", "Invalid replay count: " + std::string(e.what()), 
                                    std::chrono::milliseconds(0));
            }
        }
        
    private:
        std::shared_ptr<ScriptingEngine> engine_;
    };
    
    class HistoryCommand : public ICommand {
    public:
        HistoryCommand(std::shared_ptr<ScriptingEngine> engine)
            : engine_(engine) {}
        
        const std::string& getName() const override { static const std::string name = "history"; return name; }
        const std::string& getDescription() const override { 
            static const std::string desc = "Show or manage command history"; 
            return desc; 
        }
        const std::vector<std::string>& getAliases() const override { 
            static const std::vector<std::string> aliases = {"hist", "command-history"}; 
            return aliases; 
        }
        const std::string& getUsage() const override { 
            static const std::string usage = "history [--clear] [--max <size>] [--show]"; 
            return usage; 
        }
        bool requiresBrain() const override { return false; }
        bool isGlobal() const override { return true; }
        
        CommandResult execute(const std::vector<std::string>& args) override {
            try {
                bool clear = false;
                bool show = false;
                int maxSize = -1;
                
                // Parse flags
                for (size_t i = 0; i < args.size(); ++i) {
                    if (args[i] == "--clear") {
                        clear = true;
                    } else if (args[i] == "--show") {
                        show = true;
                    } else if (args[i] == "--max" && i + 1 < args.size()) {
                        maxSize = std::stoi(args[++i]);
                    }
                }
                
                if (clear) {
                    engine_->clearHistory();
                    return CommandResult(true, "Command history cleared", "", 
                                        std::chrono::milliseconds(0));
                }
                
                if (maxSize > 0) {
                    engine_->setMaxHistorySize(maxSize);
                    return CommandResult(true, "Max history size set to " + std::to_string(maxSize), "", 
                                        std::chrono::milliseconds(0));
                }
                
                std::vector<std::string> commandHistory = engine_->getHistory();
                
                if (show || commandHistory.empty()) {
                    std::stringstream output;
                    output << "Command History (" << commandHistory.size() << " commands):\n\n";
                    
                    for (size_t i = 0; i < commandHistory.size(); ++i) {
                        output << (i + 1) << ". " << commandHistory[i] << "\n";
                    }
                    
                    if (commandHistory.empty()) {
                        output << "(No commands recorded yet)\n";
                    }
                    
                    return CommandResult(true, output.str(), "", std::chrono::milliseconds(0));
                } else {
                    return CommandResult(true, "", "Use --show to display history", 
                                        std::chrono::milliseconds(0));
                }
            } catch (const std::exception& e) {
                return CommandResult(false, "", "History command error: " + std::string(e.what()), 
                                    std::chrono::milliseconds(0));
            }
        }
        
    private:
        std::shared_ptr<ScriptingEngine> engine_;
    };
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<CheckpointManager> checkpointManager_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<CommandManager> commandManager_;
    std::shared_ptr<ScriptingEngine> scriptingEngine_;
    std::shared_ptr<ScriptInterpreter> scriptInterpreter_;
};

// Advanced command system initialization helper
inline std::shared_ptr<AdvancedCommandManager> initializeAdvancedCommands(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<CheckpointManager> checkpointManager,
    std::shared_ptr<Config> config,
    std::shared_ptr<ILogger> logger) {
    
    return std::make_shared<AdvancedCommandManager>(brain, checkpointManager, config, logger);
}

} // namespace nlm