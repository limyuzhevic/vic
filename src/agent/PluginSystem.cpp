#include "PluginSystem.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <mutex>
#include <dlfcn.h> // For dynamic loading
#include <memory>
#include <thread>

namespace nlm {

// Plugin manager implementation
struct PluginManager::Impl {
    std::unordered_map<std::string, std::unique_ptr<Plugin>> plugins;
    std::unordered_map<std::string, PluginConfig> pluginConfigs;
    std::unordered_map<std::string, std::map<std::string, std::string>> pluginSettings;
    std::unordered_map<std::string, bool> pluginInitialized;
    std::unordered_map<std::string, std::vector<std::string>> pluginDependencies;
    
    std::mutex mutex;
    bool systemInitialized;
    
    Impl() : systemInitialized(false) {}
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}

PluginManager::~PluginManager() = default;

PluginManager::PluginManager(PluginManager&&) noexcept = default;

PluginManager& PluginManager::operator=(PluginManager&&) noexcept = default;

PluginManager& PluginManager::getInstance() {
    static PluginManager instance;
    return instance;
}

PluginLoadResult PluginManager::loadPlugin(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if plugin already loaded
    std::string pluginName = extractPluginNameFromFile(filepath);
    if (isPluginLoaded(pluginName)) {
        return PluginLoadResult::ALREADY_LOADED;
    }
    
    try {
        // Load plugin from dynamic library
        void* handle = dlopen(filepath.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Failed to load plugin " << filepath << ": " << dlerror() << std::endl;
            return PluginLoadResult::FAILED;
        }
        
        // Get plugin factory function
        using PluginFactoryFunc = std::unique_ptr<Plugin>(*)();
        auto factoryFunc = reinterpret_cast<PluginFactoryFunc>(dlsym(handle, "createPlugin"));
        if (!factoryFunc) {
            dlclose(handle);
            std::cerr << "Plugin " << filepath << " does not export createPlugin function" << std::endl;
            return PluginLoadResult::INVALID_FORMAT;
        }
        
        // Create plugin instance
        auto plugin = factoryFunc();
        if (!plugin) {
            dlclose(handle);
            std::cerr << "Plugin factory returned null for " << filepath << std::endl;
            return PluginLoadResult::FAILED;
        }
        
        // Store plugin
        std::string name = plugin->getName();
        plugins[name] = std::move(plugin);
        
        // Load plugin configuration
        loadPluginConfig(name, filepath);
        
        // Store handle for later cleanup
        // In a real implementation, we'd store the handle in a map
        
        return PluginLoadResult::SUCCESS;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception loading plugin " << filepath << ": " << e.what() << std::endl;
        return PluginLoadResult::FAILED;
    }
}

PluginLoadResult PluginManager::loadPlugin(const std::string& name, PluginFactory factory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (isPluginLoaded(name)) {
        return PluginLoadResult::ALREADY_LOADED;
    }
    
    try {
        auto plugin = factory();
        if (!plugin) {
            std::cerr << "Plugin factory returned null for " << name << std::endl;
            return PluginLoadResult::FAILED;
        }
        
        plugins[name] = std::move(plugin);
        
        // Load default configuration
        PluginConfig config(name);
        config.settings = getDefaultPluginConfig(name);
        pImpl->pluginConfigs[name] = config;
        
        return PluginLoadResult::SUCCESS;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception creating plugin " << name << ": " << e.what() << std::endl;
        return PluginLoadResult::FAILED;
    }
}

bool PluginManager::unloadPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        return false;
    }
    
    // Shutdown plugin if initialized
    if (pImpl->pluginInitialized[name]) {
        it->second->shutdown();
        pImpl->pluginInitialized[name] = false;
    }
    
    // Remove from all maps
    plugins.erase(it);
    pImpl->pluginConfigs.erase(name);
    pImpl->pluginSettings.erase(name);
    pImpl->pluginDependencies.erase(name);
    pImpl->pluginInitialized.erase(name);
    
    return true;
}

