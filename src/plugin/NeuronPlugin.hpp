#pragma once

#include "PluginBase.hpp"
#include "../brain/Neuron.hpp"
#include <random>
#include <algorithm>

namespace nlm {

class NeuronPlugin : public Plugin<NeuronPlugin> {
public:
    NeuronPlugin() {
        // Set plugin metadata
        metadata.name = "Neuron_Plugin";
        metadata.version = "1.0.0";
        metadata.description = "Advanced neuron model implementation (LIF, AdEx, Izhikevich) with configurable parameters";
        metadata.author = "NLM Team";
        metadata.pluginType = "neuron";
        metadata.isBuiltIn = true;
        metadata.libraryPath = "";
    }
    
    const std::string& getName() const override { return metadata.name; }
    const std::string& getVersion() const override { return metadata.version; }
    const std::string& getDescription() const override { return metadata.description; }
    const std::string& getAuthor() const override { return metadata.author; }
    
    std::unique_ptr<PluginBase> clone() const override {
        return std::make_unique<NeuronPlugin>(*this);
    }
    
    void* getLibraryHandle() const override { return nullptr; }
    void setLibraryHandle(void* handle) override {}
    
    // Plugin lifecycle
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        Plugin<NeuronPlugin>::initialize(config);
        
        // Initialize neuron model parameters from config
        for (const auto& pair : config) {
            setConfig(pair.first, pair.second);
        }
        
        // Initialize random number generator
        rng_.seed(static_cast<unsigned int>(std::time(nullptr)));
        
        return true;
    }
    
    void shutdown() override {
        Plugin<NeuronPlugin>::shutdown();
    }
    
    // Resource management
    void reserveResources(size_t numNeurons, size_t numSynapses) override {
        maxNeurons_ = numNeurons;
        maxSynapses_ = numSynapses;
        neurons_.clear();
        neurons_.reserve(numNeurons);
    }
    
    void releaseResources() override {
        neurons_.clear();
        maxNeurons_ = 0;
        maxSynapses_ = 0;
    }
    
    // Neuron creation and management
    NeuronId createNeuron(NeuronType type = NeuronType::Internal) {
        NeuronId id = nextNeuronId_++;
        neurons_.emplace_back(std::make_unique<Neuron>(id));
        neurons_.back()->setType(type);
        initializeNeuron(neurons_.back().get(), type);
        return id;
    }
    
    Neuron* getNeuron(NeuronId id) {
        auto it = neuronMap_.find(id);
        return it != neuronMap_.end() ? it->second : nullptr;
    }
    
    const Neuron* getNeuron(NeuronId id) const {
        auto it = neuronMap_.find(id);
        return it != neuronMap_.end() ? it->second : nullptr;
    }
    
    // Neuron initialization with model parameters
    void initializeNeuron(Neuron* neuron, NeuronType type) {
        if (!neuron) return;
        
        // Set membrane potential based on neuron type
        switch (type) {
            case NeuronType::Sensory:
                neuron->setRestingPotential(-60.0f);
                neuron->setThreshold(-45.0f);
                neuron->setFiringRate(20.0f);
                break;
            case NeuronType::Motor:
                neuron->setRestingPotential(-65.0f);
                neuron->setThreshold(-50.0f);
                neuron->setFiringRate(15.0f);
                break;
            case NeuronType::Excitatory:
                neuron->setRestingPotential(-70.0f);
                neuron->setThreshold(-55.0f);
                neuron->setFiringRate(25.0f);
                break;
            case NeuronType::Inhibitory:
                neuron->setRestingPotential(-65.0f);
                neuron->setThreshold(-50.0f);
                neuron->setFiringRate(10.0f);
                break;
            default: // Internal
                neuron->setRestingPotential(-70.0f);
                neuron->setThreshold(-55.0f);
                neuron->setFiringRate(5.0f);
                break;
        }
        
        // Set other parameters from configuration
        if (configMap.find("neuron_leak_conductance") != configMap.end()) {
            float conductance = std::stof(configMap["neuron_leak_conductance"]);
            neuron->setLeakConductance(conductance);
        }
        
        if (configMap.find("neuron_refractory_period") != configMap.end()) {
            uint32_t period = static_cast<uint32_t>(std::stoi(configMap["neuron_refractory_period"]));
            neuron->setRefractoryPeriod(period);
        }
        
        // Randomize parameters for biological realism
        neuron->initializeRandom(rng_);
    }
    
    // Statistics
    size_t getNeuronCount() const override { return neurons_.size(); }
    size_t getSynapseCount() const override { return maxSynapses_; }
    float getMemoryUsage() const override { 
        float totalMemory = sizeof(NeuronPlugin);
        for (const auto& neuron : neurons_) {
            totalMemory += sizeof(Neuron);
        }
        return totalMemory;
    }
    
    // Plugin-specific operations
    void update(double timestep) override {
        // Update all neurons
        for (auto& neuron : neurons_) {
            // This would integrate with the actual brain simulation
            // For now, just keep neurons active
            neuron->step(static_cast<Timestamp>(timestep * 1000.0));
        }
    }
    
    void reset() override {
        for (auto& neuron : neurons_) {
            neuron->reset();
            initializeNeuron(neuron.get(), neuron->getType());
        }
    }
    
    // Configuration management
    void enablePlasticity(NeuronId id, bool hebbian, bool stdp, bool rewardModulated) {
        Neuron* neuron = getNeuron(id);
        if (neuron) {
            neuron->enablePlasticity(hebbian, stdp, rewardModulated);
        }
    }
    
    PluginMetadata getMetadata() const { return metadata; }
    
private:
    PluginMetadata metadata;
    std::vector<std::unique_ptr<Neuron>> neurons_;
    std::unordered_map<NeuronId, Neuron*> neuronMap_;
    std::mt19937 rng_;
    NeuronId nextNeuronId_ = 1;
    size_t maxNeurons_ = 0;
    size_t maxSynapses_ = 0;
};

} // namespace nlm