#include "ExtensionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

struct ExtensionManager::Impl {
    // Extension point management
    std::unordered_map<std::string, ExtensionPoint> extensionPoints;
    
    // Hook management
    std::unordered_map<std::string, std::vector<std::shared_ptr<IHook>>> hooks;
    
    // Interface extensions
    std::unordered_map<std::string, std::vector<std::shared_ptr<IInterfaceExtension>>> interfaceExtensions;
    
    // Implementation extensions
    std::unordered_map<std::string, std::vector<std::shared_ptr<IImplementationExtension>>> implementationExtensions;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Statistics
    size_t registerCount = 0;
    size_t unregisterCount = 0;
    size_t executeCount = 0;
    size_t errorCount = 0;
};

ExtensionManager::ExtensionManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("ExtensionManager initialized");
}

ExtensionManager::~ExtensionManager() {
    NLM_LOG_INFO("ExtensionManager shutdown");
}

bool ExtensionManager::registerExtensionPoint(const ExtensionPoint& extensionPoint) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->extensionPoints.find(extensionPoint.name) != pImpl->extensionPoints.end()) {
        NLM_LOG_ERROR("Extension point already registered: " + extensionPoint.name);
        return false;
    }
    
    pImpl->extensionPoints[extensionPoint.name] = extensionPoint;
    NLM_LOG_INFO("Extension point registered: " + extensionPoint.name);
    pImpl->registerCount++;
    return true;
}

bool ExtensionManager::unregisterExtensionPoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->extensionPoints.find(name);
    if (it == pImpl->extensionPoints.end()) {
        NLM_LOG_ERROR("Extension point not found: " + name);
        return false;
    }
    
    // Remove associated hooks and extensions
    pImpl->hooks.erase(name);
    pImpl->interfaceExtensions.erase(name);
    pImpl->implementationExtensions.erase(name);
    
    pImpl->extensionPoints.erase(it);
    NLM_LOG_INFO("Extension point unregistered: " + name);
    pImpl->unregisterCount++;
    return true;
}

ExtensionPoint ExtensionManager::getExtensionPoint(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->extensionPoints.find(name);
    if (it == pImpl->extensionPoints.end()) {
        NLM_LOG_ERROR("Extension point not found: " + name);
        return ExtensionPoint();
    }
    
    return it->second;
}

std::vector<ExtensionPoint> ExtensionManager::getAllExtensionPoints() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ExtensionPoint> points;
    points.reserve(pImpl->extensionPoints.size());
    for (const auto& pair : pImpl->extensionPoints) {
        points.push_back(pair.second);
    }
    return points;
}

