#include "SystemManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

struct SystemEventListenerManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<ISystemEventListener>> listenersByName;
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<ISystemEventListener>>> listenersByType;
    
    std::mutex mutex;
    
    size_t registerCount = 0;
};

SystemEventListenerManager::SystemEventListenerManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SystemEventListenerManager initialized");
}

SystemEventListenerManager::~SystemEventListenerManager() {
    NLM_LOG_INFO("SystemEventListenerManager shutdown");
}

bool SystemEventListenerManager::registerListener(std::shared_ptr<ISystemEventListener> listener) {
    if (!listener) {
        NLM_LOG_ERROR("Cannot register null event listener");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    const auto& name = listener->getListenerName();
    if (pImpl->listenersByName.find(name) != pImpl->listenersByName.end()) {
        NLM_LOG_ERROR("Event listener already registered: " + name);
        return false;
    }
    
    pImpl->listenersByName[name] = listener;
    
    for (auto eventType : listener->getEventTypes()) {
        pImpl->listenersByType[std::type_index(typeid(eventType))].push_back(listener);
    }
    
    NLM_LOG_INFO("Event listener registered: " + name);
    pImpl->registerCount++;
    return true;
}

bool SystemEventListenerManager::unregisterListener(const std::string& listenerName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->listenersByName.find(listenerName);
    if (it == pImpl->listenersByName.end()) {
        NLM_LOG_ERROR("Event listener not found: " + listenerName);
        return false;
    }
    
    auto listener = it->second;
    
    // Remove from listeners by type
    for (auto eventType : listener->getEventTypes()) {
        auto typeIt = pImpl->listenersByType.find(std::type_index(typeid(eventType)));
        if (typeIt != pImpl->listenersByType.end()) {
            auto& listeners = typeIt->second;
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
                [&listenerName](const std::shared_ptr<ISystemEventListener>& l) {
                    return l->getListenerName() == listenerName;
                }),
                listeners.end());
        }
    }
    
    pImpl->listenersByName.erase(it);
    
    NLM_LOG_INFO("Event listener unregistered: " + listenerName);
    return true;
}

bool SystemEventListenerManager::unregisterListener(std::type_index listenerType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->listenersByType.find(listenerType);
    if (it == pImpl->listenersByType.end()) {
        NLM_LOG_ERROR("Event listener not found for type: " + std::string(listenerType.name()));
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
    
    NLM_LOG_INFO("Event listeners unregistered for type: " + std::string(listenerType.name()));
    return true;
}

void SystemEventListenerManager::distributeEvent(const SystemEvent& event) {
    distributeEventAsync(event);
}

void SystemEventListenerManager::distributeEventAsync(const SystemEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Find listeners for this event type
    auto typeIt = pImpl->listenersByType.find(std::type_index(typeid(event.type)));
    if (typeIt == pImpl->listenersByType.end()) {
        return;
    }
    
    auto& listeners = typeIt->second;
    
    NLM_LOG_INFO("Distributing event " + std::to_string(static_cast<int>(event.type)) +
                " to " + std::to_string(listeners.size()) + " listeners");
    
    for (auto& listener : listeners) {
        try {
            listener->notifyEvent(event);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception notifying event listener " + listener->getListenerName() + ": " + std::string(e.what()));
        }
    }
}

std::vector<std::shared_ptr<ISystemEventListener>> SystemEventListenerManager::getListenersForEvent(SystemEventType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->listenersByType.find(std::type_index(typeid(type)));
    if (it == pImpl->listenersByType.end()) {
        return {};
    }
    
    return it->second;
}

std::vector<std::shared_ptr<ISystemEventListener>> SystemEventListenerManager::getAllListeners() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::shared_ptr<ISystemEventListener>> allListeners;
    allListeners.reserve(pImpl->listenersByName.size());
    
    for (const auto& pair : pImpl->listenersByName) {
        allListeners.push_back(pair.second);
    }
    
    return allListeners;
}

void SystemEventListenerManager::clearListeners() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->listenersByName.clear();
    pImpl->listenersByType.clear();
    NLM_LOG_INFO("All event listeners cleared");
}

SystemEventListenerManager& SystemEventListenerManager::getInstance() {
    static SystemEventListenerManager instance;
    return instance;
}

