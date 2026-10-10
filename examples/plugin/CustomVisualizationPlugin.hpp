// Custom visualization plugin example implementing the IVisualizationPlugin interface
#pragma once

#include "../../src/plugin/visualization/IVisualizationPlugin.hpp"
#include "../../src/visualization/VisualizationInterface.hpp"

namespace nlm {
namespace plugin {

class CustomVisualizationPlugin : public IVisualizationPlugin {
private:
    bool initialized;
    PluginConfig currentConfig;
    std::map<std::string, std::string> visualizationParameters;
    std::string outputFormat;
    
public:
    CustomVisualizationPlugin() : initialized(false), outputFormat("text") {}
    
    std::string getName() const override {
        return "CustomVisualizationPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getAuthor() const override {
        return "NLM Developer";
    }
    
    std::string getDescription() const override {
        return "A custom visualization plugin that provides advanced visualization capabilities";
    }
    
    bool initialize(const PluginConfig& config) override {
        initialized = true;
        currentConfig = config;
        visualizationParameters = getDefaultParameters();
        return initializeVisualization();
    }
    
    void shutdown() override {
        shutdownVisualization();
        initialized = false;
        visualizationParameters.clear();
    }
    
    bool isInitialized() const override {
        return initialized;
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"nlm_visualization", "nlm_core"};
    }
    
    // IVisualizationPlugin interface implementation
    bool initializeVisualization() override {
        // Initialize visualization system
        return true;
    }
    
    void shutdownVisualization() override {
        // Cleanup visualization resources
    }
    
    bool isVisualizationReady() const override {
        return initialized;
    }
    
    void visualizeNeurons(const std::vector<std::shared_ptr<Neuron>>& neurons) override {
        // Custom neuron visualization
        std::cout << "Custom Visualization: Rendering " << neurons.size() << " neurons" << std::endl;
        for (const auto& neuron : neurons) {
            std::cout << "  Neuron " << neuron->getId() << ": V=" << neuron->getMembranePotential() << "mV" << std::endl;
        }
    }
    
    void visualizeSynapses(const std::vector<std::shared_ptr<Synapse>>& synapses) override {
        // Custom synapse visualization
        std::cout << "Custom Visualization: Rendering " << synapses.size() << " synapses" << std::endl;
        for (const auto& synapse : synapses) {
            // Synapse visualization logic
        }
    }
    
    void visualizeMemory(const Memory& memory) override {
        // Custom memory visualization
        std::cout << "Custom Visualization: Memory usage " << memory.getUsedCapacity() << "/" << memory.getTotalCapacity() << std::endl;
    }
    
    void visualizeCognition(const NeuralPlanner& planner) override {
        // Custom cognition visualization
        std::cout << "Custom Visualization: Cognitive state - " << planner.getCurrentState() << std::endl;
    }
    
    void visualizeEnvironment(const World& world) override {
        // Custom environment visualization
        std::cout << "Custom Visualization: Environment visualization not implemented" << std::endl;
    }
    
    void updateVisualization(Timestamp currentTime, TimestepDuration dt) override {
        // Real-time visualization update
    }
    
    void renderFrame() override {
        // Render current frame
    }
    
    bool configureVisualization(const std::map<std::string, std::string>& parameters) override {
        visualizationParameters = parameters;
        return true;
    }
    
    std::map<std::string, std::string> getVisualizationParameters() const override {
        return visualizationParameters;
    }
    
    std::vector<std::string> getSupportedOutputFormats() const override {
        return {"text", "csv", "json", "png", "svg"};
    }
    
    bool setOutputFormat(const std::string& format) override {
        if (std::find(getSupportedOutputFormats().begin(), getSupportedOutputFormats().end(), format) != getSupportedOutputFormats().end()) {
            outputFormat = format;
            return true;
        }
        return false;
    }
    
    std::string getCurrentOutputFormat() const override {
        return outputFormat;
    }
    
    std::map<std::string, std::string> getDefaultParameters() const override {
        return {
            {"visualizationMode", "3D"},
            {"renderStyle", "shaded"},
            {"showConnections", "true"},
            {"showActivity", "true"},
            {"animationSpeed", "1.0"}
        };
    }
    
    std::vector<std::string> getSupportedParameterTypes() const override {
        return {"visualizationMode", "renderStyle", "showConnections", "showActivity", "animationSpeed"};
    }
};

} // namespace plugin
} // namespace nlm