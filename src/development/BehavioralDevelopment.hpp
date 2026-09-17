#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>
#include <cmath>

namespace nlm {

class BehavioralDevelopment {
public:
    BehavioralDevelopment();
    ~BehavioralDevelopment();
    
    // Update behavioral patterns based on development
    void update(Brain* brain, RandomGenerator& rng, double age, double dt);
    
    // Develop curiosity and exploration behavior
    void developExploration(Brain* brain, RandomGenerator& rng, double age);
    
    // Develop habituation and attention
    void developAttention(Brain* brain, RandomGenerator& rng, double age);
    
    // Develop stress response
    void developStressResponse(Brain* brain, RandomGenerator& rng, double age);
    
    // Develop social behaviors
    void developSocialBehavior(Brain* brain, RandomGenerator& rng, double age);
    
    // Get current behavioral profile
    BehavioralProfile getProfile() const;
    
    // Get behavioral tendency for a specific behavior
    float getBehavioralTendency(const char* behaviorName) const;
    
private:
    struct Impl;
    Impl* pImpl;
    BehavioralProfile profile;
};

} // namespace nlm