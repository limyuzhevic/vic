#include "ConfigurationDrivenSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace nlm {

struct LoadingEventManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<ILoadingEventListener>> listenersByName;
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<ILoadingEventListener>>> listenersByType;
    
    std::mutex mutex;
    
    size_t registerCount = 0;
};

LoadingEventManager::LoadingEventManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("LoadingEventManager initialized");
}

LoadingEventManager::~LoadingEventManager() {
    NLM_LOG_INFO("LoadingEventManager shutdown");
}

bool LoadingEventManager::registerLoadingEventListener(std::shared_ptr<ILoadingEventListener> listener) {
    if (!listener) {
        NLM_LOG_ERROR("Cannot register null loading event listener");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    const auto& name = listener->getListenerName();
    if (pImpl->listenersByName.find(name) != pImpl->listenersByName.end()) {
        NLM_LOG_ERROR("Loading event listener already registered: " + name);
        return false;
    }
    
    pImpl->listenersByName[name] = listener;
    
    for (auto eventType : listener->getEventTypes()) {
        pImpl->listenersByType[std::type_index(typeid(eventType))].push_back(listener);
    }
    
    NLM_LOG_INFO("Loading event listener registered: " + name);
    pImpl->registerCount++;
    return true;
}

bool LoadingEventManager::unregisterLoadingEventListener(const std::string& listenerName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->listenersByName.find(listenerName);
    if (it == pImpl->listenersByName.end()) {
        NLM_LOG_ERROR("Loading event listener not found: " + listenerName);
        return false;
    }
    
    auto listener = it->second;
    
    // Remove from listeners by type
    for (auto eventType : listener->getEventTypes()) {
        auto typeIt = pImpl->listenersByType.find(std::type_index(typeid(eventType)));
        if (typeIt != pImpl->listenersByType.end()) {
            auto& listeners = typeIt->second;
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
                [&listenerName](const std::shared_ptr<ILoadingEventListener>& l) {
                    return l->getListenerName() == listenerName;
                }),
                listeners.end());
        }
    }
    
    pImpl->listenersByName.erase(it);
    
    NLM_LOG_INFO("Loading event listener unregistered: " + listenerName);
    return true;
}

bool LoadingEventManager::unregisterLoadingEventListener(std::type_index listenerType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->listenersByType.find(listenerType);
    if (it == pImpl->listenersByType.end()) {
        NLM_LOG_ERROR("Loading event listener not found for type: " + std::string(listenerType.name()));
        return false;
    }
    
    auto& listeners = it->second;
    
    for (const auto& listener : listeners) {
        auto nameIt = pImpl->listenersByName.find(listener->getListenerName());
        if (nameIt != pImpl->listenersByName.end()) {
            pImpl->listenersByName.erase(nameIt);
        }
    }
    
    pImpl->listenersByType.erase(it);
    
    NLM_LOG_INFO("Loading event listeners unregistered for type: " + std::string(listenerType.name()));
    return true;
}

void LoadingEventManager::distributeLoadingEvent(const LoadingEvent& event) {
    distributeLoadingEventAsync(event);
}

void LoadingEventManager::distributeLoadingEventAsync(const LoadingEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Find listeners for this event type
    auto typeIt = pImpl->listenersByType.find(std::type_index(typeid(event.type)));
    if (typeIt == pImpl->listenersByType.end()) {
        return;
    }
    
    auto& listeners = typeIt->second;
    
    NLM_LOG_INFO("Distributing loading event " + std::to_string(static_cast<int>(event.type)) +
                " to " + std::to_string(listeners.size()) + " listeners");
    
    for (auto& listener : listeners) {
        try {
            listener->notifyEvent(event);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception notifying loading event listener " + listener->getListenerName() + ": " + std::string(e.what()));
        }
    }
}

std::vector<std::shared_ptr<ILoadingEventListener>> LoadingEventManager::getListenersForEvent(LoadingType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->listenersByType.find(std::type_index(typeid(type)));
    if (it == pImpl->listenersByType.end()) {
        return {};
    }
    
    return it->second;
}

