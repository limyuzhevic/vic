#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include <unordered_set>

namespace nlm {

struct AttentionalSelection::Impl {
    // Neural population dynamics for attention
    std::unordered_map<uint64_t, AttentionalPopulation> attentionalPopulations;
    std::unordered_map<uint64_t, float> attentionalGains;
    std::unordered_map<uint64_t, float> attentionalInhibitions;
    
    // Competition tracking
    std::vector<MemoryTrace> competingMemoryTraces;
    std::vector<PredictionErrorFocus> activePredictionErrors;
    
    // Goal tracking
    std::vector<float> currentGoalVector;
    std::vector<uint64_t> attendedNeurons;
    std::vector<uint64> attendedRegions;
    
    // State management
    AttentionState currentState;
    AttentionState previousState;
    SimulationStep lastAttentionSwitch;
    
    // Temporal dynamics
    std::deque<float> attentionTimeSeries;
    float attentionVariability;
    
    // Salience and bias accumulation
    std::unordered_map<uint64_t, float> accumulatedSalience;
    std::unordered_map<uint64_t, float> accumulatedBias;
    
    // Processing buffers
    std::vector<float> sensoryInputBuffer;
    std::vector<float> attentionalOutputBuffer;
    std::vector<float> predictionErrorBuffer;
    
