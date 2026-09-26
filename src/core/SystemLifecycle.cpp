#include "SystemLifecycle.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>
#include <ctime>

namespace nlm {

struct SystemLifecycleManager::Impl {
    // Lifecycle management
    std::unordered_map<std::string, std::shared_ptr<ISystemLifecycle>> lifecyclesByName;
    std::unordered_map<std::type_index, std::shared_ptr<ISystemLifecycle>> lifecyclesByType;
    std::unordered_map<std::string, SystemLifecycleConfig> lifecycleConfigs;
    std::unordered_map<std::string, LifecycleStage> lifecycleStages;
    std::unordered_map<std::string, std::vector<std::string>> lifecycleDependencies;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Event management
    SystemLifecycleManager lifecycleEventManager;
    
    // Statistics
    size_t registerCount = 0;
    size_t startCount = 0;
    size_t stopCount = 0;
    size_t errorCount = 0;
};

SystemLifecycleManager::SystemLifecycleManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SystemLifecycleManager initialized");
}

SystemLifecycleManager::~SystemLifecycleManager() {
    NLM_LOG_INFO("SystemLifecycleManager shutdown");
}

bool SystemLifecycleManager::registerLifecycleListener(std::shared_ptr<ILifecycleListener> listener) {
    return pImpl->lifecycleEventManager.registerListener(listener);
}

bool SystemLifecycleManager::unregisterLifecycleListener(const std::string& listenerName) {
    return pImpl->lifecycleEventManager.unregisterListener(listenerName);
}

bool SystemLifecycleManager::unregisterLifecycleListener(std::type_index listenerType) {
    return pImpl->lifecycleEventManager.unregisterListener(listenerType);
}

void SystemLifecycleManager::distributeLifecycleEvent(const LifecycleEvent& event) {
    pImpl->lifecycleEventManager.distributeEvent(event);
}

void SystemLifecycleManager::distributeLifecycleEventAsync(const LifecycleEvent& event) {
    pImpl->lifecycleEventManager.distributeEventAsync(event);
}

std::vector<std::shared_ptr<ILifecycleListener>> SystemLifecycleManager::getListenersForEvent(LifecycleEventType type) const {
    return pImpl->lifecycleEventManager.getListenersForEvent(type);
}

std::vector<std::shared_ptr<ILifecycleListener>> SystemLifecycleManager::getAllListeners() const {
    return pImpl->lifecycleEventManager.getAllListeners();
}

void SystemLifecycleManager::clearListeners() {
    pImpl->lifecycleEventManager.clearListeners();
}

std::string SystemLifecycleManager::getLifecycleStageString(LifecycleStage stage) {
    switch (stage) {
        case LifecycleStage::NotStarted: return "NotStarted";
        case LifecycleStage::Initializing: return "Initializing";
        case LifecycleStage::Running: return "Running";
        case LifecycleStage::Paused: return "Paused";
        case LifecycleStage::Stopping: return "Stopping";
        case LifecycleStage::Shutdown: return "Shutdown";
        case LifecycleStage::Restarting: return "Restarting";
        case LifecycleStage::Error: return "Error";
        case LifecycleStage::Maintenance: return "Maintenance";
        default: return "Unknown";
    }
}

std::string SystemLifecycleManager::getLifecycleEventTypeString(LifecycleEventType type) {
    switch (type) {
        case LifecycleEventType::SystemStart: return "SystemStart";
        case LifecycleEventType::SystemInitialized: return "SystemInitialized";
        case LifecycleEventType::SystemRunning: return "SystemRunning";
        case LifecycleEventType::SystemPaused: return "SystemPaused";
        case LifecycleEventType::SystemStopping: return "SystemStopping";
        case LifecycleEventType::SystemShutdown: return "SystemShutdown";
        case LifecycleEventType::SystemRestarted: return "SystemRestarted";
        case LifecycleEventType::SystemError: return "SystemError";
        case LifecycleEventType::SystemMaintenance: return "SystemMaintenance";
        case LifecycleEventType::ConfigurationChanged: return "ConfigurationChanged";
        case LifecycleEventType::DependencyResolved: return "DependencyResolved";
        case LifecycleEventType::HealthCheck: return "HealthCheck";
        case LifecycleEventType::Backup: return "Backup";
        case LifecycleEventType::Restore: return "Restore";
        default: return "Unknown";
    }
}

SystemLifecycleManager& SystemLifecycleManager::getInstance() {
    static SystemLifecycleManager instance;
    return instance;
}

void SystemLifecycleManager::reset() {
    static SystemLifecycleManager instance;
    instance.~SystemLifecycleManager();
    new (&instance) SystemLifecycleManager();
}

SystemLifecycle::SystemLifecycle(const std::string& name)
    : systemName_(name), currentStage_(LifecycleStage::NotStarted), inErrorState_(false) {
    NLM_LOG_INFO("SystemLifecycle created: " + name);
}

SystemLifecycle::~SystemLifecycle() {
    NLM_LOG_INFO("SystemLifecycle destroyed: " + systemName_);
}

bool SystemLifecycle::start() {
    NLM_LOG_INFO("Starting system lifecycle: " + systemName_);
    
    if (!canTransitionTo(LifecycleStage::Running)) {
        NLM_LOG_ERROR("Cannot transition to Running state for system: " + systemName_);
        return false;
    }
    
    return transitionTo(LifecycleStage::Running, "System start requested");
}

bool SystemLifecycle::stop() {
    NLM_LOG_INFO("Stopping system lifecycle: " + systemName_);
    
    if (!canTransitionTo(LifecycleStage::Shutdown)) {
        NLM_LOG_ERROR("Cannot transition to Shutdown state for system: " + systemName_);
        return false;
    }
    
    return transitionTo(LifecycleStage::Shutdown, "System stop requested");
}

bool SystemLifecycle::pause() {
    NLM_LOG_INFO("Pausing system lifecycle: " + systemName_);
    
    if (!canTransitionTo(LifecycleStage::Paused)) {
        NLM_LOG_ERROR("Cannot transition to Paused state for system: " + systemName_);
        return false;
    }
    
    return transitionTo(LifecycleStage::Paused, "System pause requested");
}

bool SystemLifecycle::resume() {
    NLM_LOG_INFO("Resuming system lifecycle: " + systemName_);
    
    if (!canTransitionTo(LifecycleStage::Running)) {
        NLM_LOG_ERROR("Cannot transition to Running state for system: " + systemName_);
        return false;
    }
    
    return transitionTo(LifecycleStage::Running, "System resume requested");
}

bool SystemLifecycle::restart() {
    NLM_LOG_INFO("Restarting system lifecycle: " + systemName_);
    
    if (!canTransitionTo(LifecycleStage::Restarting)) {
        NLM_LOG_ERROR("Cannot transition to Restarting state for system: " + systemName_);
        return false;
    }
    
    // Stop first
    if (!stop()) {
        NLM_LOG_ERROR("Failed to stop system before restart: " + systemName_);
        return false;
    }
    
    // Start again
    return start();
}

bool SystemLifecycle::shutdown() {
    NLM_LOG_INFO("Shutting down system lifecycle: " + systemName_);
    
    if (!canTransitionTo(LifecycleStage::Shutdown)) {
        NLM_LOG_ERROR("Cannot transition to Shutdown state for system: " + systemName_);
        return false;
    }
    
    return transitionTo(LifecycleStage::Shutdown, "System shutdown requested");
}

bool SystemLifecycle::handleLifecycleEvent(const LifecycleEvent& event) {
    NLM_LOG_INFO("SystemLifecycle handling event " + getLifecycleEventTypeString(event.type) +
                " for system " + systemName_);
    
    // Handle specific event types
    switch (event.type) {
        case LifecycleEventType::SystemStart:
            if (event.error) {
                return handleError(event.message);
            }
            break;
        case LifecycleEventType::SystemError:
            return handleError(event.message);
        case LifecycleEventType::ConfigurationChanged:
            // Note: Configuration update logic would be implemented in derived classes
            break;
        default:
            break;
    }
    
    // Note: Event handling would be implemented in derived classes
    return true;
}

void SystemLifecycle::configure(const SystemLifecycleConfig& config) {
    config_ = config;
    NLM_LOG_INFO("SystemLifecycle configured: " + systemName_ + " (stage: " + 
                getLifecycleStageString(config.initialStage) + ")");
}

SystemLifecycleConfig SystemLifecycle::getConfig() const {
    return config_;
}

bool SystemLifecycle::transitionTo(LifecycleStage newStage, const std::string& reason) {
    if (!canTransitionTo(newStage)) {
        NLM_LOG_ERROR("Cannot transition from " + getLifecycleStageString(currentStage_) +
                     " to " + getLifecycleStageString(newStage) + " for system: " + systemName_);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(historyMutex_);
    
    // Create lifecycle event
    LifecycleEvent event(LifecycleEventType::SystemStart, 
                        "Transitioning from " + getLifecycleStageString(currentStage_) + 
                        " to " + getLifecycleStageString(newStage) + ": " + reason);
    
    // Log the transition
    lifecycleHistory_.push_back(event);
    
    NLM_LOG_INFO("SystemLifecycle transitioning from " + getLifecycleStageString(currentStage_) +
                " to " + getLifecycleStageString(newStage) + ": " + reason);
    
    currentStage_ = newStage;
    
    // Handle the transition
    bool success = false;
    
    switch (newStage) {
        case LifecycleStage::Running:
            success = internalStart();
            break;
        case LifecycleStage::Shutdown:
            success = internalStop();
            break;
        case LifecycleStage::Paused:
            success = internalPause();
            break;
        case LifecycleStage::Restarting:
            success = internalRestart();
            break;
        default:
            success = true; // No action needed for these states
            break;
    }
    
    if (success) {
        // Notify success
        LifecycleEvent successEvent(LifecycleEventType::SystemInitialized,
                                   "System successfully transitioned to " + getLifecycleStageString(newStage));
        distributeLifecycleEvent(successEvent);
    } else {
        // Handle failure
        handleError("Failed to transition to " + getLifecycleStageString(newStage));
        currentStage_ = LifecycleStage::Error;
    }
    
    return success;
}

bool SystemLifecycle::canTransitionTo(LifecycleStage newStage) const {
    // Basic state transition rules
    switch (currentStage_) {
        case LifecycleStage::NotStarted:
            return newStage == LifecycleStage::Running || newStage == LifecycleStage::Error;
        case LifecycleStage::Initializing:
            return newStage == LifecycleStage::Running || newStage == LifecycleStage::Error;
        case LifecycleStage::Running:
            return newStage == LifecycleStage::Paused || newStage == LifecycleStage::Stopping || 
                   newStage == LifecycleStage::Restarting || newStage == LifecycleStage::Shutdown;
        case LifecycleStage::Paused:
            return newStage == LifecycleStage::Running || newStage == LifecycleStage::Restarting || 
                   newStage == LifecycleStage::Shutdown;
        case LifecycleStage::Stopping:
            return newStage == LifecycleStage::Shutdown;
        case LifecycleStage::Restarting:
            return newStage == LifecycleStage::Running || newStage == LifecycleStage::Error;
        case LifecycleStage::Error:
            return newStage == LifecycleStage::NotStarted || newStage == LifecycleStage::Initializing;
        case LifecycleStage::Maintenance:
            return newStage == LifecycleStage::Running || newStage == LifecycleStage::Error;
        case LifecycleStage::Shutdown:
            return false; // Final state
        default:
            return false;
    }
}

bool SystemLifecycle::handleError(const std::string& errorMessage) {
    std::lock_guard<std::mutex> lock(errorMutex_);
    
    errorHistory_.push_back(errorMessage);
    inErrorState_ = true;
    currentStage_ = LifecycleStage::Error;
    
    NLM_LOG_ERROR("SystemLifecycle error in system " + systemName_ + ": " + errorMessage);
    
    // Notify error event
    LifecycleEvent errorEvent(LifecycleEventType::SystemError, errorMessage, true);
    distributeLifecycleEvent(errorEvent);
    
    return true;
}

bool SystemLifecycle::recoverFromError() {
    std::lock_guard<std::mutex> lock(errorMutex_);
    
    if (!inErrorState_) {
        return true; // Not in error state, nothing to recover from
    }
    
    NLM_LOG_INFO("SystemLifecycle recovering from error in system " + systemName_);
    
    // Note: Recovery logic would be implemented in derived classes
    inErrorState_ = false;
    currentStage_ = LifecycleStage::NotStarted;
    
    return true;
}

void SystemLifecycle::distributeLifecycleEvent(const LifecycleEvent& event) {
    lifecycleEventManager.distributeEvent(event);
}

std::map<std::string, std::any> SystemLifecycle::getStatistics() const {
    std::lock_guard<std::mutex> historyLock(historyMutex_);
    std::lock_guard<std::mutex> errorLock(errorMutex_);
    
    std::map<std::string, std::any> stats;
    
    stats["system_name"] = systemName_;
    stats["current_stage"] = static_cast<int>(currentStage_);
    stats["stage_string"] = getLifecycleStageString(currentStage_);
    stats["is_running"] = isRunning();
    stats["is_paused"] = isPaused();
    stats["is_stopped"] = isStopped();
    stats["error_state"] = inErrorState_;
    stats["error_count"] = errorHistory_.size();
    stats["event_count"] = lifecycleHistory_.size();
    
    if (!lifecycleHistory_.empty()) {
        auto lastEvent = lifecycleHistory_.back();
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastEvent.timestamp);
        stats["last_event_time"] = duration.count();
        stats["last_event_type"] = getLifecycleEventTypeString(lastEvent.type);
    }
    
    return stats;
}

bool SystemLifecycle::validate() const {
    NLM_LOG_INFO("Validating system lifecycle: " + systemName_);
    
    bool success = true;
    
    // Validate lifecycle stage
    if (currentStage_ == LifecycleStage::Error) {
        NLM_LOG_ERROR("System in error state: " + systemName_);
        success = false;
    }
    
    // Validate event history
    if (lifecycleHistory_.size() > 1000) {
        NLM_LOG_WARNING("Large lifecycle history for system: " + systemName_);
    }
    
    // Validate error history
    if (errorHistory_.size() > 100) {
        NLM_LOG_WARNING("Large error history for system: " + systemName_);
    }
    
    return success;
}

std::string SystemLifecycle::validateAll() const {
    NLM_LOG_INFO("Validating all lifecycle aspects for system: " + systemName_);
    
    std::stringstream ss;
    bool overallSuccess = validate();
    
    ss << "System Lifecycle Validation for " << systemName_ << ":" << std::endl;
    ss << "  Current Stage: " << getLifecycleStageString(currentStage_) << std::endl;
    ss << "  Is Running: " << (isRunning() ? "Yes" : "No") << std::endl;
    ss << "  Is Paused: " << (isPaused() ? "Yes" : "No") << std::endl;
    ss << "  In Error State: " << (inErrorState_ ? "Yes" : "No") << std::endl;
    ss << "  Error Count: " << errorHistory_.size() << std::endl;
    ss << "  Event Count: " << lifecycleHistory_.size() << std::endl;
    ss << "  Overall Status: " << (overallSuccess ? "PASS" : "FAIL") << std::endl;
    
    return ss.str();
}

SystemLifecycleConfig SystemLifecycleUtils::createLifecycleConfig(const std::string& systemName,
                                                                 LifecycleStage initialStage) {
    SystemLifecycleConfig config;
    config.systemName = systemName;
    config.initialStage = initialStage;
    config.timeout = std::chrono::milliseconds(30000);
    return config;
}

bool SystemLifecycleUtils::validateLifecycleStage(LifecycleStage stage, const std::string& systemName) {
    // Validate that the stage is a valid lifecycle stage
    switch (stage) {
        case LifecycleStage::NotStarted:
        case LifecycleStage::Initializing:
        case LifecycleStage::Running:
        case LifecycleStage::Paused:
        case LifecycleStage::Stopping:
        case LifecycleStage::Shutdown:
        case LifecycleStage::Restarting:
        case LifecycleStage::Error:
        case LifecycleStage::Maintenance:
            return true;
        default:
            NLM_LOG_ERROR("Invalid lifecycle stage for system " + systemName + ": " +
                         std::to_string(static_cast<int>(stage)));
            return false;
    }
}

std::string SystemLifecycleUtils::getLifecycleStageDescription(LifecycleStage stage) {
    switch (stage) {
        case LifecycleStage::NotStarted: return "System has not started yet";
        case LifecycleStage::Initializing: return "System is initializing";
        case LifecycleStage::Running: return "System is running normally";
        case LifecycleStage::Paused: return "System is paused";
        case LifecycleStage::Stopping: return "System is stopping";
        case LifecycleStage::Shutdown: return "System is shut down";
        case LifecycleStage::Restarting: return "System is restarting";
        case LifecycleStage::Error: return "System is in an error state";
        case LifecycleStage::Maintenance: return "System is under maintenance";
        default: return "Unknown system state";
    }
}

LifecycleEvent SystemLifecycleUtils::createLifecycleEvent(LifecycleEventType type, const std::string& message,
                                                         const std::map<std::string, std::any>& data) {
    return LifecycleEvent(type, message, data);
}

bool SystemLifecycleUtils::isLifecycleEventError(LifecycleEventType type) {
    return type == LifecycleEventType::SystemError;
}

std::string SystemLifecycleUtils::formatLifecycleStatistics(const std::map<std::string, std::any>& stats) {
    std::stringstream ss;
    ss << "Lifecycle Statistics:" << std::endl;
    
    for (const auto& pair : stats) {
        ss << "  " << pair.first << ": ";
        if (pair.second.type() == typeid(int)) {
            ss << std::any_cast<int>(pair.second) << std::endl;
        } else if (pair.second.type() == typeid(std::string)) {
            ss << std::any_cast<std::string>(pair.second) << std::endl;
        } else if (pair.second.type() == typeid(double)) {
            ss << std::any_cast<double>(pair.second) << std::endl;
        } else {
            ss << "[...]" << std::endl;
        }
    }
    
    return ss.str();
}

