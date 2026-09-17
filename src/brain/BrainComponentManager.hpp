// Brain Component Manager - Manages all NLM subsystems
// Provides a cleaner interface to brain components than direct pointer access

#pragma once

#include "../brain/Brain.hpp"
#include <string>
#include <typeindex>
#include <unordered_map>

namespace nlm {

// Component type enumeration for type-safe component management
enum class ComponentType {
    RandomGenerator,
    SpikeSystem,
    STDP,
    Hebbian,
    StructuralPlasticity,
    NeuralWorkingMemory,
    NeuralEpisodicMemory,
    NeuralAssociativeMemory,
    PredictionSystem,
    NeuralPlanner,
    ConceptFormation,
    AttentionalSelection,
    DevelopmentSystem,
    Dopamine,
    Curiosity,
    PredictionError,
    Novelty,
    Count
};

// BrainComponentManager - Provides clean component access and lifecycle management
class BrainComponentManager {
public:
    BrainComponentManager() = default;
    ~BrainComponentManager() = default;
    
    // Register a brain with the manager
    void registerBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    // Get a component by type using template specialization
    template<typename Component>
    Component* getComponent() {
        if (!brain_) return nullptr;
        
        auto type = getComponentType<Component>();
        auto it = components_.find(type);
        
        if (it != components_.end()) {
            return dynamic_cast<Component*>(it->second.get());
        }
        
        // Try to get from brain and cache it
        Component* component = retrieveComponent<Component>();
        if (component) {
            components_[type] = std::unique_ptr<void>(component);
        }
        
        return component;
    }
    
    // Get a component by type (const version)
    template<typename Component>
    const Component* getComponent() const {
        if (!brain_) return nullptr;
        
        auto type = getComponentType<Component>();
        auto it = components_.find(type);
        
        if (it != components_.end()) {
            return dynamic_cast<const Component*>(it->second.get());
        }
        
        // Try to get from brain and cache it
        const Component* component = retrieveComponent<Component>();
        if (component) {
            components_[type] = std::unique_ptr<void>(const_cast<void*>(static_cast<const void*>(component)));
        }
        
        return component;
    }
    
    // Check if component exists
    template<typename Component>
    bool hasComponent() const {
        return getComponent<Component>() != nullptr;
    }
    
    // Get component type name for logging/debugging
    static std::string getComponentTypeName(ComponentType type) {
        switch (type) {
            case ComponentType::RandomGenerator: return "RandomGenerator";
            case ComponentType::SpikeSystem: return "SpikeSystem";
            case ComponentType::STDP: return "STDP";
            case ComponentType::Hebbian: return "Hebbian";
            case ComponentType::StructuralPlasticity: return "StructuralPlasticity";
            case ComponentType::NeuralWorkingMemory: return "NeuralWorkingMemory";
            case ComponentType::NeuralEpisodicMemory: return "NeuralEpisodicMemory";
            case ComponentType::NeuralAssociativeMemory: return "NeuralAssociativeMemory";
            case ComponentType::PredictionSystem: return "PredictionSystem";
            case ComponentType::NeuralPlanner: return "NeuralPlanner";
            case ComponentType::ConceptFormation: return "ConceptFormation";
            case ComponentType::AttentionalSelection: return "AttentionalSelection";
            case ComponentType::DevelopmentSystem: return "DevelopmentSystem";
            case ComponentType::Dopamine: return "Dopamine";
            case ComponentType::Curiosity: return "Curiosity";
            case ComponentType::PredictionError: return "PredictionError";
            case ComponentType::Novelty: return "Novelty";
            case ComponentType::Count: return "Count";
            default: return "Unknown";
        }
    }
    
