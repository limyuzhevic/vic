#pragma once

#include "Factory.hpp"
#include "BrainFactory.hpp"
#include "NeuronFactory.hpp"
#include "SynapseFactory.hpp"
#include "ConfigFactory.hpp"
#include <memory>
#include <string>

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
 * Default implementation of BrainFactory
 */
class DefaultBrainFactory : public BrainFactory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override;
    bool canCreate(const std::string& typeName) const override;
};

/**
 * Default implementation of NeuronFactory
 */
class DefaultNeuronFactory : public NeuronFactory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override;
    bool canCreate(const std::string& typeName) const override;
};

/**
 * Default implementation of SynapseFactory
 */
class DefaultSynapseFactory : public SynapseFactory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override;
    bool canCreate(const std::string& typeName) const override;
};

/**
 * Default implementation of ConfigFactory
 */
class DefaultConfigFactory : public ConfigFactory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override;
    bool canCreate(const std::string& typeName) const override;
};

/**
 * Factory creator functions
 */
std::unique_ptr<BrainFactory> createBrainFactory();
std::unique_ptr<NeuronFactory> createNeuronFactory();
std::unique_ptr<SynapseFactory> createSynapseFactory();
std::unique_ptr<ConfigFactory> createConfigFactory();
}
