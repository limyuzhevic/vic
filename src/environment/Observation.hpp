#pragma once

#include "../motor/Action.hpp"
#include "../sensory/SensoryInput.hpp"

namespace nlm {

// Sensory input observation.
//
// Phase 5 design choice: this is the ONE Observation class in nlm.
// There used to be a second, incompatible Observation in
// src/world/ (a vector<float> wrapper with setFromPercept), but it
// was in no build target, included by nothing, and would collide with
// this class if both were ever linked (same namespace, same name).
// It was deleted; its only functionality (flattening a percept to
// floats) already exists as SensoryPercept::getAllSignals(), which is
// what the live AgentBrain/SimpleWorld loop uses. Novelty::detectNovelty
// and Reward::computeReward take an Observation by forward declaration
// but are placeholders that never inspect it, and no caller uses them.
class Observation {
public:
    Observation();
    Observation(const SensoryInput& input);
    ~Observation();
    
    // Get the sensory input
    const SensoryInput* getSensoryInput() const;
    
    // Check if observation is valid
    bool isValid() const;
    
    // Clone
    std::unique_ptr<Observation> clone() const;
    
private:
    std::unique_ptr<SensoryInput> sensoryInput_;
};

} // namespace nlm
