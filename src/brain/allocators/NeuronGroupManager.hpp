// Neuron group management with configuration-driven approach
#pragma once

#include "IComponent.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Neuron group allocation result
struct NeuronGroupAllocation {
    std::string name;
    NeuronType type;
    std::vector<Neuron*> neurons;
    float activityLevel;
    float resourceUsage;
    bool isActive;
    std::map<std::string, std::any> metadata;

    NeuronGroupAllocation() : type(NeuronType::Internal), activityLevel(0.0f),
        resourceUsage(0.0f), isActive(false) {}
    NeuronGroupAllocation(std::string n, NeuronType t) : name(std::move(n)), type(t),
        activityLevel(0.0f), resourceUsage(0.0f), isActive(false) {}
};

// Neuron group allocator interface
class NeuronGroupAllocator {
public:
    virtual ~NeuronGroupAllocator() = default;

    // Allocation strategy
    virtual std::vector<NeuronGroupAllocation> allocateNeurons(
        const std::vector<Neuron*>& allNeurons,
        const std::map<std::string, std::any>& config) = 0;

    virtual std::string getStrategyName() const = 0;
    virtual void configure(const std::map<std::string, std::any>& config) = 0;

    // Dynamic allocation based on brain state
    virtual void updateAllocation(const std::map<std::string, float>& brainState,
        Timestamp deltaTime) = 0;

    virtual std::vector<NeuronGroupAllocation> getCurrentAllocation() const = 0;
    virtual void reset() = 0;
};

// Static neuron group allocator (current hard-coded approach)
class StaticNeuronGroupAllocator : public NeuronGroupAllocator {
public:
    StaticNeuronGroupAllocator() = default;
    ~StaticNeuronGroupAllocator() override = default;

    std::vector<NeuronGroupAllocation> allocateNeurons(
        const std::vector<Neuron*>& allNeurons,
        const std::map<std::string, std::any>& config) override;

    std::string getStrategyName() const override { return "Static"; }
    void configure(const std::map<std::string, std::any>& config) override;
    void updateAllocation(const std::map<std::string, float>& brainState,
        Timestamp deltaTime) override;
    std::vector<NeuronGroupAllocation> getCurrentAllocation() const override;
    void reset() override;

private:
    std::vector<NeuronGroupAllocation> staticAllocation_;
    std::map<std::string, std::any> config_;
};

// Configuration-driven neuron group allocator
class ConfigDrivenNeuronGroupAllocator : public NeuronGroupAllocator {
public:
    ConfigDrivenNeuronGroupAllocator() = default;
    ~ConfigDrivenNeuronGroupAllocator() override = default;

    std::vector<NeuronGroupAllocation> allocateNeurons(
        const std::vector<Neuron*>& allNeurons,
        const std::map<std::string, std::any>& config) override;

    std::string getStrategyName() const override { return "ConfigurationDriven"; }
    void configure(const std::map<std::string, std::any>& config) override;
    void updateAllocation(const std::map<std::string, float>& brainState,
        Timestamp deltaTime) override;
    std::vector<NeuronGroupAllocation> getCurrentAllocation() const override;
    void reset() override;

private:
    std::vector<NeuronGroupAllocation> configAllocation_;
    std::map<std::string, std::any> config_;
    std::map<NeuronType, std::vector<std::string>> typeToGroups_;
};

// Dynamic neuron group allocator (adaptive based on brain state)
class DynamicNeuronGroupAllocator : public NeuronGroupAllocator {
public:
    DynamicNeuronGroupAllocator() = default;
    ~DynamicNeuronGroupAllocator() override = default;

    std::vector<NeuronGroupAllocation> allocateNeurons(
        const std::vector<Neuron*>& allNeurons,
        const std::map<std::string, std::any>& config) override;

    std::string getStrategyName() const override { return "Dynamic"; }
    void configure(const std::map<std::string, std::any>& config) override;
    void updateAllocation(const std::map<std::string, float>& brainState,
        Timestamp deltaTime) override;
    std::vector<NeuronGroupAllocation> getCurrentAllocation() const override;
    void reset() override;

private:
    std::vector<NeuronGroupAllocation> dynamicAllocation_;
    std::map<std::string, std::any> config_;
    std::map<std::string, float> brainStateHistory_;
    float allocationStability_;
    float adaptationRate_;
};

// Neuron group manager - orchestrates allocation strategies
class NeuronGroupManager {
public:
    NeuronGroupManager(std::shared_ptr<Config> config);
    ~NeuronGroupManager() = default;

    // Allocation management
    void initialize(const std::vector<Neuron*>& allNeurons);
    void updateAllocation(const std::map<std::string, float>& brainState, Timestamp deltaTime);
    void setAllocationStrategy(NeuronDistributionStrategy strategy);
    NeuronDistributionStrategy getAllocationStrategy() const;

    // Resource management
    float getTotalResourceUsage() const;
    float getAvailableResources() const;
    bool allocateResources(const std::string& groupName, float amount);
    void releaseResources(const std::string& groupName, float amount);

