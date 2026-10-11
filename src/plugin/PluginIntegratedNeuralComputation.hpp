#pragma once

#include "PluginBase.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <vector>

namespace nlm {

class PluginIntegratedNeuralComputation {
public:
    PluginIntegratedNeuralComputation();
    ~PluginIntegratedNeuralComputation();
    
    // Plugin-based neural computation
    void processNeuronsWithPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    void processSynapsesWithPlugins(std::vector<Synapse*>& synapses, double deltaTime);
    void initializeNeuronsWithPlugins(NeuralRegion* region);
    
    // Plugin-specific processing
    void applyNeuralNetworkPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    void applyMemoryPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    void applyNeuromodulationPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    void applyCognitionPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    void applyDevelopmentPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    void applyPlasticityPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    void applyVisualizationPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    void applyInputOutputPlugins(std::vector<Neuron*>& neurons, double deltaTime);
    
    // Plugin coordination
    void coordinatePluginNeurals(std::vector<Neuron*>& neurons, double deltaTime);
    void integratePluginOutputs(std::vector<Neuron*>& neurons, double deltaTime);
    
    // Plugin management
    void addPlugin(std::shared_ptr<NLMPlugin> plugin);
    void removePlugin(const std::string& pluginName);
    std::vector<std::shared_ptr<NLMPlugin>> getPlugins() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
