// Component Interface Base
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <any>
#include <functional>
#include "Types.hpp"

namespace nlm {

// Forward declarations for common types
class Brain;
class Config;
class DependencyInjector;

// Component lifecycle states
enum class ComponentState {
    Inactive,
    Initializing,
    Active,
    Updating,
    ShuttingDown,
    Shutdown
};

// Component metadata for registration and discovery
struct ComponentMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string category;
    std::vector<std::string> dependencies;
    std::map<std::string, std::any> configuration;
    bool isRequired;
    bool isSingleton;
};

// IComponent - Base interface for all pluggable brain modules
class IComponent {
public:
    virtual ~IComponent() = default;

    // Component lifecycle management
    virtual ComponentState getState() const = 0;
    virtual bool initialize(DependencyInjector& injector) = 0;
    virtual void update(Timestamp deltaTime) = 0;
    virtual void shutdown() = 0;

    // Component information
    virtual const ComponentMetadata& getMetadata() const = 0;
    virtual std::string getName() const = 0;

    // Dependency injection
    virtual void setDependency(const std::string& name, std::shared_ptr<IComponent> component) = 0;
    template<typename T>
    std::shared_ptr<T> getDependency(const std::string& name) const;

    // Configuration management
    virtual void configure(const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getConfiguration() const = 0;

    // Event handling
    virtual void onEvent(const std::string& eventName, const std::any& data) = 0;
    virtual void subscribeToEvent(const std::string& eventName,
        std::function<void(const std::any&)> callback) = 0;

    // Component equality comparison
    virtual bool equals(const IComponent* other) const = 0;

protected:
    std::map<std::string, std::shared_ptr<IComponent>> dependencies_;
    std::map<std::string, std::vector<std::function<void(const std::any&)>>> eventHandlers_;
    ComponentMetadata metadata_;
};

// ISensor - Interface for sensory processing components
class ISensor : public virtual IComponent {
public:
    virtual ~ISensor() = default;

    // Sensor configuration
    virtual void setInputScale(float scale) = 0;
    virtual void setNoiseLevel(float level) = 0;
    virtual void enableFiltering(bool enable) = 0;

    // Sensory processing
    virtual std::vector<float> processInput(const std::vector<float>& rawInput) = 0;
    virtual std::string getModality() const = 0;
    virtual size_t getOutputSize() const = 0;

    // Sensor-specific methods
    virtual void calibrate() = 0;
    virtual std::vector<float> getSensorResponse() const = 0;
};

// IActuator - Interface for motor output components
class IActuator : public virtual IComponent {
public:
    virtual ~IActuator() = default;

    // Actuator configuration
    virtual void setOutputRange(float min, float max) = 0;
    virtual void setResponseTime(float time) = 0;
    virtual void enableSafetyLimits(bool enable) = 0;

    // Output generation
    virtual std::vector<float> generateOutput(const std::vector<float>& neuralActivity) = 0;
    virtual std::string getActionType() const = 0;
    virtual size_t getInputSize() const = 0;

    // Actuator control
    virtual void setGain(float gain) = 0;
    virtual void setDeadband(float deadband) = 0;
    virtual bool isSafeToExecute() const = 0;
};

// IModulator - Interface for neuromodulation components
class IModulator : public virtual IComponent {
public:
    virtual ~IModulator() = default;

    // Neuromodulator configuration
    virtual void setModulationStrength(float strength) = 0;
    virtual void setBaseline(float baseline) = 0;
    virtual void setAdaptationRate(float rate) = 0;

    // Neuromodulation processing
    virtual float modulate(float input, float context) = 0;
    virtual std::string getModulationType() const = 0;
    virtual std::vector<std::string> getTargetSystems() const = 0;

    // Neuromodulator state
    virtual float getCurrentLevel() const = 0;
    virtual float getTargetLevel() const = 0;
    virtual void resetModulation() = 0;
    virtual void enableAdaptation(bool enable) = 0;
};

// IMemory - Interface for memory system components
class IMemory : public virtual IComponent {
public:
    virtual ~IMemory() = default;

    // Memory configuration
    virtual void setCapacity(size_t capacity) = 0;
    virtual void setDecayRate(float rate) = 0;
    virtual void setConsolidationInterval(size_t steps) = 0;

    // Memory operations
    virtual bool store(const std::vector<float>& data, uint64_t id = 0) = 0;
    virtual std::optional<std::vector<float>> retrieve(uint64_t id) = 0;
    virtual bool remove(uint64_t id) = 0;
    virtual void clear() = 0;

    // Memory management
    virtual size_t getActiveCount() const = 0;
    virtual size_t getCapacity() const = 0;
    virtual float getUtilizedPercentage() const = 0;
    virtual std::vector<uint64_t> getActiveIds() const = 0;

    // Memory consolidation
    virtual void consolidate() = 0;
    virtual void replayMemories(size_t count) = 0;
    virtual void enableReplay(bool enable) = 0;
};

// IPlasticity - Interface for learning/plasticity components
class IPlasticity : public virtual IComponent {
public:
    virtual ~IPlasticity() = default;

    // Plasticity configuration
    virtual void setLearningRate(float rate) = 0;
    virtual void setEligibilityTraceDecay(float decay) = 0;
    virtual void setModulationFactor(float factor) = 0;

    // Plasticity operations
    virtual void updatePlasticity(float neuromodulatorLevel, Timestamp deltaTime) = 0;
    virtual float calculateWeightChange(float preActivity, float postActivity,
        const std::vector<float>& eligibilityTrace) = 0;
    virtual std::string getPlasticityRule() const = 0;

    // Plasticity state
    virtual float getCurrentLearningRate() const = 0;
    virtual size_t getEligibilityTraceLength() const = 0;
    virtual void resetEligibilityTraces() = 0;
    virtual void setLearningEnabled(bool enable) = 0;
};

// NeuronGroupDescriptor - Configuration for neuron group organization
struct NeuronGroupDescriptor {
    std::string name;
    NeuronType type;
    size_t neuronCount;
    std::string sensoryModality; // For sensory groups
    std::string motorAction;     // For motor groups
    std::map<std::string, std::any> properties;

    NeuronGroupDescriptor() : name(""), type(NeuronType::Internal),
        neuronCount(0), sensoryModality(""), motorAction("") {}

    NeuronGroupDescriptor(std::string n, NeuronType t, size_t count)
        : name(std::move(n)), type(t), neuronCount(count) {}
};

// NeuronDistributionStrategy - How neurons are allocated to groups
enum class NeuronDistributionStrategy {
    Static,        // Hard-coded distribution (current implementation)
    Configuration, // Configuration-driven distribution
    Dynamic,       // Dynamic based on brain state
    Adaptive       // Adaptive based on performance
};

// Component factory function type
using ComponentFactory = std::function<std::shared_ptr<IComponent>(DependencyInjector& injector)>;

// Event data structure for cross-component communication
struct ComponentEvent {
    std::string sender;
    std::string eventType;
    Timestamp timestamp;
    std::any data;
    std::map<std::string, std::any> metadata;

    ComponentEvent() : timestamp(0.0) {}
    ComponentEvent(std::string s, std::string et, std::any d)
        : sender(std::move(s)), eventType(std::move(et)), data(std::move(d)), timestamp(0.0) {}
};

} // namespace nlm