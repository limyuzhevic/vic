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

class ScriptingEngine {
public:
    ScriptingEngine(std::shared_ptr<ILogger> logger)
        : logger_(logger) {}
    
    // Parse and execute a command line
    CommandResult executeLine(const std::string& line, 
                             std::shared_ptr<Brain> brain,
                             std::shared_ptr<CheckpointManager> checkpointManager,
                             std::shared_ptr<Config> config,
                             CommandManager* commandManager) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Trim whitespace from line
            std::string trimmedLine = trim(line);
            
            if (trimmedLine.empty()) {
                return CommandResult(true, "", "", std::chrono::milliseconds(0));
            }
            
            if (trimmedLine[0] == '#') {
                return CommandResult(true, "", "", std::chrono::milliseconds(0));
            }
            
            // Parse command and arguments
            size_t cmdEnd = trimmedLine.find(' ');
            if (cmdEnd == std::string::npos) {
                cmdEnd = trimmedLine.size();
            }
            
            std::string command = trimmedLine.substr(0, cmdEnd);
            std::string argsPart = (cmdEnd < trimmedLine.size()) ? 
                                   trimmedLine.substr(cmdEnd + 1) : "";
            
            // Parse arguments (simple space-separated, quoted strings supported)
            std::vector<std::string> args;
            parseArguments(argsPart, args);
            
