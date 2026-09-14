#include "Maturation.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../brain/Brain.hpp"

namespace nlm;

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    // Experience-dependent maturation parameters
    float experienceMaturationFactor;      // Learning experience scaling
    float environmentalMaturationBoost;     // Environmental interaction boost
    float neuromodulationMaturation;        // Neuromodulatory influence
    float attentionMaturationModulator;     // Attention-based modulation
    
    // Critical period reopening dynamics
    float criticalPeriodReopeningStrength;  // Ability to reopen critical period
    float predictionDrivenMaturation;       // Prediction error driven maturation
    
    // Maturation state
    float developmentalAge;                // Current developmental age
    float lastMaturationUpdate;             // Time of last maturation update
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , experienceMaturationFactor(1.0f)
        , environmentalMaturationBoost(1.0f)
        , neuromodulationMaturation(0.0f)
        , attentionMaturationModulator(1.0f)
        , criticalPeriodReopeningStrength(0.3f)
        , predictionDrivenMaturation(0.0f)
        , developmentalAge(0.0f)
        , lastMaturationUpdate(0.0f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    TimestepDuration dt = 0.001;  // Assuming 1ms timestep
    update(brain, *brain->getRandomGenerator(), dt);
}

void Maturation::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    pImpl->lastMaturationUpdate += dt;
    pImpl->developmentalAge += dt;
    
    // Calculate experience-dependent maturation rate
    float maturationRate = calculateMaturationRate(brain);
    
    // Update progress based on maturation rate
    float progressIncrement = maturationRate * dt * calculateMaturationModulator(brain);
    pImpl->progress = std::min(pImpl->progress + progressIncrement, 1.0f);
    
    // Update mature neuron properties based on progress
    updateMatureProperties();
    
    // Check for critical period reopening
    checkCriticalPeriodReopening(brain);
    
    // Apply neuromodulatory influences
    integrateNeuromodulation(brain);
    
    // Update based on prediction accuracy if available
    updateFromPrediction(brain);
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

void Maturation::updateMatureProperties() {
    // Linear interpolation between immature and mature properties
    float matureThreshold = -45.0f;  // More positive than immature (-55mV)
    float matureRestingPotential = -65.0f;  // Less negative than immature (-70mV)
    float matureTimeConstant = 15.0f;  // Faster time constant
    
    pImpl->matureThreshold = -55.0f + (matureThreshold - (-55.0f)) * pImpl->progress;
    pImpl->matureRestingPotential = -70.0f + (matureRestingPotential - (-70.0f)) * pImpl->progress;
    pImpl->matureTimeConstant = 20.0f + (matureTimeConstant - 20.0f) * pImpl->progress;
}

void Maturation::checkCriticalPeriodReopening(Brain* brain) {
    if (!brain) return;
    
    // Critical period can reopen with strong novelty or prediction errors
    float reopeningTrigger = pImpl->criticalPeriodReopeningStrength * 
                            (1.0f - pImpl->progress) *  // More likely early in development
                            (1.0f + pImpl->predictionDrivenMaturation);  // Boosted by prediction error
    
    // Small probability of reopening critical period
    if (rng.uniformReal(0.0f, 1.0f) < reopeningTrigger * 0.01f) {
        // Critical period reopening allows for additional structural changes
        // This would enable synaptic refinement even after critical period
        NLM_LOG_INFO("Critical period reopening detected during maturation");
        pImpl->criticalPeriodReopeningStrength *= 0.9f;  // Diminishing returns
    }
}

void Maturation::integrateNeuromodulation(Brain* brain) {
    // Get neuromodulatory levels if available
    if (brain) {
        // Integrate dopamine for reinforcement-driven maturation
        if (auto dopamine = brain->getDopamine()) {
            float dopamineLevel = dopamine->getLevel();
            pImpl->neuromodulationMaturation = std::min(dopamineLevel, 1.0f);
            
            // Dopamine enhances maturation
            pImpl->experienceMaturationFactor *= (1.0f + dopamineLevel * 0.2f);
        }
        
        // Integrate curiosity for exploration-driven development
        if (auto curiosity = brain->getCuriosity()) {
            float curiosityLevel = curiosity->getLevel();
            pImpl->attentionMaturationModulator *= (1.0f + curiosityLevel * 0.1f);
        }
    }
}

void Maturation::updateFromPrediction(Brain* brain) {
    if (!brain) return;
    
    // Get prediction system for error/accuracy
    if (auto predictionSystem = brain->getPredictionSystem()) {
        float predictionError = predictionSystem->getPredictionError();
        float predictionAccuracy = 1.0f - predictionError;
        
        // High prediction error accelerates maturation (learning)
        if (predictionError > 0.5f) {
            pImpl->predictionDrivenMaturation = std::min(1.0f, 
                                                        pImpl->predictionDrivenMaturation + 
                                                        (predictionError - 0.5f) * 0.1f);
        }
        
        // High prediction accuracy stabilizes maturation
        if (predictionAccuracy > 0.9f) {
            pImpl->predictionDrivenMaturation *= 0.9f;
        }
        
        // Update maturation based on prediction
        pImpl->predictionDrivenMaturation *= predictionAccuracy * 0.5f;
    }
}

float Maturation::calculateMaturationRate(Brain* brain) {
    // Base maturation rate
    float baseRate = 0.001f;  // Slow baseline maturation
    
    // Scale by experience
    float experienceMultiplier = pImpl->experienceMaturationFactor;
    
    // Scale by environmental interaction
    float environmentMultiplier = pImpl->environmentalMaturationBoost;
    
    // Scale by neuromodulation
    float neuromodulationMultiplier = 1.0f + pImpl->neuromodulationMaturation * 0.5f;
    
    // Scale by attention modulation
    float attentionMultiplier = pImpl->attentionMaturationModulator;
    
    // Scale by prediction-driven maturation
    float predictionMultiplier = 1.0f + pImpl->predictionDrivenMaturation;
    
    float rate = baseRate * experienceMultiplier * environmentMultiplier * 
                 neuromodulationMultiplier * attentionMultiplier * predictionMultiplier;
    
    return rate;
}

float Maturation::calculateMaturationModulator(Brain* brain) {
    // Modulate maturation based on current developmental stage
    // This would integrate with DevelopmentSystem if available
    
    float modulator = 1.0f;
    
    // Higher maturation rate during critical period
    if (pImpl->progress < 0.3f) {
        modulator *= 1.5f;
    }
    
    // Enhanced maturation with strong environmental input
    if (brain && brain->getTotalSpikeCount() > 100) {
        float spikeRate = brain->getTotalSpikeCount() / (pImpl->developmentalAge + 0.001f);
        modulator *= std::min(2.0f, 1.0f + spikeRate * 0.001f);
    }
    
    return modulator;
}

} // namespace nlm