bool ExtensionManager::registerHook(const std::string& extensionPointName, std::shared_ptr<IHook> hook) {
    if (!hook) {
        NLM_LOG_ERROR("Cannot register null hook");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto epIt = pImpl->extensionPoints.find(extensionPointName);
    if (epIt == pImpl->extensionPoints.end()) {
        NLM_LOG_ERROR("Extension point not found: " + extensionPointName);
        return false;
    }
    
    pImpl->hooks[extensionPointName].push_back(hook);
    NLM_LOG_INFO("Hook registered: " + hook->getHookName() + " for extension point " + extensionPointName);
    return true;
}

bool ExtensionManager::unregisterHook(const std::string& extensionPointName, const std::string& hookName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto hooksIt = pImpl->hooks.find(extensionPointName);
    if (hooksIt == pImpl->hooks.end()) {
        NLM_LOG_ERROR("No hooks found for extension point: " + extensionPointName);
        return false;
    }
    
    auto& hookList = hooksIt->second;
    auto hookIt = std::remove_if(hookList.begin(), hookList.end(),
        [&hookName](const std::shared_ptr<IHook>& hook) {
            return hook->getHookName() == hookName;
        });
    
    if (hookIt == hookList.end()) {
        NLM_LOG_ERROR("Hook not found: " + hookName + " for extension point " + extensionPointName);
        return false;
    }
    
    hookList.erase(hookIt, hookList.end());
    NLM_LOG_INFO("Hook unregistered: " + hookName + " for extension point " + extensionPointName);
    return true;
}

std::vector<std::shared_ptr<IHook>> ExtensionManager::getHooks(const std::string& extensionPointName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->hooks.find(extensionPointName);
    if (it == pImpl->hooks.end()) {
        return {};
    }
    
    return it->second;
}

bool ExtensionManager::registerInterfaceExtension(const std::string& extensionPointName,
                                                  std::shared_ptr<IInterfaceExtension> extension) {
    if (!extension) {
        NLM_LOG_ERROR("Cannot register null interface extension");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto epIt = pImpl->extensionPoints.find(extensionPointName);
    if (epIt == pImpl->extensionPoints.end()) {
        NLM_LOG_ERROR("Extension point not found: " + extensionPointName);
        return false;
    }
    
    pImpl->interfaceExtensions[extensionPointName].push_back(extension);
    NLM_LOG_INFO("Interface extension registered: " + extension->getInterfaceName() + 
                " for extension point " + extensionPointName);
    return true;
}

std::vector<std::shared_ptr<IInterfaceExtension>> ExtensionManager::getInterfaceExtensions(
    const std::string& extensionPointName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->interfaceExtensions.find(extensionPointName);
    if (it == pImpl->interfaceExtensions.end()) {
        return {};
    }
    
    return it->second;
}

bool ExtensionManager::registerImplementationExtension(const std::string& extensionPointName,
                                                       std::shared_ptr<IImplementationExtension> extension) {
    if (!extension) {
        NLM_LOG_ERROR("Cannot register null implementation extension");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto epIt = pImpl->extensionPoints.find(extensionPointName);
    if (epIt == pImpl->extensionPoints.end()) {
        NLM_LOG_ERROR("Extension point not found: " + extensionPointName);
        return false;
    }
    
    pImpl->implementationExtensions[extensionPointName].push_back(extension);
    NLM_LOG_INFO("Implementation extension registered: " + extension->getExtensionPoint() +
                " for extension point " + extensionPointName);
    return true;
}

std::vector<std::shared_ptr<IImplementationExtension>> ExtensionManager::getImplementationExtensions(
    const std::string& extensionPointName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->implementationExtensions.find(extensionPointName);
    if (it == pImpl->implementationExtensions.end()) {
        return {};
    }
    
    return it->second;
}

std::any ExtensionManager::executeHooks(const std::string& extensionPointName, const std::any& argument) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto hooksIt = pImpl->hooks.find(extensionPointName);
    if (hooksIt == pImpl->hooks.end() || hooksIt->second.empty()) {
        return std::any();
    }
    
    auto& hooks = hooksIt->second;
    if (hooks.empty()) {
        return std::any();
    }
    
    NLM_LOG_INFO("Executing " + std::to_string(hooks.size()) + " hooks for extension point: " + extensionPointName);
    
    try {
        for (auto& hook : hooks) {
            hook->execute(argument);
        }
        pImpl->executeCount++;
        return std::any();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception executing hooks for extension point " + extensionPointName + ": " + std::string(e.what()));
        pImpl->errorCount++;
        return std::any();
    }
}

std::vector<std::any> ExtensionManager::executeAllHooks(const std::string& extensionPointName, const std::any& argument) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto hooksIt = pImpl->hooks.find(extensionPointName);
    if (hooksIt == pImpl->hooks.end() || hooksIt->second.empty()) {
        return {};
    }
    
    auto& hooks = hooksIt->second;
    std::vector<std::any> results;
    results.reserve(hooks.size());
    
    NLM_LOG_INFO("Executing " + std::to_string(hooks.size()) + " hooks for extension point: " + extensionPointName);
    
    try {
        for (auto& hook : hooks) {
            hook->execute(argument);
            results.push_back(std::any());
        }
        pImpl->executeCount++;
        return results;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception executing hooks for extension point " + extensionPointName + ": " + std::string(e.what()));
        pImpl->errorCount++;
        return {};
    }
}

std::vector<std::string> ExtensionManager::getExtensionPointsForType(ExtensionType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> points;
    
    for (const auto& pair : pImpl->extensionPoints) {
        // This is a simplified check - in reality, you'd need to map extension types
        if (type == ExtensionType::Hook) {
            if (pImpl->hooks.find(pair.first) != pImpl->hooks.end()) {
                points.push_back(pair.first);
            }
        } else if (type == ExtensionType::Interface) {
            if (pImpl->interfaceExtensions.find(pair.first) != pImpl->interfaceExtensions.end()) {
                points.push_back(pair.first);
            }
        } else if (type == ExtensionType::Implementation) {
            if (pImpl->implementationExtensions.find(pair.first) != pImpl->implementationExtensions.end()) {
                points.push_back(pair.first);
            }
        }
    }
    
    return points;
}

bool ExtensionManager::hasExtensionPoint(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->extensionPoints.find(name) != pImpl->extensionPoints.end();
}

ExtensionManager& ExtensionManager::getInstance() {
    static ExtensionManager instance;
    return instance;
}

void ExtensionManager::reset() {
    static ExtensionManager instance;
    // Clean up all extension points
    for (const auto& pair : instance.pImpl->extensionPoints) {
        instance.unregisterExtensionPoint(pair.first);
    }
    instance.pImpl->extensionPoints.clear();
    instance.pImpl->hooks.clear();
    instance.pImpl->interfaceExtensions.clear();
    instance.pImpl->implementationExtensions.clear();
    instance.pImpl->registerCount = 0;
    instance.pImpl->unregisterCount = 0;
    instance.pImpl->executeCount = 0;
    instance.pImpl->errorCount = 0;
}

struct ExtensionSystem::Impl {
    std::unique_ptr<IExtensionManager> manager;
    bool initialized = false;
    std::mutex mutex;
};

ExtensionSystem::ExtensionSystem() : manager_(ExtensionManager::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->manager = &manager_;
    NLM_LOG_INFO("ExtensionSystem created");
}

ExtensionSystem::~ExtensionSystem() {
    shutdown();
    NLM_LOG_INFO("ExtensionSystem destroyed");
}

bool ExtensionSystem::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("ExtensionSystem already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing ExtensionSystem");
    
    try {
        // Register common extension points
        registerCommonExtensionPoints();
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("ExtensionSystem initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing ExtensionSystem: " + std::string(e.what()));
        return false;
    }
}

