#include "Maturation.hpp"

namespace nlm {

struct Maturation::Impl {
    // Maturation state
    float progress;                    // 0.0 = immature, 1.0 = mature
    float maturationRate;              // Base rate of maturation (activity-dependent)
    float consolidationRate;           // Rate of Hebbian consolidation
    float hebbianLearningRate;         // Strength of Hebbian learning during maturation
    
    // Mature target properties
    MembranePotential matureThreshold;    // Target threshold potential
    MembranePotential matureRestingPotential; // Target resting potential
    float matureTimeConstant;           // Target membrane time constant
    
    // Activity history for Hebbian consolidation
    std::unordered_map<NeuronId, float> activityHistory;
    
    // Synapses with high correlated activity (Hebbian consolidation candidates)
    std::vector<Synapse*> strongCorrelatedSynapses;
    
    // Critical period regulation
    float criticalPeriodEnd;           // Simulation step when critical period ends
    float plasticityReduction;          // Rate of plasticity reduction with age
    
    // Statistics
    size_t neuronsMatured;
    size_t synapsesConsolidated;
    size_t hebbianEvents;
    
    Impl(std::shared_ptr<Config> config) 
        : progress(0.0f)
        , maturationRate(config ? config->getOr<float>("maturation_rate", 0.001f) : 0.001f)
        , consolidationRate(config ? config->getOr<float>("consolidation_rate", 0.0001f) : 0.0001f)
        , hebbianLearningRate(config ? config->getOr<float>("hebbian_learning_rate", 0.01f) : 0.01f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , criticalPeriodEnd(300.0f)  // Critical period ends at step 300
        , plasticityReduction(config ? config->getOr<float>("plasticity_reduction", 0.01f) : 0.01f)
        , neuronsMatured(0)
        , synapsesConsolidated(0)
        , hebbianEvents(0) {}
};

Maturation::Maturation(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Maturation::~Maturation() = default;

void Maturation::update(Brain* brain, SimulationStep currentStep, RandomGenerator& rng) {
    if (!brain) return;
    
    // Update maturation progress based on activity and developmental stage
    float activitySum = 0.0f;
    size_t activeNeuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                float activity = std::abs(state.membranePotential - state.restingPotential);
                
                if (activity > 0.1f) {
                    activitySum += activity;
                    activeNeuronCount++;
                    
                    // Record activity for Hebbian consolidation
                    pImpl->activityHistory[neuron->getId()] = activity;
                }
            }
        }
    }
    
    float avgActivity = activeNeuronCount > 0 ? activitySum / activeNeuronCount : 0.0f;
    
    // Calculate maturation rate based on activity and developmental stage
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    float age = brain->getDevelopmentSystem()->getDevelopmentalAge();
    float maturationRate = calculateMaturationRate(avgActivity, stage, age);
    
    // Advance maturation progress
    progress_ += maturationRate;
    if (progress_ > 1.0f) progress_ = 1.0f;
    
    // Apply Hebbian consolidation for strongly correlated synapses
    for (auto& region : brain->getRegions()) {
        for (Synapse* synapse : region->getSynapses()) {
            // Check if synapse has high correlated activity (Hebbian consolidation)
            if (std::abs(synapse->getWeight()) > 0.1f) {
                float eligibility = std::abs(synapse->getEligibilityTrace());
                if (eligibility > 0.5f) {
                    // Hebbian consolidation: strengthen synapses with correlated activity
                    if (rng.bernoulli(pImpl->consolidationRate * progress_)) {
                        float currentWeight = synapse->getWeight();
                        synapse->addToWeight(pImpl->hebbianLearningRate * progress_);
                        ++pImpl->synapsesConsolidated;
                        ++pImpl->hebbianEvents;
                    }
                }
            }
        }
    }
    
    // Mature neurons based on progress
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                matureNeuron(neuron, progress_, *brain->getDevelopmentSystem());
            }
        }
    }
    
    // Log statistics periodically
    if (currentStep % 1000 == 0) {
        NLM_LOG_INFO(std::string("Maturation update: Progress=") + std::to_string(progress_) +
                     std::string(", Activity=") + std::to_string(avgActivity) +
                     std::string(", Mature neurons=") + std::to_string(pImpl->neuronsMatured) +
                     std::string(", Consolidated synapses=") + std::to_string(pImpl->synapsesConsolidated));
    }
}

void Maturation::matureNeuron(Neuron* neuron, float progress, const DevelopmentSystem& devSys) {
    if (!neuron) return;
    
    auto& state = neuron->getState();
    
    // Check if neuron has already reached maturity
    if (progress >= 1.0f) {
        ++pImpl->neuronsMatured;
        return;
    }
    
    // Gradually adjust properties toward mature values
    float maturationFactor = progress * devSys.getPlasticityModifier();
    
    // Adjust threshold potential
    float currentThreshold = state.threshold;
    float targetThreshold = matureThreshold_;
    float thresholdAdjustment = (targetThreshold - currentThreshold) * maturationFactor * 0.01f;
    neuron->setThreshold(currentThreshold + thresholdAdjustment);
    
    // Adjust resting potential
    float currentResting = state.restingPotential;
    float targetResting = matureRestingPotential_;
    float restingAdjustment = (targetResting - currentResting) * maturationFactor * 0.01f;
    neuron->setRestingPotential(currentResting + restingAdjustment);
    
    // Adjust time constant
    float currentTimeConstant = state.leakConductance;
    float targetTimeConstant = matureTimeConstant_;
    float timeConstantAdjustment = (targetTimeConstant - currentTimeConstant) * maturationFactor * 0.01f;
    neuron->setLeakConductance(currentTimeConstant + timeConstantAdjustment);
    
    // Apply age-dependent plasticity reduction
    float age = devSys.getDevelopmentalAge();
    if (age > pImpl->criticalPeriodEnd) {
        // After critical period, reduce plasticity
        float reductionFactor = 1.0f - std::min(1.0f, (age - pImpl->criticalPeriodEnd) * pImpl->plasticityReduction);
        neuron->setRefractoryPeriod(state.refractoryPeriod * reductionFactor);
    }
}

float Maturation::calculateMaturationRate(float activity, DevelopmentalStage stage, float age) const {
    // Activity-dependent maturation: more active neurons mature faster
    float activityFactor = 1.0f + std::min(10.0f, activity * 10.0f);
    
    // Stage-dependent modulation
    float stageFactor = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            stageFactor = 1.5f;  // Initial stage: accelerated maturation
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageFactor = 1.2f;  // Critical period: still high but slowing down
            break;
        case DevelopmentalStage::Maturation:
            stageFactor = 1.0f;  // Maturation: steady rate
            break;
        case DevelopmentalStage::Adult:
            stageFactor = 0.7f;  // Adult: slower maturation
            break;
        case DevelopmentalStage::Aging:
            stageFactor = 0.5f;  // Aging: reduced capacity
            break;
        default:
            stageFactor = 1.0f;
    }
    
    // Age-dependent reduction
    float ageFactor = 1.0f;
    if (age > pImpl->criticalPeriodEnd) {
        float ageExcess = (age - pImpl->criticalPeriodEnd) / 1000.0f;
        ageFactor = std::max(0.1f, 1.0f - ageExcess);
    }
    
    return pImpl->maturationRate * activityFactor * stageFactor * ageFactor;
}

} // namespace nlm
