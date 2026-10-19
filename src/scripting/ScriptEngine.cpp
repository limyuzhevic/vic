// Script Engine Implementation

#include "nlm/scripting/ScriptEngine.hpp"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <regex>

namespace nlm {
namespace scripting {

ScriptEngine::ScriptEngine() : scriptMonitoringEnabled_(false), scriptSecurityEnabled_(false) {
}

ScriptEngine::~ScriptEngine() {
}

bool ScriptEngine::parseScript(const std::string& scriptFile) {
    std::ifstream file(scriptFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string scriptName = scriptFile;
    size_t lastSlash = scriptName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        scriptName = scriptName.substr(lastSlash + 1);
    }
    
    std::string scriptContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    return loadScript(scriptName, scriptContent);
}

bool ScriptEngine::validateScript(const std::string& scriptContent) {
    // Determine script type based on content
    std::string scriptType;
    
    if (scriptContent.find("#") == 0 || scriptContent.find("!/bin/bash") != std::string::npos) {
        scriptType = "bash";
    } else if (scriptContent.find("import ") != std::string::npos || scriptContent.find("def ") != std::string::npos) {
        scriptType = "python";
    } else if (scriptContent.find("{") != std::string::npos && scriptContent.find("}") != std::string::npos) {
        scriptType = "json";
    } else {
        scriptType = "generic";
    }
    
    return validateScriptSyntax(scriptContent, scriptType);
}

bool ScriptEngine::loadScript(const std::string& scriptName, const std::string& scriptContent) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    scripts_[scriptName] = scriptContent;
    scriptStatus_[scriptName] = false;
    scriptLogs_[scriptName].clear();
    
    std::cout << "Script loaded: " << scriptName << " (" << scriptContent.size() << " bytes)" << std::endl;
    
    // Validate script
    if (!validateScript(scriptContent)) {
        std::cout << "Warning: Script " << scriptName << " failed validation" << std::endl;
    }
    
    return true;
}

bool ScriptEngine::runScript(const std::string& scriptName, const std::vector<std::string>& arguments) {
    if (scripts_.find(scriptName) == scripts_.end()) {
        std::cout << "Error: Script not found: " << scriptName << std::endl;
        return false;
    }
    
    if (!validateScriptParameters(scriptName, arguments)) {
        return false;
    }
    
    return executeScript(scriptName, arguments);
}

bool ScriptEngine::runScriptFromContent(const std::string& scriptContent, const std::vector<std::string>& arguments) {
    std::string tempName = "temp_" + std::to_string(std::time(nullptr));
    
    if (!loadScript(tempName, scriptContent)) {
        return false;
    }
    
    bool result = executeScript(tempName, arguments);
    
    // Clean up temporary script
    deleteScript(tempName);
    
    return result;
}

bool ScriptEngine::saveScript(const std::string& scriptName, const std::string& scriptContent) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    scripts_[scriptName] = scriptContent;
    
    std::cout << "Script saved: " << scriptName << std::endl;
    return true;
}

bool ScriptEngine::deleteScript(const std::string& scriptName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    scripts_.erase(scriptName);
    scriptStatus_.erase(scriptName);
    scriptLogs_.erase(scriptName);
    
    std::cout << "Script deleted: " << scriptName << std::endl;
    return true;
}

bool ScriptEngine::renameScript(const std::string& oldName, const std::string& newName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (scripts_.find(oldName) == scripts_.end()) {
        return false;
    }
    
    scripts_[newName] = scripts_[oldName];
    scriptStatus_[newName] = scriptStatus_[oldName];
    scriptLogs_[newName] = scriptLogs_[oldName];
    
    scripts_.erase(oldName);
    scriptStatus_.erase(oldName);
    scriptLogs_.erase(oldName);
    
    std::cout << "Script renamed: " << oldName << " -> " << newName << std::endl;
    return true;
}

