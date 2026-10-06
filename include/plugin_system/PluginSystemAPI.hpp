#pragma once

#include "PluginSystem_Singleton.hpp"

namespace nlm {

// Convenience functions for plugin system usage
inline bool initializePluginSystem(std::shared_ptr<Config> config) {
    return PluginSystem::getInstance().initialize(config);
}

inline bool loadPlugin(const std::string& pluginPath) {
    return PluginSystem::loadPlugin(pluginPath);
}

inline bool unloadPlugin(const std::string& pluginName) {
    return PluginSystem::unloadPlugin(pluginName);
}

inline bool enablePlugin(const std::string& pluginName) {
    return PluginSystem::enablePlugin(pluginName);
}

inline bool disablePlugin(const std::string& pluginName) {
    return PluginSystem::disablePlugin(pluginName);
}

inline bool generatePluginTemplate(const std::string& templateType,
                                  const std::string& outputPath,
                                  const std::string& pluginName = "",
                                  const std::string& serviceName = "") {
    return PluginSystem::generatePluginTemplate(templateType, outputPath, pluginName, serviceName);
}

inline void setBrainReference(Brain* brain) {
    PluginSystem::setBrainReference(brain);
}

inline void logPluginSystemStatus() {
    PluginSystem::logStatus();
}

} // namespace nlm
