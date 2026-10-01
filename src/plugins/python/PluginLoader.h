// PluginLoader.h
// Python interface for plugin management

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PluginInterface.h"

namespace nlm {

    // Python plugin loader interface
    class PluginLoader {
    public:
        PluginLoader();
        ~PluginLoader();

        // Plugin loading from Python
        bool loadPythonPlugin(const std::string& moduleName);
        bool unloadPythonPlugin(const std::string& moduleName);
        std::shared_ptr<PluginInterface> getPythonPlugin(const std::string& moduleName) const;
        std::vector<std::string> getLoadedPythonPlugins() const;
        
        // Plugin registration from Python
        template<typename PluginType>
        bool registerPythonPlugin(const std::string& moduleName, const std::string& version = "1.0.0",
                                 const std::string& description = "",
                                 const std::string& author = "",
                                 const std::vector<std::string>& dependencies = {});
        
        // Plugin discovery from Python
        std::vector<std::string> discoverPythonPlugins(const std::string& directory) const;
        bool isPluginRegistered(const std::string& moduleName) const;
        
        // Plugin configuration and initialization
        bool configurePythonPlugin(const std::string& moduleName,
                                  const std::map<std::string, std::any>& config);
        bool initializePythonPlugin(const std::string& moduleName);
        bool startPythonPlugin(const std::string& moduleName);
        bool stopPythonPlugin(const std::string& moduleName);
        bool unloadPythonPluginCompletely(const std::string& moduleName);
        
        // Plugin information
        const PluginMetadata& getPythonPluginMetadata(const std::string& moduleName) const;
        std::map<std::string, PluginState> getAllPythonPluginStates() const;
        
        // Plugin interface access
        std::shared_ptr<NeuralComponentPlugin> getNeuralComponentPlugin(const std::string& moduleName) const;
        std::shared_ptr<MemoryPlugin> getMemoryPlugin(const std::string& moduleName) const;
        std::shared_ptr<ProcessingPlugin> getProcessingPlugin(const std::string& moduleName) const;
        
        // Python decorator for plugin registration
        template<typename T>
        class PluginRegistration {
        public:
            PluginRegistration(const std::string& name, const std::string& version,
                             const std::string& description, const std::string& author,
                             const std::vector<std::string>& dependencies = {}) {
                // Register plugin when the object is created
                loader.registerPythonPlugin<T>(name, version, description, author, dependencies);
            }
        };
        
        // Python-specific utilities
        static std::string getPluginDirectory();
        static std::vector<std::string> getPluginDirectories();
        static bool setPluginDirectory(const std::string& directory);
        
        // Plugin event handling
        void addPythonEventListener(const std::string& eventType,
                                   std::function<void(const std::string&, const pybind11::dict&) > listener);
        void removePythonEventListener(const std::string& eventType,
                                      const std::function<void(const std::string&, const pybind11::dict&)>& listener);
        
        // Plugin introspection
        pybind11::dict getPluginInfo(const std::string& moduleName) const;
        pybind11::list getPluginDependencies(const std::string& moduleName) const;
        pybind11::list getPluginCapabilities(const std::string& moduleName) const;
        pybind11::dict getPluginConfig(const std::string& moduleName) const;
        
        // Plugin validation
        bool validatePythonPlugin(const std::string& moduleName) const;
        bool validatePluginSecurity(const std::string& moduleName) const;
        
        // Plugin system management
        void cleanupAllPythonPlugins();
        void shutdown();
        bool isInitialized() const;
        size_t getLoadedPluginCount() const;
        
        // Plugin factory methods
        template<typename T, typename... Args>
        std::shared_ptr<T> createPluginInstance(const std::string& moduleName, Args&&... args);
        