bool PluginManager::unloadAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Shutdown all plugins
    for (auto& [name, plugin] : plugins) {
        if (pImpl->pluginInitialized[name]) {
            plugin->shutdown();
        }
    }
    
    // Clear all maps
    plugins.clear();
    pImpl->pluginConfigs.clear();
    pImpl->pluginSettings.clear();
    pImpl->pluginDependencies.clear();
    pImpl->pluginInitialized.clear();
    
    return true;
}

bool PluginManager::isPluginLoaded(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return plugins.find(name) != plugins.end();
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(plugins.size());
    for (const auto& pair : plugins) {
        names.push_back(pair.first);
    }
    return names;
}

std::string PluginManager::getPluginInfo(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        return "Plugin not found: " + name;
    }
    
    const auto& plugin = it->second;
    std::ostringstream oss;
    oss << "Plugin: " << plugin->getName() << "\n";
    oss << "Version: " << plugin->getVersion() << "\n";
    oss << "Description: " << plugin->getDescription() << "\n";
    oss << "Type: " << static_cast<int>(plugin->getType()) << "\n";
    oss << "Initialized: " << (pImpl->pluginInitialized.count(name) && pImpl->pluginInitialized.at(name) ? "Yes" : "No") << "\n";
    
    auto deps = plugin->getDependencies();
    if (!deps.empty()) {
        oss << "Dependencies: ";
        for (size_t i = 0; i < deps.size(); ++i) {
            oss << deps[i];
            if (i < deps.size() - 1) oss << ", ";
        }
        oss << "\n";
    }
    
    auto caps = plugin->getCapabilities();
    if (!caps.empty()) {
        oss << "Capabilities: ";
        for (size_t i = 0; i < caps.size(); ++i) {
            oss << caps[i];
            if (i < caps.size() - 1) oss << ", ";
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool PluginManager::initializePlugin(const std::string& name, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        std::cerr << "Cannot initialize plugin " << name << ": not found" << std::endl;
        return false;
    }
    
    // Check if already initialized
    if (pImpl->pluginInitialized[name]) {
        std::cerr << "Plugin " << name << " already initialized" << std::endl;
        return false;
    }
    
    // Check dependencies
    if (!checkDependencies(name)) {
        std::cerr << "Cannot initialize plugin " << name << ": missing dependencies" << std::endl;
        return false;
    }
    
    // Configure plugin
    if (!config.empty()) {
        configurePlugin(name, config);
    }
    
    // Initialize plugin
    bool success = it->second->initialize(pImpl->pluginSettings[name]);
    if (success) {
        pImpl->pluginInitialized[name] = true;
        initializeDependencies(name);
    }
    
    return success;
}

void PluginManager::shutdownPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        return;
    }
    
    // Cleanup dependencies
    cleanupDependencies(name);
    
    // Shutdown plugin
    if (pImpl->pluginInitialized[name]) {
        it->second->shutdown();
        pImpl->pluginInitialized[name] = false;
    }
}

void PluginManager::initializeAllPlugins(const std::map<std::string, std::map<std::string, std::string>>& configs) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->systemInitialized) {
        std::cerr << "Plugin system already initialized" << std::endl;
        return;
    }
    
    // Determine initialization order based on dependencies
    std::vector<std::string> initializationOrder = getInitializationOrder();
    
    for (const auto& name : initializationOrder) {
        // Get config for this plugin
        auto configIt = configs.find(name);
        std::map<std::string, std::string> config = (configIt != configs.end()) ? configIt->second : std::map<std::string, std::string>();
        
        if (!initializePlugin(name, config)) {
            std::cerr << "Failed to initialize plugin " << name << ", aborting system initialization" << std::endl;
            // In a real implementation, we might want to rollback already initialized plugins
            return;
        }
    }
    
    pImpl->systemInitialized = true;
}

void PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->systemInitialized) {
        return;
    }
    
    // Determine shutdown order (reverse of initialization)
    std::vector<std::string> shutdownOrder = getShutdownOrder();
    
    for (const auto& name : shutdownOrder) {
        shutdownPlugin(name);
    }
    
    pImpl->systemInitialized = false;
}

void PluginManager::updateAllPlugins(double timestep) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& [name, plugin] : plugins) {
        if (pImpl->pluginInitialized[name]) {
            plugin->update(timestep);
        }
    }
}

std::string PluginManager::getAllPluginStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== NLM Plugin System Status ===\n";
    oss << "System Initialized: " << (pImpl->systemInitialized ? "Yes" : "No") << "\n";
    oss << "Loaded Plugins: " << plugins.size() << "\n";
    oss << "\n";
    
    for (const auto& [name, plugin] : plugins) {
        oss << "--- " << name << " ---\n";
        oss << plugin->getStatus() << "\n";
        
        if (pImpl->pluginInitialized[name]) {
            oss << "Initialized: Yes\n";
        } else {
            oss << "Initialized: No\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::vector<std::string> PluginManager::getPluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> result;
    for (const auto& pair : plugins) {
        if (pair.second->getType() == type) {
            result.push_back(pair.first);
        }
    }
    return result;
}

std::vector<std::string> PluginManager::getPluginsWithCapability(const std::string& capability) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> result;
    for (const auto& pair : plugins) {
        if (pair.second->hasCapability(capability)) {
            result.push_back(pair.first);
        }
    }
    return result;
}

std::vector<std::string> PluginManager::getPluginsWithDependency(const std::string& dependency) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> result;
    for (const auto& pair : plugins) {
        if (pair.second->hasDependency(dependency)) {
            result.push_back(pair.first);
        }
    }
    return result;
}

bool PluginManager::configurePlugin(const std::string& name, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        return false;
    }
    
    // Merge with existing settings
    auto& settings = pImpl->pluginSettings[name];
    settings.insert(config.begin(), config.end());
    
    // Store in plugin config
    if (pImpl->pluginConfigs.find(name) != pImpl->pluginConfigs.end()) {
        pImpl->pluginConfigs[name].settings = settings;
    }
    
    return true;
}

std::map<std::string, std::string> PluginManager::getPluginConfig(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginSettings.find(name);
    if (it != pImpl->pluginSettings.end()) {
        return it->second;
    }
    
    return std::map<std::string, std::string>();
}

std::string PluginManager::getSystemInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Plugin System Info:\n";
    oss << "Version: " << PLUGIN_SYSTEM_VERSION << "\n";
    oss << "Loaded Plugins: " << plugins.size() << "\n";
    oss << "System Initialized: " << (pImpl->systemInitialized ? "Yes" : "No") << "\n";
    oss << "\n";
    
    // Count plugins by type
    std::map<PluginType, int> typeCounts;
    for (const auto& pair : plugins) {
        typeCounts[pair.second->getType()]++;
    }
    
    oss << "Plugin Types:\n";
    for (const auto& pair : typeCounts) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

bool PluginManager::savePluginState(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json state;
        state["version"] = PLUGIN_SYSTEM_VERSION;
        state["system_initialized"] = pImpl->systemInitialized;
        
        // Save plugin states
        nlohmann::json pluginsJson = nlohmann::json::array();
        for (const auto& [name, plugin] : plugins) {
            nlohmann::json pluginState;
            pluginState["name"] = name;
            pluginState["initialized"] = pImpl->pluginInitialized[name];
            pluginState["config"] = pImpl->pluginConfigs[name].settings;
            pluginsJson.push_back(pluginState);
        }
        state["plugins"] = pluginsJson;
        
        file << state.dump(2);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving plugin state: " << e.what() << std::endl;
        return false;
    }
}

bool PluginManager::loadPluginState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json state;
        file >> state;
        
        // Check version
        if (state.find("version") != state.end() && state["version"] != PLUGIN_SYSTEM_VERSION) {
            std::cerr << "Plugin state version mismatch" << std::endl;
            return false;
        }
        
        // Load system state
        pImpl->systemInitialized = state.value("system_initialized", false);
        
        // Load plugin states
        if (state.find("plugins") != state.end()) {
            // In a real implementation, we would reload plugins from state
            // For now, just clear and rebuild
            unloadAllPlugins();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading plugin state: " << e.what() << std::endl;
        return false;
    }
}

