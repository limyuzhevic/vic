#pragma once

#include "../core/Types/Types.hpp"
#include "../interfaces/IBrainInterface.hpp"
#include "../interfaces/IMemorySystem.hpp"
#include "../interfaces/INeuromodulationSystem.hpp"
#include "../interfaces/IPredictionSystem.hpp"
#include "../interfaces/ICognitionSystem.hpp"
#include "../interfaces/IDevelopmentSystem.hpp"
#include "../interfaces/IPlasticitySystem.hpp"
#include "../interfaces/IDynamicsSystem.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <map>
#include <string>
#include <typeindex>
#include <functional>
#include <vector>

namespace nlm {

// Component type identifiers
enum class ComponentType {
    Brain,
    WorkingMemory,
    EpisodicMemory,
    AssociativeMemory,
    PredictionSystem,
    NeuralPlanner,
    ConceptFormation,
    Attention,
    DevelopmentSystem,
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
    SpikeSystem,
    STDP,
    Hebbian,
    StructuralPlasticity
};

// Component configuration
struct ComponentConfig {
    std::string name;
    std::map<std::string, std::string> parameters;
    bool enabled = true;
    std::vector<std::string> dependencies;
};

// Abstract factory interface for creating components
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    virtual std::shared_ptr<IBrainInterface> createBrain(std::shared_ptr<Config> config) = 0;
    virtual std::shared_ptr<IMemorySystem> createWorkingMemory() = 0;
    virtual std::shared_ptr<IMemorySystem> createEpisodicMemory() = 0;
    virtual std::shared_ptr<IMemorySystem> createAssociativeMemory() = 0;
    virtual std::shared_ptr<IPredictionSystem> createPredictionSystem() = 0;
    virtual std::shared_ptr<ICognitionSystem> createNeuralPlanner() = 0;
    virtual std::shared_ptr<ICognitionSystem> createConceptFormation() = 0;
    virtual std::shared_ptr<ICognitionSystem> createAttention() = 0;
    virtual std::shared_ptr<IDevelopmentSystem> createDevelopmentSystem() = 0;
    virtual std::shared_ptr<INeuromodulationSystem> createDopamine() = 0;
    virtual std::shared_ptr<INeuromodulationSystem> createCuriosity() = 0;
    virtual std::shared_ptr<INeuromodulationSystem> createNovelty() = 0;
    virtual std::shared_ptr<INeuromodulationSystem> createPredictionError() = 0;
    virtual std::shared_ptr<IDynamicsSystem> createSpikeSystem() = 0;
    virtual std::shared_ptr<IDynamicsSystem> createSTDP() = 0;
    virtual std::shared_ptr<IDynamicsSystem> createHebbian() = 0;
    virtual std::shared_ptr<IPlasticitySystem> createStructuralPlasticity() = 0;

    virtual std::shared_ptr<IBrainInterface> createBrainAdapter(std::shared_ptr<Config> config) = 0;
};

// Default component factory implementation
class DefaultComponentFactory : public IComponentFactory {
public:
    DefaultComponentFactory() = default;
    
    std::shared_ptr<IBrainInterface> createBrain(std::shared_ptr<Config> config) override {
        return std::make_shared<BrainAdapter>(config);
    }
    
    std::shared_ptr<IMemorySystem> createWorkingMemory() override {
        return std::make_shared<WorkingMemoryImpl>(100);
    }
    
    std::shared_ptr<IMemorySystem> createEpisodicMemory() override {
        return std::make_shared<EpisodicMemoryImpl>(1000);
    }
    
    std::shared_ptr<IMemorySystem> createAssociativeMemory() override {
        return std::make_shared<AssociativeMemoryImpl>();
    }
    
    std::shared_ptr<IPredictionSystem> createPredictionSystem() override {
        return std::make_shared<PredictionSystemImpl>();
    }
    
    std::shared_ptr<ICognitionSystem> createNeuralPlanner() override {
        return std::make_shared<NeuralPlannerImpl>();
    }
    
    std::shared_ptr<ICognitionSystem> createConceptFormation() override {
        return std::make_shared<ConceptFormationImpl>();
    }
    
    std::shared_ptr<ICognitionSystem> createAttention() override {
        return std::make_shared<AttentionalSelectionImpl>();
    }
    
    std::shared_ptr<IDevelopmentSystem> createDevelopmentSystem() override {
        return std::make_shared<DevelopmentSystemImpl>();
    }
    
    std::shared_ptr<INeuromodulationSystem> createDopamine() override {
        return std::make_shared<DopamineImpl>();
    }
    
    std::shared_ptr<INeuromodulationSystem> createCuriosity() override {
        return std::make_shared<CuriosityImpl>();
    }
    
    std::shared_ptr<INeuromodulationSystem> createNovelty() override {
        return std::make_shared<NoveltyImpl>();
    }
    
    std::shared_ptr<INeuromodulationSystem> createPredictionError() override {
        return std::make_shared<PredictionErrorImpl>();
    }
    
    std::shared_ptr<IDynamicsSystem> createSpikeSystem() override {
        return std::make_shared<SpikeSystemImpl>();
    }
    
    std::shared_ptr<IDynamicsSystem> createSTDP() override {
        return std::make_shared<STDPImpl>();
    }
    
    std::shared_ptr<IDynamicsSystem> createHebbian() override {
        return std::make_shared<HebbianImpl>();
    }
    
    std::shared_ptr<IPlasticitySystem> createStructuralPlasticity() override {
        return std::make_shared<StructuralPlasticityImpl>();
    }
    
