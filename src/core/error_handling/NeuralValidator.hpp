// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Neural system validation and error handling
 */
class NeuralValidator {
public:
    static void validateNeuralSystemInitialization(const Brain* brain);
    static void validateNeuronParameters(const Neuron& neuron);
    static void validateSynapticConnectivity(const Synapse& synapse);
    static void validateNeuralPopulation(const NeuralRegion& region);
    static void handleNeuralSystemError(const std::exception& e);
};

} // namespace nlm
