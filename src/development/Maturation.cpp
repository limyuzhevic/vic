#include "Maturation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Maturation::Impl {
    float progress;
    float developmentalAge;
    float maturationRate;
    float learningScore;
    
    // Mature properties
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    Impl() 
        : progress(0.0f)
        , developmentalAge(0.0)
        , maturationRate(0.001f)
        , learningScore(0.0f)
        , matureThreshold(15.0f)
        , matureRestingPotential(-65.0f)
        , matureTimeConstant(0.02f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(class Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    // Calculate learning score from brain systems
    pImpl->learningScore = 0.0f;
    
    if (brain->getDopamine()) {
        float rewardSignal = brain->getDopamine()->getLevel();
        pImpl->learningScore += rewardSignal * 1.5f;
    }
    
    if (brain->getAcetylcholine()) {
        float attentionSignal = brain->getAcetylcholine()->getLevel();
        pImpl->learningScore += attentionSignal * 1.0f;
    }
    
    if (brain->getNovelty()) {
        float noveltyLevel = brain->getNovelty()->getLevel();
        pImpl->learningScore += noveltyLevel * 2.0f;
    }
    
    // Cap learning score
    pImpl->learningScore = std::min(1.0f, pImpl->learningScore);
    
    // Age-dependent maturation rate
    float ageModifier = 1.0f;
    if (pImpl->developmentalAge < 300.0) {
        ageModifier = pImpl->developmentalAge / 300.0f * 1.5f;
    } else if (pImpl->developmentalAge > 1200.0) {
        ageModifier = 1.5f;  // Slower in late maturation
    }
    
    // Learning-dependent maturation
    float learningInfluence = std::pow(pImpl->learningScore, 0.5f);
    
    // Stochastic element for biological realism
    float stochasticFactor = 1.0f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.3f;
    
    // Calculate maturation progress increase
    float maturationIncrease = maturationRate * ageModifier * learningInfluence * stochasticFactor;
    
    pImpl->progress = std::min(1.0f, pImpl->progress + maturationIncrease);
    
    NLM_LOG_INFO("Maturation: progress=" + std::to_string(pImpl->progress) + 
                " learningScore=" + std::to_string(pImpl->learningScore) +
                " age=" + std::to_string(pImpl->developmentalAge));
    
    // Update developmental age
    pImpl->developmentalAge += 0.001f;
}

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl->matureThreshold;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl->matureRestingPotential;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl->matureTimeConstant;
}

} // namespace nlm