std::vector<std::string> ScriptEngine::getAvailableScripts() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    for (const auto& pair : scripts_) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool ScriptEngine::analyzeScript(const std::string& scriptName) {
    if (scripts_.find(scriptName) == scripts_.end()) {
        return false;
    }
    
    std::cout << "Analyzing script: " << scriptName << std::endl;
    
    const std::string& scriptContent = scripts_.at(scriptName);
    
    // Analyze script complexity
    std::string complexity = analyzeScriptComplexity(scriptContent);
    std::cout << "  Complexity: " << complexity << std::endl;
    
    // Analyze script dependencies
    std::string dependencies = analyzeScriptDependencies(scriptContent);
    if (!dependencies.empty()) {
        std::cout << "  Dependencies: " << dependencies << std::endl;
    }
    
    // Get script info
    std::string info = getScriptInfo(scriptName);
    std::cout << info << std::endl;
    
    return true;
}

std::string ScriptEngine::getScriptInfo(const std::string& scriptName) const {
    if (scripts_.find(scriptName) == scripts_.end()) {
        return "Script not found: " + scriptName;
    }
    
    const std::string& scriptContent = scripts_.at(scriptName);
    
    std::stringstream ss;
    
    ss << "Script Information:" << std::endl;
    ss << "===================" << std::endl;
    ss << "Name: " << scriptName << std::endl;
    ss << "Size: " << scriptContent.size() << " bytes" << std::endl;
    ss << "Status: " << (scriptStatus_.at(scriptName) ? "Running" : "Stopped") << std::endl;
    ss << "Log Entries: " << scriptLogs_.at(scriptName).size() << std::endl;
    
    // Determine script type
    std::string scriptType;
    if (scriptContent.find("#") == 0 || scriptContent.find("#!/bin/bash") != std::string::npos) {
        scriptType = "Bash Script";
    } else if (scriptContent.find("import ") != std::string::npos || scriptContent.find("def ") != std::string::npos) {
        scriptType = "Python Script";
    } else if (scriptContent.find("{") != std::string::npos && scriptContent.find("}") != std::string::npos) {
        scriptType = "JSON Script";
    } else {
        scriptType = "Generic Script";
    }
    
    ss << "Type: " << scriptType << std::endl;
    
    return ss.str();
}

bool ScriptEngine::integrateWithNLM(const std::string& scriptName) {
    std::cout << "Integrating script: " << scriptName << " with NLM" << std::endl;
    
    if (!setupNLMEnvironment(scriptName)) {
        return false;
    }
    
    if (!integrateWithConfigSystem(scriptName)) {
        return false;
    }
    
    if (!integrateWithPerformanceSystem(scriptName)) {
        return false;
    }
    
    std::cout << "Script integration completed" << std::endl;
    return true;
}

bool ScriptEngine::setupScriptEnvironment(const std::string& scriptName) {
    std::cout << "Setting up environment for script: " << scriptName << std::endl;
    
    // Create script-specific environment variables
    // Setup NLM integration
    // Initialize script context
    
    return true;
}

bool ScriptEngine::enableScriptMonitoring() {
    scriptMonitoringEnabled_ = true;
    std::cout << "Script monitoring enabled" << std::endl;
    return true;
}

bool ScriptEngine::disableScriptMonitoring() {
    scriptMonitoringEnabled_ = false;
    std::cout << "Script monitoring disabled" << std::endl;
    return true;
}

bool ScriptEngine::enableScriptSecurity() {
    scriptSecurityEnabled_ = true;
    std::cout << "Script security enabled" << std::endl;
    return true;
}

bool ScriptEngine::disableScriptSecurity() {
    scriptSecurityEnabled_ = false;
    std::cout << "Script security disabled" << std::endl;
    return true;
}

bool ScriptEngine::isScriptRunning(const std::string& scriptName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (scriptStatus_.find(scriptName) == scriptStatus_.end()) {
        return false;
    }
    
    return scriptStatus_.at(scriptName);
}

std::string ScriptEngine::getScriptStatus(const std::string& scriptName) const {
    if (scriptStatus_.find(scriptName) == scriptStatus_.end()) {
        return "Unknown";
    }
    
    std::stringstream ss;
    
    ss << "Status: " << (scriptStatus_.at(scriptName) ? "Running" : "Stopped") << std::endl;
    ss << "Log Entries: " << scriptLogs_.at(scriptName).size() << std::endl;
    
    return ss.str();
}

// Internal helper methods

bool ScriptEngine::executeScript(const std::string& scriptName, const std::vector<std::string>& arguments) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    const std::string& scriptContent = scripts_.at(scriptName);
    
    std::cout << "Executing script: " << scriptName << std::endl;
    
    // Determine script type and execute accordingly
    if (scriptContent.find("#") == 0 || scriptContent.find("#!/bin/bash") != std::string::npos) {
        std::cout << "  Executing as Bash script" << std::endl;
        // Execute bash script
    } else if (scriptContent.find("import ") != std::string::npos || scriptContent.find("def ") != std::string::npos) {
        std::cout << "  Executing as Python script" << std::endl;
        // Execute Python script
    } else if (scriptContent.find("{") != std::string::npos && scriptContent.find("}") != std::string::npos) {
        std::cout << "  Executing as JSON script" << std::endl;
        // Execute JSON script
    } else {
        std::cout << "  Executing as generic script" << std::endl;
        // Execute generic script
    }
    
    // Log script execution
    logScriptEvent(scriptName, "execute", "Script executed with " + std::to_string(arguments.size()) + " arguments");
    
    // Update script status
    updateScriptStatus(scriptName, true);
    
    // Simulate script execution
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    updateScriptStatus(scriptName, false);
    
    std::cout << "Script execution completed" << std::endl;
    
    return true;
}

bool ScriptEngine::parseNLMCommands(const std::string& scriptContent) {
    // Parse NLM-specific commands
    std::regex nlmCommandRegex(R"(nlm:|NLM:)");
    
    if (std::regex_search(scriptContent, nlmCommandRegex)) {
        std::cout << "NLM commands detected" << std::endl;
        return true;
    }
    
    return false;
}

bool ScriptEngine::parsePythonScript(const std::string& scriptContent) {
    // Parse Python script
    if (scriptContent.find("import ") != std::string::npos) {
        std::cout << "Python imports detected" << std::endl;
    }
    
    return true;
}

bool ScriptEngine::parseBashScript(const std::string& scriptContent) {
    // Parse Bash script
    if (scriptContent.find("#!/bin/bash") != std::string::npos) {
        std::cout << "Bash script detected" << std::endl;
    }
    
    return true;
}

bool ScriptEngine::parseJSONScript(const std::string& scriptContent) {
    // Parse JSON script
    try {
        nlohmann::json::parse(scriptContent);
        std::cout << "Valid JSON script" << std::endl;
        return true;
    } catch (...) {
        return false;
    }
}

bool ScriptEngine::validateScriptSyntax(const std::string& scriptContent, const std::string& scriptType) {
    if (scriptType == "bash") {
        // Basic bash syntax validation
        return scriptContent.find("#") == 0;
    } else if (scriptType == "python") {
        // Basic Python syntax validation
        return scriptContent.find("def ") != std::string::npos || 
               scriptContent.find("import ") != std::string::npos;
    } else if (scriptType == "json") {
        // JSON validation
        try {
            nlohmann::json::parse(scriptContent);
            return true;
        } catch (...) {
            return false;
        }
    } else {
        return !scriptContent.empty();
    }
}

bool ScriptEngine::validateScriptParameters(const std::string& scriptName, const std::vector<std::string>& arguments) {
    std::cout << "Validating script parameters for: " << scriptName << std::endl;
    
    // For now, accept any parameters
    return true;
}

bool ScriptEngine::validateScriptSecurity(const std::string& scriptContent) {
    if (!scriptSecurityEnabled_) {
        return true;
    }
    
    // Security validation
    if (scriptContent.find("rm -rf") != std::string::npos || 
        scriptContent.find("eval ") != std::string::npos) {
        std::cout << "Warning: Potentially dangerous script content detected" << std::endl;
        return false;
    }
    
    return true;
}

