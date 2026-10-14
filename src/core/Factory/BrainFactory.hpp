// Factory system for creating NLM components
// Provides factory patterns for easy extension and configuration

#ifndef NLM_FACTORY_H
#define NLM_FACTORY_H

#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace nlm {

// Component type enumeration for factory registration
enum class ComponentType {
    Neuron,
    Synapse,
    NeuralRegion,
    NeuralPopulation,
    PlasticityRule,
    STDP,
    Hebbian,
    StructuralPlasticity,
    Neuromodulator,
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
    WorkingMemory,
    EpisodicMemory,
    AssociativeMemory,
    PredictionSystem,
    NeuralPlanner,
    ConceptFormation,
    AttentionalSelection,
    DevelopmentSystem,
    Synaptogenesis,
    Pruning,
    Maturation
};

// Abstract factory interface
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    virtual std::string getName() const = 0;
    virtual ComponentType getType() const = 0;
    virtual bool canCreate(ComponentType type) const = 0;
    virtual std::shared_ptr<void> create(const Config& config) = 0;
};

// Concrete factory for creating brain components
class BrainComponentFactory {
public:
    BrainComponentFactory();
    ~BrainComponentFactory();
    
    // Register a component factory
    void registerFactory(std::unique_ptr<IComponentFactory> factory);
    
    // Create a component of specified type
    template<typename T>
    std::shared_ptr<T> createComponent(ComponentType type, const Config& config) {
        auto it = std::find_if(factories_.begin(), factories_.end(),
            [type](const std::unique_ptr<IComponentFactory>& factory) {
                return factory->getType() == type;
            });
        
        if (it == factories_.end()) {
            throw std::runtime_error("No factory registered for component type: " + 
                                     std::to_string(static_cast<int>(type)));
        }
        
        auto result = (*it)->create(config);
        return std::static_pointer_cast<T>(result);
    }
    
    // Get component name
    std::string getComponentName(ComponentType type) const;
    
    // Check if component type is supported
    bool supportsComponent(ComponentType type) const;
    
    // Get all registered component types
    std::vector<ComponentType> getSupportedTypes() const;
    
    // Default factories registration
    static BrainComponentFactory& getInstance();
    
private:
    std::vector<std::unique_ptr<IComponentFactory>> factories_;
    std::map<ComponentType, std::string> typeNames_;
};

// Neuron factory class
class NeuronFactory : public IComponentFactory {
public:
    std::string getName() const override { return "NeuronFactory"; }
    ComponentType getType() const override { return ComponentType::Neuron; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::Neuron; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of neuron creation
        throw std::runtime_error("NeuronFactory::create not implemented yet");
    }
};

// Synapse factory class
class SynapseFactory : public IComponentFactory {
public:
    std::string getName() const override { return "SynapseFactory"; }
    ComponentType getType() const override { return ComponentType::Synapse; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::Synapse; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of synapse creation
        throw std::runtime_error("SynapseFactory::create not implemented yet");
    }
};

// Neural region factory class
class NeuralRegionFactory : public IComponentFactory {
public:
    std::string getName() const override { return "NeuralRegionFactory"; }
    ComponentType getType() const override { return ComponentType::NeuralRegion; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::NeuralRegion; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of neural region creation
        throw std::runtime_error("NeuralRegionFactory::create not implemented yet");
    }
};

// Neural population factory class
class NeuralPopulationFactory : public IComponentFactory {
public:
    std::string getName() const override { return "NeuralPopulationFactory"; }
    ComponentType getType() const override { return ComponentType::NeuralPopulation; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::NeuralPopulation; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of neural population creation
        throw std::runtime_error("NeuralPopulationFactory::create not implemented yet");
    }
};

// Plasticity rule factory class
class PlasticityRuleFactory : public IComponentFactory {
public:
    std::string getName() const override { return "PlasticityRuleFactory"; }
    ComponentType getType() const override { return ComponentType::PlasticityRule; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::PlasticityRule; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of plasticity rule creation
        throw std::runtime_error("PlasticityRuleFactory::create not implemented yet");
    }
};

// Specific plasticity rule factories
class STDPFactory : public IComponentFactory {
public:
    std::string getName() const override { return "STDPFactory"; }
    ComponentType getType() const override { return ComponentType::STDP; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::STDP; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of STDP creation
        throw std::runtime_error("STDPFactory::create not implemented yet");
    }
};

