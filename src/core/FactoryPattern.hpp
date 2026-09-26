/*
 * NLM Factory Pattern - Object Creation Framework
 * Provides abstract factory, factory methods, and object pool patterns for extensible neural system
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <any>
#include <stdexcept>
#include <mutex>
#include <deque>

namespace nlm {

// Forward declarations for factory types
class IFactory;
class IObjectFactory;
class IObjectPool;
class FactoryRegistry;

// Factory Type Enum
enum class FactoryType {
    Static,
    Dynamic,
    Abstract,
    Method
};

// Factory Metadata
struct FactoryMetadata {
    std::string name;
    std::string description;
    FactoryType type;
    bool enabled = true;
    std::vector<std::string> dependencies;
    std::map<std::string, std::any> metadata;
    
    FactoryMetadata() = default;
    FactoryMetadata(const std::string& name, const std::string& desc, FactoryType type)
        : name(name), description(desc), type(type), enabled(true) {}
};

// Abstract Factory Interface
class IFactory {
public:
    virtual ~IFactory() = default;
    
    virtual const FactoryMetadata& getMetadata() const = 0;
    virtual FactoryType getType() const = 0;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    virtual bool isInitialized() const { return initialized_; }
    
    // Factory methods for creating objects
    virtual std::any create(const std::string& objectType, const std::map<std::string, std::any>& params = {}) = 0;
    virtual bool canCreate(const std::string& objectType) const = 0;
    
    // Factory configuration
    virtual void configure(const std::map<std::string, std::any>& config) {}
    
protected:
    bool initialized_ = false;
};

// Object Factory Interface
class IObjectFactory {
public:
    virtual ~IObjectFactory() = default;
    
    virtual std::string getObjectType() const = 0;
    virtual std::any createObject(const std::map<std::string, std::any>& params = {}) = 0;
    virtual bool canCreateObject(const std::map<std::string, std::any>& params) const = 0;
};

// Object Pool Interface
class IObjectPool {
public:
    virtual ~IObjectPool() = default;
    
    virtual std::string getObjectType() const = 0;
    virtual std::any acquire(const std::map<std::string, std::any>& params = {}) = 0;
    virtual bool release(std::any object) = 0;
    virtual size_t getAvailableCount() const = 0;
    virtual size_t getMaxSize() const = 0;
    virtual void setMaxSize(size_t maxSize) = 0;
};

// Factory Registry - Manages all factories
class IFactoryRegistry {
public:
    virtual ~IFactoryRegistry() = default;
    
    // Factory lifecycle management
    virtual bool registerFactory(std::shared_ptr<IFactory> factory) = 0;
    virtual bool unregisterFactory(const std::string& factoryName) = 0;
    virtual bool initializeFactory(const std::string& factoryName) = 0;
    virtual bool shutdownFactory(const std::string& factoryName) = 0;
    
    // Object creation
    virtual std::any createObject(const std::string& factoryName, const std::string& objectType, 
                                const std::map<std::string, std::any>& params = {}) = 0;
    
    // Query factories
    virtual std::vector<std::string> getFactoryNames() const = 0;
    virtual std::vector<std::string> getFactoryNames(const std::string& category) const = 0;
    virtual FactoryMetadata getFactoryMetadata(const std::string& factoryName) const = 0;
    
    // Object pool management
    virtual bool registerPool(const std::string& poolName, std::shared_ptr<IObjectPool> pool) = 0;
    virtual std::shared_ptr<IObjectPool> getPool(const std::string& poolName) = 0;
    virtual std::any acquireObject(const std::string& poolName, const std::map<std::string, std::any>& params = {}) = 0;
    virtual bool releaseObject(const std::string& poolName, std::any object) = 0;
    
    // Factory methods (alternative to class hierarchies)
    virtual bool registerFactoryMethod(const std::string& factoryName, const std::string& objectType,
                                      std::function<std::any(const std::map<std::string, std::any>&)> method) = 0;
};

// Default implementation of the factory registry
class FactoryRegistry : public IFactoryRegistry {
public:
    FactoryRegistry();
    ~FactoryRegistry() override;
    
    // Factory lifecycle management
    bool registerFactory(std::shared_ptr<IFactory> factory) override;
    bool unregisterFactory(const std::string& factoryName) override;
    bool initializeFactory(const std::string& factoryName) override;
    bool shutdownFactory(const std::string& factoryName) override;
    
    // Object creation
    std::any createObject(const std::string& factoryName, const std::string& objectType,
                         const std::map<std::string, std::any>& params = {}) override;
    
    // Query factories
    std::vector<std::string> getFactoryNames() const override;
    std::vector<std::string> getFactoryNames(const std::string& category) const override;
    FactoryMetadata getFactoryMetadata(const std::string& factoryName) const override;
    
    // Object pool management
    bool registerPool(const std::string& poolName, std::shared_ptr<IObjectPool> pool) override;
    std::shared_ptr<IObjectPool> getPool(const std::string& poolName) override;
    std::any acquireObject(const std::string& poolName, const std::map<std::string, std::any>& params = {}) override;
    bool releaseObject(const std::string& poolName, std::any object) override;
    
    // Factory methods
    bool registerFactoryMethod(const std::string& factoryName, const std::string& objectType,
                              std::function<std::any(const std::map<std::string, std::any>&)> method) override;
    
    // Global instance access
    static FactoryRegistry& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Static Factory - Creates objects using static methods
class StaticFactory : public IFactory {
public:
    StaticFactory(const std::string& name, const std::string& description);
    ~StaticFactory() override;
    
    const FactoryMetadata& getMetadata() const override { return metadata_; }
    FactoryType getType() const override { return FactoryType::Static; }
    
    bool initialize() override;
    void shutdown() override;
    
    std::any create(const std::string& objectType, const std::map<std::string, std::any>& params = {}) override;
    bool canCreate(const std::string& objectType) const override;
    
    // Register static creation method
    template<typename T, typename... Args>
    void registerCreator(const std::string& objectType, std::function<T(Args...)> creator) {
        creators_[objectType] = [creator](const std::map<std::string, std::any>& params) -> std::any {
            return creator(convertParams<T>(params)...);
        };
    }
    
    // Helper to convert parameters
    template<typename T>
    T convertParams(const std::map<std::string, std::any>& params) {
        // Implementation would depend on parameter mapping
        // This is a simplified version
        throw std::runtime_error("Parameter conversion not implemented");
    }
    
private:
    FactoryMetadata metadata_;
    std::map<std::string, std::function<std::any(const std::map<std::string, std::any>&)>> creators_;
};

// Dynamic Factory - Creates objects using dynamic loading
class DynamicFactory : public IFactory {
public:
    DynamicFactory(const std::string& name, const std::string& description);
    ~DynamicFactory() override;
    
    const FactoryMetadata& getMetadata() const override { return metadata_; }
    FactoryType getType() const override { return FactoryType::Dynamic; }
    
    bool initialize() override;
    void shutdown() override;
    
    std::any create(const std::string& objectType, const std::map<std::string, std::any>& params = {}) override;
    bool canCreate(const std::string& objectType) const override;
    
    // Load factory from file
    bool loadFromFile(const std::string& filepath);
    
    // Load factory from symbol
    bool loadFromSymbol(const std::string& symbolName);
    
private:
    FactoryMetadata metadata_;
    std::map<std::string, std::string> objectTypeToSymbol;
    void* handle_; // Dynamic library handle
};

// Factory System - Manages factory lifecycle and object creation
class FactorySystem {
public:
    FactorySystem();
    ~FactorySystem();
    
    // Initialize the factory system
    bool initialize();
    
    // Shutdown the factory system
    void shutdown();
    
    // Get the global registry
    IFactoryRegistry& getRegistry() { return registry_; }
    
    // Get object by type (using registry to find appropriate factory)
    std::any getObject(const std::string& objectType, const std::map<std::string, std::any>& params = {});
    
    // Check if object can be created
    bool canCreateObject(const std::string& objectType) const;
    
    // Global instance access
    static FactorySystem& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    IFactoryRegistry& registry_;
};

// Object Factory Template - Convenience for common factory patterns
class ObjectFactory {
public:
    template<typename T, typename... Args>
    static std::function<std::any(const std::map<std::string, std::any>&)> createCreator(Args... args) {
        return [args...](const std::map<std::string, std::any>& params) -> std::any {
            // Create object with provided arguments
            return std::make_shared<T>(args...);
        };
    }
    
    template<typename T>
    static std::function<std::any(const std::map<std::string, std::any>&)> createParameterizedCreator() {
        return [](const std::map<std::string, std::any>& params) -> std::any {
            // Create object with parameters from map
            // Implementation would depend on T's constructor
            throw std::runtime_error("Parameterized creation not implemented for type " + std::string(typeid(T).name());
        };
    }
};

} // namespace nlm