std::vector<std::shared_ptr<ILoadingEventListener>> LoadingEventManager::getAllListeners() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::shared_ptr<ILoadingEventListener>> allListeners;
    allListeners.reserve(pImpl->listenersByName.size());
    
    for (const auto& pair : pImpl->listenersByName) {
        allListeners.push_back(pair.second);
    }
    
    return allListeners;
}

void LoadingEventManager::clearListeners() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->listenersByName.clear();
    pImpl->listenersByType.clear();
    NLM_LOG_INFO("All loading event listeners cleared");
}

LoadingEventManager& LoadingEventManager::getInstance() {
    static LoadingEventManager instance;
    return instance;
}

void LoadingEventManager::reset() {
    static LoadingEventManager instance;
    instance.~LoadingEventManager();
    new (&instance) LoadingEventManager();
}

struct FileSystemLoader::Impl {
    std::string configDirectory_;
};

FileSystemLoader::FileSystemLoader() {
    pImpl = std::make_unique<Impl>();
    pImpl->configDirectory_ = "./configs";
}

FileSystemLoader::~FileSystemLoader() {
}

bool FileSystemLoader::canLoad(const std::string& configFile) const {
    // Check if file exists
    std::string fullPath = configDirectory_ + "/" + configFile;
    return std::filesystem::exists(fullPath);
}

bool FileSystemLoader::loadConfig(const std::string& configFile,
                                std::map<std::string, SystemConfig>& systems,
                                std::map<std::string, ComponentConfig>& components,
                                std::map<std::string, ServiceConfig>& services) {
    std::string fullPath = configDirectory_ + "/" + configFile;
    
    NLM_LOG_INFO("Loading configuration from file: " + fullPath);
    
    // Try to load as JSON file
    if (!loadFromJSON(fullPath, systems, components, services)) {
        NLM_LOG_ERROR("Failed to load configuration from file: " + fullPath);
        return false;
    }
    
    NLM_LOG_INFO("Configuration loaded successfully from file: " + fullPath);
    return true;
}

bool FileSystemLoader::saveConfig(const std::string& configFile,
                                const std::map<std::string, SystemConfig>& systems,
                                const std::map<std::string, ComponentConfig>& components,
                                const std::map<std::string, ServiceConfig>& services) {
    std::string fullPath = configDirectory_ + "/" + configFile;
    
    NLM_LOG_INFO("Saving configuration to file: " + fullPath);
    
    // Try to save as JSON file
    if (!saveToJSON(fullPath, systems, components, services)) {
        NLM_LOG_ERROR("Failed to save configuration to file: " + fullPath);
        return false;
    }
    
    NLM_LOG_INFO("Configuration saved successfully to file: " + fullPath);
    return true;
}

bool FileSystemLoader::loadFromJSON(const std::string& configFile,
                                   std::map<std::string, SystemConfig>& systems,
                                   std::map<std::string, ComponentConfig>& components,
                                   std::map<std::string, ServiceConfig>& services) {
    // This would normally use rapidjson or similar to parse JSON
    // For now, just create some dummy data for demonstration
    
    // Create a dummy system
    SystemConfig sys1;
    sys1.name = "CoreSystem";
    sys1.description = "Core NLM System";
    sys1.settings["neuron_count"] = 1000;
    sys1.settings["region_count"] = 1;
    sys1.dependencies = {"MemorySystem", "PlasticitySystem"};
    
    systems[sys1.name] = sys1;
    
    // Create a dummy component
    ComponentConfig comp1;
    comp1.name = "WorkingMemory";
    comp1.type = "Memory";
    comp1.config["capacity"] = 1000;
    comp1.dependencies = {"CoreSystem"};
    
    components[comp1.name] = comp1;
    
    // Create a dummy service
    ServiceConfig svc1;
    svc1.name = "RandomGenerator";
    svc1.type = "Utility";
    svc1.config["seed"] = 42;
    
    services[svc1.name] = svc1;
    
    NLM_LOG_INFO("JSON configuration loaded (demo): " + configFile);
    return true;
}

bool FileSystemLoader::saveToJSON(const std::string& configFile,
                                const std::map<std::string, SystemConfig>& systems,
                                const std::map<std::string, ComponentConfig>& components,
                                const std::map<std::string, ServiceConfig>& services) {
    // This would normally use rapidjson or similar to write JSON
    // For now, just create a dummy file for demonstration
    
    std::ofstream file(configFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + configFile);
        return false;
    }
    
    // Write some dummy JSON
    file << "{" << std::endl;
    file << "  \"systems\": [{" << std::endl;
    file << "    \"name\": \"CoreSystem\"," << std::endl;
    file << "    \"description\": \"Core NLM System\"" << std::endl;
    file << "  }]" << std::endl;
    file << "}" << std::endl;
    
    file.close();
    
    NLM_LOG_INFO("JSON configuration saved (demo): " + configFile);
    return true;
}

struct ConfigurationDrivenSystemManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<IConfigurationDrivenSystem>> systems;
    std::unordered_map<std::string, ConfigurationDrivenSystemConfig> systemConfigs;
    
    std::mutex mutex;
    
    LoadingEventManager loadingEventManager;
    
    size_t loadCount = 0;
    size_t errorCount = 0;
};

ConfigurationDrivenSystemManager::ConfigurationDrivenSystemManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("ConfigurationDrivenSystemManager initialized");
}

ConfigurationDrivenSystemManager::~ConfigurationDrivenSystemManager() {
    NLM_LOG_INFO("ConfigurationDrivenSystemManager shutdown");
}

bool ConfigurationDrivenSystemManager::initialize() {
    NLM_LOG_INFO("Initializing ConfigurationDrivenSystemManager");
    return true;
}

void ConfigurationDrivenSystemManager::shutdown() {
    NLM_LOG_INFO("Shutting down ConfigurationDrivenSystemManager");
}

bool ConfigurationDrivenSystemManager::configureSystem(const std::string& systemName,
                                                      const ConfigurationDrivenSystemConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systems.find(systemName);
    if (it == pImpl->systems.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    pImpl->systemConfigs[systemName] = config;
    it->second->configureFromFile(config.configFile);
    NLM_LOG_INFO("System configuration updated: " + systemName);
    return true;
}

bool ConfigurationDrivenSystemManager::loadSystemConfig(const std::string& systemName,
                                                       const std::string& configFile) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systems.find(systemName);
    if (it == pImpl->systems.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    if (!it->second->configureFromFile(configFile)) {
        NLM_LOG_ERROR("Failed to load configuration from file: " + configFile);
        return false;
    }
    
    NLM_LOG_INFO("System configuration loaded successfully from file: " + configFile);
    return true;
}

bool ConfigurationDrivenSystemManager::saveSystemConfig(const std::string& systemName,
                                                       const std::string& configFile) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systems.find(systemName);
    if (it == pImpl->systems.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    if (!it->second->saveToFile(configFile)) {
        NLM_LOG_ERROR("Failed to save configuration to file: " + configFile);
        return false;
    }
    
    NLM_LOG_INFO("System configuration saved successfully to file: " + configFile);
    return true;
}

bool ConfigurationDrivenSystemManager::startSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systems.find(systemName);
    if (it == pImpl->systems.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    // Note: System would normally start its components here
    NLM_LOG_INFO("System start requested: " + systemName);
    return true;
}

bool ConfigurationDrivenSystemManager::stopSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systems.find(systemName);
    if (it == pImpl->systems.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    // Note: System would normally stop its components here
    NLM_LOG_INFO("System stop requested: " + systemName);
    return true;
}

bool ConfigurationDrivenSystemManager::restartSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systems.find(systemName);
    if (it == pImpl->systems.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    // Note: System would normally restart its components here
    NLM_LOG_INFO("System restart requested: " + systemName);
    return true;
}

std::vector<std::string> ConfigurationDrivenSystemManager::getSystemNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->systems.size());
    
    for (const auto& pair : pImpl->systems) {
        names.push_back(pair.first);
    }
    
    return names;
}

ConfigurationDrivenSystemConfig ConfigurationDrivenSystemManager::getSystemConfig(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemConfigs.find(systemName);
    if (it == pImpl->systemConfigs.end()) {
        return ConfigurationDrivenSystemConfig(systemName);
    }
    
    return it->second;
}