void ExtensionSystem::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down ExtensionSystem");
    
    try {
        pImpl->initialized = false;
        NLM_LOG_INFO("ExtensionSystem shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down ExtensionSystem: " + std::string(e.what()));
    }
}

void ExtensionSystem::registerCommonExtensionPoints() {
    // Register common extension points for NLM
    std::vector<ExtensionPoint> commonPoints = {
        ExtensionPoint("neural_simulation", "Neural simulation hook", typeid(void), false),
        ExtensionPoint("memory_management", "Memory management extension", typeid(void), false),
        ExtensionPoint("plasticity_control", "Plasticity control hook", typeid(void), false),
        ExtensionPoint("neuromodulation", "Neuromodulation interface", typeid(void), false),
        ExtensionPoint("prediction", "Prediction system extension", typeid(void), false),
        ExtensionPoint("cognition", "Cognition interface", typeid(void), false),
        ExtensionPoint("development", "Development system hook", typeid(void), false),
        ExtensionPoint("action_selection", "Action selection interface", typeid(void), false),
        ExtensionPoint("sensory_processing", "Sensory processing hook", typeid(void), false),
        ExtensionPoint("motor_control", "Motor control interface", typeid(void), false)
    };
    
    for (const auto& point : commonPoints) {
        manager_.registerExtensionPoint(point);
    }
    
    NLM_LOG_INFO("Registered " + std::to_string(commonPoints.size()) + " common extension points");
}

ExtensionSystem& ExtensionSystem::getInstance() {
    static ExtensionSystem instance;
    return instance;
}

void ExtensionSystem::reset() {
    static ExtensionSystem instance;
    instance.shutdown();
    ExtensionManager::reset();
}

} // namespace nlm