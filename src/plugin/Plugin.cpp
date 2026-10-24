#include "Plugin.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <dlfcn.h>

namespace nlm {
namespace plugin {

// PluginManager implementation
PluginManager::PluginManager() {
    // Register default factories
    registerFactory(std::make_unique<FilePluginLoader>());
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::string pluginName = extractPluginName(pluginPath);
    if (hasPlugin(pluginName)) {
        return false; // Plugin already loaded
    }
    
    // Try to load via factories
    for (const auto& pair : factories_) {
        PluginFactory* factory = pair.second.get();
        if (isPluginAvailableForFactory(pluginPath, factory)) {
            return loadPluginFromFactory(pluginName, pair.first);
        }
    }
    
    // Try direct file loading
    return loadPluginFromFile(pluginPath);
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end()) {
        return false;
    }
    
    // Shutdown plugin
    it->second->shutdown();
    
    // Remove from loaded plugins
    loadedPlugins_.erase(it);
    
    // Remove from plugin paths
    pluginPaths_.erase(pluginName);
    
    // Remove from plugin configs
    pluginConfigs_.erase(pluginName);
    
    return true;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    if (!hasPlugin(pluginName)) {
        return false;
    }
    
    std::string pluginPath = getPluginPath(pluginName);
    if (pluginPath.empty()) {
        return false;
    }
    
    // Unload existing plugin
    unloadPlugin(pluginName);
    
    // Reload plugin
    return loadPlugin(pluginPath);
}

bool PluginManager::hasPlugin(const std::string& pluginName) const {
    return loadedPlugins_.find(pluginName) != loadedPlugins_.end();
}

void PluginManager::clearPlugins() {
    // Shutdown all plugins
    for (auto& pair : loadedPlugins_) {
        pair.second->shutdown();
    }
    
    // Clear all plugin containers
    loadedPlugins_.clear();
    pluginPaths_.clear();
    pluginConfigs_.clear();
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::vector<std::string> names;
    names.reserve(loadedPlugins_.size());
    
    for (const auto& pair : loadedPlugins_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    std::vector<std::string> availablePlugins;
    
    // Collect plugins from factories
    for (const auto& pair : factories_) {
        PluginFactory* factory = pair.second.get();
        std::vector<std::string> factoryPlugins = factory->getAvailablePlugins();
        availablePlugins.insert(availablePlugins.end(), 
                              factoryPlugins.begin(), factoryPlugins.end());
    }
    
    // Remove duplicates
    std::sort(availablePlugins.begin(), availablePlugins.end());
    availablePlugins.erase(std::unique(availablePlugins.begin(), availablePlugins.end()), availablePlugins.end());
    
    return availablePlugins;
}

Plugin* PluginManager::getPlugin(const std::string& pluginName) {
    auto it = loadedPlugins_.find(pluginName);
    return it != loadedPlugins_.end() ? it->second.get() : nullptr;
}

std::vector<Plugin*> PluginManager::getPluginsOfType(PluginType type) const {
    std::vector<Plugin*> plugins;
    
    for (const auto& pair : loadedPlugins_) {
        Plugin* plugin = pair.second.get();
        if (plugin->getType() == type) {
            plugins.push_back(plugin);
        }
    }
    
    return plugins;
}

bool PluginManager::configurePlugin(const std::string& pluginName,
                                   const std::map<std::string, std::any>& config) {
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end()) {
        return false;
    }
    
    pluginConfigs_[pluginName] = config;
    
    // Re-initialize plugin with new configuration
    it->second->shutdown();
    if (!it->second->initialize(config)) {
        return false;
    }
    
    return true;
}

std::map<std::string, std::any> PluginManager::getPluginConfig(const std::string& pluginName) const {
    auto it = pluginConfigs_.find(pluginName);
    return it != pluginConfigs_.end() ? it->second : std::map<std::string, std::any>{};
}

void PluginManager::registerFactory(std::unique_ptr<PluginFactory> factory) {
    if (factory) {
        factories_[factory->getFactoryName()] = std::move(factory);
    }
}

bool PluginManager::unregisterFactory(const std::string& factoryName) {
    auto it = factories_.find(factoryName);
    if (it == factories_.end()) {
        return false;
    }
    
    factories_.erase(it);
    return true;
}

PluginFactory* PluginManager::getFactory(const std::string& factoryName) {
    auto it = factories_.find(factoryName);
    return it != factories_.end() ? it->second.get() : nullptr;
}

bool PluginManager::validatePlugin(const std::string& pluginPath) const {
    return !pluginPath.empty() && std::filesystem::exists(pluginPath);
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& pluginName) const {
    auto pluginIt = loadedPlugins_.find(pluginName);
    if (pluginIt == loadedPlugins_.end()) {
        return std::vector<std::string>{};
    }
    
    return pluginIt->second->getDependencies();
}

std::string PluginManager::getPluginVersion(const std::string& pluginName) const {
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end()) {
        return "";
    }
    
