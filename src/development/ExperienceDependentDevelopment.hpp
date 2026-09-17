#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

class ExperienceDependentDevelopment {
public:
    ExperienceDependentDevelopment();
    ~ExperienceDependentDevelopment();
    
    // Update development based on experience and learning
    void update(Brain* brain, RandomGenerator& rng, double age, double dt);
    
    // Integrate Hebbian learning with developmental plasticity
    void integrateHebbianDevelopment(Brain* brain, RandomGenerator& rng, double dt);
    
    // Use memory to guide developmental changes
    void memoryGuidedDevelopment(Brain* brain, double dt);
    
    // Adapt development based on environmental demands
    void adaptiveDevelopment(Brain* brain, RandomGenerator& rng, double dt);
    
    // Track learning experiences for development
    void recordLearningExperience(Brain* brain, float reward, double timestamp);
    
    // Get experience integration efficiency
    float getExperienceIntegrationEfficiency() const;
    
private:
    struct Impl;
    Impl* pImpl;
    std::vector<float> recentRewards;
    double lastExperienceUpdate;
};

} // namespace nlm