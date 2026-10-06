#pragma once

#include "Plugin.hpp"
#include "PluginSystem_impl.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void Plugin::registerService(const ServiceInfo& serviceInfo) {
    // In a real implementation, this would register the service with the plugin system
    // For now, this is a placeholder
    NLM_LOG_INFO(std::string("Plugin registering service: ") + serviceInfo.name);
}

void Plugin::unregisterService(const std::string& serviceName) {
    // In a real implementation, this would unregister the service from the plugin system
    // For now, this is a placeholder
    NLM_LOG_INFO(std::string("Plugin unregistering service: ") + serviceName);
}

void Plugin::emitEvent(const PluginEvent& event) {
    // In a real implementation, this would emit the event to listeners
    // For now, this is a placeholder
    NLM_LOG_INFO(std::string("Plugin emitting event: ") + std::to_string(static_cast<int>(event.type)));
}

} // namespace nlm