    return it->second->getVersion();
}

std::string PluginManager::getPluginDescription(const std::string& pluginName) const {
    auto it = loadedPlugins_.find(pluginName);
    if (it == loadedPlugins_.end()) {
        return "";
    }
    
    return it->second->getDescription();
}

bool PluginManager::loadPluginFromFactory(const std::string& pluginName, const std::string& factoryName) {
    PluginFactory* factory = getFactory(factoryName);
    if (!factory) {
        return false;
    }
    
    std::unique_ptr<Plugin> plugin = factory->createPlugin(pluginName);
    if (!plugin) {
        return false;
    }
    
    // Initialize plugin with default configuration
    std::map<std::string, std::any> config;
    if (!plugin->initialize(config)) {
        return false;
    }
    
    // Store plugin
    loadedPlugins_[pluginName] = std::move(plugin);
    
    return true;
}

bool PluginManager::loadPluginFromFile(const std::string& pluginPath) {
    std::string pluginName = extractPluginName(pluginPath);
    if (pluginName.empty()) {
        return false;
    }
    
    // Try each factory to load the plugin
    for (const auto& pair : factories_) {
        PluginFactory* factory = pair.second.get();
        if (isPluginAvailableForFactory(pluginPath, factory)) {
            return loadPluginFromFactory(pluginName, pair.first);
        }
    }
    
    return false;
}

std::string PluginManager::extractPluginName(const std::string& path) const {
    std::filesystem::path filepath(path);
    return filepath.stem().string();
}

bool PluginManager::isPluginAvailableForFactory(const std::string& pluginPath, PluginFactory* factory) const {
    if (!factory) return false;
    
    std::vector<std::string> supportedExtensions = factory->getSupportedExtensions();
    std::filesystem::path filepath(pluginPath);
    std::string extension = filepath.extension().string();
    
    if (extension.empty()) extension = "." + extension;
    
    for (const auto& ext : supportedExtensions) {
        if (extension == ext) {
            return true;
        }
    }
    
    return false;
}

std::string PluginManager::getPluginPath(const std::string& pluginName) const {
    auto it = pluginPaths_.find(pluginName);
    return it != pluginPaths_.end() ? it->second : "";
}

// FilePluginLoader implementation
FilePluginLoader::FilePluginLoader() = default;

bool FilePluginLoader::canLoad(const std::string& path) const {
    if (path.empty()) return false;
    
    std::filesystem::path filepath(path);
    std::string extension = filepath.extension().string();
    if (extension.empty()) extension = "." + extension;
    
    std::vector<std::string> supportedExtensions = getSupportedExtensions();
    return std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end();
}

std::unique_ptr<Plugin> FilePluginLoader::loadPlugin(const std::string& path) {
    // This is a simplified implementation - a real implementation would
    // dynamically load shared libraries and instantiate plugins
    // For now, return nullptr to indicate not implemented
    return nullptr;
}

std::vector<std::string> FilePluginLoader::getSupportedExtensions() const {
    return {".so", ".dll", ".dylib"};
}

