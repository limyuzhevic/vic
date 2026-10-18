// Dependency Injection and Abstraction Framework for NLM
// 
// This file provides:
// 1. Abstract interfaces for core components
// 2. Dependency injection containers
// 3. Factory patterns for component creation
// 4. Abstract component implementations
// 5. Service locator pattern
// 6. Component registration and discovery
// 7. Advanced dependency resolution
// 8. Configuration-driven component setup
// 9. Lifecycle management
// 10. Integration with existing components

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace nlm {

// Forward declarations for core abstractions
class NeuronInterface;
class SynapseInterface;
class BrainInterface;
class ConfigInterface;
class EnvironmentInterface;
class SensorySystemInterface;
class MotorSystemInterface;
class MemorySystemInterface;
class PlasticityRuleInterface;
class NeuromodulationInterface;
class DevelopmentSystemInterface;

// Component registration and discovery
template<typename T>
class ComponentRegistry {
public:
    using FactoryFunc = std::function<std::shared_ptr<T>()>;
    
    static ComponentRegistry& getInstance() {
        static ComponentRegistry instance;
        return instance;
    }
    
    // Register a component factory
    void registerComponent(const std::string& name, FactoryFunc factory) {
        factories_[name] = factory;
    }
    
    // Get a component by name
    std::shared_ptr<T> getComponent(const std::string& name) {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second();n        }
        return nullptr;
    }
    
    // Check if component is registered
    bool hasComponent(const std::string& name) const {
        return factories_.find(name) != factories_.end();
    }
    
    // List all registered components
    std::vector<std::string> listComponents() const {
        std::vector<std::string> names;
        for (const auto& pair : factories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Clear all components
    void clear() {
        factories_.clear();
    }
    
private:
    ComponentRegistry() = default;
    std::unordered_map<std::string, FactoryFunc> factories_;
};

// Abstract interfaces for core components
class Component {
public:
    virtual ~Component() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
};

class NeuronInterface : public virtual Component {
public:
    virtual ~NeuronInterface() = default;
    
    // Core neuron properties
    virtual std::string getId() const = 0;
    virtual std::string getType() const = 0;
    virtual double getMembranePotential() const = 0;
    virtual double getThreshold() const = 0;
    virtual double getFiringRate() const = 0;
    virtual bool isActive() const = 0;
    virtual bool isRefractory() const = 0;
    
    // State manipulation
    virtual void setMembranePotential(double potential) = 0;
    virtual void setThreshold(double threshold) = 0;
    virtual void setFiringRate(double rate) = 0;
    
    // Dynamics
    virtual bool step(double currentTime, double dt) = 0;
    virtual void injectCurrent(double current) = 0;
    virtual void receiveExcitatoryInput(double amplitude) = 0;
    virtual void receiveInhibitoryInput(double amplitude) = 0;
    
    // Connectivity
    virtual void addIncomingSynapse(std::shared_ptr<SynapseInterface> synapse) = 0;
    virtual void addOutgoingSynapse(std::shared_ptr<SynapseInterface> synapse) = 0;
    virtual std::vector<std::shared_ptr<SynapseInterface>> getIncomingSynapses() const = 0;
    virtual std::vector<std::shared_ptr<SynapseInterface>> getOutgoingSynapses() const = 0;
    
    // Plasticity
    virtual bool hasPlasticity() const = 0;
    virtual void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) = 0;
    
    // Analysis
    virtual double getExcitabilityScore() const = 0;
    virtual std::string getStateDescription() const = 0;
    
    // Reset and initialization
    virtual void reset() = 0;
    virtual void initializeRandom(double seed) = 0;
};

class SynapseInterface : public virtual Component {
public:
    virtual ~SynapseInterface() = default;
    
    // Core synapse properties
    virtual std::string getId() const = 0;
    virtual double getWeight() const = 0;
    virtual void setWeight(double weight) = 0;
    virtual bool isEnabled() const = 0;
    virtual bool isPlastic() const = 0;
    
    // Plasticity control
    virtual void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) = 0;
    virtual void setSTDPParameters(double ltpWeight, double ltdWeight, double tau) = 0;
    
    // Connectivity
    virtual void connectTo(std::shared_ptr<NeuronInterface> preNeuron, std::shared_ptr<NeuronInterface> postNeuron) = 0;
    virtual std::shared_ptr<NeuronInterface> getPreNeuron() const = 0;
    virtual std::shared_ptr<NeuronInterface> getPostNeuron() const = 0;
    
    // Analysis
    virtual double getStrengthScore() const = 0;
    virtual std::string getStateDescription() const = 0;
    
    // Management
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual void schedulePrune(double probability) = 0;
};