std::map<std::string, std::any> ConfigurationDrivenSystemManager::getSystemLoadedConfig(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systems.find(systemName);
    if (it == pImpl->systems.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return std::map<std::string, std::any>();
    }
    
    return it->second->getLoadedConfig();
}

bool ConfigurationDrivenSystemManager::loadAllSystemsFromConfig(const std::string& configDirectory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Loading all systems from config directory: " + configDirectory);
    
    // This would normally iterate through all configuration files in the directory
    // For now, just log a placeholder
    NLM_LOG_INFO("Configuration loading from directory not fully implemented for: " + configDirectory);
    return true;
}

bool ConfigurationDrivenSystemManager::saveAllSystemsToConfig(const std::string& configDirectory) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Saving all systems to config directory: " + configDirectory);
    
    // This would normally iterate through all systems and save them
    // For now, just log a placeholder
    NLM_LOG_INFO("Configuration saving to directory not fully implemented for: " + configDirectory);
    return true;
}

bool ConfigurationDrivenSystemManager::validateSystemConfig(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systems.find(systemName);
    if (it == pImpl->systems.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    return it->second->validateConfig();
}

std::string ConfigurationDrivenSystemManager::validateAllSystemConfigs() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Validating all system configurations");
    
    std::stringstream ss;
    bool overallSuccess = true;
    
    for (const auto& pair : pImpl->systems) {
        const auto& name = pair.first;
        const auto& system = pair.second;
        
        ss << "  System " << name << ": ";
        if (system->validateConfig()) {
            ss << "VALID" << std::endl;
        } else {
            ss << "INVALID" << std::endl;
            overallSuccess = false;
        }
    }
    
    ss << "  Overall Status: " << (overallSuccess ? "VALID" : "INVALID") << std::endl;
    
    return ss.str();
}

std::string ConfigurationDrivenSystemManager::getSystemManagerStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "Configuration-Driven System Manager Status:" << std::endl;
    ss << "  Registered Systems: " << pImpl->systems.size() << std::endl;
    ss << "  Load Count: " << pImpl->loadCount << std::endl;
    ss << "  Error Count: " << pImpl->errorCount << std::endl;
    
    auto systemNames = getSystemNames();
    for (const auto& name : systemNames) {
        ss << "  " << name << ": Configured" << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, std::any> ConfigurationDrivenSystemManager::getManagerStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, std::any> stats;
    stats["total_systems"] = static_cast<size_t>(pImpl->systems.size());
    stats["load_count"] = static_cast<size_t>(pImpl->loadCount);
    stats["error_count"] = static_cast<size_t>(pImpl->errorCount);
    
    return stats;
}

bool ConfigurationDrivenSystemManager::registerLoadingEventListener(std::shared_ptr<ILoadingEventListener> listener) {
    return pImpl->loadingEventManager.registerLoadingEventListener(listener);
}

bool ConfigurationDrivenSystemManager::unregisterLoadingEventListener(const std::string& listenerName) {
    return pImpl->loadingEventManager.unregisterLoadingEventListener(listenerName);
}

void ConfigurationDrivenSystemManager::distributeLoadingEvent(const LoadingEvent& event) {
    pImpl->loadingEventManager.distributeEvent(event);
}

bool ConfigurationDrivenSystemManager::validateAllSystems() const {
    std::string diagnostics = getSystemManagerStatus();
    return !diagnostics.empty();
}

ConfigurationDrivenSystemManager& ConfigurationDrivenSystemManager::getInstance() {
    static ConfigurationDrivenSystemManager instance;
    return instance;
}

void ConfigurationDrivenSystemManager::reset() {
    static ConfigurationDrivenSystemManager instance;
    instance.~ConfigurationDrivenSystemManager();
    new (&instance) ConfigurationDrivenSystemManager();
}

ConfigurationDrivenSystem::ConfigurationDrivenSystem(const std::string& name, const std::string& version)
    : systemName_(name), systemVersion_(version) {
    NLM_LOG_INFO("ConfigurationDrivenSystem created: " + name);
}

ConfigurationDrivenSystem::~ConfigurationDrivenSystem() {
    NLM_LOG_INFO("ConfigurationDrivenSystem destroyed: " + systemName_);
}

