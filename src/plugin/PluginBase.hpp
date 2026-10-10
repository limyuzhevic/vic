// Core plugin base class interface for NLM neural simulation
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_set>

// Forward declarations for core types
namespace nlm {
    class Neuron;
    class Synapse;
    class Memory;
    class NeuralPlanner;
    class World;
    class Neuromodulator;
    
    // Type aliases for compatibility
    using Timestamp = double;
    using TimestepDuration = double;
    using NeuronId = uint32_t;
    using SynapseHandle = uint32_t;
    using RegionId = uint32_t;
    using PopulationId = uint32_t;
    using FiringRate = float;
    using MembranePotential = float;
    using FiringState = uint32_t;
    using PlasticityFlags = uint32_t;
    using NeuronType = uint32_t;
    using MemoryPriority = uint32_t;
    
    // Core type includes
    #include "../core/Types/Types.hpp"

enum class PluginCategory {
    NEURON,
    SYNAPSE,
    MEMORY,
    COGNITION,
    NEUROMODULATION,
    VISUALIZATION,
    ENVIRONMENT,
    ACTION,
    UNKNOWN
};

// Plugin configuration structure
struct PluginConfig {
    std::string name;
    std::map<std::string, std::string> settings;
    std::vector<std::string> enabledFeatures;
    bool enabled;
    std::string configFile;
};

// Plugin event information for plugin communication
struct PluginEvent {
    std::string eventId;
    std::string sourcePlugin;
    std::string targetPlugin;
    std::map<std::string, std::string> data;
    double timestamp;
    bool processed;
};

class IEventSubscriber {
public:
    virtual ~IEventSubscriber() = default;
    virtual void onEvent(const PluginEvent& event) = 0;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Lifecycle
    virtual bool initialize(const PluginConfig& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Plugin information
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual PluginCategory getCategory() const = 0;
    
    // Event handling
    virtual void subscribeEvent(const std::string& eventId, IEventSubscriber* subscriber);
    virtual void unsubscribeEvent(const std::string& eventId, IEventSubscriber* subscriber);
    virtual void publishEvent(const PluginEvent& event);
    
    // Configuration
    virtual void updateConfiguration(const PluginConfig& config);
    virtual PluginConfig getCurrentConfiguration() const;
};