void SystemEventListenerManager::reset() {
    static SystemEventListenerManager instance;
    instance.~SystemEventListenerManager();
    new (&instance) SystemEventListenerManager();
}

System::System(const std::string& name, const std::string& version)
    : systemName_(name), systemVersion_(version), state_(SystemState::NotInitialized) {
    NLM_LOG_INFO("System created: " + name + " (v" + version + ")");
}

System::~System() {
    shutdown();
    NLM_LOG_INFO("System destroyed: " + systemName_);
}

bool System::initialize() {
    NLM_LOG_INFO("Initializing system: " + systemName_);
    
    try {
        if (!internalInitialize()) {
            NLM_LOG_ERROR("Internal initialization failed for system: " + systemName_);
            state_ = SystemState::Error;
            return false;
        }
        
        state_ = SystemState::Initialized;
        
        // Notify initialization complete
        SystemEvent event(SystemEventType::SystemStart, "System initialized successfully: " + systemName_);
        notifyEvent(event);
        
        NLM_LOG_INFO("System initialized successfully: " + systemName_);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing system " + systemName_ + ": " + std::string(e.what()));
        state_ = SystemState::Error;
        return false;
    }
}

void System::shutdown() {
    if (state_ == SystemState::Shutdown || state_ == SystemState::NotInitialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down system: " + systemName_);
    
    try {
        internalShutdown();
        state_ = SystemState::ShuttingDown;
        
        // Notify shutdown complete
        SystemEvent event(SystemEventType::SystemStop, "System shutting down: " + systemName_);
        notifyEvent(event);
        
        state_ = SystemState::Shutdown;
        
        NLM_LOG_INFO("System shutdown complete: " + systemName_);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down system " + systemName_ + ": " + std::string(e.what()));
        state_ = SystemState::Error;
    }
}

void System::registerEventListener(std::shared_ptr<ISystemEventListener> listener) {
    if (!listener) {
        NLM_LOG_ERROR("Cannot register null event listener");
        return;
    }
    
    std::lock_guard<std::mutex> lock(eventListenersMutex_);
    eventListeners_.push_back(listener);
    NLM_LOG_INFO("Event listener registered for system " + systemName_ + ": " + listener->getListenerName());
}

void System::unregisterEventListener(const std::string& listenerName) {
    std::lock_guard<std::mutex> lock(eventListenersMutex_);
    
    auto it = std::remove_if(eventListeners_.begin(), eventListeners_.end(),
        [&listenerName](const std::shared_ptr<ISystemEventListener>& listener) {
            return listener->getListenerName() == listenerName;
        });
    
    eventListeners_.erase(it, eventListeners_.end());
    NLM_LOG_INFO("Event listener unregistered for system " + systemName_ + ": " + listenerName);
}

void System::notifyEvent(const SystemEvent& event) {
    std::lock_guard<std::mutex> lock(eventListenersMutex_);
    
    for (auto& listener : eventListeners_) {
        try {
            listener->notifyEvent(event);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception notifying event listener " + listener->getListenerName() + ": " + std::string(e.what()));
        }
    }
}

void System::configure(const std::map<std::string, std::any>& config) {
    config_ = config;
}

std::map<std::string, std::any> System::getConfig() const {
    return config_;
}

std::map<std::string, std::any> System::getStatistics() const {
    std::map<std::string, std::any> stats;
    stats["system_name"] = systemName_;
    stats["system_version"] = systemVersion_;
    stats["system_state"] = static_cast<int>(state_);
    stats["initialized"] = isInitialized();
    return stats;
}

struct SystemManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<ISystem>> systemsByName;
    std::unordered_map<std::type_index, std::shared_ptr<ISystem>> systemsByType;
    std::unordered_map<std::string, std::vector<std::string>> systemDependencies;
    std::unordered_map<std::string, SystemState> systemStates;
    std::unordered_map<std::string, std::map<std::string, std::any>> systemConfigs;
    
    std::mutex mutex;
    
    SystemEventListenerManager eventManager;
    
    size_t registerCount = 0;
    size_t initializeCount = 0;
    size_t shutdownCount = 0;
    size_t errorCount = 0;
};

SystemManager::SystemManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SystemManager initialized");
}

