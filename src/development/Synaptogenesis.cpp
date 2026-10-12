#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
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
#include <random>
#include <unordered_set>

namespace nlm {

struct Synaptogenesis::Impl {
    // Formation rate parameters
    float formationRate;
    float targetDensity;
    float baseFormationRate;
    float criticalPeriodBoost;
    
    // Activity-dependent formation parameters
    float hebbianFormationStrength;
    float stdpFormationThreshold;
    float activityThreshold;
    
    // Neuromodulation integration
    float dopamineInfluence;
    float rewardInfluence;
    float curiosityInfluence;
    float noveltyInfluence;
    
    // Homeostatic regulation
    float homeostaticTargetEI;
    float homeostaticProportionalGain;
    float homeostaticIntegral;
    
    // Developmental stage parameters
    float stageSpecificRates[5];  // Initial, CriticalPeriod, Maturation, Adult, Aging
    
    // Neural competition and resource allocation
    float competitionFactor;
    float resourceAllocationEfficiency;
    
    // Target density control
    float currentDensity;
    float densityError;
    
    // Memory integration for activity patterns
    std::unordered_set<NeuronId> recentlyActiveNeurons;
    float activityHistoryDecay;
    
    // Molecular mechanisms for stabilization
    std::vector<float> synapseStabilizationFactors;
    std::vector<float> birthDates;
    
    // Recent formation tracking
    std::vector<SynapseId> recentSynapses;
    float synapseLifespan;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), baseFormationRate(0.001f),
             criticalPeriodBoost(2.0f), hebbianFormationStrength(0.5f),
             stdpFormationThreshold(0.01f), activityThreshold(0.1f),
             dopamineInfluence(1.0f), rewardInfluence(1.0f),
             curiosityInfluence(1.0f), noveltyInfluence(1.0f),
             homeostaticTargetEI(1.0f), homeostaticProportionalGain(0.1f),
             homeostaticIntegral(0.0f), competitionFactor(0.5f),
             resourceAllocationEfficiency(0.8f), currentDensity(0.0f),
             densityError(0.0f), activityHistoryDecay(0.95f),
             synapseLifespan(1000.0f) {
        
        // Initialize stage-specific rates
        stageSpecificRates[0] = 0.001f;  // Initial
        stageSpecificRates[1] = 0.01f;   // CriticalPeriod (high)
        stageSpecificRates[2] = 0.005f;  // Maturation
        stageSpecificRates[3] = 0.0005f; // Adult
        stageSpecificRates[4] = 0.0001f; // Aging
        
        // Reserve memory for molecular mechanisms
        synapseStabilizationFactors.reserve(1000);
        birthDates.reserve(1000);
        recentSynapses.reserve(100);
    }
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {
    NLM_LOG_INFO("Synaptogenesis initialized with target density: " + std::to_string(pImpl->targetDensity));
}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::max(0.0f, rate);
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.01f, 1.0f);
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    pImpl->currentDensity = calculateCurrentSynapseDensity(brain);
    pImpl->densityError = pImpl->targetDensity - pImpl->currentDensity;
    
    // Update homeostatic regulation
    updateHomeostaticRegulation(brain, dt);
    
    // Update neuromodulation influences
    updateNeuromodulationInfluences(brain);
    
    // Calculate formation rate for current developmental stage
    float stageMultiplier = getStageMultiplier(brain);
    pImpl->formationRate = pImpl->baseFormationRate * stageMultiplier;
    
    // Apply homeostatic and neuromodulatory modulation
    float homeostaticMod = 1.0f + pImpl->homeostaticIntegral * pImpl->homeostaticProportionalGain;
    float neuromodulationMod = 1.0f + pImpl->dopamineInfluence + pImpl->rewardInfluence + 
                              pImpl->curiosityInfluence + pImpl->noveltyInfluence;
    
    float effectiveFormationRate = pImpl->formationRate * homeostaticMod * neuromodulationMod;
    
    // Perform activity-dependent synapse formation
    performActivityDependentFormation(brain, rng, dt, effectiveFormationRate);
    
    // Update recent synapse tracking
    updateRecentSynapseTracking(brain, dt);
    
    // Apply target density control
    applyTargetDensityControl(brain, rng, dt);
    
    // Update activity history
    updateActivityHistory(brain, dt);
    
    // Log development status
    if (pImpl->currentStep % 1000 == 0) {
        NLM_LOG_INFO("Synaptogenesis: Stage=" + std::to_string(static_cast<int>(brain->getDevelopmentalStage())) + 
                    ", Density=" + std::to_string(pImpl->currentDensity) + 
                    ", FormationRate=" + std::to_string(effectiveFormationRate));
    }
}

float Synaptogenesis::calculateCurrentSynapseDensity(Brain* brain) const {
    if (!brain) return 0.0f;
    
    size_t totalPossibleConnections = 0;
    size_t totalActualConnections = 0;
    
    // Calculate density across all regions
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        size_t neuronCount = region->getTotalNeuronCount();
        size_t populationCount = region->getPopulationCount();
        
        // Estimate possible connections based on population sizes
        for (size_t i = 0; i < populationCount; ++i) {
            auto* pop = region->getPopulation(PopulationId(i));
            if (pop) {
                totalPossibleConnections += pop->size() * populationCount;
            }
        }
        
        // Count actual synapses
        totalActualConnections += region->getSynapseCount();
    }
    
    return totalPossibleConnections > 0 ? 
        static_cast<float>(totalActualConnections) / static_cast<float>(totalPossibleConnections) : 0.0f;
}

void Synaptogenesis::updateHomeostaticRegulation(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    float eiRatio = brain->getExcitationInhibitionRatio();
    float error = pImpl->homeostaticTargetEI - eiRatio;
    
    // Proportional-integral control for homeostasis
    pImpl->homeostaticIntegral += error * pImpl->homeostaticProportionalGain * dt;
    pImpl->homeostaticIntegral = std::clamp(pImpl->homeostaticIntegral, -1.0f, 1.0f);
    
    // Apply homeostatic formation modulation
    float homeostaticMod = 1.0f + error * 0.1f + pImpl->homeostaticIntegral * 0.05f;
    
    // Modify formation rate based on homeostasis
    pImpl->formationRate *= homeostaticMod;
}

void Synaptogenesis::updateNeuromodulationInfluences(Brain* brain) {
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
        
        // Dopamine specifically enhances synaptogenesis
        pImpl->formationRate *= (1.0f + dopamineLevel * 0.3f);
    }
    
    if (auto* reward = brain->getReward()) {
        float rewardValue = reward->getValue();
        pImpl->rewardInfluence = rewardValue * 0.4f;
        
        // Reward enhances formation of new connections
        pImpl->formationRate *= (1.0f + rewardValue * 0.2f);
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        pImpl->curiosityInfluence = curiosityLevel * 0.3f;
        
        // Curiosity drives exploration and synaptogenesis
        pImpl->formationRate *= (1.0f + curiosityLevel * 0.25f);
    }
    
    if (auto* novelty = brain->getNovelty()) {
        float noveltyLevel = novelty->getLevel();
        pImpl->noveltyInfluence = noveltyLevel * 0.2f;
        
        // Novelty enhances formation of new associations
        pImpl->formationRate *= (1.0f + noveltyLevel * 0.2f);
    }
}

void Synaptogenesis::performActivityDependentFormation(Brain* brain, RandomGenerator& rng, 
                                                      TimestepDuration dt, float effectiveRate) {
    if (!brain) return;
    
    // Get plasticity systems for STDP and Hebbian integration
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    
    const auto& regions = brain->getRegions();
    
    // Iterate through regions for synapse formation
    for (auto& region : regions) {
        if (!region) continue;
        
        // Get neurons from the region
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) continue;
        
        // Calculate formation probability based on activity patterns
        float activityBasedFormationProb = calculateActivityBasedFormationProbability(brain, neurons, stdp, hebbian);
        
        // Apply Hebbian principles: neurons that fire together should wire together
        performHebbianFormation(region.get(), neurons, rng, dt, activityBasedFormationProb, effectiveRate);
        
        // Apply STDP-based formation for spike-timing dependent connections
        performSTDPBasedFormation(region.get(), neurons, rng, dt, stdp, effectiveRate);
        
        // Apply neural competition for resource allocation
        applyNeuralCompetition(region.get(), neurons, rng, dt);
        
        // Molecular stabilization for newly formed synapses
        applyMolecularStabilization(region.get(), rng, dt);
    }
}

void Synaptogenesis::performHebbianFormation(NeuralRegion* region, const std::vector<Neuron*>& neurons,
                                           RandomGenerator& rng, TimestepDuration dt, 
                                           float baseProb, float effectiveRate) {
    if (!region || neurons.size() < 2) return;
    
    size_t neuronCount = neurons.size();
    
    // Calculate Hebbian formation probability
    float hebbianFormationProb = baseProb * pImpl->hebbianFormationStrength;
    
    // Create connectivity matrix for efficiency
    std::vector<std::vector<bool>> connectionAttempts(neuronCount, std::vector<bool>(neuronCount, false));
    
    for (size_t i = 0; i < neuronCount; ++i) {
        for (size_t j = i + 1; j < neuronCount; ++j) {
            if (connectionAttempts[i][j]) continue;
            
            // Get neurons
            Neuron* neuronA = neurons[i];
            Neuron* neuronB = neurons[j];
            
            // Calculate Hebbian eligibility based on co-activity patterns
            float hebbianEligibility = calculateHebbianEligibility(neuronA, neuronB, dt);
            
            // Apply formation probability
            float formationProb = hebbianFormationProb * hebbianEligibility;
            
            if (rng.bernoulli(formationProb * effectiveRate)) {
                // Form new synapse
                SynapseId synapseId = region->addSynapse(neuronA->getId(), neuronB->getId());
                if (synapseId != INVALID_SYNAPSE_ID) {
                    auto* synapse = region->getSynapse(synapseId);
                    if (synapse) {
                        // Set synapse properties
                        synapse->setType(SynapseType::Excitatory);  // Default to excitatory
                        synapse->setWeight(rng.uniformReal(0.1f, 0.5f));  // Random initial weight
                        
                        // Enable Hebbian plasticity
                        synapse->enablePlasticity(true, false, false);
                        
                        // Record formation for molecular tracking
                        pImpl->recentSynapses.push_back(synapseId);
                        if (pImpl->recentSynapses.size() > pImpl->synapseLifespan) {
                            pImpl->recentSynapses.erase(pImpl->recentSynapses.begin());
                        }
                        
                        NLM_LOG_INFO("Synaptogenesis: Formed Hebbian synapse between neurons " + 
                                    std::to_string(neuronA->getId().index()) + " and " + 
                                    std::to_string(neuronB->getId().index()));
                    }
                }
            }
            
            connectionAttempts[i][j] = connectionAttempts[j][i] = true;
        }
    }
}

