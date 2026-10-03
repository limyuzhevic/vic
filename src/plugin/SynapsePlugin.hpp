#pragma once

#include "PluginBase.hpp"
#include "../brain/Synapse.hpp"
#include <map>

namespace nlm {

class SynapsePlugin : public Plugin<SynapsePlugin> {
public:
    SynapsePlugin() {
        // Set plugin metadata
        metadata.name = "Synapse_Plugin";
        metadata.version = "1.0.0";
        metadata.description = "Advanced synapse model implementation with delay, plasticity, and short-term dynamics";
        metadata.author = "NLM Team";
        metadata.pluginType = "synapse";
        metadata.isBuiltIn = true;
        metadata.libraryPath = "";
    }
    
    const std::string& getName() const override { return metadata.name; }
    const std::string& getVersion() const override { return metadata.version; }
    const std::string& getDescription() const override { return metadata.description; }
    const std::string& getAuthor() const override { return metadata.author; }
    
    std::unique_ptr<PluginBase> clone() const override {
        return std::make_unique<SynapsePlugin>(*this);
    }
    
    void* getLibraryHandle() const override { return nullptr; }
    void setLibraryHandle(void* handle) override {}
    
    // Plugin lifecycle
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        Plugin<SynapsePlugin>::initialize(config);
        
        // Initialize synapse model parameters from config
        for (const auto& pair : config) {
            setConfig(pair.first, pair.second);
        }
        
        return true;
    }
    
    void shutdown() override {
        synapses_.clear();
        Plugin<SynapsePlugin>::shutdown();
    }
    
    // Resource management
    void reserveResources(size_t numNeurons, size_t numSynapses) override {
        maxNeurons_ = numNeurons;
        maxSynapses_ = numSynapses;
        synapses_.clear();
        synapses_.reserve(numSynapses);
    }
    
    void releaseResources() override {
        synapses_.clear();
        maxNeurons_ = 0;
        maxSynapses_ = 0;
    }
    
    // Synapse creation and management
    SynapseId createSynapse(NeuronId source, NeuronId target) {
        SynapseId id = nextSynapseId_++;
        synapses_.emplace_back(std::make_unique<Synapse>(id, source, target));
        Synapse* synapse = synapses_.back().get();
        synapseMap_[id] = synapse;
        
        // Initialize synapse parameters from configuration
        configureSynapse(synapse);
        
        return id;
    }
    
    Synapse* getSynapse(SynapseId id) {
        auto it = synapseMap_.find(id);
        return it != synapseMap_.end() ? it->second : nullptr;
    }
    
    const Synapse* getSynapse(SynapseId id) const {
        auto it = synapseMap_.find(id);
        return it != synapseMap_.end() ? it->second : nullptr;
    }
    
    // Configure synapse with parameters from config
    void configureSynapse(Synapse* synapse) {
        if (!synapse) return;
        
        // Set synaptic type based on configuration
        if (configMap.find("synapse_type") != configMap.end()) {
            std::string typeStr = configMap["synapse_type"];
            if (typeStr == "excitatory") {
                synapse->setType(SynapseType::Excitatory);
            } else if (typeStr == "inhibitory") {
                synapse->setType(SynapseType::Inhibitory);
            } else if (typeStr == "modulatory") {
                synapse->setType(SynapseType::Modulatory);
            }
        }
        
        // Set synaptic weight
        if (configMap.find("synapse_initial_weight") != configMap.end()) {
            float weight = std::stof(configMap["synapse_initial_weight"]);
            synapse->setWeight(weight);
        }
        
        // Set delay
        if (configMap.find("synapse_delay") != configMap.end()) {
            uint32_t delay = static_cast<uint32_t>(std::stoi(configMap["synapse_delay"]));
            synapse->setDelay(delay);
        }
        
        // Enable plasticity rules
        if (configMap.find("synapse_hebbian") != configMap.end()) {
            bool hebbian = configMap["synapse_hebbian"] == "true";
            bool stdp = configMap.find("synapse_stdp") != configMap.end() && 
                       configMap["synapse_stdp"] == "true";
            bool rewardModulated = configMap.find("synapse_reward_modulated") != configMap.end() && 
                                 configMap["synapse_reward_modulated"] == "true";
            
            synapse->enablePlasticity(hebbian, stdp, rewardModulated);
        }
    }
    
    // Statistics
    size_t getNeuronCount() const override { return maxNeurons_; }
    size_t getSynapseCount() const override { return synapses_.size(); }
    float getMemoryUsage() const override { 
        float totalMemory = sizeof(SynapsePlugin);
        for (const auto& synapse : synapses_) {
            totalMemory += sizeof(Synapse);
        }
        return totalMemory;
    }
    
    // Plugin-specific operations
    void update(double timestep) override {
        // Update all synapses
        for (auto& synapse : synapses_) {
            // This would integrate with the actual brain simulation
            // For now, just keep synapses active
            synapse->step(static_cast<Timestamp>(timestep * 1000.0));
        }
    }
    
    void reset() override {
        for (auto& synapse : synapses_) {
            synapse->reset();
        }
    }
    
    // Connect neurons
    bool connectNeurons(NeuronId source, NeuronId target) {
        Neuron* sourceNeuron = getNeuronFromPlugin(source);
        Neuron* targetNeuron = getNeuronFromPlugin(target);
        
        if (!sourceNeuron || !targetNeuron) return false;
        
        SynapseId synId = createSynapse(source, target);
        Synapse* synapse = getSynapse(synId);
        
        if (synapse) {
            sourceNeuron->addOutgoingSynapse(synapse);
            targetNeuron->addIncomingSynapse(synapse);
            return true;
        }
        
        return false;
    }
    
    // Helper to get neurons from this plugin (would need neuron plugin integration)
    Neuron* getNeuronFromPlugin(NeuronId id) { return nullptr; }
    
    PluginMetadata getMetadata() const { return metadata; }
    
private:
    PluginMetadata metadata;
    std::vector<std::unique_ptr<Synapse>> synapses_;
    std::unordered_map<SynapseId, Synapse*> synapseMap_;
    std::unordered_map<NeuronId, Neuron*> neuronMap_; // Would be populated by neuron plugin
    SynapseId nextSynapseId_ = 1;
    size_t maxNeurons_ = 0;
    size_t maxSynapses_ = 0;
};

} // namespace nlm