class BrainInterface : public virtual Component {
public:
    virtual ~BrainInterface() = default;
    
    // Core brain operations
    virtual void initialize() = 0;
    virtual void reset() = 0;
    virtual void step(double timestep) = 0;
    virtual void step(SimulationStep step, double time) = 0;
    
    // Configuration
    virtual void configure(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfig(const std::string& key) const = 0;
    
    // Neuron and synapse management
    virtual std::shared_ptr<NeuronInterface> createNeuron(std::string type, std::string id) = 0;
    virtual std::shared_ptr<SynapseInterface> createSynapse(std::string type, std::string id) = 0;
    virtual std::shared_ptr<NeuronInterface> getNeuron(std::string id) const = 0;
    virtual std::shared_ptr<SynapseInterface> getSynapse(std::string id) const = 0;
    
    // System statistics
    virtual size_t getNeuronCount() const = 0;
    virtual size_t getSynapseCount() const = 0;
    virtual size_t getActiveNeuronCount() const = 0;
    virtual double getAverageFiringRate() const = 0;
    virtual double getExcitementInhibitionRatio() const = 0;
    
    // Analysis and monitoring
    virtual std::string getSystemStatus() const = 0;
    virtual std::vector<std::shared_ptr<NeuronInterface>> getAllNeurons() const = 0;
    virtual std::vector<std::shared_ptr<SynapseInterface>> getAllSynapses() const = 0;
    
    // State management
    virtual void saveState(const std::string& filename) = 0;
    virtual void loadState(const std::string& filename) = 0;
    
    // Development and learning
    virtual void advanceDevelopment(double timestep) = 0;
    virtual void applyReward(double reward, double predicted) = 0;
    virtual std::string getDevelopmentalStage() const = 0;
    
    // Sensory and motor integration
    virtual void receiveSensoryInput(const std::string& sensorType, const std::vector<double>& data) = 0;
    virtual std::vector<double> produceMotorOutput() const = 0;
};

class ConfigInterface : public virtual Component {
public:
    virtual ~ConfigInterface() = default;
    
    // Configuration operations
    virtual void set(const std::string& key, const std::string& value) = 0;
    virtual std::string get(const std::string& key) const = 0;
    virtual bool has(const std::string& key) const = 0;
    virtual void remove(const std::string& key) = 0;
    virtual std::vector<std::string> getKeys() const = 0;
    virtual void clear() = 0;
    
    // Validation
    virtual bool validate() const = 0;
    virtual std::string getValidationErrors() const = 0;
    
    // File operations
    virtual bool loadFromFile(const std::string& filename) = 0;
    virtual bool saveToFile(const std::string& filename) const = 0;
    
    // Export/import
    virtual std::string exportToString() const = 0;
    virtual bool importFromString(const std::string& data) = 0;
};

class EnvironmentInterface : public virtual Component {
public:
    virtual ~EnvironmentInterface() = default;
    
    // Environment configuration
    virtual void configure(double width, double height, double timestep) = 0;
    virtual void reset() = 0;
    virtual void step(double timestep) = 0;
    
    // Agent management
    virtual void setAgent(std::shared_ptr<BrainInterface> agent) = 0;
    virtual std::shared_ptr<BrainInterface> getAgent() const = 0;
    
    // World management
    virtual void addObject(const std::string& type, double x, double y, double radius) = 0;
    virtual void removeObject(double x, double y) = 0;
    virtual bool isValidPosition(double x, double y) const = 0;
    
    // Sensing
    virtual std::vector<double> getSensoryInput(const std::string& modality) const = 0;
    
    // Statistics
    virtual double getWidth() const = 0;
    virtual double getHeight() const = 0;
    virtual double getSimulationTime() const = 0;
    virtual size_t getObjectCount() const = 0;
};

class PlasticityRuleInterface : public virtual Component {
public:
    virtual ~PlasticityRuleInterface() = default;
    
    // Rule properties
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0;
    virtual double getLearningRate() const = 0;
    virtual void setLearningRate(double rate) = 0;
    
    // Plasticity computation
    virtual double computeChange(std::shared_ptr<SynapseInterface> synapse, 
                                std::shared_ptr<NeuronInterface> preNeuron,
                                std::shared_ptr<NeuronInterface> postNeuron,
                                double dt) = 0;
    
    // Configuration
    virtual void configure(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfig(const std::string& key) const = 0;
};

// Dependency injection container
class DependencyInjector {
public:
    DependencyInjector() = default;
    
    // Register a component factory
    template<typename T>
    void registerComponent(const std::string& name, std::function<std::shared_ptr<T>()> factory) {
        ComponentRegistry<T>::getInstance().registerComponent(name, factory);
    }
    
    // Register a component instance
    template<typename T>
    void registerInstance(const std::string& name, std::shared_ptr<T> instance) {
        instances_[name] = instance;
    }
    
    // Get a component by name and type
    template<typename T>
    std::shared_ptr<T> getComponent(const std::string& name) {
        // Check for registered instance first
        auto it = instances_.find(name);
        if (it != instances_.end()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        
        // Check for registered factory
        return ComponentRegistry<T>::getInstance().getComponent(name);
    }
    
    // Check if component exists
    template<typename T>
    bool hasComponent(const std::string& name) const {
        // Check for registered instance
        auto it = instances_.find(name);
        if (it != instances_.end()) {
            return std::dynamic_pointer_cast<T>(it->second) != nullptr;
        }
        
        // Check for registered factory
        return ComponentRegistry<T>::getInstance().hasComponent(name);
    }
    
    // Get all components of a specific type
    template<typename T>
    std::vector<std::shared_ptr<T>> getAllComponents() {
        std::vector<std::shared_ptr<T>> components;
        
        // Check registered instances
        for (const auto& pair : instances_) {
            auto component = std::dynamic_pointer_cast<T>(pair.second);
            if (component) {
                components.push_back(component);
            }
        }
        
        // Check registered factories
        auto& registry = ComponentRegistry<T>::getInstance();
        for (const auto& name : registry.listComponents()) {
            auto component = registry.getComponent(name);
            if (component) {
                components.push_back(component);
            }
        }
        
        return components;
    }
    
    // Clear all components
    void clear() {
        instances_.clear();
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<Component>> instances_;
};

// Abstract component factory interface
class ComponentFactory {
public:
    virtual ~ComponentFactory() = default;
    virtual std::shared_ptr<Component> createComponent(const std::string& type) = 0;
    virtual std::vector<std::string> getSupportedTypes() const = 0;
};

// Configuration-driven component creation
class ConfigurationComponentFactory : public ComponentFactory {
public:
    ConfigurationComponentFactory(std::shared_ptr<ConfigInterface> config)
        : config_(config) {}
    
    std::shared_ptr<Component> createComponent(const std::string& type) override {
        // Create component based on type and configuration
        if (type == "neuron") {
            return createNeuron();
        } else if (type == "synapse") {
            return createSynapse();
        } else if (type == "brain") {
            return createBrain();
        } else if (type == "environment") {
            return createEnvironment();
        }
        throw std::runtime_error("Unknown component type: " + type);
    }
    
    std::vector<std::string> getSupportedTypes() const override {
        return {"neuron", "synapse", "brain", "environment"};
    }
    
private:
    std::shared_ptr<NeuronInterface> createNeuron() {
        // Create neuron with configuration
        std::string neuronType = config_->get("neuron_type").value_or("internal");
        std::string neuronId = config_->get("neuron_id").value_or("1");
        
        auto neuron = std::make_shared<NeuronInterface>();
        neuron->setType(neuronType);
        return neuron;
    }
    
    std::shared_ptr<SynapseInterface> createSynapse() {
        // Create synapse with configuration
        std::string synapseType = config_->get("synapse_type").value_or("excitatory");
        std::string synapseId = config_->get("synapse_id").value_or("1");
        double initialWeight = std::stod(config_->get("synapse_weight").value_or("0.5"));
        
        auto synapse = std::make_shared<SynapseInterface>();
        synapse->setWeight(initialWeight);
        return synapse;
    }
    
    std::shared_ptr<BrainInterface> createBrain() {
        // Create brain with configuration
        std::string brainId = config_->get("brain_id").value_or("default");
        size_t neuronCount = std::stoi(config_->get("brain_neuron_count").value_or("100"));
        size_t regionCount = std::stoi(config_->get("brain_region_count").value_or("1"));
        
        auto brain = std::make_shared<BrainInterface>();
        brain->configure(neuronCount, regionCount);
        return brain;
    }
    
    std::shared_ptr<EnvironmentInterface> createEnvironment() {
        // Create environment with configuration
        double width = std::stod(config_->get("env_width").value_or("100.0"));
        double height = std::stod(config_->get("env_height").value_or("100.0"));
        double timestep = std::stod(config_->get("env_timestep").value_or("0.001"));
        
        auto environment = std::make_shared<EnvironmentInterface>();
        environment->configure(width, height, timestep);
        return environment;
    }
    
    std::shared_ptr<ConfigInterface> config_;
};

// Service locator pattern for component access
class ServiceLocator {
public:
    static DependencyInjector& getInjector() {
        static DependencyInjector injector;
        return injector;
    }
    
    static ComponentFactory& getFactory() {
        static ConfigurationComponentFactory factory(std::make_shared<ConfigInterface>());
        return factory;
    }
    
    static void setFactory(ComponentFactory& factory) {
        factory_ = &factory;
    }
    
    template<typename T>
    static std::shared_ptr<T> getService(const std::string& name) {
        return getInjector().getComponent<T>(name);
    }
    
    static void registerService(ComponentFactory& factory) {
        setFactory(factory);
    }
    
private:
    static ComponentFactory* factory_;
};

// Template component implementations
class DefaultNeuron : public NeuronInterface {
public:
    std::string getName() const override { return "DefaultNeuron"; }
    std::string getVersion() const override { return "2.0.0"; }
    
    // Implementation of NeuronInterface methods
    std::string getId() const override { return id_; }
    std::string getType() const override { return type_; }
    double getMembranePotential() const override { return membranePotential_; }
    double getThreshold() const override { return threshold_; }
    double getFiringRate() const override { return firingRate_; }
    bool isActive() const override { return isActive_; }
    bool isRefractory() const override { return isRefractory_; }
    
    void setMembranePotential(double potential) override { membranePotential_ = potential; }
    void setThreshold(double threshold) override { threshold_ = threshold; }
    void setFiringRate(double rate) override { firingRate_ = rate; }
    
    bool step(double currentTime, double dt) override {
        // Simple LIF implementation
        membranePotential_ += (restingPotential_ - membranePotential_) * dt / timeConstant_;
        
        if (membranePotential_ >= threshold_) {
            isActive_ = true;
            membranePotential_ = resetPotential_;
            isRefractory_ = true;
            refractoryTimer_ = refractoryPeriod_;
        }
        
        if (isRefractory_) {
            refractoryTimer_ -= dt;
            if (refractoryTimer_ <= 0) {
                isRefractory_ = false;
            }
        }
        
        return isActive_;
    }
    
    void injectCurrent(double current) override { totalCurrent_ += current; }
    void receiveExcitatoryInput(double amplitude) override { totalCurrent_ += amplitude; }
    void receiveInhibitoryInput(double amplitude) override { totalCurrent_ -= amplitude; }
    
    void addIncomingSynapse(std::shared_ptr<SynapseInterface> synapse) override { incomingSynapses_.push_back(synapse); }
    void addOutgoingSynapse(std::shared_ptr<SynapseInterface> synapse) override { outgoingSynapses_.push_back(synapse); }
    std::vector<std::shared_ptr<SynapseInterface>> getIncomingSynapses() const override { return incomingSynapses_; }
    std::vector<std::shared_ptr<SynapseInterface>> getOutgoingSynapses() const override { return outgoingSynapses_; }
    
    bool hasPlasticity() const override { return plasticityEnabled_; }
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) override {
        plasticityEnabled_ = hebbian || stdp || rewardModulated;
    }
    
    double getExcitabilityScore() const override {
        return std::max(0.0, (membranePotential_ - threshold_) / (threshold_ - (-80.0)));
    }
    std::string getStateDescription() const override {
        std::stringstream ss;
        ss << "Default Neuron: " << id_ << " (Type: " << type_ << ")\n";
        ss << "  Membrane Potential: " << membranePotential_ << " mV\n";
        ss << "  Threshold: " << threshold_ << " mV\n";
        ss << "  Active: " << (isActive() ? "Yes" : "No") << "\n";
        ss << "  Refractory: " << (isRefractory() ? "Yes" : "No") << "\n";
        return ss.str();
    }
    
    void reset() override {
        membranePotential_ = restingPotential_;
        isActive_ = false;
        isRefractory_ = false;
        refractoryTimer_ = 0.0;
        totalCurrent_ = 0.0;
    }
    
    void initializeRandom(double seed) override {
        std::mt19937 rng(static_cast<unsigned int>(seed));
        std::uniform_real_distribution<double> dist(-10.0, 10.0);
        
        restingPotential_ = -70.0 + dist(rng);
        threshold_ = -55.0 + dist(rng);
        resetPotential_ = restingPotential_ + dist(rng);
        timeConstant_ = 20.0 + dist(rng);
        refractoryPeriod_ = 5 + static_cast<int>(dist(rng) * 5);
    }
    
private:
    std::string id_;
    std::string type_;
    double membranePotential_;
    double threshold_;
    double restingPotential_;
    double resetPotential_;
    double timeConstant_;
    uint32_t refractoryPeriod_;
    double refractoryTimer_;
    double firingRate_;
    bool isActive_;
    bool isRefractory_;
    double totalCurrent_;
    bool plasticityEnabled_;
    
    std::vector<std::shared_ptr<SynapseInterface>> incomingSynapses_;
    std::vector<std::shared_ptr<SynapseInterface>> outgoingSynapses_;
};

class DefaultSynapse : public SynapseInterface {
public:
    std::string getName() const override { return "DefaultSynapse"; }
    std::string getVersion() const override { return "2.0.0"; }
    
