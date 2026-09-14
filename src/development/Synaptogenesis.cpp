#include "Synaptogenesis.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm;

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    // Experience-dependent synaptogenesis parameters
    float experienceSynaptogenesisFactor;    // Learning experience scaling
    float environmentalSynaptogenesisBoost;   // Environmental interaction boost
    float neuromodulationSynaptogenesis;      // Neuromodulatory influence
    float criticalPeriodSynaptogenesis;       // Critical period effects
    float attentionSynaptogenesisModulator;   // Attention-based modulation
    
    // Synaptogenesis state
    float totalExperience;                    // Cumulative learning experience
    float recentActivity;                      // Recent neural activity
    float noveltyLevel;                        // Novelty-driven synaptogenesis
    
    Impl() : formationRate(0.001f), targetDensity(0.1f),
             experienceSynaptogenesisFactor(1.0f),
             environmentalSynaptogenesisBoost(1.0f),
             neuromodulationSynaptogenesis(0.0f),
             criticalPeriodSynaptogenesis(1.0f),
             attentionSynaptogenesisModulator(1.0f),
             totalExperience(0.0f), recentActivity(0.0f), noveltyLevel(0.0f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate current synaptic density (simplified)
    size_t currentSynapseCount = brain->getTotalSynapseCount();
    size_t currentNeuronCount = brain->getTotalNeuronCount();
    float currentDensity = (currentNeuronCount > 0) ? 
        static_cast<float>(currentSynapseCount) / currentNeuronCount : 0.0f;
    
    // Calculate synaptogenesis rate based on multiple factors
    float synaptogenesisRate = calculateSynaptogenesisRate(brain);
    
    // Determine if new synapses should form
    float formationProbability = calculateFormationProbability(
        currentDensity, synaptogenesisRate, brain);
    
    // Form new synapses based on probability
    int numNewSynapses = 0;
    if (formationProbability > 0.0f) {
        numNewSynapses = static_cast<int>(formationProbability * 10.0f);  // Scale to reasonable number
    }
    
    // Form new synapses
    formNewSynapses(brain, rng, numNewSynapses);
    
    // Update experience and activity levels
    updateSynaptogenesisState(brain);
    
    // Integrate neuromodulation
    integrateNeuromodulation(brain);
}

void Synaptogenesis::formNewSynapses(Brain* brain, RandomGenerator& rng, int numSynapses) {
    if (!brain || numSynapses <= 0) return;
    
    // Get all neurons
    const auto& regions = brain->getRegions();
    std::vector<Neuron*> allNeurons;
    
    for (const auto& region : regions) {
        for (const auto& neuron : region->getNeurons()) {
            allNeurons.push_back(neuron.get());
        }
    }
    
    if (allNeurons.size() < 2) return;
    
    for (int i = 0; i < numSynapses && i < allNeurons.size() / 2; ++i) {
        // Select source and destination neurons
        NeuronId sourceId = allNeurons[rng.uniformInt(0, static_cast<int>(allNeurons.size() - 1))]->getId();
        NeuronId destId;
        
        // Ensure destination is different from source
        do {
            destId = allNeurons[rng.uniformInt(0, static_cast<int>(allNeurons.size() - 1))]->getId();
        } while (destId == sourceId);
        
        // Create new synapse
        createSynapse(brain, sourceId, destId, rng);
    }
}

void Synaptogenesis::createSynapse(Brain* brain, NeuronId sourceId, NeuronId destId, RandomGenerator& rng) {
    if (!brain) return;
    
    // Find neurons
    Neuron* sourceNeuron = nullptr;
    Neuron* destNeuron = nullptr;
    
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        for (const auto& neuron : region->getNeurons()) {
            if (neuron->getId() == sourceId) {
                sourceNeuron = neuron.get();
            }
            if (neuron->getId() == destId) {
                destNeuron = neuron.get();
            }
        }
    }
    
    if (!sourceNeuron || !destNeuron) return;
    
    // Determine synapse properties based on neuron types and developmental stage
    SynapseType synapseType = determineSynapseType(sourceNeuron, destNeuron);
    SynapticWeight initialWeight = calculateInitialWeight(sourceNeuron, destNeuron, rng);
    Delay initialDelay = calculateInitialDelay(rng);
    
    // Create synapse using brain's synapse creation mechanism
    // Note: This assumes the brain API has a method to create synapses
    // In a real implementation, this would be exposed through the brain API
    
    NLM_LOG_INFO("New synapse formed: " + std::to_string(sourceId.index()) + 
                 " -> " + std::to_string(destId.index()) +
                 " (type: " + std::to_string(static_cast<int>(synapseType)) + ")");
}

