#pragma once

#include "../brain/Neuron.hpp"
#include <vector>
#include "../core/Types/Types.hpp"

namespace nlm {

struct NeuronDistributor {
    static void distributeMotorNeurons(std::vector<Neuron*>& forward,
                                      std::vector<Neuron*>& backward,
                                      std::vector<Neuron*>& left,
                                      std::vector<Neuron*>& right,
                                      std::vector<Neuron*>& interact,
                                      std::vector<Neuron*>& wait,
                                      const std::vector<Neuron*>& allMotorNeurons);
    static void distributeSensoryNeurons(std::vector<Neuron*>& vision,
                                        std::vector<Neuron*>& touch,
                                        std::vector<Neuron*>& internal,
                                        std::vector<Neuron*>& proprioception,
                                        const std::vector<Neuron*>& allSensoryNeurons);
};

} // namespace nlm