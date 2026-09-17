#include "ExperimentFactory.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

struct ExperimentFactoryRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<IExperimentFactory>> factories;
    
    Impl() {
        // Register default factory
        registerFactory("default", std::make_unique<DefaultExperimentFactory>());
    }
    
    void registerFactory(const std::string& name, std::unique_ptr<IExperimentFactory> factory) {
        if (factory && factory->isAvailable()) {
            factories[name] = std::move(factory);
            NLM_LOG_INFO("Registered experiment factory: " + name);
        } else {
            NLM_LOG_WARNING("Attempted to register unavailable factory: " + name);
        }
    }
    
    std::shared_ptr<Phase6IntegratedExperiment> createExperiment(
        const std::string& factoryName, const Phase6Config& config) {
        
        auto it = factories.find(factoryName);
        if (it != factories.end()) {
            NLM_LOG_INFO("Creating experiment using factory: " + factoryName);
            return it->second->create(config);
        }
        
        NLM_LOG_ERROR("Factory not found: " + factoryName);
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
            NLM_LOG_INFO("Unregistered experiment factory: " + name);
            factories.erase(it);
        }
    }
};

ExperimentFactoryRegistry::ExperimentFactoryRegistry() : pImpl(std::make_unique<Impl>()) {}

ExperimentFactoryRegistry& ExperimentFactoryRegistry::getInstance() {
    static ExperimentFactoryRegistry instance;
    return instance;
}

void ExperimentFactoryRegistry::registerFactory(const std::string& name, std::unique_ptr<IExperimentFactory> factory) {
    pImpl->registerFactory(name, std::move(factory));
}

std::shared_ptr<Phase6IntegratedExperiment> ExperimentFactoryRegistry::createExperiment(
    const std::string& factoryName, const Phase6Config& config) {
    
    return pImpl->createExperiment(factoryName, config);
}

std::vector<std::string> ExperimentFactoryRegistry::getRegisteredFactories() const {
    return pImpl->getRegisteredFactories();
}

bool ExperimentFactoryRegistry::hasFactory(const std::string& name) const {
    return pImpl->hasFactory(name);
}

void ExperimentFactoryRegistry::unregisterFactory(const std::string& name) {
    pImpl->unregisterFactory(name);
}

DefaultExperimentFactory::DefaultExperimentFactory() = default;
DefaultExperimentFactory::~DefaultExperimentFactory() = default;

std::shared_ptr<Phase6IntegratedExperiment> DefaultExperimentFactory::create(const Phase6Config& config) {
    NLM_LOG_INFO("Creating default Phase 6 integrated experiment");
    return std::make_shared<Phase6IntegratedExperiment>();
}

} // namespace nlm
