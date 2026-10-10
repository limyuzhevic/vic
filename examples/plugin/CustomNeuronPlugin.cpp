// Custom neuron plugin example implementation
#include "CustomNeuronPlugin.hpp"

namespace nlm {
namespace plugin {

// Plugin factory function
extern "C" IPlugin* createPlugin() {
    return new CustomNeuronPlugin();
}

extern "C" void destroyPlugin(IPlugin* plugin) {
    delete plugin;
}

} // namespace plugin
} // namespace nlm