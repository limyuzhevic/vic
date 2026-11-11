#include "ScriptingEngine.hpp"
#include "CoreBrainController.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>

namespace nlm {

bool ScriptingEngine::initialized = false;
std::string ScriptingEngine::enginePath = "";
ScriptType ScriptingEngine::defaultScriptType = ScriptType::LUA;
bool ScriptingEngine::sandboxMode = true;
double ScriptingEngine::executionTimeout = 30.0; // 30 seconds

std::map<std::string, std::string> ScriptingEngine::scriptCache;
std::map<std::string, ScriptFunction> ScriptingEngine::registeredFunctions;
std::vector<std::string> ScriptingEngine::loadedScripts;

ScriptingEngine::ScriptingEngine() {}

ScriptingEngine::~ScriptingEngine() {
    shutdown();
}

bool ScriptingEngine::initialize() {
    if (initialized) {
        return true;
    }
    
    std::cout << "Initializing scripting engine..." << std::endl;
    
    // Initialize default scripting engines
    if (!initializeLua()) {
        std::cout << "Warning: Lua engine not available" << std::endl;
    }
    
    if (!initializePython()) {
        std::cout << "Warning: Python engine not available" << std::endl;
    }
    
    if (!initializeJavaScript()) {
        std::cout << "Warning: JavaScript engine not available" << std::endl;
    }
    
    // Set up default script directory
    if (enginePath.empty()) {
        enginePath = "./scripts";
    }
    
    // Create scripts directory if it doesn't exist
    std::filesystem::create_directories(enginePath);
    
    initialized = true;
    
    std::cout << "Scripting engine initialized successfully" << std::endl;
    return true;
}

void ScriptingEngine::shutdown() {
    if (!initialized) {
        return;
    }
    
    std::cout << "Shutting down scripting engine..." << std::endl;
    
    cleanupScripts();
    
    scriptCache.clear();
    registeredFunctions.clear();
    loadedScripts.clear();
    
    initialized = false;
    
    std::cout << "Scripting engine shutdown complete" << std::endl;
}

bool ScriptingEngine::initializeLua() {
    // In a real implementation, this would initialize the Lua engine
    // For this demo, we'll simulate it being available
    std::cout << "Lua engine initialized" << std::endl;
    return true;
}

bool ScriptingEngine::initializePython() {
    // In a real implementation, this would initialize the Python engine
    // For this demo, we'll simulate it being available
    std::cout << "Python engine initialized" << std::endl;
    return true;
}

bool ScriptingEngine::initializeJavaScript() {
    // In a real implementation, this would initialize the JavaScript engine
    // For this demo, we'll simulate it being available
    std::cout << "JavaScript engine initialized" << std::endl;
    return true;
}

ScriptResult ScriptingEngine::executeScript(const std::string& scriptPath, CoreBrainController& controller) {
    if (!initialized) {
        return ScriptResult{false, "", "Scripting engine not initialized", 0.0, std::chrono::high_resolution_clock::now()};
    }
    
    std::string path = sanitizeScriptPath(scriptPath);
    
    // Check if script is in cache
    auto cacheIt = scriptCache.find(path);
    if (cacheIt != scriptCache.end()) {
        std::cout << "Executing cached script: " << path << std::endl;
        return executeScript(cacheIt->second, "lua");
    }
    
    // Load script from file
    std::ifstream file(path);
    if (!file.is_open()) {
        return ScriptResult{false, "", "Failed to open script file: " + path, 0.0, std::chrono::high_resolution_clock::now()};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string script = buffer.str();
    file.close();
    
    // Cache the script
    scriptCache[path] = script;
    
    return executeScript(script, "lua");
}

ScriptResult ScriptingEngine::executeScript(const std::string& script, const std::string& language) {
    if (!initialized) {
        return ScriptResult{false, "", "Scripting engine not initialized", 0.0, std::chrono::high_resolution_clock::now()};
    }
    
    ScriptType scriptType;
    if (language == "lua") scriptType = ScriptType::LUA;
    else if (language == "python") scriptType = ScriptType::PYTHON;
    else if (language == "javascript") scriptType = ScriptType::JAVASCRIPT;
    else {
        scriptType = defaultScriptType;
    }
    
    // Validate script
    if (!validateScript(script, scriptType)) {
        return ScriptResult{false, "", "Script validation failed", 0.0, std::chrono::high_resolution_clock::now()};
    }
    
    // Record execution start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    ScriptResult result;
    result.timestamp = startTime;
    
    try {
        // Execute script based on type
        if (scriptType == ScriptType::LUA) {
            result = executeLuaScript(script, *reinterpret_cast<CoreBrainController*>(nullptr)); // Placeholder
        } else if (scriptType == ScriptType::PYTHON) {
            result = executePythonScript(script, *reinterpret_cast<CoreBrainController*>(nullptr)); // Placeholder
        } else if (scriptType == ScriptType::JAVASCRIPT) {
            result = executeJavaScriptScript(script, *reinterpret_cast<CoreBrainController*>(nullptr)); // Placeholder
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.error = "Exception during script execution: " + std::string(e.what());
    }
    
    // Calculate execution time
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Check timeout
    if (result.executionTime > executionTimeout) {
        result.success = false;
        result.error = "Script execution timed out after " + std::to_string(executionTimeout) + " seconds";
    }
    
    return result;
}

ScriptResult ScriptingEngine::executeLuaScript(const std::string& script, CoreBrainController& controller) {
    std::cout << "Executing Lua script..." << std::endl;
    
    // In a real implementation, this would execute the script using the Lua engine
    // For this demo, we'll simulate execution
    
    // Simple simulation: check for brain-related commands
    if (script.find("brain") != std::string::npos || script.find("Brain") != std::string::npos) {
        std::cout << "Brain-related script detected" << std::endl;
        return ScriptResult{true, "Script executed successfully\nBrain operations simulated", "", 0.5, std::chrono::high_resolution_clock::now()};
    }
    
    return ScriptResult{true, "Script executed successfully\nLua simulation complete", "", 0.1, std::chrono::high_resolution_clock::now()};
}

ScriptResult ScriptingEngine::executePythonScript(const std::string& script, CoreBrainController& controller) {
    std::cout << "Executing Python script..." << std::endl;
    
    // In a real implementation, this would execute the script using the Python engine
    // For this demo, we'll simulate execution
    
    // Simple simulation: check for brain-related commands
    if (script.find("brain") != std::string::npos || script.find("Brain") != std::string::npos) {
        std::cout << "Brain-related script detected" << std::endl;
        return ScriptResult{true, "Script executed successfully\nBrain operations simulated", "", 0.5, std::chrono::high_resolution_clock::now()};
    }
    
    return ScriptResult{true, "Script executed successfully\nPython simulation complete", "", 0.1, std::chrono::high_resolution_clock::now()};
}

ScriptResult ScriptingEngine::executeJavaScriptScript(const std::string& script, CoreBrainController& controller) {
    std::cout << "Executing JavaScript script..." << std::endl;
    
    // In a real implementation, this would execute the script using the JavaScript engine
    // For this demo, we'll simulate execution
    
    // Simple simulation: check for brain-related commands
    if (script.find("brain") != std::string::npos || script.find("Brain") != std::string::npos) {
        std::cout << "Brain-related script detected" << std::endl;
        return ScriptResult{true, "Script executed successfully\nBrain operations simulated", "", 0.5, std::chrono::high_resolution_clock::now()};
    }
    
    return ScriptResult{true, "Script executed successfully\nJavaScript simulation complete", "", 0.1, std::chrono::high_resolution_clock::now()};
}

void ScriptingEngine::registerFunction(const ScriptFunction& func) {
    registeredFunctions[func.name] = func;
    std::cout << "Registered script function: " << func.name << std::endl;
}

bool ScriptingEngine::unregisterFunction(const std::string& name) {
    auto it = registeredFunctions.find(name);
    if (it != registeredFunctions.end()) {
        registeredFunctions.erase(it);
        std::cout << "Unregistered script function: " << name << std::endl;
        return true;
    }
    return false;
}

bool ScriptingEngine::loadScript(const std::string& path) {
    std::string sanitizedPath = sanitizeScriptPath(path);
    
    if (isScriptLoaded(sanitizedPath)) {
        std::cout << "Script already loaded: " << sanitizedPath << std::endl;
        return true;
    }
    
    std::ifstream file(sanitizedPath);
    if (!file.is_open()) {
        std::cout << "Failed to load script: " << sanitizedPath << std::endl;
        return false;
    }
    
    // Check file extension
    std::string extension = sanitizedPath.substr(sanitizedPath.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Load script based on extension
    ScriptResult result;
    if (extension == "lua") {
        std::stringstream buffer;
        buffer << file.rdbuf();
        scriptCache[sanitizedPath] = buffer.str();
        std::cout << "Loaded Lua script: " << sanitizedPath << std::endl;
    } else if (extension == "py") {
        std::stringstream buffer;
        buffer << file.rdbuf();
        scriptCache[sanitizedPath] = buffer.str();
        std::cout << "Loaded Python script: " << sanitizedPath << std::endl;
    } else if (extension == "js") {
        std::stringstream buffer;
        buffer << file.rdbuf();
        scriptCache[sanitizedPath] = buffer.str();
        std::cout << "Loaded JavaScript script: " << sanitizedPath << std::endl;
    } else {
        std::cout << "Unsupported script format: " << extension << std::endl;
        return false;
    }
    
    file.close();
    loadedScripts.push_back(sanitizedPath);
    
    return true;
}

std::string ScriptingEngine::getScriptPath(const std::string& name) const {
    for (const auto& script : loadedScripts) {
        std::string filename = std::filesystem::path(script).filename().string();
        if (filename == name) {
            return script;
        }
    }
    return "";
}

void ScriptingEngine::listScripts() {
    std::cout << "=== LOADED SCRIPTS ===" << std::endl;
    
    if (loadedScripts.empty()) {
        std::cout << "No scripts loaded" << std::endl;
        return;
    }
    
    for (const auto& script : loadedScripts) {
        std::string filename = std::filesystem::path(script).filename().string();
        std::cout << "  " << filename << " -> " << script << std::endl;
    }
    
    std::cout << std::endl;
}

bool ScriptingEngine::isScriptLoaded(const std::string& name) const {
    for (const auto& script : loadedScripts) {
        std::string filename = std::filesystem::path(script).filename().string();
        if (filename == name) {
            return true;
        }
    }
    return false;
}

bool ScriptingEngine::removeScript(const std::string& name) {
    for (auto it = loadedScripts.begin(); it != loadedScripts.end(); ++it) {
        std::string filename = std::filesystem::path(*it).filename().string();
        if (filename == name) {
            // Remove from cache
            scriptCache.erase(*it);
            
            // Remove from loaded scripts
            loadedScripts.erase(it);
            
            std::cout << "Removed script: " << name << std::endl;
            return true;
        }
    }
    
    std::cout << "Script not found: " << name << std::endl;
    return false;
}

std::vector<std::string> ScriptingEngine::getLoadedScripts() const {
    return loadedScripts;
}

std::map<std::string, ScriptFunction> ScriptingEngine::getRegisteredFunctions() const {
    return registeredFunctions;
}

void ScriptingEngine::setEnginePath(const std::string& path) {
    enginePath = path;
    std::cout << "Script engine path set to: " << enginePath << std::endl;
}

void ScriptingEngine::setDefaultScriptType(ScriptType type) {
    defaultScriptType = type;
    std::cout << "Default script type set to: " << (type == ScriptType::LUA ? "LUA" :
                                                   type == ScriptType::PYTHON ? "Python" :
                                                   type == ScriptType::JAVASCRIPT ? "JavaScript" : "CUSTOM") << std::endl;
}

void ScriptingEngine::enableSandboxMode(bool enable) {
    sandboxMode = enable;
    std::cout << "Sandbox mode " << (enable ? "enabled" : "disabled") << std::endl;
}

void ScriptingEngine::setTimeout(double seconds) {
    executionTimeout = seconds;
    std::cout << "Script execution timeout set to " << seconds << " seconds" << std::endl;
}

std::string ScriptingEngine::sanitizeScriptPath(const std::string& path) {
    std::string result = path;
    
    // Remove leading ./ if present
    if (result.find("./") == 0) {
        result = result.substr(2);
    }
    
    // Remove any directory traversal attempts
    size_t pos = 0;
    while ((pos = result.find("..", pos)) != std::string::npos) {
        if (pos > 0 && result[pos - 1] == '/') {
            // Remove the .. and preceding directory
            size_t prevPos = result.rfind('/', pos - 1);
            if (prevPos != std::string::npos) {
                result.erase(prevPos, pos - prevPos + 3);
                pos = prevPos;
            } else {
                pos += 3;
            }
        } else {
            pos += 3;
        }
    }
    
    return result;
}

bool ScriptingEngine::validateScript(const std::string& script, ScriptType type) {
    if (!sandboxMode) {
        return true;
    }
    
    // Basic validation checks
    if (script.empty()) {
        return false;
    }
    
    // Check for dangerous commands based on script type
    if (type == ScriptType::LUA) {
        // Lua validation
        if (script.find("os.execute") != std::string::npos) {
            return false;
        }
        if (script.find("io.open") != std::string::npos && script.find("readonly") == std::string::npos) {
            return false;
        }
    } else if (type == ScriptType::PYTHON) {
        // Python validation
        if (script.find("import os") != std::string::npos && script.find("import os.system") == std::string::npos) {
            return false;
        }
        if (script.find("import subprocess") != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

ScriptResult ScriptingEngine::executeFunction(const ScriptFunction& func, const std::vector<std::string>& args) {
    try {
        return func.handler(args);
    } catch (const std::exception& e) {
        return ScriptResult{false, "", "Function execution error: " + std::string(e.what()), 0.0, std::chrono::high_resolution_clock::now()};
    }
}

void ScriptingEngine::cleanupScripts() {
    std::cout << "Cleaning up scripts..." << std::endl;
    
    // Save scripts to cache if needed
    for (const auto& script : loadedScripts) {
        std::cout << "Script: " << script << std::endl;
    }
    
    scriptCache.clear();
}

} // namespace nlm