class HebbianFactory : public IComponentFactory {
public:
    std::string getName() const override { return "HebbianFactory"; }
    ComponentType getType() const override { return ComponentType::Hebbian; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::Hebbian; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of Hebbian creation
        throw std::runtime_error("HebbianFactory::create not implemented yet");
    }
};

class StructuralPlasticityFactory : public IComponentFactory {
public:
    std::string getName() const override { return "StructuralPlasticityFactory"; }
    ComponentType getType() const override { return ComponentType::StructuralPlasticity; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::StructuralPlasticity; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of structural plasticity creation
        throw std::runtime_error("StructuralPlasticityFactory::create not implemented yet");
    }
};

// Neuromodulator factory class
class NeuromodulatorFactory : public IComponentFactory {
public:
    std::string getName() const override { return "NeuromodulatorFactory"; }
    ComponentType getType() const override { return ComponentType::Neuromodulator; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::Neuromodulator; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of neuromodulator creation
        throw std::runtime_error("NeuromodulatorFactory::create not implemented yet");
    }
};

// Specific neuromodulator factories
class DopamineFactory : public IComponentFactory {
public:
    std::string getName() const override { return "DopamineFactory"; }
    ComponentType getType() const override { return ComponentType::Dopamine; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::Dopamine; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of dopamine creation
        throw std::runtime_error("DopamineFactory::create not implemented yet");
    }
};

class CuriosityFactory : public IComponentFactory {
public:
    std::string getName() const override { return "CuriosityFactory"; }
    ComponentType getType() const override { return ComponentType::Curiosity; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::Curiosity; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of curiosity creation
        throw std::runtime_error("CuriosityFactory::create not implemented yet");
    }
};

class NoveltyFactory : public IComponentFactory {
public:
    std::string getName() const override { return "NoveltyFactory"; }
    ComponentType getType() const override { return ComponentType::Novelty; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::Novelty; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of novelty creation
        throw std::runtime_error("NoveltyFactory::create not implemented yet");
    }
};

class PredictionErrorFactory : public IComponentFactory {
public:
    std::string getName() const override { return "PredictionErrorFactory"; }
    ComponentType getType() const override { return ComponentType::PredictionError; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::PredictionError; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of prediction error creation
        throw std::runtime_error("PredictionErrorFactory::create not implemented yet");
    }
};

// Memory system factory class
class MemoryFactory : public IComponentFactory {
public:
    std::string getName() const override { return "MemoryFactory"; }
    ComponentType getType() const override { return ComponentType::WorkingMemory; } // Default to working memory
    bool canCreate(ComponentType type) const override {
        return type == ComponentType::WorkingMemory ||
               type == ComponentType::EpisodicMemory ||
               type == ComponentType::AssociativeMemory;
    }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of memory system creation
        throw std::runtime_error("MemoryFactory::create not implemented yet");
    }
};

// Prediction system factory class
class PredictionSystemFactory : public IComponentFactory {
public:
    std::string getName() const override { return "PredictionSystemFactory"; }
    ComponentType getType() const override { return ComponentType::PredictionSystem; }
    bool canCreate(ComponentType type) const override { return type == ComponentType::PredictionSystem; }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of prediction system creation
        throw std::runtime_error("PredictionSystemFactory::create not implemented yet");
    }
};

// Cognition system factory class
class CognitionFactory : public IComponentFactory {
public:
    std::string getName() const override { return "CognitionFactory"; }
    ComponentType getType() const override { return ComponentType::NeuralPlanner; } // Default to neural planner
    bool canCreate(ComponentType type) const override {
        return type == ComponentType::NeuralPlanner ||
               type == ComponentType::ConceptFormation ||
               type == ComponentType::AttentionalSelection;
    }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of cognition system creation
        throw std::runtime_error("CognitionFactory::create not implemented yet");
    }
};

// Development system factory class
class DevelopmentFactory : public IComponentFactory {
public:
    std::string getName() const override { return "DevelopmentFactory"; }
    ComponentType getType() const override { return ComponentType::DevelopmentSystem; }
    bool canCreate(ComponentType type) const override {
        return type == ComponentType::DevelopmentSystem ||
               type == ComponentType::Synaptogenesis ||
               type == ComponentType::Pruning ||
               type == ComponentType::Maturation;
    }
    std::shared_ptr<void> create(const Config& config) override {
        // Implementation of development system creation
        throw std::runtime_error("DevelopmentFactory::create not implemented yet");
    }
};