    Impl() : currentState(AttentionState::IDLE), previousState(AttentionState::IDLE),
             lastAttentionSwitch(0), attentionVariability(0.0f) {
        attentionTimeSeries.reserve(100);
    }
};

AttentionalSelection::AttentionalSelection() : brain_(nullptr),
    acetylcholine_(nullptr), norepinephrine_(nullptr), dopamine_(nullptr),
    serotonin_(nullptr), curiosity_(nullptr), currentAttentionState_(AttentionState::IDLE),
    goalAttentionStrength_(0.0f), attentionalBreadth_(0.5f), attentionalDepth_(0.5f),
    attentionalFlexibility_(0.5f), attentionalScope_(AttentionState::IDLE) {
    pImpl = std::make_unique<Impl>();
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain,
                                    Acetylcholine* ach,
                                    Norepinephrine* ne,
                                    Dopamine* da,
                                    Serotonin* sero,
                                    Curiosity* curiosity) {
    brain_ = brain;
    acetylcholine_ = ach;
    norepinephrine_ = ne;
    dopamine_ = da;
    serotonin_ = sero;
    curiosity_ = curiosity;
    
    // Initialize attentional dynamics
    dynamics_.focusPersistence = 100.0f;
    dynamics_.refreshRate = 0.95f;
    dynamics_.captureThreshold = 0.7f;
    dynamics_.disengagementThreshold = 0.3f;
    dynamics_.switchingCost = 0.5f;
    dynamics_.costOfError = 1.0f;
    
    // Reset attentional state
    reset();
    
    NLM_LOG_INFO("Comprehensive AttentionalSelection system initialized");
}

void AttentionalSelection::update(SimulationStep currentStep, Timestamp currentTime) {
    // Integrate neuromodulator signals
    integrateNeuromodulatorSignals();
    
    // Update prediction error based attention
    updatePredictionErrorBasedAttention();
    
    // Process bottom-up salience from sensory input
    processBottomUpSalience(pImpl->sensoryInputBuffer);
    
    // Process top-down bias from goals
    processTopDownBias();
    
    // Manage working memory competition
    manageWorkingMemoryCompetition();
    
    // Control attentional scope
    controlAttentionalScope();
    
    // Update attention state
    updateAttentionState();
    
    // Enforce attentional constraints
    enforceAttentionalConstraints();
    
    // Maintain working memory traces
    maintainWorkingMemoryTraces();
}

void AttentionalSelection::processSensoryInput(const std::vector<float>& sensoryPattern,
                                            std::vector<float>& gatedOutput) {
    if (!acetylcholine_ || sensoryPattern.empty()) {
        gatedOutput = sensoryPattern;
        return;
    }
    
    // ACh-mediated sensory gating based on attentional state
    float cholinergicGain = (acetylcholine_) ? acetylcholine_->getLevel() * 2.0f : 0.0f;
    cholinergicGain = std::clamp(cholinergicGain, 0.0f, 2.0f);
    
    // Modulate sensory processing based on attentional scope
    float scopeModulator = (currentAttentionState_ == AttentionState::EXPLOITING) ? 0.7f :
                          (currentAttentionState_ == AttentionState::SCANNING) ? 1.3f :
                          1.0f;
    
    // Apply cholinergic gating
    gatedOutput.resize(sensoryPattern.size());
    for (size_t i = 0; i < sensoryPattern.size(); ++i) {
        float baseSignal = sensoryPattern[i];
        float cholinergic modulation = cholinergicGain * scopeModulator;
        
        // Feature binding - enhance correlated features
        float enhancement = 1.0f + (cholinergic modulation - 1.0f) * 0.5f;
        enhancement = std::clamp(enhancement, 0.5f, 2.0f);
        
        gatedOutput[i] = baseSignal * enhancement;
        
        // Store attentional gain for this input channel
        pImpl->attentionalGains[i] = modulation;
    }
}

std::vector<uint64_t> AttentionalSelection::selectWorkingMemoryTraces(
    const std::vector<uint64_t>& memoryTraces,
    float predictionError,
    const std::vector<float>& goalVector) {
    std::vector<MemoryTrace> traces;
    
    // Convert memory traces to competition format
    for (uint64_t neuronId : memoryTraces) {
        MemoryTrace trace;
        trace.neuronId = neuronId;
        
        // Get current activation
        auto& population = pImpl->attentionalPopulations[neuronId];
        trace.activation = std::accumulate(std::begin(population.activationHistory),
                                          std::end(population.activationHistory), 0.0f) / population.activationHistory.size();
        
        // Use prediction error from history or current
        trace.predictionError = (predictionErrorHistory_.find(neuronId) != predictionErrorHistory_.end()) ?
                               predictionErrorHistory_[neuronId] : std::abs(predictionError);
        
        // Calculate goal alignment
        trace.goalAlignment = calculateGoalAlignment(neuronId, goalVector);
        
        // Get novelty
        trace.novelty = population.uncertainty * 0.5f;
        
        // Get reward prediction
        trace.rewardPrediction = population.rewardPrediction;
        
        // Get uncertainty
        trace.uncertainty = population.uncertainty;
        
        // Calculate attentional weight
        trace.attentionalWeight = calculateAttentionalWeight(trace, goalVector);
        
        traces.push_back(trace);
    }
    
    // Sort traces by attentional weight
    std::sort(traces.begin(), traces.end(),
              [](const MemoryTrace& a, const MemoryTrace& b) {
                  return a.attentionalWeight > b.attentionalWeight;
              });
    
    // Select top traces based on attentional focus
    std::vector<uint64_t> selected;
    float selectionThreshold = calculateSelectionThreshold(traces, predictionError);
    
    for (const auto& trace : traces) {
        if (trace.attentionalWeight >= selectionThreshold) {
            selected.push_back(trace.neuronId);
        }
    }
    
    return selected;
}

void AttentionalSelection::applyPredictiveAttention(float predictionError,
                                                   float predictionConfidence,
                                                   const std::vector<float>& sensoryInput) {
    // Create prediction error focus
    PredictionErrorFocus focus;
    focus.errorMagnitude = std::abs(predictionError);
    focus.errorSign = (predictionError >= 0.0f) ? 1.0f : -1.0f;
    focus.expectedValue = predictionError * predictionConfidence;
    focus.surprise = (1.0f - predictionConfidence) * focus.errorMagnitude;
    focus.urgency = calculateErrorUrgency(focus);
    
    // Add to prediction error queue
    pImpl->activePredictionErrors.push_back(focus);
    
    // Store prediction error history
    if (!sensoryInput.empty()) {
        for (size_t i = 0; i < std::min(sensoryInput.size(), 10.0f); ++i) {
            uint64_t neuronId = static_cast<uint64_t>(i);
            predictionErrorHistory_[neuronId] = predictionError;
        }
    }
    
    // Update attentional populations based on prediction error
    for (auto& [neuronId, population] : pImpl->attentionalPopulations) {
        // Increase attentional focus on neurons with high prediction error
        float errorInfluence = std::min(1.0f, population.uncertainty * focus.urgency);
        
        // Modulate neural population dynamics
        population.dwellTime += errorInfluence * 10.0f;
        population.currentState = (errorInfluence > 0.5f) ? AttentionState::FOCUSED :
                                 (population.dwellTime > 50.0f) ? AttentionState::EXPLOITING :
                                 AttentionState::SCANNING;
        
        // Store the prediction error for this neuron
        auto& pop = pImpl->attentionalPopulations[neuronId];
        pop.uncertainty = std::min(1.0f, pop.uncertainty + errorInfluence * 0.1f);
    }
}

void AttentionalSelection::applyGoalDirectedAttention(const std::vector<float>& goalVector,
                                                     const std::vector<uint64_t>& availableActions,
                                                     std::vector<float>& actionWeights) {
    // Update goal vector
    currentGoalVector_ = goalVector;
    
    // Calculate goal attention strength
    float totalGoalImportance = 0.0f;
    for (float importance : goalVector) {
        totalGoalImportance += importance;
    }
    goalAttentionStrength_ = (totalGoalImportance > 0.0f) ? totalGoalImportance / goalVector.size() : 0.0f;
    
    // Apply goal weights to attentional populations
    for (uint64_t actionId : availableActions) {
        GoalWeights& weights = pImpl->goalWeights_[actionId];
        
        // Calculate goal alignment
        float goalAlignment = calculateActionGoalAlignment(actionId, goalVector);
        
        // Update goal weights based on current state
        weights.importance = goalAlignment * 0.7f + weights.importance * 0.3f;
        weights.salience = calculateActionSalience(actionId);
        weights.urgency = calculateActionUrgency(actionId, goalVector);
        weights.expectancies = calculateActionExpectancies(actionId, goalVector);
        
        // Apply to action weights
        float actionWeight = weights.importance * weights.salience * weights.urgency;
        actionWeights[actionId] = actionWeight;
        
        // Update attentional population
        if (pImpl->attentionalPopulations.find(actionId) != pImpl->attentionalPopulations.end()) {
            auto& population = pImpl->attentionalPopulations[actionId];
            population.bias += weights.urgency * 0.1f;
            population.bias = std::min(1.0f, population.bias);
            
            // Update attention state
            population.currentState = (weights.urgency > 0.7f) ? AttentionState::FOCUSED :
                                     (weights.importance > 0.5f) ? AttentionState::EXPLOITING :
                                     AttentionState::SCANNING;
        }
    }
}

void AttentionalSelection::adjustAttentionalScope(AttentionState newScope, float duration) {
    // Enforce attentional flexibility constraint
    float flexibilityReduction = (attentionalFlexibility_ < 0.5f) ? 0.5f : 0.0f;
    
    // Apply switching cost
    if (currentAttentionState_ != newScope) {
        float switchCost = dynamics_.switchingCost * (1.0f - attentionalFlexibility_);
        dynamics_.focusPersistence *= (1.0f - switchCost);
    }
    
    // Update attentional scope
    attentionalScope_ = newScope;
    
    // Adjust scope parameters based on new state
    switch (newScope) {
        case AttentionState::IDLE:
            attentionalBreadth_ = 0.1f;
            attentionalDepth_ = 0.1f;
            dynamics_.focusPersistence *= 0.8f;
            break;
        case AttentionState::SCANNING:
            attentionalBreadth_ = 1.0f - attentionalFlexibility_ * 0.3f;
            attentionalDepth_ = 0.3f + attentionalFlexibility_ * 0.4f;
            break;
        case AttentionState::EXPLOITING:
            attentionalBreadth_ = 0.5f;
            attentionalDepth_ = 1.0f - attentionalFlexibility_ * 0.2f;
            break;
        case AttentionState::RETURNING:
            attentionalBreadth_ = 0.7f;
            attentionalDepth_ = 0.7f;
            break;
        default:
            attentionalBreadth_ = 0.5f;
            attentionalDepth_ = 0.5f;
            break;
    }
    
    currentAttentionState_ = newScope;
}

float AttentionalSelection::getAttentionalFocus(uint64_t neuronId) const {
    auto it = pImpl->attentionalPopulations.find(neuronId);
    if (it != pImpl->attentionalPopulations.end()) {
        const auto& population = it->second;
        
        // Calculate attentional focus based on population state
        float stateValue = 0.0f;
        switch (population.currentState) {
            case AttentionState::IDLE: stateValue = 0.1f; break;
            case AttentionState::SCANNING: stateValue = 0.5f; break;
            case AttentionState::EXPLOITING: stateValue = 0.8f; break;
            case AttentionState::FOCUSED: stateValue = 1.0f; break;
            case AttentionState::RETURNING: stateValue = 0.6f; break;
        }
        
        // Combine state value with bias and salience
        float focus = stateValue * (population.bias + 0.1f) * (population.salience + 0.1f);
        
        // Apply attentional breadth modulation
        focus *= attentionalBreadth_;
        
        return focus;
    }
    
    return 0.0f;
}

void AttentionalSelection::reset() {
    // Clear attentional populations
    pImpl->attentionalPopulations.clear();
    pImpl->attentionalGains.clear();
    pImpl->attentionalInhibitions.clear();
    
    // Reset goal tracking
    pImpl->goalWeights_.clear();
    pImpl->attendedNeurons.clear();
    pImpl->attendedRegions.clear();
    
    // Reset prediction error tracking
    pImpl->activePredictionErrors.clear();
    predictionErrorHistory_.clear();
    
    // Reset state
    pImpl->currentState = AttentionState::IDLE;
    pImpl->previousState = AttentionState::IDLE;
    pImpl->lastAttentionSwitch = 0;
    
    // Reset buffers
    pImpl->sensoryInputBuffer.clear();
    pImpl->attentionalOutputBuffer.clear();
    pImpl->predictionErrorBuffer.clear();
    
    // Reset attentional dynamics
    dynamics_.focusPersistence = 100.0f;
    pImpl->attentionTimeSeries.clear();
    pImpl->attentionVariability = 0.0f;
    
    // Reset accumulated salience and bias
    pImpl->accumulatedSalience.clear();
    pImpl->accumulatedBias.clear();
    
    // Reset attention state
    currentAttentionState_ = AttentionState::IDLE;
    previousAttentionState_ = AttentionState::IDLE;
    goalAttentionStrength_ = 0.0f;
    attentionalBreadth_ = 0.5f;
    attentionalDepth_ = 0.5f;
    attentionalFlexibility_ = 0.5f;
    attentionalScope_ = AttentionState::IDLE;
    
    // Reset neuromodulator signals
    NeuromodulatorSignals emptySignals = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    lastNeuromodulatorSignals_ = emptySignals;
}

// Helper methods implementation
void AttentionalSelection::updateAttentionalGains() {
    for (auto& [neuronId, population] : pImpl->attentionalPopulations) {
        // Update attentional gains based on neuromodulators and state
        float cholinergicInfluence = (acetylcholine_) ? acetylcholine_->getLevel() : 0.0f;
        float noradrenergicInfluence = (norepinephrine_) ? norepinephrine_->getLevel() : 0.0f;
        
        // Update gain
        float gainUpdate = (cholinergicInfluence * 0.5f + noradrenergicInfluence * 0.3f);
        pImpl->attentionalGains[neuronId] += gainUpdate * 0.1f;
        
        // Clamp gain to reasonable range
        pImpl->attentionalGains[neuronId] = std::clamp(pImpl->attentionalGains[neuronId], 0.0f, 2.0f);
    }
}

void AttentionalSelection::processBottomUpSalience(const std::vector<float>& sensoryInput) {
    if (sensoryInput.empty()) return;
    
    for (size_t i = 0; i < sensoryInput.size(); ++i) {
        uint64_t neuronId = static_cast<uint64_t>(i);
        
        // Calculate bottom-up salience based on sensory input
        float inputStrength = sensoryInput[i];
        float novelty = (curiosity_) ? curiosity_->getExplorationDrive() : 0.0f;
        
        // Update accumulated salience
        pImpl->accumulatedSalience[neuronId] += inputStrength * novelty * 0.1f;
        
        // Apply decay
        pImpl->accumulatedSalience[neuronId] *= 0.95f;
        
        // Update attentional population
        if (pImpl->attentionalPopulations.find(neuronId) == pImpl->attentionalPopulations.end()) {
            AttentionalPopulation newPop;
            newPop.neuronId = neuronId;
            newPop.salience = 0.0f;
            newPop.bias = 0.0f;
            newPop.uncertainty = 0.5f;
            newPop.rewardPrediction = 0.0f;
            newPop.currentState = AttentionState::SCANNING;
            newPop.dwellTime = 0.0f;
            newPop.switchingCost = dynamics_.switchingCost;
            
            // Initialize activation history
            for (int j = 0; j < 10; ++j) {
                newPop.activationHistory[j] = 0.0f;
            }
            
            pImpl->attentionalPopulations[neuronId] = newPop;
        }
        
        auto& population = pImpl->attentionalPopulations[neuronId];
        population.salience = pImpl->accumulatedSalience[neuronId];
        
        // Update activation history
        for (int j = 8; j >= 0; --j) {
            population.activationHistory[j+1] = population.activationHistory[j];
        }
        population.activationHistory[0] = inputStrength;
    }
}

void AttentionalSelection::processTopDownBias() {
    // Update attentional populations based on goal vector
    for (auto& [neuronId, population] : pImpl->attentionalPopulations) {
        // Calculate goal alignment
        float goalAlignment = calculateGoalAlignment(neuronId, currentGoalVector_);
        
        // Update bias based on goal alignment
        population.bias += goalAlignment * goalAttentionStrength_ * 0.05f;
        population.bias = std::clamp(population.bias, 0.0f, 1.0f);
        
        // Update attention state based on bias and salience
        float attentionScore = population.bias * 0.6f + population.salience * 0.4f;
        
        if (attentionScore > 0.8f) {
            population.currentState = AttentionState::FOCUSED;
        } else if (attentionScore > 0.5f) {
            population.currentState = AttentionState::EXPLOITING;
        } else {
            population.currentState = AttentionState::SCANNING;
        }
        
        // Store in consolidated structure
        pImpl->attentionalPopulations[neuronId] = population;
    }
}

void AttentionalSelection::integrateNeuromodulatorSignals() {
    NeuromodulatorSignals signals;
    
    // Get current neuromodulator levels
    signals.acetylcholine = (acetylcholine_) ? acetylcholine_->getLevel() : 0.0f;
    signals.norepinephrine = (norepinephrine_) ? norepinephrine_->getLevel() : 0.0f;
    signals.dopamine = (dopamine_) ? dopamine_->getLevel() : 0.0f;
    signals.serotonin = (serotonin_) ? serotonin_->getLevel() : 0.0f;
    signals.curiosity = (curiosity_) ? curiosity_->getExplorationDrive() : 0.0f;
    
    // Store previous signals
    lastNeuromodulatorSignals_ = signals;
    
    // Update attentional dynamics based on neuromodulators
    // ACh: enhances feature binding and sensory processing
    if (signals.acetylcholine > 0.5f) {
        for (auto& [neuronId, population] : pImpl->attentionalPopulations) {
            population.bias += (signals.acetylcholine - 0.5f) * 0.2f;
        }
    }
    
    // NE: increases arousal and attentional focus
    if (signals.norepinephrine > 0.5f) {
        dynamics_.focusPersistence *= 1.2f;
        dynamics_.captureThreshold *= 0.9f;
    }
    
    // DA: modulates reward-based attention
    if (std::abs(signals.dopamine) > 0.5f) {
        for (auto& [neuronId, population] : pImpl->attentionalPopulations) {
            population.rewardPrediction += signals.dopamine * 0.1f;
            population.rewardPrediction = std::clamp(population.rewardPrediction, -1.0f, 1.0f);
        }
    }
    
    // Serotonin: modulates behavioral state
    if (signals.serotonin > 0.5f) {
        // High serotonin promotes systematic exploration
        attentionalFlexibility_ = std::min(1.0f, attentionalFlexibility_ + 0.1f);
    } else if (signals.serotonin < 0.3f) {
        // Low serotonin promotes impulsive action
        attentionalFlexibility_ = std::max(0.0f, attentionalFlexibility_ - 0.1f);
    }
}

void AttentionalSelection::updatePredictionErrorBasedAttention() {
    // Process prediction errors from queue
    for (const auto& focus : pImpl->activePredictionErrors) {
        // Update attentional populations based on prediction error
        for (auto& [neuronId, population] : pImpl->attentionalPopulations) {
            // Increase uncertainty based on prediction error
            population.uncertainty = std::min(1.0f, population.uncertainty + focus.urgency * 0.05f);
            
            // Update expected value based on error sign
            if (focus.errorSign > 0.0f) {
                // Positive error - increase expected value
                population.expectedValue += focus.errorMagnitude * 0.1f;
            } else {
                // Negative error - decrease expected value
                population.expectedValue -= focus.errorMagnitude * 0.1f;
            }
            
            population.expectedValue = std::clamp(population.expectedValue, -1.0f, 1.0f);
            
            // Update attention state based on error
            if (focus.urgency > 0.7f) {
                population.currentState = AttentionState::FOCUSED;
            }
        }
    }
    
    // Clear processed prediction errors
    pImpl->activePredictionErrors.clear();
}

void AttentionalSelection::manageWorkingMemoryCompetition() {
    // Create memory trace competition
    for (auto& [neuronId, population] : pImpl->attentionalPopulations) {
        MemoryTrace trace;
        trace.neuronId = neuronId;
        
        // Calculate trace attributes
        trace.activation = std::accumulate(std::begin(population.activationHistory),
                                          std::end(population.activationHistory), 0.0f) / 10.0f;
        
        float activationStrength = population.bias * 0.6f + population.salience * 0.4f;
        trace.goalAlignment = activationStrength * goalAttentionStrength_;
        
        // Calculate novelty
        float totalActivation = 0.0f;
        for (float activation : population.activationHistory) {
            totalActivation += std::abs(activation);
        }
        trace.novelty = (totalActivation / 10.0f) * (1.0f - population.uncertainty);
        
        // Calculate reward prediction
        trace.rewardPrediction = population.rewardPrediction;
        
        // Calculate uncertainty
        trace.uncertainty = population.uncertainty;
        
        // Calculate attentional weight
        trace.attentionalWeight = calculateAttentionalWeight(trace, currentGoalVector_);
        
        pImpl->competingMemoryTraces.push_back(trace);
    }
    
    // Sort traces by attentional weight
    std::sort(pImpl->competingMemoryTraces.begin(), pImpl->competingMemoryTraces.end(),
              [](const MemoryTrace& a, const MemoryTrace& b) {
                  return a.attentionalWeight > b.attentionalWeight;
              });
    
    // Select winners
    pImpl->attendedNeurons.clear();
    float threshold = calculateSelectionThreshold(pImpl->competingMemoryTraces, 0.0f);
    
    for (const auto& trace : pImpl->competingMemoryTraces) {
        if (trace.attentionalWeight >= threshold && pImpl->attendedNeurons.size() < 10) {
            pImpl->attendedNeurons.push_back(trace.neuronId);
        }
    }
    
    // Clear for next iteration
    pImpl->competingMemoryTraces.clear();
}

void AttentionalSelection::controlAttentionalScope() {
    // Update attentional scope based on current state
    if (currentAttentionState_ != attentionalScope_) {
        // Switch occurred - apply switching cost
        float cost = dynamics_.switchingCost * (1.0f - attentionalFlexibility_);
        dynamics_.focusPersistence *= (1.0f - cost);
        
        // Update scope
        attentionalScope_ = currentAttentionState_;
        
        // Record time series
        pImpl->attentionTimeSeries.push_back(attentionalBreadth_);
        if (pImpl->attentionTimeSeries.size() > 100) {
            pImpl->attentionTimeSeries.pop_front();
        }
    }
    
    // Adapt scope based on neuromodulator levels
    float neuromodulationFactor = 0.0f;
    if (acetylcholine_ && norepinephrine_ && dopamine_) {
        neuromodulationFactor = (acetylcholine_->getLevel() + 
                               norepinephrine_->getLevel() + 
                               std::abs(dopamine_->getLevel())) / 3.0f;
    }
    
    // Adjust attentional breadth based on neuromodulation
    if (neuromodulationFactor > 0.8f) {
        attentionalBreadth_ = std::min(1.0f, attentionalBreadth_ + 0.05f);
    } else if (neuromodulationFactor < 0.3f) {
        attentionalBreadth_ = std::max(0.1f, attentionalBreadth_ - 0.05f);
    }
    
    // Ensure attentional breadth stays within reasonable bounds
    attentionalBreadth_ = std::clamp(attentionalBreadth_, 0.1f, 1.0f);
}

void AttentionalSelection::updateAttentionState() {
    // Update attention state based on dynamics
    float timeSinceSwitch = static_cast<float>(currentStep) - pImpl->lastAttentionSwitch;
    
    if (timeSinceSwitch > dynamics_.focusPersistence) {
        // Focus time exceeded - switch attention
        previousAttentionState_ = currentAttentionState_;
        
        // Determine next attention state
        if (currentAttentionState_ == AttentionState::IDLE) {
            currentAttentionState_ = AttentionState::SCANNING;
        } else if (currentAttentionState_ == AttentionState::SCANNING) {
            currentAttentionState_ = AttentionState::EXPLOITING;
        } else if (currentAttentionState_ == AttentionState::EXPLOITING) {
            currentAttentionState_ = AttentionState::RETURNING;
        } else {
            currentAttentionState_ = AttentionState::IDLE;
        }
        
        pImpl->lastAttentionSwitch = currentStep;
    }
    
    // Update attentional flexibility based on performance
    if (!pImpl->attentionTimeSeries.empty()) {
        float recentBreadth = pImpl->attentionTimeSeries.back();
        float meanBreadth = std::accumulate(pImpl->attentionTimeSeries.begin(), 
                                           pImpl->attentionTimeSeries.end(), 0.0f) /
                           pImpl->attentionTimeSeries.size();
        
        pImpl->attentionVariability = std::abs(recentBreadth - meanBreadth);
        
        // Adapt attentional flexibility based on variability
        attentionalFlexibility_ = std::clamp(0.5f + pImpl->attentionVariability - 0.5f, 0.1f, 1.0f);
    }
}

void AttentionalSelection::enforceAttentionalConstraints() {
    // Enforce attentional breadth constraints
    attentionalBreadth_ = std::clamp(attentionalBreadth_, 0.1f, 1.0f);
    attentionalDepth_ = std::clamp(attentionalDepth_, 0.1f, 1.0f);
    attentionalFlexibility_ = std::clamp(attentionalFlexibility_, 0.1f, 1.0f);
    
    // Apply attentional gains constraints
    for (auto& [neuronId, gain] : pImpl->attentionalGains) {
        gain = std::clamp(gain, 0.0f, 2.0f);
    }
    
    // Apply attentional inhibitions constraints
    for (auto& [neuronId, inhibition] : pImpl->attentionalInhibitions) {
        inhibition = std::clamp(inhibition, 0.0f, 1.0f);
    }
}

void AttentionalSelection::maintainWorkingMemoryTraces() {
    // Decay attentional populations over time
    for (auto& [neuronId, population] : pImpl->attentionalPopulations) {
        // Decay salience and bias
        population.salience *= 0.95f;
        population.bias *= 0.98f;
        
        // Update dwell time
        population.dwellTime += 1.0f;
        
        // Reset if dwell time too long
        if (population.dwellTime > 100.0f) {
            population.dwellTime = 0.0f;
        }
    }
}

// Helper method implementations
float AttentionalSelection::calculateGoalAlignment(uint64_t neuronId,
                                                   const std::vector<float>& goalVector) const {
    if (goalVector.empty()) return 0.5f;
    
    // Simple heuristic for goal alignment
    // In a full implementation, this would use neural mapping
    float alignment = 0.5f + static_cast<float>(neuronId % 7) / 7.0f * 0.5f;
    
    // Modulate by goal vector
    for (size_t i = 0; i < std::min(goalVector.size(), 3.0f); ++i) {
        alignment *= (1.0f + goalVector[i] * 0.2f);
    }
    
    return std::clamp(alignment, 0.0f, 1.0f);
}

float AttentionalSelection::calculateAttentionalWeight(const MemoryTrace& trace,
                                                     const std::vector<float>& goalVector) const {
    float weight = 0.0f;
    
    // Combine multiple factors
    weight += trace.activation * 0.2f;
    weight += trace.goalAlignment * 0.3f;
    weight += trace.rewardPrediction * 0.2f;
    weight += (1.0f - trace.uncertainty) * 0.2f;  // Less uncertainty = higher weight
    weight += trace.novelty * 0.1f;
    
    return std::clamp(weight, 0.0f, 1.0f);
}

float AttentionalSelection::calculateSelectionThreshold(const std::vector<MemoryTrace>& traces,
                                                      float predictionError) const {
    if (traces.empty()) return 0.5f;
    
    // Calculate threshold based on number of traces and prediction error
    float threshold = 0.5f;
    
    // Adjust based on prediction error
    threshold += std::abs(predictionError) * 0.3f;
    
    // Adjust based on number of traces
    float traceCountFactor = std::min(1.0f, traces.size() / 10.0f);
    threshold *= (1.0f + traceCountFactor * 0.2f);
    
    return std::clamp(threshold, 0.0f, 1.0f);
}

float AttentionalSelection::calculateErrorUrgency(const PredictionErrorFocus& focus) const {
    float urgency = focus.surprise * 0.5f + focus.errorMagnitude * 0.3f;
    urgency += (1.0f - focus.errorConfidence) * 0.2f;  // Need errorConfidence in struct
    
    return std::clamp(urgency, 0.0f, 1.0f);
}

float AttentionalSelection::calculateActionGoalAlignment(uint64_t actionId,
                                                        const std::vector<float>& goalVector) const {
    if (goalVector.empty()) return 0.5f;
    
    // Simple heuristic for action-goal alignment
    float alignment = static_cast<float>(actionId % 5) / 5.0f * 0.8f + 0.2f;
    
    // Modulate by goal vector
    for (float goalComponent : goalVector) {
        alignment *= (1.0f + goalComponent * 0.1f);
    }
    
    return std::clamp(alignment, 0.0f, 1.0f);
}

float AttentionalSelection::calculateActionSalience(uint64_t actionId) const {
    // Simple heuristic for action salience
    return (static_cast<float>(actionId % 3) + 0.5f) / 3.0f;
}

float AttentionalSelection::calculateActionUrgency(uint64_t actionId,
                                                 const std::vector<float>& goalVector) const {
    // Calculate urgency based on goal alignment and time pressure
    float urgency = calculateActionGoalAlignment(actionId, goalVector);
    
    // Add some randomness
    urgency *= (0.8f + static_cast<float>(actionId % 5) / 10.0f);
    
    return std::clamp(urgency, 0.0f, 1.0f);
}

float AttentionalSelection::calculateActionExpectancies(uint64_t actionId,
                                                      const std::vector<float>& goalVector) const {
    // Calculate expected outcomes from action
    float expectancy = 0.5f + static_cast<float>(actionId % 7) / 7.0f * 0.5f;
    
    // Modulate by goal vector
    for (float goalComponent : goalVector) {
        expectancy *= (1.0f + goalComponent * 0.15f);
    }
    
    return std::clamp(expectancy, 0.0f, 1.0f);
}

} // namespace nlm