    // Neuron group access
    NeuronGroupAllocation getGroup(const std::string& name) const;
    std::vector<NeuronGroupAllocation> getGroupsByType(NeuronType type) const;
    std::vector<NeuronGroupAllocation> getAllGroups() const;
    std::vector<Neuron*> getNeurons(const std::string& groupName) const;

    // Configuration management
    void loadConfiguration(const std::string& configSection);
    void saveConfiguration(std::string& configSection) const;
    void setDefaultConfiguration();
    std::map<std::string, std::any> getGroupConfiguration(const std::string& name) const;

    // State management
    void reset();
    std::map<std::string, float> getCurrentBrainState() const;
    void updateBrainState(const std::string& stateName, float value);

    // Statistics
    size_t getGroupCount() const;
    size_t getNeuronCount() const;
    float getUtilizationPercentage() const;

    // Component interface (for integration with dependency injection)
    class NeuronGroupAllocatorComponent : public IComponent {
    public:
        NeuronGroupAllocatorComponent(std::shared_ptr<NeuronGroupManager> manager);
        ~NeuronGroupAllocatorComponent() override = default;

        // IComponent implementation
        ComponentState getState() const override;
        bool initialize(DependencyInjector& injector) override;
        void update(Timestamp deltaTime) override;
        void shutdown() override;
        const ComponentMetadata& getMetadata() const override;
        std::string getName() const override;
        void setDependency(const std::string& name, std::shared_ptr<IComponent> component) override;
        template<typename T>
        std::shared_ptr<T> getDependency(const std::string& name) const;
        void configure(const std::map<std::string, std::any>& config) override;
        std::map<std::string, std::any> getConfiguration() const override;
        void onEvent(const std::string& eventName, const std::any& data) override;
        void subscribeToEvent(const std::string& eventName,
            std::function<void(const std::any&)> callback) override;
        bool equals(const IComponent* other) const override;

    private:
        std::shared_ptr<NeuronGroupManager> manager_;
        ComponentMetadata metadata_;
    };

private:
    // Allocation strategies
    std::unique_ptr<NeuronGroupAllocator> createAllocator(NeuronDistributionStrategy strategy);

    // Component access (for backward compatibility)
    std::shared_ptr<IComponent> getComponent(const std::string& name) const;

    // Internal state
    std::unique_ptr<NeuronGroupAllocator> currentAllocator_;
    NeuronDistributionStrategy currentStrategy_;
    std::vector<NeuronGroupAllocation> neuronGroups_;
    std::vector<Neuron*> allNeurons_;

    std::shared_ptr<Config> config_;
    std::map<std::string, float> brainState_;
    float totalResources_;
    float allocatedResources_;

    // Component registry (for dependency injection)
    std::map<std::string, std::shared_ptr<IComponent>> components_;
};

// Configuration structures for neuron groups
struct MotorGroupConfig {
    std::string name;
    std::vector<std::string> actionTypes;
    float baseResourceCost;
    float activityThreshold;
    float adaptationRate;

    MotorGroupConfig() : name(""), baseResourceCost(1.0f), activityThreshold(0.5f),
        adaptationRate(0.1f) {}
    MotorGroupConfig(std::string n) : name(std::move(n)), baseResourceCost(1.0f),
        activityThreshold(0.5f), adaptationRate(0.1f) {}
};

struct SensoryGroupConfig {
    std::string name;
    std::string modality;  // "vision", "touch", "internal", "proprioception"
    size_t expectedNeuronCount;
    float inputScale;
    float noiseLevel;
    float filteringEnabled;

    SensoryGroupConfig() : name(""), expectedNeuronCount(0), inputScale(1.0f),
        noiseLevel(0.0f), filteringEnabled(false) {}
    SensoryGroupConfig(std::string n, std::string m) : name(std::move(n)),
        modality(std::move(m)), expectedNeuronCount(0), inputScale(1.0f),
        noiseLevel(0.0f), filteringEnabled(false) {}
};

// Global configuration for neuron group distribution
struct NeuronGroupDistributionConfig {
    NeuronDistributionStrategy defaultStrategy;
    std::map<std::string, MotorGroupConfig> motorGroups;
    std::map<std::string, SensoryGroupConfig> sensoryGroups;
    float resourceAllocationRatio;
    float stabilityFactor;
    float adaptationRate;
    std::map<std::string, std::any> dynamicAllocationParams;

    NeuronGroupDistributionConfig() : defaultStrategy(NeuronDistributionStrategy::Configuration),
        resourceAllocationRatio(1.0f), stabilityFactor(0.9f), adaptationRate(0.05f) {}
};

// Configuration loading/saving helpers
template<typename T>
T getConfigValue(const std::map<std::string, std::any>& config, const std::string& key, const T& defaultValue);

bool saveConfigToFile(const NeuronGroupDistributionConfig& config, const std::string& filepath);
bool loadConfigFromFile(NeuronGroupDistributionConfig& config, const std::string& filepath);

} // namespace nlm

// Include implementations for inline functions
#include "NeuronGroupManager.cpp"