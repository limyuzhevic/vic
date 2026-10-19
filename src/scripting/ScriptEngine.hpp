// Script Engine
// This class provides scripting and automation capabilities for NLM

#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

namespace nlm {
namespace scripting {

class ScriptEngine {
public:
    ScriptEngine();
    ~ScriptEngine();
    
    // Script parsing and validation
    bool parseScript(const std::string& scriptFile);
    bool validateScript(const std::string& scriptContent);
    bool loadScript(const std::string& scriptName, const std::string& scriptContent);
    
    // Script execution
    bool runScript(const std::string& scriptName, const std::vector<std::string>& arguments = {});
    bool runScriptFromContent(const std::string& scriptContent, const std::vector<std::string>& arguments = {});
    
    // Script management
    bool saveScript(const std::string& scriptName, const std::string& scriptContent);
    bool deleteScript(const std::string& scriptName);
    bool renameScript(const std::string& oldName, const std::string& newName);
    std::vector<std::string> getAvailableScripts() const;
    
    // Script analysis
    bool analyzeScript(const std::string& scriptName);
    std::string getScriptInfo(const std::string& scriptName) const;
    
    // Script integration
    bool integrateWithNLM(const std::string& scriptName);
    bool setupScriptEnvironment(const std::string& scriptName);
    
    // Script monitoring
    bool enableScriptMonitoring();
    bool disableScriptMonitoring();
    bool isMonitoringEnabled() const { return scriptMonitoringEnabled_; }
    
    // Script security
    bool enableScriptSecurity();
    bool disableScriptSecurity();
    bool isSecurityEnabled() const { return scriptSecurityEnabled_; }
    
    // Script execution status
    bool isScriptRunning(const std::string& scriptName) const;
    std::string getScriptStatus(const std::string& scriptName) const;
    
private:
    // Internal script storage
    std::map<std::string, std::string> scripts_;
    std::map<std::string, bool> scriptStatus_;
    std::map<std::string, std::vector<std::string>> scriptLogs_;
    
    bool scriptMonitoringEnabled_;
    bool scriptSecurityEnabled_;
    
    mutable std::mutex mutex_;
    
    // Script execution
    bool executeScript(const std::string& scriptName, const std::vector<std::string>& arguments);
    
    // Script parsing
    bool parseNLMCommands(const std::string& scriptContent);
    bool parsePythonScript(const std::string& scriptContent);
    bool parseBashScript(const std::string& scriptContent);
    bool parseJSONScript(const std::string& scriptContent);
    
    // Script validation
    bool validateScriptSyntax(const std::string& scriptContent, const std::string& scriptType);
    bool validateScriptParameters(const std::string& scriptName, const std::vector<std::string>& arguments);
    bool validateScriptSecurity(const std::string& scriptContent);
    
    // Script integration
    bool setupNLMEnvironment(const std::string& scriptName);
    bool integrateWithConfigSystem(const std::string& scriptName);
    bool integrateWithPerformanceSystem(const std::string& scriptName);
    
    // Script monitoring
    void logScriptEvent(const std::string& scriptName, const std::string& event, const std::string& message);
    void updateScriptStatus(const std::string& scriptName, bool running);
    
    // Script security
    bool validateScriptPermissions(const std::string& scriptName);
    bool validateScriptContent(const std::string& scriptContent, const std::string& scriptType);
    
    // Script analysis
    std::string analyzeScriptComplexity(const std::string& scriptContent) const;
    std::string analyzeScriptDependencies(const std::string& scriptContent) const;
    std::string analyzeScriptPerformance(const std::string& scriptName) const;
    
    // Script utilities
    std::string generateScriptTemplate(const std::string& scriptType) const;
    std::string getScriptDocumentation(const std::string& scriptName) const;
    bool exportScriptDocumentation(const std::string& scriptName, const std::string& outputFile) const;
};

} // namespace scripting
} // namespace nlm