SystemManager::~SystemManager() {
    shutdownAllSystems();
    NLM_LOG_INFO("SystemManager shutdown");
}

bool SystemManager::registerSystem(std::shared_ptr<ISystem> system) {
    if (!system) {
        NLM_LOG_ERROR("Cannot register null system");
        return false;
    }
    
    const auto& name = system->getSystemName();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->systemsByName.find(name) != pImpl->systemsByName.end()) {
        NLM_LOG_ERROR("System already registered: " + name);
        return false;
    }
    
    pImpl->systemsByName[name] = system;
    pImpl->systemsByType[std::type_index(typeid(*system))] = system;
    pImpl->systemStates[name] = system->getSystemState();
    
    NLM_LOG_INFO("System registered: " + name + " (" + system->getSystemVersion() + ")");
    pImpl->registerCount++;
    return true;
}

bool SystemManager::unregisterSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemsByName.find(systemName);
    if (it == pImpl->systemsByName.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    auto system = it->second;
    
    // Shutdown if running
    if (system->isRunning()) {
        shutdownSystem(systemName);
    }
    
    // Remove from all maps
    pImpl->systemsByName.erase(it);
    
    auto typeIt = pImpl->systemsByType.begin();
    while (typeIt != pImpl->systemsByType.end()) {
        if (typeIt->second.get() == system.get()) {
            typeIt = pImpl->systemsByType.erase(typeIt);
        } else {
            ++typeIt;
        }
    }
    
    pImpl->systemStates.erase(systemName);
    pImpl->systemConfigs.erase(systemName);
    pImpl->systemDependencies.erase(systemName);
    
    NLM_LOG_INFO("System unregistered: " + systemName);
    return true;
}

