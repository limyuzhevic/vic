#include "SystemInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {

Component::Component(const std::string& name, const std::string& version)
    : componentName_(name), componentVersion_(version), 
      componentId_(name + "_" + version), state_(ComponentState::NotInitialized) {
    NLM_LOG_INFO("Component created: " + name + " (v" + version + ")");
}

Component::~Component() {
    shutdown();
    NLM_LOG_INFO("Component destroyed: " + componentName_);
}

bool Component::initialize() {
    NLM_LOG_INFO("Initializing component: " + componentName_);
    
    try {
        if (!internalInitialize()) {
            NLM_LOG_ERROR("Internal initialization failed for component: " + componentName_);
            state_ = ComponentState::Error;
            return false;
        }
        
        state_ = ComponentState::Initialized;
        
        NLM_LOG_INFO("Component initialized successfully: " + componentName_);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing component " + componentName_ + ": " + std::string(e.what()));
        state_ = ComponentState::Error;
        return false;
    }
}

void Component::shutdown() {
    if (state_ == ComponentState::Shutdown || state_ == ComponentState::NotInitialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down component: " + componentName_);
    
    try {
        internalShutdown();
        state_ = ComponentState::ShuttingDown;
        state_ = ComponentState::Shutdown;
        
        NLM_LOG_INFO("Component shutdown complete: " + componentName_);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down component " + componentName_ + ": " + std::string(e.what()));
        state_ = ComponentState::Error;
    }
}

void Component::configure(const std::map<std::string, std::any>& config) {
    config_ = config;
}

std::map<std::string, std::any> Component::getConfig() const {
    return config_;
}

std::map<std::string, std::any> Component::getMetadata() const {
    std::map<std::string, std::any> metadata;
    metadata["name"] = componentName_;
    metadata["version"] = componentVersion_;
    metadata["id"] = componentId_;
    metadata["state"] = static_cast<int>(state_);
    return metadata;
}

bool Component::start() {
    NLM_LOG_INFO("Starting component: " + componentName_);
    state_ = ComponentState::Starting;
    
    // Note: Component-specific start logic would be implemented in derived classes
    bool success = true; // Placeholder
    
    if (success) {
        state_ = ComponentState::Running;
        NLM_LOG_INFO("Component started successfully: " + componentName_);
    } else {
        state_ = ComponentState::Error;
        NLM_LOG_ERROR("Failed to start component: " + componentName_);
    }
    
    return success;
}

bool Component::stop() {
    NLM_LOG_INFO("Stopping component: " + componentName_);
    state_ = ComponentState::Stopping;
    
    // Note: Component-specific stop logic would be implemented in derived classes
    bool success = true; // Placeholder
    
    if (success) {
        state_ = ComponentState::Initialized;
        NLM_LOG_INFO("Component stopped successfully: " + componentName_);
    } else {
        state_ = ComponentState::Error;
        NLM_LOG_ERROR("Failed to stop component: " + componentName_);
    }
    
    return success;
}

DefaultComponent::DefaultComponent(const std::string& name, const std::string& version)
    : Component(name, version) {
    NLM_LOG_INFO("DefaultComponent created: " + name);
}

DefaultComponent::~DefaultComponent() {
    NLM_LOG_INFO("DefaultComponent destroyed: " + componentName_);
}

bool DefaultComponent::internalInitialize() {
    NLM_LOG_INFO("DefaultComponent internal initialization: " + componentName_);
    return true;
}

void DefaultComponent::internalShutdown() {
    NLM_LOG_INFO("DefaultComponent internal shutdown: " + componentName_);
}

System::System(const std::string& name, const std::string& version)
    : systemName_(name), systemVersion_(version), 
      systemId_(name + "_" + version), state_(SystemState::NotInitialized) {
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
        distributeEvent(event);
        
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
        distributeEvent(event);
        
        state_ = SystemState::Shutdown;
        
        NLM_LOG_INFO("System shutdown complete: " + systemName_);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down system " + systemName_ + ": " + std::string(e.what()));
        state_ = SystemState::Error;
    }
}

bool System::registerComponent(std::shared_ptr<IComponent> component) {
    if (!component) {
        NLM_LOG_ERROR("Cannot register null component");
        return false;
    }
    
    const auto& name = component->getComponentName();
    
    std::lock_guard<std::mutex> lock(componentsMutex_);
    
    if (components_.find(name) != components_.end()) {
        NLM_LOG_ERROR("Component already registered: " + name);
        return false;
    }
    
    components_[name] = component;
    
    // Initialize the component
    if (!component->initialize()) {
        NLM_LOG_ERROR("Failed to initialize component: " + name);
        components_.erase(name);
        return false;
    }
    
    NLM_LOG_INFO("Component registered: " + name);
    return true;
}

std::shared_ptr<IComponent> System::getComponent(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    
    auto it = components_.find(componentName);
    if (it == components_.end()) {
        return nullptr;
    }
    
    return it->second;
}

std::vector<std::shared_ptr<IComponent>> System::getComponents() const {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    
    std::vector<std::shared_ptr<IComponent>> components;
    components.reserve(components_.size());
    
    for (const auto& pair : components_) {
        components.push_back(pair.second);
    }
    
    return components;
}

bool System::hasComponent(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    return components_.find(componentName) != components_.end();
}

bool System::registerService(const std::string& serviceName, std::any service) {
    std::lock_guard<std::mutex> lock(servicesMutex_);
    
    services_[serviceName] = service;
    NLM_LOG_INFO("Service registered: " + serviceName);
    return true;
}

