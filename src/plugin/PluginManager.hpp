// Plugin system for NLM
// Provides dynamic loading of experiments, plugins, and extensions

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <any>
#include <filesystem>
#include <stdexcept>
#include <mutex>
#include <dlfcn.h>

namespace nlm {

// Plugin system version
constexpr int PLUGIN_SYSTEM_VERSION = 1;

// Plugin type identifiers
enum class PluginType {
    Experiment,
    SensoryPlugin,
    MotorPlugin,
    MemoryPlugin,
    NeuromodulationPlugin,
    PredictionPlugin,
    CognitivePlugin,
    WorldPlugin,
    Extension
};

// Plugin metadata
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    PluginType type;
    std::vector<std::string> dependencies;
    std::string filepath;
    
    // Plugin interface version
    int interfaceVersion;
    
    // Plugin capabilities
    std::vector<std::string> capabilities;
    
    PluginInfo() : interfaceVersion(PLUGIN_SYSTEM_VERSION) {}
};

// Plugin exception types
class PluginException : public std::runtime_error {
public:
    PluginException(const std::string& message) : std::runtime_error(message) {}
};

class PluginLoadException : public PluginException {
public:
    PluginLoadException(const std::string& message) : PluginException("Plugin load failed: " + message) {}
};

class PluginInterfaceMismatchException : public PluginException {
public:
    PluginInterfaceMismatchException(const std::string& message) : PluginException("Plugin interface mismatch: " + message) {}
};

class PluginNotFoundException : public PluginException {
public:
    PluginNotFoundException(const std::string& message) : PluginException("Plugin not found: " + message) {}
};

// Base plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin info
    virtual const PluginInfo& getInfo() const = 0;
    
    // Initialize plugin
    virtual bool initialize(const std::shared_ptr<Config>& config) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Check if plugin is initialized
    virtual bool isInitialized() const = 0;
    
    // Get plugin capabilities
    virtual const std::vector<std::string>& getCapabilities() const = 0;
    
    // Get dependency names
    virtual const std::vector<std::string>& getDependencies() const = 0;
    
    // Check if plugin supports a capability
    virtual bool supportsCapability(const std::string& capability) const {
        return std::find(getCapabilities().begin(), getCapabilities().end(), capability) != getCapabilities().end();n    }
    
    // Virtual destructor for proper cleanup
    virtual void onPluginLoaded() {}
    virtual void onPluginUnloaded() {}
    
    // Plugin lifecycle hooks
    virtual void onSimulationStart() {}
    virtual void onSimulationStep() {}\n    virtual void onSimulationEnd() {}
    
    // Configuration management
    virtual bool configure(const std::string& key, const ConfigValue& value) = 0;
    virtual std::optional<ConfigValue> getConfig(const std::string& key) const = 0;
    
    // Plugin metadata access
    virtual std::string getVersion() const { return getInfo().version; }
    virtual std::string getDescription() const { return getInfo().description; }
    virtual std::string getAuthor() const { return getInfo().author; }
    virtual std::string getLicense() const { return getInfo().license; }
    
    // Plugin state
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};

// Experiment plugin interface
class IExperiment : public IPlugin {
public:
    // Experiment metadata
    virtual std::string getExperimentName() const = 0;
    virtual std::string getExperimentDescription() const = 0;
    virtual std::string getExperimentVersion() const = 0;
    
    // Experiment execution
    virtual bool runExperiment(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world) = 0;
    virtual bool runExperiment(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config) = 0;
    
    // Experiment configuration
    virtual std::vector<std::string> getRequiredConfigKeys() const = 0;
    virtual std::vector<std::string> getOptionalConfigKeys() const = 0;
    
    // Experiment results
    struct ExperimentResult {
        std::string experimentName;
        std::string timestamp;
        double duration;
        std::unordered_map<std::string, double> metrics;
        std::unordered_map<std::string, std::string> parameters;
        std::string status; // "success", "failed", "in_progress"
        std::string errorMessage;
        
        ExperimentResult() : duration(0.0), status("pending") {}
    };
    
    virtual ExperimentResult getLastResult() const = 0;
    
    // Experiment type
    virtual bool isLongRunning() const { return false; }
    virtual int getEstimatedDuration() const { return 3600; } // seconds
    
    // Experiment categorization
    virtual std::string getCategory() const = 0;
    virtual std::string getSubcategory() const { return "general"; }
    
    // Experiment scheduling
    virtual bool requiresReset() const { return true; }
    virtual bool supportsConcurrentExecution() const { return false; }
};

// Plugin loading interface
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    
    virtual bool canLoad(const std::string& filepath) const = 0;
    
    virtual std::unique_ptr<IPlugin> loadPlugin(const std::string& filepath) = 0;
    
    virtual bool unloadPlugin(const std::string& name) = 0;
    
    virtual bool isPluginLoaded(const std::string& name) const = 0;
    
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
};

// Dynamic library plugin loader
class DynamicLibraryLoader : public IPluginLoader {
public:
    DynamicLibraryLoader() = default;
    ~DynamicLibraryLoader() override {
        for (auto& [name, handle] : loadedPlugins_) {
            if (handle) {
                dlclose(handle);
            }
        }
    }
    
    bool canLoad(const std::string& filepath) const override {
        if (!std::filesystem::exists(filepath)) {
            return false;
        }
        
        std::string ext = getFileExtension(filepath);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        return ext == "so" || ext == "dylib" || ext == "dll" || ext == "plugin";
    }
    
    std::unique_ptr<IPlugin> loadPlugin(const std::string& filepath) override {
        if (!canLoad(filepath)) {
            throw PluginLoadException("Cannot load plugin: unsupported file type");
        }
        
        // Open library
        void* handle = dlopen(filepath.c_str(), RTLD_LAZY);
        if (!handle) {
            throw PluginLoadException("Failed to open library: " + std::string(dlerror()));
        }
        
        // Get plugin factory function
        typedef IPlugin*(*CreatePluginFunc)();
        CreatePluginFunc createFunc = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
        if (!createFunc) {
            dlclose(handle);
            throw PluginLoadException("Plugin factory function not found");
        }
        
        // Create plugin instance
        std::unique_ptr<IPlugin> plugin(createFunc());
        if (!plugin) {
            dlclose(handle);
            throw PluginLoadException("Plugin creation failed");
        }
        
        // Get plugin info
        const PluginInfo& info = plugin->getInfo();
        
        // Check interface version
        if (info.interfaceVersion != PLUGIN_SYSTEM_VERSION) {
            dlclose(handle);
            throw PluginInterfaceMismatchException("Plugin interface version mismatch");
        }
        
        // Check if already loaded
        if (isPluginLoaded(info.name)) {
            dlclose(handle);
            throw PluginLoadException("Plugin already loaded: " + info.name);
        }
        
        // Store plugin handle
        loadedPlugins_[info.name] = handle;
        plugin->onPluginLoaded();
        
        return plugin;
    }
    
    bool unloadPlugin(const std::string& name) override {
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        void* handle = it->second;
        dlclose(handle);
        loadedPlugins_.erase(it);
        return true;
    }
    
    bool isPluginLoaded(const std::string& name) const override {
        return loadedPlugins_.find(name) != loadedPlugins_.end();
    }
    
    std::vector<std::string> getLoadedPlugins() const override {
        std::vector<std::string> names;
        for (const auto& pair : loadedPlugins_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void setLibraryPath(const std::string& path) {
        libraryPath_ = path;
    }
    
    std::string getLibraryPath() const {
        return libraryPath_;
    }
    
    // Search for plugins in library path
    std::vector<std::string> findPlugins(const std::string& extension = "") const {
        std::vector<std::string> plugins;
        
        if (!std::filesystem::exists(libraryPath_)) {
            return plugins;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(libraryPath_)) {
            if (entry.is_regular_file()) {
                std::string ext = getFileExtension(entry.path().string());
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                if (extension.empty() || ext == extension) {
                    plugins.push_back(entry.path().string());
                }
            }
        }
        
        return plugins;
    }

private:
    std::string getFileExtension(const std::string& filename) const {
        if (filename.find_last_of('.') == std::string::npos) {
            return "";
        }
        return filename.substr(filename.find_last_of('.') + 1);
    }
    
    std::string libraryPath_ = ".";
    std::unordered_map<std::string, void*> loadedPlugins_;
};

// Plugin manager
class PluginManager {
public:
    static PluginManager& getInstance() {
        static PluginManager instance;
        return instance;
    }
    
    void initialize(const std::string& libraryPath = "plugins") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (initialized_) {
            NLM_LOG_WARNING("Plugin manager already initialized");
            return;
        }
        
        loader_ = std::make_unique<DynamicLibraryLoader>();
        loader_->setLibraryPath(libraryPath);
        
        // Discover available plugins
        discoverPlugins();
        
        NLM_LOG_INFO("Plugin manager initialized with " + std::to_string(pluginCount_) + " plugins");
        
        initialized_ = true;
    }
    
    void shutdown() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!initialized_) {
            return;
        }
        
        // Shutdown all plugins in reverse order
        for (auto it = loadedPlugins_.rbegin(); it != loadedPlugins_.rend(); ++it) {
            auto& plugin = it->second;
            if (plugin && plugin->isInitialized()) {
                try {
                    plugin->shutdown();
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error shutting down plugin " + plugin->getInfo().name + ": " + e.what());
                }
            }
        }
        
        loadedPlugins_.clear();
        loader_.reset();
        
        pluginCount_ = 0;
        initialized_ = false;
        
        NLM_LOG_INFO("Plugin manager shutdown completed");
    }
    
    // Load plugin from file
    std::string loadPlugin(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!initialized_) {
            throw PluginException("Plugin manager not initialized");
        }
        
        if (!loader_->canLoad(filepath)) {
            throw PluginLoadException("Cannot load plugin from: " + filepath);
        }
        
        try {
            auto plugin = loader_->loadPlugin(filepath);
            const PluginInfo& info = plugin->getInfo();
            
            // Check dependencies
            if (!checkDependencies(plugin.get())) {
                loader_->unloadPlugin(info.name);
                throw PluginLoadException("Plugin dependencies not met for: " + info.name);
            }
            
            // Add to loaded plugins
            loadedPlugins_[info.name] = std::move(plugin);
            pluginCount_++;
            
            NLM_LOG_INFO("Loaded plugin: " + info.name + " v" + info.version);
            
            return info.name;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to load plugin from " + filepath + ": " + e.what());
            throw;
        }
    }
    
    // Load plugin from directory
    std::vector<std::string> loadPluginsFromDirectory(const std::string& directory) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> loaded;
        
        if (!std::filesystem::exists(directory)) {
            NLM_LOG_ERROR("Plugin directory does not exist: " + directory);
            return loaded;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                try {
                    std::string name = loadPlugin(entry.path().string());
                    loaded.push_back(name);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to load plugin from " + entry.path().string() + ": " + e.what());
                }
            }
        }
        
        return loaded;
    }
    
    // Unload plugin
    bool unloadPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        try {
            it->second->onPluginUnloaded();
            loader_->unloadPlugin(name);
            loadedPlugins_.erase(it);
            pluginCount_--;
            
            NLM_LOG_INFO("Unloaded plugin: " + name);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error unloading plugin " + name + ": " + e.what());
            return false;
        }
    }
    
    // Get plugin by name
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(name);
        if (it == loadedPlugins_.end()) {
            throw PluginNotFoundException("Plugin not found: " + name);
        }
        
        return it->second;
    }
    
    // Get experiment plugin
    std::shared_ptr<IExperiment> getExperiment(const std::string& name) {
        std::shared_ptr<IPlugin> plugin = getPlugin(name);
        
        // Try to cast to experiment interface
        auto experiment = std::dynamic_pointer_cast<IExperiment>(plugin);
        if (!experiment) {
            throw PluginException("Plugin is not an experiment: " + name);
        }
        
        return experiment;
    }
    
    // Get all plugins of a specific type
    std::vector<std::shared_ptr<IPlugin>> getPluginsOfType(PluginType type) {
        std::vector<std::shared_ptr<IPlugin>> plugins;
        
        for (const auto& pair : loadedPlugins_) {
            const auto& plugin = pair.second;
            if (plugin->getInfo().type == type) {
                plugins.push_back(plugin);
            }
        }
        
        return plugins;
    }
    
    // Get all experiment plugins
    std::vector<std::shared_ptr<IExperiment>> getExperiments() {
        std::vector<std::shared_ptr<IExperiment>> experiments;
        
        for (const auto& plugin : loadedPlugins_) {
            auto experiment = std::dynamic_pointer_cast<IExperiment>(plugin.second);
            if (experiment) {
                experiments.push_back(experiment);
            }
        }
        
        return experiments;
    }
    
    // Get all plugins
    std::vector<std::shared_ptr<IPlugin>> getAllPlugins() {
        std::vector<std::shared_ptr<IPlugin>> plugins;
        
        for (const auto& pair : loadedPlugins_) {
            plugins.push_back(pair.second);
        }
        
        return plugins;
    }
    
    // Check if plugin is loaded
    bool isPluginLoaded(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return loadedPlugins_.find(name) != loadedPlugins_.end();
    }
    
    // Get all loaded plugin names
    std::vector<std::string> getLoadedPluginNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        
        for (const auto& pair : loadedPlugins_) {
            names.push_back(pair.first);
        }
        
        return names;
    }
    
    // Check dependencies
    bool checkDependencies(IPlugin* plugin) const {
        for (const auto& dependency : plugin->getDependencies()) {
            if (!isPluginLoaded(dependency)) {
                NLM_LOG_WARNING("Plugin " + plugin->getInfo().name + " depends on missing plugin: " + dependency);
                return false;
            }
        }
        return true;
    }
    
    // Enable/disable plugin
    bool setPluginEnabled(const std::string& name, bool enabled) {
        auto plugin = getPlugin(name);
        plugin->setEnabled(enabled);
        return true;
    }
    
    // Configure plugin
    bool configurePlugin(const std::string& name, const std::string& key, const ConfigValue& value) {
        auto plugin = getPlugin(name);
        return plugin->configure(key, value);
    }
    
    // Get plugin configuration
    std::optional<ConfigValue> getPluginConfig(const std::string& name, const std::string& key) {
        auto plugin = getPlugin(name);
        return plugin->getConfig(key);
    }
    
    // Initialize all plugins
    void initializeAllPlugins(const std::shared_ptr<Config>& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& pair : loadedPlugins_) {
            auto& plugin = pair.second;
            
            if (plugin->isEnabled()) {
                try {
                    if (plugin->initialize(config)) {
                        NLM_LOG_INFO("Initialized plugin: " + plugin->getInfo().name);
                    } else {
                        NLM_LOG_ERROR("Failed to initialize plugin: " + plugin->getInfo().name);
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error initializing plugin " + plugin->getInfo().name + ": " + e.what());
                }
            }
        }
    }
    
    // Check if plugin manager is initialized
    bool isInitialized() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return initialized_;
    }
    
    // Get plugin count
    size_t getPluginCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pluginCount_;
    }
    
    // Find experiment by name
    std::shared_ptr<IExperiment> findExperiment(const std::string& name) {
        for (auto& experiment : getExperiments()) {
            if (experiment->getExperimentName() == name) {
                return experiment;
            }
        }
        return nullptr;
    }
    
    // Find experiment by category
    std::vector<std::shared_ptr<IExperiment>> findExperimentsByCategory(const std::string& category) {
        std::vector<std::shared_ptr<IExperiment>> experiments;
        
        for (auto& experiment : getExperiments()) {
            if (experiment->getCategory() == category) {
                experiments.push_back(experiment);
            }
        }
        
        return experiments;
    }
    
    // Find experiment by subcategory
    std::vector<std::shared_ptr<IExperiment>> findExperimentsBySubcategory(const std::string& subcategory) {
        std::vector<std::shared_ptr<IExperiment>> experiments;
        
        for (auto& experiment : getExperiments()) {
            if (experiment->getSubcategory() == subcategory) {
                experiments.push_back(experiment);
            }
        }
        
        return experiments;
    }

