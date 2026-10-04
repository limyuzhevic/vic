// PluginBase.cpp
#include "PluginBase.hpp"
#include <stdexcept>

namespace nlm {

// Default implementations for PluginBase

std::string PluginBase::getName() const {
    return "Base Plugin";
}

std::string PluginBase::getVersion() const {
    return "1.0.0";
}

std::string PluginBase::getDescription() const {
    return "Base plugin class";
}

std::string PluginBase::getAuthor() const {
    return "NLM Team";
}

bool PluginBase::initialize(const Config& config) {
    setConfiguration(config);
    return validateConfiguration();
}

void PluginBase::shutdown() {
    // Default implementation does nothing
}

bool PluginBase::isInitialized() const {
    return true;
}

const std::type_info& PluginBase::getTypeInfo() const {
    return typeid(PluginBase);
}

void PluginBase::setConfiguration(const Config& config) {
    // Default implementation - override in concrete implementations
}

Config PluginBase::getConfiguration() const {
    Config config;
    // Return default configuration
    return config;
}

bool PluginBase::isEnabled() const {
    return true;
}

void PluginBase::setEnabled(bool enabled) {
    // Default implementation - can be overridden
}

bool PluginBase::validateConfiguration() const {
    return true;
}

std::string PluginBase::getValidationError() const {
    return "";
}

std::string PluginBase::getId() const {
    return "plugin_base_v1.0";
}

std::chrono::steady_clock::time_point PluginBase::getLoadTime() const {
    return std::chrono::steady_clock::now();
}

} // namespace nlm