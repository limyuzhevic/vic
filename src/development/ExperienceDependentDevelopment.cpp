#include "ExperienceDependentDevelopment.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct ExperienceDependentDevelopment::Impl {
    double lastUpdate;
    float experienceIntegration;
    
    Impl() : lastUpdate(0.0), experienceIntegration(0.0f) {}
};

ExperienceDependentDevelopment::ExperienceDependentDevelopment() : pImpl(new Impl) {}

ExperienceDependentDevelopment::~ExperienceDependentDevelopment() = default;

void ExperienceDependentDevelopment::update(Brain* brain, RandomGenerator& rng, double age, double dt) {
    integrateHebbianDevelopment(brain, rng, dt);
    memoryGuidedDevelopment(brain, dt);
    adaptiveDevelopment(brain, rng, dt);
}

void ExperienceDependentDevelopment::integrateHebbianDevelopment(Brain* brain, RandomGenerator& rng, double dt) {
    // Hebbian learning is always active but modulated by development
    // In a full implementation, this would connect with the brain's Hebbian plasticity system
    float hebbianStrength = 0.5f * (1.0f - getExperienceIntegrationEfficiency());  // Less experience = more Hebbian
    
    // The brain's hebbian system should use this factor
    if (brain && brain->getHebbian()) {
        // In a full implementation, would configure hebbian weights
    }
}

void ExperienceDependentDevelopment::memoryGuidedDevelopment(Brain* brain, double dt) {
    if (!brain) return;
    
    // Use episodic memory to guide development
    if (brain->getEpisodicMemory()) {
        size_t episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
        
        // More episodes = more efficient development
        float integrationEfficiency = std::min(1.0f, episodeCount / 100.0f);
        pImpl->experienceIntegration = integrationEfficiency;
        
        // Record development based on experiences
        recordLearningExperience(brain, 0.5f, 0.0);  // Default reward
    }
}

void ExperienceDependentDevelopment::adaptiveDevelopment(Brain* brain, RandomGenerator& rng, double dt) {
    double ageFraction = std::min(1.0, getDevelopmentalAge() / 960.0);
    
    // Adapt development based on random experiences
    float experienceDrivenChange = rng.nextFloat() * 0.1f * (1.0f - ageFraction);
    
    // Apply experience-driven developmental changes
    // In a full implementation, this would modify neural properties
}

void ExperienceDependentDevelopment::recordLearningExperience(Brain* brain, float reward, double timestamp) {
    recentRewards.push_back(reward);
    
    // Keep only recent experiences for efficiency
    if (recentRewards.size() > 100) {
        recentRewards.erase(recentRewards.begin());
    }
    
    lastExperienceUpdate = timestamp;
}

float ExperienceDependentDevelopment::getExperienceIntegrationEfficiency() const {
    if (recentRewards.empty()) return 0.0f;
    
    // Calculate average reward as a measure of experience quality
    float totalReward = 0.0f;
    for (float reward : recentRewards) {
        totalReward += reward;
    }
    
    return totalReward / recentRewards.size();
}

} // namespace nlm