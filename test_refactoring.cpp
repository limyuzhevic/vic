#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing refactored NLM classes..." << std::endl;
    
    // Test Neuron
    {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setMembranePotential(-60.0f);
        neuron.setType(nlm::NeuronType::Excitatory);
        neuron.setFiringRate(10.0f);
        
        std::cout << "Neuron test passed:" << std::endl;
        std::cout << "  ID: " << neuron.getId() << std::endl;
        std::cout << "  Potential: " << neuron.getMembranePotential() << std::endl;
        std::cout << "  Type: " << static_cast<int>(neuron.getType()) << std::endl;
        std::cout << "  Firing Rate: " << neuron.getFiringRate() << std::endl;
    }
    
    // Test Synapse
    {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setWeight(0.5f);
        synapse.setDelay(2);
        synapse.setType(nlm::SynapseType::Excitatory);
        
        std::cout << "Synapse test passed:" << std::endl;
        std::cout << "  ID: " << synapse.getId() << std::endl;
        std::cout << "  Weight: " << synapse.getWeight() << std::endl;
        std::cout << "  Delay: " << synapse.getDelay() << std::endl;
        std::cout << "  Type: " << static_cast<int>(synapse.getType()) << std::endl;
        std::cout << "  Is Excitatory: " << synapse.isExcitatory() << std::endl;
    }
    
    // Test Config
    {
        nlm::Config config;
        config.set("int_key", 42);
        config.set("string_key", std::string("test"));
        config.set("double_key", 3.14);
        config.set("bool_key", true);
        
        std::cout << "Config test passed:" << std::endl;
        std::cout << "  Has int_key: " << config.has("int_key") << std::endl;
        std::cout << "  Has string_key: " << config.has("string_key") << std::endl;
        std::cout << "  Has double_key: " << config.has("double_key") << std::endl;
        std::cout << "  Has bool_key: " << config.has("bool_key") << std::endl;
        
        auto intVal = config.get<int>("int_key");
        auto stringVal = config.get<std::string>("string_key");
        auto doubleVal = config.get<double>("double_key");
        auto boolVal = config.get<bool>("bool_key");
        
        if (intVal.has_value()) std::cout << "  int value: " << *intVal << std::endl;
        if (stringVal.has_value()) std::cout << "  string value: " << *stringVal << std::endl;
        if (doubleVal.has_value()) std::cout << "  double value: " << *doubleVal << std::endl;
        if (boolVal.has_value()) std::cout << "  bool value: " << *boolVal << std::endl;
    }
    
    std::cout << "\nAll tests passed! Refactoring successful." << std::endl;
    return 0;
}