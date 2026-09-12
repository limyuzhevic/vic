// Brain utilities implementation
#include "BrainUtils.hpp"
#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

float BrainUtils::getExcitationInhibitionRatio(Brain* brain) {
    if (!brain) return 0.0f;
    return brain->getExcitationInhibitionRatio();
}

size_t BrainUtils::getTotalSpikeCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalSpikeCount();
}

size_t BrainUtils::getPendingSpikeEventCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getPendingSpikeEventCount();
}

size_t BrainUtils::getTotalNeuronCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalNeuronCount();
}

size_t BrainUtils::getTotalSynapseCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalSynapseCount();
}

size_t BrainUtils::getActiveNeuronCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getActiveNeuronCount();
}

size_t BrainUtils::getFiringNeuronCount(Brain* brain) {
    if (!brain) return 0;
    return brain->getFiringNeuronCount();
}

float BrainUtils::getAverageFiringRate(Brain* brain) {
    if (!brain) return 0.0f;
    return brain->getAverageFiringRate();
}

std::unique_ptr<Action> BrainUtils::produceAction(Brain* brain) {
    if (!brain) return std::make_unique<Action>(ActionType::Wait);
    return brain->produceAction();
}

void BrainUtils::applyNeuromodulation(Brain* brain, const Neuromodulator& neuromodulator) {
    if (brain) {
        brain->applyNeuromodulation(neuromodulator);
    }
}

RandomGenerator* BrainUtils::getRandomGenerator(Brain* brain) {
    if (!brain) return nullptr;
    return brain->getRandomGenerator();
}

} // namespace nlm