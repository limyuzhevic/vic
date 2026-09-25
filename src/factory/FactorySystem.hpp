// Factory System for NLM Component Creation
// Provides factory patterns for creating neural network components

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <stdexcept>
#include <mutex>

namespace nlm {
namespace factory {

// Forward declarations
class IComponentFactory;
class ComponentFactoryRegistry;

// Component metadata for factory registration
struct ComponentMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string category;
    std::string author;
    std::vector<std::string> tags;
    bool isAbstract = false;
    std::unordered_map<std::string, std::string> properties;
};

// Product base class for all created components
class IComponent {
public:
    virtual ~IComponent() = default;
    virtual ComponentMetadata getMetadata() const = 0;
    virtual std::string getId() const = 0;
    virtual bool initialize(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual bool shutdown() = 0;
    virtual void update(double timestep) {}
    virtual std::any getProperty(const std::string& name) const { return std::any(); }
    virtual void setProperty(const std::string& name, const std::any& value) {}
};

// Abstract factory interface
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    virtual std::string getFactoryId() const = 0;
    virtual ComponentMetadata getMetadata() const = 0;
    virtual std::vector<std::string> getSupportedProductTypes() const = 0;
    virtual std::unique_ptr<IComponent> create(const std::string& productType, 
                                               const std::unordered_map<std::string, std::any>& config) = 0;
    virtual bool canCreate(const std::string& productType) const = 0;
    virtual std::vector<ComponentMetadata> getAvailableProducts() const = 0;
};

// Typed component factory template
template<typename T, typename... Args>
class TypedComponentFactory : public IComponentFactory {
public:
    using ComponentCreator = std::function<std::unique_ptr<T>(Args...)>;
    
    TypedComponentFactory(const std::string& factoryId,
                         const ComponentMetadata& metadata,
                         const std::vector<std::string>& supportedTypes,
                         ComponentCreator creator)
        : factoryId_(factoryId)
        , metadata_(metadata)
        , supportedTypes_(supportedTypes)
        , creator_(std::move(creator)) {}
    
    std::string getFactoryId() const override {
        return factoryId_;
    }
    
    ComponentMetadata getMetadata() const override {
        return metadata_;
    }
    
    std::vector<std::string> getSupportedProductTypes() const override {
        return supportedTypes_;
    }
    
    std::unique_ptr<IComponent> create(const std::string& productType,
                                       const std::unordered_map<std::string, std::any>& config) override {
        if (!canCreate(productType)) {
            throw std::runtime_error("Unsupported product type: " + productType);
        }
        
        // Convert config to specific types
        return creator_(convertConfig<T>(config));
    }
    
    bool canCreate(const std::string& productType) const override {
        return std::find(supportedTypes_.begin(), supportedTypes_.end(), productType) != supportedTypes_.end();
    }
    
    std::vector<ComponentMetadata> getAvailableProducts() const override {
        // For typed factories, return a list of possible product configurations
        // This can be overridden by derived classes
        return {};
    }
    
protected:
    virtual Args... convertConfig(const std::unordered_map<std::string, std::any>& config) {
        // Default implementation - can be overridden
        return Args{}...;
    }
    
    std::string factoryId_;
    ComponentMetadata metadata_;
    std::vector<std::string> supportedTypes_;
    ComponentCreator creator_;
};

// Factory registry for managing component factories
class ComponentFactoryRegistry {
public:
    ComponentFactoryRegistry();
    ~ComponentFactoryRegistry();
    
    // Factory registration
    bool registerFactory(std::unique_ptr<IComponentFactory> factory);
    bool unregisterFactory(const std::string& factoryId);
    
    // Factory queries
    std::unique_ptr<IComponentFactory> getFactory(const std::string& factoryId) const;
    std::vector<std::unique_ptr<IComponentFactory>> getFactories() const;
    std::vector<std::unique_ptr<IComponentFactory>> getFactoriesByProductType(const std::string& productType) const;
    
    // Factory lifecycle
    bool startAllFactories();
    bool stopAllFactories();
    bool shutdownAllFactories();
    
    // Component creation
    std::unique_ptr<IComponent> createComponent(const std::string& factoryId,
                                                 const std::string& productType,
                                                 const std::unordered_map<std::string, std::any>& config);
    
    // Discovery and metadata
    std::vector<ComponentMetadata> getAllComponents() const;
    std::vector<ComponentMetadata> getComponentsByCategory(const std::string& category) const;
    std::vector<ComponentMetadata> getComponentsByTag(const std::string& tag) const;
    
    // Statistics
    size_t getFactoryCount() const;
    size_t getComponentCount() const;
    
    // Configuration management
    void setGlobalConfiguration(const std::unordered_map<std::string, std::any>& config);
    std::unordered_map<std::string, std::any> getGlobalConfiguration() const;
    
    // Utility functions
    static ComponentFactoryRegistry& getInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    ComponentFactoryRegistry(const ComponentFactoryRegistry&) = delete;
    ComponentFactoryRegistry& operator=(const ComponentFactoryRegistry&) = delete;
};

// Specialized factories for NLM components

// Neuron factory for creating different neuron models
class NeuronFactory : public TypedComponentFactory<class Neuron, const ConfigValue&> {
public:
    NeuronFactory();
    
    std::vector<ComponentMetadata> getAvailableProducts() const override;
    std::vector<std::string> getSupportedProductTypes() const override;
    
protected:
    Args... convertConfig(const std::unordered_map<std::string, std::any>& config) override;
};

// Synapse factory for creating different synapse types
class SynapseFactory : public TypedComponentFactory<class Synapse, SynapseId, NeuronId, NeuronId> {
public:
    SynapseFactory();
    
    std::vector<ComponentMetadata> getAvailableProducts() const override;
    std::vector<std::string> getSupportedProductTypes() const override;
    
protected:
    Args... convertConfig(const std::unordered_map<std::string, std::any>& config) override;
};

// Plasticity rule factory
class PlasticityRuleFactory : public TypedComponentFactory<class PlasticityRule> {
public:
    PlasticityRuleFactory();
    
    std::vector<ComponentMetadata> getAvailableProducts() const override;
    std::vector<std::string> getSupportedProductTypes() const override;
    
protected:
    Args... convertConfig(const std::unordered_map<std::string, std::any>& config) override;
};

// Memory system factory
class MemorySystemFactory : public TypedComponentFactory<class IMemorySystem> {
public:
    MemorySystemFactory();
    
    std::vector<ComponentMetadata> getAvailableProducts() const override;
    std::vector<std::string> getSupportedProductTypes() const override;
    
protected:
    Args... convertConfig(const std::unordered_map<std::string, std::any>& config) override;
};

// Factory builder for easy factory creation
class FactoryBuilder {
public:
    FactoryBuilder& name(const std::string& name);
    FactoryBuilder& version(const std::string& version);
    FactoryBuilder& description(const std::string& description);
    FactoryBuilder& category(const std::string& category);
    FactoryBuilder& author(const std::string& author);
    FactoryBuilder& addTag(const std::string& tag);
    FactoryBuilder& addProperty(const std::string& key, const std::string& value);
    FactoryBuilder& addProductType(const std::string& productType);
    
    template<typename T, typename... Args>
    std::unique_ptr<IComponentFactory> buildFactory(
        std::function<std::unique_ptr<T>(Args...)> creator) {
        // Implementation would depend on specific factory type
        throw std::runtime_error("FactoryBuilder not fully implemented for this use case");
    }
    
    std::unique_ptr<IComponentFactory> build() {
        // Create and register the factory
        throw std::runtime_error("FactoryBuilder::build() not implemented");
    }
    
private:
    ComponentMetadata metadata_;
};

// Factory utilities
namespace utils {
    // Factory validation
    bool validateFactoryConfiguration(const IComponentFactory& factory,
                                      const std::unordered_map<std::string, std::any>& config);
    
    // Factory discovery
    std::vector<std::string> discoverAvailableFactories(const std::string& directory);
    
    // Factory dependency management
    bool checkFactoryDependencies(const IComponentFactory& factory,
                                  const std::vector<std::string>& loadedFactories);
    
    // Thread-safe factory operations
    template<typename Func>
    auto threadSafeFactoryOperation(Func&& func) -> decltype(func()) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        return func();
    }
    
    // Factory metadata utilities
    std::string generateFactoryId(const std::string& prefix, const ComponentMetadata& metadata);
    std::string generateProductId(const std::string& factoryId, const std::string& productType);
}

} // namespace factory
} // namespace nlm

// Factory registration macros
#define REGISTER_COMPONENT_FACTORY(FactoryClass, FactoryId) \
    class FactoryClass##_registrar { \
    public: \
        FactoryClass##_registrar() { \
            static FactoryClass factory; \
            nlm::factory::ComponentFactoryRegistry::getInstance().registerFactory(std::make_unique<FactoryClass>(factory)); \
        } \
    }; \
    static FactoryClass##_registrar FactoryClass##_instance;

// Factory product type macros
#define SUPPORTS_PRODUCT_TYPE(FactoryClass, ProductType) \
    FactoryClass##_instance.addProductType(ProductType)

// Factory configuration macros
#define FACTORY_PROPERTY(FactoryClass, Key, Value) \
    FactoryClass##_instance.addProperty(Key, Value)

namespace nlm {
namespace factory {
    // Global factory instances
    extern ComponentFactoryRegistry& getComponentFactoryRegistry();
    
    // Convenience functions for creating components
    template<typename ProductType>
    std::unique_ptr<ProductType> createComponent(const std::string& factoryId,
                                                 const std::unordered_map<std::string, std::any>& config = {}) {
        auto& registry = getComponentFactoryRegistry();
        return std::unique_ptr<ProductType>(static_cast<ProductType*>(
            registry.createComponent(factoryId, typeid(ProductType).name(), config).release()));
    }
    
    // Component creation by category
    template<typename ProductType>
    std::vector<std::unique_ptr<ProductType>> createComponents(
        const std::vector<std::string>& factoryIds,
        const std::unordered_map<std::string, std::any>& config = {}) {
        std::vector<std::unique_ptr<ProductType>> components;
        for (const auto& factoryId : factoryIds) {
            auto component = createComponent<ProductType>(factoryId, config);
            if (component) {
                components.push_back(std::move(component));
            }
        }
        return components;
    }
}
} // namespace nlm

// Factory registry implementation
namespace nlm {
namespace factory {

struct ComponentFactoryRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<IComponentFactory>> factories;
    std::unordered_map<std::string, std::any> globalConfiguration;
    bool initialized = false;
    std::mutex mutex;
    std::mutex configMutex;
};

ComponentFactoryRegistry::ComponentFactoryRegistry() : pImpl(std::make_unique<Impl>()) {}

ComponentFactoryRegistry::~ComponentFactoryRegistry() {
    shutdownAllFactories();
}

bool ComponentFactoryRegistry::registerFactory(std::unique_ptr<IComponentFactory> factory) {
    if (!factory) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string factoryId = factory->getFactoryId();
    if (factoryId.empty()) {
        return false;
    }
    
    // Check if factory with same ID already exists
    if (pImpl->factories.find(factoryId) != pImpl->factories.end()) {
        return false;  // Factory already registered
    }
    
    pImpl->factories[factoryId] = std::move(factory);
    return true;
}

bool ComponentFactoryRegistry::unregisterFactory(const std::string& factoryId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryId);
    if (it == pImpl->factories.end()) {
        return false;
    }
    
    // Stop factory if running
    if (it->second->getMetadata().isAbstract) {
        // Abstract factories typically don't have lifecycle management
    }
    
    pImpl->factories.erase(it);
    return true;
}

std::unique_ptr<IComponentFactory> ComponentFactoryRegistry::getFactory(const std::string& factoryId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryId);
    return it != pImpl->factories.end() ? std::make_unique<IComponentFactory>(*it->second) : nullptr;
}

std::vector<std::unique_ptr<IComponentFactory>> ComponentFactoryRegistry::getFactories() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::unique_ptr<IComponentFactory>> factories;
    factories.reserve(pImpl->factories.size());
    
    for (const auto& pair : pImpl->factories) {
        factories.push_back(std::make_unique<IComponentFactory>(*pair.second));
    }
    
    return factories;
}

std::vector<std::unique_ptr<IComponentFactory>> ComponentFactoryRegistry::getFactoriesByProductType(const std::string& productType) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::unique_ptr<IComponentFactory>> factories;
    
    for (const auto& pair : pImpl->factories) {
        if (pair.second->canCreate(productType)) {
            factories.push_back(std::make_unique<IComponentFactory>(*pair.second));
        }
    }
    
    return factories;
}

bool ComponentFactoryRegistry::startAllFactories() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->factories) {
        // Abstract factories typically don't have start/stop lifecycle
        // This can be overridden by derived classes
    }
    
    return true;
}

bool ComponentFactoryRegistry::stopAllFactories() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : pImpl->factories) {
        // Abstract factories typically don't have start/stop lifecycle
        // This can be overridden by derived classes
    }
    
    return true;
}

bool ComponentFactoryRegistry::shutdownAllFactories() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->factories.clear();
    return true;
}