    // Implementation of SynapseInterface methods
    std::string getId() const override { return id_; }
    double getWeight() const override { return weight_; }
    void setWeight(double weight) override { weight_ = weight; }
    bool isEnabled() const override { return enabled_; }
    bool isPlastic() const override { return plasticityEnabled_; }
    
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) override {
        plasticityEnabled_ = hebbian || stdp || rewardModulated;
        ltpEnabled_ = hebbian;
        ltdEnabled_ = stdp;
        rewardEnabled_ = rewardModulated;
    }
    
    void setSTDPParameters(double ltpWeight, double ltdWeight, double tau) override {
        ltpWeight_ = ltpWeight;
        ltdWeight_ = ltdWeight;
        stdpTau_ = tau;
    }
    
    void connectTo(std::shared_ptr<NeuronInterface> preNeuron, std::shared_ptr<NeuronInterface> postNeuron) override {
        preNeuron_ = preNeuron;
        postNeuron_ = postNeuron;
    }
    
    std::shared_ptr<NeuronInterface> getPreNeuron() const override { return preNeuron_; }
    std::shared_ptr<NeuronInterface> getPostNeuron() const override { return postNeuron_; }
    
    double getStrengthScore() const override {
        return std::max(0.0, (weight_ - (-1.0)) / 2.0);
    }
    
