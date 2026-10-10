// PluginBase.cpp - Implementation of base plugin interface
#include "PluginBase.hpp"

namespace nlm {

void IPlugin::subscribeEvent(const std::string& eventId, IEventSubscriber* subscriber) {
    // Default implementation - derived classes should override
}

void IPlugin::unsubscribeEvent(const std::string& eventId, IEventSubscriber* subscriber) {
    // Default implementation - derived classes should override
}

void IPlugin::publishEvent(const PluginEvent& event) {
    // Default implementation - derived classes should override
}

void IPlugin::updateConfiguration(const PluginConfig& config) {
    // Default implementation - derived classes should override
}

PluginConfig IPlugin::getCurrentConfiguration() const {
    PluginConfig config;
    config.name = getName();
    config.enabled = true;
    return config;
}

} // namespace nlm