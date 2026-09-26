#include "ModuleSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct ModuleRegistry::Impl {
    // Module management
    std::unordered_map<std::string, std::unique_ptr<IModule>> modules;
    std::unordered_map<std::string, ModuleMetadata> moduleMetadata;
    std::unordered_map<std::string, ModuleState> moduleStates;
    std::unordered_map<std::string, std::map<std::string, std::any>> moduleConfigs;
    std::unordered_map<std::string, std::map<std::string, std::any>> moduleServices;
    std::unordered_map<std::string, std::vector<std::string>> moduleExtensionPoints;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Module loader
    std::unique_ptr<IModuleLoader> moduleLoader;
    
    // Statistics
    size_t registerCount = 0;
    size_t loadCount = 0;
    size_t unloadCount = 0;
    size_t initializeCount = 0;
    size_t errorCount = 0;
};

ModuleRegistry::ModuleRegistry() : pImpl(std::make_unique<Impl>()) {
    pImpl->moduleLoader = std::make_unique<FileModuleLoader>();
    pImpl->moduleLoader->setModuleDirectory("./modules");
    NLM_LOG_INFO("ModuleRegistry initialized");
}

ModuleRegistry::~ModuleRegistry() {
    NLM_LOG_INFO("ModuleRegistry shutdown");
}

bool ModuleRegistry::loadModule(const std::string& moduleName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->modules.find(moduleName);
    if (it == pImpl->modules.end()) {
        NLM_LOG_ERROR("Module not found: " + moduleName);
        return false;
    }
    
    auto& module = it->second;
    if (!module) {
        NLM_LOG_ERROR("Module is null: " + moduleName);
        return false;
    }
    
    if (pImpl->moduleStates[moduleName] == ModuleState::Initialized) {
        NLM_LOG_INFO("Module already loaded: " + moduleName);
        return true;
    }
    
    // Check dependencies
    auto dependencies = resolveDependencies(moduleName);
    if (!dependencies.empty()) {
        NLM_LOG_ERROR("Module has unresolved dependencies: " + moduleName);
        return false;
    }
    
    // Check conflicts
    if (!checkConflicts(moduleName)) {
        NLM_LOG_ERROR("Module conflicts detected: " + moduleName);
        return false;
    }
    
    NLM_LOG_INFO("Loading module: " + moduleName);
    
    try {
        // Load the module
        if (!module->load()) {
            NLM_LOG_ERROR("Failed to load module: " + moduleName);
            pImpl->moduleStates[moduleName] = ModuleState::Error;
            pImpl->errorCount++;
            return false;
        }
        
        pImpl->moduleStates[moduleName] = ModuleState::Loaded;
        
        // Initialize the module
        if (!initializeModule(moduleName)) {
            NLM_LOG_ERROR("Failed to initialize module: " + moduleName);
            module->unload();
            pImpl->moduleStates[moduleName] = ModuleState::Error;
            pImpl->errorCount++;
            return false;
        }
        
        pImpl->moduleStates[moduleName] = ModuleState::Initialized;
        pImpl->loadCount++;
        
        NLM_LOG_INFO("Module loaded successfully: " + moduleName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading module " + moduleName + ": " + std::string(e.what()));
        pImpl->moduleStates[moduleName] = ModuleState::Error;
        pImpl->errorCount++;
        return false;
    }
}

bool ModuleRegistry::unloadModule(const std::string& moduleName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->modules.find(moduleName);
    if (it == pImpl->modules.end()) {
        NLM_LOG_ERROR("Module not found: " + moduleName);
        return false;
    }
    
    auto& module = it->second;
    if (!module) {
        NLM_LOG_ERROR("Module is null: " + moduleName);
        return false;
    }
    
    NLM_LOG_INFO("Unloading module: " + moduleName);
    
    try {
        module->shutdown();
        if (!module->unload()) {
            NLM_LOG_WARNING("Module unload returned false: " + moduleName);
        }
        
        pImpl->moduleStates[moduleName] = ModuleState::Unloaded;
        pImpl->unloadCount++;
        
        NLM_LOG_INFO("Module unloaded successfully: " + moduleName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception unloading module " + moduleName + ": " + std::string(e.what()));
        return false;
    }
}

bool ModuleRegistry::enableModule(const std::string& moduleName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->modules.find(moduleName);
    if (it == pImpl->modules.end()) {
        NLM_LOG_ERROR("Module not found: " + moduleName);
        return false;
    }
    
    pImpl->moduleMetadata[moduleName].enabled = true;
    
    // If module is already loaded, reload it
    if (pImpl->moduleStates[moduleName] == ModuleState::Initialized) {
        unloadModule(moduleName);
        loadModule(moduleName);
    }
    
    NLM_LOG_INFO("Module enabled: " + moduleName);
    return true;
}

bool ModuleRegistry::disableModule(const std::string& moduleName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->modules.find(moduleName);
    if (it == pImpl->modules.end()) {
        NLM_LOG_ERROR("Module not found: " + moduleName);
        return false;
    }
    
    // If module is loaded, unload it first
    if (pImpl->moduleStates[moduleName] == ModuleState::Initialized) {
        unloadModule(moduleName);
    }
    
    pImpl->moduleMetadata[moduleName].enabled = false;
    
    NLM_LOG_INFO("Module disabled: " + moduleName);
    return true;
}

bool ModuleRegistry::registerModule(std::unique_ptr<IModule> module) {
    if (!module) {
        NLM_LOG_ERROR("Cannot register null module");
        return false;
    }
    
    const auto& metadata = module->getMetadata();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if module with same name already exists
    if (pImpl->modules.find(metadata.name) != pImpl->modules.end()) {
        NLM_LOG_ERROR("Module already registered: " + metadata.name);
        return false;
    }
    
    pImpl->modules[metadata.name] = std::move(module);
    pImpl->moduleMetadata[metadata.name] = metadata;
    pImpl->moduleStates[metadata.name] = ModuleState::Unloaded;
    
    NLM_LOG_INFO("Module registered: " + metadata.name + " (v" + metadata.version.toString() + ")");
    pImpl->registerCount++;
    return true;
}

bool ModuleRegistry::unregisterModule(const std::string& moduleName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->modules.find(moduleName);
    if (it == pImpl->modules.end()) {
        NLM_LOG_ERROR("Module not found: " + moduleName);
        return false;
    }
    
    // If module is loaded, unload it first
    if (pImpl->moduleStates[moduleName] == ModuleState::Initialized) {
        unloadModule(moduleName);
    }
    
    pImpl->modules.erase(it);
    pImpl->moduleMetadata.erase(moduleName);
    pImpl->moduleStates.erase(moduleName);
    pImpl->moduleConfigs.erase(moduleName);
    pImpl->moduleServices.erase(moduleName);
    pImpl->moduleExtensionPoints.erase(moduleName);
    
    NLM_LOG_INFO("Module unregistered: " + moduleName);
    return true;
}

std::vector<std::string> ModuleRegistry::getModuleNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->modules.size());
    for (const auto& pair : pImpl->modules) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> ModuleRegistry::getModuleNames(ModuleCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->moduleMetadata) {
        if (pair.second.category == category) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::vector<std::string> ModuleRegistry::getModuleNames(const std::vector<ModuleCategory>& categories) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->moduleMetadata) {
        if (std::find(categories.begin(), categories.end(), pair.second.category) != categories.end()) {
            names.push_back(pair.first);
        }
    }
    return names;
}

ModuleMetadata ModuleRegistry::getModuleMetadata(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->moduleMetadata.find(moduleName);
    if (it == pImpl->moduleMetadata.end()) {
        NLM_LOG_ERROR("Module metadata not found: " + moduleName);
        return ModuleMetadata();
    }
    
    return it->second;
}

ModuleState ModuleRegistry::getModuleState(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->moduleStates.find(moduleName);
    if (it == pImpl->moduleStates.end()) {
        return ModuleState::Unloaded;
    }
    
    return it->second;
}

std::vector<std::string> ModuleRegistry::resolveDependencies(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->moduleMetadata.find(moduleName);
    if (it == pImpl->moduleMetadata.end()) {
        NLM_LOG_ERROR("Module metadata not found: " + moduleName);
        return {};
    }
    
    const auto& dependencies = it->second.dependencies;
    std::vector<std::string> unsatisfied;
    
    for (const auto& dep : dependencies) {
        if (pImpl->moduleStates.find(dep) == pImpl->moduleStates.end() ||
            pImpl->moduleStates.at(dep) != ModuleState::Initialized) {
            unsatisfied.push_back(dep);
        }
    }
    
    return unsatisfied;
}