// Configuration-based component creator
class ComponentCreator {
public:
    ComponentCreator();
    ~ComponentCreator();
    
    // Create a brain with specified configuration
    static std::shared_ptr<Brain> createBrain(const Config& config);
    
    // Create a complete integrated brain
    static std::shared_ptr<Brain> createIntegratedBrain(const Config& config);
    
    // Create a brain with specific configuration
    static std::shared_ptr<Brain> createBrain(const std::string& configName);
    
    // Create a brain with custom settings
    static std::shared_ptr<Brain> createCustomBrain(const Config& config, 
                                                    const std::vector<ComponentType>& componentTypes);
    
    // Get available component types for a brain
    static std::vector<ComponentType> getBrainComponents(const Config& config);
    
    // Check if configuration is valid
    static bool isConfigValid(const Config& config);
    
    // Get configuration defaults
    static Config getDefaultConfig(ComponentType componentType);
    
    // Register a custom brain configuration
    static void registerBrainConfig(const std::string& name, const Config& config);
    
    // Get all registered brain configurations
    static std::vector<std::string> getRegisteredBrainConfigs();
    
    // Create brain from configuration name
    static std::shared_ptr<Brain> createBrainFromName(const std::string& configName);
    
private:
    static std::map<std::string, Config> brainConfigs_;
};

// Configuration profile for different brain types
struct BrainProfile {
    std::string name;
    std::string description;
    Config config;
    std::vector<ComponentType> preferredComponents;
    float expectedPerformanceScore;
    std::string useCase;
    
    BrainProfile(const std::string& n, const std::string& d, const Config& c)
        : name(n), description(d), config(c), expectedPerformanceScore(0.0f), useCase("") {}
};

// Brain profile manager
class BrainProfileManager {
public:
    BrainProfileManager();
    ~BrainProfileManager();
    
    // Register a brain profile
    void registerProfile(const BrainProfile& profile);
    
    // Get a brain profile by name
    BrainProfile getProfile(const std::string& name) const;
    
    // Get all registered profiles
    std::vector<std::string> getProfileNames() const;
    
    // Find best profile for configuration
    BrainProfile findBestProfile(const Config& config) const;
    
    // Get profiles by use case
    std::vector<BrainProfile> getProfilesByUseCase(const std::string& useCase) const;
    
    // Get profiles by performance range
    std::vector<BrainProfile> getProfilesByPerformance(float minScore, float maxScore) const;
    
    // Delete a profile
    bool deleteProfile(const std::string& name);
    
    // Update a profile
    bool updateProfile(const std::string& name, const BrainProfile& newProfile);
    
    // Get default profiles
    static BrainProfileManager& getInstance();
    
private:
    std::map<std::string, BrainProfile> profiles_;
    std::vector<BrainProfile> defaultProfiles_;
};

// Utility functions for factory operations
namespace FactoryUtils {
    // Validate configuration for component creation
    bool validateConfigForComponent(ComponentType type, const Config& config);
    
    // Get component defaults
    Config getComponentDefaults(ComponentType type);
    
    // Merge configurations
    Config mergeConfigs(const Config& baseConfig, const Config& overrideConfig);
    
    // Create configuration from string
    Config configFromString(const std::string& configString);
    
    // Create configuration from name-value pairs
    Config configFromPairs(const std::vector<std::pair<std::string, std::string>>& pairs);
    
    // Get all available component factories
    std::vector<ComponentType> getAvailableComponentTypes();
    
    // Get component factory by type
    std::unique_ptr<IComponentFactory> getFactory(ComponentType type);
    
    // Get all factories
    std::vector<std::unique_ptr<IComponentFactory>> getAllFactories();
    
    // Initialize default factories
    void initializeDefaultFactories();
    
    // Clean up all factories
    void cleanup();
    
    // Get factory instance
    static BrainComponentFactory& getBrainComponentFactory();
    
    // Get profile manager instance
    static BrainProfileManager& getBrainProfileManager();
    
    // Get component creator instance
    static ComponentCreator& getComponentCreator();
}

} // namespace nlm

#endif // NLM_FACTORY_H
