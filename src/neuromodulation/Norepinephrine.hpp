#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <boost/shared_ptr.hpp>

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
    
    // Signal arousal to the brain
    void signalArousal(float intensity, boost::shared_ptr<Brain> brain);
    
    // Reset neuromodulator to baseline
    void reset();
    
    // Signal effect to brain
    void signal(Brain* brain) override {
        boost::shared_ptr<Brain> brainPtr(brain);
        signalArousal(1.0f, brainPtr);
    }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