// PluginConfig implementation
PluginConfig::PluginConfig() = default;

void PluginConfig::addPlugin(const PluginEntry& entry) {
    if (isValidPluginName(entry.name)) {
        plugins_[entry.name] = entry;
    }
}

bool PluginConfig::removePlugin(const std::string& name) {
    auto it = plugins_.find(name);
    if (it == plugins_.end()) {
        return false;
    }
    
    plugins_.erase(it);
    return true;
}

bool PluginConfig::updatePlugin(const std::string& name, const PluginEntry& entry) {
    if (!isValidPluginName(entry.name)) {
        return false;
    }
    
    plugins_[entry.name] = entry;
    return true;
}

PluginConfig::PluginEntry* PluginConfig::getPlugin(const std::string& name) {
    auto it = plugins_.find(name);
    return it != plugins_.end() ? &it->second : nullptr;
}

const PluginConfig::PluginEntry* PluginConfig::getPlugin(const std::string& name) const {
    auto it = plugins_.find(name);
    return it != plugins_.end() ? &it->second : nullptr;
}

bool PluginConfig::hasPlugin(const std::string& name) const {
    return plugins_.find(name) != plugins_.end();
}

std::vector<std::string> PluginConfig::getPluginNames() const {
    std::vector<std::string> names;
    names.reserve(plugins_.size());
    
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool PluginConfig::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    return loadFromString(content);
}

bool PluginConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.good()) {
        return false;
    }
    
    std::string content = saveToString();
    file << content;
    return file.good();
}

bool PluginConfig::loadFromString(const std::string& content) {
    // Simple JSON/YAML parsing would go here
    // For now, just clear validation errors
    validationErrors_.clear();
    
    if (!validate()) {
        return false;
    }
    
    // Parse content (simplified)
    plugins_.clear();
    
    return true;
}

std::string PluginConfig::saveToString() const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    
    for (const auto& pair : plugins_) {
        const PluginEntry& entry = pair.second;
        oss << "  \"" << entry.name << "\": {" << std::endl;
        oss << "    \"path\": \"" << entry.path << "\"," << std::endl;
        oss << "    \"autoLoad\": " << (entry.autoLoad ? "true" : "false") << "," << std::endl;
        oss << "    \"enabled\": " << (entry.enabled ? "true" : "false") << std::endl;
        oss << "  }," << std::endl;
    }
    
    oss << "}" << std::endl;
    return oss.str();
}

