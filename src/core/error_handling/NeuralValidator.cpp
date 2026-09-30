#include "NeuralValidator.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Types/Types.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void NeuralValidator::validateNeuralSystemInitialization(const Brain* brain) {
    if (!brain) {
        throw BrainInitializationException("Brain object is null", "NeuralValidator");
    }
    
    // Validate that essential systems are properly initialized
    if (!brain->getConfig()) {
        throw BrainInitializationException("Brain configuration is not initialized", "NeuralValidator");
    }
    
    // Validate spike system
    if (!brain->getSpikeSystem()) {
        throw BrainInitializationException("Spike system is not initialized", "NeuralValidator");
    }
    
    // Validate plasticity systems
    if (!brain->getSTDP() || !brain->getHebbian() || !brain->getStructuralPlasticity()) {
        throw BrainInitializationException("Plasticity systems are not initialized", "NeuralValidator");
    }
    
    // Validate neuron count
    size_t neuronCount = brain->getConfig()->getOr<size_t>("neuron_count", 0);
    if (neuronCount == 0) {
        throw BrainInitializationException("Neuron count must be greater than zero", "NeuralValidator");
    }
}

void NeuralValidator::validateNeuronParameters(const Neuron& neuron) {
    const NeuronState& state = neuron.getState();
    
    // Validate membrane potential
    if (state.membranePotential < -100.0f || state.membranePotential > 50.0f) {
        std::map<std::string, std::string> context;
        context["neuron_id"] = std::to_string(neuron.getId().value);
        context["membrane_potential"] = std::to_string(state.membranePotential);
        context["valid_range"] = "[-100.0, 50.0] mV";
        
        throw MemoryException(
            "Invalid neuron membrane potential",
            context,
            "NeuralValidator"
        );
    }
    
    // Validate threshold
    VALIDATE_RANGE(state.threshold, -80.0f, 0.0f, "neuron_threshold");
    
    // Validate firing rate
    if (state.firingRate < 0.0f || state.firingRate > 1000.0f) {
        std::map<std::string, std::string> context;
        context["neuron_id"] = std::to_string(neuron.getId().value);
        context["firing_rate"] = std::to_string(state.firingRate);
        context["valid_range"] = "[0.0, 1000.0] Hz";
        
        throw PlasticityException(
            "Invalid neuron firing rate",
            context,
            "NeuralValidator"
        );
    }
    
    // Validate refractory period
    if (state.refractoryPeriod < 0) {
        throw PlasticityException(
            "Invalid neuron refractory period",
            {{ "refractory_period", std::to_string(state.refractoryPeriod) }},
            "NeuralValidator"
        );
    }
}

void NeuralValidator::validateSynapticConnectivity(const Synapse& synapse) {
    // Validate synaptic weight
    SynapticWeight weight = synapse.getWeight();
    VALIDATE_RANGE(weight, -1.0f, 1.0f, "synaptic_weight");
    
    // Validate delay
    Delay delay = synapse.getDelay();
    VALIDATE_POSITIVE(delay, "synaptic_delay");
    
    // Validate source and destination neurons are not the same
    if (synapse.getSourceNeuron() == synapse.getDestinationNeuron()) {
        throw PlasticityException(
            "Synapse cannot connect neuron to itself",
            {{ "source_neuron", std::to_string(synapse.getSourceNeuron().value) },
             { "destination_neuron", std::to_string(synapse.getDestinationNeuron().value) }},
            "NeuralValidator"
        );
    }
}

void NeuralValidator::validateNeuralPopulation(const NeuralRegion& region) {
    // Validate region size
    size_t neuronCount = region.getNeuronCount();
    if (neuronCount == 0) {
        throw MemoryException(
            "Neural region must contain at least one neuron",
            {{ "region_id", std::to_string(region.getId().value) }},
            "NeuralValidator"
        );
    }
    
    // Validate neuron types are balanced
    auto neuronTypes = region.getNeuronTypes();
    if (neuronTypes.find(NeuronType::Excitatory) == neuronTypes.end() &&
        neuronTypes.find(NeuronType::Inhibitory) == neuronTypes.end()) {
        std::map<std::string, std::string> context;
        context["region_id"] = std::to_string(region.getId().value);
        context["region_name"] = region.getName();
        
        throw PlasticityException(
            "Neural region must contain at least one excitatory or inhibitory neuron",
            context,
            "NeuralValidator"
        );
    }
}

void NeuralValidator::handleNeuralSystemError(const std::exception& e) {
    try {
        throw;
    } catch (const BrainException& be) {
        LOG_ERROR("Neural system error: " + std::string(be.whatWithDetails()));
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected neural system error: " + std::string(e.what()));
        throw BrainRuntimeException(
            "Unexpected error in neural system",
            {{ "error", std::string(e.what()) }},
            "NeuralValidator"
        );
    }
}

} // namespace nlm
