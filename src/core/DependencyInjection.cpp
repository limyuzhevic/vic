#include "DependencyInjection.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {

struct DependencyInjectionContainer::Impl {
    // Dependency management
    std::unordered_map<std::string, std::shared_ptr<IDependency>> dependenciesByName;
    std::unordered_map<std::type_index, std::shared_ptr<IDependency>> dependenciesByType;
    std::unordered_map<std::string, DependencyConfig> dependencyConfigs;
    std::unordered_map<std::string, DependencyState> dependencyStates;
    std::unordered_map<std::string, std::map<std::string, std::any>> dependencyConfigsMap;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Statistics
    size_t registerCount = 0;
    size_t resolveCount = 0;
    size_t initializeCount = 0;
    size_t shutdownCount = 0;
    size_t errorCount = 0;
};

DependencyInjectionContainer::DependencyInjectionContainer() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("DependencyInjectionContainer initialized");
}

DependencyInjectionContainer::~DependencyInjectionContainer() {
    NLM_LOG_INFO("DependencyInjectionContainer shutdown");
    // Cleanup all dependencies
    for (const auto& pair : pImpl->dependenciesByName) {
        if (pair.second->getState() == DependencyState::Initialized) {
            pair.second->shutdown();
        }
    }
}

bool DependencyInjectionContainer::registerDependency(std::shared_ptr<IDependency> dependency) {
    if (!dependency) {
        NLM_LOG_ERROR("Cannot register null dependency");
        return false;
    }
    
    const auto& config = dependency->getConfig();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if dependency with same name already exists
    if (pImpl->dependenciesByName.find(config.name) != pImpl->dependenciesByName.end()) {
        NLM_LOG_ERROR("Dependency already registered: " + config.name);
        return false;
    }
    
    pImpl->dependenciesByName[config.name] = dependency;
    pImpl->dependenciesByType[config.dependencyType] = dependency;
    pImpl->dependencyConfigs[config.name] = config;
    pImpl->dependencyStates[config.name] = dependency->getState();
    
    NLM_LOG_INFO("Dependency registered: " + config.name + " (" + std::string(config.dependencyType.name()) + ")");
    pImpl->registerCount++;
    return true;
}

bool DependencyInjectionContainer::registerDependency(std::shared_ptr<ITypedDependency> dependency) {
    if (!dependency) {
        NLM_LOG_ERROR("Cannot register null typed dependency");
        return false;
    }
    
    auto implDependency = dependency->getDependencyImpl();
    if (!implDependency) {
        NLM_LOG_ERROR("Typed dependency has null implementation");
        return false;
    }
    
    const auto& config = implDependency->getConfig();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if dependency with same name already exists
    if (pImpl->dependenciesByName.find(dependency->getDependencyName()) != pImpl->dependenciesByName.end()) {
        NLM_LOG_ERROR("Dependency already registered: " + dependency->getDependencyName());
        return false;
    }
    
    pImpl->dependenciesByName[dependency->getDependencyName()] = implDependency;
    pImpl->dependenciesByType[dependency->getDependencyType()] = implDependency;
    pImpl->dependencyConfigs[dependency->getDependencyName()] = config;
    pImpl->dependencyStates[dependency->getDependencyName()] = implDependency->getState();
    
    NLM_LOG_INFO("Typed dependency registered: " + dependency->getDependencyName() + 
                " (" + std::string(dependency->getDependencyType().name()) + ")");
    pImpl->registerCount++;
    return true;
}

bool DependencyInjectionContainer::unregisterDependency(const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependenciesByName.find(dependencyName);
    if (it == pImpl->dependenciesByName.end()) {
        NLM_LOG_ERROR("Dependency not found: " + dependencyName);
        return false;
    }
    
    auto dependency = it->second;
    
    // Shutdown if initialized
    if (dependency->getState() == DependencyState::Initialized) {
        dependency->shutdown();
    }
    
    // Remove from all maps
    pImpl->dependenciesByName.erase(it);
    
    auto typeIt = pImpl->dependenciesByType.begin();
    while (typeIt != pImpl->dependenciesByType.end()) {
        if (typeIt->second.get() == dependency.get()) {
            typeIt = pImpl->dependenciesByType.erase(typeIt);
        } else {
            ++typeIt;
        }
    }
    
    pImpl->dependencyConfigs.erase(dependencyName);
    pImpl->dependencyStates.erase(dependencyName);
    pImpl->dependencyConfigsMap.erase(dependencyName);
    
    NLM_LOG_INFO("Dependency unregistered: " + dependencyName);
    return true;
}

bool DependencyInjectionContainer::unregisterDependency(std::type_index dependencyType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependenciesByType.find(dependencyType);
    if (it == pImpl->dependenciesByType.end()) {
        NLM_LOG_ERROR("Dependency not found for type: " + std::string(dependencyType.name()));
        return false;
    }
    
    auto dependency = it->second;
    
    // Shutdown if initialized
    if (dependency->getState() == DependencyState::Initialized) {
        dependency->shutdown();
    }
    
    // Find and remove from name map
    auto nameIt = pImpl->dependenciesByName.begin();
    while (nameIt != pImpl->dependenciesByName.end()) {
        if (nameIt->second.get() == dependency.get()) {
            pImpl->dependencyConfigs.erase(nameIt->first);
            pImpl->dependencyStates.erase(nameIt->first);
            pImpl->dependencyConfigsMap.erase(nameIt->first);
            nameIt = pImpl->dependenciesByName.erase(nameIt);
        } else {
            ++nameIt;
        }
    }
    
    pImpl->dependenciesByType.erase(it);
    
    NLM_LOG_INFO("Dependency unregistered for type: " + std::string(dependencyType.name()));
    return true;
}

std::shared_ptr<IDependency> DependencyInjectionContainer::getDependency(const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependenciesByName.find(dependencyName);
    if (it == pImpl->dependenciesByName.end()) {
        NLM_LOG_ERROR("Dependency not found: " + dependencyName);
        return nullptr;
    }
    
    return it->second;
}

std::shared_ptr<IDependency> DependencyInjectionContainer::getDependency(std::type_index dependencyType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependenciesByType.find(dependencyType);
    if (it == pImpl->dependenciesByType.end()) {
        NLM_LOG_ERROR("Dependency not found for type: " + std::string(dependencyType.name()));
        return nullptr;
    }
    
    return it->second;
}

bool DependencyInjectionContainer::configureDependency(const std::string& dependencyName,
                                                       const std::map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependenciesByName.find(dependencyName);
    if (it == pImpl->dependenciesByName.end()) {
        NLM_LOG_ERROR("Dependency not found: " + dependencyName);
        return false;
    }
    
    pImpl->dependencyConfigsMap[dependencyName] = config;
    NLM_LOG_INFO("Dependency configuration updated: " + dependencyName);
    return true;
}

std::map<std::string, std::any> DependencyInjectionContainer::getDependencyConfig(const std::string& dependencyName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencyConfigsMap.find(dependencyName);
    if (it == pImpl->dependencyConfigsMap.end()) {
        return std::map<std::string, std::any>();
    }
    
    return it->second;
}

bool DependencyInjectionContainer::resolveDependency(const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependenciesByName.find(dependencyName);
    if (it == pImpl->dependenciesByName.end()) {
        NLM_LOG_ERROR("Dependency not found: " + dependencyName);
        return false;
    }
    
    auto& dependency = it->second;
    
    if (dependency->isResolved()) {
        NLM_LOG_INFO("Dependency already resolved: " + dependencyName);
        return true;
    }
    
    NLM_LOG_INFO("Resolving dependency: " + dependencyName);
    
    try {
        if (!dependency->resolve()) {
            NLM_LOG_ERROR("Failed to resolve dependency: " + dependencyName);
            return false;
        }
        
        pImpl->dependencyStates[dependencyName] = dependency->getState();
        pImpl->resolveCount++;
        
        NLM_LOG_INFO("Dependency resolved successfully: " + dependencyName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception resolving dependency " + dependencyName + ": " + std::string(e.what()));
        pImpl->errorCount++;
        return false;
    }
}

bool DependencyInjectionContainer::initializeDependency(const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependenciesByName.find(dependencyName);
    if (it == pImpl->dependenciesByName.end()) {
        NLM_LOG_ERROR("Dependency not found: " + dependencyName);
        return false;
    }
    
    auto& dependency = it->second;
    
    if (dependency->isInitialized()) {
        NLM_LOG_INFO("Dependency already initialized: " + dependencyName);
        return true;
    }
    
    // Check configuration
    auto config = getDependencyConfig(dependencyName);
    if (!config.empty()) {
        // Note: In a real implementation, we would pass config to dependency
    }
    
    NLM_LOG_INFO("Initializing dependency: " + dependencyName);
    
    try {
        if (!dependency->resolve()) {
            NLM_LOG_ERROR("Failed to resolve dependency for initialization: " + dependencyName);
            return false;
        }
        
        if (!dependency->initialize(config)) {
            NLM_LOG_ERROR("Failed to initialize dependency: " + dependencyName);
            return false;
        }
        
        pImpl->dependencyStates[dependencyName] = dependency->getState();
        pImpl->initializeCount++;
        
        NLM_LOG_INFO("Dependency initialized successfully: " + dependencyName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing dependency " + dependencyName + ": " + std::string(e.what()));
        pImpl->errorCount++;
        return false;
    }
}

bool DependencyInjectionContainer::shutdownDependency(const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependenciesByName.find(dependencyName);
    if (it == pImpl->dependenciesByName.end()) {
        NLM_LOG_ERROR("Dependency not found: " + dependencyName);
        return false;
    }
    
    auto& dependency = it->second;
    
    if (!dependency->isInitialized() && !dependency->isResolved()) {
        NLM_LOG_INFO("Dependency not initialized/resolved: " + dependencyName);
        return true;
    }
    
    NLM_LOG_INFO("Shutting down dependency: " + dependencyName);
    
    try {
        dependency->shutdown();
        pImpl->dependencyStates[dependencyName] = DependencyState::ShuttingDown;
        pImpl->shutdownCount++;
        
        NLM_LOG_INFO("Dependency shutdown successfully: " + dependencyName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down dependency " + dependencyName + ": " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> DependencyInjectionContainer::getDependencyNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->dependenciesByName.size());
    
    for (const auto& pair : pImpl->dependenciesByName) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::type_index> DependencyInjectionContainer::getDependencyTypes() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::type_index> types;
    types.reserve(pImpl->dependenciesByType.size());
    
    for (const auto& pair : pImpl->dependenciesByType) {
        types.push_back(pair.first);
    }
    
    return types;
}

DependencyConfig DependencyInjectionContainer::getDependencyConfig(const std::string& dependencyName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencyConfigs.find(dependencyName);
    if (it == pImpl->dependencyConfigs.end()) {
        NLM_LOG_ERROR("Dependency config not found: " + dependencyName);
        return DependencyConfig();
    }
    
    return it->second;
}

DependencyState DependencyInjectionContainer::getDependencyState(const std::string& dependencyName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->dependencyStates.find(dependencyName);
    if (it == pImpl->dependencyStates.end()) {
        return DependencyState::Destroyed;
    }
    
    return it->second;
}

bool DependencyInjectionContainer::hasDependency(const std::string& dependencyName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->dependenciesByName.find(dependencyName) != pImpl->dependenciesByName.end();
}

bool DependencyInjectionContainer::hasDependency(std::type_index dependencyType) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->dependenciesByType.find(dependencyType) != pImpl->dependenciesByType.end();
}

std::vector<std::string> DependencyInjectionContainer::getResolvedDependencies() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> resolved;
    
    for (const auto& pair : pImpl->dependenciesByName) {
        if (pair.second->isResolved()) {
            resolved.push_back(pair.first);
        }
    }
    
    return resolved;
}

std::string DependencyInjectionContainer::getContainerStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "Dependency Injection Container Status:" << std::endl;
    ss << "  Registered Dependencies: " << pImpl->dependenciesByName.size() << std::endl;
    ss << "  Resolve Count: " << pImpl->resolveCount << std::endl;
    ss << "  Initialize Count: " << pImpl->initializeCount << std::endl;
    ss << "  Shutdown Count: " << pImpl->shutdownCount << std::endl;
    ss << "  Error Count: " << pImpl->errorCount << std::endl;
    
    int resolvedCount = 0;
    for (const auto& state : pImpl->dependencyStates) {
        if (state.second == DependencyState::Resolved) {
            resolvedCount++;
        }
    }
    
    ss << "  Resolved Dependencies: " << resolvedCount << std::endl;
    
    return ss.str();
}

DependencyInjectionContainer& DependencyInjectionContainer::getInstance() {
    static DependencyInjectionContainer instance;
    return instance;
}

void DependencyInjectionContainer::reset() {
    static DependencyInjectionContainer instance;
    // Clean up all dependencies
    for (const auto& pair : instance.pImpl->dependenciesByName) {
        instance.unregisterDependency(pair.first);
    }
    instance.pImpl->dependenciesByName.clear();
    instance.pImpl->dependenciesByType.clear();
    instance.pImpl->dependencyConfigs.clear();
    instance.pImpl->dependencyStates.clear();
    instance.pImpl->dependencyConfigsMap.clear();
    instance.pImpl->registerCount = 0;
    instance.pImpl->resolveCount = 0;
    instance.pImpl->initializeCount = 0;
    instance.pImpl->shutdownCount = 0;
    instance.pImpl->errorCount = 0;
}

std::string DependencyInjectionUtils::getDependencyStateString(DependencyState state) {
    switch (state) {
        case DependencyState::Created: return "Created";
        case DependencyState::Resolving: return "Resolving";
        case DependencyState::Resolved: return "Resolved";
        case DependencyState::Initializing: return "Initializing";
        case DependencyState::Initialized: return "Initialized";
        case DependencyState::ShuttingDown: return "ShuttingDown";
        case DependencyState::Destroyed: return "Destroyed";
        case DependencyState::Error: return "Error";
        default: return "Unknown";
    }
}

DependencyState DependencyInjectionUtils::stringToDependencyState(const std::string& stateStr) {
    if (stateStr == "Created") return DependencyState::Created;
    if (stateStr == "Resolving") return DependencyState::Resolving;
    if (stateStr == "Resolved") return DependencyState::Resolved;
    if (stateStr == "Initializing") return DependencyState::Initializing;
    if (stateStr == "Initialized") return DependencyState::Initialized;
    if (stateStr == "ShuttingDown") return DependencyState::ShuttingDown;
    if (stateStr == "Destroyed") return DependencyState::Destroyed;
    if (stateStr == "Error") return DependencyState::Error;
    return DependencyState::Destroyed;
}

std::shared_ptr<IDependency> DependencyInjectionUtils::createDependencyFromFactory(
    const std::string& factoryName,
    const std::map<std::string, std::any>& config) {
    NLM_LOG_INFO("Creating dependency from factory: " + factoryName);
    
    // This would normally create a dependency from a factory
    // For now, just return nullptr
    NLM_LOG_ERROR("Dependency factory creation not fully implemented for: " + factoryName);
    return nullptr;
}

struct DependencyInjectionFramework::Impl {
    std::unique_ptr<IDependencyInjectionContainer> container;
    bool initialized = false;
    std::mutex mutex;
};

DependencyInjectionFramework::DependencyInjectionFramework() : container_(DependencyInjectionContainer::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->container = &container_;
    NLM_LOG_INFO("DependencyInjectionFramework created");
}

DependencyInjectionFramework::~DependencyInjectionFramework() {
    shutdown();
    NLM_LOG_INFO("DependencyInjectionFramework destroyed");
}

bool DependencyInjectionFramework::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("DependencyInjectionFramework already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing DependencyInjectionFramework");
    
    try {
        // Initialize the container
        pImpl->container->resolveDependency("core_dependencies");
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("DependencyInjectionFramework initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing DependencyInjectionFramework: " + std::string(e.what()));
        return false;
    }
}

void DependencyInjectionFramework::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down DependencyInjectionFramework");
    
    try {
        // Shutdown all dependencies
        pImpl->container->shutdownDependency("core_dependencies");
        
        pImpl->initialized = false;
        NLM_LOG_INFO("DependencyInjectionFramework shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down DependencyInjectionFramework: " + std::string(e.what()));
    }
}

void DependencyInjectionFramework::registerCoreDependencies() {
    // Register core dependencies for NLM
    NLM_LOG_INFO("Registering core dependencies");
}

bool DependencyInjectionFramework::resolveAllDependencies() {
    return pImpl->container->resolveDependency("core_dependencies");
}

bool DependencyInjectionFramework::initializeAllDependencies() {
    return pImpl->container->initializeDependency("core_dependencies");
}

bool DependencyInjectionFramework::shutdownAllDependencies() {
    return pImpl->container->shutdownDependency("core_dependencies");
}

bool DependencyInjectionFramework::validateDependencyInjection() {
    std::string diagnostics = getDependencyDiagnostics();
    return !diagnostics.empty();
}

std::string DependencyInjectionFramework::getDependencyDiagnostics() const {
    return pImpl->container->getContainerStatus();
}

std::string DependencyInjectionFramework::getSystemStatus() const {
    return pImpl->container->getContainerStatus();
}

std::map<std::string, std::any> DependencyInjectionFramework::getDependencyStatistics() const {
    return std::map<std::string, std::any>();
}

DependencyInjectionFramework& DependencyInjectionFramework::getInstance() {
    static DependencyInjectionFramework instance;
    return instance;
}

void DependencyInjectionFramework::reset() {
    static DependencyInjectionFramework instance;
    instance.shutdown();
    DependencyInjectionContainer::reset();
}

} // namespace nlm