    std::string getStateDescription() const override {
        std::stringstream ss;
        ss << "Default Synapse: " << id_ << "\n";
        ss << "  Weight: " << weight_ << "\n";
        ss << "  Enabled: " << (enabled_ ? "Yes" : "No") << "\n";
        ss << "  Plastic: " << (plasticityEnabled_ ? "Yes" : "No") << "\n";
        ss << "  Strength Score: " << getStrengthScore();
        return ss.str();
    }
    
    void enable() override { enabled_ = true; }
    void disable() override { enabled_ = false; }
    void schedulePrune(double probability) override { pruneProbability_ = probability; }
    
private:
    std::string id_;
    double weight_;
    bool enabled_;
    bool plasticityEnabled_;
    bool ltpEnabled_;
    bool ltdEnabled_;
    bool rewardEnabled_;
    double ltpWeight_;
    double ltdWeight_;
    double stdpTau_;
    double pruneProbability_;
    
    std::shared_ptr<NeuronInterface> preNeuron_;
    std::shared_ptr<NeuronInterface> postNeuron_;
};

class DefaultBrain : public BrainInterface {
public:
    std::string getName() const override { return "DefaultBrain"; }
    std::string getVersion() const override { return "2.0.0"; }
    
    // Implementation of BrainInterface methods
    void initialize() override {
        for (const auto& neuron : neurons_) {
            neuron->reset();
        }
    }
    
    void reset() override {
        for (const auto& neuron : neurons_) {
            neuron->reset();
        }
        for (const auto& synapse : synapses_) {
            synapse->disable();
        }
    }
    
