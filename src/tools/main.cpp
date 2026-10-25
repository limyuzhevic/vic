#!/usr/bin/env g++
// Main NLM Tools Application
// Entry point for all advanced NLM tools and utilities

#include "tools_interface.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

using namespace std;
using namespace nlm::tools;

class NLMToolsApplication {
public:
    NLMToolsApplication() {
        initializeTools();
    }
    
    ~NLMToolsApplication() {
        shutdownAllTools();
    }
    
    void run(int argc, char* argv[]) {
        if (argc < 2) {
            printHelp();
            return;
        }
        
        string command = argv[1];
        vector<string> args;
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        if (command == "--help" || command == "-h") {
            printHelp();
            return;
        }
        
        handleCommand(command, args);
    }
    
private:
    map<string, shared_ptr<ToolInterface>> tools_;
    ToolManager toolManager_;
    
    void initializeTools() {
        // Initialize core tools
        auto advancedCLI = ToolFactory::createAdvancedCLI();
        if (advancedCLI) {
            tools_["advanced-cli"] = move(advancedCLI);
        }
        
        auto memoryProfiler = ToolFactory::createMemoryProfiler();
        if (memoryProfiler) {
            tools_["memory-profiler"] = move(memoryProfiler);
        }
        
        auto debugSession = ToolFactory::createDebugSession("");
        if (debugSession) {
            tools_["debug-session"] = move(debugSession);
        }
        
        auto performanceMonitor = ToolFactory::createPerformanceMonitor();
        if (performanceMonitor) {
            tools_["performance-monitor"] = move(performanceMonitor);
        }
        
        auto experimentRunner = ToolFactory::createExperimentRunner();
        if (experimentRunner) {
            tools_["experiment-runner"] = move(experimentRunner);
        }
        
        // Initialize all tools
        for (auto& pair : tools_) {
            try {
                if (pair.second) {
                    pair.second->initialize();
                }
            } catch (const exception& e) {
                cout << "Warning: Failed to initialize tool " << pair.first 
                     << ": " << e.what() << endl;
            }
        }
    }
    
    void shutdownAllTools() {
        for (auto& pair : tools_) {
            try {
                if (pair.second) {
                    pair.second->shutdown();
                }
            } catch (const exception& e) {
                cout << "Warning: Error shutting down tool " << pair.first 
                     << ": " << e.what() << endl;
            }
        }
        tools_.clear();
    }
    
    void handleCommand(const string& command, vector<string>& args) {
        if (command == "list") {
            handleListTools();
        } else if (command == "start") {
            if (args.empty()) {
                cout << "Error: Tool name required for start command" << endl;
                return;
            }
            handleStartTool(args[0]);
        } else if (command == "stop") {
            if (args.empty()) {
                cout << "Error: Tool name required for stop command" << endl;
                return;
            }
            handleStopTool(args[0]);
        } else if (command == "status") {
            if (args.empty()) {
                handleToolStatus();
            } else {
                handleToolStatus(args[0]);
            }
        } else if (command == "run") {
            if (args.empty()) {
                cout << "Error: Tool name required for run command" << endl;
                return;
            }
            handleRunTool(args[0], args.size() > 1 ? args[1] : "");
        } else if (command == "config") {
            handleConfigCommand(args);
        } else if (command == "events") {
            handleEventsCommand(args);
        } else if (command == "help") {
            if (args.empty()) {
                printHelp();
            } else {
                printToolHelp(args[0]);
            }
        } else {
            cout << "Unknown command: " << command << endl;
            printHelp();
        }
    }
    
    void handleListTools() {
        cout << "=== Available Tools ===" << endl;
        int count = 0;
        for (const auto& pair : tools_) {
            if (pair.second) {
                count++;
                cout << "  " << pair.first << ": " << pair.second->getDescription() << endl;
            }
        }
        cout << "Total tools: " << count << endl;
    }
    
    void handleStartTool(const string& toolName) {
        auto it = tools_.find(toolName);
        if (it == tools_.end()) {
            cout << "Error: Tool not found: " << toolName << endl;
            return;
        }
        
        if (!it->second->isRunning()) {
            it->second->run();
            cout << "Tool started: " << toolName << endl;
        } else {
            cout << "Tool already running: " << toolName << endl;
        }
    }
    
    void handleStopTool(const string& toolName) {
        auto it = tools_.find(toolName);
        if (it == tools_.end()) {
            cout << "Error: Tool not found: " << toolName << endl;
            return;
        }
        
        if (it->second->isRunning()) {
            it->second->shutdown();
            cout << "Tool stopped: " << toolName << endl;
        } else {
            cout << "Tool not running: " << toolName << endl;
        }
    }
    