double SystemLifecycleUtils::calculateLifecycleUptime(const std::chrono::steady_clock::time_point& startTime,
                                                     const std::chrono::steady_clock::time_point& endTime) {
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    return static_cast<double>(duration.count());
}

struct SystemLifecycleManagerImpl::Impl {
    std::unordered_map<std::string, std::shared_ptr<ISystemLifecycle>> lifecyclesByName;
    std::unordered_map<std::type_index, std::shared_ptr<ISystemLifecycle>> lifecyclesByType;
    std::unordered_map<std::string, SystemLifecycleConfig> lifecycleConfigs;
    std::unordered_map<std::string, LifecycleStage> lifecycleStages;
    std::unordered_map<std::string, std::vector<std::string>> lifecycleDependencies;
    
    std::mutex mutex;
    
    SystemLifecycleManager lifecycleEventManager;
    
    size_t registerCount = 0;
    size_t startCount = 0;
    size_t stopCount = 0;
    size_t errorCount = 0;
};

SystemLifecycleManagerImpl::SystemLifecycleManagerImpl() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SystemLifecycleManagerImpl initialized");
}

SystemLifecycleManagerImpl::~SystemLifecycleManagerImpl() {
    NLM_LOG_INFO("SystemLifecycleManagerImpl shutdown");
}

bool SystemLifecycleManagerImpl::registerLifecycle(std::shared_ptr<ISystemLifecycle> lifecycle) {
    if (!lifecycle) {
        NLM_LOG_ERROR("Cannot register null lifecycle");
        return false;
    }
    
    const auto& name = lifecycle->getSystemName();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->lifecyclesByName.find(name) != pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle already registered: " + name);
        return false;
    }
    
    pImpl->lifecyclesByName[name] = lifecycle;
    pImpl->lifecyclesByType[std::type_index(typeid(*lifecycle))] = lifecycle;
    pImpl->lifecycleStages[name] = lifecycle->getLifecycleStage();
    
    NLM_LOG_INFO("Lifecycle registered: " + name);
    pImpl->registerCount++;
    return true;
}

bool SystemLifecycleManagerImpl::unregisterLifecycle(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecyclesByName.find(systemName);
    if (it == pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle not found: " + systemName);
        return false;
    }
    
    auto lifecycle = it->second;
    
    // Shutdown if running
    if (lifecycle->isRunning()) {
        lifecycle->stop();
    }
    
    // Remove from all maps
    pImpl->lifecyclesByName.erase(it);
    
    auto typeIt = pImpl->lifecyclesByType.begin();
    while (typeIt != pImpl->lifecyclesByType.end()) {
        if (typeIt->second.get() == lifecycle.get()) {
            typeIt = pImpl->lifecyclesByType.erase(typeIt);
        } else {
            ++typeIt;
        }
    }
    
    pImpl->lifecycleStages.erase(systemName);
    pImpl->lifecycleConfigs.erase(systemName);
    pImpl->lifecycleDependencies.erase(systemName);
    
    NLM_LOG_INFO("Lifecycle unregistered: " + systemName);
    return true;
}

bool SystemLifecycleManagerImpl::startSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecyclesByName.find(systemName);
    if (it == pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle not found: " + systemName);
        return false;
    }
    
    auto& lifecycle = it->second;
    
    if (lifecycle->isRunning()) {
        NLM_LOG_INFO("Lifecycle already running: " + systemName);
        return true;
    }
    
    // Check dependencies
    auto dependencies = getSystemDependencies(systemName);
    for (const auto& dep : dependencies) {
        auto depIt = pImpl->lifecyclesByName.find(dep);
        if (depIt == pImpl->lifecyclesByName.end() || 
            !depIt->second->isRunning()) {
            NLM_LOG_ERROR("Dependency not running: " + dep + " (required by " + systemName + ")");
            return false;
        }
    }
    
    NLM_LOG_INFO("Starting lifecycle: " + systemName);
    
    try {
        if (!lifecycle->start()) {
            NLM_LOG_ERROR("Failed to start lifecycle: " + systemName);
            pImpl->errorCount++;
            return false;
        }
        
        pImpl->lifecycleStages[systemName] = lifecycle->getLifecycleStage();
        pImpl->startCount++;
        
        NLM_LOG_INFO("Lifecycle started successfully: " + systemName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception starting lifecycle " + systemName + ": " + std::string(e.what()));
        pImpl->errorCount++;
        return false;
    }
}

bool SystemLifecycleManagerImpl::stopSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecyclesByName.find(systemName);
    if (it == pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle not found: " + systemName);
        return false;
    }
    
    auto& lifecycle = it->second;
    
    if (!lifecycle->isRunning()) {
        NLM_LOG_INFO("Lifecycle not running: " + systemName);
        return true;
    }
    
    NLM_LOG_INFO("Stopping lifecycle: " + systemName);
    
    try {
        if (!lifecycle->stop()) {
            NLM_LOG_ERROR("Failed to stop lifecycle: " + systemName);
            return false;
        }
        
        pImpl->lifecycleStages[systemName] = lifecycle->getLifecycleStage();
        pImpl->stopCount++;
        
        NLM_LOG_INFO("Lifecycle stopped successfully: " + systemName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception stopping lifecycle " + systemName + ": " + std::string(e.what()));
        return false;
    }
}

bool SystemLifecycleManagerImpl::pauseSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecyclesByName.find(systemName);
    if (it == pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle not found: " + systemName);
        return false;
    }
    
    auto& lifecycle = it->second;
    
    NLM_LOG_INFO("Pausing lifecycle: " + systemName);
    return lifecycle->pause();
}

bool SystemLifecycleManagerImpl::resumeSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecyclesByName.find(systemName);
    if (it == pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle not found: " + systemName);
        return false;
    }
    
    auto& lifecycle = it->second;
    
    NLM_LOG_INFO("Resuming lifecycle: " + systemName);
    return lifecycle->resume();
}

bool SystemLifecycleManagerImpl::restartSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecyclesByName.find(systemName);
    if (it == pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle not found: " + systemName);
        return false;
    }
    
    auto& lifecycle = it->second;
    
    NLM_LOG_INFO("Restarting lifecycle: " + systemName);
    return lifecycle->restart();
}

bool SystemLifecycleManagerImpl::shutdownSystem(const std::string& systemName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecyclesByName.find(systemName);
    if (it == pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle not found: " + systemName);
        return false;
    }
    
    auto& lifecycle = it->second;
    
    NLM_LOG_INFO("Shutting down lifecycle: " + systemName);
    return lifecycle->shutdown();
}

std::vector<std::string> SystemLifecycleManagerImpl::getSystemNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->lifecyclesByName.size());
    
    for (const auto& pair : pImpl->lifecyclesByName) {
        names.push_back(pair.first);
    }
    
    return names;
}

LifecycleStage SystemLifecycleManagerImpl::getSystemLifecycleStage(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecycleStages.find(systemName);
    if (it == pImpl->lifecycleStages.end()) {
        return LifecycleStage::NotStarted;
    }
    
    return it->second;
}

bool SystemLifecycleManagerImpl::isSystemRunning(const std::string& systemName) const {
    auto stage = getSystemLifecycleStage(systemName);
    return stage == LifecycleStage::Running;
}

bool SystemLifecycleManagerImpl::configureSystemLifecycle(const std::string& systemName,
                                                        const SystemLifecycleConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecyclesByName.find(systemName);
    if (it == pImpl->lifecyclesByName.end()) {
        NLM_LOG_ERROR("Lifecycle not found: " + systemName);
        return false;
    }
    
    pImpl->lifecycleConfigs[systemName] = config;
    it->second->configure(config);
    NLM_LOG_INFO("Lifecycle configuration updated: " + systemName);
    return true;
}

SystemLifecycleConfig SystemLifecycleManagerImpl::getSystemLifecycleConfig(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecycleConfigs.find(systemName);
    if (it == pImpl->lifecycleConfigs.end()) {
        return SystemLifecycleConfig(systemName);
    }
    
    return it->second;
}

bool SystemLifecycleManagerImpl::registerSystemDependency(const std::string& systemName,
                                                        const std::string& dependencyName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!hasSystem(dependencyName)) {
        NLM_LOG_ERROR("Dependency not found: " + dependencyName + " (required by " + systemName + ")");
        return false;
    }
    
    pImpl->lifecycleDependencies[systemName].push_back(dependencyName);
    NLM_LOG_INFO("System dependency registered: " + dependencyName + " for " + systemName);
    return true;
}

std::vector<std::string> SystemLifecycleManagerImpl::getSystemDependencies(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->lifecycleDependencies.find(systemName);
    if (it == pImpl->lifecycleDependencies.end()) {
        return {};
    }
    
    return it->second;
}

bool SystemLifecycleManagerImpl::checkSystemDependencies(const std::string& systemName) const {
    auto dependencies = getSystemDependencies(systemName);
    
    for (const auto& dep : dependencies) {
        if (!hasSystem(dep)) {
            NLM_LOG_ERROR("System dependency not found: " + dep + " (required by " + systemName + ")");
            return false;
        }
    }
    
    return true;
}

bool SystemLifecycleManagerImpl::startAllSystems() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Starting all systems");
    
    bool success = true;
    for (const auto& pair : pImpl->lifecyclesByName) {
        const auto& name = pair.first;
        auto& lifecycle = pair.second;
        
        if (!lifecycle->isRunning()) {
            if (!startSystem(name)) {
                NLM_LOG_ERROR("Failed to start system: " + name);
                success = false;
            }
        }
    }
    
    return success;
}

bool SystemLifecycleManagerImpl::stopAllSystems() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Stopping all systems");
    
    bool success = true;
    for (const auto& pair : pImpl->lifecyclesByName) {
        const auto& name = pair.first;
        auto& lifecycle = pair.second;
        
        if (lifecycle->isRunning()) {
            if (!stopSystem(name)) {
                NLM_LOG_ERROR("Failed to stop system: " + name);
                success = false;
            }
        }
    }
    
    return success;
}

bool SystemLifecycleManagerImpl::pauseAllSystems() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Pausing all systems");
    
    bool success = true;
    for (const auto& pair : pImpl->lifecyclesByName) {
        const auto& name = pair.first;
        auto& lifecycle = pair.second;
        
        if (lifecycle->isRunning()) {
            if (!lifecycle->pause()) {
                NLM_LOG_ERROR("Failed to pause system: " + name);
                success = false;
            }
        }
    }
    
    return success;
}

bool SystemLifecycleManagerImpl::resumeAllSystems() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Resuming all systems");
    
    bool success = true;
    for (const auto& pair : pImpl->lifecyclesByName) {
        const auto& name = pair.first;
        auto& lifecycle = pair.second;
        
        if (lifecycle->isPaused()) {
            if (!lifecycle->resume()) {
                NLM_LOG_ERROR("Failed to resume system: " + name);
                success = false;
            }
        }
    }
    
    return success;
}

std::string SystemLifecycleManagerImpl::getLifecycleStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "System Lifecycle Manager Status:" << std::endl;
    ss << "  Registered Lifecycles: " << pImpl->lifecyclesByName.size() << std::endl;
    ss << "  Start Count: " << pImpl->startCount << std::endl;
    ss << "  Stop Count: " << pImpl->stopCount << std::endl;
    ss << "  Error Count: " << pImpl->errorCount << std::endl;
    
    int runningCount = 0;
    for (const auto& stage : pImpl->lifecycleStages) {
        if (stage.second == LifecycleStage::Running) {
            runningCount++;
        }
    }
    
    ss << "  Running Lifecycles: " << runningCount << std::endl;
    
    auto lifecycleNames = getSystemNames();
    for (const auto& name : lifecycleNames) {
        ss << "  " << name << ": " << getLifecycleStageString(getSystemLifecycleStage(name)) << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, std::any> SystemLifecycleManagerImpl::getLifecycleStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, std::any> stats;
    stats["total_lifecycles"] = static_cast<size_t>(pImpl->lifecyclesByName.size());
    stats["running_lifecycles"] = static_cast<size_t>(pImpl->startCount - pImpl->stopCount);
    stats["error_count"] = static_cast<size_t>(pImpl->errorCount);
    
    return stats;
}

bool SystemLifecycleManagerImpl::registerLifecycleListener(std::shared_ptr<ILifecycleListener> listener) {
    return pImpl->lifecycleEventManager.registerListener(listener);
}

bool SystemLifecycleManagerImpl::unregisterLifecycleListener(const std::string& listenerName) {
    return pImpl->lifecycleEventManager.unregisterListener(listenerName);
}

void SystemLifecycleManagerImpl::distributeLifecycleEvent(const LifecycleEvent& event) {
    pImpl->lifecycleEventManager.distributeEvent(event);
}

bool SystemLifecycleManagerImpl::validateAllSystems() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NLM_LOG_INFO("Validating all system lifecycles");
    
    bool success = true;
    
    for (const auto& pair : pImpl->lifecyclesByName) {
        const auto& name = pair.first;
        const auto& lifecycle = pair.second;
        
        if (!lifecycle->isRunning()) {
            NLM_LOG_WARNING("Lifecycle not running: " + name);
        }
        
        if (!lifecycle->validate()) {
            NLM_LOG_ERROR("Lifecycle validation failed: " + name);
            success = false;
        }
    }
    
    return success;
}

std::string SystemLifecycleManagerImpl::getLifecycleDiagnostics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "System Lifecycle Diagnostics:" << std::endl;
    
    int totalSystems = pImpl->lifecyclesByName.size();
    int runningSystems = 0;
    
    for (const auto& stage : pImpl->lifecycleStages) {
        if (stage.second == LifecycleStage::Running) {
            runningSystems++;
        }
    }
    
    ss << "  Total Systems: " << totalSystems << std::endl;
    ss << "  Running Systems: " << runningSystems << std::endl;
    ss << "  System Health: " << (runningSystems == totalSystems ? "OK" : "WARNING") << std::endl;
    
    return ss.str();
}

bool SystemLifecycleManagerImpl::hasSystem(const std::string& systemName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lifecyclesByName.find(systemName) != pImpl->lifecyclesByName.end();
}

SystemLifecycleManagerImpl& SystemLifecycleManagerImpl::getInstance() {
    static SystemLifecycleManagerImpl instance;
    return instance;
}

void SystemLifecycleManagerImpl::reset() {
    static SystemLifecycleManagerImpl instance;
    instance.~SystemLifecycleManagerImpl();
    new (&instance) SystemLifecycleManagerImpl();
}

struct SystemLifecycleFramework::Impl {
    std::unique_ptr<ISystemLifecycleManager> manager;
    bool initialized = false;
    std::mutex mutex;
};

SystemLifecycleFramework::SystemLifecycleFramework() : manager_(SystemLifecycleManagerImpl::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->manager = &manager_;
    NLM_LOG_INFO("SystemLifecycleFramework created");
}

SystemLifecycleFramework::~SystemLifecycleFramework() {
    shutdown();
    NLM_LOG_INFO("SystemLifecycleFramework destroyed");
}

bool SystemLifecycleFramework::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("SystemLifecycleFramework already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing SystemLifecycleFramework");
    
    try {
        // Initialize the manager
        pImpl->manager->startAllSystems();
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("SystemLifecycleFramework initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing SystemLifecycleFramework: " + std::string(e.what()));
        return false;
    }
}

void SystemLifecycleFramework::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down SystemLifecycleFramework");
    
    try {
        // Shutdown all systems
        pImpl->manager->stopAllSystems();
        
        pImpl->initialized = false;
        NLM_LOG_INFO("SystemLifecycleFramework shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down SystemLifecycleFramework: " + std::string(e.what()));
    }
}

void SystemLifecycleFramework::registerCoreLifecycles() {
    // Register core lifecycles for NLM
    // This would normally create and register various core lifecycles
    NLM_LOG_INFO("Registering core lifecycles");
}

bool SystemLifecycleFramework::startAllSystems() {
    return pImpl->manager->startAllSystems();
}

bool SystemLifecycleFramework::stopAllSystems() {
    return pImpl->manager->stopAllSystems();
}

bool SystemLifecycleFramework::pauseAllSystems() {
    return pImpl->manager->pauseAllSystems();
}

bool SystemLifecycleFramework::resumeAllSystems() {
    return pImpl->manager->resumeAllSystems();
}

bool SystemLifecycleFramework::validateSystemLifecycles() {
    return pImpl->manager->validateAllSystems();
}

bool SystemLifecycleFramework::performSystemHealthChecks() {
    std::string diagnostics = getLifecycleDiagnostics();
    return !diagnostics.empty();
}

std::string SystemLifecycleFramework::getLifecycleStatus() const {
    return pImpl->manager->getLifecycleStatus();
}

std::map<std::string, std::any> SystemLifecycleFramework::getLifecycleStatistics() const {
    return pImpl->manager->getLifecycleStatistics();
}

void SystemLifecycleFramework::notifyLifecycleEvent(const LifecycleEvent& event) {
    pImpl->manager->distributeLifecycleEvent(event);
}

SystemLifecycleFramework& SystemLifecycleFramework::getInstance() {
    static SystemLifecycleFramework instance;
    return instance;
}

void SystemLifecycleFramework::reset() {
    static SystemLifecycleFramework instance;
    instance.shutdown();
    SystemLifecycleManagerImpl::reset();
}

} // namespace nlm