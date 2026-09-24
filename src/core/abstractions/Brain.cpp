// Brain implementation
#include "Brain.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

Brain::Brain(std::shared_ptr<Config> config, std::shared_ptr<DependencyInjector> injector)
    : config_(std::move(config)), injector_(std::move(injector)),
    initialized_(false), lastUpdateTime_(0.0), componentsInitialized_(0), componentsActive_(0) {}

bool Brain::initialize() {
    if (initialized_) return true;

    std::cout << "Initializing abstract brain with pluggable components..." << std::endl;

    // Configure components from config
    configureComponents();

    // Register all components
    for (const auto& config : componentConfigs_) {
        if (config.enabled) {
            // This would need to create components from factories
            // For now, just log
            std::cout << "Would create component: " << config.name << std::endl;
        }
    }

    // Initialize all registered components
    for (auto& [name, component] : components_) {
        if (initializeComponent(component)) {
            componentsInitialized_++;
        }
    }

    initialized_ = true;
    std::cout << "Brain initialization complete. " << componentsInitialized_ << " components initialized." << std::endl;

    return true;
}

void Brain::update(Timestamp deltaTime) {
    if (!initialized_) {
        std::cerr << "Brain not initialized!" << std::endl;
        return;
    }

    updateComponentStates(deltaTime);

    for (auto& [name, component] : components_) {
        if (componentStates_[name] == ComponentState::Active) {
            updateComponent(component, deltaTime);
        }
    }

    lastUpdateTime_ = deltaTime;
}

void Brain::shutdown() {
    if (!initialized_) return;

    std::cout << "Shutting down brain components..." << std::endl;

    for (auto& [name, component] : components_) {
        shutdownComponent(component);
    }

    components_.clear();
    componentStates_.clear();
    eventHandlers_.clear();
    componentConfigs_.clear();

    initialized_ = false;
    componentsInitialized_ = 0;
    componentsActive_ = 0;

    std::cout << "Brain shutdown complete." << std::endl;
}

void Brain::registerComponent(std::shared_ptr<IComponent> component) {
    if (!component) return;

    std::string name = component->getName();
    components_[name] = std::move(component);
    componentStates_[name] = ComponentState::Inactive;

    std::cout << "Registered component: " << name << std::endl;
}

bool Brain::registerComponentType(const BrainComponentInfo& info) {
    componentInfo_[info.name] = info;

    // Add to component configs
    BrainComponentConfig config;
    config.name = info.name;
    config.type = info.type;
    config.configuration = info.configuration;
    config.dependencies = info.dependencies;
    config.enabled = info.required;

    componentConfigs_.push_back(config);

    return true;
}

bool Brain::unregisterComponent(const std::string& name) {
    auto it = components_.find(name);
    if (it != components_.end()) {
        shutdownComponent(it->second);
        components_.erase(it);
        componentStates_.erase(name);
        componentInfo_.erase(name);
        return true;
    }
    return false;
}