bool SystemManager::initializeSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemsByName.find(systemName);
    if (it == pImpl->systemsByName.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    auto& system = it->second;
    
    if (system->isInitialized()) {
        NLM_LOG_INFO("System already initialized: " + systemName);
        return true;
    }
    
    // Check dependencies
    auto dependencies = getSystemDependencies(systemName);
    for (const auto& dep : dependencies) {
        if (!isSystemInitialized(dep)) {
            NLM_LOG_ERROR("System dependency not initialized: " + dep + " (required by " + systemName + ")");
            return false;
        }
    }
    
    NLM_LOG_INFO("Initializing system: " + systemName);
    
    try {
        if (!system->initialize()) {
            NLM_LOG_ERROR("Failed to initialize system: " + systemName);
            pImpl->errorCount++;
            return false;
        }
        
        pImpl->systemStates[systemName] = system->getSystemState();
        pImpl->initializeCount++;
        
        NLM_LOG_INFO("System initialized successfully: " + systemName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing system " + systemName + ": " + std::string(e.what()));
        pImpl->errorCount++;
        return false;
    }
}

bool SystemManager::shutdownSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemsByName.find(systemName);
    if (it == pImpl->systemsByName.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    auto& system = it->second;
    
    if (!system->isInitialized() && !system->isRunning()) {
        NLM_LOG_INFO("System not initialized/running: " + systemName);
        return true;
    }
    
    NLM_LOG_INFO("Shutting down system: " + systemName);
    
    try {
        system->shutdown();
        pImpl->systemStates[systemName] = system->getSystemState();
        pImpl->shutdownCount++;
        
        NLM_LOG_INFO("System shutdown successfully: " + systemName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down system " + systemName + ": " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> SystemManager::getSystemNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->systemsByName.size());
    
    for (const auto& pair : pImpl->systemsByName) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> SystemManager::getSystemNames(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    
    for (const auto& pair : pImpl->systemsByName) {
        const auto& system = pair.second;
        std::string systemCategory = "unknown"; // Would normally extract from system metadata
        if (systemCategory == category) {
            names.push_back(pair.first);
        }
    }
    
    return names;
}

std::shared_ptr<ISystem> SystemManager::getSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemsByName.find(systemName);
    if (it == pImpl->systemsByName.end()) {
        return nullptr;
    }
    
    return it->second;
}

std::shared_ptr<ISystem> SystemManager::getSystem(const std::string& category, const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->systemsByName) {
        if (pair.first == name) {
            return pair.second;
        }
    }
    
    return nullptr;
}

SystemState SystemManager::getSystemState(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemStates.find(systemName);
    if (it == pImpl->systemStates.end()) {
        return SystemState::NotInitialized;
    }
    
    return it->second;
}

bool SystemManager::isSystemInitialized(const std::string& systemName) const {
    return getSystemState(systemName) == SystemState::Initialized ||
           getSystemState(systemName) == SystemState::Running;
}

bool SystemManager::isSystemRunning(const std::string& systemName) const {
    auto system = getSystem(systemName);
    return system ? system->isRunning() : false;
}

bool SystemManager::configureSystem(const std::string& systemName,
                                  const std::map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemsByName.find(systemName);
    if (it == pImpl->systemsByName.end()) {
        NLM_LOG_ERROR("System not found: " + systemName);
        return false;
    }
    
    pImpl->systemConfigs[systemName] = config;
    it->second->configure(config);
    NLM_LOG_INFO("System configuration updated: " + systemName);
    return true;
}

std::map<std::string, std::any> SystemManager::getSystemConfig(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemConfigs.find(systemName);
    if (it == pImpl->systemConfigs.end()) {
        return std::map<std::string, std::any>();
    }
    
    return it->second;
}

bool SystemManager::registerSystemDependency(const std::string& systemName,
                                           const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!hasService(dependencyName)) {
        NLM_LOG_ERROR("Dependency not found: " + dependencyName + " (required by " + systemName + ")");
        return false;
    }
    
    pImpl->systemDependencies[systemName].push_back(dependencyName);
    NLM_LOG_INFO("System dependency registered: " + dependencyName + " for " + systemName);
    return true;
}

std::vector<std::string> SystemManager::getSystemDependencies(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->systemDependencies.find(systemName);
    if (it == pImpl->systemDependencies.end()) {
        return {};
    }
    
    return it->second;
}

bool SystemManager::checkSystemDependencies(const std::string& systemName) const {
    auto dependencies = getSystemDependencies(systemName);
    
    for (const auto& dep : dependencies) {
        if (!hasService(dep)) {
            NLM_LOG_ERROR("System dependency not found: " + dep + " (required by " + systemName + ")");
            return false;
        }
    }
    
    return true;
}

bool SystemManager::initializeAllSystems() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Initializing all systems");
    
    std::vector<std::string> initializedSystems;
    
    // First pass: initialize all systems with no dependencies
    bool success = true;
    for (const auto& pair : pImpl->systemsByName) {
        const auto& name = pair.first;
        
        auto dependencies = getSystemDependencies(name);
        if (dependencies.empty()) {
            if (initializeSystem(name)) {
                initializedSystems.push_back(name);
            } else {
                success = false;
            }
        }
    }
    
    // Second pass: initialize systems with satisfied dependencies
    for (const auto& pair : pImpl->systemsByName) {
        const auto& name = pair.first;
        
        if (std::find(initializedSystems.begin(), initializedSystems.end(), name) != initializedSystems.end()) {
            continue;
        }
        
        auto dependencies = getSystemDependencies(name);
        bool depsSatisfied = true;
        for (const auto& dep : dependencies) {
            if (std::find(initializedSystems.begin(), initializedSystems.end(), dep) == initializedSystems.end()) {
                depsSatisfied = false;
                break;
            }
        }
        
        if (depsSatisfied) {
            if (initializeSystem(name)) {
                initializedSystems.push_back(name);
            } else {
                success = false;
            }
        }
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to initialize all systems");
        return false;
    }
    
    NLM_LOG_INFO("Successfully initialized " + std::to_string(initializedSystems.size()) + " systems");
    return true;
}

void SystemManager::shutdownAllSystems() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Shutting down all systems");
    
    // Shutdown systems in reverse order
    std::vector<std::string> systemNames;
    for (const auto& pair : pImpl->systemsByName) {
        systemNames.push_back(pair.first);
    }
    
    for (auto it = systemNames.rbegin(); it != systemNames.rend(); ++it) {
        shutdownSystem(*it);
    }
}

bool SystemManager::startAllSystems() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Starting all systems");
    
    bool success = true;
    for (const auto& pair : pImpl->systemsByName) {
        const auto& name = pair.first;
        auto& system = pair.second;
        
        if (!system->isInitialized()) {
            NLM_LOG_ERROR("System not initialized: " + name);
            success = false;
            continue;
        }
        
        if (!system->isRunning()) {
            if (!system->start()) {
                NLM_LOG_ERROR("Failed to start system: " + name);
                success = false;
            }
        }
    }
    
    return success;
}

bool SystemManager::stopAllSystems() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Stopping all systems");
    
    bool success = true;
    for (const auto& pair : pImpl->systemsByName) {
        const auto& name = pair.first;
        auto& system = pair.second;
        
        if (system->isRunning()) {
            if (!system->stop()) {
                NLM_LOG_ERROR("Failed to stop system: " + name);
                success = false;
            }
        }
    }
    
    return success;
}

std::string SystemManager::getSystemStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "System Manager Status:" << std::endl;
    ss << "  Registered Systems: " << pImpl->systemsByName.size() << std::endl;
    ss << "  Initialize Count: " << pImpl->initializeCount << std::endl;
    ss << "  Shutdown Count: " << pImpl->shutdownCount << std::endl;
    ss << "  Error Count: " << pImpl->errorCount << std::endl;
    
    int initializedCount = 0;
    int runningCount = 0;
    
    for (const auto& state : pImpl->systemStates) {
        if (state.second == SystemState::Initialized || state.second == SystemState::Running) {
            initializedCount++;
        }
        if (state.second == SystemState::Running) {
            runningCount++;
        }
    }
    
    ss << "  Initialized Systems: " << initializedCount << std::endl;
    ss << "  Running Systems: " << runningCount << std::endl;
    
    auto systemNames = getSystemNames();
    for (const auto& name : systemNames) {
        ss << "  " << name << ": " << static_cast<int>(getSystemState(name)) << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, std::any> SystemManager::getSystemStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, std::any> stats;
    stats["total_systems"] = static_cast<size_t>(pImpl->systemsByName.size());
    stats["initialized_systems"] = static_cast<size_t>(pImpl->initializeCount);
    stats["shutdown_systems"] = static_cast<size_t>(pImpl->shutdownCount);
    stats["error_systems"] = static_cast<size_t>(pImpl->errorCount);
    
    return stats;
}

bool SystemManager::registerEventListener(std::shared_ptr<ISystemEventListener> listener) {
    return pImpl->eventManager.registerListener(listener);
}

bool SystemManager::unregisterEventListener(const std::string& listenerName) {
    return pImpl->eventManager.unregisterListener(listenerName);
}

void SystemManager::distributeEvent(const SystemEvent& event) {
    pImpl->eventManager.distributeEvent(event);
}

bool SystemManager::validateSystemIntegration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Validating system integration");
    
    bool success = true;
    
    // Check for duplicate system names
    std::unordered_map<std::string, int> nameCounts;
    for (const auto& pair : pImpl->systemsByName) {
        nameCounts[pair.first]++;
    }
    
    for (const auto& pair : nameCounts) {
        if (pair.second > 1) {
            NLM_LOG_ERROR("Duplicate system name: " + pair.first);
            success = false;
        }
    }
    
    // Check system dependencies
    for (const auto& pair : pImpl->systemsByName) {
        const auto& name = pair.first;
        if (!checkSystemDependencies(name)) {
            NLM_LOG_ERROR("System dependency check failed: " + name);
            success = false;
        }
    }
    
    return success;
}

bool SystemManager::performSystemDiagnostics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Performing system diagnostics");
    
    bool success = true;
    
    for (const auto& pair : pImpl->systemsByName) {
        const auto& name = pair.first;
        const auto& system = pair.second;
        
        if (!isSystemInitialized(name)) {
            NLM_LOG_WARNING("System not initialized: " + name);
        }
        
        // Note: In a real implementation, we would perform system-specific diagnostics
    }
    
    return success;
}

std::string SystemManager::getSystemDiagnostics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "System Diagnostics:" << std::endl;
    
    int totalSystems = pImpl->systemsByName.size();
    int initializedSystems = 0;
    int runningSystems = 0;
    
    for (const auto& state : pImpl->systemStates) {
        if (state.second == SystemState::Initialized || state.second == SystemState::Running) {
            initializedSystems++;
        }
        if (state.second == SystemState::Running) {
            runningSystems++;
        }
    }
    
    ss << "  Total Systems: " << totalSystems << std::endl;
    ss << "  Initialized Systems: " << initializedSystems << std::endl;
    ss << "  Running Systems: " << runningSystems << std::endl;
    ss << "  System Health: " << (initializedSystems == totalSystems ? "OK" : "WARNING") << std::endl;
    
    return ss.str();
}

