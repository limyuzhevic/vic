// ComponentFactory.cpp implementation
#include "ComponentFactory.hpp"
#include <stdexcept>
#include <iostream>

namespace nlm {

// Static registry for component creators
static std::unordered_map<std::string, ComponentFactory::ComponentCreator>&
    ComponentFactory::getRegistry() {
    static std::unordered_map<std::string, ComponentFactory::ComponentCreator> registry;
    return registry;
}

void ComponentFactory::registerComponent(const std::string& type, ComponentCreator creator) {
    if (creator == nullptr) {
        throw std::invalid_argument("Component creator cannot be null");
    }
    getRegistry()[type] = std::move(creator);
}

std::unique_ptr<BrainComponent> ComponentFactory::createComponent(const std::string& type) {
    auto it = getRegistry().find(type);
    if (it == getRegistry().end()) {
        throw std::runtime_error("Unknown component type: " + type);
    }
    return it->second();
}

bool ComponentFactory::isRegistered(const std::string& type) {
    return getRegistry().find(type) != getRegistry().end();
}

std::vector<std::string> ComponentFactory::getRegisteredTypes() {
    std::vector<std::string> types;
    types.reserve(getRegistry().size());
    for (const auto& pair : getRegistry()) {
        types.push_back(pair.first);
    }
    return types;
}

std::unique_ptr<BrainComponent> ComponentFactory::createFromConfig(
    const std::string& type, const std::string& config) {
    // Basic config parsing - could be enhanced with JSON/YAML
    auto component = createComponent(type);
    
    // TODO: Parse config and apply to component
    // For now, just initialize
    if (!component->initialize()) {
        throw std::runtime_error("Failed to initialize component: " + type);
    }
    
    return component;
}

} // namespace nlm
