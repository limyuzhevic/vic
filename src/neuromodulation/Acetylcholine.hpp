#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

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
    
    // Acetylcholine signaling
    void signalNovelty(float novelty);
    void signalMemoryConsolidation();
    void signalAttentionFocus();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
