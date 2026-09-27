#ifndef SCRIPTING_INTERFACE_HPP
#define SCRIPTING_INTERFACE_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <functional>
#include <any>
#include <chrono>

namespace nlm {
namespace integration {

// Script language enumeration
enum class ScriptLanguage {
    PYTHON,
    R,
    MATLAB,
    JULIA,
    LUA,
    JAVASCRIPT,
    CUSTOM
};

// Script execution result
struct ScriptExecutionResult {
    std::string script_id;                // Unique script identifier
    std::string script_name;              // Human-readable name
    std::string language;                 // Language used
    std::string result;                   // Output result
    std::string error;                    // Error message (if any)
    double execution_time;                // Time taken to execute
    bool success;                         // Whether execution was successful
    std::map<std::string, std::any> metadata;  // Additional metadata
};

// Script function signature
using ScriptFunction = std::function<std::any(const std::vector<std::any>&)>;

// Script interface class
class ScriptInterface {
public:
    virtual ~ScriptInterface() = default;
    
    // Interface information
    virtual ScriptLanguage getLanguage() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getName() const = 0;
    
    // Script management
    virtual std::string loadScript(const std::string& script_code, 
                                  const std::string& script_name = "") = 0;
    virtual std::string loadScriptFromFile(const std::string& filename) = 0;
    virtual bool unloadScript(const std::string& script_id) = 0;
    virtual bool saveScript(const std::string& script_id, const std::string& filename) = 0;
    
    // Script execution
    virtual ScriptExecutionResult executeScript(const std::string& script_id) = 0;
    virtual ScriptExecutionResult executeScript(const std::string& script_id,
                                               const std::map<std::string, std::any>& variables) = 0;
    virtual std::any executeFunction(const std::string& script_id,
                                   const std::string& function_name,
                                   const std::vector<std::any>& arguments) = 0;
    
    // Variable management
    virtual bool setVariable(const std::string& script_id,
                           const std::string& variable_name,
                           const std::any& value) = 0;
    virtual std::any getVariable(const std::string& script_id,
                               const std::string& variable_name) = 0;
    virtual std::map<std::string, std::any> getAllVariables(const std::string& script_id) = 0;
    virtual bool clearVariables(const std::string& script_id) = 0;
    
    // State management
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual bool shutdown() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    
    // Statistics and monitoring
    virtual std::map<std::string, std::any> getStatistics() const = 0;
    virtual std::string getSystemInfo() const = 0;
    virtual bool isInitialized() const = 0;
};

// Python interface implementation
class PythonInterface : public ScriptInterface {
private:
    std::map<std::string, std::string> scripts_;           // script_id -> script code
    std::map<std::string, std::map<std::string, std::any>> variables_; // script_id -> variables
    std::mutex mutex_;
    bool initialized_;
    std::string python_path_;
    std::map<std::string, std::string> import_paths_;

public:
    PythonInterface() : initialized_(false) {}

    // Interface information
    ScriptLanguage getLanguage() const override { return ScriptLanguage::PYTHON; }
    std::string getVersion() const override { return "3.8.0"; } // In real implementation, would detect actual version
    std::string getName() const override { return "Python"; }
    
    // Script management
    std::string loadScript(const std::string& script_code, const std::string& script_name = "") override;
    std::string loadScriptFromFile(const std::string& filename) override;
    bool unloadScript(const std::string& script_id) override;
    bool saveScript(const std::string& script_id, const std::string& filename) override;
    
    // Script execution
    ScriptExecutionResult executeScript(const std::string& script_id) override;
    ScriptExecutionResult executeScript(const std::string& script_id,
                                       const std::map<std::string, std::any>& variables) override;
    std::any executeFunction(const std::string& script_id,
                           const std::string& function_name,
                           const std::vector<std::any>& arguments) override;
    
    // Variable management
    bool setVariable(const std::string& script_id,
                    const std::string& variable_name,
                    const std::any& value) override;
    std::any getVariable(const std::string& script_id,
                        const std::string& variable_name) override;
    std::map<std::string, std::any> getAllVariables(const std::string& script_id) override;
    bool clearVariables(const std::string& script_id) override;
    
    // State management
    bool initialize(const std::map<std::string, std::string>& config) override;
    bool shutdown() override;
    bool pause() override { return true; }
    bool resume() override { return true; }
    
    // Statistics and monitoring
    std::map<std::string, std::any> getStatistics() const override;
    std::string getSystemInfo() const override;
    bool isInitialized() const override { return initialized_; }
    
    // Python-specific functionality
    std::vector<std::string> getAvailableModules() const;
    bool importModule(const std::string& module_name);
    std::any executePythonCode(const std::string& code, const std::map<std::string, std::any>& variables = {});
};

// R interface implementation
class RInterface : public ScriptInterface {
private:
    std::map<std::string, std::string> scripts_;
    std::map<std::string, std::map<std::string, std::any>> variables_;
    std::mutex mutex_;
    bool initialized_;
    std::string r_path_;
    std::map<std::string, std::string> lib_paths_;

public:
    RInterface() : initialized_(false) {}

    // Interface information
    ScriptLanguage getLanguage() const override { return ScriptLanguage::R; }
    std::string getVersion() const override { return "4.0.0"; } // In real implementation, would detect actual version
    std::string getName() const override { return "R"; }
    
    // Script management
    std::string loadScript(const std::string& script_code, const std::string& script_name = "") override;
    std::string loadScriptFromFile(const std::string& filename) override;
    bool unloadScript(const std::string& script_id) override;
    bool saveScript(const std::string& script_id, const std::string& filename) override;
    
