#include "EnhancedSynaptogenesis.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/RandomGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>

namespace nlm {

struct EnhancedSynaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    // Enhanced parameters
    float activityThreshold;          // Activity threshold for formation
    float formationRateFactor;         // Rate modulation factor
    bool homeostaticRegulation;       // Enable homeostatic regulation
    
    // Developmental stage
    DevelopmentalStage developmentalStage;
    
    // Synaptogenesis type
    SynaptogenesisType synaptogenesisType;
    
    // State tracking
    std::map<NeuronId, float> neuronActivities;  // Track neuron activities
    float currentDensity;                         // Current synapse density
    float homeostaticTarget;                      // Homeostatic target
    
    Impl() : formationRate(0.001f), targetDensity(0.1f),
             activityThreshold(0.5f), formationRateFactor(1.0f),
             homeostaticRegulation(true), 
             developmentalStage(DevelopmentalStage::EMBRYONIC),
             synaptogenesisType(ACTIVITY_DEPENDENT),
             currentDensity(0.0f), homeostaticTarget(0.1f) {}
};

EnhancedSynaptogenesis::EnhancedSynaptogenesis() : pImpl(new Impl) {}

EnhancedSynaptogenesis::~EnhancedSynaptogenesis() = default;

void EnhancedSynaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) {
        return;
    }
    
    // Update neuron activities from brain
    updateNeuronActivities(brain);
    
    // Apply developmental stage modulation
    applyDevelopmentalModulation();
    
    // Apply homeostatic regulation
    applyHomeostaticRegulation(brain);
    
    // Create new synapses based on the selected type
    createNewSynapses(brain, rng);
}

void EnhancedSynaptogenesis::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float EnhancedSynaptogenesis::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

void EnhancedSynaptogenesis::setFormationRateFactor(float factor) {
    pImpl->formationRateFactor = std::max(0.0f, factor);
}

float EnhancedSynaptogenesis::getFormationRateFactor() const {
    return pImpl->formationRateFactor;
}

void EnhancedSynaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.0f, 1.0f);
}

float EnhancedSynaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void EnhancedSynaptogenesis::setHomeostaticRegulation(bool enabled) {
    pImpl->homeostaticRegulation = enabled;
}

bool EnhancedSynaptogenesis::isHomeostaticRegulationEnabled() const {
    return pImpl->homeostaticRegulation;
}

void EnhancedSynaptogenesis::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

DevelopmentalStage EnhancedSynaptogenesis::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void EnhancedSynaptogenesis::setSynaptogenesisType(SynaptogenesisType type) {
    pImpl->synaptogenesisType = type;
}

EnhancedSynaptogenesis::SynaptogenesisType EnhancedSynaptogenesis::getSynaptogenesisType() const {
    return pImpl->synaptogenesisType;
}

void EnhancedSynaptogenesis::updateNeuronActivities(Brain* brain) {
    // Get all neurons from brain
    std::vector<NeuronId> neuronIds = brain->getAllNeuronIds();
    
    for (NeuronId id : neuronIds) {
        // Get neuron activity (simplified: just get spike count)
        float activity = brain->getNeuronActivity(id);
        pImpl->neuronActivities[id] = activity;
    }
}

void EnhancedSynaptogenesis::applyDevelopmentalModulation() {
    switch (pImpl->developmentalStage) {
        case DevelopmentalStage::EMBRYONIC:
            // High formation rate, guided by cues
            pImpl->formationRateFactor *= 2.0f;
            break;
            
        case DevelopmentalStage::MATURATION:
            // Moderate formation, activity-dependent
            pImpl->formationRateFactor *= 1.0f;
            break;
            
        case DevelopmentalStage::ADULT:
            // Low formation rate, homeostatic control
            pImpl->formationRateFactor *= 0.5f;
            break;
            
        default:
            break;
    }
}

