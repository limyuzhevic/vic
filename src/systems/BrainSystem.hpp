#pragma once

#include "core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class Logger;

/**
 * @class BrainSystem
 * @brief Abstract base class for all brain systems.
 * 
 * This class provides the interface for modular brain subsystems.
 * Each system is responsible for a specific aspect of brain function:
 * - SpikeSystem: Handles neural firing and spike propagation
 * - NeuromodulationSystem: Manages neuromodulators like dopamine
 * - MemorySystem: Handles working and episodic memory
 * - PlasticitySystem: Manages synaptic plasticity rules (STDP, Hebbian)
 * - DevelopmentSystem: Controls brain development stages
 * - CognitionSystem: Handles planning and concept formation
 * 
 * Systems are initialized, configured, and stepped through a unified interface,
 * enabling modular architecture and easier testing and maintenance.
 */
class BrainSystem {
public:
    virtual ~BrainSystem() = default;
    
    /**
     * @brief Initialize the system
     * @return true if initialization succeeds, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Shut down the system
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Update system for one simulation step
     * @param step Current simulation step
     * @param time Current simulation time
     */
    virtual void step(SimulationStep step, Timestamp time) = 0;
    
    /**
     * @brief Configure the system with parameters
     * @param config Configuration object
     * @return true if configuration succeeds, false otherwise
     */
    virtual bool configure(const std::shared_ptr<Config>& config) = 0;
    
    /**
     * @brief Get the system name for identification
     * @return System name string
     */
    virtual const std::string& getName() const = 0;
    
    /**
     * @brief Get system status information
     * @return Status string with system information
     */
    virtual std::string getStatus() const = 0;
    
    /**
     * @brief Check if system is initialized
     * @return true if system is initialized
     */
    virtual bool isInitialized() const = 0;
    
    /**
     * @brief Set brain reference for system integration
     * @param brain Pointer to parent brain
     */
    virtual void setBrainReference(Brain* brain) = 0;
    
    /**
     * @brief Get system memory usage
     * @return Estimated memory usage in bytes
     */
    virtual size_t getMemoryUsage() const = 0;
    
    /**
     * @brief Log system status for debugging
     */
    virtual void logStatus() const = 0;
    
    // CRTP base class for common implementation
    template<typename Derived>
    class Base {
    public:
        virtual ~Base() = default;
        
        bool initialize() {
            if (!initialized_) {
                static_cast<Derived*>(this)->doInitialize();
                initialized_ = true;
            }
            return initialized_;
        }
        
        void shutdown() {
            if (initialized_) {
                static_cast<Derived*>(this)->doShutdown();
                initialized_ = false;
            }
        }
        
        void step(SimulationStep step, Timestamp time) {
            if (initialized_) {
                static_cast<Derived*>(this)->doStep(step, time);
            }
        }
        
        bool configure(const std::shared_ptr<Config>& config) {
            return static_cast<Derived*>(this)->doConfigure(config);
        }
        
        void setBrainReference(Brain* brain) {
            brain_ = brain;
            static_cast<Derived*>(this)->doSetBrainReference(brain);
        }
        
        bool isInitialized() const { return initialized_; }
        
        const std::string& getName() const { return static_cast<const Derived*>(this)->getNameImpl(); }
        
        std::string getStatus() const {
            return static_cast<const Derived*>(this)->getStatusImpl();
        }
        
        size_t getMemoryUsage() const {
            return static_cast<const Derived*>(this)->getMemoryUsageImpl();
        }
        
        void logStatus() const {
            Logger* logger = getLogger();
            if (logger) {
                logger->info("Status: " + getStatus());
            }
        }
        
    protected:
        Base() : initialized_(false), brain_(nullptr) {}
        
        // Virtual methods to be implemented by derived classes
        virtual void doInitialize() = 0;
        virtual void doShutdown() = 0;
        virtual void doStep(SimulationStep step, Timestamp time) = 0;
        virtual bool doConfigure(const std::shared_ptr<Config>& config) = 0;
        virtual void doSetBrainReference(Brain* brain) {}
        
        virtual const char* getNameImpl() const = 0;
        virtual std::string getStatusImpl() const = 0;
        virtual size_t getMemoryUsageImpl() const = 0;
        
        // Helper method to get logger
        Logger* getLogger() {
            if (brain_ && brain_->getConfig()) {
                return Logger::getGlobal();
            }
            return nullptr;
        }
        
    private:
        bool initialized_;
        Brain* brain_;
    };
};

/**
 * @class SystemRegistry
 * @brief Registry for brain systems
 * 
 * This class manages the registration and creation of brain systems.
 * It provides a factory pattern for creating system instances and
 * maintains a mapping of system names to their factory functions.
 */
class SystemRegistry {
public:
    using SystemFactory = std::function<std::unique_ptr<BrainSystem>(Brain*)>;
    
    SystemRegistry() = default;
    ~SystemRegistry() = default;
    
    /**
     * @brief Register a system factory
     * @param name System name identifier
     * @param factory Factory function to create system instances
     */
    void registerSystem(const std::string& name, SystemFactory factory) {
        systemFactories_[name] = std::move(factory);
    }
    
