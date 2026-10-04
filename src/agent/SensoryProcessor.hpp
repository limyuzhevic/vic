#pragma once

#include "SensoryPercept.hpp"
#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

class SensoryProcessor {
public:
    SensoryProcessor(float noveltyDecay = 0.99f);
    
    void processInput(const SensoryPercept& percept,
                      const std::vector<Neuron*>& visionNeurons,
                      const std::vector<Neuron*>& touchNeurons,
                      const std::vector<Neuron*>& internalNeurons,
                      const std::vector<Neuron*>& proprioceptionNeurons,
                      std::vector<float>& previousVision,
                      float predictionError = 0.0f);
    
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    void setCuriosityGain(float gain) { curiosityGain_ = gain; }
    
private:
    float noveltyLevel_;
    float curiosityLevel_;
    bool curiosityEnabled_;
    float curiosityGain_;
    float noveltyDecay_;
};

} // namespace nlm