    void step(double timestep) override {
        // Simulate all neurons
        for (const auto& neuron : neurons_) {
            neuron->step(currentTime_, timestep);
        }
        
        currentTime_ += timestep;
    }
    
    void step(SimulationStep step, double time) override {
        currentTime_ = time;
        step(0.001); // Use default timestep
    }
    
    void configure(const std::string& key, const std::string& value) override {
        // Store configuration
        config_[key] = value;
    }
    
    std::string getConfig(const std::string& key) const override {
        auto it = config_.find(key);
        if (it != config_.end()) {
            return it->second;
        }
        return "";
    }
    
    std::shared_ptr<NeuronInterface> createNeuron(std::string type, std::string id) override {
        auto neuron = std::make_shared<DefaultNeuron>();
        neuron->setType(type);
        neurons_.push_back(neuron);
        return neuron;
    }
    
    std::shared_ptr<SynapseInterface> createSynapse(std::string type, std::string id) override {
        auto synapse = std::make_shared<DefaultSynapse>();
        synapses_.push_back(synapse);
        return synapse;
    }
    
    std::shared_ptr<NeuronInterface> getNeuron(std::string id) const override {
        for (const auto& neuron : neurons_) {
            if (neuron->getId() == id) {
                return neuron;
            }
        }
        return nullptr;
    }
    
    std::shared_ptr<SynapseInterface> getSynapse(std::string id) const override {
        for (const auto& synapse : synapses_) {
            if (synapse->getId() == id) {
                return synapse;
            }
        }
        return nullptr;
    }
    
    size_t getNeuronCount() const override { return neurons_.size(); }
    size_t getSynapseCount() const override { return synapses_.size(); }
    size_t getActiveNeuronCount() const override {
        size_t count = 0;
        for (const auto& neuron : neurons_) {
            if (neuron->isActive()) {
                count++;
            }
        }
        return count;
    }
    
    double getAverageFiringRate() const override {
        double total = 0.0;
        for (const auto& neuron : neurons_) {
            total += neuron->getFiringRate();
        }
        return neurons_.empty() ? 0.0 : total / neurons_.size();
    }
    
    double getExcitementInhibitionRatio() const override {
        // Simplified implementation
        return 1.0;
    }
    
    std::string getSystemStatus() const override {
        std::stringstream ss;
        ss << "=== Default Brain Status ===\n";
        ss << "Neurons: " << getNeuronCount() << "\n";
        ss << "Synapses: " << getSynapseCount() << "\n";
        ss << "Active Neurons: " << getActiveNeuronCount() << "\n";
        ss << "Average Firing Rate: " << getAverageFiringRate() << " Hz\n";
        ss << "Excitatory/Inhibitory Ratio: " << getExcitementInhibitionRatio();
        return ss.str();
    }
    
    std::vector<std::shared_ptr<NeuronInterface>> getAllNeurons() const override {
        std::vector<std::shared_ptr<NeuronInterface>> result;
        for (const auto& neuron : neurons_) {
            result.push_back(neuron);
        }
        return result;
    }
    
    std::vector<std::shared_ptr<SynapseInterface>> getAllSynapses() const override {
        std::vector<std::shared_ptr<SynapseInterface>> result;
        for (const auto& synapse : synapses_) {
            result.push_back(synapse);
        }
        return result;
    }
    
    void saveState(const std::string& filename) override {
        // TODO: Implement state saving
    }
    
    void loadState(const std::string& filename) override {
        // TODO: Implement state loading
    }
    
    void advanceDevelopment(double timestep) override {
        // TODO: Implement development
    }
    
    void applyReward(double reward, double predicted) override {
        // TODO: Implement reward modulation
    }
    
    std::string getDevelopmentalStage() const override {
        return "Phase2";
    }
    
    void receiveSensoryInput(const std::string& sensorType, const std::vector<double>& data) override {
        // TODO: Implement sensory input processing
    }
    
    std::vector<double> produceMotorOutput() const override {
        // TODO: Implement motor output
        return std::vector<double>();
    }
    
private:
    std::vector<std::shared_ptr<NeuronInterface>> neurons_;
    std::vector<std::shared_ptr<SynapseInterface>> synapses_;
    std::unordered_map<std::string, std::string> config_;
    double currentTime_;
};

class DefaultConfig : public ConfigInterface {
public:
    std::string getName() const override { return "DefaultConfig"; }
    std::string getVersion() const override { return "2.0.0"; }
    