bool ScriptEngine::setupNLMEnvironment(const std::string& scriptName) {
    std::cout << "Setting up NLM environment for script: " << scriptName << std::endl;
    
    // Initialize NLM integration
    // Setup environment variables
    // Configure script context
    
    return true;
}

bool ScriptEngine::integrateWithConfigSystem(const std::string& scriptName) {
    std::cout << "Integrating script with NLM configuration system" << std::endl;
    
    // Integrate with configuration management
    // Setup configuration access
    
    return true;
}

bool ScriptEngine::integrateWithPerformanceSystem(const std::string& scriptName) {
    std::cout << "Integrating script with NLM performance system" << std::endl;
    
    // Integrate with performance monitoring
    // Setup performance profiling
    
    return true;
}

void ScriptEngine::logScriptEvent(const std::string& scriptName, const std::string& event, const std::string& message) {
    if (!scriptMonitoringEnabled_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    ss << "[" << timestamp << "] " << event << ": " << message;
    
    scriptLogs_[scriptName].push_back(ss.str());
}

void ScriptEngine::updateScriptStatus(const std::string& scriptName, bool running) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    scriptStatus_[scriptName] = running;
}

bool ScriptEngine::validateScriptPermissions(const std::string& scriptName) {
    std::cout << "Validating permissions for script: " << scriptName << std::endl;
    
    // For now, assume permissions are valid
    return true;
}

bool ScriptEngine::validateScriptContent(const std::string& scriptContent, const std::string& scriptType) {
    return validateScriptSyntax(scriptContent, scriptType);
}

std::string ScriptEngine::analyzeScriptComplexity(const std::string& scriptContent) const {
    // Simple complexity analysis
    int lineCount = std::count(scriptContent.begin(), scriptContent.end(), '\n');
    int functionCount = std::count(scriptContent.begin(), scriptContent.end(), '{'); // Simplified
    
    if (lineCount < 10 && functionCount < 2) {
        return "Simple";
    } else if (lineCount < 50 && functionCount < 5) {
        return "Moderate";
    } else {
        return "Complex";
    }
}

std::string ScriptEngine::analyzeScriptDependencies(const std::string& scriptContent) const {
    // Check for imports or dependencies
    if (scriptContent.find("import ") != std::string::npos) {
        return "python_modules";
    } else if (scriptContent.find("require") != std::string::npos) {
        return "npm_modules";
    } else {
        return "";
    }
}

std::string ScriptEngine::analyzeScriptPerformance(const std::string& scriptName) const {
    std::stringstream ss;
    
    ss << "Performance analysis for " << scriptName << ":" << std::endl;
    ss << "Execution time: N/A (mock)" << std::endl;
    ss << "Memory usage: N/A (mock)" << std::endl;
    ss << "CPU usage: N/A (mock)" << std::endl;
    
    return ss.str();
}

std::string ScriptEngine::generateScriptTemplate(const std::string& scriptType) const {
    if (scriptType == "bash") {
        return "#!/bin/bash\n# NLM Script Template\n\necho \"Hello from NLM Script\"\n\n# Add your commands here\n";
    } else if (scriptType == "python") {
        return "#!/usr/bin/env python3\n# NLM Script Template\n\nimport sys\n\nprint(\"Hello from NLM Script\")\n\n# Add your code here\n";
    } else {
        return "# NLM Script Template\n# Add your commands here\n";
    }
}

std::string ScriptEngine::getScriptDocumentation(const std::string& scriptName) const {
    return "Documentation for " + scriptName + " would be displayed here.";
}

bool ScriptEngine::exportScriptDocumentation(const std::string& scriptName, const std::string& outputFile) const {
    std::string doc = getScriptDocumentation(scriptName);
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << doc;
    
    return true;
}

} // namespace scripting
} // namespace nlm
