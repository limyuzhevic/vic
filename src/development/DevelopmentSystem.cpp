#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double synaptogenesisRate;  // Rate of new synapse formation
    double pruningRate;        // Rate of synapse elimination
    double maturationProgress;  // Progress through neural maturation
    double criticalPeriodProgress; // Progress through critical period
    double targetDensity;       // Target synapse density for synaptogenesis
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), 
             stageAge(0.0), synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             maturationProgress(0.0), criticalPeriodProgress(0.0), targetDensity(0.1f) {}
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

void DevelopmentSystem::initializeDevelopment(Brain* brain) {
    // Initialize development system for a brain
    if (!brain) return;
    
    // Set initial developmental stage
    setStage(DevelopmentalStage::Initial);
    
    // Initialize maturation system
    // This would normally involve more complex initialization
    pImpl->maturationProgress = 0.0;
    
    // Initialize synaptogenesis parameters
    pImpl->synaptogenesisRate = calculateSynaptogenesisRateInitialStage(0.0f);
    pImpl->targetDensity = 0.1f; // Target synapse density
    
    // Initialize pruning parameters
    pImpl->pruningRate = calculatePruningRateInitialStage(0.0f);
    pImpl->threshold = 0.01f; // Pruning threshold
    
    NLM_LOG_INFO("Development system initialized for brain");
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
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Update developmental parameters based on stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Initial stage: setup neural architecture
            // Higher synaptogenesis, lower pruning
            pImpl->synaptogenesisRate = 0.0005f;
            pImpl->pruningRate = 0.000001f;
            pImpl->maturationProgress = 0.1f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: high plasticity, rapid learning
            // Update critical period progress
            pImpl->criticalPeriodProgress = std::min(1.0, pImpl->stageAge / 300.0);
            pImpl->synaptogenesisRate = 0.0003f * (1.0 + pImpl->criticalPeriodProgress);
            pImpl->pruningRate = 0.000005f * (1.0 - pImpl->criticalPeriodProgress * 0.5f);
            pImpl->maturationProgress = 0.3f + 0.4f * pImpl->criticalPeriodProgress;
            break;
        case DevelopmentalStage::Maturation:
            // Maturation: refine neural connections
            pImpl->synaptogenesisRate = 0.0001f;
            pImpl->pruningRate = 0.00002f;
            pImpl->maturationProgress = 0.7f + 0.3f * (pImpl->stageAge / 600.0);
            break;
        case DevelopmentalStage::Adult:
            // Adult stage: stable, limited plasticity
            pImpl->synaptogenesisRate = 0.00001f;
            pImpl->pruningRate = 0.00003f;
            pImpl->maturationProgress = 1.0f;
            break;
        case DevelopmentalStage::Aging:
            // Aging stage: gradual degradation, structural changes
            pImpl->synaptogenesisRate = 0.000001f;
            pImpl->pruningRate = 0.00005f;
            pImpl->maturationProgress = 1.0f;
            break;
    }
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Implement developmental stages (critical periods, maturation, etc.)
    // Activity-dependent regulation and integration with neural development
    
    // Update developmental age
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Update developmental parameters based on stage and activity
    updateDevelopmentalParameters(brain, dt);
    
    // Apply maturation effects based on developmental stage
    applyMaturation(brain);
    
    // Apply synaptogenesis effects during development
    applySynaptogenesis(brain, rng);
    
    // Apply pruning effects during development
    applyPruning(brain, rng);
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    }
}

void DevelopmentSystem::updateDevelopmentalParameters(Brain* brain, TimestepDuration dt) {
    // Update developmental parameters based on neural activity and stage
    // Complex activity-dependent regulation of developmental processes
    
    // Calculate network-wide activity metrics
    float avgFiringRate = calculateNetworkFiringRate(brain);
    float networkActivity = calculateNetworkActivity(brain);
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Initial stage: setup neural architecture with activity-dependent tuning
            // Adjust based on overall network activity
            pImpl->synaptogenesisRate = calculateSynaptogenesisRateInitialStage(networkActivity);
            pImpl->pruningRate = calculatePruningRateInitialStage(networkActivity);
            pImpl->maturationProgress = 0.1f;
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: activity-dependent synaptic strengthening/weakening
            // Implement Hebbian-like activity-dependent plasticity
            pImpl->criticalPeriodProgress = std::min(1.0, pImpl->stageAge / 300.0);
            
            // Adjust rates based on critical period progress and activity
            pImpl->synaptogenesisRate = calculateSynaptogenesisRateCriticalPeriod(networkActivity, avgFiringRate);
            pImpl->pruningRate = calculatePruningRateCriticalPeriod(networkActivity, avgFiringRate);
            
            // Apply activity-dependent modulation to strengthen learning
            applyCriticalPeriodModulation(brain, networkActivity);
            break;
            
        case DevelopmentalStage::Maturation:
            // Maturation: refine based on functional connectivity and activity patterns
            pImpl->synaptogenesisRate = calculateSynaptogenesisRateMaturation(networkActivity);
            pImpl->pruningRate = calculatePruningRateMaturation(networkActivity);
            
            // Apply homeostatic regulation to stabilize network
            pImpl->maturationProgress = std::min(1.0, pImpl->stageAge / 600.0);
            applyHomeostaticRegulation(brain);
            break;
            
        case DevelopmentalStage::Adult:
            // Adult stage: fine-tune based on learning and maintenance
            pImpl->synaptogenesisRate = calculateSynaptogenesisRateAdult(networkActivity);
            pImpl->pruningRate = calculatePruningRateAdult(networkActivity);
            
            // Maintain adult properties with subtle regulation
            applyAdultHomeostasis(brain);
            break;
            
        case DevelopmentalStage::Aging:
            // Aging stage: gradual structural changes with adaptive compensation
            pImpl->synaptogenesisRate = calculateSynaptogenesisRateAging(networkActivity);
            pImpl->pruningRate = calculatePruningRateAging(networkActivity, age_);
            
            // Apply aging-specific adaptations
            applyAgingAdaptations(brain);
            break;
    }
}

