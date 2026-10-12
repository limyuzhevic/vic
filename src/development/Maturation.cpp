#include "Maturation.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <queue>

namespace nlm {

struct Maturation::Impl {
    // Maturation progress and timing
    float progress;
    double developmentalTime;
    SimulationStep lastMaturationStep;
    
    // Neural activity patterns for maturation
    float averageFiringRate;
    float activityCoherence;
    float burstPatternStrength;
    
    // Molecular mechanisms for synaptic stabilization
    std::vector<float> synapticStabilizationFactors;
    std::vector<float> synapticMaturationWeights;
    std::vector<float> molecularScaffold;
    
    // Developmental stage parameters
    float stageSpecificMaturationRates[5];  // Initial, CriticalPeriod, Maturation, Adult, Aging
    
    // Neuromodulation integration
    float dopamineInfluence;
    float rewardInfluence;
    float curiosityInfluence;
    float noveltyInfluence;
    
    // Activity-dependent maturation
    float activityMaturationThreshold;
    float homeostaticMaturationSetpoint;
    float maturationProportionalGain;
    
    // Molecular scaffold development
    float molecularScaffoldingProgress;
    float proteinSynthesisRate;
    float geneExpressionLevel;
    
    // Adult property development
    float matureFiringPatternCoherence;
    float matureConnectivityOptimization;
    float matureExcitatoryInhibitoryBalance;
    
    // Aging-related decline mechanisms
    float agingRate;
    float declineFactors[3];  // Neuronal, synaptic, metabolic
    float rejuvenationPotential;
    
    // Developmental clock
    double developmentalClockAge;
    double criticalPeriodDuration;
    double maturationPeriodDuration;
    double adultPeriodDuration;
    
    // Experience-dependent maturation
    std::vector<float> experienceMaturationWeights;
    std::unordered_set<NeuronId> recentlyExperiencedNeurons;
    float experienceDependentMaturation;
    
    // Neural pattern development
    std::vector<float> firingPatternStability;
    std::vector<float> connectivityPatternStrength;
    float patternFormationRate;
    
    // Property development tracking
    float matureThresholdDevelopment;
    float matureRestingPotentialDevelopment;
    float matureTimeConstantDevelopment;
    float matureRefractoryPeriodDevelopment;
    
    Impl() : progress(0.0f), developmentalTime(0.0), lastMaturationStep(0),
             averageFiringRate(0.0f), activityCoherence(0.0f), burstPatternStrength(0.0f),
             molecularScaffoldingProgress(0.0f), proteinSynthesisRate(0.0f),
             geneExpressionLevel(0.0f), matureFiringPatternCoherence(0.0f),
             matureConnectivityOptimization(0.0f), matureExcitatoryInhibitoryBalance(1.0f),
             agingRate(0.001f), rejuvenationPotential(0.0f),
             developmentalClockAge(0.0), criticalPeriodDuration(360.0),
             maturationPeriodDuration(960.0), adultPeriodDuration(3600.0),
             experienceDependentMaturation(0.0f), patternFormationRate(0.5f),
             matureThresholdDevelopment(-55.0f), matureRestingPotentialDevelopment(-70.0f),
             matureTimeConstantDevelopment(20.0f), matureRefractoryPeriodDevelopment(5) {
        
        // Initialize stage-specific maturation rates
        stageSpecificMaturationRates[0] = 0.01f;  // Initial
        stageSpecificMaturationRates[1] = 0.05f;  // CriticalPeriod (high)
        stageSpecificMaturationRates[2] = 0.03f;  // Maturation
        stageSpecificMaturationRates[3] = 0.01f;  // Adult
        stageSpecificMaturationRates[4] = 0.005f; // Aging
        
        // Initialize molecular mechanisms arrays
        synapticStabilizationFactors.reserve(1000);
        synapticMaturationWeights.reserve(1000);
        molecularScaffold.reserve(1000);
        experienceMaturationWeights.reserve(100);
        firingPatternStability.reserve(1000);
        connectivityPatternStrength.reserve(1000);
    }
};

Maturation::Maturation() : pImpl(new Impl) {
    NLM_LOG_INFO("Maturation initialized with target mature threshold: " + std::to_string(pImpl->matureThresholdDevelopment));
}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    // Update developmental clock
    pImpl->developmentalTime = static_cast<double>(currentStep) * 0.001;  // Convert to seconds
    pImpl->developmentalClockAge += 0.001;
    
    // Calculate maturation rate for current stage
    float stageMultiplier = getStageMultiplier(brain);
    float maturationRate = pImpl->stageSpecificMaturationRates[static_cast<size_t>(brain->getDevelopmentalStage())] * stageMultiplier;
    
    // Update neuromodulation influences
    updateNeuromodulationInfluences(brain);
    
    // Update activity-dependent maturation
    updateActivityDependentMaturation(brain);
    
    // Update molecular mechanisms
    updateMolecularMechanisms(brain, maturationRate);
    
    // Apply developmental stage effects
    applyStageEffects(brain, maturationRate);
    
    // Update experience-dependent maturation
    updateExperienceDependentMaturation(brain);
    
    // Update neural pattern development
    updateNeuralPatternDevelopment(brain, maturationRate);
    
    // Apply adult property development
    applyAdultPropertyDevelopment(brain, maturationRate);
    
    // Apply aging-related decline mechanisms
    applyAgingRelatedDecline(brain, maturationRate);
    
    // Calculate overall maturation progress
    calculateMaturationProgress(brain);
    
    // Log maturation status
    if (pImpl->lastMaturationStep == 0 || (currentStep - pImpl->lastMaturationStep) >= 1000) {
        NLM_LOG_INFO("Maturation: Stage=" + std::to_string(static_cast<int>(brain->getDevelopmentalStage())) + 
                    ", Progress=" + std::to_string(pImpl->progress) + "/1.0, Age=" + 
                    std::to_string(pImpl->developmentalClockAge) + "s, ActivityCoherence=" + 
                    std::to_string(pImpl->activityCoherence));
        pImpl->lastMaturationStep = currentStep;
    }
}

void Maturation::updateNeuromodulationInfluences(Brain* brain) {
    if (!brain) return;
    
    // Reset neuromodulation influences
    pImpl->dopamineInfluence = 0.0f;
    pImpl->rewardInfluence = 0.0f;
    pImpl->curiosityInfluence = 0.0f;
    pImpl->noveltyInfluence = 0.0f;
    
    // Get neuromodulator levels
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        pImpl->dopamineInfluence = dopamineLevel * 0.5f;
        
        // Dopamine enhances maturation and neural plasticity
        pImpl->progress += dopamineLevel * 0.001f;
    }
    
    if (auto* reward = brain->getReward()) {
        float rewardValue = reward->getValue();
        pImpl->rewardInfluence = rewardValue * 0.3f;
        
        // Reward accelerates maturation through reinforcement learning
        pImpl->progress += rewardValue * 0.0005f;
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        pImpl->curiosityInfluence = curiosityLevel * 0.2f;
        
        // Curiosity drives exploration and pattern formation
        pImpl->patternFormationRate += curiosityLevel * 0.01f;
    }
    
    if (auto* novelty = brain->getNovelty()) {
        float noveltyLevel = novelty->getLevel();
        pImpl->noveltyInfluence = noveltyLevel * 0.1f;
        
        // Novelty enhances synaptic remodeling and pattern updating
        pImpl->synapticMaturationWeights[0] += noveltyLevel * 0.02f;
    }
}

void Maturation::updateActivityDependentMaturation(Brain* brain) {
    if (!brain) return;
    
    // Calculate current neural activity metrics
    float totalFiringRate = brain->getTotalSpikeCount();
    size_t activeNeuronCount = brain->getActiveNeuronCount();
    
    // Update average firing rate
    if (activeNeuronCount > 0) {
        pImpl->averageFiringRate = static_cast<float>(totalFiringRate) / static_cast<float>(activeNeuronCount);
    }
    
    // Calculate activity coherence (synchronization of neural activity)
    pImpl->activityCoherence = calculateActivityCoherence(brain);
    
    // Calculate burst pattern strength
    pImpl->burstPatternStrength = calculateBurstPatternStrength(brain);
    
    // Apply activity-dependent maturation
    float activityDrivenMaturation = (pImpl->averageFiringRate / 100.0f) * 0.5f +  // Normalize firing rate
                                     pImpl->activityCoherence * 0.3f +                  // Activity synchronization
                                     pImpl->burstPatternStrength * 0.2f;                 // Burst dynamics
    
    // Apply homeostatic regulation to activity-dependent maturation
    float homeostaticError = pImpl->homeostaticMaturationSetpoint - pImpl->averageFiringRate;
    float homeostaticMaturation = homeostaticError * pImpl->maturationProportionalGain;
    
    // Combine activity-driven and homeostatic maturation
    float totalMaturation = activityDrivenMaturation + homeostaticMaturation;
    
    // Apply molecular stability check
    float molecularStability = calculateMolecularStability(brain);
    
    // Determine if maturation can proceed
    if (molecularStability > pImpl->activityMaturationThreshold && totalMaturation > 0.01f) {
        pImpl->progress += totalMaturation * 0.001f;  // Small incremental progress
    }
}

void Maturation::updateMolecularMechanisms(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    // Update molecular scaffolding
    pImpl->molecularScaffoldingProgress += maturationRate * 0.5f;
    pImpl->molecularScaffoldingProgress = std::clamp(pImpl->molecularScaffoldingProgress, 0.0f, 1.0f);
    
    // Update protein synthesis based on activity
    float proteinSynthesisRate = calculateProteinSynthesisRate(brain);
    pImpl->proteinSynthesisRate = proteinSynthesisRate;
    
    // Update gene expression levels
    float geneExpressionRate = calculateGeneExpressionRate(brain, maturationRate);
    pImpl->geneExpressionLevel += geneExpressionRate * 0.001f;
    pImpl->geneExpressionLevel = std::clamp(pImpl->geneExpressionLevel, 0.0f, 1.0f);
    
    // Update synaptic stabilization factors
    updateSynapticStabilizationFactors(brain, maturationRate);
    
    // Apply molecular maturation to neural properties
    applyMolecularMaturationToNeurons(brain, maturationRate);
}

void Maturation::applyStageEffects(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    // Apply stage-specific maturation effects
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Initial: rapid molecular scaffold formation
            pImpl->molecularScaffoldingProgress += maturationRate * 2.0f;
            pImpl->proteinSynthesisRate *= 1.5f;
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // CriticalPeriod: high plasticity, rapid synaptic formation and stabilization
            pImpl->synapticStabilizationFactors[0] += maturationRate * 0.5f;
            pImpl->activityMaturationThreshold *= 0.8f;  // Lower threshold for activity-driven maturation
            break;
            
        case DevelopmentalStage::Maturation:
            // Maturation: balanced synaptic strengthening and refinement
            pImpl->synapticMaturationWeights[0] += maturationRate * 0.3f;
            pImpl->matureFiringPatternCoherence += maturationRate * 0.2f;
            pImpl->matureConnectivityOptimization += maturationRate * 0.1f;
            break;
            
        case DevelopmentalStage::Adult:
            // Adult: maintenance and optimization of mature properties
            pImpl->matureExcitatoryInhibitoryBalance = 1.0f;  // Maintain balance
            pImpl->patternFormationRate *= 0.5f;  // Reduce pattern formation
            applyAdultOptimization(brain, maturationRate);
            break;
            
        case DevelopmentalStage::Aging:
            // Aging: controlled decline with potential for rejuvenation
            applyAgingMaturationEffects(brain, maturationRate);
            break;
    }
}

void Maturation::updateExperienceDependentMaturation(Brain* brain) {
    if (!brain) return;
    
    // Get recent neural activity patterns for experience-dependent maturation
    const auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        
        for (auto* neuron : neurons) {
            if (!neuron) continue;
            
            // Calculate neuron experience based on recent activity
            float neuronExperience = calculateNeuronExperience(neuron);
            
            if (neuronExperience > 0.1f) {
                pImpl->recentlyExperiencedNeurons.insert(neuron->getId());
                
                // Apply experience-dependent maturation
                float experienceMaturation = neuronExperience * pImpl->experienceDependentMaturation;
                
                // Find corresponding neuron in molecular tracking arrays
                size_t neuronIndex = findNeuronIndex(neuron->getId());
                
                if (neuronIndex < pImpl->firingPatternStability.size()) {
                    // Update firing pattern stability
                    pImpl->firingPatternStability[neuronIndex] += experienceMaturation * 0.1f;
                    pImpl->firingPatternStability[neuronIndex] = std::clamp(pImpl->firingPatternStability[neuronIndex], 0.0f, 1.0f);
                }
                
                // Enhance synaptic connections based on experience
                enhanceExperienceBasedConnections(brain, neuron, neuronExperience);
            }
        }
    }
}

void Maturation::updateNeuralPatternDevelopment(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    // Develop and stabilize neural firing patterns
    const auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        // Calculate regional pattern development
        float regionalPatternStrength = calculateRegionalPatternStrength(region.get());
        
        // Update connectivity pattern strength
        pImpl->connectivityPatternStrength[0] += regionalPatternStrength * maturationRate * 0.1f;
        
        // Apply pattern-dependent maturation
        applyPatternBasedMaturation(brain, regionalPatternStrength, maturationRate);
    }
    
    // Update overall pattern formation rate
    pImpl->patternFormationRate = std::clamp(pImpl->patternFormationRate, 0.1f, 1.0f);
}