            // Check for special commands
            if (command == "exit" || command == "quit") {
                return CommandResult(true, "Script ended.", "", std::chrono::milliseconds(0));
            } else if (command == "help" || command == "?") {
                return showHelp(args);
            } else if (command == "echo") {
                std::string output = "";
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i > 0) output += " ";
                    output += args[i];
                }
                return CommandResult(true, output, "", std::chrono::milliseconds(0));
            } else if (command == "if") {
                return handleConditional(args, brain, checkpointManager, config, commandManager);
            } else if (command == "while") {
                return handleLoop(args, brain, checkpointManager, config, commandManager);
            } else if (command == "for") {
                return handleForLoop(args, brain, checkpointManager, config, commandManager);
            } else if (command == "set") {
                return handleVariableSet(args, brain, checkpointManager, config, commandManager);
            } else if (command == "run") {
                return handleRunCommand(args, brain, checkpointManager, config, commandManager);
            } else if (command == "monitor") {
                return handleMonitorCommand(args, brain, checkpointManager, config, commandManager);
            } else if (command == "checkpoint") {
                return handleCheckpointCommand(args, brain, checkpointManager, config, commandManager);
            } else if (command == "script") {
                return handleIncludeScript(args, brain, checkpointManager, config, commandManager);
            } else {
                // Try to execute via command manager
                if (commandManager) {
                    CommandResult result = commandManager->executeCommand(command, args);
                    auto endTime = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                    result.executionTime = duration;
                    return result;
                } else {
                    return CommandResult(false, "", "Command manager not available", 
                                        std::chrono::milliseconds(0));
                }
            }
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            return CommandResult(false, "", "Script error: " + std::string(e.what()), duration);
        }
    }
    
    // Load and execute a script file
    CommandResult executeScript(const std::string& filepath,
                               std::shared_ptr<Brain> brain,
                               std::shared_ptr<CheckpointManager> checkpointManager,
                               std::shared_ptr<Config> config,
                               CommandManager* commandManager) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return CommandResult(false, "", "Cannot open script file: " + filepath, 
                                    std::chrono::milliseconds(0));
            }
            
            std::stringstream scriptContent;
            scriptContent << file.rdbuf();
            std::string content = scriptContent.str();
            
            logger_->info("Executing script: " + filepath);
            
            // Execute script line by line
            std::vector<std::string> lines;
            std::stringstream lineStream(content);
            std::string line;
            while (std::getline(lineStream, line)) {
                lines.push_back(line);
            }
            
            std::stringstream output;
            int lineNum = 0;
            bool scriptSuccess = true;
            
            for (const auto& line : lines) {
                lineNum++;
                CommandResult result = executeLine(line, brain, checkpointManager, config, commandManager);
                
                if (!result.success && !line.empty() && line[0] != '#') {
                    // Error in script
                    output << "Line " << lineNum << ": " << result.errorMessage << "\n";
                    scriptSuccess = false;
                    
                    // Continue unless fatal error
                    if (line.find("fatal") != std::string::npos || 
                        line.find("exit") != std::string::npos) {
                        break;
                    }
                } else if (!result.output.empty()) {
                    output << result.output << "\n";
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            output << "\nScript execution completed in " << duration.count() << " ms";
            
            if (!scriptSuccess) {
                output << " (with errors)";
            }
            
            logger_->info("Script executed: " + filepath + ", success: " + std::string(scriptSuccess ? "yes" : "no"));
            
            return CommandResult(scriptSuccess, output.str(), "", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            return CommandResult(false, "", "Script execution error: " + std::string(e.what()), duration);
        }
    }
    
    // Get command history
    std::vector<std::string> getHistory() const {
        return history_;
    }
    
    // Clear history
    void clearHistory() {
        history_.clear();
    }
    
    // Set maximum history size
    void setMaxHistorySize(size_t maxSize) {
        maxHistorySize_ = maxSize;
        if (history_.size() > maxSize) {
            history_.erase(history_.begin(), history_.begin() + (history_.size() - maxSize));
        }
    }
    
    // Replay last N commands
    CommandResult replayCommands(size_t count,
                                std::shared_ptr<Brain> brain,
                                std::shared_ptr<CheckpointManager> checkpointManager,
                                std::shared_ptr<Config> config,
                                CommandManager* commandManager) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            if (count == 0 || history_.empty()) {
                return CommandResult(true, "", "No commands to replay", std::chrono::milliseconds(0));
            }
            
            size_t replayCount = std::min(count, history_.size());
            std::vector<std::string> commandsToReplay;
            
            if (count == -1) {
                // Replay all
                commandsToReplay = history_;
            } else {
                // Replay last N
                commandsToReplay = std::vector<std::string>(history_.end() - replayCount, history_.end());
            }
            
            logger_->info("Replaying " + std::to_string(replayCount) + " commands");
            
            std::stringstream output;
            bool replaySuccess = true;
            
            for (const auto& cmd : commandsToReplay) {
                CommandResult result = executeLine(cmd, brain, checkpointManager, config, commandManager);
                
                if (!result.success && !cmd.empty()) {
                    output << "Error replaying: " << cmd << " - " << result.errorMessage << "\n";
                    replaySuccess = false;
                }
                
                output << result.output << "\n";
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            output << "\nReplay completed in " << duration.count() << " ms";
            
            logger_->info("Command replay completed: " + std::to_string(replayCount) + " commands");
            
            return CommandResult(replaySuccess, output.str(), "", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            return CommandResult(false, "", "Replay error: " + std::string(e.what()), duration);
        }
    }
    
private:
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
    
    void parseArguments(const std::string& argsPart, std::vector<std::string>& args) {
        std::string current;
        bool inQuotes = false;
        char quoteChar = 0;
        
        for (size_t i = 0; i < argsPart.size(); ++i) {
            char c = argsPart[i];
            
            if (!inQuotes && (c == ' ' || c == '\t')) {
                if (!current.empty()) {
                    args.push_back(current);
                    current.clear();
                }
            } else if (c == '\"' || c == '\'') {
                if (!inQuotes) {
                    inQuotes = true;
                    quoteChar = c;
                } else if (quoteChar == c) {
                    inQuotes = false;
                    quoteChar = 0;
                } else {
                    current += c;
                }
            } else {
                current += c;
            }
        }
        
        if (!current.empty()) {
            args.push_back(current);
        }
    }
    
    CommandResult showHelp(const std::vector<std::string>& args) {
        std::stringstream output;
        
        if (args.empty()) {
            output << "=== NLM Scripting Help ===\n\n";
            output << "Available commands:\n";
            output << "  echo <text>                    - Echo text\n";
            output << "  if <condition> <command>       - Conditional execution\n";
            output << "  while <condition> <command>    - Loop while condition true\n";
            output << "  for <var> <start> <end> <step> <command> - For loop\n";
            output << "  set <var> <value>              - Set variable\n";
            output << "  run <command> [args...]        - Execute system command\n";
            output << "  monitor <command> [args...]    - Execute monitoring command\n";
            output << "  checkpoint <command> [args...] - Execute checkpoint command\n";
            output << "  script <file>                 - Include and execute script file\n";
            output << "  help [command]                 - Show help information\n";
            output << "  exit/quit                      - End script\n";
        } else {
            // Help for specific command
            output << "Help for: " << args[0] << "\n";
            output << "(Use 'help' without arguments for full list)\n";
        }
        
        return CommandResult(true, output.str(), "", std::chrono::milliseconds(0));
    }
    
    CommandResult handleConditional(const std::vector<std::string>& args,
                                    std::shared_ptr<Brain> brain,
                                    std::shared_ptr<CheckpointManager> checkpointManager,
                                    std::shared_ptr<Config> config,
                                    CommandManager* commandManager) {
        if (args.size() < 2) {
            return CommandResult(false, "", "if command requires condition and command", 
                                std::chrono::milliseconds(0));
        }
        
        std::string condition = args[0];
        std::vector<std::string> cmdArgs(args.begin() + 1, args.end());
        
        bool conditionResult = evaluateCondition(condition, brain, checkpointManager, config);
        
        if (conditionResult) {
            // Execute the command
            return executeLine(cmdArgs[0], brain, checkpointManager, config, commandManager);
        } else {
            return CommandResult(true, "", "", std::chrono::milliseconds(0));
        }
    }
    
    CommandResult handleLoop(const std::vector<std::string>& args,
                            std::shared_ptr<Brain> brain,
                            std::shared_ptr<CheckpointManager> checkpointManager,
                            std::shared_ptr<Config> config,
                            CommandManager* commandManager) {
        if (args.size() < 2) {
            return CommandResult(false, "", "while command requires condition and command", 
                                std::chrono::milliseconds(0));
        }
        
        std::string condition = args[0];
        std::vector<std::string> cmdArgs(args.begin() + 1, args.end());
        
        CommandResult result(true, "", "", std::chrono::milliseconds(0));
        
        while (evaluateCondition(condition, brain, checkpointManager, config)) {
            // Execute command once per iteration
            CommandResult iterationResult = executeLine(cmdArgs[0], brain, checkpointManager, config, commandManager);
            
            if (!iterationResult.success) {
                result = iterationResult;
                break;
            }
            
            // Add iteration output
            result.output += iterationResult.output + "\n";
            
            // Prevent infinite loops
            static size_t maxIterations = 1000;
            static std::unordered_map<std::string, size_t> loopCounters;
            
            std::string loopKey = condition + "_" + cmdArgs[0];
            loopCounters[loopKey]++;
            if (loopCounters[loopKey] > maxIterations) {
                result.output += "WARNING: Maximum iterations reached, breaking loop\n";
                break;
            }
        }
        
        return result;
    }
    
    CommandResult handleForLoop(const std::vector<std::string>& args,
                                std::shared_ptr<Brain> brain,
                                std::shared_ptr<CheckpointManager> checkpointManager,
                                std::shared_ptr<Config> config,
                                CommandManager* commandManager) {
        if (args.size() < 5) {
            return CommandResult(false, "", "for command requires var start end step command", 
                                std::chrono::milliseconds(0));
        }
        
        std::string varName = args[0];
        try {
            long start = std::stol(args[1]);
            long end = std::stol(args[2]);
            long step = std::stoi(args[3]);
            std::vector<std::string> cmdArgs(args.begin() + 4, args.end());
            
            CommandResult result(true, "", "", std::chrono::milliseconds(0));
            
            for (long i = start; (step > 0) ? (i <= end) : (i >= end); i += step) {
                // Set loop variable
                setVariable(varName, std::to_string(i), brain, checkpointManager, config, commandManager);
                
                // Execute command
                CommandResult iterationResult = executeLine(cmdArgs[0], brain, checkpointManager, config, commandManager);
                
                if (!iterationResult.success) {
                    result = iterationResult;
                    break;
                }
                
                result.output += iterationResult.output + "\n";
            }
            
            return result;
            
        } catch (const std::exception& e) {
            return CommandResult(false, "", "For loop error: " + std::string(e.what()), 
                                std::chrono::milliseconds(0));
        }
    }
    
    CommandResult handleVariableSet(const std::vector<std::string>& args,
                                    std::shared_ptr<Brain> brain,
                                    std::shared_ptr<CheckpointManager> checkpointManager,
                                    std::shared_ptr<Config> config,
                                    CommandManager* commandManager) {
        if (args.size() < 2) {
            return CommandResult(false, "", "set command requires variable name and value", 
                                std::chrono::milliseconds(0));
        }
        
        return setVariable(args[0], args[1], brain, checkpointManager, config, commandManager);
    }
    
    CommandResult handleRunCommand(const std::vector<std::string>& args,
                                   std::shared_ptr<Brain> brain,
                                   std::shared_ptr<CheckpointManager> checkpointManager,
                                   std::shared_ptr<Config> config,
                                   CommandManager* commandManager) {
        if (args.empty()) {
            return CommandResult(false, "", "run command requires command name", 
                                std::chrono::milliseconds(0));
        }
        
        // This is a simplified run command - in real implementation would call system()
        std::string command = args[0];
        logger_->info("Running system command: " + command);
        
        // For this example, just echo the command
        std::stringstream output;
        output << "Executing: " << command << "\n";
        if (args.size() > 1) {
            output << "Arguments: ";
            for (size_t i = 1; i < args.size(); ++i) {
                if (i > 1) output << ", ";
                output << args[i];
            }
            output << "\n";
        }
        output << "(Note: Actual system command execution requires implementation)\n";
        
        return CommandResult(true, output.str(), "", std::chrono::milliseconds(0));
    }
    
    CommandResult handleMonitorCommand(const std::vector<std::string>& args,
                                       std::shared_ptr<Brain> brain,
                                       std::shared_ptr<CheckpointManager> checkpointManager,
                                       std::shared_ptr<Config> config,
                                       CommandManager* commandManager) {
        if (args.empty()) {
            return CommandResult(false, "", "monitor command requires subcommand", 
                                std::chrono::milliseconds(0));
        }
        
        std::string subcommand = args[0];
        std::vector<std::string> cmdArgs(args.begin() + 1, args.end());
        
        return executeLine(subcommand, brain, checkpointManager, config, commandManager);
    }
    
    CommandResult handleCheckpointCommand(const std::vector<std::string>& args,
                                          std::shared_ptr<Brain> brain,
                                          std::shared_ptr<CheckpointManager> checkpointManager,
                                          std::shared_ptr<Config> config,
                                          CommandManager* commandManager) {
        if (args.empty()) {
            return CommandResult(false, "", "checkpoint command requires subcommand", 
                                std::chrono::milliseconds(0));
        }
        
        std::string subcommand = args[0];
        std::vector<std::string> cmdArgs(args.begin() + 1, args.end());
        
        return executeLine(subcommand, brain, checkpointManager, config, commandManager);
    }
    
    CommandResult handleIncludeScript(const std::vector<std::string>& args,
                                      std::shared_ptr<Brain> brain,
                                      std::shared_ptr<CheckpointManager> checkpointManager,
                                      std::shared_ptr<Config> config,
                                      CommandManager* commandManager) {
        if (args.empty()) {
            return CommandResult(false, "", "script command requires file path", 
                                std::chrono::milliseconds(0));
        }
        
        std::string filepath = args[0];
        
        // For relative paths, prepend script directory
        if (filepath.find('/') != 0 && filepath.find('\\') != 0) {
            filepath = "./scripts/" + filepath;
        }
        
        return executeScript(filepath, brain, checkpointManager, config, commandManager);
    }
    
    bool evaluateCondition(const std::string& condition,
                          std::shared_ptr<Brain> brain,
                          std::shared_ptr<CheckpointManager> checkpointManager,
                          std::shared_ptr<Config> config) {
        // Parse condition (simplified evaluation)
        if (condition.find("spikes>") != std::string::npos) {
            size_t pos = condition.find("spikes>");
            std::string valueStr = condition.substr(pos + 7);
            try {
                size_t value = std::stoul(valueStr);
                return brain->getTotalSpikeCount() > value;
            } catch (...) { return false; }
        } else if (condition.find("neurons<") != std::string::npos) {
            size_t pos = condition.find("neurons<");
            std::string valueStr = condition.substr(pos + 8);
            try {
                size_t value = std::stoul(valueStr);
                return brain->getActiveNeuronCount() < value;
            } catch (...) { return false; }
        } else if (condition == "learning") {
            // Check if learning is occurring (placeholder)
            return true;
        } else if (condition == "stable") {
            // Check stability (placeholder)
            return true;
        } else if (condition == "true") {
            return true;
        } else if (condition == "false") {
            return false;
        }
        
        return false;
    }
    
    CommandResult setVariable(const std::string& name, const std::string& value,
                             std::shared_ptr<Brain> brain,
                             std::shared_ptr<CheckpointManager> checkpointManager,
                             std::shared_ptr<Config> config,
                             CommandManager* commandManager) {
        // Store variable in config (simplified)
        if (config) {
            config->set("script_var_" + name, value, ConfigSource::Runtime);
        }
        
        return CommandResult(true, "", "", std::chrono::milliseconds(0));
    }
    
    CommandResult getVariable(const std::string& name,
                             std::shared_ptr<Brain> brain,
                             std::shared_ptr<CheckpointManager> checkpointManager,
                             std::shared_ptr<Config> config,
                             CommandManager* commandManager) {
        if (config && config->has("script_var_" + name)) {
            auto varValue = config->get<std::string>("script_var_" + name);
            if (varValue) {
                return CommandResult(true, *varValue, "", std::chrono::milliseconds(0));
            }
        }
        
        return CommandResult(true, "", "", std::chrono::milliseconds(0));
    }
    
    std::vector<std::string> history_;
    size_t maxHistorySize_ = 100;
    
    std::shared_ptr<ILogger> logger_;
};

