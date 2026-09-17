#pragma once

#include <memory>
#include <string>

namespace nlm {

// Brain factory interface
class IBrainFactory {
public:
    virtual ~IBrainFactory() = default;
    virtual std::shared_ptr<class Brain> create(const std::shared_ptr<class Config>& config) = 0;
    virtual std::string getName() const = 0;
    virtual bool isAvailable() const = 0;
};

// Default brain factory implementation
class DefaultBrainFactory : public IBrainFactory {
public:
    DefaultBrainFactory();
    ~DefaultBrainFactory() override;
    
    std::shared_ptr<Brain> create(const std::shared_ptr<Config>& config) override;
    std::string getName() const override { return "DefaultBrainFactory"; }
    bool isAvailable() const override { return true; }
};

// Brain factory registry
class BrainFactoryRegistry {
public:
    BrainFactoryRegistry();
    ~BrainFactoryRegistry() = default;
    
    // Disable copying
    BrainFactoryRegistry(const BrainFactoryRegistry&) = delete;
    BrainFactoryRegistry& operator=(const BrainFactoryRegistry&) = delete;
    
    // Singleton access
    static BrainFactoryRegistry& getInstance();
    
    // Register a factory
    void registerFactory(const std::string& name, std::unique_ptr<IBrainFactory> factory);
    
    // Create brain by name
    std::shared_ptr<Brain> createBrain(const std::string& factoryName, const std::shared_ptr<Config>& config);
    
    // Get all registered factory names
    std::vector<std::string> getRegisteredFactories() const;
    
    // Check if factory is registered
    bool hasFactory(const std::string& name) const;
    
    // Remove factory
    void unregisterFactory(const std::string& name);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// World factory interface
class IWorldFactory {
public:
    virtual ~IWorldFactory() = default;
    virtual std::unique_ptr<class World> create() = 0;
    virtual std::string getName() const = 0;
    virtual bool isAvailable() const = 0;
};

// Default world factory implementation
class DefaultWorldFactory : public IWorldFactory {
public:
    DefaultWorldFactory();
    ~DefaultWorldFactory() override;
    
    std::unique_ptr<World> create() override;
    std::string getName() const override { return "DefaultWorldFactory"; }
    bool isAvailable() const override { return true; }
};

// World factory registry
class WorldFactoryRegistry {
public:
    WorldFactoryRegistry();
    ~WorldFactoryRegistry() = default;
    
    // Disable copying
    WorldFactoryRegistry(const WorldFactoryRegistry&) = delete;
    WorldFactoryRegistry& operator=(const WorldFactoryRegistry&) = delete;
    
    // Singleton access
    static WorldFactoryRegistry& getInstance();
    
    // Register a factory
    void registerFactory(const std::string& name, std::unique_ptr<IWorldFactory> factory);
    
    // Create world by name
    std::unique_ptr<World> createWorld(const std::string& factoryName);
    
    // Get all registered factory names
    std::vector<std::string> getRegisteredFactories() const;
    
    // Check if factory is registered
    bool hasFactory(const std::string& name) const;
    
    // Remove factory
    void unregisterFactory(const std::string& name);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Abstract brain creation interface
class IBrainCreator {
public:
    virtual ~IBrainCreator() = default;
    virtual bool canCreate(const std::shared_ptr<Config>& config) const = 0;
    virtual std::shared_ptr<Brain> create(const std::shared_ptr<Config>& config) = 0;
    virtual std::string getDescription() const = 0;
};

// Abstract world creation interface
class IWorldCreator {
public:
    virtual ~IWorldCreator() = default;
    virtual bool canCreate() const = 0;
    virtual std::unique_ptr<World> create() = 0;
    virtual std::string getDescription() const = 0;
};

// System creator registry
class SystemCreatorRegistry {
public:
    SystemCreatorRegistry();
    ~SystemCreatorRegistry() = default;
    
    // Brain creator management
    void registerBrainCreator(std::unique_ptr<IBrainCreator> creator);
    std::vector<IBrainCreator*> getBrainCreators() const;
    std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config);
    
    // World creator management
    void registerWorldCreator(std::unique_ptr<IWorldCreator> creator);
    std::vector<IWorldCreator*> getWorldCreators() const;
    std::unique_ptr<World> createWorld();
    
    // Singleton access
    static SystemCreatorRegistry& getInstance();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
