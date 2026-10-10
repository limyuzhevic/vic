#pragma once

#include "AbstractFactory.hpp"
#include <stdexcept>
#include <memory>
#include <algorithm>

namespace nlm {

// AbstractFactory implementation
void AbstractFactory::registerFactory(const std::string& typeName, std::unique_ptr<AbstractFactory> factory) {
    if (!factory) {
        throw std::invalid_argument("Factory cannot be null");
    }
    factories[typeName] = std::move(factory);
}

void AbstractFactory::registerCreator(const std::string& typeName, std::function<std::shared_ptr<NLMObject>()> creator) {
    if (!creator) {
        throw std::invalid_argument("Creator cannot be null");
    }
    creators[typeName] = creator;
}

std::shared_ptr<NLMObject> AbstractFactory::create(const std::string& typeName) {
    auto it = creators.find(typeName);
    if (it != creators.end()) {
        return it->second();
    }
    
    auto it2 = factories.find(typeName);
    if (it2 != factories.end()) {
        return it2->second->create(typeName);
    }
    
    throw std::invalid_argument("Cannot create object of type: " + typeName);
}

bool AbstractFactory::canCreate(const std::string& typeName) const {
    return creators.find(typeName) != creators.end() || factories.find(typeName) != factories.end();
}

std::vector<std::string> AbstractFactory::getAvailableTypes() const {
    std::vector<std::string> types;
    types.reserve(creators.size() + factories.size());
    
    for (const auto& pair : creators) {
        types.push_back(pair.first);
    }
    
    for (const auto& pair : factories) {
        if (std::find(types.begin(), types.end(), pair.first) == types.end()) {
            types.push_back(pair.first);
        }
    }
    
    return types;
}

void AbstractFactory::clear() {
    creators.clear();
    factories.clear();
}

// ObjectFactory implementation
void ObjectFactory::registerFactory(const std::string& typeName, std::unique_ptr<AbstractFactory> factory) {
    if (!factory) {
        throw std::invalid_argument("Factory cannot be null");
    }
    factories[typeName] = std::move(factory);
}

void ObjectFactory::registerCreator(const std::string& typeName, std::function<std::shared_ptr<NLMObject>()> creator) {
    if (!creator) {
        throw std::invalid_argument("Creator cannot be null");
    }
    creators[typeName] = creator;
}

std::shared_ptr<NLMObject> ObjectFactory::create(const std::string& typeName) {
    auto it = creators.find(typeName);
    if (it != creators.end()) {
        return it->second();
    }
    
    auto it2 = factories.find(typeName);
    if (it2 != factories.end()) {
        return it2->second->create(typeName);
    }
    
    throw std::invalid_argument("Cannot create object of type: " + typeName);
}

bool ObjectFactory::canCreate(const std::string& typeName) const {
    return creators.find(typeName) != creators.end() || factories.find(typeName) != factories.end();
}

std::vector<std::string> ObjectFactory::getAvailableTypes() const {
    std::vector<std::string> types;
    types.reserve(creators.size() + factories.size());
    
    for (const auto& pair : creators) {
        types.push_back(pair.first);
    }
    
    for (const auto& pair : factories) {
        if (std::find(types.begin(), types.end(), pair.first) == types.end()) {
            types.push_back(pair.first);
        }
    }
    
    return types;
}

void ObjectFactory::clear() {
    creators.clear();
    factories.clear();
}

// FactoryManager implementation
std::unique_ptr<FactoryManager> FactoryManager::instance = nullptr;

FactoryManager& FactoryManager::getInstance() {
    if (!instance) {
        instance = std::make_unique<FactoryManager>();
    }
    return *instance;
}

ObjectFactory& FactoryManager::getObjectFactory() {
    return objectFactory;
}

void FactoryManager::registerFactories() {
    // Register default factories
    objectFactory.registerCreator("Brain", []() { return std::make_shared<Brain>(); });
    objectFactory.registerCreator("NeuralRegion", []() { return std::make_shared<NeuralRegion>(); });
    objectFactory.registerCreator("NeuralPopulation", []() { return std::make_shared<NeuralPopulation>(); });
    objectFactory.registerCreator("Neuron", []() { return std::make_shared<Neuron>(); });
    objectFactory.registerCreator("NeuronState", []() { return std::make_shared<NeuronState>(); });
    objectFactory.registerCreator("Synapse", []() { return std::make_shared<Synapse>(); });
    objectFactory.registerCreator("SynapticConnection", []() { return std::make_shared<SynapticConnection>(); });
    objectFactory.registerCreator("Config", []() { return std::make_shared<Config>(); });
    objectFactory.registerCreator("ConfigParameter", []() { return std::make_shared<ConfigParameter>(); });
    objectFactory.registerCreator("ConfigGroup", []() { return std::make_shared<ConfigGroup>(); });
}

void FactoryManager::clear() {
    objectFactory.clear();
}
}