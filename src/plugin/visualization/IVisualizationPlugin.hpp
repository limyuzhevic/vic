// Visualization plugin base class interface
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../PluginBase.hpp"

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class Memory;
class NeuralPlanner;
class World;

class IVisualizationPlugin : public IPlugin {
public:
    virtual ~IVisualizationPlugin() = default;
    
    // Plugin category identification
    virtual PluginCategory getCategory() const override { return PluginCategory::VISUALIZATION; }
    
    // Visualization system creation and management
    virtual bool initializeVisualization() = 0;
    virtual void shutdownVisualization() = 0;
    virtual bool isVisualizationReady() const = 0;
    
    // Data visualization
    virtual void visualizeNeurons(const std::vector<std::shared_ptr<Neuron>>& neurons) = 0;
    virtual void visualizeSynapses(const std::vector<std::shared_ptr<Synapse>>& synapses) = 0;
    virtual void visualizeMemory(const Memory& memory) = 0;
    virtual void visualizeCognition(const NeuralPlanner& planner) = 0;
    virtual void visualizeEnvironment(const World& world) = 0;
    
    // Real-time visualization
    virtual void updateVisualization(Timestamp currentTime, TimestepDuration dt) = 0;
    virtual void renderFrame() = 0;
    
    // Visualization configuration
    virtual bool configureVisualization(const std::map<std::string, std::string>& parameters) = 0;
    virtual std::map<std::string, std::string> getVisualizationParameters() const = 0;
    
    // Output formats
    virtual std::vector<std::string> getSupportedOutputFormats() const = 0;
    virtual bool setOutputFormat(const std::string& format) = 0;
    virtual std::string getCurrentOutputFormat() const = 0;
    
    // Plugin-specific configuration
    virtual std::map<std::string, std::string> getDefaultParameters() const = 0;
    virtual std::vector<std::string> getSupportedParameterTypes() const = 0;
};

} // namespace nlm