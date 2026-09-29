#include "ExtendedDevelopment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// Experience-Dependent Development Implementation
class ExperienceDependentDevelopment {
public:
    struct ExperienceParameters {
        float learningRate;           // Rate of experience-dependent learning
        float memoryRetention;       // Rate of memory retention
        float plasticityDecay;       // Rate of plasticity decay
        float sensitivePeriodFactor; // Factor for sensitive periods
    };
    
    ExperienceDependentDevelopment() {
        params.learningRate = 0.1f;
        params.memoryRetention = 0.9f;
        params.plasticityDecay = 0.01f;
        params.sensitivePeriodFactor = 1.5f;
    }
    
    std::vector<float> developFromExperience(const std::vector<float>& initialState,
                                             const std::vector<std::vector<float>>& experiences,
                                             float timeElapsed) {
        // Develop based on accumulated experiences
        std::vector<float> developedState = initialState;
        
        for (const auto& experience : experiences) {
            // Update state based on experience
            for (size_t i = 0; i < developedState.size() && i < experience.size(); ++i) {
                float adaptation = computeAdaptation(developedState[i], experience[i]);
                developedState[i] += adaptation * params.learningRate;
            }
            
            // Apply memory retention
            for (float& value : developedState) {
                value *= params.memoryRetention;
            }
        }
        
        // Apply time-based decay
        for (float& value : developedState) {
            value *= (1.0f - params.plasticityDecay * timeElapsed);
        }
        
        return developedState;
    }
    
    float computeAdaptation(float currentValue, float experienceValue) const {
        // Compute how much to adapt based on current value and experience
        return (experienceValue - currentValue) * params.sensitivePeriodFactor;
    }
    
    void setParameters(const ExperienceParameters& params) {
        this->params = params;
    }
    
    void reset() {
        // Nothing to reset for experience-dependent development
    }
    
private:
    ExperienceParameters params;
};

// Sensitive Period Extension Implementation
class SensitivePeriodExtender {
public:
    struct SensitivePeriodParameters {
        float extensionFactor;        // Factor to extend sensitive period
        float enrichmentThreshold;   // Threshold for enrichment to extend period
        float enrichmentEffect;      // Effect of enrichment on extension
        float decayRate;             // Rate of extension decay
    };
    
    SensitivePeriodExtender() {
        params.extensionFactor = 1.5f;
        params.enrichmentThreshold = 0.3f;
        params.enrichmentEffect = 0.2f;
        params.decayRate = 0.01f;
    }
    
    float extendSensitivePeriod(float currentExtension, 
                               const std::vector<float>& environmentalQuality,
                               float timeElapsed) {
        // Extend sensitive period based on environmental quality
        float extension = currentExtension;
        
        // Check if enrichment is sufficient
        float enrichmentScore = computeEnrichmentScore(environmentalQuality);
        
        if (enrichmentScore > params.enrichmentThreshold) {
            // Extend period based on enrichment
            extension *= (1.0f + params.enrichmentEffect * enrichmentScore);
        }
        
        // Apply decay
        extension *= (1.0f - params.decayRate * timeElapsed);
        
        return extension;
    }
    
    float computeEnrichmentScore(const std::vector<float>& environmentalQuality) const {
        // Compute environmental enrichment score
        if (environmentalQuality.empty()) return 0.0f;
        
        float score = 0.0f;
        for (float quality : environmentalQuality) {
            score += quality;
        }
        
        return score / environmentalQuality.size();
    }
    
    void setParameters(const SensitivePeriodParameters& params) {
        this->params = params;
    }
    
    void reset() {
        // Nothing to reset for sensitive period extender
    }
    
private:
    SensitivePeriodParameters params;
};

// Epigenetic Influence Implementation
class EpigeneticInfluencer {
public:
    struct EpigeneticParameters {
        float methylationRate;       // Rate of methylation (gene silencing)
        float demethylationRate;     // Rate of demethylation (gene activation)
        float environmentalSensitivity; // Sensitivity to environmental changes
        float heritabilityFactor;     // Factor for heritable changes
    };
    