void Synaptogenesis::performSTDPBasedFormation(NeuralRegion* region, const std::vector<Neuron*>& neurons,
                                              RandomGenerator& rng, TimestepDuration dt, 
                                              STDP* stdp, float effectiveRate) {
    if (!region || !stdp || neurons.size() < 2) return;
    
    // For STDP-based formation, we look at recent spike patterns
    // This simulates the idea that spike timing can promote synapse formation
    
    std::vector<Neuron*> recentlyActiveNeurons;
    for (auto* neuron : neurons) {
        auto spikeHistory = neuron->getSpikeHistory();
        if (!spikeHistory.empty()) {
            Timestamp lastSpikeTime = spikeHistory.back();
            Timestamp currentTime = static_cast<Timestamp>(dt);
            
            // Check if neuron was recently active
            if (currentTime - lastSpikeTime < dt * 10.0f) {  // Active within last 10 timesteps
                recentlyActiveNeurons.push_back(neuron);
            }
        }
    }
    
    if (recentlyActiveNeurons.size() < 2) return;
    
    // Calculate STDP-based formation probability
    float stdpFormationProb = effectiveRate * pImpl->stdpFormationThreshold;
    
    // Pair recently active neurons for potential synapse formation
    for (size_t i = 0; i < recentlyActiveNeurons.size(); ++i) {
        for (size_t j = i + 1; j < recentlyActiveNeurons.size(); ++j) {
            Neuron* neuronA = recentlyActiveNeurons[i];
            Neuron* neuronB = recentlyActiveNeurons[j];
            
            // Get spike timing for STDP eligibility
            auto spikeA = neuronA->getSpikeHistory();
            auto spikeB = neuronB->getSpikeHistory();
            
            if (spikeA.empty() || spikeB.empty()) continue;
            
            Timestamp lastPreSpike = spikeA.back();
            Timestamp lastPostSpike = spikeB.back();
            
            float timeDifference = std::abs(static_cast<float>(lastPreSpike - lastPostSpike));
            
            // STDP window for formation: time difference within 10-100ms
            if (timeDifference > 10.0f && timeDifference < 100.0f) {
                // Calculate STDP eligibility based on time difference
                float stdpEligibility = std::exp(-timeDifference / 20.0f);  // Exponential decay
                
                // Apply formation probability
                float formationProb = stdpFormationProb * stdpEligibility;
                
                if (rng.bernoulli(formationProb)) {
                    // Check if synapse already exists
                    bool synapseExists = false;
                    for (const auto& synapse : region->getSynapses()) {
                        if ((synapse->getSourceNeuron() == neuronA->getId() && 
                             synapse->getDestinationNeuron() == neuronB->getId()) ||
                            (synapse->getSourceNeuron() == neuronB->getId() && 
                             synapse->getDestinationNeuron() == neuronA->getId())) {
                            synapseExists = true;
                            break;
                        }
                    }
                    
                    if (!synapseExists) {
                        // Form new synapse based on STDP timing
                        SynapseId synapseId = region->addSynapse(neuronA->getId(), neuronB->getId());
                        if (synapseId != INVALID_SYNAPSE_ID) {
                            auto* synapse = region->getSynapse(synapseId);
                            if (synapse) {
                                // Set synapse properties based on STDP timing
                                if (lastPreSpike < lastPostSpike) {
                                    // Pre before post: LTP-like formation
                                    synapse->setType(SynapseType::Excitatory);
                                    synapse->setWeight(0.3f + timeDifference * 0.002f);  // Weight increases with time diff
                                } else {
                                    // Post before pre: LTD-like or weak formation
                                    synapse->setType(SynapseType::Excitatory);
                                    synapse->setWeight(0.1f);
                                }
                                
                                // Enable STDP plasticity
                                synapse->enablePlasticity(false, true, false);
                                
                                NLM_LOG_INFO("Synaptogenesis: Formed STDP-based synapse (t_diff=" + 
                                            std::to_string(timeDifference) + "ms)");
                            }
                        }
                    }
                }
            }
        }
    }
}

void Synaptogenesis::applyNeuralCompetition(NeuralRegion* region, const std::vector<Neuron*>& neurons,
                                          RandomGenerator& rng, TimestepDuration dt) {
    if (!region || neurons.size() < 2) return;
    
    // Neural competition for limited resources
    // Neurons with higher activity get priority for new synapse formation
    
    std::vector<std::pair<Neuron*, float>> neuronActivityRanks;
    
    for (auto* neuron : neurons) {
        float activity = calculateNeuronActivity(neuron);
        neuronActivityRanks.push_back({neuron, activity});
    }
    
    // Sort neurons by activity (highest first)
    std::sort(neuronActivityRanks.begin(), neuronActivityRanks.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });n					// Calculate competition threshold
    size_t competitionThreshold = static_cast<size_t>(neurons.size() * pImpl->competitionFactor);
    
    // Determine which neurons get priority for new connections
    std::vector<Neuron*> highActivityNeurons;
    for (size_t i = 0; i < std::min(competitionThreshold, neuronActivityRanks.size()); ++i) {
        highActivityNeurons.push_back(neuronActivityRanks[i].first);
    }
    
    // Calculate available resources for synapse formation
    float availableResources = 1.0f - (static_cast<float>(region->getSynapseCount()) / 
                                     static_cast<float>(neurons.size() * neurons.size()));
    
    // Allocate resources to high-activity neurons
    float resourcePerNeuron = availableResources / std::max(1.0f, static_cast<float>(highActivityNeurons.size()));
    
    for (auto* neuron : highActivityNeurons) {
        // Higher activity neurons get more resources
        float neuronResources = resourcePerNeuron * 
                               (1.0f + neuronActivityRanks[&neuron - &neurons[0]].second / 10.0f);
        
        // Form new synapses with increased probability
        if (neuronResources > 0.0f) {
            NeuronId neuronId = neuron->getId();
            
            // Find target neurons for connection
            size_t targetCount = static_cast<size_t>(neuronResources * neurons.size());
            targetCount = std::min(targetCount, neurons.size() - 1);
            
            for (size_t i = 0; i < targetCount; ++i) {
                // Select target neuron (not self)
                size_t targetIdx;
                do {
                    targetIdx = rng.pickRandomIndex(static_cast<uint64_t>(neurons.size()));
                } while (targetIdx == &neuron - &neurons[0]);
                
                Neuron* targetNeuron = neurons[targetIdx];
                NeuronId targetId = targetNeuron->getId();
                
                // Check if synapse already exists
                bool synapseExists = false;
                for (const auto& synapse : region->getSynapses()) {
                    if ((synapse->getSourceNeuron() == neuronId && 
                         synapse->getDestinationNeuron() == targetId) ||
                        (synapse->getSourceNeuron() == targetId && 
                         synapse->getDestinationNeuron() == neuronId)) {
                        synapseExists = true;
                        break;
                    }
                }
                
                if (!synapseExists) {
                    // Form new synapse through competition
                    SynapseId synapseId = region->addSynapse(neuronId, targetId);
                    if (synapseId != INVALID_SYNAPSE_ID) {
                        auto* synapse = region->getSynapse(synapseId);
                        if (synapse) {
                            synapse->setType(SynapseType::Excitatory);
                            synapse->setWeight(0.2f);  // Moderate initial weight
                            
                            NLM_LOG_INFO("Synaptogenesis: Competition-based synapse formation");
                        }
                    }
                }
            }
        }
    }
}