bool PluginConfig::validate() const {
    validationErrors_.clear();
    
    for (const auto& pair : plugins_) {
        const PluginEntry& entry = pair.second;
        
        if (entry.name.empty()) {
            validationErrors_.push_back("Plugin name cannot be empty");
        }
        
        if (!entry.path.empty() && !isValidPath(entry.path)) {
            validationErrors_.push_back("Invalid plugin path for plugin: " + entry.name);
        }
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> PluginConfig::getValidationErrors() const {
    return validationErrors_;
}

std::string PluginConfig::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool PluginConfig::isValidPluginName(const std::string& name) {
    return !name.empty() && 
           std::all_of(name.begin(), name.end(),
                      [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

bool PluginConfig::isValidPath(const std::string& path) {
    return !path.empty();
}

// Default brain component plugin implementation
bool DefaultBrainComponentPlugin::initialize(const std::map<std::string, std::any>& config) {
    initialized_ = true;
    return true;
}

void DefaultBrainComponentPlugin::shutdown() {
    initialized_ = false;
}

bool DefaultBrainComponentPlugin::isInitialized() const {
    return initialized_;
}

std::vector<std::string> DefaultBrainComponentPlugin::getDependencies() const {
    return {};
}

std::vector<std::string> DefaultBrainComponentPlugin::getOptionalDependencies() const {
    return {};
}

bool DefaultBrainComponentPlugin::canHandle(const std::string& key) const {
    return key == "brain_component" || key == "neural_region" || key == "neuron_population";
}

std::any DefaultBrainComponentPlugin::execute(const std::string& operation, 
                                             const std::map<std::string, std::any>& parameters) {
    // Implementation would depend on the specific operation
    return std::any();
}

std::string DefaultBrainComponentPlugin::getComponentId() const {
    return "default_brain_component";
}

std::any DefaultBrainComponentPlugin::createComponent(const std::map<std::string, std::any>& config) {
    // Implementation would create a brain component
    return std::any();
}

bool DefaultBrainComponentPlugin::configureComponent(const std::string& componentId,
                                                    const std::map<std::string, std::any>& config) {
    // Implementation would configure a brain component
    return true;
}

bool DefaultBrainComponentPlugin::destroyComponent(const std::string& componentId) {
    // Implementation would destroy a brain component
    return true;
}

bool DefaultBrainComponentPlugin::activateComponent(const std::string& componentId) {
    // Implementation would activate a brain component
    return true;
}

bool DefaultBrainComponentPlugin::deactivateComponent(const std::string& componentId) {
    // Implementation would deactivate a brain component
    return true;
}

bool DefaultBrainComponentPlugin::isComponentActive(const std::string& componentId) const {
    // Implementation would check if a brain component is active
    return false;
}

std::vector<std::string> DefaultBrainComponentPlugin::getActiveComponents() const {
    return {};
}

// Default plasticity rule plugin implementation
bool DefaultPlasticityRulePlugin::initialize(const std::map<std::string, std::any>& config) {
    initialized_ = true;
    
    // Extract configuration
    if (config.find("rule_id") != config.end()) {
        ruleId_ = std::any_cast<std::string>(config.at("rule_id"));
    }
    
    if (config.find("learning_rate") != config.end()) {
        learningRate_ = std::any_cast<float>(config.at("learning_rate"));
    }
    
    if (config.find("baseline_decay") != config.end()) {
        baselineDecay_ = std::any_cast<float>(config.at("baseline_decay"));
    }
    
    if (config.find("requires_stdp_spikes") != config.end()) {
        requiresSpikes_ = std::any_cast<bool>(config.at("requires_stdp_spikes"));
    }
    
    return true;
}

void DefaultPlasticityRulePlugin::shutdown() {
    initialized_ = false;
}

bool DefaultPlasticityRulePlugin::isInitialized() const {
    return initialized_;
}

std::vector<std::string> DefaultPlasticityRulePlugin::getDependencies() const {
    return {"brain_component"};
}

std::vector<std::string> DefaultPlasticityRulePlugin::getOptionalDependencies() const {
    return {"environment"};
}

bool DefaultPlasticityRulePlugin::canHandle(const std::string& key) const {
    return key == "plasticity_rule" || key == "synaptic_weight" || key == "learning";
}

std::any DefaultPlasticityRulePlugin::execute(const std::string& operation,
                                             const std::map<std::string, std::any>& parameters) {
    // Implementation would execute the specific operation
    return std::any();
}

std::string DefaultPlasticityRulePlugin::getRuleId() const {
    return ruleId_;
}

float DefaultPlasticityRulePlugin::getLearningRate() const {
    return learningRate_;
}

float DefaultPlasticityRulePlugin::getBaselineDecay() const {
    return baselineDecay_;
}

bool DefaultPlasticityRulePlugin::requiresPrePostSynapticSpikes() const {
    return requiresSpikes_;
}

float DefaultPlasticityRulePlugin::computeWeightChange(const std::map<std::string, std::any>& preSynapticInfo,
                                                     const std::map<std::string, std::any>& postSynapticInfo,
                                                     const std::map<std::string, std::any>& synapseInfo) {
    // Implementation would compute synaptic weight change
    return 0.0f;
}

bool DefaultPlasticityRulePlugin::shouldApplyPlasticity(const std::map<std::string, std::any>& context) {
    // Implementation would determine if plasticity should be applied
    return false;
}

} // namespace plugin
} // namespace nlm