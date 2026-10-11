#pragma once

#include "../cli/PluginManager.hpp"
#include "PluginInterface.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>

namespace nlm {
namespace plugins {

class PluginManager {
public:
    static bool initializePlugins(const std::vector<std::string>& pluginNames, 
                                 std::shared_ptr<PluginManager> manager);
    static bool shutdownPlugins(std::shared_ptr<PluginManager> manager);
    static std::string getPluginList();
};

} // namespace plugins
} // namespace nlm
