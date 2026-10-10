// Custom visualization plugin example implementation
#include "CustomVisualizationPlugin.hpp"

namespace nlm {
namespace plugin {

// Plugin factory function
extern "C" IPlugin* createPlugin() {
    return new CustomVisualizationPlugin();
}

extern "C" void destroyPlugin(IPlugin* plugin) {
    delete plugin;
}

} // namespace plugin
} // namespace nlm