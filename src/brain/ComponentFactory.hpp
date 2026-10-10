// ComponentFactory.hpp - Factory for creating brain components
#pragma once

#include "BrainComponent.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

// Component factory for creating different types of brain components
class ComponentFactory {
public:
    // Type definition for component creation function
    using ComponentCreator = std::function<std::unique_ptr<BrainComponent>()>;
    
    // Register a component type with the factory
    static void registerComponent(const std::string& type, ComponentCreator creator);
    
    // Create a component of the specified type
    static std::unique_ptr<BrainComponent> createComponent(const std::string& type);
    
    // Check if a component type is registered
    static bool isRegistered(const std::string& type);
    
    // Get all registered component types
    static std::vector<std::string> getRegisteredTypes();
    
    // Create a component from configuration
    static std::unique_ptr<BrainComponent> createFromConfig(
        const std::string& type, const std::string& config);
    
private:
    static std::unordered_map<std::string, ComponentCreator>& getRegistry();
};

// NeuralDynamicsComponent - Component for neural dynamics management
class NeuralDynamicsComponent : public BrainComponent {
public:
    NeuralDynamicsComponent();
    ~NeuralDynamicsComponent() override;
    
    bool initialize() override;
    void update() override;
    std::string getName() const override { return "NeuralDynamicsManager"; }
    std::string getType() const override { return "neural_dynamics"; }
    bool isActive() const override { return active_; }
    void setActive(bool active) override { active_ = active; }
    
    // Neural dynamics specific methods
    void setCurrentDynamics(std::unique_ptr<class NeuralDynamics> dynamics);
    void addNeuron(Neuron* neuron);
    void removeNeuron(NeuronId id);
    
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
};

// PlasticityComponent - Component for plasticity management
class PlasticityComponent : public BrainComponent {
public:
    PlasticityComponent();
    ~PlasticityComponent() override;
    
    bool initialize() override;
    void update() override;
    std::string getName() const override { return "PlasticityManager"; }
    std::string getType() const override { return "plasticity"; }
    bool isActive() const override { return active_; }
    void setActive(bool active) override { active_ = active; }
    
    // Plasticity specific methods
    void enablePlasticityType(const std::string& type, bool enable);
    bool isPlasticityTypeEnabled(const std::string& type) const;
    
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
};

// MemoryComponent - Component for memory management
class MemoryComponent : public BrainComponent {
public:
    MemoryComponent();
    ~MemoryComponent() override;
    
    bool initialize() override;
    void update() override;
    std::string getName() const override { return "MemoryManager"; }
    std::string getType() const override { return "memory"; }
    bool isActive() const override { return active_; }
    void setActive(bool active) override { active_ = active; }
    
    // Memory specific methods
    void enableMemorySystem(const std::string& type, bool enable);
    bool isMemorySystemEnabled(const std::string& type) const;
    
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
};

// PredictionComponent - Component for prediction management
class PredictionComponent : public BrainComponent {
public:
    PredictionComponent();
    ~PredictionComponent() override;
    
    bool initialize() override;
    void update() override;
    std::string getName() const override { return "PredictionManager"; }
    std::string getType() const override { return "prediction"; }
    bool isActive() const override { return active_; }
    void setActive(bool active) override { active_ = active; }
    
    // Prediction specific methods
    void enablePrediction(bool enable);
    bool isPredictionEnabled() const;
    
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
};

} // namespace nlm