bool ModuleRegistry::checkConflicts(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->moduleMetadata.find(moduleName);
    if (it == pImpl->moduleMetadata.end()) {
        NLM_LOG_ERROR("Module metadata not found: " + moduleName);
        return false;
    }
    
    const auto& conflicts = it->second.conflictingModules;
    
    for (const auto& conflict : conflicts) {
        auto conflictIt = pImpl->moduleMetadata.find(conflict);
        if (conflictIt != pImpl->moduleMetadata.end() && conflictIt->second.enabled) {
            NLM_LOG_ERROR("Module " + moduleName + " conflicts with enabled module: " + conflict);
            return false;
        }
    }
    
    return true;
}

bool ModuleRegistry::configureModule(const std::string& moduleName,
                                   const std::map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->modules.find(moduleName);
    if (it == pImpl->modules.end()) {
        NLM_LOG_ERROR("Module not found: " + moduleName);
        return false;
    }
    
    pImpl->moduleConfigs[moduleName] = config;
    it->second->configure(config);
    NLM_LOG_INFO("Module configuration updated: " + moduleName);
    return true;
}

std::map<std::string, std::any> ModuleRegistry::getModuleConfig(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->moduleConfigs.find(moduleName);
    if (it == pImpl->moduleConfigs.end()) {
        return std::map<std::string, std::any>();
    }
    
    return it->second;
}

bool ModuleRegistry::initializeAllModules() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Initializing all modules");
    
    std::vector<std::string> loadedModules;
    
    // First pass: load all enabled modules
    for (const auto& pair : pImpl->modules) {
        const auto& moduleName = pair.first;
        const auto& metadata = pImpl->moduleMetadata[moduleName];
        
        if (!metadata.enabled) {
            continue;
        }
        
        if (loadModule(moduleName)) {
            loadedModules.push_back(moduleName);
        }
    }
    
    NLM_LOG_INFO("Successfully loaded " + std::to_string(loadedModules.size()) + " modules");
    
    // Check for initialization errors
    for (const auto& name : loadedModules) {
        if (pImpl->moduleStates[name] != ModuleState::Initialized) {
            NLM_LOG_ERROR("Module initialization failed: " + name);
            return false;
        }
    }
    
    return true;
}

void ModuleRegistry::shutdownAllModules() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Shutting down all modules");
    
    // Unload modules in reverse order
    std::vector<std::string> moduleNames;
    for (const auto& pair : pImpl->modules) {
        moduleNames.push_back(pair.first);
    }
    
    for (auto it = moduleNames.rbegin(); it != moduleNames.rend(); ++it) {
        unloadModule(*it);
    }
}

bool ModuleRegistry::reloadModule(const std::string& moduleName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Shutdown if initialized
    if (pImpl->moduleStates[moduleName] == ModuleState::Initialized) {
        unloadModule(moduleName);
    }
    
    // Load again
    return loadModule(moduleName);
}

std::string ModuleRegistry::getSystemStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "Module System Status:" << std::endl;
    ss << "  Registered Modules: " << pImpl->modules.size() << std::endl;
    ss << "  Load Count: " << pImpl->loadCount << std::endl;
    ss << "  Unload Count: " << pImpl->unloadCount << std::endl;
    ss << "  Error Count: " << pImpl->errorCount << std::endl;
    
    int initializedCount = 0;
    for (const auto& state : pImpl->moduleStates) {
        if (state.second == ModuleState::Initialized) {
            initializedCount++;
        }
    }
    
    ss << "  Initialized Modules: " << initializedCount << std::endl;
    
    auto moduleNames = getModuleNames();
    for (const auto& name : moduleNames) {
        ss << "  " << name << ": " << static_cast<int>(pImpl->moduleStates.at(name)) << std::endl;
    }
    
    return ss.str();
}

ModuleRegistry& ModuleRegistry::getInstance() {
    static ModuleRegistry instance;
    return instance;
}

void ModuleRegistry::reset() {
    static ModuleRegistry instance;
    // Clean up all modules
    for (const auto& pair : instance.pImpl->modules) {
        instance.unregisterModule(pair.first);
    }
    instance.pImpl->modules.clear();
    instance.pImpl->moduleMetadata.clear();
    instance.pImpl->moduleStates.clear();
    instance.pImpl->moduleConfigs.clear();
    instance.pImpl->moduleServices.clear();
    instance.pImpl->moduleExtensionPoints.clear();
    instance.pImpl->registerCount = 0;
    instance.pImpl->loadCount = 0;
    instance.pImpl->unloadCount = 0;
    instance.pImpl->initializeCount = 0;
    instance.pImpl->errorCount = 0;
}

bool ModuleRegistry::registerService(const std::string& moduleName,
                                   const std::string& serviceName,
                                   std::any service) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->modules.find(moduleName);
    if (it == pImpl->modules.end()) {
        NLM_LOG_ERROR("Module not found: " + moduleName);
        return false;
    }
    
    pImpl->moduleServices[serviceName] = service;
    NLM_LOG_INFO("Service registered by module " + moduleName + ": " + serviceName);
    return true;
}

std::any ModuleRegistry::getService(const std::string& serviceName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->moduleServices.find(serviceName);
    if (it == pImpl->moduleServices.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return std::any();
    }
    
    return it->second;
}

bool ModuleRegistry::hasService(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->moduleServices.find(serviceName) != pImpl->moduleServices.end();
}

bool ModuleRegistry::registerExtensionPoint(const std::string& moduleName,
                                           const std::string& extensionPointName,
                                           const std::type_index& type,
                                           bool optional) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->modules.find(moduleName);
    if (it == pImpl->modules.end()) {
        NLM_LOG_ERROR("Module not found: " + moduleName);
        return false;
    }
    
    pImpl->moduleExtensionPoints[extensionPointName].push_back(moduleName);
    NLM_LOG_INFO("Extension point registered by module " + moduleName + ": " + extensionPointName);
    return true;
}

std::vector<std::string> ModuleRegistry::getExtensionPoints(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> extensionPoints;
    
    for (const auto& pair : pImpl->moduleExtensionPoints) {
        const auto& modules = pair.second;
        if (std::find(modules.begin(), modules.end(), moduleName) != modules.end()) {
            extensionPoints.push_back(pair.first);
        }
    }
    
    return extensionPoints;
}

struct FileModuleLoader::Impl {
    std::string moduleDirectory_;
};

FileModuleLoader::FileModuleLoader() {
    pImpl = std::make_unique<Impl>();
    pImpl->moduleDirectory_ = "./modules";
}

FileModuleLoader::~FileModuleLoader() {
}
bool FileModuleLoader::canLoad(const std::string& modulePath) const {
    // Check if file exists
    return std::filesystem::exists(modulePath);
}

std::unique_ptr<IModule> FileModuleLoader::loadModule(const std::string& modulePath) {
    // This would normally load a module from a file
    // For now, just return nullptr
    NLM_LOG_ERROR("Module loading from file not fully implemented for: " + modulePath);
    return nullptr;
}

bool FileModuleLoader::unloadModule(std::unique_ptr<IModule>& module) {
    // This would normally unload a module
    // For now, just return false
    NLM_LOG_ERROR("Module unloading not fully implemented");
    return false;
}

struct ModuleManager::Impl {
    std::unique_ptr<IModuleRegistry> registry;
    bool initialized = false;
    std::mutex mutex;
};

ModuleManager::ModuleManager() : registry_(ModuleRegistry::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->registry = &registry_;
    NLM_LOG_INFO("ModuleManager created");
}

ModuleManager::~ModuleManager() {
    shutdown();
    NLM_LOG_INFO("ModuleManager destroyed");
}

bool ModuleManager::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("ModuleManager already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing ModuleManager");
    
    try {
        // Initialize the registry
        registry_->initializeAllModules();
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("ModuleManager initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing ModuleManager: " + std::string(e.what()));
        return false;
    }
}

void ModuleManager::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down ModuleManager");
    
    try {
        // Shutdown all modules
        registry_->shutdownAllModules();
        
        pImpl->initialized = false;
        NLM_LOG_INFO("ModuleManager shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down ModuleManager: " + std::string(e.what()));
    }
}

bool ModuleManager::loadModulesFromConfig(const std::string& configFile) {
    // This would normally load modules from a configuration file
    NLM_LOG_INFO("Loading modules from config file: " + configFile);
    return true;
}

bool ModuleManager::saveModuleConfig(const std::string& configFile) const {
    // This would normally save modules to a configuration file
    NLM_LOG_INFO("Saving module config to file: " + configFile);
    return true;
}

