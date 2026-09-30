#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"

namespace nlm {

class NeuronInitializer {
public:
    NeuronInitializer();
    
    // Biological parameter initialization with constraints
    void initializeParameters(RandomGenerator& rng, NeuronState& state);
    
    // Randomize specific parameters with biological constraints
    void randomizeMembranePotential(RandomGenerator& rng, NeuronState& state);
    void randomizeThreshold(RandomGenerator& rng, NeuronState& state);
    void randomizeRestingPotential(RandomGenerator& rng, NeuronState& state);
    void randomizeResetPotential(RandomGenerator& rng, NeuronState& state);
    void randomizeRefractoryPeriod(RandomGenerator& rng, NeuronState& state);
    
    // Get default biological values
    static constexpr float getDefaultRestingPotential() { return -70.0f; }
    static constexpr float getDefaultThreshold() { return -55.0f; }
    static constexpr float getDefaultResetPotential() { return -70.0f; }
    static constexpr uint32_t getDefaultRefractoryPeriod() { return 5; }
    
    // Parameter bounds for biological plausibility
    struct ParameterBounds {
        float minMembranePotential = -80.0f;
        float maxMembranePotential = -60.0f;
        float minThreshold = -60.0f;
        float maxThreshold = -50.0f;
        float minRestingPotential = -75.0f;
        float maxRestingPotential = -65.0f;
        float minResetPotential = -75.0f;
        float maxResetPotential = -60.0f;
        uint32_t minRefractoryPeriod = 2;
        uint32_t maxRefractoryPeriod = 10;
    };
    
    static const ParameterBounds& getParameterBounds() { return bounds_; }
    
private:
    static ParameterBounds bounds_;
};

} // namespace nlm