void DevelopmentSystem::applyCriticalPeriodModulation(Brain* brain, float networkActivity) {
    // Enhance plasticity during critical period based on activity
    // Activity-dependent sensitization of neural circuits
    
    float plasticityMultiplier = 1.0f + networkActivity * 0.5f; // High activity increases plasticity
    
    // Enhance maturation during critical period
    if (pImpl->maturationProgress < 0.5f) {
        pImpl->maturationProgress += 0.001f * plasticityMultiplier;
    }
    
    // Increase synaptic competition during critical period
    for (auto& region : brain->getRegions()) {
        auto& neurons = region->getAllNeurons();
        for (auto& neuron : neurons) {
            // Enhance responsiveness based on critical period progress
            if (pImpl->criticalPeriodProgress > 0.3f) {
                float responsivenessBoost = (pImpl->criticalPeriodProgress - 0.3f) * 0.1f * plasticityMultiplier;
                
                // Enhance neuron excitability
                auto& state = neuron->getState();
                state.restingPotential -= responsivenessBoost * 0.5f;
                
                // Lower firing threshold for enhanced sensitivity
                float thresholdReduction = responsivenessBoost * 0.02f;
                float currentThreshold = neuron->getThreshold();
                neuron->setThreshold(currentThreshold - thresholdReduction);
            }
        }
    }
}

void DevelopmentSystem::applyHomeostaticRegulation(Brain* brain) {
    // Homeostatic regulation during maturation
    // Stabilize network activity and prevent runaway excitation/inhibition
    
    float targetActivityLevel = 0.5f; // Target for optimal network function
    float currentActivity = calculateNetworkActivity(brain);
    float deviation = currentActivity - targetActivityLevel;
    
    // Apply opposing adjustments based on deviation
    for (auto& region : brain->getRegions()) {
        auto& neurons = region->getAllNeurons();
        for (auto& neuron : neurons) {
            auto& state = neuron->getState();
            
            // Adjust synaptic efficacy based on network-wide activity
            float efficacyAdjustment = -deviation * 0.01f * (pImpl->maturationProgress * 2.0f);
            float currentEfficacy = neuron->getEfficacy();
            neuron->setEfficacy(currentEfficacy + efficacyAdjustment);
            
            // Adjust firing threshold
            float thresholdAdjustment = -deviation * 0.005f;
            float currentThreshold = neuron->getThreshold();
            neuron->setThreshold(currentThreshold + thresholdAdjustment);
            
            // Adjust resting potential
            float restingAdjustment = -deviation * 0.003f;
            state.restingPotential += restingAdjustment;
        }
    }
}

void DevelopmentSystem::applyAdultHomeostasis(Brain* brain) {
    // Maintain adult neural properties with subtle regulation
    // Fine-tune for optimal performance in stable adult phase
    
    float adultStabilityFactor = 1.0f - pImpl->maturationProgress; // More stable in adult stage
    
    for (auto& region : brain->getRegions()) {
        auto& neurons = region->getAllNeurons();
        for (auto& neuron : neurons) {
            auto& state = neuron->getState();
            
            // Maintain firing threshold around optimal adult range
            float targetThreshold = -50.0f;
            float thresholdDrift = (pImpl->stageAge * 0.0001f) * adultStabilityFactor;
            state.threshold += (targetThreshold - state.threshold) * thresholdDrift;
            
            // Stabilize membrane potential
            float targetMembranePotential = -65.0f;
            state.membranePotential += (targetMembranePotential - state.membranePotential) * 0.001f * adultStabilityFactor;
        }
    }
}

void DevelopmentSystem::applyAgingAdaptations(Brain* brain) {
    // Apply aging-specific adaptations for maintaining function
    // Gradual structural changes with compensatory mechanisms
    
    float agingProgress = std::min(1.0f, static_cast<float>(age_) / 3600.0f); // Aging over longer period
    
    for (auto& region : brain->getRegions()) {
        auto& neurons = region->getAllNeurons();
        for (auto& neuron : neurons) {
            auto& state = neuron->getState();
            
            // Gradual reduction in excitability with aging
            float agingFactor = agingProgress * 0.01f;
            state.threshold += agingFactor * 0.5f; // Increase threshold (less excitable)
            
            // Compensatory strengthening of remaining connections
            float compensationFactor = agingFactor * 0.5f;
            float currentEfficacy = neuron->getEfficacy();
            neuron->setEfficacy(currentEfficacy + compensationFactor);
            
            // Adjust resting potential towards more negative values
            float restingAdjustment = -agingFactor * 0.3f;
            state.restingPotential += restingAdjustment;
        }
    }
}

float DevelopmentSystem::calculateNetworkFiringRate(Brain* brain) const {
    if (!brain) return 0.0f;
    
    float totalFiringRate = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            totalFiringRate += neuron->getFiringRate();
            ++neuronCount;
        }
    }
    
    return (neuronCount > 0) ? (totalFiringRate / neuronCount) : 0.0f;
}

float DevelopmentSystem::calculateNetworkActivity(Brain* brain) const {
    if (!brain) return 0.0f;
    
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            const auto& state = neuron->getState();
            
            // Calculate activity based on firing state and membrane potential
            float activity = 0.0f;
            if (state.firingState == FiringState::Active) {
                activity = 1.0f;
            } else if (state.firingState == FiringState::Refractory) {
                activity = 0.5f;
            } else {
                activity = std::max(0.0f, (std::abs(state.membranePotential - state.restingPotential) / 20.0f));
            }
            
            totalActivity += activity;
            ++neuronCount;
        }
    }
    
    return (neuronCount > 0) ? (totalActivity / neuronCount) : 0.0f;
}

float DevelopmentSystem::calculateSynaptogenesisRateInitialStage(float networkActivity) const {
    // Synaptogenesis rate in initial developmental stage
    return 0.0005f * (1.0f + networkActivity * 0.5f);
}

float DevelopmentSystem::calculatePruningRateInitialStage(float networkActivity) const {
    // Pruning rate in initial developmental stage
    return 0.000001f * (1.0f - networkActivity * 0.3f);
}

float DevelopmentSystem::calculateSynaptogenesisRateCriticalPeriod(float networkActivity, float avgFiringRate) const {
    // Synaptogenesis rate during critical period
    float rate = 0.0003f * (2.0f - pImpl->criticalPeriodProgress);
    
    // Modulate by network activity (higher activity increases synaptogenesis)
    rate *= (1.0f + networkActivity * 0.5f);
    
    // Modulate by average firing rate (moderate firing promotes formation)
    rate *= (1.0f + avgFiringRate * 2.0f);
    
    return rate;
}

float DevelopmentSystem::calculatePruningRateCriticalPeriod(float networkActivity, float avgFiringRate) const {
    // Pruning rate during critical period
    float rate = 0.000005f * (1.0f + pImpl->criticalPeriodProgress * 0.5f);
    
    // Modulate by network activity (higher activity increases pruning)
    rate *= (1.0f - networkActivity * 0.3f);
    
    // Modulate by average firing rate
    rate *= (1.0f + avgFiringRate);
    
    return rate;
}

float DevelopmentSystem::calculateSynaptogenesisRateMaturation(float networkActivity) const {
    // Synaptogenesis rate during maturation
    float rate = 0.0001f * (1.0f - pImpl->maturationProgress);
    
    // Modulate by network activity
    rate *= (1.0f + networkActivity * 0.2f);
    
    return rate;
}

float DevelopmentSystem::calculatePruningRateMaturation(float networkActivity) const {
    // Pruning rate during maturation
    float rate = 0.00002f * pImpl->maturationProgress;
    
    // Modulate by network activity
    rate *= (1.0f + networkActivity * 0.5f);
    
    return rate;
}

float DevelopmentSystem::calculateSynaptogenesisRateAdult(float networkActivity) const {
    // Synaptogenesis rate in adult stage (stable, minimal)
    return 0.00001f * (1.0f + networkActivity * 0.1f);
}

float DevelopmentSystem::calculatePruningRateAdult(float networkActivity) const {
    // Pruning rate in adult stage (stable, minimal)
    return 0.00003f * (1.0f - networkActivity * 0.2f);
}

float DevelopmentSystem::calculateSynaptogenesisRateAging(float networkActivity) const {
    // Synaptogenesis rate in aging stage (very low)
    return 0.000001f * (1.0f + networkActivity * 0.05f);
}

float DevelopmentSystem::calculatePruningRateAging(float networkActivity, double currentAge) const {
    // Pruning rate in aging stage (increases with age)
    float ageFactor = static_cast<float>(std::min(1.0, currentAge / 1800.0)); // 30 minutes = full aging
    float rate = 0.00003f * (1.0f + ageFactor);
    
    // Modulate by network activity (higher activity may require more pruning)
    rate *= (1.0f + networkActivity * 0.3f);
    
    return rate;
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f + pImpl->criticalPeriodProgress * 0.2f;
        case DevelopmentalStage::Maturation: return 0.5f + pImpl->maturationProgress * 0.3f;
        case DevelopmentalStage::Adult: return 0.2f + (1.0f - pImpl->maturationProgress) * 0.3f;
        case DevelopmentalStage::Aging: return 0.1f;
        default: return 0.5f;
    }
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

void DevelopmentSystem::applyMaturation(Brain* brain) {
    // Apply maturation effects to neurons based on developmental stage
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        auto& neurons = region->getAllNeurons();
        for (auto& neuron : neurons) {
            auto& state = neuron->getState();
            
            // Apply stage-dependent maturation effects
            switch (pImpl->stage) {
                case DevelopmentalStage::CriticalPeriod:
                    // Enhance responsiveness during critical period
                    if (state.firingState == FiringState::Resting) {
                        float activationBoost = getCriticalPeriodProgress() * 0.1f;
                        state.restingPotential -= activationBoost;
                    }
                    break;
                case DevelopmentalStage::Maturation:
                    // Refine firing properties
                    if (pImpl->maturationProgress > 0.5f) {
                        // Stabilize firing threshold
                        float targetThreshold = -50.0f;
                        state.threshold += (targetThreshold - state.threshold) * 0.01f;
                    }
                    break;
                case DevelopmentalStage::Adult:
                    // Maintain adult properties
                    break;
                case DevelopmentalStage::Aging:
                    // Gradual reduction in excitability
                    state.threshold += 0.001f; // Increase threshold (less excitable)
                    break;
                default:
                    break;
            }
        }
    }
}

void DevelopmentSystem::applySynaptogenesis(Brain* brain, RandomGenerator& rng) {
    // Apply synaptogenesis effects during development
    if (!brain) return;
    
    // This would typically interact with the Synaptogenesis system
    // For now, log developmental progress
    if (getSynaptogenesisRate() > 0.0f) {
        NLM_LOG_INFO("Synaptogenesis active in " + std::string(getStageName()) + 
                     " stage (rate: " + std::to_string(getSynaptogenesisRate()) + ")");
    }
}

void DevelopmentSystem::applyPruning(Brain* brain, RandomGenerator& rng) {
    // Apply pruning effects during development
    if (!brain) return;
    
    // This would typically interact with the Pruning system
    // For now, log developmental progress
    if (getPruningRate() > 0.0f) {
        NLM_LOG_INFO("Pruning active in " + std::string(getStageName()) + 
                     " stage (rate: " + std::to_string(getPruningRate()) + ")");
    }
}

float DevelopmentSystem::getSynaptogenesisRate() const {
    return static_cast<float>(pImpl->synaptogenesisRate);
}

float DevelopmentSystem::getPruningRate() const {
    return static_cast<float>(pImpl->pruningRate);
}

void DevelopmentSystem::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = rate;
}

void DevelopmentSystem::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

float DevelopmentSystem::getMaturationProgress() const {
    return static_cast<float>(pImpl->maturationProgress);
}

float DevelopmentSystem::getCriticalPeriodEnd() const {
    return 300.0f; // Critical period lasts 300 simulation time units
}

bool DevelopmentSystem::isMaturationComplete() const {
    return pImpl->maturationProgress >= 1.0f;
}

bool DevelopmentSystem::isCriticalPeriodComplete() const {
    return pImpl->criticalPeriodProgress >= 1.0f;
}

float DevelopmentSystem::calculateNetworkActivity(Brain* brain) {
    if (!brain) return 0.0f;
    
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            const auto& state = neuron->getState();
            
            // Calculate activity based on firing state and membrane potential
            float activity = 0.0f;
            if (state.firingState == FiringState::Active) {
                activity = 1.0f;
            } else if (state.firingState == FiringState::Refractory) {
                activity = 0.5f;
            } else {
                activity = std::max(0.0f, (std::abs(state.membranePotential - state.restingPotential) / 20.0f));
            }
            
            totalActivity += activity;
            ++neuronCount;
        }
    }
    
    return (neuronCount > 0) ? (totalActivity / neuronCount) : 0.0f;
}

} // namespace nlm