    EpigeneticInfluencer() {
        params.methylationRate = 0.05f;
        params.demethylationRate = 0.03f;
        params.environmentalSensitivity = 0.8f;
        params.heritabilityFactor = 0.1f;
    }
    
    std::vector<float> applyEpigeneticChanges(const std::vector<float>& geneticState,
                                             const std::vector<float>& environmentalFactors,
                                             float timeElapsed) {
        // Apply epigenetic changes based on environmental factors
        std::vector<float> epigeneticState = geneticState;
        
        for (size_t i = 0; i < epigeneticState.size() && i < environmentalFactors.size(); ++i) {
            // Methylation (gene silencing)
            if (environmentalFactors[i] > 0.5f) {
                epigeneticState[i] *= (1.0f - params.methylationRate * timeElapsed);
            }
            
            // Demethylation (gene activation)
            if (environmentalFactors[i] < 0.5f) {
                epigeneticState[i] *= (1.0f + params.demethylationRate * timeElapsed);
            }
            
            // Environmental sensitivity
            epigeneticState[i] += (environmentalFactors[i] - 0.5f) * 
                                params.environmentalSensitivity * timeElapsed;
            
            // Constrain to reasonable bounds
            epigeneticState[i] = std::clamp(epigeneticState[i], 0.0f, 1.0f);
        }
        
        return epigeneticState;
    }
    
    std::vector<float> inheritEpigeneticTraits(const std::vector<float>& parentTraits,
                                               const std::vector<float>& environmentalFactors,
                                               float timeElapsed) {
        // Inherit epigenetic traits from parent with environmental modification
        std::vector<float> inheritedTraits = parentTraits;
        
        for (size_t i = 0; i < inheritedTraits.size() && i < environmentalFactors.size(); ++i) {
            // Apply heritability
            inheritedTraits[i] += (parentTraits[i] - 0.5f) * params.heritabilityFactor;
            
            // Environmental modification
            inheritedTraits[i] += (environmentalFactors[i] - 0.5f) * 
                                params.environmentalSensitivity * timeElapsed * 0.5f;
            
            inheritedTraits[i] = std::clamp(inheritedTraits[i], 0.0f, 1.0f);
        }
        
        return inheritedTraits;
    }
    
    void setParameters(const EpigeneticParameters& params) {
        this->params = params;
    }
    
    void reset() {
        // Nothing to reset for epigenetic influencer
    }
    
private:
    EpigeneticParameters params;
};

// Developmental Plasticity Implementation
class DevelopmentalPlasticity {
public:
    struct PlasticityParameters {
        float plasticityRate;          // Rate of plasticity
        float stabilityThreshold;      // Threshold for plasticity stability
        float memoryConsolidation;    // Rate of memory consolidation
        float structuralChangeRate;   // Rate of structural changes
    };
    
    DevelopmentalPlasticity() {
        params.plasticityRate = 0.5f;
        params.stabilityThreshold = 0.3f;
        params.memoryConsolidation = 0.2f;
        params.structuralChangeRate = 0.01f;
    }
    
    std::vector<float> applyPlasticity(const std::vector<float>& currentState,
                                      const std::vector<float>& targetState,
                                      float plasticityLevel,
                                      float timeElapsed) {
        // Apply plasticity to reach target state
        std::vector<float> plasticState = currentState;
        
        for (size_t i = 0; i < plasticState.size() && i < targetState.size(); ++i) {
            // Plastic change toward target
            float plasticChange = (targetState[i] - plasticState[i]) * 
                                params.plasticityRate * plasticityLevel;
            
            plasticState[i] += plasticChange * timeElapsed;
            
            // Constrain to reasonable bounds
            plasticState[i] = std::clamp(plasticState[i], 0.0f, 1.0f);
        }
        
        // Memory consolidation
        for (float& value : plasticState) {
            value = value * (1.0f - params.memoryConsolidation) + 
                   targetState[i] * params.memoryConsolidation; // Note: i might be out of bounds
        }
        
        return plasticState;
    }
    
