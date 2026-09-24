// Dependency injector implementation
#include "DependencyInjector.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace nlm {

DependencyInjector::DependencyInjector(std::shared_ptr<Config> config)
    : config_(std::move(config)) {}

template<typename T, typename... Args>
void DependencyInjector::registerComponent(const std::string& name,
    std::function<std::shared_ptr<IComponent>(Args...)> factory,
    Args&&... args) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Create bound factory
    auto boundFactory = [factory, args...]() mutable -> std::shared_ptr<IComponent> {
        return factory(args...);
    };

    factories_[name] = [boundFactory](DependencyInjector& injector) -> std::shared_ptr<IComponent> {
        return boundFactory();
    };

    // Create default instance
    ComponentInstance instance = createComponentInstance(ComponentConfig(name, typeid(T).name()));
    if (instance.component) {
        componentInstances_[name].push_back(std::move(instance));
    }
}

void DependencyInjector::registerComponentType(const std::string& name, const ComponentConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    configs_[name] = config;
}

void DependencyInjector::registerFactory(const std::string& name, ComponentFactory factory) {
    std::lock_guard<std::mutex> lock(mutex_);
    factories_[name] = std::move(factory);
}

void DependencyInjector::unregisterComponent(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = componentInstances_.find(name);
    if (it != componentInstances_.end()) {
        for (auto& instance : it->second) {
            shutdownComponentInstance(instance);
        }
        componentInstances_.erase(it);
    }
}

bool DependencyInjector::initializeComponents() {
    std::lock_guard<std::mutex> lock(mutex_);

    // First pass: create all components
    for (auto& [name, config] : configs_) {
        for (size_t i = 0; i < config.instances; ++i) {
            ComponentInstance instance = createComponentInstance(config);
            if (instance.component) {
                componentInstances_[name].push_back(std::move(instance));
            }
        }
    }

    // Second pass: initialize all components
    bool allInitialized = true;
    for (auto& [name, instances] : componentInstances_) {
        for (auto& instance : instances) {
            if (!initializeComponentInstance(instance)) {
                allInitialized = false;
            }
        }
    }

    return allInitialized;
}

void DependencyInjector::updateComponents(Timestamp deltaTime) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& [name, instances] : componentInstances_) {
        for (auto& instance : instances) {
            if (instance.state == ComponentState::Active) {
                updateComponentInstance(instance, deltaTime);
            }
        }
    }
}

void DependencyInjector::shutdownComponents() {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& [name, instances] : componentInstances_) {
        for (auto& instance : instances) {
            shutdownComponentInstance(instance);
        }
    }

    componentInstances_.clear();
}

std::shared_ptr<IComponent> DependencyInjector::getComponent(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = componentInstances_.find(name);
    if (it != componentInstances_.end() && !it->second.empty()) {
        return it->second[0].component;
    }
    return nullptr;
}

template<typename T>
std::shared_ptr<T> DependencyInjector::getComponent(const std::string& name) const {
    auto component = getComponent(name);
    if (component) {
        return std::dynamic_pointer_cast<T>(component);
    }
    return nullptr;
}

std::vector<std::string> DependencyInjector::getComponentNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(componentInstances_.size());
    for (const auto& pair : componentInstances_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> DependencyInjector::getComponentNamesByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;

    for (const auto& pair : configs_) {
        if (pair.second.configuration.find("category") != pair.second.configuration.end()) {
            auto cat = std::any_cast<std::string>(pair.second.configuration["category"]);
            if (cat == category) {
                names.push_back(pair.first);
            }
        }
    }

    return names;
}

ComponentState DependencyInjector::getComponentState(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = componentInstances_.find(name);
    if (it != componentInstances_.end() && !it->second.empty()) {
        return it->second[0].state;
    }
    return ComponentState::Inactive;
}

void DependencyInjector::setConfig(std::shared_ptr<Config> config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = std::move(config);
}

std::shared_ptr<Config> DependencyInjector::getConfig() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_;
}

void DependencyInjector::publishEvent(const ComponentEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = eventSubscriptions_.find(event.eventType);
    if (it != eventSubscriptions_.end()) {
        for (const auto& callback : it->second) {
            callback(event);
        }
    }
}

void DependencyInjector::subscribeToEvent(const std::string& eventType,
    std::function<void(const ComponentEvent&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventSubscriptions_[eventType].push_back(std::move(callback));
}

template<typename T>
std::shared_ptr<T> DependencyInjector::locate() const {
    std::lock_guard<std::mutex> lock(mutex_);

    // Find component of type T
    for (const auto& [name, instances] : componentInstances_) {
        for (const auto& instance : instances) {
            auto component = std::dynamic_pointer_cast<T>(instance.component);
            if (component) {
                return component;
            }
        }
    }

    return nullptr;
}

template<typename T>
bool DependencyInjector::hasService() const {
    return locate<T>() != nullptr;
}

bool DependencyInjector::isComponentHealthy(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = componentInstances_.find(name);
    if (it == componentInstances_.end() || it->second.empty()) {
        return false;
    }

    const auto& instance = it->second[0];
    return instance.state == ComponentState::Active;
}

std::map<std::string, std::string> DependencyInjector::getComponentHealthStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<std::string, std::string> status;

    for (const auto& [name, instances] : componentInstances_) {
        if (!instances.empty()) {
            ComponentState state = instances[0].state;
            status[name] = componentStateToString(state);
        }
    }

    return status;
}

