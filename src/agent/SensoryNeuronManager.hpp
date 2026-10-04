#pragma once

#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

class SensoryNeuronManager {
public:
    SensoryNeuronManager(std::shared_ptr<Brain> brain);
    
    void distributeAndGroupSensoryNeurons();
    const std::vector<Neuron*>& getSensoryVision() const { return sensoryVision_; }
    const std::vector<Neuron*>& getSensoryTouch() const { return sensoryTouch_; }
    const std::vector<Neuron*>& getSensoryInternal() const { return sensoryInternal_; }
    const std::vector<Neuron*>& getSensoryProprioception() const { return sensoryProprioception_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
};

} // namespace nlm