    std::shared_ptr<IBrainInterface> createBrainAdapter(std::shared_ptr<Config> config) override {
        return std::make_shared<BrainAdapter>(config);
    }
};

// Dependency injection container
class DependencyInjector {
public:
    DependencyInjector() = default;
    ~DependencyInjector() = default;
    
    // Set the component factory
    void setFactory(std::shared_ptr<IComponentFactory> factory) {
        factory_ = factory;
    }
    
    // Get a component
    template<typename T>
    std::shared_ptr<T> getComponent() {
        auto it = components_.find(typeid(T));
        if (it != components_.end()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        
        // Create component if factory exists
        if (factory_) {
            std::shared_ptr<T> component = createComponent<T>();
            if (component) {
                components_[typeid(T)] = component;
            }
            return component;
        }
        
        return nullptr;
    }
    
    // Register a component instance
    template<typename T>
    void registerComponent(std::shared_ptr<T> component) {
        components_[typeid(T)] = component;
    }
    
    // Clear all components
    void clear() {
        components_.clear();
    }
    
    // Get brain interface (specifically IBrainInterface)
    std::shared_ptr<IBrainInterface> getBrain() {
        return std::dynamic_pointer_cast<IBrainInterface>(getComponent<IBrainInterface>());
    }
    
    // Get specific component types
    std::shared_ptr<IMemorySystem> getWorkingMemory() {
        return std::dynamic_pointer_cast<IMemorySystem>(getComponent<IWorkingMemorySystem>());
    }
    
    std::shared_ptr<IMemorySystem> getEpisodicMemory() {
        return std::dynamic_pointer_cast<IMemorySystem>(getComponent<IEpisodicMemorySystem>());
    }
    
    std::shared_ptr<IMemorySystem> getAssociativeMemory() {
        return std::dynamic_pointer_cast<IMemorySystem>(getComponent<IAssociativeMemorySystem>());
    }
    
    // Get all system components
    std::vector<std::shared_ptr<IMemorySystem>> getAllMemorySystems() {
        std::vector<std::shared_ptr<IMemorySystem>> result;
        
        auto workingMem = getWorkingMemory();
        if (workingMem) result.push_back(workingMem);
        
        auto episodicMem = getEpisodicMemory();
        if (episodicMem) result.push_back(episodicMem);
        
        auto associativeMem = getAssociativeMemory();
        if (associativeMem) result.push_back(associativeMem);
        
        return result;
    }
    
    // Check if injector is configured
    bool isInitialized() const {
        return factory_ != nullptr;
    }

private:
    // Create component using factory
    template<typename T>
    std::shared_ptr<T> createComponent() {
        if (!factory_) return nullptr;
        
        // Map type to factory method
        if (std::is_same<T, IBrainInterface>::value) {
            return factory_->createBrain(nullptr);
        } else if (std::is_same<T, IWorkingMemorySystem>::value) {
            return factory_->createWorkingMemory();
        } else if (std::is_same<T, IEpisodicMemorySystem>::value) {
            return factory_->createEpisodicMemory();
        } else if (std::is_same<T, IAssociativeMemorySystem>::value) {
            return factory_->createAssociativeMemory();
        } else if (std::is_same<T, IPredictionSystem>::value) {
            return factory_->createPredictionSystem();
        } else if (std::is_same<T, ICognitionSystem>::value) {
            return factory_->createNeuralPlanner();  // Default to planner
        }
        
        return nullptr;
    }
    
    std::shared_ptr<IComponentFactory> factory_;
    std::map<std::type_index, std::shared_ptr<void>> components_;
};

// Component registry for configuration-based component selection
class ComponentRegistry {
public:
    ComponentRegistry() = default;
    
    // Register a component implementation for a type
    template<typename Interface, typename Implementation>
    void registerComponent() {
        componentRegistry_[std::type_index(typeid(Interface))] = 
            []() -> std::shared_ptr<void> {
                return std::make_shared<Implementation>();
            };
    }
    
    // Get component implementation
    template<typename Interface>
    std::shared_ptr<Interface> getComponent() {
        auto it = componentRegistry_.find(std::type_index(typeid(Interface)));
        if (it != componentRegistry_.end()) {
            auto instance = it->second();
            return std::dynamic_pointer_cast<Interface>(instance);
        }
        return nullptr;
    }
    
    // Clear registry
    void clear() {
        componentRegistry_.clear();
    }
    
    // Get all registered component types
    std::vector<std::type_index> getRegisteredTypes() const {
        std::vector<std::type_index> types;
        for (const auto& pair : componentRegistry_) {
            types.push_back(pair.first);
        }
        return types;
    }

private:
    std::map<std::type_index, std::function<std::shared_ptr<void>()>> componentRegistry_;
};

// Global dependency injector instance
inline DependencyInjector& getInjector() {
    static DependencyInjector injector;
    return injector;
}

// Global component registry instance  
inline ComponentRegistry& getComponentRegistry() {
    static ComponentRegistry registry;
    return registry;
}

// Convenience functions for dependency injection
inline void initializeInjector(std::shared_ptr<IComponentFactory> factory) {
    getInjector().setFactory(factory);
}

inline std::shared_ptr<IBrainInterface> getBrain() {
    return getInjector().getBrain();
}

inline std::shared_ptr<IMemorySystem> getWorkingMemory() {
    return getInjector().getWorkingMemory();
}

inline std::shared_ptr<IMemorySystem> getEpisodicMemory() {
    return getInjector().getEpisodicMemory();
}

inline std::shared_ptr<IMemorySystem> getAssociativeMemory() {
    return getInjector().getAssociativeMemory();
}

} // namespace nlm
