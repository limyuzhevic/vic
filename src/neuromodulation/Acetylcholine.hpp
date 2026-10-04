#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced neuromodulation effects
    void initialize(class Brain* brain) override;
    void applyToNeuralExcitability() override;
    void applyToPlasticity(class STDP* stdp, class Hebbian* hebbian) override;
    void applyToMemory(class NeuralEpisodicMemory* episodicMemory) override;
    void applyToAttention(class AttentionalSelection* attention) override;
    void applyToDevelopment(class DevelopmentSystem* development) override;
    
    // Novelty detection and attention modulation
    void signalNovelty(float novelty);
    void enhanceAttentionDuringNovelty(bool isNovel);
    float getAttentionModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm