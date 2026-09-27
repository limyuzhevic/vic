#include "BrainScriptingTool.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <ctime>

namespace nlm {
namespace cli {

BrainScriptingTool::BrainScriptingTool() {
    scriptVariables["working_dir"] = ".";
    scriptVariables["brain_type"] = "neural_network";
    scriptVariables["timestamp"] = "";
}

BrainScriptingTool::~BrainScriptingTool() {
}

bool BrainScriptingTool::LoadScript(const std::string& scriptPath) {
    try {
        std::ifstream file(scriptPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open script file: " << scriptPath << std::endl;
            return false;
        }
        
        std::string line;
        std::string scriptContent;
        
        while (std::getline(file, line)) {
            scriptContent += line + "\n";
        }
        
        loadedScripts.push_back(scriptPath);
        
        std::cout << "Loaded script: " << scriptPath << " (" << scriptContent.length() << " characters)\n";
        
        // Update timestamp variable
        time_t now = std::time(nullptr);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", std::localtime(&now));
        scriptVariables["timestamp"] = timestamp;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading script: " << e.what() << std::endl;
        return false;
    }
}

bool BrainScriptingTool::ExecuteScript(const std::string& scriptPath) {
    try {
        // Load script first
        if (!LoadScript(scriptPath)) {
            return false;
        }
        
        std::ifstream file(scriptPath);
        std::string line;
        
        while (std::getline(file, line)) {
            if (!line.empty() && line[0] != '#') {
                // Execute command line
                ExecuteScriptLine(line);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error executing script: " << e.what() << std::endl;
        return false;
    }
}

bool BrainScriptingTool::ExecuteScriptString(const std::string& script) {
    try {
        std::stringstream ss(script);
        std::string line;
        
        while (std::getline(ss, line)) {
            if (!line.empty() && line[0] != '#') {
                ExecuteScriptLine(line);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error executing script string: " << e.what() << std::endl;
        return false;
    }
}

bool BrainScriptingTool::SaveScript(const std::string& scriptPath, const std::string& content) {
    try {
        std::ofstream file(scriptPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        
        // Update loaded scripts list
        bool exists = false;
        for (const auto& path : loadedScripts) {
            if (path == scriptPath) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            loadedScripts.push_back(scriptPath);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving script: " << e.what() << std::endl;
        return false;
    }
}

bool BrainScriptingTool::RegisterFunction(const std::string& name, std::function<bool(const std::vector<std::string>&)> func) {
    functionRegistry[name] = func;
    return true;
}

bool BrainScriptingTool::UnregisterFunction(const std::string& name) {
    auto it = functionRegistry.find(name);
    if (it != functionRegistry.end()) {
        functionRegistry.erase(it);
        return true;
    }
    return false;
}

bool BrainScriptingTool::ListFunctions(std::vector<std::string>& functionNames) const {
    functionNames.clear();
    for (const auto& pair : functionRegistry) {
        functionNames.push_back(pair.first);
    }
    return true;
}

bool BrainScriptingTool::ExecuteScriptWithArgs(const std::string& scriptPath, const std::vector<std::string>& args) {
    try {
        // Store original arguments for script access
        std::map<std::string, std::string> originalArgs = scriptVariables;
        
        // Parse arguments
        for (size_t i = 0; i < args.size(); ++i) {
            scriptVariables["arg_" + std::to_string(i)] = args[i];
        }
        
        // Execute script
        bool result = ExecuteScript(scriptPath);
        
        // Restore original arguments
        scriptVariables = originalArgs;
        
        return result;
    } catch (const std::exception& e) {
        std::cerr << "Error executing script with arguments: " << e.what() << std::endl;
        return false;
    }
}

bool BrainScriptingTool::ExecuteScriptBatch(const std::vector<std::string>& scriptPaths) {
    try {
        bool allSuccess = true;
        
        for (const auto& scriptPath : scriptPaths) {
            std::cout << "Executing script: " << scriptPath << "\n";
            if (!ExecuteScript(scriptPath)) {
                std::cerr << "Failed to execute script: " << scriptPath << "\n";
                allSuccess = false;
            }
        }
        
        return allSuccess;
    } catch (const std::exception& e) {
        std::cerr << "Error executing script batch: " << e.what() << std::endl;
        return false;
    }
}

bool BrainScriptingTool::CreateScriptTemplate(const std::string& templateName, const std::string& content) {
    try {
        // Create a script template
        std::string templatePath = "./templates/" + templateName + ".script";
        
        // Create templates directory if it doesn't exist
        std::ofstream tempFile(templatePath);
        if (!tempFile.is_open()) {
            // Try to create the directory
            std::filesystem::create_directories("./templates");
            tempFile.open(templatePath);
            if (!tempFile.is_open()) {
                return false;
            }
        }
        
        tempFile << content;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating script template: " << e.what() << std::endl;
        return false;
    }
}

bool BrainScriptingTool::GetScriptTemplate(const std::string& templateName, std::string& content) const {
    try {
        std::string templatePath = "./templates/" + templateName + ".script";
        std::ifstream file(templatePath);
        
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error getting script template: " << e.what() << std::endl;
        return false;
    }
}

bool BrainScriptingTool::SetScriptVariables(const std::map<std::string, std::string>& variables) {
    scriptVariables = variables;
    return true;
}

std::string BrainScriptingTool::GetScriptVariable(const std::string& variable) const {
    auto it = scriptVariables.find(variable);
    if (it != scriptVariables.end()) {
        return it->second;
    }
    return "";
}

// Helper function for script execution
bool BrainScriptingTool::ExecuteScriptLine(const std::string& line) {
    // Parse and execute a single line of script
    std::istringstream iss(line);
    std::string command;
    iss >> command;
    
    if (command == "echo") {
        std::string text;
        std::getline(iss, text);
        if (!text.empty() && text[0] == ' ') {
            text = text.substr(1);
        }
        std::cout << text << std::endl;
        return true;
    } else if (command == "set") {
        std::string key, value;
        if (iss >> key) {
            std::getline(iss, value);
            if (!value.empty() && value[0] == ' ') {
                value = value.substr(1);
            }
            scriptVariables[key] = value;
            return true;
        }
    } else if (command == "print_var") {
        std::string varName;
        if (iss >> varName) {
            std::cout << varName << " = " << GetScriptVariable(varName) << std::endl;
            return true;
        }
    } else if (command == "call_func") {
        std::string funcName;
        if (iss >> funcName) {
            auto it = functionRegistry.find(funcName);
            if (it != functionRegistry.end()) {
                std::vector<std::string> args;
                std::string arg;
                while (iss >> arg) {
                    args.push_back(arg);
                }
                return it->second(args);
            }
        }
    } else if (command == "if") {
        // Simple if statement parsing
        std::string condition;
        std::getline(iss, condition);
        if (!condition.empty() && condition[0] == ' ') {
            condition = condition.substr(1);
        }
        
        // Simple condition evaluation (check variable)
        size_t eqPos = condition.find('=');
        if (eqPos != std::string::npos) {
            std::string varName = condition.substr(0, eqPos);
            std::string value = condition.substr(eqPos + 1);
            
            std::string varValue = GetScriptVariable(varName);
            if (varValue == value) {
                // Condition true - execute next line (simplified)
                return true;
            }
        }
    } else if (command == "for") {
        // Simple for loop parsing
        std::string var, range, action;
        std::getline(iss, var);
        if (!var.empty() && var[0] == ' ') {
            var = var.substr(1);
        }
        
        // Parse for loop: for var in start-end
        size_t inPos = var.find(" in ");
        if (inPos != std::string::npos) {
            std::string loopVar = var.substr(0, inPos);
            std::string rangeStr = var.substr(inPos + 4);
            
            size_t dashPos = rangeStr.find('-');
            if (dashPos != std::string::npos) {
                int start = std::stoi(rangeStr.substr(0, dashPos));
                int end = std::stoi(rangeStr.substr(dashPos + 1));
                
                for (int i = start; i <= end; ++i) {
                    scriptVariables[loopVar] = std::to_string(i);
                    // In a real implementation, we would execute the loop body
                }
            }
        }
    }
    
    std::cout << "Unknown script command: " << command << std::endl;
    return false;
}

} // namespace cli
} // namespace nlm