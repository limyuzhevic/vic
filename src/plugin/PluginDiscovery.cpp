#include <filesystem>
#include <fstream>
#include <algorithm>
#include <vector>

namespace nlm {

namespace {
    const std::vector<std::string> pluginExtensions = {".so", ".dll", ".dylib"};
    const std::vector<std::string> builtInPluginFiles = {
        "libnlm_plugin_memory.so",
        "libnlm_plugin_prediction.so",
        "libnlm_plugin_cognition.so",
        "libnlm_plugin_neuromodulation.so",
        "libnlm_plugin_development.so",
        "libnlm_plugin_plasticity.so"
    };
}

std::vector<std::string> PluginDiscovery::discoverPlugins(const std::string& directory) {
    std::vector<std::string> discoveredPlugins;
    
    try {
        if (!std::filesystem::exists(directory)) {
            return discoveredPlugins;
        }

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                for (const auto& ext : pluginExtensions) {
                    if (filename.size() >= ext.size() && 
                        filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0) {
                        discoveredPlugins.push_back(entry.path().string());
                        break;
                    }
                }
            }
        }
    } catch (const std::exception&) {
        // Log error in production
    }

    return discoveredPlugins;
}

std::vector<std::string> PluginDiscovery::discoverBuiltInPlugins() {
    return builtInPluginFiles;
}

} // namespace nlm