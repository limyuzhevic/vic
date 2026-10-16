#pragma once

#include "../Brain.hpp"

namespace nlm {

class BrainNeuromodulation : public Brain {
public:
    // Neuromodulation systems access
    Dopamine* getDopamine() override;
    Curiosity* getCuriosity() override;
    Novelty* getNovelty() override;
    PredictionError* getPredictionErrorSignal() override;
    
    // Neuromodulation effects
    void applyNeuromodulation(const class Neuromodulator& signal) override;
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem() override;
    DevelopmentalStage getDevelopmentalStage() const override;
    void setDevelopmentalStage(DevelopmentalStage stage) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm