#include "NeuronDistributor.hpp"
#include <algorithm>

namespace nlm {

void NeuronDistributor::distributeMotorNeurons(std::vector<Neuron*>& forward,
                                             std::vector<Neuron*>& backward,
                                             std::vector<Neuron*>& left,
                                             std::vector<Neuron*>& right,
                                             std::vector<Neuron*>& interact,
                                             std::vector<Neuron*>& wait,
                                             const std::vector<Neuron*>& allMotorNeurons) {
    forward.clear();
    backward.clear();
    left.clear();
    right.clear();
    interact.clear();
    wait.clear();
    
    for (Neuron* n : allMotorNeurons) {
        size_t idx = forward.size() + backward.size() + 
                     left.size() + right.size() +
                     interact.size() + wait.size();
        
        switch (idx % 6) {
            case 0: forward.push_back(n); break;
            case 1: backward.push_back(n); break;
            case 2: left.push_back(n); break;
            case 3: right.push_back(n); break;
            case 4: interact.push_back(n); break;
            case 5: wait.push_back(n); break;
        }
    }
}

void NeuronDistributor::distributeSensoryNeurons(std::vector<Neuron*>& vision,
                                                 std::vector<Neuron*>& touch,
                                                 std::vector<Neuron*>& internal,
                                                 std::vector<Neuron*>& proprioception,
                                                 const std::vector<Neuron*>& allSensoryNeurons) {
    vision.clear();
    touch.clear();
    internal.clear();
    proprioception.clear();
    
    for (Neuron* n : allSensoryNeurons) {
        size_t idx = vision.size() + touch.size() + 
                     internal.size() + proprioception.size();
        
        switch (idx % 4) {
            case 0: vision.push_back(n); break;
            case 1: touch.push_back(n); break;
            case 2: internal.push_back(n); break;
            case 3: proprioception.push_back(n); break;
        }
    }
}

} // namespace nlm