std::unique_ptr<IComponent> ComponentFactoryRegistry::createComponent(
    const std::string& factoryId,
    const std::string& productType,
    const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(factoryId);
    if (it == pImpl->factories.end()) {
        throw std::runtime_error("Factory not found: " + factoryId);
    }
    
    // Merge with global configuration
    std::unordered_map<std::string, std::any> mergedConfig = pImpl->globalConfiguration;
    mergedConfig.insert(config.begin(), config.end());
    
    return it->second->create(productType, mergedConfig);
}

std::vector<ComponentMetadata> ComponentFactoryRegistry::getAllComponents() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ComponentMetadata> components;
    
    for (const auto& pair : pImpl->factories) {
        auto factory = pair.second;
        auto products = factory->getAvailableProducts();
        components.insert(components.end(), products.begin(), products.end());
    }
    
    return components;
}

std::vector<ComponentMetadata> ComponentFactoryRegistry::getComponentsByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ComponentMetadata> components;
    
    for (const auto& pair : pImpl->factories) {
        auto factory = pair.second;
        ComponentMetadata factoryMetadata = factory->getMetadata();
        
        if (factoryMetadata.category == category) {
            auto products = factory->getAvailableProducts();
            components.insert(components.end(), products.begin(), products.end());
        }
    }
    
    return components;
}

std::vector<ComponentMetadata> ComponentFactoryRegistry::getComponentsByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ComponentMetadata> components;
    
    for (const auto& pair : pImpl->factories) {
        auto factory = pair.second;
        ComponentMetadata factoryMetadata = factory->getMetadata();
        
        if (std::find(factoryMetadata.tags.begin(), factoryMetadata.tags.end(), tag) != factoryMetadata.tags.end()) {
            auto products = factory->getAvailableProducts();
            components.insert(components.end(), products.begin(), products.end());
        }
    }
    
    return components;
}

size_t ComponentFactoryRegistry::getFactoryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->factories.size();
}

size_t ComponentFactoryRegistry::getComponentCount() const {
    return getAllComponents().size();
}

void ComponentFactoryRegistry::setGlobalConfiguration(const std::unordered_map<std::string, std::any>& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->globalConfiguration = config;
}

std::unordered_map<std::string, std::any> ComponentFactoryRegistry::getGlobalConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->globalConfiguration;
}

ComponentFactoryRegistry& ComponentFactoryRegistry::getInstance() {
    static ComponentFactoryRegistry instance;
    return instance;
}

} // namespace factory
} // namespace nlm

