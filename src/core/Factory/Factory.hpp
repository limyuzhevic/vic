#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>

namespace nlm {

// Factory registration interface
class FactoryRegistrarBase {
public:
    virtual ~FactoryRegistrarBase() = default;
    virtual std::string getName() const = 0;
    virtual int getPriority() const { return 0; }
};

// Template factory for creating objects
template<typename T, typename Id = std::string>
class Factory {
public:
    using CreatorFunc = std::function<std::unique_ptr<T>(const Id&, const std::map<std::string, std::string>&)>
        ;
    using ValidatorFunc = std::function<bool(const Id&, const std::map<std::string, std::string>&)>;
    using DeleterFunc = std::function<void(T*)>;
    
    Factory(const std::string& name) : name_(name) {}
    
    // Register a creator function
    void registerCreator(const std::string& key, CreatorFunc creator, int priority = 0) {
        creators_[key] = std::make_pair(std::move(creator), priority);
        updateCreatorOrder();
    }
    
    // Register a validator function
    void registerValidator(const std::string& key, ValidatorFunc validator) {
        validators_[key] = std::move(validator);
    }
    
    // Register a deleter function
    void registerDeleter(DeleterFunc deleter) {
        deleter_ = std::move(deleter);
    }
    
    // Create an object
    std::unique_ptr<T> create(const Id& id, const std::map<std::string, std::string>& params = {}) {
        // Find the best matching creator (highest priority)
        std::string bestKey;
        int bestPriority = -1;
        
        for (const auto& [key, pair] : creators_) {
            if (pair.second > bestPriority) {
                bestKey = key;
                bestPriority = pair.second;
            }
        }
        
        if (bestKey.empty()) {
            throw std::runtime_error("No creator registered for object type: " + name_);
        }
        
        // Validate parameters
        auto it = validators_.find(bestKey);
        if (it != validators_.end() && !it->second(id, params)) {
            throw std::runtime_error("Validation failed for object creation: " + name_);
        }
        
        return creators_[bestKey].first(id, params);
    }
    
    // Check if a creator is registered
    bool hasCreator(const std::string& key) const {
        return creators_.find(key) != creators_.end();
    }
    
    // Get all registered keys
    std::vector<std::string> getRegisteredKeys() const {
        std::vector<std::string> keys;
        keys.reserve(creators_.size());
        for (const auto& [key, _] : creators_) {
            keys.push_back(key);
        }
        return keys;
    }
    
    // Get factory name
    const std::string& getName() const { return name_; }
    
protected:
    void updateCreatorOrder() {
        // Sort creators by priority (highest first)
        std::vector<std::pair<int, std::string>> priorityPairs;
        for (const auto& [key, pair] : creators_) {
            priorityPairs.push_back(std::make_pair(pair.second, key));
        }
        
        std::sort(priorityPairs.begin(), priorityPairs.end(), std::greater<>());
        
        // Reconstruct creators map in priority order
        std::map<std::string, std::pair<CreatorFunc, int>> sortedCreators;
        for (const auto& [priority, key] : priorityPairs) {
            sortedCreators[key] = creators_[key];
        }
        
        creators_ = std::move(sortedCreators);
    }
    
private:
    std::string name_;
    std::map<std::string, std::pair<CreatorFunc, int>> creators_;
    std::map<std::string, ValidatorFunc> validators_;
    DeleterFunc deleter_;
};

// Singleton base class
template<typename T>
class Singleton {
protected:
    Singleton() = default;
    virtual ~Singleton() = default;
    
public:
    // Disable copying
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    // Get the singleton instance
    static T& getInstance() {
        static T instance;
        return instance;
    }
    
    // Get the singleton instance (const version)
    static const T& getInstance() const {
        static const T instance;
        return instance;
    }
};

// Registry base class for factory registration
template<typename Derived>
class FactoryRegistry : public Singleton<FactoryRegistry<Derived>> {
public:
    using FactoryRegistrarBasePtr = std::unique_ptr<FactoryRegistrarBase>;
    
    // Register a factory registrar
    template<typename Registrar>
    void registerFactory(std::unique_ptr<Registrar> registrar) {
        registrar->setRegistry(this);
        factories_[registrar->getName()] = std::move(registrar);
    }
    
    // Get a factory by name
    FactoryRegistrarBase* getFactory(const std::string& name) {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    // Get all registered factory names
    std::vector<std::string> getFactoryNames() const {
        std::vector<std::string> names;
        names.reserve(factories_.size());
        for (const auto& [name, _] : factories_) {
            names.push_back(name);
        }
        return names;
    }
    
    // Initialize the registry (call once)
    void initialize() {
        // Call derived class initialization
        static_cast<Derived*>(this)->initializeFactories();
    }
    
protected:
    virtual void initializeFactories() = 0;
    
    // Set the registry pointer in registrar (protected for derived classes)
    template<typename Registrar>
    void setRegistry(Registrar* registrar, FactoryRegistry* registry) {
        registrar->setRegistry(registry);
    }
    
private:
    std::map<std::string, FactoryRegistrarBasePtr> factories_;
};

} // namespace nlm
