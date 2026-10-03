#pragma once

#include "PluginBase.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include <map>

namespace nlm {

class STDPPlugin : public Plugin<STDPPlugin> {
public:
    STDPPlugin() {
        // Set plugin metadata
        metadata.name = "STDP_Plugin";
        metadata.version = "1.0.0";
        metadata.description = "Spike-Timing-Dependent Plasticity plugin with configurable parameters";
        metadata.author = "NLM Team";
        metadata.pluginType = "plasticity";
        metadata.isBuiltIn = true;
        metadata.libraryPath = "";
    }
    
    const std::string& getName() const override { return metadata.name; }
    const std::string& getVersion() const override { return metadata.version; }
    const std::string& getDescription() const override { return metadata.description; }
    const std::string& getAuthor() const override { return metadata.author; }
    
    std::unique_ptr<PluginBase> clone() const override {
        return std::make_unique<STDPPlugin>(*this);
    }
    
    void* getLibraryHandle() const override { return nullptr; }
    void setLibraryHandle(void* handle) override {}
    
    // Plugin lifecycle
    bool initialize(const std::unordered_map<std::string, std::string>& config) override {
        Plugin<STDPPlugin>::initialize(config);
        
        // Initialize STDP parameters from config
        for (const auto& pair : config) {
            setConfig(pair.first, pair.second);
        }
        
        // Initialize internal STDP instances
        if (!stdp_) {
            stdp_ = std::make_unique<STDP>();
        }
        
        // Configure STDP parameters
        configureFromConfig();
        
        return true;
    }
    
    void shutdown() override {
        stdp_.reset();
        Plugin<STDPPlugin>::shutdown();
    }
    
    // Configuration management
    void configureFromConfig() {
        if (configMap.find("stdp_ltp_weight") != configMap.end()) {
            float weight = std::stof(configMap["stdp_ltp_weight"]);
            if (stdp_) {
                // This would require access to STDP's internal state
                // For now, we just store the configuration
            }
        }
        
        if (configMap.find("stdp_ltd_weight") != configMap.end()) {
            float weight = std::stof(configMap["stdp_ltd_weight"]);
            if (stdp_) {
                // Similar to above
            }
        }
        
        if (configMap.find("stdp_tau") != configMap.end()) {
            float tau = std::stof(configMap["stdp_tau"]);
            if (stdp_) {
                // Similar to above
            }
        }
    }
    
    // Resource management
    void reserveResources(size_t numNeurons, size_t numSynapses) override {
        // Pre-allocate resources for STDP operations
        maxNeurons_ = numNeurons;
        maxSynapses_ = numSynapses;
    }
    
    void releaseResources() override {
        maxNeurons_ = 0;
        maxSynapses_ = 0;
    }
    
    // Statistics
    size_t getNeuronCount() const override { return maxNeurons_; }
    size_t getSynapseCount() const override { return maxSynapses_; }
    float getMemoryUsage() const override { return sizeof(STDPPlugin); }
    
    // Plugin-specific operations
    void update(double timestep) override {
        if (stdp_) {
            // Update STDP parameters based on configuration
            // This would typically be called during simulation
        }
    }
    
    void reset() override {
        if (stdp_) {
            stdp_->reset();
        }
    }
    
    // External interface to actual STDP implementation
    void applySTDP(Neuron* preNeuron, Neuron* postNeuron, 
                   float weightBefore, float& weightAfter) {
        if (!stdp_ || !preNeuron || !postNeuron) return;
        
        // Simulate STDP calculation
        // This would use the actual STDP implementation from plasticity/STDP.cpp
        float timeDiff = 0.0f; // Would be calculated from spike times
        weightAfter = weightBefore + stdp_->calculateWeightChange(timeDiff, weightBefore);
    }
    
    PluginMetadata getMetadata() const { return metadata; }
    
private:
    PluginMetadata metadata;
    std::unique_ptr<STDP> stdp_;
    size_t maxNeurons_ = 0;
    size_t maxSynapses_ = 0;
};

} // namespace nlm