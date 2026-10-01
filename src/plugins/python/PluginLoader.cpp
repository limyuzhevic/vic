// PluginLoader.cpp
// Python interface for plugin management implementation

#include "PluginLoader.h"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <filesystem>

namespace nlm {

    // Implementation details for PluginLoader
    struct PluginLoader::Impl {
        std::map<std::string, std::shared_ptr<PythonPluginWrapper>> pythonPlugins;
        std::map<std::string, pybind11::module_> pythonModules;
        std::map<std::string, PluginMetadata> pluginMetadata;
        std::map<std::string, std::map<std::string, std::any>> pluginConfigs;
        std::map<std::string, PluginState> pluginStates;
        std::map<std::string, std::vector<std::function<void(const std::string&, const pybind11::dict&)>>> eventListeners;
        std::vector<std::string> pluginDirectories;
        std::string defaultPluginDirectory;
        bool initialized = false;
        std::mutex mutex;
    };

    PluginLoader::PluginLoader() : pImpl(std::make_unique<Impl>()) {
        // Initialize Python interpreter if not already initialized
        pybind11::initialize_interpreter();
        
        // Set up default plugin directory
        pImpl->defaultPluginDirectory = "plugins";
        pImpl->pluginDirectories.push_back(pImpl->defaultPluginDirectory);
        
        // Try to get Python path from environment
        const char* pythonPath = std::getenv("PYTHONPATH");
        if (pythonPath) {
            std::istringstream iss(pythonPath);
            std::string path;
            while (std::getline(iss, path, ':')) {
                pImpl->pluginDirectories.push_back(path);
            }
        }
    }

    PluginLoader::~PluginLoader() {
        shutdown();
        pybind11::finalize_interpreter();
    }

    bool PluginLoader::loadPythonPlugin(const std::string& moduleName) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        if (isPluginRegistered(normalizedName)) {
            return true; // Already loaded
        }

        if (!loadPythonModule(normalizedName)) {
            return false;
        }

        auto moduleIt = pImpl->pythonModules.find(normalizedName);
        if (moduleIt == pImpl->pythonModules.end()) {
            return false;
        }

        if (!validatePythonPluginModule(moduleIt->second)) {
            unloadPythonModule(normalizedName);
            return false;
        }

        if (!createPluginEntry(normalizedName, moduleIt->second)) {
            unloadPythonModule(normalizedName);
            return false;
        }