    /**
     * @brief Create a system instance
     * @param name System name
     * @param brain Parent brain instance
     * @return Unique pointer to system instance, or nullptr if not found
     */
    std::unique_ptr<BrainSystem> createSystem(const std::string& name, Brain* brain) {
        auto it = systemFactories_.find(name);
        if (it != systemFactories_.end()) {
            return it->second(brain);
        }
        return nullptr;
    }
    
    /**
     * @brief Check if system is registered
     * @param name System name
     * @return true if system is registered
     */
    bool hasSystem(const std::string& name) const {
        return systemFactories_.find(name) != systemFactories_.end();
    }
    
    /**
     * @brief Get all registered system names
     * @return Vector of system names
     */
    std::vector<std::string> getRegisteredSystems() const {
        std::vector<std::string> names;
        names.reserve(systemFactories_.size());
        for (const auto& pair : systemFactories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * @brief Unregister a system type
     * @param name System name
     */
    void unregisterSystem(const std::string& name) {
        systemFactories_.erase(name);
    }
    
    /**
     * @brief Get registry instance (singleton pattern)
     * @return Reference to global registry
     */
    static SystemRegistry& getInstance() {
        static SystemRegistry instance;
        return instance;
    }
    
    /**
     * @brief Initialize default system factories
     * @param brain Parent brain instance for factory functions
     */
    void initializeDefaultFactories(Brain* brain);
    
private:
    std::unordered_map<std::string, SystemFactory> systemFactories_;
};

/**
 * @class BrainSystemFacade
 * @brief Facade for brain system composition and management
 * 
 * This class provides a unified interface for managing multiple brain systems.
 * It handles system lifecycle, configuration, and provides convenient access
 * to individual systems while maintaining separation of concerns.
 */
class BrainSystemFacade : public BrainSystem {
public:
    BrainSystemFacade();
    ~BrainSystemFacade() override;
    
    // BrainSystem interface implementation
    bool initialize() override;
    void shutdown() override;
    void step(SimulationStep step, Timestamp time) override;
    bool configure(const std::shared_ptr<Config>& config) override;
    const std::string& getName() const override;
    std::string getStatus() const override;
    bool isInitialized() const override;
    void setBrainReference(Brain* brain) override;
    void logStatus() const override;
    size_t getMemoryUsage() const override;
    
    /**
     * @brief Add a system to the facade
     * @tparam T System type
     * @tparam Args System constructor arguments
     * @param args Constructor arguments
     * @return Pointer to added system
     */
    template<typename T, typename... Args>
    T* addSystem(Args&&... args) {
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = system.get();
        systems_.push_back(std::move(system));
        systemMap_[T::getName()] = rawPtr;
        return rawPtr;
    }
    
    /**
     * @brief Get a system by type
     * @tparam T System type
     * @return Pointer to system, or nullptr if not found
     */
    template<typename T>
    T* getSystem() {
        auto it = std::find_if(systems_.begin(), systems_.end(),
            [this](const std::unique_ptr<BrainSystem>& sys) {
                return dynamic_cast<T*>(sys.get()) != nullptr;
            });
        return it != systems_.end() ? dynamic_cast<T*>(it->get()) : nullptr;
    }
    
    /**
     * @brief Get a system by type (const version)
     * @tparam T System type
     * @return Const pointer to system, or nullptr if not found
     */
    template<typename T>
    const T* getSystem() const {
        auto it = std::find_if(systems_.begin(), systems_.end(),
            [this](const std::unique_ptr<BrainSystem>& sys) {
                return dynamic_cast<const T*>(sys.get()) != nullptr;
            });
        return it != systems_.end() ? dynamic_cast<const T*>(it->get()) : nullptr;
    }
    
    /**
     * @brief Get a system by name
     * @param name System name
     * @return Pointer to system, or nullptr if not found
     */
    BrainSystem* getSystemByName(const std::string& name) {
        auto it = systemMap_.find(name);
        return it != systemMap_.end() ? it->second : nullptr;
    }
    
    /**
     * @brief Get all system names
     * @return Vector of system names
     */
    std::vector<std::string> getSystemNames() const {
        std::vector<std::string> names;
        names.reserve(systemMap_.size());
        for (const auto& pair : systemMap_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * @brief Apply function to all systems
     * @param func Function to apply to each system
     */
    void forEachSystem(std::function<void(BrainSystem*)> func) {
        for (auto& sys : systems_) {
            func(sys.get());
        }
    }
    
    /**
     * @brief Apply const function to all systems
     * @param func Const function to apply to each system
     */
    void forEachSystem(std::function<void(const BrainSystem*)> func) const {
        for (const auto& sys : systems_) {
            func(sys.get());
        }
    }
    
    /**
     * @brief Get number of systems
     * @return Number of systems
     */
    size_t getSystemCount() const { return systems_.size(); }
    
    /**
     * @brief Load configuration from config object
     * @param config Configuration object
     */
    void loadSystemConfigurations(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Reset all systems
     */
    void resetAllSystems();
    
private:
    std::vector<std::unique_ptr<BrainSystem>> systems_;
    std::unordered_map<std::string, BrainSystem*> systemMap_;
    bool initialized_;
};

} // namespace nlm