void EnhancedSynaptogenesis::applyHomeostaticRegulation(Brain* brain) {
    if (!pImpl->homeostaticRegulation) {
        return;
    }
    
    // Get brain statistics for density calculation
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    // Calculate current density
    pImpl->currentDensity = (totalNeurons > 0) ? 
        static_cast<float>(totalSynapses) / static_cast<float>(totalNeurons) : 0.0f;
    
    // Homeostatic target based on developmental stage
    switch (pImpl->developmentalStage) {
        case DevelopmentalStage::EMBRYONIC:
            pImpl->homeostaticTarget = 0.2f;
            break;
        case DevelopmentalStage::MATURATION:
            pImpl->homeostaticTarget = 0.15f;
            break;
        case DevelopmentalStage::ADULT:
            pImpl->homeostaticTarget = 0.1f;
            break;
        default:
            pImpl->homeostaticTarget = pImpl->targetDensity;
    }
    
    // Adjust formation rate to reach homeostatic target
    float densityError = pImpl->homeostaticTarget - pImpl->currentDensity;
    pImpl->formationRateFactor *= (1.0f + 0.1f * densityError);
}

void EnhancedSynaptogenesis::createNewSynapses(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Get all regions to iterate through neurons
    const auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        // Get neurons from this region
        const auto& neurons = region->getNeurons();
        
        for (const auto& neuron : neurons) {
            if (!neuron) continue;
            
            NeuronId pre = neuron->getId();
            
            // Consider other neurons as potential postsynaptic targets
            for (const auto& postNeuron : neurons) {
                if (!postNeuron) continue;
                
                NeuronId post = postNeuron->getId();
                
                // Skip self-connections
                if (pre == post) continue;
                
                // Skip if synapse already exists
                if (brain->getStructuralPlasticity()->getSynapse(pre, post)) {
                    continue;
                }
                
                // Calculate formation probability based on type
                float probability = calculateFormationProbability(
                    brain, pre, post, pImpl->neuronActivities, pImpl->synaptogenesisType);
                
                // Apply developmental stage modulation
                probability *= pImpl->formationRateFactor;
                
                // Check if synapse should form
                if (rng.nextFloat() < probability) {
                    // Create synapse with appropriate initial weight
                    SynapticWeight initialWeight = calculateInitialWeight(
                        brain, pre, post, pImpl->synaptogenesisType);
                    
                    // Try to create the synapse
                    brain->getStructuralPlasticity()->createSynapse(pre, post, initialWeight);
                }
            }
        }
    }
}

float EnhancedSynaptogenesis::calculateFormationProbability(
    Brain* brain,
    NeuronId pre,
    NeuronId post,
    const std::map<NeuronId, float>& activities,
    SynaptogenesisType type) {
    
    float preActivity = activities.count(pre) ? activities.at(pre) : 0.0f;
    float postActivity = activities.count(post) ? activities.at(post) : 0.0f;
    
    switch (type) {
        case ACTIVITY_DEPENDENT:
            // Forms based on joint activity
            return (preActivity * postActivity) * pImpl->formationRate;
            
        case HETEROCHROMATIC:
            // Forms based on proximity in activity space
            float activityDistance = std::abs(preActivity - postActivity);
            return std::exp(-activityDistance) * pImpl->formationRate;
            
        case MODULAR:
            // For now, use basic probability for modular connections
            return pImpl->formationRate * 1.0f;
            
        case EMEBRYONIC_GUIDED:
            // Forms based on developmental cues
            return pImpl->formationRate * 1.5f; // Higher for embryonic
            
        case EXPERIENCE_DEPENDENT:
            // Forms based on specific spike timing patterns
            // For now, use activity-based probability
            return (preActivity * postActivity) * pImpl->formationRate * 0.5f;
            
        default:
            return pImpl->formationRate;
    }
}

SynapticWeight EnhancedSynaptogenesis::calculateInitialWeight(
    Brain* brain,
    NeuronId pre,
    NeuronId post,
    SynaptogenesisType type) {
    
    switch (type) {
        case ACTIVITY_DEPENDENT:
            // Weight proportional to average activity
            float preActivity = 0.5f; // Simplified
            float postActivity = 0.5f; // Simplified
            return (preActivity + postActivity) / 2.0f;
            
        case HETEROCHROMATIC:
            // Weight based on similarity
            float preActivity = 0.5f; // Simplified
            float postActivity = 0.5f; // Simplified
            return std::exp(-std::abs(preActivity - postActivity));
            
        case MODULAR:
            return 0.5f; // Standard strength for modular connections
            
        case EMEBRYONIC_GUIDED:
            return 0.3f; // Weaker initial embryonic connections
            
        case EXPERIENCE_DEPENDENT:
            // Weight based on recent experience
            return 0.4f; // Simplified
            
        default:
            return 0.5f;
    }
}

} // namespace nlm