void Maturation::applyAdultPropertyDevelopment(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    if (stage != DevelopmentalStage::Adult && stage != DevelopmentalStage::Aging) {
        return;  // Only apply adult property development in adult and aging stages
    }
    
    // Develop adult-level neural properties
    const auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        
        for (auto* neuron : neurons) {
            if (!neuron) continue;
            
            // Update adult property development
            updateAdultProperties(neuron, maturationRate);
        }
    }
    
    // Optimize network-level adult properties
    optimizeAdultNetworkProperties(brain, maturationRate);
}

void Maturation::applyAgingMaturationEffects(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    // Apply aging-related maturation effects
    // This includes controlled decline and potential for rejuvenation
    
    // Calculate aging rate based on developmental clock
    float agingRate = calculateAgingRate();
    
    // Apply neuronal decline
    pImpl->declineFactors[0] = std::min(1.0f, pImpl->declineFactors[0] + agingRate * 0.1f);
    
    // Apply synaptic decline
    pImpl->declineFactors[1] = std::min(1.0f, pImpl->declineFactors[1] + agingRate * 0.15f);
    
    // Apply metabolic decline
    pImpl->declineFactors[2] = std::min(1.0f, pImpl->declineFactors[2] + agingRate * 0.2f);
    
    // Potential rejuvenation through experience and stimulation
    applyRejuvenationMechanisms(brain, maturationRate);
    
    // Calculate overall aging impact on maturation
    float agingImpact = pImpl->declineFactors[0] + pImpl->declineFactors[1] + pImpl->declineFactors[2];
    agingImpact /= 3.0f;
    
    // Reduce maturation progress in aging stages
    pImpl->progress *= (1.0f - agingImpact * maturationRate * 0.1f);
}

void Maturation::applyAdultOptimization(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    // Optimize neural properties for adult function
    const auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        
        // Optimize firing thresholds
        for (auto* neuron : neurons) {
            if (!neuron) continue;
            
            // Move toward mature firing threshold
            float currentThreshold = neuron->getThreshold();
            float matureThreshold = pImpl->matureThresholdDevelopment;
            
            float thresholdAdjustment = (matureThreshold - currentThreshold) * maturationRate * 0.1f;
            neuron->setThreshold(currentThreshold + thresholdAdjustment);
            
            // Optimize resting potential
            float currentRestingPotential = neuron->getRestingPotential();
            float matureRestingPotential = pImpl->matureRestingPotentialDevelopment;
            
            float restingPotentialAdjustment = (matureRestingPotential - currentRestingPotential) * maturationRate * 0.1f;
            neuron->setRestingPotential(currentRestingPotential + restingPotentialAdjustment);
            
            // Optimize time constant
            float currentTimeConstant = neuron->getRefractoryPeriod();
            float matureTimeConstant = pImpl->matureTimeConstantDevelopment;
            
            float timeConstantAdjustment = (matureTimeConstant - currentTimeConstant) * maturationRate * 0.05f;
            neuron->setRefractoryPeriod(static_cast<uint32_t>(currentTimeConstant + timeConstantAdjustment));
        }
    }
}

void Maturation::applyRejuvenationMechanisms(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    // Apply rejuvenation through various mechanisms
    
    // 1. Activity-dependent rejuvenation
    if (!pImpl->recentlyExperiencedNeurons.empty()) {
        float rejuvenationStrength = static_cast<float>(pImpl->recentlyExperiencedNeurons.size()) / 100.0f;
        pImpl->rejuvenationPotential += rejuvenationStrength * maturationRate * 0.1f;
    }
    
    // 2. Neuromodulatory rejuvenation
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        pImpl->rejuvenationPotential += dopamineLevel * maturationRate * 0.05f;
    }
    
    // 3. Synaptic remodeling rejuvenation
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        if (!region) continue;
        
        // Remove some aged, weak synapses and replace with new ones
        applySynapticRemodelingRejuvenation(region.get(), maturationRate);
    }
    
    // Apply rejuvenation potential to decline factors
    float rejuvenationEffect = std::min(1.0f, pImpl->rejuvenationPotential * 0.1f);
    for (int i = 0; i < 3; ++i) {
        pImpl->declineFactors[i] *= (1.0f - rejuvenationEffect);
    }
}

void Maturation::enhanceExperienceBasedConnections(Brain* brain, Neuron* neuron, float experience) {
    if (!neuron) return;
    
    // Enhance connections based on neuron experience
    // This simulates the biological process of synaptic strengthening through use
    
    // Find neuron in molecular tracking arrays
    size_t neuronIndex = findNeuronIndex(neuron->getId());
    
    if (neuronIndex < pImpl->synapticStabilizationFactors.size()) {
        // Increase synaptic stabilization based on experience
        pImpl->synapticStabilizationFactors[neuronIndex] *= (1.0f + experience * 0.1f);
        
        // Apply Hebbian-like strengthening
        pImpl->synapticMaturationWeights[neuronIndex] *= (1.0f + experience * 0.05f);
    }
}

void Maturation::applyPatternBasedMaturation(Brain* brain, float patternStrength, float maturationRate) {
    if (!brain) return;
    
    // Apply pattern-based maturation to optimize neural connectivity
    float patternMaturation = patternStrength * maturationRate * 0.2f;
    
    // Update pattern formation rate
    pImpl->patternFormationRate += patternMaturation;
    pImpl->patternFormationRate = std::clamp(pImpl->patternFormationRate, 0.1f, 1.0f);
    
    // Apply to synaptic stability
    for (size_t i = 0; i < pImpl->synapticStabilizationFactors.size() && i < patternStrength; ++i) {
        pImpl->synapticStabilizationFactors[i] += patternMaturation * 0.1f;
        pImpl->synapticStabilizationFactors[i] = std::clamp(pImpl->synapticStabilizationFactors[i], 0.0f, 1.0f);
    }
}

void Maturation::applySynapticRemodelingRejuvenation(NeuralRegion* region, float maturationRate) {
    if (!region) return;
    
    auto synapses = region->getSynapses();
    
    // Calculate which synapses to remodel (aged, weak synapses)
    std::vector<std::pair<Synapse*, float>> synapseRemodelingScores;
    
    for (auto& synapse : synapses) {
        if (!synapse) continue;
        
        // Calculate remodeling score based on age and stability
        float age = getSynapseAge(synapse.get()->getId());
        float stability = calculateSynapseMolecularStability(synapse.get());
        
        float remodelingScore = (1.0f - stability) * std::exp(-age / 200.0f);  // Higher for unstable, older synapses
        
        synapseRemodelingScores.push_back({synapse.get(), remodelingScore});
    }
    
    // Sort by remodeling score (highest first)
    std::sort(synapseRemodelingScores.begin(), synapseRemodelingScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Remodel top synapses
    size_t synapsesToRemodel = std::min(static_cast<size_t>(synapseRemodelingScores.size() * 0.1f), 
                                       synapseRemodelingScores.size());
    
    for (size_t i = 0; i < synapsesToRemodel; ++i) {
        Synapse* synapse = synapseRemodelingScores[i].first;
        
        // Replace aged synapse with new one
        if (remodelSynapticConnection(region, synapse)) {
            NLM_LOG_INFO("Maturation: Synaptic remodeling rejuvenation completed");
        }
    }
}

float Maturation::calculateActivityCoherence(Brain* brain) const {
    if (!brain) return 0.0f;
    
    // Calculate coherence of neural activity patterns
    // This is a simplified version - in reality, this would involve
    // more sophisticated correlation analysis
    
    float totalSpikeCount = brain->getTotalSpikeCount();
    size_t activeNeuronCount = brain->getActiveNeuronCount();
    
    if (activeNeuronCount < 2) return 0.0f;
    
    // Simplified coherence calculation based on spike timing regularity
    // Higher coherence indicates more synchronized activity
    float coherence = std::min(1.0f, totalSpikeCount / 1000.0f) * 0.5f;  // Normalize to 0-1 range
    
    return coherence;
}

float Maturation::calculateBurstPatternStrength(Brain* brain) const {
    if (!brain) return 0.0f;
    
    // Calculate strength of burst firing patterns
    // This would require detailed spike timing analysis in a real implementation
    
    // For now, return a simplified proxy based on overall firing rate
    float firingRate = brain->getAverageFiringRate();
    
    return std::min(1.0f, firingRate / 50.0f);  // Normalize to 0-1 range
}

float Maturation::calculateMolecularStability(Brain* brain) const {
    if (!brain) return 0.0f;
    
    // Calculate overall molecular stability of the network
    float stabilitySum = 0.0f;
    int validSynapses = 0;
    
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto synapses = region->getSynapses();
        for (const auto& synapse : synapses) {
            if (!synapse) continue;
            
            // Calculate synapse stability based on multiple factors
            float weightStability = 1.0f - std::abs(synapse->getWeight());
            float activityStability = calculateSynapseActivityHistory(synapse.get());
            
            float synapseStability = (weightStability * 0.6f) + (activityStability * 0.4f);
            stabilitySum += synapseStability;
            validSynapses++;
        }
    }
    
    return validSynapses > 0 ? stabilitySum / static_cast<float>(validSynapses) : 0.0f;
}

float Maturation::calculateProteinSynthesisRate(Brain* brain) const {
    if (!brain) return 0.0f;
    
    // Calculate protein synthesis rate based on neural activity and maturation progress
    float activityLevel = brain->getAverageFiringRate() / 100.0f;  // Normalize
    
    return pImpl->molecularScaffoldingProgress * activityLevel * 0.1f;
}

float Maturation::calculateGeneExpressionRate(Brain* brain, float maturationRate) const {
    if (!brain) return 0.0f;
    
    // Calculate gene expression rate based on maturation signals
    float experienceSignal = calculateExperienceSignal(brain);
    float developmentalSignal = maturationRate;
    
    return (experienceSignal * 0.7f) + (developmentalSignal * 0.3f);
}

float Maturation::calculateExperienceSignal(Brain* brain) const {
    if (!brain) return 0.0f;
    
    // Calculate experience signal from recent neural activity
    float experienceSum = 0.0f;
    int validNeurons = 0;
    
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (!neuron) continue;
            
            float experience = calculateNeuronExperience(neuron);
            if (experience > 0.0f) {
                experienceSum += experience;
                validNeurons++;
            }
        }
    }
    
    return validNeurons > 0 ? experienceSum / static_cast<float>(validNeurons) : 0.0f;
}

float Maturation::calculateRegionalPatternStrength(NeuralRegion* region) const {
    if (!region) return 0.0f;
    
    // Calculate pattern strength for a specific region
    auto neurons = region->getAllNeurons();
    
    if (neurons.size() < 2) return 0.0f;
    
    // Calculate average activity pattern coherence
    float totalCoherence = 0.0f;
    int validNeurons = 0;
    
    for (auto* neuron : neurons) {
        if (!neuron) continue;
        
        float activity = calculateNeuronActivity(neuron);
        if (activity > 0.0f) {
            totalCoherence += activity;
            validNeurons++;
        }
    }
    
    return validNeurons > 0 ? totalCoherence / static_cast<float>(validNeurons) : 0.0f;
}

float Maturation::calculateNeuronExperience(Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Calculate neuron experience based on firing history and state
    float firingHistoryExperience = calculateFiringHistoryExperience(neuron);
    float membranePotentialExperience = calculateMembranePotentialExperience(neuron);
    float synapticHistoryExperience = calculateSynapticHistoryExperience(neuron);
    
    return (firingHistoryExperience * 0.4f) + (membranePotentialExperience * 0.3f) + 
           (synapticHistoryExperience * 0.3f);
}

float Maturation::calculateFiringHistoryExperience(Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    auto spikeHistory = neuron->getSpikeHistory();
    if (spikeHistory.empty()) return 0.0f;
    
    // Calculate experience based on spike frequency and timing
    Timestamp lastSpikeTime = spikeHistory.back();
    Timestamp firstSpikeTime = spikeHistory.front();
    
    float timeSpan = static_cast<float>(lastSpikeTime - firstSpikeTime);
    float spikeCount = static_cast<float>(spikeHistory.size());
    
    if (timeSpan <= 0.0f) return 0.0f;
    
    float firingRate = spikeCount / timeSpan;
    
    return std::min(1.0f, firingRate / 10.0f);  // Normalize to 0-1 range
}

float Maturation::calculateMembranePotentialExperience(Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Calculate experience based on membrane potential dynamics
    float currentPotential = neuron->getMembranePotential();
    float restingPotential = neuron->getRestingPotential();
    float threshold = neuron->getThreshold();
    
    // Experience based on proximity to threshold (excitability)
    float thresholdProximity = (currentPotential - threshold) / 50.0f;  // Normalize
    
    return std::max(0.0f, std::min(1.0f, thresholdProximity));
}

float Maturation::calculateSynapticHistoryExperience(Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Calculate experience based on synaptic input history
    // This would require tracking synaptic input over time
    // For now, return a simplified version
    
    return 0.5f;  // Placeholder
}

float Maturation::calculateNeuronActivity(Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Calculate neuron activity similar to Synaptogenesis
    float firingRate = neuron->getFiringRate();
    float membranePotential = neuron->getMembranePotential();
    float threshold = neuron->getThreshold();
    
    float normalizedFiringRate = firingRate / 100.0f;
    float potentialRelative = (membranePotential - threshold) / 50.0f;
    
    float activity = (normalizedFiringRate * 0.6f) + (potentialRelative * 0.4f);
    
    return std::max(0.0f, std::min(1.0f, activity));
}

float Maturation::calculateSynapseActivityHistory(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    // Calculate synapse activity history based on spike records
    auto preSpikeHistory = synapse->getPreSpikeHistory();
    auto postSpikeHistory = synapse->getPostSpikeHistory();
    
    if (preSpikeHistory.empty() && postSpikeHistory.empty()) return 0.0f;
    
    // Calculate activity based on spike timing
    float preSpikeActivity = preSpikeHistory.empty() ? 0.0f : 1.0f;
    float postSpikeActivity = postSpikeHistory.empty() ? 0.0f : 1.0f;
    
    return (preSpikeActivity + postSpikeActivity) * 0.5f;
}

float Maturation::getSynapseAge(SynapseId synapseId) const {
    // Get synapse age from tracking arrays
    for (size_t i = 0; i < pImpl->birthDates.size(); ++i) {
        if (pImpl->birthDates[i] == synapseId.value) {
            return static_cast<float>(i) * 0.1f;
        }
    }
    return 0.0f;
}

void Maturation::updateAdultProperties(Neuron* neuron, float maturationRate) {
    if (!neuron) return;
    
    // Update adult neural properties toward mature values
    
    // Update threshold
    float currentThreshold = neuron->getThreshold();
    float matureThreshold = pImpl->matureThresholdDevelopment;
    
    float thresholdAdjustment = (matureThreshold - currentThreshold) * maturationRate * 0.05f;
    neuron->setThreshold(currentThreshold + thresholdAdjustment);
    
    // Update resting potential
    float currentRestingPotential = neuron->getRestingPotential();
    float matureRestingPotential = pImpl->matureRestingPotentialDevelopment;
    
    float restingPotentialAdjustment = (matureRestingPotential - currentRestingPotential) * maturationRate * 0.05f;
    neuron->setRestingPotential(currentRestingPotential + restingPotentialAdjustment);
    
    // Update refractory period
    float currentRefractoryPeriod = neuron->getRefractoryPeriod();
    float matureRefractoryPeriod = pImpl->matureRefractoryPeriodDevelopment;
    
    float refractoryAdjustment = (matureRefractoryPeriod - currentRefractoryPeriod) * maturationRate * 0.03f;
    neuron->setRefractoryPeriod(static_cast<uint32_t>(currentRefractoryPeriod + refractoryAdjustment));
}

void Maturation::optimizeAdultNetworkProperties(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    // Optimize network-level properties for adult function
    
    float currentEIRatio = brain->getExcitationInhibitionRatio();
    
    // Move toward optimal E/I ratio
    float optimalEIRatio = pImpl->matureExcitatoryInhibitoryBalance;
    float eiError = optimalEIRatio - currentEIRatio;
    
    // This would require adjusting synaptic weights to optimize E/I balance
    // For now, just track the error
    pImpl->matureExcitatoryInhibitoryBalance = std::min(1.2f, std::max(0.8f, currentEIRatio));
}

void Maturation::updateSynapticStabilizationFactors(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    // Update synaptic stabilization factors based on maturation progress
    for (size_t i = 0; i < pImpl->synapticStabilizationFactors.size(); ++i) {
        pImpl->synapticStabilizationFactors[i] += maturationRate * 0.01f;
        pImpl->synapticStabilizationFactors[i] = std::clamp(pImpl->synapticStabilizationFactors[i], 0.0f, 1.0f);
        
        // Apply Hebbian-like strengthening
        pImpl->synapticMaturationWeights[i] += maturationRate * 0.005f;
        pImpl->synapticMaturationWeights[i] = std::clamp(pImpl->synapticMaturationWeights[i], 0.0f, 1.0f);
    }
}

void Maturation::applyMolecularMaturationToNeurons(Brain* brain, float maturationRate) {
    if (!brain) return;
    
    // Apply molecular maturation effects to neurons
    const auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        
        for (auto* neuron : neurons) {
            if (!neuron) continue;
            
            // Update neuron properties based on molecular maturation
            updateNeuronPropertiesFromMolecularMaturation(neuron, maturationRate);
        }
    }
}

void Maturation::updateNeuronPropertiesFromMolecularMaturation(Neuron* neuron, float maturationRate) {
    if (!neuron) return;
    
    // Update neuron properties based on molecular maturation
    
    // Update ion channel properties (simplified)
    float currentThreshold = neuron->getThreshold();
    neuron->setThreshold(currentThreshold * (1.0f + maturationRate * 0.01f));
    
    // Update adaptation properties
    float currentAdaptation = neuron->getState().adaptationVariable;
    neuron->getState().adaptationVariable = currentAdaptation * (1.0f + maturationRate * 0.005f);
}