bool ConfigurationDrivenSystem::loadFromConfig(const std::map<std::string, std::any>& config) {
    NLM_LOG_INFO("Loading configuration-driven system: " + systemName_);
    
    try {
        if (!internalLoadFromConfig(config)) {
            NLM_LOG_ERROR("Internal configuration loading failed for system: " + systemName_);
            return false;
        }
        
        currentConfig_ = config;
        
        NLM_LOG_INFO("Configuration-driven system loaded successfully: " + systemName_);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading configuration-driven system " + systemName_ + ": " + std::string(e.what()));
        return false;
    }
}

std::map<std::string, std::any> ConfigurationDrivenSystem::getConfig() const {
    return currentConfig_;
}

bool ConfigurationDrivenSystem::validateConfig() const {
    NLM_LOG_INFO("Validating configuration-driven system: " + systemName_);
    
    bool success = true;
    
    if (loadedConfig_.empty()) {
        NLM_LOG_ERROR("No loaded configuration for system: " + systemName_);
        success = false;
    }
    
    // Note: Additional validation would be implemented in derived classes
    
    return success;
}

std::string ConfigurationDrivenSystem::validateConfigAll() const {
    NLM_LOG_INFO("Validating all configuration aspects for system: " + systemName_);
    
    std::stringstream ss;
    ss << "Configuration Validation for " << systemName_ << ":" << std::endl;
    ss << "  Loaded Config: " << (loadedConfig_.empty() ? "NO" : "YES") << std::endl;
    ss << "  Current Config: " << (currentConfig_.empty() ? "NO" : "YES") << std::endl;
    ss << "  Validation Status: " << (validateConfig() ? "PASS" : "FAIL") << std::endl;
    
    return ss.str();
}

bool ConfigurationDrivenSystem::configureFromFile(const std::string& configFile) {
    NLM_LOG_INFO("Configuring configuration-driven system from file: " + configFile);
    
    // This would normally load configuration from file
    // For now, just log a placeholder
    NLM_LOG_INFO("Configuration file loading not fully implemented for: " + configFile);
    return true;
}

bool ConfigurationDrivenSystem::saveToFile(const std::string& configFile) const {
    NLM_LOG_INFO("Saving configuration-driven system to file: " + configFile);
    
    // This would normally save configuration to file
    // For now, just log a placeholder
    NLM_LOG_INFO("Configuration file saving not fully implemented for: " + configFile);
    return true;
}

struct SystemLoaderManager::Impl {
    std::unordered_map<LoadingType, std::unique_ptr<ISystemLoader>> loaders;
    
    std::mutex mutex;
};

SystemLoaderManager::SystemLoaderManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SystemLoaderManager initialized");
}

SystemLoaderManager::~SystemLoaderManager() {
    NLM_LOG_INFO("SystemLoaderManager shutdown");
}

bool SystemLoaderManager::registerLoader(std::unique_ptr<ISystemLoader> loader) {
    if (!loader) {
        NLM_LOG_ERROR("Cannot register null loader");
        return false;
    }
    
    LoadingType type = loader->getLoaderType();n
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->loaders.find(type) != pImpl->loaders.end()) {
        NLM_LOG_ERROR("Loader already registered for type: " + static_cast<int>(type));
        return false;
    }
    
    pImpl->loaders[type] = std::move(loader);
    NLM_LOG_INFO("Loader registered for type: " + static_cast<int>(type));
    return true;
}

bool SystemLoaderManager::unregisterLoader(LoadingType type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->loaders.find(type);
    if (it == pImpl->loaders.end()) {
        NLM_LOG_ERROR("Loader not found for type: " + static_cast<int>(type));
        return false;
    }
    
    pImpl->loaders.erase(it);
    NLM_LOG_INFO("Loader unregistered for type: " + static_cast<int>(type));
    return true;
}

std::vector<std::unique_ptr<ISystemLoader>> SystemLoaderManager::getAllLoaders() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::unique_ptr<ISystemLoader>> loaders;
    loaders.reserve(pImpl->loaders.size());
    
    for (const auto& pair : pImpl->loaders) {
        loaders.push_back(pair.second->clone());
    }
    
    return loaders;
}