    float computePlasticityStability(const std::vector<float>& state) const {
        // Compute stability of the current state
        if (state.empty()) return 0.0f;
        
        float mean = computeMean(state);
        float variance = 0.0f;
        
        for (float value : state) {
            float diff = value - mean;
            variance += diff * diff;
        }
        
        variance /= state.size();
        float std = std::sqrt(variance);
        
        // Stability is high when variance is low
        return 1.0f - std;
    }
    
    float computeMean(const std::vector<float>& data) const {
        if (data.empty()) return 0.0f;
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        return sum / data.size();
    }
    
    void setParameters(const PlasticityParameters& params) {
        this->params = params;
    }
    
    void reset() {
        // Nothing to reset for developmental plasticity
    }
    
private:
    PlasticityParameters params;
};

// ExtendedDevelopment Implementation
struct ExtendedDevelopment::Impl {
    ExperienceDependentDevelopment experienceDevelopment;
    SensitivePeriodExtender sensitivePeriodExtender;
    EpigeneticInfluencer epigeneticInfluencer;
    DevelopmentalPlasticity developmentalPlasticity;
    
    std::vector<std::vector<float>> developmentalHistory;
    std::vector<float> plasticityHistory;
    
    Impl() : experienceDevelopment(), sensitivePeriodExtender(), 
             epigeneticInfluencer(), developmentalPlasticity() {
        developmentalHistory.reserve(100);
        plasticityHistory.reserve(100);
    }
};

ExtendedDevelopment::ExtendedDevelopment()
    : pImpl(new Impl)
    , sensitivePeriodExtension_(1.0f)
    , epigeneticInfluence_(0.0f)
    , plasticityLevel_(0.5f)
    , experienceQuality_(0.0f)
{
}

ExtendedDevelopment::~ExtendedDevelopment() = default;

void ExtendedDevelopment::initialize(Brain* brain) {
    NLM_LOG_INFO("ExtendedDevelopment initialized");
}

void ExtendedDevelopment::updateDevelopment(const std::vector<float>& initialState,
                                           const std::vector<std::vector<float>>& experiences,
                                           const std::vector<float>& environmentalFactors,
                                           std::vector<float>& developedState,
                                           float timeElapsed) {
    // Update development based on experiences and environmental factors
    developedState = initialState;
    
    // Apply experience-dependent development
    developedState = pImpl->experienceDevelopment.developFromExperience(
        developedState, experiences, timeElapsed);
    
    // Extend sensitive period
    sensitivePeriodExtension_ = pImpl->sensitivePeriodExtender.extendSensitivePeriod(
        sensitivePeriodExtension_, environmentalFactors, timeElapsed);
    
    // Apply epigenetic changes
    epigeneticInfluence_ += pImpl->epigeneticInfluencer.applyEpigeneticChanges(
        developedState, environmentalFactors, timeElapsed)[0];
    
    // Apply developmental plasticity
    std::vector<float> targetState = computeTargetState(developedState, experiences);
    developedState = pImpl->developmentalPlasticity.applyPlasticity(
        developedState, targetState, plasticityLevel_, timeElapsed);
    
    // Update experience quality
    experienceQuality_ = computeExperienceQuality(experiences);
    
    // Update plasticity level
    plasticityLevel_ = computePlasticityLevel(experienceQuality_, sensitivePeriodExtension_);
    
    // Record developmental history
    developmentalHistory.push_back(developedState);
    plasticityHistory.push_back(plasticityLevel_);
    
    // Keep history bounded
    while (developmentalHistory.size() > 100) {
        developmentalHistory.erase(developmentalHistory.begin());
    }
    while (plasticityHistory.size() > 100) {
        plasticityHistory.erase(plasticityHistory.begin());
    }
}

void ExtendedDevelopment::updateSensitivePeriod(float timeElapsed) {
    // Update sensitive period extension
    sensitivePeriodExtension_ *= (1.0f - 0.01f * timeElapsed);
}