    // Script execution
    ScriptExecutionResult executeScript(const std::string& script_id) override;
    ScriptExecutionResult executeScript(const std::string& script_id,
                                       const std::map<std::string, std::any>& variables) override;
    std::any executeFunction(const std::string& script_id,
                           const std::string& function_name,
                           const std::vector<std::any>& arguments) override;
    
    // Variable management
    bool setVariable(const std::string& script_id,
                    const std::string& variable_name,
                    const std::any& value) override;
    std::any getVariable(const std::string& script_id,
                        const std::string& variable_name) override;
    std::map<std::string, std::any> getAllVariables(const std::string& script_id) override;
    bool clearVariables(const std::string& script_id) override;
    
    // State management
    bool initialize(const std::map<std::string, std::string>& config) override;
    bool shutdown() override;
    bool pause() override { return true; }
    bool resume() override { return true; }
    
    // Statistics and monitoring
    std::map<std::string, std::any> getStatistics() const override;
    std::string getSystemInfo() const override;
    bool isInitialized() const override { return initialized_; }
    
    // R-specific functionality
    std::vector<std::string> getAvailablePackages() const;
    bool installPackage(const std::string& package_name);
    bool loadPackage(const std::string& package_name);
};

// MATLAB interface implementation (simplified)
class MatlabInterface : public ScriptInterface {
private:
    std::map<std::string, std::string> scripts_;
    std::map<std::string, std::map<std::string, std::any>> variables_;
    std::mutex mutex_;
    bool initialized_;

public:
    MatlabInterface() : initialized_(false) {}

    // Interface information
    ScriptLanguage getLanguage() const override { return ScriptLanguage::MATLAB; }
    std::string getVersion() const override { return "9.12"; } // In real implementation, would detect actual version
    std::string getName() const override { return "MATLAB"; }
    
    // Script management
    std::string loadScript(const std::string& script_code, const std::string& script_name = "") override;
    std::string loadScriptFromFile(const std::string& filename) override;
    bool unloadScript(const std::string& script_id) override;
    bool saveScript(const std::string& script_id, const std::string& filename) override;
    
    // Script execution
    ScriptExecutionResult executeScript(const std::string& script_id) override;
    ScriptExecutionResult executeScript(const std::string& script_id,
                                       const std::map<std::string, std::any>& variables) override;
    std::any executeFunction(const std::string& script_id,
                           const std::string& function_name,
                           const std::vector<std::any>& arguments) override;
    
    // Variable management
    bool setVariable(const std::string& script_id,
                    const std::string& variable_name,
                    const std::any& value) override;
    std::any getVariable(const std::string& script_id,
                        const std::string& variable_name) override;
    std::map<std::string, std::any> getAllVariables(const std::string& script_id) override;
    bool clearVariables(const std::string& script_id) override;
    
    // State management
    bool initialize(const std::map<std::string, std::string>& config) override;
    bool shutdown() override;
    bool pause() override { return true; }
    bool resume() override { return true; }
    
    // Statistics and monitoring
    std::map<std::string, std::any> getStatistics() const override;
    std::string getSystemInfo() const override;
    bool isInitialized() const override { return initialized_; }
};

// Scripting engine
class ScriptingEngine {
private:
    std::map<ScriptLanguage, std::unique_ptr<ScriptInterface>> interfaces_;
    std::mutex mutex_;
    bool enabled_;

public:
    ScriptingEngine() : enabled_(false) {}

    // Interface management
    bool registerInterface(ScriptLanguage language, std::unique_ptr<ScriptInterface> interface);
    bool unregisterInterface(ScriptLanguage language);
    ScriptInterface* getInterface(ScriptLanguage language) const;
    
    // Engine control
    bool enable() { enabled_ = true; return true; }
    bool disable() { enabled_ = false; return true; }
    bool isEnabled() const { return enabled_; }
    
    // Script execution
    ScriptExecutionResult executeScript(ScriptLanguage language,
                                       const std::string& script_code,
                                       const std::string& script_name = "",
                                       const std::map<std::string, std::any>& variables = {});
    std::any executeFunction(ScriptLanguage language,
                           const std::string& script_id,
                           const std::string& function_name,
                           const std::vector<std::any>& arguments);
    
    // Script management
    bool saveScript(ScriptLanguage language, const std::string& script_id,
                   const std::string& filename);
    bool loadScript(ScriptLanguage language, const std::string& filename);
    
    // Variable management
    bool setVariable(ScriptLanguage language, const std::string& script_id,
                    const std::string& variable_name, const std::any& value);
    std::any getVariable(ScriptLanguage language, const std::string& script_id,
                        const std::string& variable_name);
    
    // Statistics and monitoring
    std::map<ScriptLanguage, std::map<std::string, std::any>> getAllStatistics() const;
    std::string getSystemInfo() const;
    bool getSystemHealth() const;
    
    // Configuration
    std::map<std::string, std::string> getConfiguration() const;
    bool configure(const std::map<std::string, std::string>& config);
    
    // Persistence and backup
    bool saveScriptState(const std::string& filename) const;
    bool loadScriptState(const std::string& filename);
    
    // Maintenance
    bool initializeAllInterfaces();
    bool shutdownAllInterfaces();
    bool pauseAllInterfaces();
    bool resumeAllInterfaces();
    
    // Script management
    std::vector<std::string> listScripts(ScriptLanguage language) const;
    std::map<std::string, std::string> getScriptInfo(ScriptLanguage language,
                                                    const std::string& script_id) const;
};

} // namespace integration
} // namespace nlm

#endif // SCRIPTING_INTERFACE_HPP