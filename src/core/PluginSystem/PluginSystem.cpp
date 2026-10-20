// PluginSystem implementation

#include "PluginSystem.hpp"

namespace nlm {

// Global plugin manager implementation
PluginManager& getPluginManager() {
    static PluginManager manager;
    return manager;
}

// Convenience function implementations
Result registerPlugin(std::unique_ptr<IPlugin> plugin) {
    return getPluginManager().registerPlugin(std::move(plugin));
}

Result unregisterPlugin(const std::string& name) {
    return getPluginManager().unregisterPlugin(name);
}

IPlugin* getPlugin(const std::string& name) {
    return getPluginManager().getPlugin(name);
}

std::vector<std::string> getPluginNames() {
    return getPluginManager().getPluginNames();
}

Result initializePlugins(const Config& config) {
    return getPluginManager().initializeAll(config);
}

Result shutdownPlugins() {
    return getPluginManager().shutdownAll();
}

void updatePlugins(float dt) {
    getPluginManager().updateAll(dt);
}

void onNeuronCreatedEvent(Neuron* neuron) {
    getPluginManager().onNeuronCreated(neuron);
}

void onNeuronDestroyedEvent(Neuron* neuron) {
    getPluginManager().onNeuronDestroyed(neuron);
}

void onSynapseCreatedEvent(Synapse* synapse) {
    getPluginManager().onSynapseCreated(synapse);
}

void onSynapseDestroyedEvent(Synapse* synapse) {
    getPluginManager().onSynapseDestroyed(synapse);
}

void onSpikeEvent(Neuron* neuron, Timestamp timestamp) {
    getPluginManager().onSpike(neuron, timestamp);
}

void onInputChangedEvent(Neuron* neuron, float delta) {
    getPluginManager().onInputChanged(neuron, delta);
}

} // namespace nlm