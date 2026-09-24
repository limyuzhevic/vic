#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>
#include <type_traits>

namespace nlm {

// Forward declarations
class AgentBrain;
class SensoryPercept;
class Neuron;
class NeuralRegion;
class MotorCommand;

// Plugin system for extending the agent
class AgentPlugin {
public:
    virtual ~AgentPlugin() = default;
    
    // Plugin name for identification
    virtual std::string getName() const = 0;
    
    // Plugin version
    virtual std::string getVersion() const = 0;
    
    // Initialize plugin with agent instance
    virtual void initialize(AgentBrain* agent) = 0;
    
    // Cleanup plugin
    virtual void cleanup() = 0;
    
    // Enable/disable plugin
    virtual void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Check if plugin can process the given sensory data
    virtual bool canProcessSensory(const SensoryPercept& percept) const { return false; }
    
    // Check if plugin can handle motor commands
    virtual bool canHandleMotor(MotorCommand cmd) const { return false; }
    
protected:
    bool enabled_ = true;
    AgentBrain* agent_ = nullptr;
};

// Custom brain region registration system
class BrainRegionRegistry {
public:
    using RegionFactory = std::function<std::unique_ptr<NeuralRegion>()>;
    using NeuronFactory = std::function<std::unique_ptr<Neuron>()>;
    using SensoryProcessor = std::function<void(const SensoryPercept&, std::vector<float>&)>;
    using MotorProcessor = std::function<MotorCommand(const std::vector<float>&)>;
    
    // Register a custom neural region factory
    static void registerRegionFactory(const std::string& regionType, const RegionFactory& factory);
    
    // Register a custom neuron factory for a specific type
    static void registerNeuronFactory(NeuronType type, const NeuronFactory& factory);
    
    // Register a sensory processor for a specific region type
    static void registerSensoryProcessor(const std::string& regionType, const SensoryProcessor& processor);
    
    // Register a motor processor for a specific output type
    static void registerMotorProcessor(MotorCommand outputType, const MotorProcessor& processor);
    
    // Get region factory
    static std::unique_ptr<NeuralRegion> createRegion(const std::string& regionType);
    
    // Get neuron factory
    static std::unique_ptr<Neuron> createNeuron(NeuronType type);
    
    // Get sensory processor
    static SensoryProcessor getSensoryProcessor(const std::string& regionType);
    
    // Get motor processor
    static MotorProcessor getMotorProcessor(MotorCommand outputType);
    
private:
    static std::unordered_map<std::string, RegionFactory> regionFactories_;
    static std::unordered_map<NeuronType, NeuronFactory> neuronFactories_;
    static std::unordered_map<std::string, SensoryProcessor> sensoryProcessors_;
    static std::unordered_map<MotorCommand, MotorProcessor> motorProcessors_;
};

// Advanced neural network customization
class NeuralNetworkCustomizer {
public:
    // Custom synaptic weight initialization
    using WeightInitializer = std::function<float(float, float, float)>;
    
    // Custom activation function
    using ActivationFunction = std::function<float(float)>;
    
    // Custom plasticity rule
    using PlasticityRule = std::function<void(float&, float&, float, float, float)>;
    
    // Register custom weight initializer
    static void registerWeightInitializer(const std::string& name, const WeightInitializer& initializer);
    
    // Register custom activation function
    static void registerActivationFunction(const std::string& name, const ActivationFunction& function);
    
    // Register custom plasticity rule
    static void registerPlasticityRule(const std::string& name, const PlasticityRule& rule);
    
    // Get custom component by name
    static WeightInitializer getWeightInitializer(const std::string& name);
    static ActivationFunction getActivationFunction(const std::string& name);
    static PlasticityRule getPlasticityRule(const std::string& name);
    
    // Apply custom configuration to existing components
    static void applyCustomConfiguration(AgentBrain* agent, const std::string& configName);
    
private:
    static std::unordered_map<std::string, WeightInitializer> weightInitializers_;
    static std::unordered_map<std::string, ActivationFunction> activationFunctions_;
    static std::unordered_map<std::string, PlasticityRule> plasticityRules_;
};

// Advanced event system for agent behavior
class AgentEventSystem {
public:
    // Event types
    enum class EventType {
        SensoryInput,
        MotorOutput,
        RewardReceived,
        NoveltyDetected,
        CuriosityTriggered,
        DevelopmentStageChanged,
        MemoryStored,
        MemoryReplayed,
        ErrorOccurred,
        Custom
    };
    
