#ifndef BRAIN_SCRIPTING_TOOL_HPP
#define BRAIN_SCRIPTING_TOOL_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace nlm {
namespace cli {

class BrainScriptingTool {
public:
    BrainScriptingTool();
    ~BrainScriptingTool();

    bool LoadScript(const std::string& scriptPath);
    bool ExecuteScript(const std::string& scriptPath);
    bool ExecuteScriptString(const std::string& script);
    bool SaveScript(const std::string& scriptPath, const std::string& content);

    bool RegisterFunction(const std::string& name, std::function<bool(const std::vector<std::string>&)> func);
    bool UnregisterFunction(const std::string& name);
    bool ListFunctions(std::vector<std::string>& functionNames) const;

    bool ExecuteScriptWithArgs(const std::string& scriptPath, const std::vector<std::string>& args);
    bool ExecuteScriptBatch(const std::vector<std::string>& scriptPaths);
    bool CreateScriptTemplate(const std::string& templateName, const std::string& content);
    bool GetScriptTemplate(const std::string& templateName, std::string& content) const;

    bool SetScriptVariables(const std::map<std::string, std::string>& variables);
    std::string GetScriptVariable(const std::string& variable) const;
    std::map<std::string, std::string> GetAllScriptVariables() const { return scriptVariables; }

protected:
    std::map<std::string, std::function<bool(const std::vector<std::string>&)>> functionRegistry;
    std::map<std::string, std::string> scriptVariables;
    std::vector<std::string> loadedScripts;
};

} // namespace cli
} // namespace nlm

#endif // BRAIN_SCRIPTING_TOOL_HPP