#pragma once

#include "NeuromodulatorBase.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
// PLACEHOLDER - Phase 2
class Novelty : public NeuromodulatorBase {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    NeuromodulatorType getType() const override;
    void loadConfig(const std::string& key, float value) override;
    
    // Novelty-specific methods
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward signal for reinforcement learning
// PLACEHOLDER - Phase 2
class Reward : public NeuromodulatorBase {
public:
    Reward();
    ~Reward() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    NeuromodulatorType getType() const override;
    void loadConfig(const std::string& key, float value) override;
    
    // Reward-specific methods
    float getValue() const;
    void setValue(float value);
    void add(float delta);
    void reset();
    float computeReward(const class Observation& observation) const;
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm