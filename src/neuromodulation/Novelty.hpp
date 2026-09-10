#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <boost/shared_ptr.hpp>

namespace nlm {

class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Detect novelty from sensory input
    void detectNovelty(const class SensoryInput& observation,
                      const class SensoryInput& previousObservation,
                      boost::shared_ptr<Brain> brain);
    
    // Reset neuromodulator to baseline
    void reset();
    
    // Signal effect to brain
    void signal(Brain* brain) override {
        boost::shared_ptr<Brain> brainPtr(brain);
        detectNovelty(observation_, previousObservation_, brainPtr);
    }
    
    // Store last observations
    void setLastObservation(const class SensoryInput& obs) {
        observation_ = obs;
    }
    
    void setPreviousObservation(const class SensoryInput& obs) {
        previousObservation_ = obs;
    }
    
private:
    struct Impl;
    Impl* pImpl;
    class SensoryInput observation_;
    class SensoryInput previousObservation_;
};

} // namespace nlm
