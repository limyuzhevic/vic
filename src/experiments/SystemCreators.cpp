#include "SystemCreators.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/World.hpp"

namespace nlm {

// Brain factory registry implementation
struct BrainFactoryRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<IBrainFactory>> factories;
    
    Impl() {
        // Register default factory
        registerFactory("default", std::make_unique<DefaultBrainFactory>());
    }
    
    void registerFactory(const std::string& name, std::unique_ptr<IBrainFactory> factory) {
        if (factory && factory->isAvailable()) {
            factories[name] = std::move(factory);
            NLM_LOG_INFO("Registered brain factory: " + name);
        } else {
            NLM_LOG_WARNING("Attempted to register unavailable brain factory: " + name);
        }
    }
    
    std::shared_ptr<Brain> createBrain(const std::string& factoryName, const std::shared_ptr<Config>& config) {
        auto it = factories.find(factoryName);
        if (it != factories.end()) {
            NLM_LOG_INFO("Creating brain using factory: " + factoryName);
            return it->second->create(config);
        }
        
        NLM_LOG_ERROR("Brain factory not found: " + factoryName);
        return nullptr;
    }
    
    std::vector<std::string> getRegisteredFactories() const {
        std::vector<std::string> names;
        names.reserve(factories.size());
        for (const auto& pair : factories) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool hasFactory(const std::string& name) const {
        return factories.find(name) != factories.end();
    }
    
    void unregisterFactory(const std::string& name) {
        auto it = factories.find(name);
        if (it != factories.end()) {
            NLM_LOG_INFO("Unregistered brain factory: " + name);
            factories.erase(it);
        }
    }
};

BrainFactoryRegistry::BrainFactoryRegistry() : pImpl(std::make_unique<Impl>()) {}

BrainFactoryRegistry& BrainFactoryRegistry::getInstance() {
    static BrainFactoryRegistry instance;
    return instance;
}

void BrainFactoryRegistry::registerFactory(const std::string& name, std::unique_ptr<IBrainFactory> factory) {
    pImpl->registerFactory(name, std::move(factory));
}

std::shared_ptr<Brain> BrainFactoryRegistry::createBrain(const std::string& factoryName, const std::shared_ptr<Config>& config) {
    return pImpl->createBrain(factoryName, config);
}

std::vector<std::string> BrainFactoryRegistry::getRegisteredFactories() const {
    return pImpl->getRegisteredFactories();
}

bool BrainFactoryRegistry::hasFactory(const std::string& name) const {
    return pImpl->hasFactory(name);
}

void BrainFactoryRegistry::unregisterFactory(const std::string& name) {
    pImpl->unregisterFactory(name);
}

DefaultBrainFactory::DefaultBrainFactory() = default;
DefaultBrainFactory::~DefaultBrainFactory() = default;

std::shared_ptr<Brain> DefaultBrainFactory::create(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Creating default brain");
    return std::make_shared<Brain>(config);
}

// World factory registry implementation
struct WorldFactoryRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<IWorldFactory>> factories;
    
    Impl() {
        // Register default factory
        registerFactory("default", std::make_unique<DefaultWorldFactory>());
    }
    
    void registerFactory(const std::string& name, std::unique_ptr<IWorldFactory> factory) {
        if (factory && factory->isAvailable()) {
            factories[name] = std::move(factory);
            NLM_LOG_INFO("Registered world factory: " + name);
        } else {
            NLM_LOG_WARNING("Attempted to register unavailable world factory: " + name);
        }
    }
    
    std::unique_ptr<World> createWorld(const std::string& factoryName) {
        auto it = factories.find(factoryName);
        if (it != factories.end()) {
            NLM_LOG_INFO("Creating world using factory: " + factoryName);
            return it->second->create();
        }
        
        NLM_LOG_ERROR("World factory not found: " + factoryName);
        return nullptr;
    }
    