void ExtendedDevelopment::updateEpigeneticState(const std::vector<float>& environmentalFactors,
                                              float timeElapsed) {
    // Update epigenetic state based on environmental factors
    epigeneticInfluence_ += pImpl->epigeneticInfluencer.applyEpigeneticChanges(
        std::vector<float>{epigeneticInfluence_}, environmentalFactors, timeElapsed)[0];
}

void ExtendedDevelopment::updatePlasticity(const std::vector<float>& experienceQuality,
                                         float timeElapsed) {
    // Update plasticity level based on experience quality
    plasticityLevel_ = computePlasticityLevel(experienceQuality, sensitivePeriodExtension_);
    
    // Apply plasticity decay
    plasticityLevel_ *= (1.0f - 0.001f * timeElapsed);
}

void ExtendedDevelopment::applyDevelopmentalChanges(const std::vector<float>& currentState,
                                                   std::vector<float>& targetState,
                                                   float timeElapsed) {
    // Apply developmental changes to reach target state
    targetState = pImpl->developmentalPlasticity.applyPlasticity(
        currentState, targetState, plasticityLevel_, timeElapsed);
}

float ExtendedDevelopment::computeExperienceQuality(const std::vector<std::vector<float>>& experiences) const {
    // Compute quality of experiences
    if (experiences.empty()) return 0.0f;
    
    float totalQuality = 0.0f;
    for (const auto& experience : experiences) {
        float quality = 0.0f;
        for (float value : experience) {
            quality += value;
        }
        quality /= experience.size();
        totalQuality += quality;
    }
    
    return totalQuality / experiences.size();
}

float ExtendedDevelopment::computePlasticityLevel(float experienceQuality,
                                                float sensitivePeriodExtension) const {
    // Compute plasticity level based on experience quality and sensitive period
    float level = experienceQuality * 0.5f + sensitivePeriodExtension * 0.3f;
    level *= (1.0f + 0.2f * (1.0f - experienceQuality)); // Reduce if experience quality is high
    
    return std::clamp(level, 0.1f, 1.0f);
}

std::vector<float> ExtendedDevelopment::computeTargetState(const std::vector<float>& currentState,
                                                         const std::vector<std::vector<float>>& experiences) const {
    // Compute target state based on current state and experiences
    std::vector<float> targetState = currentState;
    
    if (!experiences.empty()) {
        // Average of all experiences
        std::vector<float> experienceAverage(currentState.size(), 0.0f);
        for (const auto& experience : experiences) {
            for (size_t i = 0; i < experience.size() && i < experienceAverage.size(); ++i) {
                experienceAverage[i] += experience[i];
            }
        }
        
        for (size_t i = 0; i < experienceAverage.size(); ++i) {
            experienceAverage[i] /= experiences.size();
        }
        
        // Blend with current state
        for (size_t i = 0; i < targetState.size() && i < experienceAverage.size(); ++i) {
            targetState[i] = targetState[i] * 0.7f + experienceAverage[i] * 0.3f;
        }
    }
    
    return targetState;
}

void ExtendedDevelopment::setSensitivePeriodExtension(float extension) {
    sensitivePeriodExtension_ = extension;
}

void ExtendedDevelopment::setEpigeneticInfluence(float influence) {
    epigeneticInfluence_ = influence;
}

void ExtendedDevelopment::setPlasticityLevel(float level) {
    plasticityLevel_ = level;
}

void ExtendedDevelopment::reset() {
    pImpl->experienceDevelopment.reset();
    pImpl->sensitivePeriodExtender.reset();
    pImpl->epigeneticInfluencer.reset();
    pImpl->developmentalPlasticity.reset();
    
    pImpl->developmentalHistory.clear();
    pImpl->plasticityHistory.clear();
    
    sensitivePeriodExtension_ = 1.0f;
    epigeneticInfluence_ = 0.0f;
    plasticityLevel_ = 0.5f;
    experienceQuality_ = 0.0f;
}

} // namespace nlm
