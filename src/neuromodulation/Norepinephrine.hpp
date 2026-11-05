#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced functionality
    void signalArousal(float intensity);
    void signalVigilance(float intensity);
    
private:
    struct Impl;
    Impl* pImpl;
};

}