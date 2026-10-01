#pragma once

#include "../brain/Neuron.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

// Enumeration for different types of neuron groups
enum class NeuronGroupType {
    Motor,
    Sensory,
    Internal,
    Development,
    Neuromodulation,
    Custom
};

// Enum for motor action groups
enum class MotorActionGroup {
    Forward,
    Backward,
    TurnLeft,
    TurnRight,
    Interact,
    Wait
};

// Enum for sensory input groups
enum class SensoryInputGroup {
    Vision,
    Touch,
    Internal,
    Proprioception
};

// NeuronGroup: Abstract base class for managing collections of neurons
class NeuronGroup {
public:
    virtual ~NeuronGroup() = default;
    
    // Get the group name
    virtual const std::string& getName() const = 0;
    
    // Get the group type
    virtual NeuronGroupType getType() const = 0;
    
    // Get the group identifier
    virtual uint32_t getId() const = 0;
    
    // Get all neurons in the group
    virtual std::vector<Neuron*> getNeurons() const = 0;
    
    // Get neurons by index
    virtual Neuron* getNeuron(size_t index) = 0;
    virtual const Neuron* getNeuron(size_t index) const = 0;
    
    // Get the number of neurons
    virtual size_t size() const = 0;
    
    // Check if the group is empty
    virtual bool empty() const = 0;
    
    // Add a neuron to the group
    virtual void addNeuron(Neuron* neuron) = 0;
    
    // Remove a neuron from the group
    virtual bool removeNeuron(Neuron* neuron) = 0;
    
    // Remove neuron by index
    virtual bool removeNeuron(size_t index) = 0;
    
    // Clear all neurons from the group
    virtual void clear() = 0;
    
    // Check if a neuron is in the group
    virtual bool contains(Neuron* neuron) const = 0;
    
    // Get index of a neuron in the group
    virtual size_t getIndex(Neuron* neuron) const = 0;
    
    // Calculate activity level for the group
    virtual float getActivityLevel() const = 0;
    
    // Get average membrane potential
    virtual float getAverageMembranePotential() const = 0;
    
    // Get firing rate statistics
    virtual float getAverageFiringRate() const = 0;
    
    // Get count of active neurons
    virtual size_t getActiveCount() const = 0;
    
    // Get count of firing neurons
    virtual size_t getFiringCount() const = 0;
    
    // Reset all neurons in the group
    virtual void reset() = 0;
    
    // Update group state (for groups that need periodic updates)
    virtual void update(double timestep) = 0;
    
    // Mark the group as dirty (needs recalculation)
    virtual void markDirty() = 0;
    
    // Check if the group is dirty
    virtual bool isDirty() const = 0;
};

// NeuronGroupFactory: Factory for creating neuron groups
class NeuronGroupFactory {
public:
    virtual ~NeuronGroupFactory() = default;
    
    // Create a motor action group
    virtual std::unique_ptr<NeuronGroup> createMotorGroup(MotorActionGroup action) = 0;
    
    // Create a sensory input group
    virtual std::unique_ptr<NeuronGroup> createSensoryGroup(SensoryInputGroup input) = 0;
    
    // Create a custom group
    virtual std::unique_ptr<NeuronGroup> createCustomGroup(const std::string& name, 
                                                         NeuronGroupType type) = 0;
};

// NeuronGroupManager: Manages neuron groups and provides intelligent distribution
class NeuronGroupManager {
public:
    NeuronGroupManager(std::unique_ptr<NeuronGroupFactory> factory);
    ~NeuronGroupManager();
    
    // Initialize manager with brain regions
    void initialize(std::shared_ptr<class Brain> brain);
    
    // Register a neuron group
    void registerGroup(MotorActionGroup actionGroup, const std::string& name);
    void registerGroup(SensoryInputGroup inputGroup, const std::string& name);
    void registerGroup(NeuronGroupType type, const std::string& name);
    
    // Get a neuron group by motor action
    NeuronGroup* getMotorGroup(MotorActionGroup action);
    const NeuronGroup* getMotorGroup(MotorActionGroup action) const;
    
    // Get a neuron group by sensory input
    NeuronGroup* getSensoryGroup(SensoryInputGroup input);
    const NeuronGroup* getSensoryGroup(SensoryInputGroup input) const;
    
    // Get a neuron group by name
    NeuronGroup* getGroup(const std::string& name);
    const NeuronGroup* getGroup(const std::string& name) const;
    
    // Get all groups
    std::vector<NeuronGroup*> getAllGroups();
    std::vector<const NeuronGroup*> getAllGroups() const;
    
    // Get groups by type
    std::vector<NeuronGroup*> getGroupsByType(NeuronGroupType type);
    std::vector<const NeuronGroup*> getGroupsByType(NeuronGroupType type) const;
    
    // Process neurons from brain regions
    void processNeurons(const std::vector<std::unique_ptr<class NeuralRegion>>& regions);
    
    // Distribute neurons intelligently
    void distributeNeuron(Neuron* neuron);
    
    // Update all groups
    void update(double timestep);
    
    // Reset all groups
    void reset();
    
    // Get statistics
    size_t getTotalNeuronCount() const;
    size_t getGroupCount() const;
    float getAverageActivityLevel() const;
    
    // Set factory (for testing or custom factories)
    void setFactory(std::unique_ptr<NeuronGroupFactory> factory);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Create default neuron group factory
std::unique_ptr<NeuronGroupFactory> createDefaultNeuronGroupFactory();

} // namespace nlm