SystemManager& SystemManager::getInstance() {
    static SystemManager instance;
    return instance;
}

void SystemManager::reset() {
    static SystemManager instance;
    instance.~SystemManager();
    new (&instance) SystemManager();
}

std::string SystemIntegrationUtils::getEventTypeString(SystemEventType type) {
    switch (type) {
        case SystemEventType::SystemStart: return "SystemStart";
        case SystemEventType::SystemStop: return "SystemStop";
        case SystemEventType::SystemError: return "SystemError";
        case SystemEventType::SystemWarning: return "SystemWarning";
        case SystemEventType::ConfigurationChanged: return "ConfigurationChanged";
        case SystemEventType::ModuleLoaded: return "ModuleLoaded";
        case SystemEventType::ModuleUnloaded: return "ModuleUnloaded";
        case SystemEventType::ModuleError: return "ModuleError";
        case SystemEventType::IntegrationComplete: return "IntegrationComplete";
        case SystemEventType::ValidationComplete: return "ValidationComplete";
        default: return "Unknown";
    }
}

SystemEvent SystemIntegrationUtils::createSystemEvent(SystemEventType type, const std::string& message,
                                                    const std::map<std::string, std::any>& data) {
    return SystemEvent(type, message, data);
}

struct SystemIntegrationFramework::Impl {
    std::unique_ptr<ISystemManager> manager;
    bool initialized = false;
    std::mutex mutex;
};

SystemIntegrationFramework::SystemIntegrationFramework() : manager_(SystemManager::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->manager = &manager_;
    NLM_LOG_INFO("SystemIntegrationFramework created");
}

SystemIntegrationFramework::~SystemIntegrationFramework() {
    shutdown();
    NLM_LOG_INFO("SystemIntegrationFramework destroyed");
}

bool SystemIntegrationFramework::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("SystemIntegrationFramework already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing SystemIntegrationFramework");
    
    try {
        // Initialize the manager
        pImpl->manager->initializeAllSystems();
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("SystemIntegrationFramework initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing SystemIntegrationFramework: " + std::string(e.what()));
        return false;
    }
}

void SystemIntegrationFramework::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down SystemIntegrationFramework");
    
    try {
        // Shutdown all systems
        pImpl->manager->shutdownAllSystems();
        
        pImpl->initialized = false;
        NLM_LOG_INFO("SystemIntegrationFramework shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down SystemIntegrationFramework: " + std::string(e.what()));
    }
}

void SystemIntegrationFramework::registerCoreSystems() {
    // Register core systems for NLM
    // This would normally create and register various core systems
    NLM_LOG_INFO("Registering core systems");
}

bool SystemIntegrationFramework::initializeAllSystems() {
    return pImpl->manager->initializeAllSystems();
}

bool SystemIntegrationFramework::shutdownAllSystems() {
    pImpl->manager->shutdownAllSystems();
    return true;
}

bool SystemIntegrationFramework::validateSystemIntegration() {
    return pImpl->manager->validateSystemIntegration();
}

bool SystemIntegrationFramework::performSystemDiagnostics() {
    return pImpl->manager->performSystemDiagnostics();
}

std::string SystemIntegrationFramework::getSystemStatus() const {
    return pImpl->manager->getSystemStatus();
}

std::map<std::string, std::any> SystemIntegrationFramework::getSystemStatistics() const {
    return pImpl->manager->getSystemStatistics();
}

void SystemIntegrationFramework::notifySystemEvent(const SystemEvent& event) {
    pImpl->manager->distributeEvent(event);
}

SystemIntegrationFramework& SystemIntegrationFramework::getInstance() {
    static SystemIntegrationFramework instance;
    return instance;
}

void SystemIntegrationFramework::reset() {
    static SystemIntegrationFramework instance;
    instance.shutdown();
    SystemManager::reset();
}

} // namespace nlm