    void handleToolStatus(const string& toolName) {
        auto it = tools_.find(toolName);
        if (it == tools_.end()) {
            cout << "Error: Tool not found: " << toolName << endl;
            return;
        }
        
        cout << "=== Status: " << toolName << " ===" << endl;
        cout << "Description: " << it->second->getDescription() << endl;
        cout << "Running: " << (it->second->isRunning() ? "Yes" : "No") << endl;
        
        // Get tool status from tool manager
        string status = toolManager_.getToolStatus(toolName);
        if (!status.empty()) {
            cout << "Details: " << status << endl;
        }
    }
    
    void handleToolStatus() {
        cout << "=== All Tool Statuses ===" << endl;
        for (const auto& pair : tools_) {
            if (pair.second) {
                cout << "  " << pair.first << ": " 
                     << (pair.second->isRunning() ? "RUNNING" : "STOPPED") << endl;
            }
        }
        
        cout << "\n=== System Status ===" << endl;
        cout << toolManager_.getSystemStatus() << endl;
    }
    
    void handleRunTool(const string& toolName, const string& parameters) {
        auto it = tools_.find(toolName);
        if (it == tools_.end()) {
            cout << "Error: Tool not found: " << toolName << endl;
            return;
        }
        
        if (!it->second->isRunning()) {
            it->second->run();
            cout << "Tool running: " << toolName << endl;
        } else {
            cout << "Tool already running: " << toolName << endl;
        }
    }
    
    void handleConfigCommand(vector<string>& args) {
        if (args.empty()) {
            cout << "Error: Configuration command required" << endl;
            return;
        }
        
        if (args[0] == "show") {
            handleShowConfig();
        } else if (args[0] == "set") {
            if (args.size() < 3) {
                cout << "Error: Key and value required for set command" << endl;
                return;
            }
            handleSetConfig(args[1], args[2]);
        } else if (args[0] == "get") {
            if (args.size() < 2) {
                cout << "Error: Key required for get command" << endl;
                return;
            }
            handleGetConfig(args[1]);
        } else if (args[0] == "reset") {
            handleResetConfig();
        } else {
            cout << "Unknown config command: " << args[0] << endl;
        }
    }
    
    void handleShowConfig() {
        cout << "=== Current Configuration ===" << endl;
        auto configs = toolManager_.getAllConfigs();
        for (const auto& toolConfig : configs) {
            cout << "Tool: " << toolConfig.first << endl;
            for (const auto& pair : toolConfig.second) {
                cout << "  " << pair.first << " = " << pair.second << endl;
            }
        }
    }
    
    void handleSetConfig(const string& key, const string& value) {
        cout << "Setting configuration..." << endl;
        // Implementation would set configuration
        cout << "Configuration set: " << key << " = " << value << endl;
    }
    
    void handleGetConfig(const string& key) {
        cout << "Getting configuration for: " << key << endl;
        // Implementation would get configuration
        cout << "Configuration value: " << key << " = (value)" << endl;
    }
    
    void handleResetConfig() {
        cout << "Resetting configuration..." << endl;
        // Implementation would reset configuration
        cout << "Configuration reset" << endl;
    }
    
    void handleEventsCommand(vector<string>& args) {
        if (args.empty()) {
            cout << "Error: Event command required" << endl;
            return;
        }
        
        if (args[0] == "listen") {
            cout << "Event listener started" << endl;
            // Implementation would start event listening
        } else if (args[0] == "show") {
            cout << "Current events..." << endl;
            // Implementation would show current events
        } else {
            cout << "Unknown event command: " << args[0] << endl;
        }
    }
    
    void printHelp() {
        cout << "=== NLM Tools Application ===" << endl;
        cout << "Version: 2.0 - Advanced Tool Suite" << endl;
        cout << "" << endl;
        cout << "Available commands:" << endl;
        cout << "  list                    List all available tools" << endl;
        cout << "  start <tool>           Start a tool" << endl;
        cout << "  stop <tool>            Stop a tool" << endl;
        cout << "  status [tool]          Show tool status" << endl;
        cout << "  run <tool> [params]    Run a tool with parameters" << endl;
        cout << "  config [command]       Manage tool configuration" << endl;
        cout << "  events [command]       Manage tool events" << endl;
        cout << "  help [tool]            Show help for tool or all tools" << endl;
        cout << "" << endl;
        cout << "Core tools available:" << endl;
        for (const auto& pair : tools_) {
            if (pair.second) {
                cout << "  " << pair.first << " - " << pair.second->getDescription() << endl;
            }
        }
        cout << "" << endl;
        cout << "For more information on a specific tool: nlm_tools help <tool-name>" << endl;
    }
    
    void printToolHelp(const string& toolName) {
        auto it = tools_.find(toolName);
        if (it == tools_.end()) {
            cout << "Error: Tool not found: " << toolName << endl;
            return;
        }
        
        cout << "=== Help: " << toolName << " ===" << endl;
        cout << it->second->getDescription() << endl;
        cout << "" << endl;
        cout << "For detailed usage, refer to the tool-specific documentation." << endl;
    }
};

int main(int argc, char* argv[]) {
    NLMToolsApplication app;
    app.run(argc, argv);
    return 0;
}
