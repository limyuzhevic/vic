// Custom memory plugin example implementation
#include "CustomMemoryPlugin.hpp"

namespace nlm {
namespace plugin {

// Plugin factory function
extern "C" IPlugin* createPlugin() {
    return new CustomMemoryPlugin();
}

extern "C" void destroyPlugin(IPlugin* plugin) {
    delete plugin;
}

} // namespace plugin
} // namespace nlm