// PluginManager implementation
#include "PluginManager.hpp"
#include <sstream>

namespace nlm {

// Convenience functions implementation
template<typename T>
void registerPlugin(const std::string& name) {
    PluginFactory<T>::registerTypedPlugin<T>(name);
}

PluginManager createPluginManager(PluginRegistry& registry) {
    return PluginManager(registry);
}

// Global plugin manager implementation
PluginManager& getPluginManager() {
    static PluginManager manager;
    return manager;
}

// PluginManager implementation
PluginManager::PluginManager() : pluginRegistry() {
    // Register core plugins
    registerExperimentPlugins();
    registerNeuromodulatorPlugins();
    registerPlasticityPlugins();
    registerSensorPlugins();
    registerWorldPlugins();
    registerVisualizerPlugins();
    registerAnalyzerPlugins();
    registerUtilityPlugins();
}

void PluginManager::registerExperimentPlugins() {
    // Register experiment plugins
    registerTypedPlugin<TemporalPredictionExperimentPlugin>("TemporalPredictionExperiment");
    // Add more experiment plugins here
}

void PluginManager::registerNeuromodulatorPlugins() {
    // Register neuromodulator plugins
    // Add more neuromodulator plugins here
}

void PluginManager::registerPlasticityPlugins() {
    // Register plasticity plugins
    // Add more plasticity plugins here
}

void PluginManager::registerSensorPlugins() {
    // Register sensor plugins
    // Add more sensor plugins here
}

void PluginManager::registerWorldPlugins() {
    // Register world plugins
    // Add more world plugins here
}

void PluginManager::registerVisualizerPlugins() {
    // Register visualizer plugins
    // Add more visualizer plugins here
}

void PluginManager::registerAnalyzerPlugins() {
    // Register analyzer plugins
    // Add more analyzer plugins here
}

void PluginManager::registerUtilityPlugins() {
    // Register utility plugins
    // Add more utility plugins here
}

} // namespace nlm