// Private helper methods
void PluginManager::initializeDependencies(const std::string& pluginName) {
    auto plugin = plugins.find(pluginName);
    if (plugin == plugins.end()) return;
    
    for (const auto& dependency : plugin->second->getDependencies()) {
        auto depIt = plugins.find(dependency);
        if (depIt != plugins.end() && pImpl->pluginInitialized.find(dependency) != pImpl->pluginInitialized.end() && !pImpl->pluginInitialized[dependency]) {
            // Initialize dependency
            initializePlugin(dependency);
        }
    }
}

bool PluginManager::checkDependencies(const std::string& pluginName) const {
    auto plugin = plugins.find(pluginName);
    if (plugin == plugins.end()) return false;
    
    for (const auto& dependency : plugin->second->getDependencies()) {
        if (!isPluginLoaded(dependency)) {
            return false;
        }
        
        auto depIt = pImpl->pluginInitialized.find(dependency);
        if (depIt == pImpl->pluginInitialized.end() || !depIt->second) {
            return false;
        }
    }
    
    return true;
}

std::string PluginManager::getDependencyError(const std::string& pluginName) const {
    auto plugin = plugins.find(pluginName);
    if (plugin == plugins.end()) return "Plugin not found";
    
    for (const auto& dependency : plugin->second->getDependencies()) {
        if (!isPluginLoaded(dependency)) {
            return "Missing dependency: " + dependency;
        }
        
        auto depIt = pImpl->pluginInitialized.find(dependency);
        if (depIt == pImpl->pluginInitialized.end() || !depIt->second) {
            return "Dependency not initialized: " + dependency;
        }
    }
    
    return "All dependencies satisfied";
}

void PluginManager::cleanupDependencies(const std::string& pluginName) {
    // Find plugins that depend on this plugin
    for (auto& [name, plugin] : plugins) {
        if (pluginName == name) continue; // Don't cleanup itself
        
        for (const auto& dependency : plugin->second->getDependencies()) {
            if (dependency == pluginName && pImpl->pluginInitialized[name]) {
                shutdownPlugin(name);
            }
        }
    }
}

std::vector<std::string> PluginManager::getInitializationOrder() const {
    // Simple topological sort based on dependencies
    std::vector<std::string> result;
    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, bool> inStack;
    
    for (const auto& pair : plugins) {
        visited[pair.first] = false;
        inStack[pair.first] = false;
    }
    
    for (const auto& pair : plugins) {
        if (!visited[pair.first]) {
            topologicalSort(pair.first, visited, inStack, result);
        }
    }
    
    return result;
}

void PluginManager::topologicalSort(const std::string& pluginName, 
                                   std::unordered_map<std::string, bool>& visited,
                                   std::unordered_map<std::string, bool>& inStack,
                                   std::vector<std::string>& result) {
    visited[pluginName] = true;
    inStack[pluginName] = true;
    
    auto pluginIt = plugins.find(pluginName);
    if (pluginIt == plugins.end()) return;
    
    for (const auto& dependency : pluginIt->second->getDependencies()) {
        auto depIt = plugins.find(dependency);
        if (depIt != plugins.end() && !visited[dependency]) {
            topologicalSort(dependency, visited, inStack, result);
        }
    }
    
    inStack[pluginName] = false;
    result.push_back(pluginName);
}

std::vector<std::string> PluginManager::getShutdownOrder() const {
    std::vector<std::string> order = getInitializationOrder();
    std::reverse(order.begin(), order.end());
    return order;
}

std::string PluginManager::extractPluginNameFromFile(const std::string& filepath) {
    // Extract plugin name from file path
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string filename = filepath.substr(lastSlash + 1);
        size_t dot = filename.find_last_of(".");
        if (dot != std::string::npos) {
            return filename.substr(0, dot);
        }
        return filename;
    }
    return filepath;
}

void PluginManager::loadPluginConfig(const std::string& name, const std::string& filepath) {
    // Load default configuration
    auto config = getDefaultPluginConfig(name);
    pImpl->pluginConfigs[name] = PluginConfig(name, PluginType::CUSTOM);
    pImpl->pluginConfigs[name].settings = config;
    pImpl->pluginSettings[name] = config;
}