std::any System::getService(const std::string& serviceName) {
    std::lock_guard<std::mutex> lock(servicesMutex_);
    
    auto it = services_.find(serviceName);
    if (it == services_.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return std::any();
    }
    
    return it->second;
}

bool System::hasService(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(servicesMutex_);
    return services_.find(serviceName) != services_.end();
}

bool System::start() {
    NLM_LOG_INFO("Starting system: " + systemName_);
    
    if (!isInitialized()) {
        NLM_LOG_ERROR("System not initialized: " + systemName_);
        return false;
    }
    
    bool success = true; // Placeholder for system-specific start logic
    
    if (success) {
        state_ = SystemState::Running;
        NLM_LOG_INFO("System started successfully: " + systemName_);
    } else {
        state_ = SystemState::Error;
        NLM_LOG_ERROR("Failed to start system: " + systemName_);
    }
    
    return success;
}

bool System::stop() {
    NLM_LOG_INFO("Stopping system: " + systemName_);
    
    bool success = true; // Placeholder for system-specific stop logic
    
    if (success) {
        state_ = SystemState::Initialized;
        NLM_LOG_INFO("System stopped successfully: " + systemName_);
    } else {
        state_ = SystemState::Error;
        NLM_LOG_ERROR("Failed to stop system: " + systemName_);
    }
    
    return success;
}

void System::configure(const std::map<std::string, std::any>& config) {
    config_ = config;
}

std::map<std::string, std::any> System::getConfig() const {
    return config_;
}

std::map<std::string, std::any> System::getMetadata() const {
    std::map<std::string, std::any> metadata;
    metadata["name"] = systemName_;
    metadata["version"] = systemVersion_;
    metadata["id"] = systemId_;
    metadata["state"] = static_cast<int>(state_);
    metadata["component_count"] = static_cast<size_t>(components_.size());
    metadata["service_count"] = static_cast<size_t>(services_.size());
    return metadata;
}

std::map<std::string, std::any> System::getStatistics() const {
    std::lock_guard<std::mutex> lock(componentsMutex_);
    std::lock_guard<std::mutex> servicesLock(servicesMutex_);
    
    std::map<std::string, std::any> stats = Component::getStatistics();
    
    stats["system_name"] = systemName_;
    stats["system_version"] = systemVersion_;
    stats["system_state"] = static_cast<int>(state_);
    stats["component_count"] = static_cast<size_t>(components_.size());
    stats["service_count"] = static_cast<size_t>(services_.size());
    stats["initialized"] = isInitialized();
    stats["running"] = isRunning();
    
    return stats;
}

bool System::validate() const {
    NLM_LOG_INFO("Validating system: " + systemName_);
    
    bool success = true;
    
    // Validate all components
    for (const auto& pair : components_) {
        if (!pair.second->isInitialized()) {
            NLM_LOG_ERROR("Component not initialized: " + pair.first + " in system " + systemName_);
            success = false;
        }
    }
    
    // Validate all services
    for (const auto& pair : services_) {
        if (!pair.second.has_value()) {
            NLM_LOG_ERROR("Service has no value: " + pair.first + " in system " + systemName_);
            success = false;
        }
    }
    
    return success;
}

std::string System::validateAll() const {
    NLM_LOG_INFO("Validating all components in system: " + systemName_);
    
    std::stringstream ss;
    bool overallSuccess = true;
    
    for (const auto& pair : components_) {
        const auto& name = pair.first;
        const auto& component = pair.second;
        
        ss << "  Component " << name << ": ";
        if (component->isInitialized()) {
            ss << "OK" << std::endl;
        } else {
            ss << "NOT INITIALIZED" << std::endl;
            overallSuccess = false;
        }
    }
    
    for (const auto& pair : services_) {
        const auto& name = pair.first;
        const auto& service = pair.second;
        
        ss << "  Service " << name << ": ";
        if (service.has_value()) {
            ss << "OK" << std::endl;
        } else {
            ss << "NO VALUE" << std::endl;
            overallSuccess = false;
        }
    }
    
    ss << "  System " << systemName_ << ": " << (overallSuccess ? "VALID" : "INVALID") << std::endl;
    
    return ss.str();
}

DefaultSystem::DefaultSystem(const std::string& name, const std::string& version)
    : System(name, version) {
    NLM_LOG_INFO("DefaultSystem created: " + name);
}

DefaultSystem::~DefaultSystem() {
    NLM_LOG_INFO("DefaultSystem destroyed: " + systemName_);
}

bool DefaultSystem::internalInitialize() {
    NLM_LOG_INFO("DefaultSystem internal initialization: " + systemName_);
    return true;
}

void DefaultSystem::internalShutdown() {
    NLM_LOG_INFO("DefaultSystem internal shutdown: " + systemName_);
}

bool DefaultSystem::configurePlugins(const std::map<std::string, std::any>& pluginConfig) {
    NLM_LOG_INFO("Configuring plugins for system: " + systemName_);
    
    // Note: Plugin configuration would be implemented in derived classes
    return true;
}

bool DefaultSystem::configureServices(const std::map<std::string, std::any>& serviceConfig) {
    NLM_LOG_INFO("Configuring services for system: " + systemName_);
    
    // Note: Service configuration would be implemented in derived classes
    return true;
}

bool DefaultSystem::configureComponents(const std::map<std::string, std::any>& componentConfig) {
    NLM_LOG_INFO("Configuring components for system: " + systemName_);
    
    // Note: Component configuration would be implemented in derived classes
    return true;
}

} // namespace nlm