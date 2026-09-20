// BrainFactory.hpp - Complete version
#pragma once

#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <iostream>

namespace nlm {

// Forward declarations for all system types
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class CheckpointManager;

// System wrapper interface
class ISystem {
public:
    virtual ~ISystem() = default;
    virtual std::string getName() const = 0;
    virtual bool initialize(std::shared_ptr<Brain> brain) = 0;
    virtual void update(double timestep) = 0;
    virtual void reset() = 0;
};

// Template system wrapper
template<typename SystemType>
class SystemWrapper : public ISystem {
public:
    SystemWrapper(std::string name, std::unique_ptr<SystemType> system)
        : name_(std::move(name)), system_(std::move(system)) {}
    
    std::string getName() const override { return name_; }
    bool initialize(std::shared_ptr<Brain> brain) override {
        return system_->initialize(brain);
    }
    void update(double timestep) override {
        system_->update(timestep);
    }
    void reset() override {
        system_->reset();
    }
    
    SystemType* get() { return system_.get(); }
    const SystemType* get() const { return system_.get(); }

private:
    std::string name_;
    std::unique_ptr<SystemType> system_;
};

// BrainFactory class for system instantiation
class BrainFactory {
public:
    using SystemCreator = std::function<std::unique_ptr<void>(std::shared_ptr<Brain>)>;
    using SystemConfigMap = std::unordered_map<std::string, SystemConfig>;
    
    // Register a system factory
    template<typename SystemType>
    static void registerSystem(const std::string& name, bool enabled = true) {
        std::unique_ptr<ISystemFactory> factory = std::make_unique<SystemFactory<SystemType>>(name, enabled);
        getSystemFactories()[name] = std::move(factory);
    }
    
    // Register with custom creator function
    static void registerSystem(const std::string& name, 
                               SystemCreator creator,
                               bool enabled = true,
                               const SystemConfig& config = SystemConfig{}) {
        auto factory = std::make_unique<ISystemFactory>();
        class CustomFactory : public ISystemFactory {
        public:
            CustomFactory(std::string n, SystemCreator c, bool e, SystemConfig cfg)
                : name_(std::move(n)), creator_(std::move(c)), enabled_(e), config_(std::move(cfg)) {}
            
            std::string getSystemName() const override { return name_; }
            bool isEnabled() const override { return enabled_; }
            void setEnabled(bool enabled) override { enabled_ = enabled; }
            std::unique_ptr<void> create(std::shared_ptr<Brain> brain) override {
                if (!enabled_) return nullptr;
                return creator_(brain);
            }
            
            SystemConfig getConfig() const { return config_; }
        
        private:
            std::string name_;
            SystemCreator creator_;
            bool enabled_;
            SystemConfig config_;
        };
        
        factory = std::make_unique<CustomFactory>(name, creator, enabled, config);
        getSystemFactories()[name] = std::move(factory);
    }
    
    // Create all enabled systems and wrap them
    static bool createAndWrapSystems(std::shared_ptr<Brain> brain) {
        bool success = true;
        
        // Create each system based on configuration
        for (const auto& [name, factory] : getSystemFactories()) {
            if (!factory->isEnabled()) continue;
            
            std::cout << "Creating system: " << name << std::endl;
            auto system = factory->create(brain);
            if (system) {
                std::cout << "Successfully created system: " << name << std::endl;
                // Try to wrap the system
                if (!wrapSystem(brain, name, std::move(system))) {
                    success = false;
                }
            } else {
                std::cout << "Failed to create system: " << name << std::endl;
                success = false;
            }
        }
        
        return success;
    }
    
    // Initialize all wrapped systems
    static bool initializeWrappedSystems(std::shared_ptr<Brain> brain) {
        bool success = true;
        
        for (const auto& [name, system] : brain->getRegisteredSystems()) {
            std::cout << "Initializing system: " << name << std::endl;
            try {
                if (!system->initialize(brain)) {
                    std::cout << "Failed to initialize system: " << name << std::endl;
                    success = false;
                }
            } catch (const std::exception& e) {
                std::cout << "Exception during initialization of system " << name 
                         << ": " << e.what() << std::endl;
                success = false;
            }
        }
        
        return success;
    }
    