    // Implementation of ConfigInterface methods
    void set(const std::string& key, const std::string& value) override {
        config_[key] = value;
    }
    
    std::string get(const std::string& key) const override {
        auto it = config_.find(key);
        if (it != config_.end()) {
            return it->second;
        }
        return "";
    }
    
    bool has(const std::string& key) const override {
        return config_.find(key) != config_.end();
    }
    
    void remove(const std::string& key) override {
        config_.erase(key);
    }
    
    std::vector<std::string> getKeys() const override {
        std::vector<std::string> keys;
        for (const auto& pair : config_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    void clear() override {
        config_.clear();
    }
    
    bool validate() const override {
        // Basic validation
        for (const auto& pair : config_) {
            if (pair.first.empty() || pair.second.empty()) {
                return false;
            }
        }
        return true;
    }
    
    std::string getValidationErrors() const override {
        std::stringstream ss;
        for (const auto& pair : config_) {
            if (pair.first.empty() || pair.second.empty()) {
                ss << "Invalid configuration entry: key = \"\" or value = \"\"\n";
            }
        }
        return ss.str();
    }
    
    bool loadFromFile(const std::string& filename) override {
        // TODO: Implement file loading
        return false;
    }
    
    bool saveToFile(const std::string& filename) const override {
        // TODO: Implement file saving
        return false;
    }
    
    std::string exportToString() const override {
        std::stringstream ss;
        ss << "=== Default Config Export ===\n";
        for (const auto& pair : config_) {
            ss << pair.first << " = " << pair.second << "\n";
        }
        return ss.str();
    }
    
    bool importFromString(const std::string& data) override {
        std::stringstream ss(data);
        std::string line;
        bool success = true;
        
        while (std::getline(ss, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                set(key, value);
            }
        }
        
        return success;
    }
    
private:
    std::unordered_map<std::string, std::string> config_;
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return str;
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
};

// Initialize static service locator factory pointer
ComponentFactory* ServiceLocator::factory_ = nullptr;

} // namespace nlm

// Global helper functions for easy dependency injection
namespace nlm {
    // Register default components
    void registerDefaultComponents() {
        auto& injector = ServiceLocator::getInjector();n        
        // Register default brain
        injector.registerComponent("brain", []() {
            return std::make_shared<DefaultBrain>();
        });
        
        // Register default neuron
        injector.registerComponent("neuron", []() {
            return std::make_shared<DefaultNeuron>();
        });
        
        // Register default synapse
        injector.registerComponent("synapse", []() {
            return std::make_shared<DefaultSynapse>();
        });
        
        // Register default config
        injector.registerComponent("config", []() {
            return std::make_shared<DefaultConfig>();
        });
    }
    
    // Get a component by type and name
    template<typename T>
    std::shared_ptr<T> getComponent(const std::string& name) {
        return ServiceLocator::getInjector().getComponent<T>(name);
    }
    
    // Get a component by type only
    template<typename T>
    std::shared_ptr<T> getComponent() {
        auto& injector = ServiceLocator::getInjector();
        for (const auto& pair : injector.getAllComponents<T>()) {
            return pair;
        }
        return nullptr;
    }
    
    // Create a component using the factory
    std::shared_ptr<Component> createComponent(const std::string& type) {
        auto& factory = ServiceLocator::getFactory();
        return factory.createComponent(type);
    }
    
    // Initialize the system
    void initializeSystem() {
        registerDefaultComponents();
    }
    
    // Cleanup the system
    void cleanupSystem() {
        ServiceLocator::getInjector().clear();
    }
}

// Convenience macros for component registration
#define REGISTER_COMPONENT(component_name, component_class) \
    namespace { \
        struct ComponentRegistrationHelper { \
            ComponentRegistrationHelper() { \
                nlm::ServiceLocator::getInjector().registerComponent<component_class>(#component_name, []() { \
                    return std::make_shared<component_class>(); \
                }); \
            } \
        }; \
        static ComponentRegistrationHelper component_registration_helper; \
    }

// Usage example:
// REGISTER_COMPONENT(default_brain, DefaultBrain);
// REGISTER_COMPONENT(default_neuron, DefaultNeuron);
// REGISTER_COMPONENT(default_synapse, DefaultSynapse);
// REGISTER_COMPONENT(default_config, DefaultConfig);