std::string ModuleManager::getModuleStatus(const std::string& moduleName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ModuleMetadata metadata = registry_->getModuleMetadata(moduleName);
    ModuleState state = registry_->getModuleState(moduleName);
    
    std::stringstream ss;
    ss << moduleName << " (v" << metadata.version.toString() << ") - ";
    
    switch (state) {
        case ModuleState::Unloaded: ss << "Unloaded"; break;
        case ModuleState::Loading: ss << "Loading"; break;
        case ModuleState::Loaded: ss << "Loaded"; break;
        case ModuleState::Initializing: ss << "Initializing"; break;
        case ModuleState::Initialized: ss << "Initialized"; break;
        case ModuleState::ShuttingDown: ss << "ShuttingDown"; break;
        case ModuleState::Error: ss << "Error"; break;
        case ModuleState::Disabled: ss << "Disabled"; break;
    }
    
    ss << std::endl;
    
    // Add dependency info
    auto deps = registry_->resolveDependencies(moduleName);
    if (!deps.empty()) {
        ss << "  Dependencies: " << deps.size() << std::endl;
        for (const auto& dep : deps) {
            ss << "    - " << dep << std::endl;
        }
    }
    
    return ss.str();
}

std::string ModuleManager::getSystemStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    return registry_->getSystemStatus();
}

bool ModuleManager::loadModule(const std::string& moduleName) {
    return registry_->loadModule(moduleName);
}

bool ModuleManager::unloadModule(const std::string& moduleName) {
    return registry_->unloadModule(moduleName);
}

bool ModuleManager::initializeModule(const std::string& moduleName,
                                   const std::map<std::string, std::any>& config) {
    bool result = loadModule(moduleName);
    if (result) {
        result = registry_->configureModule(moduleName, config);
    }
    return result;
}

bool ModuleManager::shutdownModule(const std::string& moduleName) {
    return registry_->unloadModule(moduleName);
}

bool ModuleManager::checkModuleDependencies(const std::string& moduleName) const {
    auto dependencies = registry_->resolveDependencies(moduleName);
    return dependencies.empty();
}

std::vector<std::string> ModuleManager::getUnsatisfiedDependencies(const std::string& moduleName) const {
    return registry_->resolveDependencies(moduleName);
}

ModuleManager& ModuleManager::getInstance() {
    static ModuleManager instance;
    return instance;
}

void ModuleManager::reset() {
    static ModuleManager instance;
    instance.shutdown();
    ModuleRegistry::reset();
}

struct ModuleSystem::Impl {
    std::unique_ptr<ModuleManager> manager;
    bool initialized = false;
    std::mutex mutex;
};

ModuleSystem::ModuleSystem() : manager_(ModuleManager::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->manager = &manager_;
    NLM_LOG_INFO("ModuleSystem created");
}

ModuleSystem::~ModuleSystem() {
    shutdown();
    NLM_LOG_INFO("ModuleSystem destroyed");
}

bool ModuleSystem::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("ModuleSystem already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing ModuleSystem");
    
    try {
        // Initialize the manager
        if (!manager_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize module manager");
            return false;
        }
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("ModuleSystem initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing ModuleSystem: " + std::string(e.what()));
        return false;
    }
}

void ModuleSystem::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down ModuleSystem");
    
    try {
        // Shutdown the manager
        manager_->shutdown();
        
        pImpl->initialized = false;
        NLM_LOG_INFO("ModuleSystem shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down ModuleSystem: " + std::string(e.what()));
    }
}

void ModuleSystem::registerCoreModules() {
    // Register core modules for NLM
    // This would normally create and register various core modules
    NLM_LOG_INFO("Registering core modules");
}

bool ModuleSystem::loadAllModules() {
    return manager_->loadModulesFromConfig("modules.conf");
}

std::string ModuleSystem::getSystemInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "Module System Information:" << std::endl;
    ss << "  Initialized: " << (pImpl->initialized ? "Yes" : "No") << std::endl;
    ss << manager_->getSystemStatus() << std::endl;
    
    return ss.str();
}

ModuleSystem& ModuleSystem::getInstance() {
    static ModuleSystem instance;
    return instance;
}

void ModuleSystem::reset() {
    static ModuleSystem instance;
    instance.shutdown();
    ModuleManager::reset();
}

} // namespace nlm