    std::vector<std::string> getRegisteredFactories() const {
        std::vector<std::string> names;
        names.reserve(factories.size());
        for (const auto& pair : factories) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool hasFactory(const std::string& name) const {
        return factories.find(name) != factories.end();
    }
    
    void unregisterFactory(const std::string& name) {
        auto it = factories.find(name);
        if (it != factories.end()) {
            NLM_LOG_INFO("Unregistered world factory: " + name);
            factories.erase(it);
        }
    }
};

WorldFactoryRegistry::WorldFactoryRegistry() : pImpl(std::make_unique<Impl>()) {}

WorldFactoryRegistry& WorldFactoryRegistry::getInstance() {
    static WorldFactoryRegistry instance;
    return instance;
}

void WorldFactoryRegistry::registerFactory(const std::string& name, std::unique_ptr<IWorldFactory> factory) {
    pImpl->registerFactory(name, std::move(factory));
}

std::unique_ptr<World> WorldFactoryRegistry::createWorld(const std::string& factoryName) {
    return pImpl->createWorld(factoryName);
}

std::vector<std::string> WorldFactoryRegistry::getRegisteredFactories() const {
    return pImpl->getRegisteredFactories();
}

bool WorldFactoryRegistry::hasFactory(const std::string& name) const {
    return pImpl->hasFactory(name);
}

void WorldFactoryRegistry::unregisterFactory(const std::string& name) {
    pImpl->unregisterFactory(name);
}

DefaultWorldFactory::DefaultWorldFactory() = default;
DefaultWorldFactory::~DefaultWorldFactory() = default;

std::unique_ptr<World> DefaultWorldFactory::create() {
    NLM_LOG_INFO("Creating default world");
    return std::make_unique<SimpleWorld>();
}

// System creator registry implementation
struct SystemCreatorRegistry::Impl {
    std::vector<std::unique_ptr<IBrainCreator>> brainCreators;
    std::vector<std::unique_ptr<IWorldCreator>> worldCreators;
    
    Impl() {
        // Register default creators
        registerBrainCreator(std::make_unique<DefaultBrainCreator>());
        registerWorldCreator(std::make_unique<DefaultWorldCreator>());
    }
    
    void registerBrainCreator(std::unique_ptr<IBrainCreator> creator) {
        if (creator) {
            brainCreators.push_back(std::move(creator));
            NLM_LOG_INFO("Registered brain creator");
        }
    }
    
    std::vector<IBrainCreator*> getBrainCreators() const {
        std::vector<IBrainCreator*> creators;
        creators.reserve(brainCreators.size());
        for (const auto& creator : brainCreators) {
            creators.push_back(creator.get());
        }
        return creators;
    }
    
    std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) {
        for (const auto& creator : brainCreators) {
            if (creator->canCreate(config)) {
                NLM_LOG_INFO("Creating brain using creator: " + creator->getDescription());
                return creator->create(config);
            }
        }
        
        NLM_LOG_ERROR("No suitable brain creator found");
        return nullptr;
    }
    
    void registerWorldCreator(std::unique_ptr<IWorldCreator> creator) {
        if (creator) {
            worldCreators.push_back(std::move(creator));
            NLM_LOG_INFO("Registered world creator");
        }
    }
    
    std::vector<IWorldCreator*> getWorldCreators() const {
        std::vector<IWorldCreator*> creators;
        creators.reserve(worldCreators.size());
        for (const auto& creator : worldCreators) {
            creators.push_back(creator.get());
        }
        return creators;
    }
    
    std::unique_ptr<World> createWorld() {
        for (const auto& creator : worldCreators) {
            if (creator->canCreate()) {
                NLM_LOG_INFO("Creating world using creator: " + creator->getDescription());
                return creator->create();
            }
        }
        
        NLM_LOG_ERROR("No suitable world creator found");
        return nullptr;
    }
};

SystemCreatorRegistry::SystemCreatorRegistry() : pImpl(std::make_unique<Impl>()) {}

SystemCreatorRegistry& SystemCreatorRegistry::getInstance() {
    static SystemCreatorRegistry instance;
    return instance;
}

void SystemCreatorRegistry::registerBrainCreator(std::unique_ptr<IBrainCreator> creator) {
    pImpl->registerBrainCreator(std::move(creator));
}

std::vector<IBrainCreator*> SystemCreatorRegistry::getBrainCreators() const {
    return pImpl->getBrainCreators();
}

std::shared_ptr<Brain> SystemCreatorRegistry::createBrain(const std::shared_ptr<Config>& config) {
    return pImpl->createBrain(config);
}

void SystemCreatorRegistry::registerWorldCreator(std::unique_ptr<IWorldCreator> creator) {
    pImpl->registerWorldCreator(std::move(creator));
}

std::vector<IWorldCreator*> SystemCreatorRegistry::getWorldCreators() const {
    return pImpl->getWorldCreators();
}

std::unique_ptr<World> SystemCreatorRegistry::createWorld() {
    return pImpl->createWorld();
}

// Default brain creator
class DefaultBrainCreator : public IBrainCreator {
public:
    bool canCreate(const std::shared_ptr<Config>& config) const override {
        return config != nullptr;
    }
    
    std::shared_ptr<Brain> create(const std::shared_ptr<Config>& config) override {
        return std::make_shared<Brain>(config);
    }
    
    std::string getDescription() const override {
        return "Default Brain Creator";
    }
};

// Default world creator
class DefaultWorldCreator : public IWorldCreator {
public:
    bool canCreate() const override {
        return true;
    }
    
    std::unique_ptr<World> create() override {
        return std::make_unique<SimpleWorld>();
    }
    
    std::string getDescription() const override {
        return "Default World Creator";
    }
};

} // namespace nlm
