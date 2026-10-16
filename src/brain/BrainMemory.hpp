#pragma once

#include "../Brain.hpp"

namespace nlm {

class BrainMemory : public Brain {
public:
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() override;
    NeuralEpisodicMemory* getEpisodicMemory() override;
    NeuralAssociativeMemory* getAssociativeMemory() override;
    
    // Prediction system access
    PredictionSystem* getPredictionSystem() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm