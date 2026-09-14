#include "DevelopmentSystem.hpp"
#include "../../core/Random/Random.hpp"
#include "../../core/Logger/Logger.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../plasticity/StructuralPlasticity.hpp"
#include <numeric>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double neuronMaturity;
    double connectivityDensity;
    double functionalSpecialization;
    double activityLevel;
    double learningRate;
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
    
    // Biological state tracking
    std::vector<double> neuronMaturityDistribution;
    std::vector<float> synapticStrengthDistribution;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0),
             neuronMaturity(0.0), connectivityDensity(0.0), functionalSpecialization(0.0), activityLevel(0.0),
             learningRate(1.0), totalSynapsesCreated(0), totalSynapsesPruned(0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl) {}

DevelopmentSystem::~DevelopmentSystem() = default;

// Forward declare this helper function for the static member initializer
const char* DevelopmentSystem_getStageNameFromStage(DevelopmentalStage stage);

const std::unordered_map<DevelopmentalStage, StageCharacteristics> DevelopmentSystem::stageMap = {
    {DevelopmentalStage::Initial, {"Initial", 60.0, 0.5f, 0.1f, 0.0, 0.0, false, 0.0}},
    {DevelopmentalStage::CriticalPeriod, {"CriticalPeriod", 300.0, 0.3f, 0.2f, 300.0, 0.0, true, 0.3}},
    {DevelopmentalStage::Maturation, {"Maturation", 600.0, 0.1f, 0.3f, 0.0, 50.0, true, 0.5}},
    {DevelopmentalStage::Adult, {"Adult", 0.0, 0.01f, 0.2f, 0.0, 100.0, true, 0.7}},
    {DevelopmentalStage::Aging, {"Aging", 0.0, 0.005f, 0.1f, 0.0, 150.0, true, 0.5}}
};

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stepsInCurrentStage = 0;
}

void DevelopmentSystem::advanceStage() {
    // Store old stage for transition effects
    DevelopmentalStage oldStage = pImpl->stage;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            pImpl->neuronMaturity = 0.1;  // Initial neuronal maturity
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            pImpl->neuronMaturity = 0.3;  // Increased maturity
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            pImpl->neuronMaturity = 0.7;  // High maturity
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            pImpl->neuronMaturity = 0.9;  // Near mature
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            break;
    }
    
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
}

const char* DevelopmentSystem::getStageName() const {
    return DevelopmentSystem_getStageNameFromStage(pImpl->stage);
}

const char* DevelopmentSystem_getStageNameFromStage(DevelopmentalStage stage) {
    switch (stage) {
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
    
    // Update biological metrics based on current brain state
    if (brain) {
        updateMetrics(brain);
    }
    
    // Auto-advance stage based on developmental goals and metrics
    if (canAdvanceStage()) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Update biological metrics based on current brain state
    if (brain) {
        updateMetrics(brain);
    }
    
    // Auto-advance stage based on developmental goals and metrics
    if (canAdvanceStage()) {
        advanceStage();
    }
}

DevelopmentMetrics DevelopmentSystem::getMetrics() const {
    DevelopmentMetrics metrics;
    metrics.neuronMaturity = pImpl->neuronMaturity;
    metrics.connectivityDensity = pImpl->connectivityDensity;
    metrics.functionalSpecialization = pImpl->functionalSpecialization;
    metrics.activityLevel = pImpl->activityLevel;
    metrics.learningRate = pImpl->learningRate;
    metrics.synapseCount = pImpl->totalSynapsesCreated - pImpl->totalSynapsesPruned;
    metrics.neuronCount = static_cast<size_t>(pImpl->neuronMaturity * 100.0); // Estimate from maturity
    return metrics;
}

void DevelopmentSystem::updateMetrics(Brain* brain) {
    if (!brain) return;
    
    // Calculate biological metrics from current brain state
    pImpl->activityLevel = calculateActivityScore(brain);
    pImpl->connectivityDensity = calculateConnectivityDensity(brain);
    pImpl->functionalSpecialization = calculateFunctionalSpecialization(brain);
    
    // Update neuron maturity distribution
    updateNeuronMaturity(brain);
    
    // Update synaptic strength distribution
    updateSynapticStrength(brain);
}

double DevelopmentSystem::calculateActivityScore(Brain* brain) const {
    if (!brain) return 0.0;
    
    double totalActivity = 0.0;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            const auto& spikes = population->getSpikeHistory();
            if (!spikes.empty()) {
                // Calculate average firing rate
                double activity = static_cast<double>(spikes.size());
                totalActivity += activity;
                neuronCount++;
            }
        }
    }
    
    return neuronCount > 0 ? totalActivity / neuronCount : 0.0;
}