        // Plugin decorator definition
        #define plugin_register(name, version, description, author, dependencies) \
            nlm::PluginLoader::PluginRegistration<name> name##_plugin_reg(name, version, description, author, dependencies)

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        // Private helper methods
        bool loadPythonModule(const std::string& moduleName);
        bool unloadPythonModule(const std::string& moduleName);
        bool validatePythonPluginModule(const pybind11::module_& module) const;
        pybind11::module_ getModule(const std::string& moduleName) const;
        std::string normalizeModuleName(const std::string& name) const;
        bool isBuiltinPythonModule(const std::string& name) const;
        std::string getModuleFilePath(const std::string& moduleName) const;
        pybind11::dict moduleToPluginInfo(const pybind11::module_& module) const;
        pybind11::list moduleToPluginCapabilities(const pybind11::module_& module) const;
        pybind11::list moduleToPluginDependencies(const pybind11::module_& module) const;
        void extractPluginMetadata(const pybind11::module_& module, PluginMetadata& metadata) const;
        void extractPluginConfig(const pybind11::module_& module, std::map<std::string, std::any>& config) const;
        bool createPluginEntry(const std::string& moduleName, const pybind11::module_& module);
        bool cleanupPluginEntry(const std::string& moduleName);
        void updatePluginState(const std::string& moduleName, PluginState state);
        void triggerEvent(const std::string& eventType, const std::string& pluginName, const pybind11::dict& data = nullptr);
        bool checkPluginConflicts(const std::string& moduleName) const;
        void logPythonPluginEvent(const std::string& eventType, const std::string& pluginName, const pybind11::dict& data = nullptr);
        bool setupPythonPluginEnvironment(const std::string& moduleName);
        bool cleanupPythonPluginEnvironment(const std::string& moduleName);
        pybind11::dict anyToPyDict(const std::map<std::string, std::any>& config) const;
        std::map<std::string, std::any> pyDictToAny(const pybind11::dict& pyDict) const;
    };

    // Python plugin wrapper for C++ plugins
    class PythonPluginWrapper : public PluginInterface {
    public:
        PythonPluginWrapper(const std::string& moduleName, const pybind11::module_& module);
        ~PythonPluginWrapper() override;

        const PluginMetadata& getMetadata() const override;
        PluginState getState() const override;

        bool load() override;
        bool unload() override;
        bool configure(const std::map<std::string, std::any>& config) override;
        bool initialize() override;
        bool start() override;
        bool stop() override;

        std::vector<std::string> getRequiredInterfaces() const override;
        std::vector<std::string> getProvidedInterfaces() const override;

        void onEvent(const std::string& eventType, const std::any& eventData) override;
        void onPluginLoaded(std::shared_ptr<PluginInterface> plugin) override;
        void onPluginUnloaded(std::shared_ptr<PluginInterface> plugin) override;

        bool validate() override;
        std::string getError() const override;
        void reset() override;

        // Python-specific methods
        pybind11::dict getPluginInfo() const;
        pybind11::list getPluginDependencies() const;
        pybind11::list getPluginCapabilities() const;
        pybind11::dict getPluginConfig() const;

        // Neural component plugin interface
        bool registerComponent(const std::string& name, const std::any& component) override;
        bool unregisterComponent(const std::string& name) override;
        std::any getComponent(const std::string& name) const override;
        std::vector<std::string> getComponentNames() const override;
        bool hasComponent(const std::string& name) const override;

        // Memory plugin interface
        bool store(const std::string& key, const std::any& value, int ttl = -1) override;
        bool retrieve(const std::string& key, std::any& value) const override;
        bool remove(const std::string& key) override;
        bool clear() override;
        bool exists(const std::string& key) const override;
        size_t size() const override;
        std::vector<std::string> getKeys() const override;
        bool cleanup() override;
        bool compact() override;

        // Processing plugin interface
        bool process(const std::any& input, std::any& output) override;
        bool preProcess(const std::any& input) override;
        bool postProcess(const std::any& output) override;
        bool setInputSchema(const std::string& schema) override;
        bool setOutputSchema(const std::string& schema) override;
        std::string getInputSchema() const override;
        std::string getOutputSchema() const override;
        bool getStats(std::map<std::string, std::any>& stats) const override;
        void resetStats() override;

    private:
        std::string moduleName_;
        pybind11::module_ module_;
        PluginMetadata metadata_;
        PluginState state_;
        std::string error_;
        std::map<std::string, std::any> config_;
        bool validatePythonPlugin() const;
    };

} // namespace nlm

// Plugin registration decorator for Python
#define PLUGIN_REGISTER(name, version, description, author, dependencies) \
    nlm::PluginLoader::PluginRegistration<name> name##_plugin_reg(name, version, description, author, dependencies)

// Plugin factory template
template<typename T>
inline bool nlm::PluginLoader::registerPythonPlugin(const std::string& moduleName, const std::string& version,
                                                  const std::string& description, const std::string& author,
                                                  const std::vector<std::string>& dependencies) {
    // Implementation would go here
    return true;
}

// Plugin instance factory template
template<typename T, typename... Args>
inline std::shared_ptr<T> nlm::PluginLoader::createPluginInstance(const std::string& moduleName, Args&&... args) {
    // Implementation would go here
    return nullptr;
}