class ScriptInterpreter {
public:
    ScriptInterpreter(std::shared_ptr<ScriptingEngine> engine)
        : engine_(engine) {}
    
    // Interpret a script string
    CommandResult interpretScript(const std::string& script,
                                 std::shared_ptr<Brain> brain,
                                 std::shared_ptr<CheckpointManager> checkpointManager,
                                 std::shared_ptr<Config> config,
                                 CommandManager* commandManager) {
        return engine_->executeLine(script, brain, checkpointManager, config, commandManager);
    }
    
    // Interpret a script from a file
    CommandResult interpretFile(const std::string& filepath,
                                std::shared_ptr<Brain> brain,
                                std::shared_ptr<CheckpointManager> checkpointManager,
                                std::shared_ptr<Config> config,
                                CommandManager* commandManager) {
        return engine_->executeScript(filepath, brain, checkpointManager, config, commandManager);
    }
    
    // Interactive script execution
    CommandResult interactiveMode(std::shared_ptr<Brain> brain,
                                 std::shared_ptr<CheckpointManager> checkpointManager,
                                 std::shared_ptr<Config> config,
                                 CommandManager* commandManager) {
        std::stringstream output;
        output << "=== NLM Interactive Script Mode ===\n";
        output << "Type commands or 'exit' to quit.\n\n";
        
        std::string line;
        std::cout << output.str();
        
        while (std::getline(std::cin, line)) {
            if (line.empty() || line == "exit" || line == "quit") {
                break;
            }
            
            // Add to history
            engine_->getHistory().push_back(line);
            
            // Execute command
            CommandResult result = engine_->executeLine(line, brain, checkpointManager, config, commandManager);
            
            if (!result.success) {
                std::cerr << "Error: " << result.errorMessage << std::endl;
            } else if (!result.output.empty()) {
                std::cout << result.output << std::endl;
            }
        }
        
        std::cout << "\nScript session ended.\n";
        return CommandResult(true, "", "", std::chrono::milliseconds(0));
    }
    
private:
    std::shared_ptr<ScriptingEngine> engine_;
};

} // namespace nlm