    // Event structure
    struct Event {
        EventType type;
        double timestamp;
        std::string source;
        std::string data;  // JSON string for event-specific data
        std::shared_ptr<void> payload;  // Type-erased payload for complex data
    };
    
    // Event handler
    using EventHandler = std::function<void(const Event& event)>;
    
    // Event filter
    using EventFilter = std::function<bool(const Event& event)>;
    
    // Register event handler
    static void registerEventHandler(EventType type, const EventHandler& handler);
    
    // Register event filter
    static void registerEventFilter(EventType type, const EventFilter& filter);
    
    // Dispatch event
    static void dispatchEvent(const Event& event);
    
    // Get event history
    static std::vector<Event> getEventHistory(EventType type = EventType::Custom, size_t maxEvents = 100);
    
    // Clear event history
    static void clearEventHistory(EventType type = EventType::Custom);
    
private:
    static std::vector<std::pair<EventType, EventHandler>> eventHandlers_;
    static std::vector<std::pair<EventType, EventFilter>> eventFilters_;
    static std::vector<Event> eventHistory_;
};

// Advanced configuration extensions
class AdvancedConfig {
public:
    // Custom parameter groups
    struct NeuralParameters {
        float learningRate = 0.01f;
        float momentum = 0.9f;
        float decayRate = 0.0001f;
        float inhibitionStrength = 1.0f;
        float excitationStrength = 1.0f;
        bool enableSTDP = true;
        bool enableHebbian = true;
        bool enableStructuralPlasticity = true;
    };
    
    struct MemoryParameters {
        size_t capacity = 1000;
        float consolidationThreshold = 0.8f;
        bool enableReplay = true;
        float replayFrequency = 0.1f;
        float encodingStrength = 1.0f;
    };
    
    struct DevelopmentParameters {
        double growthRate = 1.0f;
        double maturationAge = 300.0f;
        float developmentalStageMultiplier = 1.0f;
        bool enableSynaptogenesis = true;
        bool enablePruning = true;
        bool enableMaturation = true;
    };
    
    struct SensoryParameters {
        float visionResolution = 0.25f;
        float touchSensitivity = 0.1f;
        float proprioceptionGain = 1.0f;
        bool enableAudio = false;
        float audioGain = 1.0f;
    };
    
    // Get advanced configuration parameters
    static const NeuralParameters& getNeuralConfig();
    static const MemoryParameters& getMemoryConfig();
    static const DevelopmentParameters& getDevelopmentConfig();
    static const SensoryParameters& getSensoryConfig();
    
    // Set advanced configuration parameters
    static void setNeuralConfig(const NeuralParameters& params);
    static void setMemoryConfig(const MemoryParameters& params);
    static void setDevelopmentConfig(const DevelopmentParameters& params);
    static void setSensoryConfig(const SensoryParameters& params);
    
    // Apply advanced configuration
    static void applyAdvancedConfig(AgentBrain* agent);
    
private:
    static NeuralParameters neuralConfig_;
    static MemoryParameters memoryConfig_;
    static DevelopmentParameters developmentConfig_;
    static SensoryParameters sensoryConfig_;
};

// High-performance batch processing
class BatchProcessor {
public:
    // Batch of sensory inputs
    struct SensoryBatch {
        std::vector<SensoryPercept> percepts;
        std::vector<double> timestamps;
        std::vector<float> rewards;
    };
    
    // Batch of motor commands
    struct MotorBatch {
        std::vector<MotorCommand> commands;
        std::vector<double> executionTimes;
        std::vector<float> outcomes;
    };
    
    // Batch processing callback
    using BatchCallback = std::function<void(const SensoryBatch& input, MotorBatch& output)>;
    
    // Register batch processor
    static void registerBatchProcessor(const BatchCallback& processor);
    
    // Process a batch of sensory inputs
    static MotorBatch processBatch(const SensoryBatch& input);
    
    // Clear all batch processors
    static void clearBatchProcessors();
    
private:
    static std::vector<BatchCallback> batchProcessors_;
};

} // namespace nlm