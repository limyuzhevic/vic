// Implementation of BasePlugin
#include "Plugin.hpp"
#include <chrono>
#include <algorithm>
#include <sstream>

namespace nlm {

// Helper function to convert plugin state to string
std::string pluginStateToString(PluginState state) {
    switch (state) {
        case PluginState::Unloaded: return "Unloaded";
        case PluginState::Loading: return "Loading";
        case PluginState::Loaded: return "Loaded";
        case PluginState::Initializing: return "Initializing";
        case PluginState::Initialized: return "Initialized";
        case PluginState::Configuring: return "Configuring";
        case PluginState::Configured: return "Configured";
        case PluginState::Activating: return "Activating";
        case PluginState::Active: return "Active";
        case PluginState::Running: return "Running";
        case PluginState::ShuttingDown: return "ShuttingDown";
        case PluginState::Failed: return "Failed";
        case PluginState::Disabled: return "Disabled";
        default: return "Unknown";
    }
}

// PluginEventImpl implementation
std::string PluginEventImpl::toString() const {
    std::ostringstream oss;
    oss << "PluginEvent(type=" << static_cast<int>(type_) 
        << ", source=" << source_ 
        << ", timestamp=" << std::chrono::system_clock::to_time_t(timestamp_)
        << ")";
    return oss.str();
}

// Factory function for creating plugin events
std::shared_ptr<PluginEvent> createPluginEvent(PluginEventType type, const std::string& source,
                                               const std::unordered_map<std::string, std::any>& data) {
    return std::make_shared<PluginEventImpl>(type, source, data);
}

} // namespace nlm
