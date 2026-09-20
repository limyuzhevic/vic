// BrainMemoryIntegration.hpp - handles memory system integration and access
#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"

namespace nlm {

class BrainMemoryIntegration {
public:
    static void receiveSensoryInput(Brain::Impl* pImpl, const class SensoryInput& input);
    static void injectCurrent(Brain::Impl* pImpl, NeuronId neuron, MembranePotential current);
    static void injectCurrentToNeurons(Brain::Impl* pImpl, NeuronType type, MembranePotential current);
    static void reset(Brain::Impl* pImpl);
    static bool save(const Brain* brain, const std::string& filepath);
    static bool load(Brain::Impl* pImpl, const std::string& filepath);
    static void logStatus(const Brain* brain);
};

} // namespace nlm