        return true;
    }

    bool PluginLoader::unloadPythonPlugin(const std::string& moduleName) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto pluginIt = pImpl->pythonPlugins.find(normalizedName);
        if (pluginIt == pImpl->pythonPlugins.end()) {
            return false;
        }

        auto plugin = pluginIt->second;
        
        if (plugin->getState() == PluginState::RUNNING) {
            plugin->stop();
        }

        cleanupPluginEntry(normalizedName);
        
        return true;
    }

    std::shared_ptr<PluginInterface> PluginLoader::getPythonPlugin(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto it = pImpl->pythonPlugins.find(normalizedName);
        if (it != pImpl->pythonPlugins.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::vector<std::string> PluginLoader::getLoadedPythonPlugins() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        std::vector<std::string> result;
        result.reserve(pImpl->pythonPlugins.size());
        
        for (const auto& pair : pImpl->pythonPlugins) {
            result.push_back(pair.first);
        }
        return result;
    }

    bool PluginLoader::isPluginRegistered(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->pythonPlugins.find(moduleName) != pImpl->pythonPlugins.end();
    }

    std::vector<std::string> PluginLoader::discoverPythonPlugins(const std::string& directory) const {
        std::vector<std::string> result;
        
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return result;
        }

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                const auto& path = entry.path();
                std::string ext = path.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                if (ext == ".py") {
                    std::string moduleName = path.stem().string();
                    result.push_back(moduleName);
                }
            }
        }
        return result;
    }

    bool PluginLoader::configurePythonPlugin(const std::string& moduleName,
                                           const std::map<std::string, std::any>& config) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto it = pImpl->pythonPlugins.find(normalizedName);
        if (it == pImpl->pythonPlugins.end()) {
            return false;
        }

        auto plugin = it->second;
        pImpl->pluginConfigs[normalizedName] = config;
        
        return plugin->configure(config);
    }

    bool PluginLoader::initializePythonPlugin(const std::string& moduleName) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto it = pImpl->pythonPlugins.find(normalizedName);
        if (it == pImpl->pythonPlugins.end()) {
            return false;
        }

        return it->second->initialize();
    }

    bool PluginLoader::startPythonPlugin(const std::string& moduleName) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto it = pImpl->pythonPlugins.find(normalizedName);
        if (it == pImpl->pythonPlugins.end()) {
            return false;
        }

        return it->second->start();
    }

    bool PluginLoader::stopPythonPlugin(const std::string& moduleName) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto it = pImpl->pythonPlugins.find(normalizedName);
        if (it == pImpl->pythonPlugins.end()) {
            return false;
        }

        return it->second->stop();
    }

    bool PluginLoader::unloadPythonPluginCompletely(const std::string& moduleName) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        cleanupPluginEntry(normalizedName);
        
        auto moduleIt = pImpl->pythonModules.find(normalizedName);
        if (moduleIt != pImpl->pythonModules.end()) {
            pImpl->pythonModules.erase(moduleIt);
        }
        
        pImpl->pluginMetadata.erase(normalizedName);
        pImpl->pluginConfigs.erase(normalizedName);
        pImpl->pluginStates.erase(normalizedName);
        
        return true;
    }

    const PluginMetadata& PluginLoader::getPythonPluginMetadata(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto it = pImpl->pluginMetadata.find(normalizedName);
        if (it != pImpl->pluginMetadata.end()) {
            return it->second;
        }
        
        static PluginMetadata emptyMetadata;
        return emptyMetadata;
    }

    std::map<std::string, PluginState> PluginLoader::getAllPythonPluginStates() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->pluginStates;
    }

    std::shared_ptr<NeuralComponentPlugin> PluginLoader::getNeuralComponentPlugin(const std::string& moduleName) const {
        auto plugin = getPythonPlugin(moduleName);
        if (plugin) {
            return std::dynamic_pointer_cast<NeuralComponentPlugin>(plugin);
        }
        return nullptr;
    }

    std::shared_ptr<MemoryPlugin> PluginLoader::getMemoryPlugin(const std::string& moduleName) const {
        auto plugin = getPythonPlugin(moduleName);
        if (plugin) {
            return std::dynamic_pointer_cast<MemoryPlugin>(plugin);
        }
        return nullptr;
    }

    std::shared_ptr<ProcessingPlugin> PluginLoader::getProcessingPlugin(const std::string& moduleName) const {
        auto plugin = getPythonPlugin(moduleName);
        if (plugin) {
            return std::dynamic_pointer_cast<ProcessingPlugin>(plugin);
        }
        return nullptr;
    }

    std::string PluginLoader::getPluginDirectory() {
        return PluginLoader().pImpl->defaultPluginDirectory;
    }

    std::vector<std::string> PluginLoader::getPluginDirectories() {
        return PluginLoader().pImpl->pluginDirectories;
    }

    bool PluginLoader::setPluginDirectory(const std::string& directory) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            return false;
        }
        
        pImpl->defaultPluginDirectory = directory;
        pImpl->pluginDirectories.clear();
        pImpl->pluginDirectories.push_back(directory);
        return true;
    }

    void PluginLoader::addPythonEventListener(const std::string& eventType,
                                            std::function<void(const std::string&, const pybind11::dict&) > listener) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->eventListeners[eventType].push_back(listener);
    }

    void PluginLoader::removePythonEventListener(const std::string& eventType,
                                                const std::function<void(const std::string&, const pybind11::dict&)>& listener) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        auto it = pImpl->eventListeners.find(eventType);
        if (it != pImpl->eventListeners.end()) {
            it->second.erase(
                std::remove(it->second.begin(), it->second.end(), listener),
                it->second.end()
            );
        }
    }

    pybind11::dict PluginLoader::getPluginInfo(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        pybind11::dict result;
        
        auto it = pImpl->pythonModules.find(normalizedName);
        if (it == pImpl->pythonModules.end()) {
            return result;
        }
        
        return moduleToPluginInfo(it->second);
    }

    pybind11::list PluginLoader::getPluginDependencies(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        pybind11::list result;
        
        auto it = pImpl->pythonModules.find(normalizedName);
        if (it == pImpl->pythonModules.end()) {
            return result;
        }
        
        return moduleToPluginDependencies(it->second);
    }

    pybind11::list PluginLoader::getPluginCapabilities(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        pybind11::list result;
        
        auto it = pImpl->pythonModules.find(normalizedName);
        if (it == pImpl->pythonModules.end()) {
            return result;
        }
        
        return moduleToPluginCapabilities(it->second);
    }

    pybind11::dict PluginLoader::getPluginConfig(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto it = pImpl->pluginConfigs.find(normalizedName);
        if (it != pImpl->pluginConfigs.end()) {
            return anyToPyDict(it->second);
        }
        
        return pybind11::dict();
    }

    bool PluginLoader::validatePythonPlugin(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        auto it = pImpl->pythonModules.find(normalizedName);
        if (it == pImpl->pythonModules.end()) {
            return false;
        }
        
        return validatePythonPluginModule(it->second);
    }

    bool PluginLoader::validatePluginSecurity(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto normalizedName = normalizeModuleName(moduleName);
        
        // Basic security validation
        if (normalizedName.empty() || normalizedName.find("..") != std::string::npos) {
            return false;
        }
        
        auto it = pImpl->pluginMetadata.find(normalizedName);
        if (it == pImpl->pluginMetadata.end()) {
            return false;
        }
        
        return true;
    }

    void PluginLoader::cleanupAllPythonPlugins() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        for (const auto& pair : pImpl->pythonPlugins) {
            pair.second->unload();
        }
        
        pImpl->pythonPlugins.clear();
        pImpl->pythonModules.clear();
        pImpl->pluginMetadata.clear();
        pImpl->pluginConfigs.clear();
        pImpl->pluginStates.clear();
    }

    void PluginLoader::shutdown() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        cleanupAllPythonPlugins();
        pImpl->initialized = false;
    }

    bool PluginLoader::isInitialized() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->initialized;
    }

    size_t PluginLoader::getLoadedPluginCount() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        return pImpl->pythonPlugins.size();
    }

    // Private helper methods implementation
    bool PluginLoader::loadPythonModule(const std::string& moduleName) {
        try {
            // Add current directory to Python path if not already there
            pybind11::module_ sys = pybind11::import("sys");
            pybind11::list path = sys.attr("path");
            
            bool found = false;
            for (size_t i = 0; i < path.size(); ++i) {
                if (std::string(pybind11::str(path[i])) == ".") {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                path.append(".");
            }
            
            // Try to import the module
            pybind11::module_ module = pybind11::import(moduleName.c_str());
            pImpl->pythonModules[moduleName] = module;
            
            return true;
        } catch (const pybind11::error_already_set& e) {
            std::cerr << "Error loading Python module " << moduleName << ": " << pybind11::str(e.what()) << std::endl;
            return false;
        }
    }

    bool PluginLoader::unloadPythonModule(const std::string& moduleName) {
        auto it = pImpl->pythonModules.find(moduleName);
        if (it != pImpl->pythonModules.end()) {
            pImpl->pythonModules.erase(it);
        }
        return true;
    }

    bool PluginLoader::validatePythonPluginModule(const pybind11::module_& module) const {
        try {
            // Check if module has required attributes
            if (!module.attr("__dict__").contains("__name__")) {
                return false;
            }
            
            // Check for plugin interface
            pybind11::dict dict = module.attr("__dict__");
            
            // Look for plugin metadata
            bool hasPluginMetadata = dict.contains("get_plugin_metadata") || 
                                   dict.contains("metadata") ||
                                   dict.contains("_plugin_metadata_");
            
            if (!hasPluginMetadata) {
                return false;
            }
            
            return true;
        } catch (const pybind11::error_already_set& e) {
            return false;
        }
    }

    pybind11::module_ PluginLoader::getModule(const std::string& moduleName) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->pythonModules.find(moduleName);
        if (it != pImpl->pythonModules.end()) {
            return it->second;
        }
        
        return pybind11::module_();
    }

    std::string PluginLoader::normalizeModuleName(const std::string& name) const {
        std::string normalized = name;
        
        // Convert to lowercase
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
        
        // Replace dots and slashes with underscores
        std::replace(normalized.begin(), normalized.end(), '.', '_');
        std::replace(normalized.begin(), normalized.end(), '/', '_');
        
        return normalized;
    }

    bool PluginLoader::isBuiltinPythonModule(const std::string& name) const {
        static const std::vector<std::string> builtinModules = {
            "sys", "os", "json", "math", "random", "datetime", "collections",
            "itertools", "functools", "operator", "typing", "importlib",
            "multiprocessing", "threading", "socket", "subprocess"
        };
        
        return std::find(builtinModules.begin(), builtinModules.end(), name) != builtinModules.end();
    }

    std::string PluginLoader::getModuleFilePath(const std::string& moduleName) const {
        try {
            pybind11::module_ sys = pybind11::import("sys");
            pybind11::module_ importlib = pybind11::import("importlib");
            pybind11::module_ find_spec = importlib.attr("util").attr("find_spec")(moduleName.c_str());
            
            if (pybind11::none().is(find_spec)) {
                return "";
            }
            
            pybind11::object spec = find_spec;
            pybind11::object origin = spec.attr("origin");
            
            if (pybind11::none().is(origin)) {
                return "";
            }
            
            return std::string(pybind11::str(origin));
        } catch (const pybind11::error_already_set& e) {
            return "";
        }
    }

    pybind11::dict PluginLoader::moduleToPluginInfo(const pybind11::module_& module) const {
        pybind11::dict result;
        
        try {
            pybind11::dict dict = module.attr("__dict__");
            
            if (dict.contains("get_plugin_metadata")) {
                pybind11::function getMetadata = dict["get_plugin_metadata"];
                if (pybind11::none().is(getMetadata())) {
                    return result;
                }
                
                pybind11::object metadataObj = getMetadata();
                if (!metadataObj.is_none()) {
                    result["metadata"] = metadataObj;
                }
            }
            
            if (dict.contains("metadata")) {
                pybind11::object metadataObj = dict["metadata"];
                if (!metadataObj.is_none()) {
                    result["metadata"] = metadataObj;
                }
            }
            
            if (dict.contains("_plugin_metadata_")) {
                pybind11::object metadataObj = dict["_plugin_metadata_"];
                if (!metadataObj.is_none()) {
                    result["metadata"] = metadataObj;
                }
            }
            
            if (dict.contains("name")) {
                result["name"] = dict["name"];
            }
            
            if (dict.contains("version")) {
                result["version"] = dict["version"];
            }
            
            if (dict.contains("description")) {
                result["description"] = dict["description"];
            }
            
            if (dict.contains("author")) {
                result["author"] = dict["author"];
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    pybind11::list PluginLoader::moduleToPluginCapabilities(const pybind11::module_& module) const {
        pybind11::list result;
        
        try {
            pybind11::dict dict = module.attr("__dict__");
            
            if (dict.contains("get_plugin_capabilities")) {
                pybind11::function getCapabilities = dict["get_plugin_capabilities"];
                if (!pybind11::none().is(getCapabilities())) {
                    pybind11::object capsObj = getCapabilities();
                    if (!capsObj.is_none()) {
                        result = capsObj.cast<pybind11::list>();
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    pybind11::list PluginLoader::moduleToPluginDependencies(const pybind11::module_& module) const {
        pybind11::list result;
        
        try {
            pybind11::dict dict = module.attr("__dict__");
            
            if (dict.contains("get_plugin_dependencies")) {
                pybind11::function getDeps = dict["get_plugin_dependencies"];
                if (!pybind11::none().is(getDeps())) {
                    pybind11::object depsObj = getDeps();
                    if (!depsObj.is_none()) {
                        result = depsObj.cast<pybind11::list>();
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
        
        return result;
    }

    void PluginLoader::extractPluginMetadata(const pybind11::module_& module, PluginMetadata& metadata) const {
        try {
            pybind11::dict dict = module.attr("__dict__");
            
            if (dict.contains("get_plugin_metadata")) {
                pybind11::function getMetadata = dict["get_plugin_metadata"];
                if (!pybind11::none().is(getMetadata())) {
                    pybind11::object metadataObj = getMetadata();
                    if (!metadataObj.is_none()) {
                        // Convert Python dict to C++ PluginMetadata
                        // This is a simplified version - in practice, you'd need proper conversion
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
    }

    void PluginLoader::extractPluginConfig(const pybind11::module_& module, std::map<std::string, std::any>& config) const {
        try {
            pybind11::dict dict = module.attr("__dict__");
            
            if (dict.contains("get_plugin_config")) {
                pybind11::function getConfig = dict["get_plugin_config"];
                if (!pybind11::none().is(getConfig())) {
                    pybind11::object configObj = getConfig();
                    if (!configObj.is_none()) {
                        // Convert Python dict to C++ map
                        config = pyDictToAny(configObj.cast<pybind11::dict>());
                    }
                }
            }
        } catch (const pybind11::error_already_set& e) {
        }
    }

    bool PluginLoader::createPluginEntry(const std::string& moduleName, const pybind11::module_& module) {
        try {
            // Extract plugin metadata
            PluginMetadata metadata;
            extractPluginMetadata(module, metadata);
            
            // Create plugin wrapper
            auto plugin = std::make_shared<PythonPluginWrapper>(moduleName, module);
            
            // Store the plugin
            pImpl->pythonPlugins[moduleName] = plugin;
            pImpl->pluginMetadata[moduleName] = metadata;
            pImpl->pluginStates[moduleName] = PluginState::UNLOADED;
            
            return true;
        } catch (const pybind11::error_already_set& e) {
            std::cerr << "Error creating plugin entry for " << moduleName << ": " << pybind11::str(e.what()) << std::endl;
            return false;
        }
    }

    bool PluginLoader::cleanupPluginEntry(const std::string& moduleName) {
        auto it = pImpl->pythonPlugins.find(moduleName);
        if (it != pImpl->pythonPlugins.end()) {
            pImpl->pythonPlugins.erase(it);
        }
        return true;
    }

    void PluginLoader::updatePluginState(const std::string& moduleName, PluginState state) {
        pImpl->pluginStates[moduleName] = state;
    }

    void PluginLoader::triggerEvent(const std::string& eventType, const std::string& pluginName, const pybind11::dict& data) {
        auto it = pImpl->eventListeners.find(eventType);
        if (it != pImpl->eventListeners.end()) {
            for (const auto& listener : it->second) {
                try {
                    listener(pluginName, data);
                } catch (const pybind11::error_already_set& e) {
                    std::cerr << "Error in event listener: " << pybind11::str(e.what()) << std::endl;
                }
            }
        }
    }

    bool PluginLoader::checkPluginConflicts(const std::string& moduleName) const {
        // Check if plugin conflicts with existing plugins
        return true;
    }

    void PluginLoader::logPythonPluginEvent(const std::string& eventType, const std::string& pluginName, const pybind11::dict& data) {
        std::cout << "[PluginLoader] " << eventType << " for plugin " << pluginName << std::endl;
    }

    bool PluginLoader::setupPythonPluginEnvironment(const std::string& moduleName) {
        // Setup environment for Python plugin
        return true;
    }

    bool PluginLoader::cleanupPythonPluginEnvironment(const std::string& moduleName) {
        // Cleanup environment for Python plugin
        return true;
    }

    pybind11::dict PluginLoader::anyToPyDict(const std::map<std::string, std::any>& config) const {
        pybind11::dict result;
        
        for (const auto& pair : config) {
            result[pair.first] = pair.second;
        }
        
        return result;
    }

    std::map<std::string, std::any> PluginLoader::pyDictToAny(const pybind11::dict& pyDict) const {
        std::map<std::string, std::any> result;
        
        for (auto it = pyDict.begin(); it != pyDict.end(); ++it) {
            std::string key = pybind11::str(it->first);
            result[key] = it->second;
        }
        
        return result;
    }

} // namespace nlm