bool DependencyInjector::loadComponentsFromConfig(const std::string& configSection) {
    if (!config_) return false;

    try {
        auto components = config_->get<std::map<std::string, std::map<std::string, std::any>>>(configSection);
        for (const auto& [name, configMap] : components) {
            ComponentConfig config;
            config.name = name;

            for (const auto& [key, value] : configMap) {
                config.configuration[key] = value;
            }

            if (config.configuration.find("type") != config.configuration.end()) {
                config.type = std::any_cast<std::string>(config.configuration["type"]);
            }

            if (config.configuration.find("enabled") != config.configuration.end()) {
                config.enabled = std::any_cast<bool>(config.configuration["enabled"]);
            }

            if (config.configuration.find("instances") != config.configuration.end()) {
                config.instances = std::any_cast<size_t>(config.configuration["instances"]);
            }

            registerComponentType(name, config);
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading components from config: " << e.what() << std::endl;
        return false;
    }
}

bool DependencyInjector::saveComponentsToConfig(std::string& configSection) const {
    if (!config_) return false;

    try {
        std::map<std::string, std::map<std::string, std::any>> components;

        for (const auto& [name, config] : configs_) {
            std::map<std::string, std::any> componentConfig;
            componentConfig["type"] = config.type;
            componentConfig["enabled"] = config.enabled;
            componentConfig["instances"] = config.instances;

            for (const auto& [key, value] : config.configuration) {
                componentConfig[key] = value;
            }

            components[name] = componentConfig;
        }

        // This would need a method in Config to set a map
        // config_->set(configSection, components);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving components to config: " << e.what() << std::endl;
        return false;
    }
}

void DependencyInjector::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [name, instances] : componentInstances_) {
        for (auto& instance : instances) {
            shutdownComponentInstance(instance);
        }
    }
    componentInstances_.clear();
    factories_.clear();
    configs_.clear();
    eventSubscriptions_.clear();
}

size_t DependencyInjector::getComponentCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& pair : componentInstances_) {
        count += pair.second.size();
    }
    return count;
}

// Implementation of private helper methods
DependencyInjector::ComponentInstance DependencyInjector::createComponentInstance(const ComponentConfig& config) {
    ComponentInstance instance;
    instance.instanceId = componentInstances_[config.name].size();
    instance.creationTime = std::chrono::steady_clock::now();

    if (factories_.find(config.name) != factories_.end()) {
        // Create using factory
        DependencyInjector& injector = *const_cast<DependencyInjector*>(this);
        instance.component = factories_[config.name](injector);
    } else {
        // Fallback to default constructor
        // This would need a more sophisticated component creation system
        instance.component = nullptr;
    }

    if (instance.component) {
        instance.state = ComponentState::Initializing;
    }

    return instance;
}

bool DependencyInjector::initializeComponentInstance(ComponentInstance& instance) {
    if (!instance.component) return false;

    try {
        DependencyInjector& injector = *const_cast<DependencyInjector*>(this);
        if (instance.component->initialize(injector)) {
            instance.state = ComponentState::Active;
            return true;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing component " << instance.component->getName()
            << ": " << e.what() << std::endl;
    }

    instance.state = ComponentState::Inactive;
    return false;
}

void DependencyInjector::updateComponentInstance(ComponentInstance& instance, Timestamp deltaTime) {
    if (!instance.component) return;

    try {
        instance.component->update(deltaTime);
        instance.lastUpdateTime = std::chrono::steady_clock::now();
    }
    catch (const std::exception& e) {
        std::cerr << "Error updating component " << instance.component->getName()
            << ": " << e.what() << std::endl;
        instance.state = ComponentState::Inactive;
    }
}

void DependencyInjector::shutdownComponentInstance(ComponentInstance& instance) {
    if (instance.component && instance.state != ComponentState::Shutdown) {
        try {
            instance.component->shutdown();
            instance.state = ComponentState::Shutdown;
        }
        catch (const std::exception& e) {
            std::cerr << "Error shutting down component " << instance.component->getName()
                << ": " << e.what() << std::endl;
        }
    }
}

std::string DependencyInjector::generateInstanceName(const std::string& baseName, size_t instanceId) const {
    if (instanceId == 0) {
        return baseName;
    }
    return baseName + "_" + std::to_string(instanceId);
}

bool DependencyInjector::resolveDependencies(ComponentInstance& instance) {
    // This would resolve dependencies between components
    // For now, return true as a placeholder
    return true;
}

std::string DependencyInjector::componentStateToString(ComponentState state) const {
    switch (state) {
    case ComponentState::Inactive: return "Inactive";
    case ComponentState::Initializing: return "Initializing";
    case ComponentState::Active: return "Active";
    case ComponentState::Updating: return "Updating";
    case ComponentState::ShuttingDown: return "ShuttingDown";
    case ComponentState::Shutdown: return "Shutdown";
    default: return "Unknown";
    }
}

} // namespace nlm