private:
    PluginManager() : initialized_(false), pluginCount_(0) {}
    ~PluginManager() {
        shutdown();
    }
    
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    void discoverPlugins() {
        if (!loader_) {
            return;
        }
        
        std::vector<std::string> plugins = loader_->findPlugins();
        NLM_LOG_INFO("Found " + std::to_string(plugins.size()) + " potential plugins");
        
        for (const auto& pluginPath : plugins) {
            try {
                std::string name = loadPlugin(pluginPath);
                NLM_LOG_DEBUG("Discovered plugin: " + name);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Could not load plugin " + pluginPath + ": " + e.what());
            }
        }
    }
    
    mutable std::mutex mutex_;
    std::unique_ptr<IPluginLoader> loader_;
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> loadedPlugins_;
    bool initialized_;
    size_t pluginCount_;
};

// Global plugin manager access
inline PluginManager& getPluginManager() {
    return PluginManager::getInstance();
}

// Plugin utilities
namespace PluginUtils {
    // Create experiment from plugin
    std::shared_ptr<IExperiment> createExperiment(const std::string& pluginName) {
        return getPluginManager().getExperiment(pluginName);
    }
    
    // Run experiment
    bool runExperiment(const std::string& experimentName, const std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config = nullptr) {
        auto experiment = getPluginManager().findExperiment(experimentName);
        if (!experiment) {
            throw PluginNotFoundException("Experiment not found: " + experimentName);
        }
        
        if (!config) {
            config = getPluginManager().getPlugin("nlm_config")->getConfig();
        }
        
        return experiment->runExperiment(brain, config);
    }
    
