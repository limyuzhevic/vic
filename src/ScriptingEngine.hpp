#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

namespace nlm {

class CoreBrainController;
class DebugConsole;

// Scripting engine types
enum class ScriptType {
    LUA, PYTHON, JAVASCRIPT, CUSTOM
};

struct ScriptResult {
    bool success;
    std::string output;
    std::string error;
    double executionTime;
    std::chrono::high_resolution_clock::time_point timestamp;
};

struct ScriptFunction {
    std::string name;
    std::string description;
    ScriptType type;
    std::function<ScriptResult(const std::vector<std::string>&)> handler;
};

class ScriptingEngine {
public:
    ScriptingEngine();
    ~ScriptingEngine();
    
    // Initialize scripting engine
    static bool initialize();
    static void shutdown();
    
    // Execute script
    static ScriptResult executeScript(const std::string& scriptPath, CoreBrainController& controller);
    static ScriptResult executeScript(const std::string& script, const std::string& language = "lua");
    
    // Register script functions
    static void registerFunction(const ScriptFunction& func);
    static bool unregisterFunction(const std::string& name);
    
    // Script file operations
    static bool loadScript(const std::string& path);
    static std::string getScriptPath(const std::string& name) const;
    
    // Script management
    static void listScripts();
    static bool isScriptLoaded(const std::string& name) const;
    static bool removeScript(const std::string& name);
    
    // Engine state
    static bool isInitialized() { return initialized; }
    static std::vector<std::string> getLoadedScripts() const;
    static std::map<std::string, ScriptFunction> getRegisteredFunctions() const;
    
    // Configuration
    static void setEnginePath(const std::string& path);
    static void setDefaultScriptType(ScriptType type);
    static void enableSandboxMode(bool enable);
    static void setTimeout(double seconds);
    
private:
    static bool initialized;
    static std::string enginePath;
    static ScriptType defaultScriptType;
    static bool sandboxMode;
    static double executionTimeout;
    
    static std::map<std::string, std::string> scriptCache;
    static std::map<std::string, ScriptFunction> registeredFunctions;
    static std::vector<std::string> loadedScripts;
    
    // Internal methods
    static bool initializeLua();
    static bool initializePython();
    static bool initializeJavaScript();
    static void cleanupScripts();
    static ScriptResult executeLuaScript(const std::string& script, CoreBrainController& controller);
    static ScriptResult executePythonScript(const std::string& script, CoreBrainController& controller);
    static ScriptResult executeJavaScriptScript(const std::string& script, CoreBrainController& controller);
    static std::string sanitizeScriptPath(const std::string& path);
    static bool validateScript(const std::string& script, ScriptType type);
    static ScriptResult executeFunction(const ScriptFunction& func, const std::vector<std::string>& args);
};

} // namespace nlm