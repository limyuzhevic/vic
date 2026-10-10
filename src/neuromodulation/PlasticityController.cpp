#include "PlasticityController.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace nlm {

struct PlasticityController::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<NeuromodulationEngine> engine;
    PlasticityControlParams params;
    PlasticityState state;
    
    // Plasticity system states
    std::unordered_map<SynapseId, SynapsePlasticityState> synapseStates;
    std::unordered_map<SynapseId, HebbianPlasticityState> hebbianStates;
    STDPState stdpState;
    std::unordered_map<SynapseId, StructuralPlasticityState> structuralStates;
    
    // History tracking
    PlasticityHistory history;
    
    // Integration state
    std::vector<const Neuromodulator*> activeNeuromodulators;
    std::unordered_map<SynapseId, float> neuromodulatorInfluences;
    
    // Safety and stability
    float minWeight;
    float maxWeight;
    float maxTotalChange;
    
    Impl() : brain(nullptr), engine(nullptr),
             minWeight(0.0f), maxWeight(10.0f), maxTotalChange(0.1f) {
        
        // Initialize STDP traces
        for (int i = 0; i < 100; ++i) {
            stdpState.tracePre[i] = 0.0f;
            stdpState.tracePost[i] = 0.0f;
        }
    }
};

PlasticityController::PlasticityController() : pImpl(new Impl) {
    NLM_LOG_INFO("PlasticityController created");
}

PlasticityController::~PlasticityController() {
    NLM_LOG_INFO("PlasticityController destroyed");
}

PlasticityController::PlasticityController(PlasticityController&& other) noexcept
    : pImpl(std::exchange(other.pImpl, nullptr)) {
    if (pImpl) {
        pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
        pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
    }
}

PlasticityController& PlasticityController::operator=(PlasticityController&& other) noexcept {
    if (this != &other) {
        shutdown();
        pImpl = std::exchange(other.pImpl, nullptr);
        if (pImpl) {
            pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
            pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
        }
    }
    return *this;
}