void Synaptogenesis::applyTargetDensityControl(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    // Adjust formation rate to reach target density
    float densityError = pImpl->targetDensity - pImpl->currentDensity;
    
    if (std::abs(densityError) > 0.01f) {  // If error > 1%
        // Calculate required formation rate adjustment
        float adjustmentFactor = 1.0f + densityError * 10.0f;  // Proportional control
        
        // Apply adjustment with bounds
        adjustmentFactor = std::clamp(adjustmentFactor, 0.5f, 2.0f);
        
        // Apply to formation rate
        pImpl->formationRate *= adjustmentFactor;
        
        NLM_LOG_INFO("Synaptogenesis: Target density adjustment - Error=" + 
                    std::to_string(densityError) + ", Factor=" + std::to_string(adjustmentFactor));
    }
}

void Synaptogenesis::updateActivityHistory(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Update activity history for future formation decisions
    const auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        
        // Decay old activity history
        for (auto* neuron : neurons) {
            // Simulate activity history update
            // In a real implementation, this would track actual neural activity
            float activity = calculateNeuronActivity(neuron);
            
            // Update recent active neurons set
            if (activity > pImpl->activityThreshold) {
                pImpl->recentlyActiveNeurons.insert(neuron->getId());
            }
        }
    }
    
    // Decay the activity history
    // Note: In a real implementation, this would involve actual decay logic
}

void Synaptogenesis::applyMolecularStabilization(NeuralRegion* region, RandomGenerator& rng, TimestepDuration dt) {
    if (!region) return;
    
    // Apply molecular mechanisms for synaptic stabilization
    // This simulates the biological process of synaptic maturation and stabilization
    
    for (const auto& synapse : region->getSynapses()) {
        // Find synapse in molecular tracking arrays
        size_t synapseIndex = findSynapseIndex(synapse.get()->getId());
        
        if (synapseIndex < pImpl->synapseStabilizationFactors.size()) {
            // Update stabilization factor based on age and activity
            float ageFactor = std::min(1.0f, static_cast<float>(pImpl->birthDates[synapseIndex]) / 100.0f);
            float activityFactor = calculateSynapseActivity(synapse.get());
            
            // Update stabilization factor
            pImpl->synapseStabilizationFactors[synapseIndex] *= 
                (1.0f + ageFactor * 0.1f) * (1.0f + activityFactor * 0.05f);
            
            // Apply stabilization threshold
            if (pImpl->synapseStabilizationFactors[synapseIndex] > 2.0f) {
                // Synapse is stabilized, reduce metabolic cost
                synapse->setWeight(synapse->getWeight() * 1.1f);  // Slight strengthening
                NLM_LOG_INFO("Synaptogenesis: Synapse stabilization completed");
            }
        }
    }
}