std::map<std::string, std::string> PluginManager::getDefaultPluginConfig(const std::string& name) {
    // Return default configuration for built-in plugins
    if (name == "core_brain" || name == "memory_system" || name == "neural_dynamics" || 
        name == "neuromodulation") {
        return {
            {"enabled", "true"},
            {"load_priority", "1.0"}
        };
    } else if (name == "logger" || name == "random_generator" || name == "visualization") {
        return {
            {"enabled", "true"},
            {"log_level", "info"},
            {"load_priority", "0.5"}
        };
    } else if (name == "exploration_behavior" || name == "learning_behavior") {
        return {
            {"enabled", "true"},
            {"exploration_rate", "0.3"},
            {"learning_rate", "0.01"},
            {"load_priority", "0.8"}
        };
    } else if (name == "working_memory" || name == "episodic_memory") {
        return {
            {"enabled", "true"},
            {"capacity", "1000"},
            {"persistence", "true"},
            {"load_priority", "0.9"}
        };
    }
    
    return {};
}

// Plugin factory implementations
namespace PluginFactories {
    std::unique_ptr<Plugin> createCoreBrainPlugin() {
        return std::make_unique<CoreBrainPlugin>();
    }
    
    std::unique_ptr<Plugin> createMemorySystemPlugin() {
        return std::make_unique<MemorySystemPlugin>();
    }
    
    std::unique_ptr<Plugin> createNeuralDynamicsPlugin() {
        return std::make_unique<NeuralDynamicsPlugin>();
    }
    
    std::unique_ptr<Plugin> createNeuromodulationPlugin() {
        return std::make_unique<NeuromodulationPlugin>();
    }
    
    std::unique_ptr<Plugin> createLoggerPlugin() {
        return std::make_unique<LoggerPlugin>();
    }
    
    std::unique_ptr<Plugin> createRandomGeneratorPlugin() {
        return std::make_unique<RandomGeneratorPlugin>();
    }
    
    std::unique_ptr<Plugin> createVisualizationPlugin() {
        return std::make_unique<VisualizationPlugin>();
    }
    
    std::unique_ptr<Plugin> createExplorationBehaviorPlugin() {
        return std::make_unique<ExplorationBehaviorPlugin>();
    }
    
    std::unique_ptr<Plugin> createLearningBehaviorPlugin() {
        return std::make_unique<LearningBehaviorPlugin>();
    }
    
    std::unique_ptr<Plugin> createWorkingMemoryPlugin() {
        return std::make_unique<WorkingMemoryPlugin>();
    }
    
    std::unique_ptr<Plugin> createEpisodicMemoryPlugin() {
        return std::make_unique<EpisodicMemoryPlugin>();
    }
}

// Plugin manifest implementations
std::string PluginManifest::toString() const {
    nlohmann::json json;
    json["name"] = name;
    json["version"] = version;
    json["description"] = description;
    json["type"] = static_cast<int>(type);
    json["author"] = author;
    json["license"] = license;
    json["dependencies"] = dependencies;
    json["capabilities"] = capabilities;
    json["metadata"] = metadata;
    
    return json.dump(2);
}

PluginManifest PluginManifest::fromString(const std::string& data) {
    try {
        nlohmann::json json = nlohmann::json::parse(data);
        PluginManifest manifest;
        manifest.name = json.value("name", "");
        manifest.version = json.value("version", "");
        manifest.description = json.value("description", "");
        manifest.type = static_cast<PluginType>(json.value("type", 0));
        manifest.author = json.value("author", "");
        manifest.license = json.value("license", "");
        manifest.dependencies = json.value("dependencies", nlohmann::json::array()).get<std::vector<std::string>>();
        manifest.capabilities = json.value("capabilities", nlohmann::json::array()).get<std::vector<std::string>>();
        manifest.metadata = json.value("metadata", nlohmann::json::object()).get<std::map<std::string, std::string>>();
        
        return manifest;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Failed to parse plugin manifest: " + std::string(e.what()));
    }
}

} // namespace nlm