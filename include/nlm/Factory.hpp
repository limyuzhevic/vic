#pragma once

#include "CommonBaseClasses.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class NeuralRegion;
class NeuralPopulation;
class Neuron;
class NeuronState;
class Synapse;
class SynapticConnection;
class Config;
class ConfigParameter;
class ConfigGroup;

/**
 * Factory base class for creating NLM objects
 */
class Factory {
public:
    virtual ~Factory() = default;
    
    // Create an object of the specified type
    virtual std::shared_ptr<NLMObject> create(const std::string& typeName) = 0;
    
    // Check if the factory can create objects of the specified type
    virtual bool canCreate(const std::string& typeName) const = 0;
};

/**
 * BrainFactory for creating brain components
 */
class BrainFactory : public Factory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override;
    bool canCreate(const std::string& typeName) const override;
};

/**
 * NeuronFactory for creating neuron objects
 */
class NeuronFactory : public Factory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override;
    bool canCreate(const std::string& typeName) const override;
};

/**
 * SynapseFactory for creating synapse objects
 */
class SynapseFactory : public Factory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override;
    bool canCreate(const std::string& typeName) const override;
};

/**
 * ConfigFactory for creating configuration objects
 */
class ConfigFactory : public Factory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override;
    bool canCreate(const std::string& typeName) const override;
};
}