    // Get experiment list
    std::vector<std::string> getExperimentList() {
        std::vector<std::string> experiments;
        
        for (auto& experiment : getPluginManager().getExperiments()) {
            experiments.push_back(experiment->getExperimentName());
        }
        
        return experiments;
    }
    
    // Get experiment info
    std::string getExperimentInfo(const std::string& experimentName) {
        auto experiment = getPluginManager().findExperiment(experimentName);
        if (!experiment) {
            throw PluginNotFoundException("Experiment not found: " + experimentName);
        }
        
        std::stringstream ss;
        ss << "Experiment: " << experiment->getExperimentName() << "\n";
        ss << "Description: " << experiment->getExperimentDescription() << "\n";
        ss << "Version: " << experiment->getExperimentVersion() << "\n";
        ss << "Category: " << experiment->getCategory() << "\n";
        ss << "Subcategory: " << experiment->getSubcategory() << "\n";
        ss << "Estimated Duration: " << experiment->getEstimatedDuration() << " seconds\n";
        ss << "Long Running: " << (experiment->isLongRunning() ? "Yes" : "No") << "\n";
        
        return ss.str();
    }
    
    // List all plugins
    std::string listPlugins() {
        std::stringstream ss;
        ss << "Loaded Plugins (" << getPluginManager().getPluginCount() << "):\n\n";
        
        for (const auto& name : getPluginManager().getLoadedPluginNames()) {
            try {
                auto plugin = getPluginManager().getPlugin(name);
                ss << "  " << name << " (v" << plugin->getVersion() << ")";
                ss << " - " << plugin->getDescription() << "\n";
            } catch (const std::exception& e) {
                ss << "  " << name << " - Error loading: " << e.what() << "\n";
            }
        }
        
        return ss.str();
    }
    
    // Plugin status
    std::string getPluginStatus(const std::string& name) {
        if (!getPluginManager().isPluginLoaded(name)) {
            return "Plugin not loaded: " + name;
        }
        
        auto plugin = getPluginManager().getPlugin(name);
        std::stringstream ss;
        ss << "Plugin: " << name << "\n";
        ss << "  Type: " << static_cast<int>(plugin->getInfo().type) << "\n";
        ss << "  Enabled: " << (plugin->isEnabled() ? "Yes" : "No") << "\n";
        ss << "  Initialized: " << (plugin->isInitialized() ? "Yes" : "No") << "\n";
        ss << "  Dependencies: ";
        for (const auto& dep : plugin->getDependencies()) {
            ss << dep << " ";
        }
        ss << "\n";
        
        return ss.str();
    }
}

} // namespace nlm