bool PlasticityController::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine) {
    try {
        pImpl->brain = brain;
        pImpl->engine = engine;
        
        if (!pImpl->brain || !pImpl->engine) {
            NLM_LOG_ERROR("PlasticityController initialization failed: invalid dependencies");
            return false;
        }
        
        NLM_LOG_INFO("PlasticityController initialization completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("PlasticityController initialization failed: " + std::string(e.what()));
        return false;
    }
}

void PlasticityController::update(TimestepDuration dt, SimulationStep step) {
    // Update state
    updateState(dt);
    
    // Update STDP traces
    if (enableSTDP_) {
        for (size_t i = 0; i < stdpState.tracePre.size(); ++i) {
            // Decay traces
            stdpState.tracePre[i] *= 0.99f;
            stdpState.tracePost[i] *= 0.99f;
            
            // Keep within bounds
            stdpState.tracePre[i] = std::max(0.0f, stdpState.tracePre[i]);
            stdpState.tracePost[i] = std::max(0.0f, stdpState.tracePost[i]);
        }
    }
    
    // Apply homeostatic regulation
    if (enableTDPlasticity_) {
        applyHomeostaticPlasticity(dt);
    }
}

void PlasticityController::computeHebbianPlasticity(const class NeuralConnection& connection,
                                                 float preActivation, float postActivation) {
    if (!enableHebbianPlasticity_) return;
    
    SynapseId synapseId = connection.getId();
    
    // Get or create hebbian state
    auto& hebbianState = pImpl->hebbianStates[synapseId];
    
    // Compute Hebbian update
    float activationProduct = preActivation * postActivation;
    
    // Apply neuromodulator modulation
    float modulationFactor = 1.0f;
    auto it = pImpl->neuromodulatorInfluences.find(synapseId);
    if (it != pImpl->neuromodulatorInfluences.end()) {
        modulationFactor = it->second;
    }
    
    // Apply novelty bonus
    float noveltyBonus = 1.0f;
    if (enableTDPlasticity_) {
        // Could be enhanced with novelty from novelity system
        noveltyBonus = 1.2f;
    }
    
    // Apply prediction error bonus
    float predictionErrorBonus = 1.0f;
    if (enableTDPlasticity_) {
        // Could be enhanced with prediction error
        predictionErrorBonus = 1.1f;
    }
    
    // Compute Hebbian change
    float hebbianChange = pImpl->state.hebbianLearningRate * 
                         activationProduct * 
                         modulationFactor * 
                         noveltyBonus * 
                         predictionErrorBonus;
    
    // Update synaptic state
    auto& synapseState = pImpl->synapseStates[synapseId];
    synapseState.currentWeight += hebbianChange;
    synapseState.eligibilityTrace += hebbianChange * 0.1f;
    synapseState.learningHistory += std::abs(hebbianChange);
    
    // Apply bounds
    enforceWeightBounds(synapseId);
    
    // Store in history
    pImpl->history.hebbianHistory.push_back(hebbianChange);
    if (pImpl->history.hebbianHistory.size() > 1000) {
        pImpl->history.hebbianHistory.erase(pImpl->history.hebbianHistory.begin());
    }
    
    NLM_LOG_DEBUG("PlasticityController: Hebbian update - change: " + 
                 std::to_string(hebbianChange) + ", weight: " +
                 std::to_string(synapseState.currentWeight));
}

void PlasticityController::computeSTDPPlasticity(const class NeuralConnection& connection,
                                                float preSpikeTime, float postSpikeTime) {
    if (!enableSTDP_) return;
    
    SynapseId synapseId = connection.getId();
    float timeDifference = postSpikeTime - preSpikeTime;
    
    // Determine STDP rule based on time difference
    float weightChange = 0.0f;
    
    if (timeDifference > 0) {
        // Post before pre (LTP)
        float trace = stdpState.tracePost[std::min(static_cast<size_t>(timeDifference * 100), stdpState.tracePost.size() - 1)];
        weightChange = trace * pImpl->state.stdpLearningRate;
    } else if (timeDifference < 0) {
        // Pre before post (LTD)
        float trace = stdpState.tracePre[std::min(static_cast<size_t>(-timeDifference * 100), stdpState.tracePre.size() - 1)];
        weightChange = -trace * pImpl->state.stdpLearningRate;
    }
    
    // Apply neuromodulator modulation
    float modulation = pImpl->state.neuromodulatorModulation;
    if (pImpl->enableTDPlasticity_) {
        // Enhance STDP with TD learning
        modulation *= (1.0f + std::abs(timeDifference) * 0.1f);
    }
    
    weightChange *= modulation;
    
    // Update synaptic state
    auto& synapseState = pImpl->synapseStates[synapseId];
    synapseState.currentWeight += weightChange;
    synapseState.stdpEligibility += weightChange * 0.05f;
    
    // Update STDP traces
    size_t timeIndex = std::min(static_cast<size_t>(std::abs(timeDifference) * 100), 
                               stdpState.tracePre.size() - 1);
    
    if (timeDifference > 0) {
        stdpState.tracePost[timeIndex] += 1.0f;
        stdpState.deltaWeights[timeIndex] += weightChange;
    } else if (timeDifference < 0) {
        stdpState.tracePre[timeIndex] += 1.0f;
        stdpState.deltaWeights[timeIndex] += weightChange;
    }
    
    // Apply bounds
    enforceWeightBounds(synapseId);
    
    // Store in history
    pImpl->history.stdpHistory.push_back(weightChange);
    if (pImpl->history.stdpHistory.size() > 1000) {
        pImpl->history.stdpHistory.erase(pImpl->history.stdpHistory.begin());
    }
    
    pImpl->state.totalPlasticityChange += std::abs(weightChange);
}

void PlasticityController::computeTDPlasticity(const class NeuralConnection& connection,
                                             float tdError, float reward) {
    if (!enableTDPlasticity_) return;
    
    SynapseId synapseId = connection.getId();
    
    // Compute TD update
    float tdChange = pImpl->state.tdLearningRate * tdError;
    
    // Apply neuromodulator modulation
    float modulationFactor = 1.0f;
    auto it = pImpl->neuromodulatorInfluences.find(synapseId);
    if (it != pImpl->neuromodulatorInfluences.end()) {
        modulationFactor = it->second;
    }
    
    tdChange *= modulationFactor;
    
    // Update synaptic state
    auto& synapseState = pImpl->synapseStates[synapseId];
    synapseState.currentWeight += tdChange;
    synapseState.burstEligibility += tdChange * 0.2f;
    
    // Update total change
    pImpl->state.totalPlasticityChange += std::abs(tdChange);
    
    // Store in history
    pImpl->history.tdHistory.push_back(tdChange);
    if (pImpl->history.tdHistory.size() > 1000) {
        pImpl->history.tdHistory.erase(pImpl->history.tdHistory.begin());
    }
    
    // Apply homeostatic pressure
    synapseState.homeostaticPressure = std::min(2.0f, 
                                              synapseState.homeostaticPressure + 
                                              std::abs(tdChange) * 0.1f);
    
    NLM_LOG_DEBUG("PlasticityController: TD update - error: " + std::to_string(tdError) +
                 ", change: " + std::to_string(tdChange));
}

void PlasticityController::computeNeuromodulatorGatedPlasticity(const class NeuralConnection& connection,
                                                              const Neuromodulator& modulator) {
    if (!pImpl->params.enableGatedPlasticity) return;
    
    SynapseId synapseId = connection.getId();
    
    // Get neuromodulator level
    float modulatorLevel = modulator.getLevel();
    
    // Check gating threshold
    if (modulatorLevel < pImpl->params.gatedPlasticityThreshold) {
        return;  // Plasticity is gated off
    }
    
    // Compute gated plasticity effect
    float gatingFactor = modulatorLevel / pImpl->params.gatedPlasticityThreshold;
    
    // Scale existing plasticity changes
    auto& synapseState = pImpl->synapseStates[synapseId];
    synapseState.currentWeight *= gatingFactor;
    synapseState.eligibilityTrace *= gatingFactor;
    
    NLM_LOG_DEBUG("PlasticityController: Neuromodulator-gated update - level: " +
                 std::to_string(modulatorLevel) + ", factor: " + std::to_string(gatingFactor));
}

void PlasticityController::computeNeuromodulatorScaledPlasticity(const class NeuralConnection& connection,
                                                                const Neuromodulator& modulator) {
    if (!pImpl->params.enableScaledPlasticity) return;
    
    SynapseId synapseId = connection.getId();
    
    // Get neuromodulator level
    float modulatorLevel = modulator.getLevel();
    
    // Compute scaling factor
    float scalingFactor = 1.0f + modulatorLevel * (pImpl->params.scaledPlasticityFactor - 1.0f);
    
    // Apply saturation
    scalingFactor = std::min(pImpl->params.plasticitySaturation, scalingFactor);
    
    // Scale synaptic weight
    auto& synapseState = pImpl->synapseStates[synapseId];
    synapseState.currentWeight *= scalingFactor;
    synapseState.eligibilityTrace *= scalingFactor;
    
    NLM_LOG_DEBUG("PlasticityController: Neuromodulator-scaled update - level: " +
                 std::to_string(modulatorLevel) + ", factor: " + std::to_string(scalingFactor));
}

void PlasticityController::computeNeuromodulatorTDPlasticity(const class NeuralConnection& connection,
                                                            const Neuromodulator& modulator,
                                                            float spikeTimeDifference) {
    if (!pImpl->params.enableTDPlasticity) return;
    
    SynapseId synapseId = connection.getId();
    
    // Get neuromodulator level
    float modulatorLevel = modulator.getLevel();
    
    // Compute time-dependent factor
    float tdFactor = 1.0f / (1.0f + std::exp(-spikeTimeDifference / pImpl->params.tdPlasticityWindow));
    
    // Apply neuromodulator modulation
    float modulation = modulatorLevel * pImpl->params.tdPlasticityWeight;
    
    // Compute TD plasticity update
    float tdUpdate = pImpl->state.tdLearningRate * tdFactor * modulation;
    
    // Update synaptic state
    auto& synapseState = pImpl->synapseStates[synapseId];
    synapseState.currentWeight += tdUpdate;
    synapseState.temporalEligibility += tdUpdate * 0.1f;
    
    NLM_LOG_DEBUG("PlasticityController: Neuromodulator-TD update - td: " +
                 std::to_string(tdUpdate) + ", spike: " + std::to_string(spikeTimeDifference));
}

void PlasticityController::computeNeuromodulatorTypeSpecificPlasticity(const class NeuralConnection& connection,
                                                                      const Neuromodulator& modulator) {
    if (!pImpl->params.enableTypeSpecificPlasticity) return;
    
    SynapseId synapseId = connection.getId();
    
    // Get neuromodulator name (placeholder - would need actual implementation)
    std::string modulatorName = modulator.getName();  // Assuming getName() method exists
    
    // Find appropriate weight for this neuromodulator type
    float typeWeight = 1.0f;
    if (!pImpl->params.typeSpecificWeights.empty()) {
        // Simple hash to find weight (would need proper mapping)
        typeWeight = pImpl->params.typeSpecificWeights[std::hash<std::string>{}(modulatorName) % 
                                                   pImpl->params.typeSpecificWeights.size()];
    }
    
    // Apply type-specific plasticity
    auto& synapseState = pImpl->synapseStates[synapseId];
    synapseState.currentWeight *= typeWeight;
    synapseState.eligibilityTrace *= typeWeight;
    
    NLM_LOG_DEBUG("PlasticityController: Neuromodulator-type specific update - name: " +
                 modulatorName + ", weight: " + std::to_string(typeWeight));
}

void PlasticityController::updateState(TimestepDuration dt) {
    // Update total plasticity change with decay
    pImpl->state.totalPlasticityChange *= (1.0f - 0.01f * static_cast<float>(dt));
    
    // Update neuromodulator modulation
    pImpl->state.neuromodulatorModulation = 
        1.0f + pImpl->params.neuromodulatorModulation * 0.1f * static_cast<float>(dt);
    
    // Update homeostatic pressure
    pImpl->state.homeostaticPressure = 
        std::max(1.0f, pImpl->state.homeostaticPressure - 0.01f * static_cast<float>(dt));
}

void PlasticityController::reset() {
    NLM_LOG_INFO("PlasticityController reset");
    
    // Reset all synapse states
    for (auto& [synapseId, state] : pImpl->synapseStates) {
        state = SynapsePlasticityState();
    }
    
    for (auto& [synapseId, state] : pImpl->hebbianStates) {
        state = HebbianPlasticityState();
    }
    
    // Reset STDP state
    std::fill(stdpState.tracePre.begin(), stdpState.tracePre.end(), 0.0f);
    std::fill(stdpState.tracePost.begin(), stdpState.tracePost.end(), 0.0f);
    std::fill(stdpState.deltaWeights.begin(), stdpState.deltaWeights.end(), 0.0f);
    
    for (auto& [synapseId, state] : pImpl->structuralStates) {
        state = StructuralPlasticityState();
    }
    
    // Reset history
    clearHistory();
    
    // Reset integration state
    pImpl->activeNeuromodulators.clear();
    pImpl->neuromodulatorInfluences.clear();
    
    // Reset state
    pImpl->state = PlasticityState();
}

void PlasticityController::setPlasticityParams(const PlasticityControlParams& params) {
    pImpl->params = params;
}

void PlasticityController::integrateWithNeuromodulators(const std::vector<const Neuromodulator*>& modulators) {
    pImpl->activeNeuromodulators = modulators;
    
    // Compute neuromodulator influences for each synapse
    for (const auto& modulator : modulators) {
        // Get neuromodulator level and name
        float level = modulator->getLevel();
        std::string name = modulator->getName();  // Assuming getName() method
        
        // Update influences (simplified - would be more complex in real system)
        for (auto& [synapseId, influence] : pImpl->neuromodulatorInfluences) {
            // Modulate influence based on neuromodulator properties
            influence *= (1.0f + level * 0.1f);
            
            // Apply bounds
            influence = std::clamp(influence, 0.5f, 2.0f);
        }
    }
}

void PlasticityController::integrateWithLearningSystems() {
    // Integrate with learning systems
    // This would involve reading from other systems and updating plasticity
    
    if (pImpl->engine) {
        // Could integrate with dopamine system
        if (auto dopamineSystem = pImpl->engine->getDopamineSystem()) {
            // Apply dopamine-based plasticity
        }
    }
}

void PlasticityController::integrateWithHomeostaticSystems() {
    // Integrate with homeostatic systems
    // This would involve maintaining synaptic stability
    
    // Apply homeostatic plasticity
    applyHomeostaticPlasticity(1.0f);
}

void PlasticityController::applyPlasticityToSynapse(SynapseId synapseId, float weightChange) {
    auto it = pImpl->synapseStates.find(synapseId);
    if (it != pImpl->synapseStates.end()) {
        it->second.currentWeight += weightChange;
        enforceWeightBounds(synapseId);
    }
}

void PlasticityController::updateConnectionWeight(SynapseId synapseId, float newWeight) {
    auto it = pImpl->synapseStates.find(synapseId);
    if (it != pImpl->synapseStates.end()) {
        it->second.currentWeight = newWeight;
        enforceWeightBounds(synapseId);
    }
}

void PlasticityController::updateStructuralProperties(SynapseId synapseId, float structuralChange) {
    auto it = pImpl->structuralStates.find(synapseId);
    if (it != pImpl->structuralStates.end()) {
        it->second.spineDensity += structuralChange;
        it->second.spineDensity = std::clamp(it->second.spineDensity, 100.0f, 10000.0f);
    }
}

void PlasticityController::applyHomeostaticPlasticity(TimestepDuration dt) {
    // Apply homeostatic plasticity to maintain stability
    for (auto& [synapseId, state] : pImpl->synapseStates) {
        // Simple homeostatic regulation: bring weight towards target
        float targetWeight = 0.5f;  // Ideal weight
        float deviation = state.currentWeight - targetWeight;
        
        float homeostaticChange = -pImpl->params.homeostaticLearningRate * 
                                  deviation * state.homeostaticPressure;
        
        state.currentWeight += homeostaticChange;
        enforceWeightBounds(synapseId);
        
        // Reduce homeostatic pressure
        state.homeostaticPressure *= (1.0f - 0.01f * static_cast<float>(dt));
    }
}

void PlasticityController::enforceWeightBounds(SynapseId synapseId) {
    auto it = pImpl->synapseStates.find(synapseId);
    if (it != pImpl->synapseStates.end()) {
        it->second.currentWeight = 
            std::clamp(it->second.currentWeight, pImpl->minWeight, pImpl->maxWeight);
    }
}

const std::vector<float>& PlasticityController::getHebbianHistory() const {
    return pImpl->history.hebbianHistory;
}

const std::vector<float>& PlasticityController::getSTDPHistory() const {
    return pImpl->history.stdpHistory;
}

const std::vector<float>& PlasticityController::getTDHistory() const {
    return pImpl->history.tdHistory;
}

void PlasticityController::clearHistory() {
    pImpl->history.hebbianHistory.clear();
    pImpl->history.stdpHistory.clear();
    pImpl->history.tdHistory.clear();
    pImpl->history.structuralHistory.clear();
}

bool PlasticityController::validatePlasticityConstraints() const {
    // Check weight bounds
    for (const auto& [synapseId, state] : pImpl->synapseStates) {
        if (state.currentWeight < pImpl->minWeight || state.currentWeight > pImpl->maxWeight) {
            NLM_LOG_ERROR("PlasticityController: Synapse weight out of bounds");
            return false;
        }
        
        // Check for numerical instabilities
        if (std::isnan(state.currentWeight) || std::isinf(state.currentWeight)) {
            NLM_LOG_ERROR("PlasticityController: Numerical instability in synapse state");
            return false;
        }
    }
    
    // Check total plasticity change
    if (pImpl->state.totalPlasticityChange > pImpl->maxTotalChange) {
        NLM_LOG_ERROR("PlasticityController: Total plasticity change exceeds limit");
        return false;
    }
    
    return true;
}

void PlasticityController::enforceStabilityConstraints() {
    // Enforce stability constraints on all synapses
    for (auto& [synapseId, state] : pImpl->synapseStates) {
        // Apply bounded regularization
        float regularization = std::max(0.0f, std::abs(state.currentWeight - 0.5f) * 0.1f);
        state.currentWeight = std::clamp(state.currentWeight, 0.1f, 0.9f);
        
        // Reduce eligibility traces
        state.eligibilityTrace *= (1.0f - regularization);
    }
}

bool PlasticityController::checkNumericalStability() const {
    for (const auto& [synapseId, state] : pImpl->synapseStates) {
        if (std::isnan(state.currentWeight) || std::isinf(state.currentWeight) ||
            std::isnan(state.eligibilityTrace) || std::isinf(state.eligibilityTrace)) {
            return false;
        }
    }
    
    return true;
}

void PlasticityController::shutdown() {
    if (pImpl) {
        reset();
        pImpl->brain.reset();
        pImpl->engine.reset();
    }
}

} // namespace nlm