// Factory registration implementation for specific NLM component types
namespace nlm {
namespace factory {

NeuronFactory::NeuronFactory()
    : TypedComponentFactory<class Neuron, const ConfigValue&>(
        "neuron_factory",
        {
            "Neuron Factory",
            "1.0.0",
            "Factory for creating neural network neurons",
            "Neural Components",
            "NLM Research Team",
            {"neuron", "neural", "brain"},
            false,
            {}
        },
        {"LIFNeuron", "IzhikevichNeuron", "HHNeuron", "GenericNeuron"},
        [](const ConfigValue& config) {
            // Default neuron creation - could be customized
            return std::make_unique<class Neuron>(NeuronId(0));
        }
    ) {}

std::vector<ComponentMetadata> NeuronFactory::getAvailableProducts() const {
    return {
        {"LIFNeuron", "1.0.0", "Leaky Integrate-and-Fire neuron model", "Neural Components", "NLM Research Team", {"lif", "neuron", "simple"}, false, {}},
        {"IzhikevichNeuron", "1.0.0", "Biophysically detailed Izhikevich model", "Neural Components", "NLM Research Team", {"izhikevich", "detailed", "neuron"}, false, {}},
        {"GenericNeuron", "1.0.0", "Generic neuron with configurable properties", "Neural Components", "NLM Research Team", {"generic", "configurable", "neuron"}, false, {}}
    };
}

std::vector<std::string> NeuronFactory::getSupportedProductTypes() const {
    return {"LIFNeuron", "IzhikevichNeuron", "HHNeuron", "GenericNeuron"};
}

// SynapseFactory implementation
SynapseFactory::SynapseFactory()
    : TypedComponentFactory<class Synapse, SynapseId, NeuronId, NeuronId>(
        "synapse_factory",
        {
            "Synapse Factory",
            "1.0.0",
            "Factory for creating synaptic connections",
            "Neural Components",
            "NLM Research Team",
            {"synapse", "connection", "plasticity"},
            false,
            {}
        },
        {"ExcitatorySynapse", "InhibitorySynapse", "PlasticSynapse"},
        [](SynapseId id, NeuronId source, NeuronId destination) {
            return std::make_unique<class Synapse>(id, source, destination);
        }
    ) {}

std::vector<ComponentMetadata> SynapseFactory::getAvailableProducts() const {
    return {
        {"ExcitatorySynapse", "1.0.0", "Excitatory synaptic connection", "Neural Components", "NLM Research Team", {"excitatory", "synapse", "positive"}, false, {}},
        {"InhibitorySynapse", "1.0.0", "Inhibitory synaptic connection", "Neural Components", "NLM Research Team", {"inhibitory", "synapse", "negative"}, false, {}},
        {"PlasticSynapse", "1.0.0", "Synaptic connection with plasticity", "Neural Components", "NLM Research Team", {"plastic", "synapse", "learning"}, false, {}}
    };
}

std::vector<std::string> SynapseFactory::getSupportedProductTypes() const {
    return {"ExcitatorySynapse", "InhibitorySynapse", "PlasticSynapse"};
}

// PlasticityRuleFactory implementation
PlasticityRuleFactory::PlasticityRuleFactory()
    : TypedComponentFactory<class PlasticityRule>(
        "plasticity_rule_factory",
        {
            "Plasticity Rule Factory",
            "1.0.0",
            "Factory for creating synaptic plasticity rules",
            "Neural Components",
            "NLM Research Team",
            {"plasticity", "learning", "adaptation"},
            false,
            {}
        },
        {"STDPLaw", "HebbianRule", "RewardModulatedRule", "CustomRule"},
        []() {
            return std::make_unique<class PlasticityRule>();
        }
    ) {}

std::vector<ComponentMetadata> PlasticityRuleFactory::getAvailableProducts() const {
    return {
        {"STDPLaw", "1.0.0", "Spike-Timing-Dependent Plasticity rule", "Neural Components", "NLM Research Team", {"stdp", "timing", "plasticity"}, false, {}},
        {"HebbianRule", "1.0.0", "Hebbian synaptic strengthening rule", "Neural Components", "NLM Research Team", {"hebbian", "associative", "plasticity"}, false, {}},
        {"RewardModulatedRule", "1.0.0", "Reward-modulated plasticity rule", "Neural Components", "NLM Research Team", {"reward", "dopamine", "plasticity"}, false, {}},
        {"CustomRule", "1.0.0", "User-defined plasticity rule template", "Neural Components", "NLM Research Team", {"custom", "user", "plasticity"}, false, {}}
    };
}

std::vector<std::string> PlasticityRuleFactory::getSupportedProductTypes() const {
    return {"STDPLaw", "HebbianRule", "RewardModulatedRule", "CustomRule"};
}

// MemorySystemFactory implementation
MemorySystemFactory::MemorySystemFactory()
    : TypedComponentFactory<class IMemorySystem>(
        "memory_system_factory",
        {
            "Memory System Factory",
            "1.0.0",
            "Factory for creating memory systems",
            "Neural Components",
            "NLM Research Team",
            {"memory", "cognitive", "storage"},
            false,
            {}
        },
        {"WorkingMemory", "EpisodicMemory", "SemanticMemory", "ProceduralMemory"},
        []() {
            return std::make_unique<class WorkingMemory>();
        }
    ) {}

std::vector<ComponentMetadata> MemorySystemFactory::getAvailableProducts() const {
    return {
        {"WorkingMemory", "1.0.0", "Short-term working memory system", "Neural Components", "NLM Research Team", {"working", "short-term", "memory"}, false, {}},
        {"EpisodicMemory", "1.0.0", "Long-term episodic memory system", "Neural Components", "NLM Research Team", {"episodic", "long-term", "memory"}, false, {}},
        {"SemanticMemory", "1.0.0", "Semantic knowledge memory system", "Neural Components", "NLM Research Team", {"semantic", "knowledge", "memory"}, false, {}},
        {"ProceduralMemory", "1.0.0", "Skill and procedure memory system", "Neural Components", "NLM Research Team", {"procedural", "skills", "memory"}, false, {}}
    };
}

std::vector<std::string> MemorySystemFactory::getSupportedProductTypes() const {
    return {"WorkingMemory", "EpisodicMemory", "SemanticMemory", "ProceduralMemory"};
}

} // namespace factory
} // namespace nlm

// Global factory instances
namespace nlm {
namespace factory {
    ComponentFactoryRegistry& getComponentFactoryRegistry() {
        return ComponentFactoryRegistry::getInstance();
    }
}
} // namespace nlm