double DevelopmentSystem::calculateConnectivityDensity(Brain* brain) const {
    if (!brain) return 0.0;
    
    size_t totalSynapses = 0;
    size_t maxPossibleSynapses = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            size_t neurons = population->getNeuronCount();
            maxPossibleSynapses += neurons * neurons; // Complete graph assumption
        }
        totalSynapses += region->getSynapseCount();
    }
    
    return maxPossibleSynapses > 0 ? static_cast<double>(totalSynapses) / maxPossibleSynapses : 0.0;
}

double DevelopmentSystem::calculateFunctionalSpecialization(Brain* brain) const {
    if (!brain) return 0.0;
    
    // Measure functional specialization by analyzing connectivity patterns
    // This is a simplified metric: higher regional specificity = higher specialization
    std::unordered_map<std::string, size_t> regionConnections;
    
    for (const auto& region : brain->getRegions()) {
        regionConnections[region->getRegionName()] = region->getSynapseCount();
    }
    
    if (regionConnections.empty()) return 0.0;
    
    // Calculate variance in connectivity across regions
    double meanConnections = 0.0;
    for (const auto& pair : regionConnections) {
        meanConnections += pair.second;
    }
    meanConnections /= regionConnections.size();
    
    double variance = 0.0;
    for (const auto& pair : regionConnections) {
        double diff = pair.second - meanConnections;
        variance += diff * diff;
    }
    variance /= regionConnections.size();
    
    return std::sqrt(variance) / (meanConnections + 0.001); // Normalized measure
}

void DevelopmentSystem::updateNeuronMaturity(Brain* brain) {
    if (!brain) return;
    
    pImpl->neuronMaturity = 0.0;
    size_t matureCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            const auto& neurons = population->getNeurons();
            for (const auto& neuron : neurons) {
                double maturity = neuron->getMaturity();
                pImpl->neuronMaturity += maturity;
                matureCount++;
            }
        }
    }
    
    pImpl->neuronMaturity /= (matureCount + 0.001);
}

void DevelopmentSystem::updateSynapticStrength(Brain* brain) {
    if (!brain) return;
    
    pImpl->totalSynapsesCreated = 0;
    pImpl->totalSynapsesPruned = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& synapse : region->getSynapses()) {
            float weight = std::abs(synapse->getWeight());
            pImpl->synapticStrengthDistribution.push_back(weight);
        }
    }
}

bool DevelopmentSystem::canAdvanceStage() const {
    const auto& stageInfo = stageMap.at(pImpl->stage);
    
    // Check time-based advancement
    if (pImpl->stageAge >= stageInfo.duration && stageInfo.duration > 0.0) {
        // Can advance based on time
    }
    
    // Check activity-based advancement
    if (stageInfo.requiresActivityTrigger) {
        if (pImpl->activityLevel >= stageInfo.activityThreshold) {
            // Can advance based on activity
        }
    }
    
    // Check metric-based advancement
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        if (pImpl->neuronMaturity >= stageInfo.maturationThreshold) {
            return true;
        }
    }
    
    if (pImpl->stage == DevelopmentalStage::Maturation) {
        if (pImpl->connectivityDensity >= 0.8 && pImpl->functionalSpecialization >= 0.7) {
            return true;
        }
    }
    
    if (pImpl->stage == DevelopmentalStage::Adult) {
        if (pImpl->neuronMaturity >= 0.95 && pImpl->activityLevel >= 0.6) {
            return true;
        }
    }
    
    return false;
}

bool DevelopmentSystem::isInDevelopmentWindow(DevelopmentalStage stage) const {
    // For now, all stages are always "in window" - can be enhanced later
    return true;
}

double DevelopmentSystem::getWindowProgress(DevelopmentalStage stage) const {
    if (stage == DevelopmentalStage::CriticalPeriod) {
        return std::min(1.0, pImpl->stageAge / 300.0);
    } else if (stage == DevelopmentalStage::Maturation) {
        return std::min(1.0, pImpl->stageAge / 600.0);
    } else if (stage == DevelopmentalStage::Adult) {
        return pImpl->neuronMaturity;
    } else if (stage == DevelopmentalStage::Aging) {
        return std::min(1.0, pImpl->stageAge / 1000.0);
    }
    return 0.0;
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f;
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

} // namespace nlm