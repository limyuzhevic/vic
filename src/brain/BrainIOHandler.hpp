// BrainIOHandler.hpp - Input/output handling (sensory/motor)
#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainIOHandler {
public:
    static void receiveSensoryInput(Brain& brain, const SensoryInput& input);
    static void injectCurrent(Brain& brain, NeuronId neuron, MembranePotential current);
    static void injectCurrentToNeurons(Brain& brain, NeuronType type, MembranePotential current);
    static std::unique_ptr<Action> produceAction(Brain& brain);
    static void processSensoryNeurons(Brain& brain);
    static void processMotorNeurons(Brain& brain);

private:
    static void normalizeInput(float& normalizedValue);
};