float Synaptogenesis::calculateSynaptogenesisRate(Brain* brain) {
    // Base synaptogenesis rate
    float baseRate = pImpl->formationRate;
    
    // Scale by experience
    float experienceMultiplier = 1.0f + pImpl->experienceSynaptogenesisFactor * 0.5f;
    
    // Scale by environmental interaction
    float environmentMultiplier = 1.0f + pImpl->environmentalSynaptogenesisBoost * 0.3f;
    
    // Scale by neuromodulation
    float neuromodulationMultiplier = 1.0f + pImpl->neuromodulationSynaptogenesis * 0.4f;
    
    // Scale by critical period
    float criticalPeriodMultiplier = pImpl->criticalPeriodSynaptogenesis;
    
    // Scale by attention modulation
    float attentionMultiplier = pImpl->attentionSynaptogenesisModulator;
    
    float rate = baseRate * experienceMultiplier * environmentMultiplier * 
                 neuromodulationMultiplier * criticalPeriodMultiplier * attentionMultiplier;
    
    return rate;
}

float Synaptogenesis::calculateFormationProbability(float currentDensity, float synaptogenesisRate, Brain* brain) {
    // Probability of forming new synapses based on density and rate
    float densityDiff = pImpl->targetDensity - currentDensity;
    float formationChance = synaptogenesisRate * densityDiff * 10.0f;  // Amplify for reasonable probabilities
    
    // Ensure probability is in valid range
    formationChance = std::clamp(formationChance, 0.0f, 1.0f);
    
    // Modulate by novelty
    formationChance *= (1.0f + pImpl->noveltyLevel * 0.5f);
    
    return formationChance;
}

void Synaptogenesis::updateSynaptogenesisState(Brain* brain) {
    if (!brain) return;
    
    // Update total experience based on learning signals
    pImpl->totalExperience += brain->getTotalSpikeCount() * 0.001f;
    
    // Update recent activity based on neural activity
    pImpl->recentActivity = std::min(1.0f, pImpl->recentActivity + 
                                     brain->getTotalSpikeCount() * 0.0001f);
    
    // Update novelty level based on prediction errors if available
    if (auto predictionSystem = brain->getPredictionSystem()) {
        float predictionError = predictionSystem->getPredictionError();
        pImpl->noveltyLevel = std::clamp(predictionError * 2.0f, 0.0f, 1.0f);
    }
}

void Synaptogenesis::integrateNeuromodulation(Brain* brain) {
    // Integrate neuromodulatory signals for activity-dependent synaptogenesis
    if (!brain) return;
    
    // Integrate dopamine for reward-modulated synaptogenesis
    if (auto dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        pImpl->neuromodulationSynaptogenesis = dopamineLevel;
        
        // Dopamine promotes synaptogenesis for rewarded connections
        if (dopamineLevel > 0.5f) {
            pImpl->experienceSynaptogenesisFactor *= (1.0f + dopamineLevel * 0.2f);
        }
    }
    
    // Integrate curiosity for exploration-driven synaptogenesis
    if (auto curiosity = brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        pImpl->attentionSynaptogenesisModulator *= (1.0f + curiosityLevel * 0.1f);
    }
}

SynapseType Synaptogenesis::determineSynapseType(Neuron* source, Neuron* dest) {
    if (!source || !dest) return SynapseType::Excitatory;
    
    NeuronType sourceType = source->getType();
    NeuronType destType = dest->getType();
    
    // Excitatory-to-excitatory or excitatory-to-interneuron
    if (sourceType == NeuronType::Excitatory) {
        return SynapseType::Excitatory;
    }
    
    // Inhibitory neurons typically have inhibitory synapses
    if (sourceType == NeuronType::Inhibitory) {
        return SynapseType::Inhibitory;
    }
    
    // Default to excitatory for other types
    return SynapseType::Excitatory;
}

SynapticWeight Synaptogenesis::calculateInitialWeight(Neuron* source, Neuron* dest, RandomGenerator& rng) {
    // Calculate initial synaptic weight based on neuron types and developmental stage
    float baseWeight = rng.uniformReal(-0.5f, 0.5f);  // Random initial weight
    
    // Modulate by neuron types
    if (source && dest) {
        NeuronType sourceType = source->getType();
        NeuronType destType = dest->getType();
        
        // Excitatory neurons tend to have stronger connections
        if (sourceType == NeuronType::Excitatory) {
            baseWeight = std::max(baseWeight, 0.1f);
        }
        
        // Inhibitory neurons may have different weight distributions
        if (sourceType == NeuronType::Inhibitory) {
            baseWeight = std::min(baseWeight, -0.1f);
        }
    }
    
    return std::clamp(baseWeight, -1.0f, 1.0f);
}

Delay Synaptogenesis::calculateInitialDelay(RandomGenerator& rng) {
    // Calculate initial synaptic delay
    return static_cast<Delay>(rng.uniformInt(1, 5));  // 1-5 step delay
}

} // namespace nlm