float Synaptogenesis::getStageMultiplier(Brain* brain) const {
    if (!brain) return 1.0f;
    
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    // Return stage-specific multiplier
    switch (stage) {
        case DevelopmentalStage::Initial:
            return pImpl->stageSpecificRates[0];
        case DevelopmentalStage::CriticalPeriod:
            return pImpl->stageSpecificRates[1];
        case DevelopmentalStage::Maturation:
            return pImpl->stageSpecificRates[2];
        case DevelopmentalStage::Adult:
            return pImpl->stageSpecificRates[3];
        case DevelopmentalStage::Aging:
            return pImpl->stageSpecificRates[4];
        default:
            return 1.0f;
    }
}

float Synaptogenesis::calculateNeuronActivity(Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Calculate neuron activity based on firing rate, spike history, and membrane potential
    float firingRate = neuron->getFiringRate();
    float membranePotential = neuron->getMembranePotential();
    
    // Normalize firing rate (typical range 0-100 Hz)
    float normalizedFiringRate = firingRate / 100.0f;
    
    // Calculate membrane potential relative to threshold
    float threshold = neuron->getThreshold();
    float potentialRelative = (membranePotential - threshold) / 50.0f;  // Assuming threshold difference ~50mV
    
    // Combine factors
    float activity = (normalizedFiringRate * 0.6f) + (potentialRelative * 0.4f);
    
    return std::max(0.0f, std::min(1.0f, activity));
}

float Synaptogenesis::calculateHebbianEligibility(Neuron* neuronA, Neuron* neuronB, TimestepDuration dt) const {
    if (!neuronA || !neuronB) return 0.0f;
    
    // Calculate Hebbian eligibility based on correlated activity patterns
    float activityA = calculateNeuronActivity(neuronA);
    float activityB = calculateNeuronActivity(neuronB);
    
    // Check for temporal correlation
    auto spikesA = neuronA->getSpikeHistory();
    auto spikesB = neuronB->getSpikeHistory();
    
    float temporalCorrelation = 0.0f;
    if (!spikesA.empty() && !spikesB.empty()) {
        Timestamp lastSpikeA = spikesA.back();
        Timestamp lastSpikeB = spikesB.back();
        float timeDiff = std::abs(static_cast<float>(lastSpikeA - lastSpikeB));
        
        // Normalize time difference
        temporalCorrelation = std::exp(-timeDiff / 50.0f);  // 50ms time constant
    }
    
    // Combine activity and temporal correlation
    float eligibility = (activityA + activityB) * 0.5f * temporalCorrelation;
    
    return eligibility;
}

float Synaptogenesis::calculateSynapseActivity(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    // Calculate synapse activity based on presynaptic and postsynaptic activity
    float presynapticActivity = 0.0f;
    float postsynapticActivity = 0.0f;
    
    // This would require access to neurons in a real implementation
    // For now, return a proxy based on synapse weight
    return synapse->getWeight() * 0.5f;
}

size_t Synaptogenesis::findSynapseIndex(SynapseId synapseId) const {
    for (size_t i = 0; i < pImpl->recentSynapses.size(); ++i) {
        if (pImpl->recentSynapses[i] == synapseId) {
            return i;
        }
    }
    return pImpl->synapseStabilizationFactors.size();  // Return out of bounds if not found
}

void Synaptogenesis::updateRecentSynapseTracking(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Remove old synapses from tracking
    pImpl->recentSynapses.erase(
        std::remove_if(pImpl->recentSynapses.begin(), pImpl->recentSynapses.end(),
                      [this, brain](SynapseId synapseId) {
                          if (!brain) return true;
                          
                          // Check if synapse still exists
                          for (const auto& region : brain->getRegions()) {
                              if (region) {
                                  for (const auto& synapse : region->getSynapses()) {
                                      if (synapse->getId() == synapseId) {
                                          return false;  // Synapse still exists
                                      }
                                  }
                              }
                          }
                          return true;  // Synapse no longer exists
                      }),
        pImpl->recentSynapses.end()
    );
}

} // namespace nlm