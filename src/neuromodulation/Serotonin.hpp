#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    virtual const char* getName() const = 0;
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    virtual float getPlasticityFactor() const = 0;
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

class Brain;

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    void initialize(Brain* brain);
    void signalReward(float reward);
    void signalSocialInteraction(float socialValue);
    void signalPunishment(float punishment);
    float getMood() const;
    float getImpulsivity() const;
    float getSocialBehavior() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm