#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            break;
    }
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double age_;      // Overall developmental age
    
    // Development parameters
    double synaptogenesisRate;      // Rate of new synapse formation
    double pruningRate;             // Rate of synapse elimination
    double maturationRate;          // Rate of synaptic strengthening
    double structuralPlasticity;    // Structural plasticity coefficient
    
    // Critical period parameters
    double criticalPeriodDuration;  // Duration of critical period
    double criticalPeriodProgress;  // Progress through critical period
    bool criticalPeriodActive;     // Whether critical period is active
    
    // Developmental timeline (in simulation seconds)
    static constexpr double INITIAL_STAGE_DURATION = 60.0;   // 60 seconds
    static constexpr double CRITICAL_PERIOD_DURATION = 300.0; // 5 minutes
    static constexpr double MATURATION_DURATION = 600.0;   // 10 minutes
    static constexpr double ADULT_DURATION = 3600.0;       // 1 hour
    static constexpr double AGING_DURATION = 7200.0;       // 2 hours
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), 
             stageAge(0.0), age_(0.0),
             synaptogenesisRate(0.0001f), pruningRate(0.00001f), maturationRate(0.001f),
             structuralPlasticity(1.0f), criticalPeriodDuration(300.0),
             criticalPeriodProgress(0.0), criticalPeriodActive(true) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stepsInCurrentStage = 0;
    
    // Reset critical period status
    if (stage == DevelopmentalStage::CriticalPeriod) {
        pImpl->criticalPeriodActive = true;
        pImpl->criticalPeriodProgress = 0.0;
    } else {
        pImpl->criticalPeriodActive = false;
    }
    
    NLM_LOG_INFO("Development system entered stage: " + std::string(getStageName()));
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            pImpl->criticalPeriodActive = false;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            break;
    }
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    // Update developmental processes based on current stage
    if (!brain) return;
    
    // Update age
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    ++pImpl->stepsInCurrentStage;
    
    // Auto-advance stage based on time in stage
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > pImpl->INITIAL_STAGE_DURATION) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > pImpl->CRITICAL_PERIOD_DURATION) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > pImpl->MATURATION_DURATION) {
        advanceStage();
    }
    
    // Apply developmental processes for current stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            applyInitialDevelopment(brain);
            break;
        case DevelopmentalStage::CriticalPeriod:
            applyCriticalPeriodDevelopment(brain);
            break;
        case DevelopmentalStage::Maturation:
            applyMaturation(brain);
            break;
        case DevelopmentalStage::Adult:
            applyAdultDevelopment(brain);
            break;
        case DevelopmentalStage::Aging:
            applyAging(brain);
            break;
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Update developmental processes with random number generator
    if (!brain) return;
    
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Update critical period progress
    if (pImpl->criticalPeriodActive) {
        pImpl->criticalPeriodProgress = std::min(1.0, pImpl->stageAge / pImpl->criticalPeriodDuration);
    }
    
    // Update eligibility traces based on developmental stage
    updateEligibilityTraces(brain, dt);
    
    // Update synaptic strength based on developmental processes
    updateSynapticStrength(brain, dt);
    
    // Auto-advance stage based on developmental age
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > pImpl->INITIAL_STAGE_DURATION) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > pImpl->CRITICAL_PERIOD_DURATION) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > pImpl->MATURATION_DURATION) {
        advanceStage();
    }
    
    // Apply developmental processes for current stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            applyInitialDevelopment(brain);
            break;
        case DevelopmentalStage::CriticalPeriod:
            applyCriticalPeriodDevelopment(brain);
            break;
        case DevelopmentalStage::Maturation:
            applyMaturation(brain);
            break;
        case DevelopmentalStage::Adult:
            applyAdultDevelopment(brain);
            break;
        case DevelopmentalStage::Aging:
            applyAging(brain);
            break;
    }
}

void DevelopmentSystem::applyInitialDevelopment(Brain* brain) {
    // Initial development: form basic connections
    if (!brain) return;
    
    // Synaptogenesis: create new synapses
    if (pImpl->synaptogenesisRate > 0.0f) {
        // Calculate how many new synapses to create
        int64_t totalNeurons = brain->getTotalNeuronCount();
        int64_t targetSynapses = totalNeurons * 2; // Target 2 synapses per neuron
        
        auto regions = brain->getAllRegions();
        for (auto* region : regions) {
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) continue;
            
            // Create new synapses with probability based on rate
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(0.0f, 1.0f);
            
            for (auto* preNeuron : neurons) {
                if (dis(gen) < pImpl->synaptogenesisRate * 0.1f) {
                    // Create synapse to random post-synaptic neuron
                    if (neurons.size() > 1) {
                        std::uniform_int_distribution<size_t> postDist(0, neurons.size() - 1);
                        size_t postIndex = postDist(gen);
                        
                        while (postIndex == neurons.size() / 2) postIndex = postDist(gen); // Avoid self-connections
                        
                        auto* postNeuron = neurons[postIndex];
                        
                        // Create new synapse
                        SynapseId synapseId = generateNextSynapseId();
                        SynapseHandle synapseHandle(synapseId);
                        
                        // Add synapses to both neurons
                        preNeuron->addOutgoingSynapse(synapseHandle);
                        postNeuron->addIncomingSynapse(synapseHandle);
                        
                        // Initialize synapse
                        auto synapse = std::make_unique<Synapse>(synapseId, preNeuron->getId(), postNeuron->getId());
                        synapse->initializeRandom(randomGenerator); // TODO: add access to rng
                        
                        // Add to brain's synapse registry
                        brain->addSynapse(std::move(synapse));
                    }
                }
            }
        }
    }
}

void DevelopmentSystem::applyCriticalPeriodDevelopment(Brain* brain) {
    // Critical period: enhanced plasticity and refinement
    if (!brain) return;
    
    // Increase plasticity modifier during critical period
    float plasticityBoost = 2.0f - pImpl->criticalPeriodProgress; // From 2.0 down to 1.0
    
    // Apply Hebbian-like refinement: strengthen correlated synapses
    auto regions = brain->getAllRegions();
    for (auto* region : regions) {
        auto synapses = region->getSynapses();
        auto neurons = region->getAllNeurons();
        
        if (synapses.empty() || neurons.empty()) continue;
        
        // For each synapse, check if pre and post neurons have correlated activity
        for (auto& synapse : synapses) {
            // Get spike history
            auto preSpikes = synapse->getPreSpikeHistory();
            auto postSpikes = synapse->getPostSpikeHistory();
            
            // If both neurons have spiked recently, strengthen connection
            if (!preSpikes.empty() && !postSpikes.empty()) {
                float strength = calculateSpikeCorrelation(preSpikes, postSpikes);
                if (strength > 0.5f) {
                    // Strengthen synapse during critical period
                    float currentWeight = synapse->getWeight();
                    float weightIncrease = (1.0f - currentWeight) * plasticityBoost * 0.1f;
                    synapse->addToWeight(weightIncrease);
                }
            }
        }
    }
}

void DevelopmentSystem::applyMaturation(Brain* brain) {
    // Maturation: synaptic strengthening and stabilization
    if (!brain) return;
    
    // Apply maturation to all synapses
    auto regions = brain->getAllRegions();
    for (auto* region : regions) {
        auto synapses = region->getSynapses();
        
        for (auto& synapse : synapses) {
            float currentWeight = synapse->getWeight();
            
            // Apply maturity-dependent strengthening
            float maturityFactor = getMaturationFactor();
            
            // Apply homeostatic plasticity to maintain stability
            float targetWeight = getTargetWeightForSynapse(synapse.get());
            float homeostaticChange = (targetWeight - currentWeight) * maturityFactor * 0.05f;
            
            synapse->addToWeight(homeostaticChange);
            
            // Apply Hebbian maturation
            float hebbianBoost = calculateHebbianMaturation(synapse.get());
            synapse->addToWeight(hebbianBoost);
        }
    }
}

void DevelopmentSystem::applyAdultDevelopment(Brain* brain) {
    // Adult development: maintenance and fine-tuning
    if (!brain) return;
    
    // Reduced plasticity in adult stage
    float adultPlasticity = 0.2f;
    
    // Apply activity-dependent plasticity
    auto regions = brain->getAllRegions();
    for (auto* region : regions) {
        auto synapses = region->getSynapses();
        
        for (auto& synapse : synapses) {
            // Apply adult plasticity rules
            float weightChange = calculateAdultPlasticity(synapse.get());
            synapse->addToWeight(weightChange * adultPlasticity);
        }
    }
}

void DevelopmentSystem::applyAging(Brain* brain) {
    // Aging: synaptic weakening and loss
    if (!brain) return;
    
    // Apply age-related synaptic changes
    float agingFactor = getAgingFactor();
    
    auto regions = brain->getAllRegions();
    for (auto* region : regions) {
        auto synapses = region->getSynapses();
        
        // Randomly prune synapses
        for (size_t i = 0; i < synapses.size(); ++i) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(0.0f, 1.0f);
            
            if (dis(gen) < pImpl->pruningRate * agingFactor) {
                // Remove this synapse
                auto synapse = synapses[i];
                brain->removeSynapse(synapse->getId());
                
                // Remove from neuron lists (this is complex - simplified here)
                // In a full implementation, would update neuron synaptic connections
            }
        }
    }
}

void DevelopmentSystem::updateEligibilityTraces(Brain* brain, TimestepDuration dt) {
    // Update eligibility traces for reward-modulated learning
    if (!brain) return;
    
    auto regions = brain->getAllRegions();
    for (auto* region : regions) {
        auto synapses = region->getSynapses();
        
        for (auto& synapse : synapses) {
            // Decay eligibility trace
            synapse->decayEligibilityTrace(0.01f * dt);
        }
    }
}

void DevelopmentSystem::updateSynapticStrength(Brain* brain, TimestepDuration dt) {
    // Update synaptic strength based on developmental stage
    if (!brain) return;
    
    float plasticityModifier = getPlasticityModifier();
    
    auto regions = brain->getAllRegions();
    for (auto* region : regions) {
        auto synapses = region->getSynapses();
        
        for (auto& synapse : synapses) {
            // Apply developmental stage-specific plasticity
            synapse->step(0.0); // Use default time
            
            // Modify plasticity based on developmental stage
            float currentLTP = synapse->getPlasticityFlags().stdp ? 0.01f : 0.0f;
            float currentLTD = synapse->getPlasticityFlags().stdp ? 0.012f : 0.0f;
            
            // Scale by developmental stage
            currentLTP *= plasticityModifier;
            currentLTD *= plasticityModifier;
        }
    }
}

float DevelopmentSystem::calculateSpikeCorrelation(const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Simple correlation: count spike pairs with small time differences
    int correlatedPairs = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 5.0f) { // Within 5ms
                correlatedPairs++;
            }
        }
    }
    
    return static_cast<float>(correlatedPairs) / std::sqrt(preSpikes.size() * postSpikes.size());
}

float DevelopmentSystem::calculateHebbianMaturation(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    // Hebbian-like maturation: correlated activity strengthens connections
    auto preSpikes = synapse->getPreSpikeHistory();
    auto postSpikes = synapse->getPostSpikeHistory();
    
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Calculate pairwise correlation
    float correlation = calculateSpikeCorrelation(preSpikes, postSpikes);
    
    // Hebbian learning rule: Δw ∝ x*y (pre * post)
    float hebbianChange = correlation * 0.01f * pImpl->maturationRate;
    
    return hebbianChange;
}

float DevelopmentSystem::calculateAdultPlasticity(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    // Adult plasticity: activity-dependent with homeostatic regulation
    float currentWeight = synapse->getWeight();
    
    // Homeostatic component
    float homeostatic = (0.0f - currentWeight) * 0.02f; // Target toward 0
    
    // Activity-dependent component (simplified)
    float activityComponent = 0.0f;
    auto preSpikes = synapse->getPreSpikeHistory();
    auto postSpikes = synapse->getPostSpikeHistory();
    
    if (!preSpikes.empty() && !postSpikes.empty()) {
        activityComponent = 0.01f; // Positive for correlated activity
    }
    
    return homeostatic + activityComponent;
}

float DevelopmentSystem::getMaturationFactor() const {
    // Return a factor representing how mature the brain is
    float ageRatio = std::min(1.0, age_ / (pImpl->MATURATION_DURATION * 2.0f));
    return ageRatio;
}

float DevelopmentSystem::getTargetWeightForSynapse(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    // Target weight based on synapse type
    if (synapse->isExcitatory()) {
        return 0.2f; // Moderate excitation
    } else if (synapse->isInhibitory()) {
        return -0.1f; // Moderate inhibition
    } else {
        return 0.0f; // Other types toward zero
    }
}

float DevelopmentSystem::getAgingFactor() const {
    // Factor representing age-related decline
    if (age_ < pImpl->ADULT_DURATION) {
        return 1.0f; // Young adult
    } else if (age_ < pImpl->AGING_DURATION) {
        // Gradual increase with age
        return 1.0f + (age_ - pImpl->ADULT_DURATION) / 3600.0f;
    } else {
        return 2.0f; // Significant aging effects
    }
}

SynapseId DevelopmentSystem::generateNextSynapseId() {
    static SynapseId nextId = 1000;
    return nextId++;
}

// Note: We need access to random generator for some operations
// This would be passed in a full implementation
RandomGenerator DevelopmentSystem::randomGenerator;

} // namespace nlm
