#pragma once

#include "Factory.hpp"
#include "BrainFactory.hpp"
#include "NeuronFactory.hpp"
#include "SynapseFactory.hpp"
#include "ConfigFactory.hpp"
#include <memory>
#include <stdexcept>

namespace nlm {

// DefaultBrainFactory implementation
class DefaultBrainFactory : public BrainFactory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override {
        if (typeName == "Brain") {
            return std::make_shared<Brain>();
        } else if (typeName == "NeuralRegion") {
            return std::make_shared<NeuralRegion>();
        } else if (typeName == "NeuralPopulation") {
            return std::make_shared<NeuralPopulation>();
        }
        throw std::invalid_argument("Cannot create object of type: " + typeName);
    }
    
    bool canCreate(const std::string& typeName) const override {
        return typeName == "Brain" || typeName == "NeuralRegion" || typeName == "NeuralPopulation";
    }
};

// DefaultNeuronFactory implementation
class DefaultNeuronFactory : public NeuronFactory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override {
        if (typeName == "Neuron") {
            return std::make_shared<Neuron>();
        } else if (typeName == "NeuronState") {
            return std::make_shared<NeuronState>();
        }
        throw std::invalid_argument("Cannot create object of type: " + typeName);
    }
    
    bool canCreate(const std::string& typeName) const override {
        return typeName == "Neuron" || typeName == "NeuronState";
    }
};

// DefaultSynapseFactory implementation
class DefaultSynapseFactory : public SynapseFactory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override {
        if (typeName == "Synapse") {
            return std::make_shared<Synapse>();
        } else if (typeName == "SynapticConnection") {
            return std::make_shared<SynapticConnection>();
        }
        throw std::invalid_argument("Cannot create object of type: " + typeName);
    }
    
    bool canCreate(const std::string& typeName) const override {
        return typeName == "Synapse" || typeName == "SynapticConnection";
    }
};

// DefaultConfigFactory implementation
class DefaultConfigFactory : public ConfigFactory {
public:
    std::shared_ptr<NLMObject> create(const std::string& typeName) override {
        if (typeName == "Config") {
            return std::make_shared<Config>();
        } else if (typeName == "ConfigParameter") {
            return std::make_shared<ConfigParameter>();
        } else if (typeName == "ConfigGroup") {
            return std::make_shared<ConfigGroup>();
        }
        throw std::invalid_argument("Cannot create object of type: " + typeName);
    }
    
    bool canCreate(const std::string& typeName) const override {
        return typeName == "Config" || typeName == "ConfigParameter" || typeName == "ConfigGroup";
    }
};

// Factory creator functions
std::unique_ptr<BrainFactory> createBrainFactory() {
    return std::make_unique<DefaultBrainFactory>();
}

std::unique_ptr<NeuronFactory> createNeuronFactory() {
    return std::make_unique<DefaultNeuronFactory>();
}

std::unique_ptr<SynapseFactory> createSynapseFactory() {
    return std::make_unique<DefaultSynapseFactory>();
}

std::unique_ptr<ConfigFactory> createConfigFactory() {
    return std::make_unique<DefaultConfigFactory>();
}
}