    // Get all registered components
    std::vector<ComponentType> getRegisteredComponents() const {
        std::vector<ComponentType> types;
        types.reserve(components_.size());
        for (const auto& pair : components_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    // Clear cached components (call when brain resets)
    void clearCache() {
        components_.clear();
    }
    
    // Get brain for component initialization
    Brain* getBrain() {
        return brain_.get();
    }
    
    // Get brain for component initialization (const)
    const Brain* getBrain() const {
        return brain_.get();
    }
    
private:
    // Template function to retrieve specific component from brain
    template<typename Component>
    Component* retrieveComponent() {
        if (!brain_) return nullptr;
        
        // Specialization for different component types
        return retrieveFromBrain<Component>();
    }
    
    template<typename Component>
    const Component* retrieveComponent() const {
        if (!brain_) return nullptr;
        
        // Specialization for different component types
        return retrieveFromBrain<Component>();
    }
    
    // Component type getter using template specialization
    template<typename Component>
    ComponentType getComponentType() const {
        return ComponentType::Count; // Default, will be specialized
    }
    
    // Template specializations for each component type
    ComponentType getComponentType<RandomGenerator>() const { return ComponentType::RandomGenerator; }
    ComponentType getComponentType<SpikeSystem>() const { return ComponentType::SpikeSystem; }
    ComponentType getComponentType<STDP>() const { return ComponentType::STDP; }
    ComponentType getComponentType<Hebbian>() const { return ComponentType::Hebbian; }
    ComponentType getComponentType<StructuralPlasticity>() const { return ComponentType::StructuralPlasticity; }
    ComponentType getComponentType<NeuralWorkingMemory>() const { return ComponentType::NeuralWorkingMemory; }
    ComponentType getComponentType<NeuralEpisodicMemory>() const { return ComponentType::NeuralEpisodicMemory; }
    ComponentType getComponentType<NeuralAssociativeMemory>() const { return ComponentType::NeuralAssociativeMemory; }
    ComponentType getComponentType<PredictionSystem>() const { return ComponentType::PredictionSystem; }
    ComponentType getComponentType<NeuralPlanner>() const { return ComponentType::NeuralPlanner; }
    ComponentType getComponentType<ConceptFormation>() const { return ComponentType::ConceptFormation; }
    ComponentType getComponentType<AttentionalSelection>() const { return ComponentType::AttentionalSelection; }
    ComponentType getComponentType<DevelopmentSystem>() const { return ComponentType::DevelopmentSystem; }
    ComponentType getComponentType<Dopamine>() const { return ComponentType::Dopamine; }
    ComponentType getComponentType<Curiosity>() const { return ComponentType::Curiosity; }
    ComponentType getComponentType<PredictionError>() const { return ComponentType::PredictionError; }
    ComponentType getComponentType<Novelty>() const { return ComponentType::Novelty; }
    
    // Template specializations for retrieval from brain
    RandomGenerator* retrieveFromBrain<RandomGenerator>() { return brain_ ? brain_->getRandomGenerator() : nullptr; }
    SpikeSystem* retrieveFromBrain<SpikeSystem>() { return brain_ ? brain_->getSpikeSystem() : nullptr; }
    STDP* retrieveFromBrain<STDP>() { return brain_ ? brain_->getSTDP() : nullptr; }
    Hebbian* retrieveFromBrain<Hebbian>() { return brain_ ? brain_->getHebbian() : nullptr; }
    StructuralPlasticity* retrieveFromBrain<StructuralPlasticity>() { return brain_ ? brain_->getStructuralPlasticity() : nullptr; }
    NeuralWorkingMemory* retrieveFromBrain<NeuralWorkingMemory>() { return brain_ ? brain_->getWorkingMemory() : nullptr; }
    NeuralEpisodicMemory* retrieveFromBrain<NeuralEpisodicMemory>() { return brain_ ? brain_->getEpisodicMemory() : nullptr; }
    NeuralAssociativeMemory* retrieveFromBrain<NeuralAssociativeMemory>() { return brain_ ? brain_->getAssociativeMemory() : nullptr; }
    PredictionSystem* retrieveFromBrain<PredictionSystem>() { return brain_ ? brain_->getPredictionSystem() : nullptr; }
    NeuralPlanner* retrieveFromBrain<NeuralPlanner>() { return brain_ ? brain_->getPlanner() : nullptr; }
    ConceptFormation* retrieveFromBrain<ConceptFormation>() { return brain_ ? brain_->getConceptFormation() : nullptr; }
    AttentionalSelection* retrieveFromBrain<AttentionalSelection>() { return brain_ ? brain_->getAttention() : nullptr; }
    DevelopmentSystem* retrieveFromBrain<DevelopmentSystem>() { return brain_ ? brain_->getDevelopmentSystem() : nullptr; }
    Dopamine* retrieveFromBrain<Dopamine>() { return brain_ ? brain_->getDopamine() : nullptr; }
    Curiosity* retrieveFromBrain<Curiosity>() { return brain_ ? brain_->getCuriosity() : nullptr; }
    PredictionError* retrieveFromBrain<PredictionError>() { return brain_ ? brain_->getPredictionErrorSignal() : nullptr; }
    Novelty* retrieveFromBrain<Novelty>() { return brain_ ? brain_->getNovelty() : nullptr; }
    
    // Const versions
    const RandomGenerator* retrieveFromBrain<RandomGenerator>() const { return brain_ ? brain_->getRandomGenerator() : nullptr; }
    const SpikeSystem* retrieveFromBrain<SpikeSystem>() const { return brain_ ? brain_->getSpikeSystem() : nullptr; }
    const STDP* retrieveFromBrain<STDP>() const { return brain_ ? brain_->getSTDP() : nullptr; }
    const Hebbian* retrieveFromBrain<Hebbian>() const { return brain_ ? brain_->getHebbian() : nullptr; }
    const StructuralPlasticity* retrieveFromBrain<StructuralPlasticity>() const { return brain_ ? brain_->getStructuralPlasticity() : nullptr; }
    const NeuralWorkingMemory* retrieveFromBrain<NeuralWorkingMemory>() const { return brain_ ? brain_->getWorkingMemory() : nullptr; }
    const NeuralEpisodicMemory* retrieveFromBrain<NeuralEpisodicMemory>() const { return brain_ ? brain_->getEpisodicMemory() : nullptr; }
    const NeuralAssociativeMemory* retrieveFromBrain<NeuralAssociativeMemory>() const { return brain_ ? brain_->getAssociativeMemory() : nullptr; }
    const PredictionSystem* retrieveFromBrain<PredictionSystem>() const { return brain_ ? brain_->getPredictionSystem() : nullptr; }
    const NeuralPlanner* retrieveFromBrain<NeuralPlanner>() const { return brain_ ? brain_->getPlanner() : nullptr; }
    const ConceptFormation* retrieveFromBrain<ConceptFormation>() const { return brain_ ? brain_->getConceptFormation() : nullptr; }
    const AttentionalSelection* retrieveFromBrain<AttentionalSelection>() const { return brain_ ? brain_->getAttention() : nullptr; }
    const DevelopmentSystem* retrieveFromBrain<DevelopmentSystem>() const { return brain_ ? brain_->getDevelopmentSystem() : nullptr; }
    const Dopamine* retrieveFromBrain<Dopamine>() const { return brain_ ? brain_->getDopamine() : nullptr; }
    const Curiosity* retrieveFromBrain<Curiosity>() const { return brain_ ? brain_->getCuriosity() : nullptr; }
    const PredictionError* retrieveFromBrain<PredictionError>() const { return brain_ ? brain_->getPredictionErrorSignal() : nullptr; }
    const Novelty* retrieveFromBrain<Novelty>() const { return brain_ ? brain_->getNovelty() : nullptr; }
    
    // Cached components
    std::unordered_map<ComponentType, std::unique_ptr<void>> components_;
    std::shared_ptr<Brain> brain_;
};

// Convenience macros for component access
#define GET_BRAIN_COMPONENT(ComponentType, componentManager, componentVar) \
    auto& componentVar = componentManager.getComponent<ComponentType>();

#define CHECK_BRAIN_COMPONENT(ComponentType, componentManager) \
    if (!componentManager.hasComponent<ComponentType>()) { \
        NLM_LOG_ERROR("Required component " #ComponentType " not available"); \
        return false; \
    }

// Example usage in Brain.cpp:
// BrainComponentManager componentManager;
// componentManager.registerBrain(brain);
// 
// CHECK_BRAIN_COMPONENT(RandomGenerator, componentManager);
// auto& rng = componentManager.getComponent<RandomGenerator>();
// 
// CHECK_BRAIN_COMPONENT(STDP, componentManager);
// auto& stdp = componentManager.getComponent<STDP>();

} // namespace nlm