    // Get system configuration
    static SystemConfig getSystemConfig(const std::string& name) {
        auto it = getSystemConfigs().find(name);
        if (it != getSystemConfigs().end()) {
            return it->second;
        }
        return SystemConfig{name};
    }
    
    // Enable/disable a specific system
    static void setSystemEnabled(const std::string& name, bool enabled) {
        auto it = getSystemFactories().find(name);
        if (it != getSystemFactories().end()) {
            it->second->setEnabled(enabled);
        }
    }
    
private:
    // Wrap raw system pointer with wrapper
    static bool wrapSystem(std::shared_ptr<Brain> brain, const std::string& name, std::unique_ptr<void> systemPtr) {
        try {
            // Try to cast to known system types
            if (name == "spike_system") {
                auto spikeSystem = std::make_unique<SystemWrapper<SpikeSystem>>(name, 
                    std::unique_ptr<SpikeSystem>(static_cast<SpikeSystem*>(systemPtr.release())));
                brain->registerSystem(name, std::move(spikeSystem));
            } else if (name == "stdp") {
                auto stdpSystem = std::make_unique<SystemWrapper<STDP>>(name,
                    std::unique_ptr<STDP>(static_cast<STDP*>(systemPtr.release())));
                brain->registerSystem(name, std::move(stdpSystem));
            } else if (name == "hebbian") {
                auto hebbianSystem = std::make_unique<SystemWrapper<Hebbian>>(name,
                    std::unique_ptr<Hebbian>(static_cast<Hebbian*>(systemPtr.release())));
                brain->registerSystem(name, std::move(hebbianSystem));
            } else if (name == "structural_plasticity") {
                auto spSystem = std::make_unique<SystemWrapper<StructuralPlasticity>>(name,
                    std::unique_ptr<StructuralPlasticity>(static_cast<StructuralPlasticity*>(systemPtr.release())));
                brain->registerSystem(name, std::move(spSystem));
            } else if (name == "working_memory") {
                auto wmSystem = std::make_unique<SystemWrapper<NeuralWorkingMemory>>(name,
                    std::unique_ptr<NeuralWorkingMemory>(static_cast<NeuralWorkingMemory*>(systemPtr.release())));
                brain->registerSystem(name, std::move(wmSystem));
            } else if (name == "episodic_memory") {
                auto emSystem = std::make_unique<SystemWrapper<NeuralEpisodicMemory>>(name,
                    std::unique_ptr<NeuralEpisodicMemory>(static_cast<NeuralEpisodicMemory*>(systemPtr.release())));
                brain->registerSystem(name, std::move(emSystem));
            } else if (name == "associative_memory") {
                auto amSystem = std::make_unique<SystemWrapper<NeuralAssociativeMemory>>(name,
                    std::unique_ptr<NeuralAssociativeMemory>(static_cast<NeuralAssociativeMemory*>(systemPtr.release())));
                brain->registerSystem(name, std::move(amSystem));
            } else if (name == "prediction_system") {
                auto psSystem = std::make_unique<SystemWrapper<PredictionSystem>>(name,
                    std::unique_ptr<PredictionSystem>(static_cast<PredictionSystem*>(systemPtr.release())));
                brain->registerSystem(name, std::move(psSystem));
            } else if (name == "neural_planner") {
                auto npSystem = std::make_unique<SystemWrapper<NeuralPlanner>>(name,
                    std::unique_ptr<NeuralPlanner>(static_cast<NeuralPlanner*>(systemPtr.release())));
                brain->registerSystem(name, std::move(npSystem));
            } else if (name == "concept_formation") {
                auto cfSystem = std::make_unique<SystemWrapper<ConceptFormation>>(name,
                    std::unique_ptr<ConceptFormation>(static_cast<ConceptFormation*>(systemPtr.release())));
                brain->registerSystem(name, std::move(cfSystem));
            } else if (name == "attention") {
                auto attSystem = std::make_unique<SystemWrapper<AttentionalSelection>>(name,
                    std::unique_ptr<AttentionalSelection>(static_cast<AttentionalSelection*>(systemPtr.release())));
                brain->registerSystem(name, std::move(attSystem));
            } else if (name == "development_system") {
                auto dsSystem = std::make_unique<SystemWrapper<DevelopmentSystem>>(name,
                    std::unique_ptr<DevelopmentSystem>(static_cast<DevelopmentSystem*>(systemPtr.release())));
                brain->registerSystem(name, std::move(dsSystem));
            } else if (name == "dopamine") {
                auto dopSystem = std::make_unique<SystemWrapper<Dopamine>>(name,
                    std::unique_ptr<Dopamine>(static_cast<Dopamine*>(systemPtr.release())));
                brain->registerSystem(name, std::move(dopSystem));
            } else if (name == "curiosity") {
                auto curSystem = std::make_unique<SystemWrapper<Curiosity>>(name,
                    std::unique_ptr<Curiosity>(static_cast<Curiosity*>(systemPtr.release())));
                brain->registerSystem(name, std::move(curSystem));
            } else if (name == "novelty") {
                auto novSystem = std::make_unique<SystemWrapper<Novelty>>(name,
                    std::unique_ptr<Novelty>(static_cast<Novelty*>(systemPtr.release())));
                brain->registerSystem(name, std::move(novSystem));
            } else if (name == "prediction_error") {
                auto peSystem = std::make_unique<SystemWrapper<PredictionError>>(name,
                    std::unique_ptr<PredictionError>(static_cast<PredictionError*>(systemPtr.release())));
                brain->registerSystem(name, std::move(peSystem));
            } else if (name == "checkpoint_manager") {
                auto cpSystem = std::make_unique<SystemWrapper<CheckpointManager>>(name,
                    std::unique_ptr<CheckpointManager>(static_cast<CheckpointManager*>(systemPtr.release())));
                brain->registerSystem(name, std::move(cpSystem));
            } else {
                std::cout << "Unknown system type for wrapping: " << name << std::endl;
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            std::cout << "Exception while wrapping system " << name << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Get static system factories registry
    static std::unordered_map<std::string, std::unique_ptr<ISystemFactory>>& getSystemFactories() {
        static std::unordered_map<std::string, std::unique_ptr<ISystemFactory>> factories;
        return factories;
    }
    
    // Get static system configurations
    static std::unordered_map<std::string, SystemConfig>& getSystemConfigs() {
        static std::unordered_map<std::string, SystemConfig> configs;
        return configs;
    }
};

// Initialize default system factories
inline void initializeDefaultFactories() {
    // Register core brain components
    BrainFactory::registerSystem<SpikeSystem>("spike_system", true);
    BrainFactory::registerSystem<STDP>("stdp", true);
    BrainFactory::registerSystem<Hebbian>("hebbian", true);
    BrainFactory::registerSystem<StructuralPlasticity>("structural_plasticity", true);
    
    // Register integrated systems
    BrainFactory::registerSystem<NeuralWorkingMemory>("working_memory", true);
    BrainFactory::registerSystem<NeuralEpisodicMemory>("episodic_memory", true);
    BrainFactory::registerSystem<NeuralAssociativeMemory>("associative_memory", true);
    BrainFactory::registerSystem<PredictionSystem>("prediction_system", true);
    
    // Register cognition systems
    BrainFactory::registerSystem<NeuralPlanner>("neural_planner", true);
    BrainFactory::registerSystem<ConceptFormation>("concept_formation", true);
    BrainFactory::registerSystem<AttentionalSelection>("attention", true);
    
    // Register neuromodulation systems
    BrainFactory::registerSystem<DevelopmentSystem>("development_system", true);
    BrainFactory::registerSystem<Dopamine>("dopamine", true);
    BrainFactory::registerSystem<Curiosity>("curiosity", true);
    BrainFactory::registerSystem<Novelty>("novelty", true);
    BrainFactory::registerSystem<PredictionError>("prediction_error", true);
    
    // Register utility systems
    BrainFactory::registerSystem<CheckpointManager>("checkpoint_manager", true);
}

} // namespace nlm