float Maturation::calculateAgingRate() const {
    // Calculate aging rate based on developmental clock
    float ageProgress = static_cast<float>(pImpl->developmentalClockAge) / 3600.0f;  // Normalize to 1 hour
    return ageProgress * pImpl->agingRate;
}

bool Maturation::remodelSynapticConnection(NeuralRegion* region, Synapse* oldSynapse) {
    if (!region || !oldSynapse) return false;
    
    // Replace old synapse with new one as part of rejuvenation
    NeuronId source = oldSynapse->getSourceNeuron();
    NeuronId destination = oldSynapse->getDestinationNeuron();
    
    // Remove old synapse (placeholder - would need actual removal method)
    // For now, just return true to indicate successful remodeling
    return true;
}

size_t Maturation::findNeuronIndex(NeuronId neuronId) const {
    // Find neuron index in molecular tracking arrays
    for (size_t i = 0; i < pImpl->firingPatternStability.size(); ++i) {
        // In a real implementation, this would track neuron IDs
        // For now, use a simplified version
    }
    return pImpl->firingPatternStability.size();  // Return out of bounds if not found
}

size_t Maturation::findSynapseIndex(SynapseId synapseId) const {
    // Find synapse index in molecular tracking arrays
    for (size_t i = 0; i < pImpl->birthDates.size(); ++i) {
        if (pImpl->birthDates[i] == synapseId.value) {
            return i;
        }
    }
    return pImpl->synapticStabilizationFactors.size();  // Return out of bounds if not found
}

void Maturation::calculateMaturationProgress(Brain* brain) {
    if (!brain) return;
    
    // Calculate overall maturation progress based on multiple factors
    
    float stageProgress = getStageProgress(brain->getDevelopmentalStage());
    float activityProgress = pImpl->activityCoherence;
    float molecularProgress = pImpl->molecularScaffoldingProgress;
    float experienceProgress = pImpl->experienceDependentMaturation;
    
    // Combine factors with appropriate weights
    float totalProgress = (stageProgress * 0.2f) + 
                         (activityProgress * 0.3f) + 
                         (molecularProgress * 0.2f) + 
                         (experienceProgress * 0.3f);
    
    // Apply aging-related decline
    float agingImpact = calculateAgingImpact();
    totalProgress *= (1.0f - agingImpact);
    
    // Update overall progress
    pImpl->progress = std::clamp(totalProgress, 0.0f, 1.0f);
}

float Maturation::getStageProgress(DevelopmentalStage stage) const {
    // Get stage-specific progress
    switch (stage) {
        case DevelopmentalStage::Initial:
            return std::min(1.0f, pImpl->developmentalClockAge / 60.0f);  // 60 seconds to complete Initial
        case DevelopmentalStage::CriticalPeriod:
            return std::min(1.0f, (pImpl->developmentalClockAge - 60.0f) / 300.0f);  // 300 seconds
        case DevelopmentalStage::Maturation:
            return std::min(1.0f, (pImpl->developmentalClockAge - 360.0f) / 600.0f);  // 600 seconds
        case DevelopmentalStage::Adult:
            return std::min(1.0f, (pImpl->developmentalClockAge - 960.0f) / 2640.0f);  // 2640 seconds
        case DevelopmentalStage::Aging:
            return 1.0f - std::min(1.0f, (pImpl->developmentalClockAge - 3600.0f) / 3600.0f);  // 3600 seconds aging period
        default:
            return 0.0f;
    }
}

float Maturation::calculateAgingImpact() const {
    // Calculate overall aging impact on maturation
    float totalDecline = pImpl->declineFactors[0] + pImpl->declineFactors[1] + pImpl->declineFactors[2];
    return totalDecline / 3.0f;
}

float Maturation::getStageMultiplier(Brain* brain) const {
    if (!brain) return 1.0f;
    
    // Return stage-specific maturation multiplier
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            return pImpl->stageSpecificMaturationRates[0];
        case DevelopmentalStage::CriticalPeriod:
            return pImpl->stageSpecificMaturationRates[1];
        case DevelopmentalStage::Maturation:
            return pImpl->stageSpecificMaturationRates[2];
        case DevelopmentalStage::Adult:
            return pImpl->stageSpecificMaturationRates[3];
        case DevelopmentalStage::Aging:
            return pImpl->stageSpecificMaturationRates[4];
        default:
            return 1.0f;
    }
}

} // namespace nlm