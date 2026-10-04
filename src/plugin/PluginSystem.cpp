// PluginSystem.cpp
#include "PluginSystem.hpp"
#include <memory>

namespace nlm {

namespace {
    std::unique_ptr<PluginManager> manager_;
    std::unique_ptr<PluginRegistry> registry_;
    std::unique_ptr<PluginFactory> factory_;
    std::unique_ptr<PluginLoader> loader_;
}

struct PluginSystem::Impl {
    Impl() : manager(std::make_unique<PluginManager>()),
             registry(std::make_unique<PluginRegistry>()),
             factory(std::make_unique<PluginFactory>()),
             loader(std::make_unique<PluginLoader>()) {}

    std::unique_ptr<PluginManager> manager;
    std::unique_ptr<PluginRegistry> registry;
    std::unique_ptr<PluginFactory> factory;
    std::unique_ptr<PluginLoader> loader;
};

PluginSystem::PluginSystem() : pImpl(std::make_unique<Impl>()) {}

PluginSystem::~PluginSystem() = default;

bool PluginSystem::initialize() {
    try {
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void PluginSystem::shutdown() {
    // Cleanup plugin system resources
}

PluginManager& PluginSystem::getManager() {
    if (!manager_) {
        manager_ = std::make_unique<PluginManager>();
    }
    return *manager_;
}

PluginRegistry& PluginSystem::getRegistry() {
    if (!registry_) {
        registry_ = std::make_unique<PluginRegistry>();
    }
    return *registry_;
}

PluginFactory& PluginSystem::getFactory() {
    if (!factory_) {
        factory_ = std::make_unique<PluginFactory>();
    }
    return *factory_;
}

PluginLoader& PluginSystem::getLoader() {
    if (!loader_) {
        loader_ = std::make_unique<PluginLoader>();
    }
    return *loader_;
}

bool PluginSystem::loadPluginsFromDirectory(const std::string& directory) {
    return getLoader().loadAllPluginsFromDirectory(directory);
}

std::vector<std::string> PluginSystem::getAvailablePlugins() const {
    return getRegistry().getPluginNames();
}

bool PluginSystem::isPluginAvailable(const std::string& name) const {
    return getRegistry().hasPlugin(name);
}

std::string PluginSystem::getPluginStatus(const std::string& name) const {
    if (isPluginAvailable(name)) {
        return "Available";
    }
    return "Not Available";
}

bool PluginSystem::configurePlugin(const std::string& name, const Config& config) {
    // This would be implemented based on specific plugin configuration needs
    return true;
}

Config PluginSystem::getPluginConfiguration(const std::string& name) const {
    Config config;
    // This would be implemented to retrieve actual plugin configuration
    return config;
}

bool PluginSystem::initializeAllPlugins() {
    return getManager().initializeAll();
}

bool PluginSystem::enableAllPlugins() {
    return getManager().enableAll();
}

bool PluginSystem::disableAllPlugins() {
    return getManager().disableAll();
}

bool PluginSystem::shutdownAllPlugins() {
    return getManager().shutdownAll();
}

size_t PluginSystem::getTotalPluginCount() const {
    return getManager().getTotalPluginCount();
}

size_t PluginSystem::getInitializedPluginCount() const {
    return getManager().getInitializedPluginCount();
}

size_t PluginSystem::getEnabledPluginCount() const {
    return getManager().getEnabledPluginCount();
}

std::string PluginSystem::getLastError() const {
    return getManager().getLastError();
}

} // namespace nlm