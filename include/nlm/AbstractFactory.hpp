#pragma once

#include "CommonBaseClasses.hpp"
#include "EventSystem.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

namespace nlm {

/**
 * Abstract factory for creating NLM objects
 * Provides a simplified interface for object creation
 */
class AbstractFactory {
public:
    virtual ~AbstractFactory() = default;
    
    // Create an object of the specified type
    virtual std::shared_ptr<NLMObject> create(const std::string& typeName) = 0;
    
    // Create a specific type of object
    template<typename T, typename... Args>
    std::shared_ptr<T> create(Args&&... args);
    
    // Check if the factory can create objects of the specified type
    virtual bool canCreate(const std::string& typeName) const = 0;
    
    // Get the list of available types
    virtual std::vector<std::string> getAvailableTypes() const = 0;
};

/**
 * Object factory registry for managing factories
 */
class ObjectFactory {
private:
    std::unordered_map<std::string, std::unique_ptr<AbstractFactory>> factories;
    std::unordered_map<std::string, std::function<std::shared_ptr<NLMObject>()>> creators;
    
public:
    // Register a factory
    void registerFactory(const std::string& typeName, std::unique_ptr<AbstractFactory> factory);
    
    // Register a creator function
    void registerCreator(const std::string& typeName, std::function<std::shared_ptr<NLMObject>()> creator);
    
    // Create an object of the specified type
    std::shared_ptr<NLMObject> create(const std::string& typeName);
    
    // Create a specific type of object
    template<typename T>
    std::shared_ptr<T> create() {
        auto obj = create(typeid(T).name());
        return std::dynamic_pointer_cast<T>(obj);
    }
    
    // Check if the factory can create objects of the specified type
    bool canCreate(const std::string& typeName) const;
    
    // Get the list of available types
    std::vector<std::string> getAvailableTypes() const;
    
    // Clear all factories
    void clear();
};

/**
 * Factory manager for managing different factories
 */
class FactoryManager {
private:
    static std::unique_ptr<FactoryManager> instance;
    ObjectFactory objectFactory;
    
protected:
    FactoryManager() = default;
    
public:
    static FactoryManager& getInstance();
    
    // Get the object factory
    ObjectFactory& getObjectFactory();
    
    // Register factories
    void registerFactories();
    
    // Clear all factories
    void clear();
};

/**
 * Template implementation for factory creation
 */
template<typename T, typename... Args>
std::shared_ptr<T> AbstractFactory::create(Args&&... args) {
    auto obj = create(typeid(T).name(), std::forward<Args>(args)...);
    return std::dynamic_pointer_cast<T>(obj);
}
}