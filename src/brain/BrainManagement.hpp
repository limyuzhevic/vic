#pragma once

#include "brain/NeuralRegion.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>

namespace nlm {

class BrainManagement {
public:
    static bool initialize(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                          const Config& config,
                          std::vector<Neuron*>& sensoryNeurons,
                          std::vector<Neuron*>& motorNeurons);
    
    static void initializeConnectivity(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                      RandomGenerator& rng,
                                      float connectionProbability,
                                      float meanWeight,
                                      float stdDevWeight);
    
    static void collectIOElements(std::vector<Neuron*>& sensoryNeurons,
                                 std::vector<Neuron*>& motorNeurons,
                                 std::vector<std::unique_ptr<NeuralRegion>>& regions);
};

} // namespace nlm
