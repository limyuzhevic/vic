// Plugin Factory and Registry

#include "CustomPlasticityPlugin.hpp"
#include "CustomNeuromodulatorPlugin.hpp"
#include "CustomSensoryPlugin.hpp"

// Plugin factory declarations
extern "C" {
    std::shared_ptr<nlm::cli::Plugin> createPlugin();
}

// Plugin registry
namespace nlm {
namespace plugins {

namespace PluginRegistry {
    inline void registerPlugin() {
        // Plugin registration would go here
        // In real implementation, this would be called during plugin loading
    }
}

} // namespace plugins
} // namespace nlm