std::shared_ptr<IComponent> Brain::getComponent(const std::string& name) const {
    auto it = components_.find(name);
    if (it != components_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> Brain::getComponentNames() const {
    std::vector<std::string> names;
    names.reserve(components_.size());
    for (const auto& pair : components_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Brain::getComponentNamesByCategory(const std::string& category) const {
    std::vector<std::string> names;

    for (const auto& pair : componentInfo_) {
        if (pair.second.category == category) {
            names.push_back(pair.first);
        }
    }

    return names;
}

bool Brain::activateComponent(const std::string& name) {
    auto it = components_.find(name);
    if (it == components_.end()) return false;

    if (componentStates_[name] == ComponentState::Inactive) {
        if (initializeComponent(it->second)) {
            componentStates_[name] = ComponentState::Active;
            componentsActive_++;
            return true;
        }
    }
    else if (componentStates_[name] == ComponentState::Active) {
        return true;  // Already active
    }

    return false;
}

bool Brain::deactivateComponent(const std::string& name) {
    auto it = components_.find(name);
    if (it == components_.end()) return false;

    if (componentStates_[name] == ComponentState::Active) {
        shutdownComponent(it->second);
        componentStates_[name] = ComponentState::Inactive;
        componentsActive_--;
        return true;
    }

    return false;
}

ComponentState Brain::getComponentState(const std::string& name) const {
    auto it = componentStates_.find(name);
    if (it != componentStates_.end()) {
        return it->second;
    }
    return ComponentState::Inactive;
}

void Brain::publishEvent(const BrainEvent& event) {
    processEvent(event);
}

void Brain::subscribeToEvent(const std::string& eventType,
    std::function<void(const BrainEvent&)> callback) {
    eventHandlers_[eventType].push_back(std::move(callback));
}

void Brain::configureComponents() {
    if (!config_) return;

    try {
        auto brainConfig = config_->get<std::map<std::string, std::map<std::string, std::any>>>("brain.components");
        for (const auto& [name, configMap] : brainConfig) {
            BrainComponentConfig componentConfig;
            componentConfig.name = name;

            for (const auto& [key, value] : configMap) {
                componentConfig.configuration[key] = value;
            }

            componentConfigs_.push_back(componentConfig);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error configuring brain components: " << e.what() << std::endl;
    }
}

void Brain::updateComponentConfigurations() {
    for (auto& [name, component] : components_) {
        applyComponentConfiguration(component);
    }
}

size_t Brain::getComponentCount() const {
    return components_.size();
}

std::map<std::string, std::string> Brain::getSystemHealthStatus() const {
    std::map<std::string, std::string> status;

    for (const auto& pair : componentStates_) {
        status[pair.first] = componentStateToString(pair.second);
    }

    return status;
}

std::map<std::string, float> Brain::getSystemMetrics() const {
    std::map<std::string, float> metrics;

    for (const auto& pair : components_) {
        auto component = pair.second;
        auto metadata = component->getMetadata();

        // This would collect actual metrics from components
        metrics[pair.first + ".initialized"] = (getComponentState(pair.first) == ComponentState::Active) ? 1.0f : 0.0f;
    }

    return metrics;
}

std::shared_ptr<Config> Brain::getConfig() const {
    return config_;
}

std::shared_ptr<DependencyInjector> Brain::getInjector() const {
    return injector_;
}

// Implementation of protected helper methods
bool Brain::initializeComponent(std::shared_ptr<IComponent> component) {
    if (!component || !injector_) return false;

    try {
        if (component->initialize(*injector_)) {
            return true;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing component " << component->getName()
            << ": " << e.what() << std::endl;
    }

    return false;
}

void Brain::updateComponent(std::shared_ptr<IComponent> component, Timestamp deltaTime) {
    if (!component) return;

    try {
        component->update(deltaTime);
    }
    catch (const std::exception& e) {
        std::cerr << "Error updating component " << component->getName()
            << ": " << e.what() << std::endl;
    }
}

void Brain::shutdownComponent(std::shared_ptr<IComponent> component) {
    if (!component) return;

    try {
        component->shutdown();
    }
    catch (const std::exception& e) {
        std::cerr << "Error shutting down component " << component->getName()
            << ": " << e.what() << std::endl;
    }
}

bool Brain::resolveComponentDependencies(std::shared_ptr<IComponent> component) {
    if (!component) return false;

    // This would resolve component dependencies
    // For now, return true as a placeholder
    return true;
}

void Brain::processEvent(const BrainEvent& event) {
    auto it = eventHandlers_.find(event.eventType);
    if (it != eventHandlers_.end()) {
        for (const auto& callback : it->second) {
            callback(event);
        }
    }
}

void Brain::deliverEventToComponent(std::shared_ptr<IComponent> component,
    const BrainEvent& event) {
    if (!component) return;

    try {
        component->onEvent(event.eventType, event.data);
    }
    catch (const std::exception& e) {
        std::cerr << "Error delivering event to component " << component->getName()
            << ": " << e.what() << std::endl;
    }
}

void Brain::applyComponentConfiguration(std::shared_ptr<IComponent> component) {
    if (!component) return;

    std::string name = component->getName();
    std::map<std::string, std::any> config = getComponentConfigFromBrain(name);

    if (!config.empty()) {
        component->configure(config);
    }
}

std::map<std::string, std::any> Brain::getComponentConfigFromBrain(const std::string& name) const {
    for (const auto& config : componentConfigs_) {
        if (config.name == name) {
            return config.configuration;
        }
    }
    return {};
}

void Brain::updateComponentStates(Timestamp deltaTime) {
    // Update component states
    for (auto& [name, state] : componentStates_) {
        if (state == ComponentState::Active) {
            state = ComponentState::Updating;
        }
    }
}

void Brain::cleanupInactiveComponents() {
    std::vector<std::string> toRemove;

    for (const auto& pair : componentStates_) {
        if (pair.second == ComponentState::Shutdown) {
            toRemove.push_back(pair.first);
        }
    }

    for (const auto& name : toRemove) {
        components_.erase(name);
        componentStates_.erase(name);
        componentInfo_.erase(name);
    }
}

// Template helper implementations

template<typename T, typename... Args>
void BrainComponentFactory::registerFactory(DependencyInjector& injector,
    const std::string& name,
    std::function<std::shared_ptr<T>(Args...)> factory,
    Args&&... args) {
    auto boundFactory = [factory, args...]() mutable -> std::shared_ptr<IComponent> {
        return factory(args...);
    };

    injector.registerFactory(name, [boundFactory](DependencyInjector& injector) -> std::shared_ptr<IComponent> {
        return boundFactory();
    });
}

template<typename T>
void BrainComponentFactory::registerDefaultFactory(DependencyInjector& injector,
    const std::string& name) {
    // This would register a default factory for type T
    // Implementation would depend on how components are constructed
    std::cout << "Default factory for " << typeid(T).name() << " not yet implemented." << std::endl;
}

template<typename T>
std::shared_ptr<T> BrainComponentFactory::createComponent(DependencyInjector& injector,
    const std::string& name) {
    auto component = injector.getComponent(name);
    if (component) {
        return std::dynamic_pointer_cast<T>(component);
    }
    return nullptr;
}

// Component state string conversion
std::string Brain::componentStateToString(ComponentState state) const {
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