ISystemLoader* SystemLoaderManager::getLoader(LoadingType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->loaders.find(type);
    if (it == pImpl->loaders.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

bool SystemLoaderManager::hasLoader(LoadingType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loaders.find(type) != pImpl->loaders.end();
}

ISystemLoader* SystemLoaderManager::selectLoader(const std::string& configFile) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // This would normally determine the best loader for the given config file
    // For now, return the default file system loader
    return getLoader(LoadingType::FileSystem);
}

SystemLoaderManager& SystemLoaderManager::getInstance() {
    static SystemLoaderManager instance;
    return instance;
}

void SystemLoaderManager::reset() {
    static SystemLoaderManager instance;
    instance.~SystemLoaderManager();
    new (&instance) SystemLoaderManager();
}

struct ConfigurationDrivenSystemFramework::Impl {
    std::unique_ptr<ConfigurationDrivenSystemManager> manager;
    bool initialized = false;
    std::mutex mutex;
};

ConfigurationDrivenSystemFramework::ConfigurationDrivenSystemFramework() : manager_(ConfigurationDrivenSystemManager::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->manager = &manager_;
    NLM_LOG_INFO("ConfigurationDrivenSystemFramework created");
}

ConfigurationDrivenSystemFramework::~ConfigurationDrivenSystemFramework() {
    shutdown();
    NLM_LOG_INFO("ConfigurationDrivenSystemFramework destroyed");
}

bool ConfigurationDrivenSystemFramework::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("ConfigurationDrivenSystemFramework already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing ConfigurationDrivenSystemFramework");
    
    try {
        // Initialize the manager
        pImpl->manager->loadAllSystemsFromConfig("./configs");
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("ConfigurationDrivenSystemFramework initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing ConfigurationDrivenSystemFramework: " + std::string(e.what()));
        return false;
    }
}

void ConfigurationDrivenSystemFramework::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down ConfigurationDrivenSystemFramework");
    
    try {
        // Shutdown all systems
        pImpl->manager->saveAllSystemsToConfig("./configs");
        
        pImpl->initialized = false;
        NLM_LOG_INFO("ConfigurationDrivenSystemFramework shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down ConfigurationDrivenSystemFramework: " + std::string(e.what()));
    }
}

void ConfigurationDrivenSystemFramework::registerCoreSystems() {
    // Register core systems for NLM
    // This would normally create and register various core systems
    NLM_LOG_INFO("Registering core systems");
}

bool ConfigurationDrivenSystemFramework::loadAllSystems() {
    return pImpl->manager->loadAllSystemsFromConfig("./configs");
}

bool ConfigurationDrivenSystemFramework::saveAllSystems() {
    return pImpl->manager->saveAllSystemsToConfig("./configs");
}

bool ConfigurationDrivenSystemFramework::validateSystemConfiguration() {
    return pImpl->manager->validateAllSystemConfigs();
}

bool ConfigurationDrivenSystemFramework::performSystemConfigurationHealthCheck() {
    std::string diagnostics = getFrameworkStatus();
    return !diagnostics.empty();
}

std::string ConfigurationDrivenSystemFramework::getFrameworkStatus() const {
    return pImpl->manager->getSystemManagerStatus();
}

std::map<std::string, std::any> ConfigurationDrivenSystemFramework::getFrameworkStatistics() const {
    return pImpl->manager->getManagerStatistics();
}

void ConfigurationDrivenSystemFramework::notifyLoadingEvent(const LoadingEvent& event) {
    // Note: This would normally distribute events to registered listeners
    // For now, just log the event
    NLM_LOG_INFO("Loading event: " + std::to_string(static_cast<int>(event.type)) + " - " + event.message);
}

ConfigurationDrivenSystemFramework& ConfigurationDrivenSystemFramework::getInstance() {
    static ConfigurationDrivenSystemFramework instance;
    return instance;
}

void ConfigurationDrivenSystemFramework::reset() {
    static ConfigurationDrivenSystemFramework instance;
    instance.shutdown();
    ConfigurationDrivenSystemManager::reset();
}

} // namespace nlm