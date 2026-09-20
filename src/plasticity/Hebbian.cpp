#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

    // Calculate NMDA receptor activation factor
    // NMDA receptors require both pre- and post-synaptic activity
    float nmdaFactor = calculateNMDAActivation(preSpikes, postSpikes, dt);
    
    // Calculate covariance-based weight change
    float covarianceDelta = calculateCovarianceChange(preSpikes, postSpikes, dt);
    
    // Apply metaplasticity - adapt learning rate based on synaptic history
    float adaptiveLearningRate = pImpl->learningRate * calculateMetaplasticityFactor(synapse);
    
    // Experience-dependent plasticity modulation
    float experienceFactor = calculateExperienceFactor(synapse);
    
    // Combined Hebbian weight change
    float delta = adaptiveLearningRate * covarianceDelta * nmdaFactor * experienceFactor;
    
    // Apply synaptic tagging for memory consolidation
    tagForConsolidation(synapse, delta);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::calculateFunctionalRemodeling(Synapse* synapse,
                                           const std::vector<Timestamp>& preSpikes,
                                           const std::vector<Timestamp>& postSpikes,
                                           float& remappingStrength) const {
    // Calculate functional neuroplasticity - cortical remapping and map expansion
    // Implements mechanisms for functional reorganization of brain areas
    
    if (!synapse) {
        remappingStrength = 0.0f;
        return;
    }
    
    // Homologous area adaptation - connections can shift to equivalent functions
    // When one pathway degrades, connections can rewire to homologous areas
    float homologousAdaptation = calculateHomologousAdaptation(synapse, preSpikes, postSpikes);
    
    // Map expansion - increased representation of frequently used pathways
    float mapExpansion = calculateMapExpansion(synapse, preSpikes, postSpikes);
    
    // Compensatory masquerade - different pathways perform similar functions
    float compensatoryMasquerade = calculateCompensatoryMasquerade(synapse, preSpikes, postSpikes);
    
    // Combine functional mechanisms with weights
    float functionalWeight = 0.4f;  // Homologous adaptation
    float expansionWeight = 0.4f;   // Map expansion
    float masqueradeWeight = 0.2f;   // Compensatory masquerade
    
    remappingStrength = (homologousAdaptation * functionalWeight +
                        mapExpansion * expansionWeight +
                        compensatoryMasquerade * masqueradeWeight);
}

float Hebbian::calculateHomologousAdaptation(Synapse* synapse,
                                            const std::vector<Timestamp>& preSpikes,
                                            const std::vector<Timestamp>& postSpikes) const {
    // Homologous area adaptation - shifting connections to equivalent brain regions
    // Modeled after studies showing cross-hemispheric compensation
    
    if (!synapse) return 0.0f;
    
    // Calculate correlation strength of pre/post spike pairs
    float correlationStrength = 0.0f;
    float timeWindow = 100.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) <= timeWindow) {
                correlationStrength += 1.0f / (1.0f + std::abs(dt) / 50.0f);
            }
        }
    }
    
    // Normalize
    correlationStrength /= static_cast<float>(preSpikes.size() * postSpikes.size());
    
    // Homologous adaptation is stronger when correlation is high
    // Represents brain's ability to use mirror regions when primary regions are damaged
    return std::min(1.0f, correlationStrength * 2.0f);
}

float Hebbian::calculateMapExpansion(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes) const {
    // Map expansion - increased representation of frequently used sensory/motor pathways
    // Observed in cortical remapping after injury or skill acquisition
    
    if (!synapse) return 0.0f;
    
    // Calculate usage frequency
    float usageFrequency = static_cast<float>(preSpikes.size() + postSpikes.size()) / 100.0f;
    
    // Map expansion is higher for frequently used pathways
    // Implemented as adaptive synaptic strengthening proportional to usage
    float expansionFactor = std::min(1.0f, usageFrequency);
    
    // More recent spikes contribute more to map expansion
    float recentActivity = calculateRecentActivity(preSpikes, postSpikes);
    
    return (expansionFactor + recentActivity) * 0.5f;
}

float Hebbian::calculateCompensatoryMasquerade(Synapse* synapse,
                                               const std::vector<Timestamp>& preSpikes,
                                               const std::vector<Timestamp>& postSpikes) const {
    // Compensatory masquerade - different neural pathways performing similar functions
    // When primary pathway is compromised, alternative pathways take over
    
    if (!synapse) return 0.0f;
    
    // Detect if there are alternative pathways that could compensate
    // In this simplified model, we look for correlated activity with timing patterns
    // that suggest alternative functional pathways
    
    float alternativePathwayStrength = 0.0f;
    
    // Check for spikes with different timing patterns that could indicate alternative pathways
    float normalTimingCorrelations = 0.0f;
    float alternativeTimingCorrelations = 0.0f;
    
    float normalWindow = 50.0f;   // Normal spike timing window
    float altWindow = 150.0f;     // Alternative spike timing window (wider)
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (std::abs(dt) <= normalWindow) {
                normalTimingCorrelations++;
            } else if (std::abs(dt) <= altWindow) {
                alternativeTimingCorrelations++;
            }
        }
    }
    
    // Compensatory masquerade strength depends on alternative pathway availability
    if (normalTimingCorrelations > 0) {
        alternativePathwayStrength = alternativeTimingCorrelations / normalTimingCorrelations;
    }
    
    return std::min(1.0f, alternativePathwayStrength);
}

float Hebbian::calculateRecentActivity(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes) const {
    // Calculate more recent activity contribution
    
    if (preSpikes.empty() && postSpikes.empty()) return 0.0f;
    
    float recentWeight = 0.0f;
    float maxTimeWindow = 100.0f;  // ms
    
    // Get most recent spike time
    Timestamp maxTime = 0;
    for (const auto& t : preSpikes) {
        if (t > maxTime) maxTime = t;
    }
    for (const auto& t : postSpikes) {
        if (t > maxTime) maxTime = t;
    }
    
    // Weight by recency
    for (Timestamp preTime : preSpikes) {
        float age = static_cast<float>(maxTime - preTime);
        if (age <= maxTimeWindow) {
            recentWeight += 1.0f - (age / maxTimeWindow);
        }
    }
    for (Timestamp postTime : postSpikes) {
        float age = static_cast<float>(maxTime - postTime);
        if (age <= maxTimeWindow) {
            recentWeight += 1.0f - (age / maxTimeWindow);
        }
    }
    
    // Normalize by number of spikes
    float totalSpikes = static_cast<float>(preSpikes.size() + postSpikes.size());
    if (totalSpikes > 0) {
        recentWeight /= totalSpikes;
    }
    
    return recentWeight;
}

} // namespace nlm

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
