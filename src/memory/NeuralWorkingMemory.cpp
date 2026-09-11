#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Pattern signatures for quick lookup
    std::vector<std::vector<float>> patternSignatures;
    
    // Temporal sequence buffer
    std::deque<size_t> temporalSequence;
    
    Impl() : brain(nullptr) {}
};

// Helper methods for working memory implementation
NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , episodicMemoryCreated_(false)
    , predictionSystemIntegrated_(false)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

std::vector<float> NeuralWorkingMemory::encodePattern(const SensoryPattern& sensory, 
                                                     const ActionContext& action) const {
    std::vector<float> encoded;
    
    // Encode sensory state to neural pattern
    encoded.insert(encoded.end(), sensory.features.begin(), sensory.features.end());
    
    // Add action encoding
    encoded.push_back(static_cast<float>(action.type));
    encoded.insert(encoded.end(), action.parameters.begin(), action.parameters.end());
    
    // Add contextual features
    encoded.push_back(sensory.position[0]);
    encoded.push_back(sensory.position[1]);
    encoded.push_back(sensory.orientation);
    
    return encoded;
}

SensoryPattern NeuralWorkingMemory::decodeSensory(const std::vector<float>& encoded) const {
    SensoryPattern sensory;
    
    // Parse encoded pattern
    size_t pos = 0;
    
    // Extract sensory features (first 10 values)
    size_t numFeatures = std::min(encoded.size(), static_cast<size_t>(10));
    sensory.features.assign(encoded.begin(), encoded.begin() + numFeatures);
    pos = numFeatures;
    
    // Extract position (next 2 values)
    if (pos + 2 < encoded.size()) {
        sensory.position[0] = encoded[pos];
        sensory.position[1] = encoded[pos + 1];
        pos += 2;
    }
    
    // Extract orientation (next value)
    if (pos < encoded.size()) {
        sensory.orientation = encoded[pos];
        pos++;
    }
    
    return sensory;
}

ActionContext NeuralWorkingMemory::decodeAction(const std::vector<float>& encoded) const {
    ActionContext action;
    
    // Parse encoded pattern
    size_t pos = 0;
    
    // Skip sensory features (first 10 values)
    pos = std::min(encoded.size(), static_cast<size_t>(10));
    
    // Extract action type (next value)
    if (pos < encoded.size()) {
        action.type = static_cast<ActionType>(static_cast<int>(encoded[pos]));
        pos++;
    }
    
    // Extract action parameters (next up to 5 values)
    size_t numParams = std::min(encoded.size() - pos, static_cast<size_t>(5));
    action.parameters.assign(encoded.begin() + pos, encoded.begin() + pos + numParams);
    
    return action;
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
}

void NeuralWorkingMemory::updateTemporalSequences() {
    // Update temporal sequence based on activation levels
    temporalSequence_.clear();
    
    std::vector<std::pair<float, size_t>> weightedIndices;
    for (size_t i = 0; i < traces_.size(); ++i) {
        weightedIndices.emplace_back(traces_[i].activationLevel, i);
    }
    
    // Sort by activation (descending) to form sequence
    std::sort(weightedIndices.begin(), weightedIndices.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Build sequence of trace IDs
    for (const auto& pair : weightedIndices) {
        temporalSequence_.push_back(pair.second);
    }
    
    lastSequenceUpdate_ = 0;  // Will be set by brain time
}

void NeuralWorkingMemory::linkTemporalSequences() {
    // Link related traces in temporal sequence
    for (size_t i = 0; i < traces_.size(); ++i) {
        traces_[i].associatedTraceIds.clear();
    }
    
    // Create temporal links (trace i is linked to i+1)
    for (size_t i = 0; i < traces_.size() - 1; ++i) {
        traces_[i].associatedTraceIds.push_back(i + 1);
        
        // Also create reverse link
        if (i + 1 < traces_.size()) {
            traces_[i + 1].associatedTraceIds.push_back(i);
        }
    }
}

void NeuralWorkingMemory::createEpisodicMemoryItem(const MemoryTrace& trace) {
    // Create episodic memory item from working memory trace
    EpisodicMemoryItem item;
    item.timestamp = trace.creationTime;
    item.sensoryState = trace.sensoryPattern.rawSignals;
    item.positionX = trace.sensoryPattern.position[0];
    item.positionY = trace.sensoryPattern.position[1];
    item.orientation = trace.sensoryPattern.orientation;
    item.action = trace.actionContext.type;
    item.reward = trace.actionContext.outcomeReward;
    item.energy = trace.actionContext.outcomeEnergy;
    item.novelty = 0.5f;  // Default novelty
    item.resultingSensoryState = trace.actionContext.outcomeSensoryState;
    item.resultingReward = trace.actionContext.outcomeReward;
    item.activeNeurons = trace.neuralPattern;
    item.neuronActivations = trace.neuralPattern;
    item.age = 0;
    
    // In a full implementation, this would be passed to the episodic memory system
    episodicMemoryCreated_ = true;
    
    NLM_LOG_INFO("Created episodic memory item from working memory trace");
}

void NeuralWorkingMemory::updatePredictions() {
    // Update predictions based on working memory contents
    if (!predictionSystemIntegrated_ || !brain_) return;
    
    for (auto& trace : traces_) {
        // Simple prediction: extend current pattern
        std::vector<float> predictedNext = trace.predictedNextState;
        
        if (predictedNext.empty()) {
            // Generate prediction based on current trace
            predictedNext = trace.neuralPattern;
            predictedNext.push_back(trace.activationLevel * 0.1f);
            predictedNext.push_back(trace.predictionConfidence);
            predictedNext.push_back(trace.sensoryPattern.position[0]);
            predictedNext.push_back(trace.sensoryPattern.position[1]);
        }
        
        trace.predictedNextState = predictedNext;
    }
}

void NeuralWorkingMemory::incorporatePredictionError(const std::vector<float>& error) {
    // Incorporate prediction error into memory traces
    if (error.size() != traces_.back().predictionError.size()) {
        // Resize if needed
        traces_.back().predictionError = std::vector<float>(error.size(), 0.0f);
    }
    
    // Blend error into existing prediction error
    for (size_t i = 0; i < std::min(error.size(), traces_.back().predictionError.size()); ++i) {
        traces_.back().predictionError[i] = 
            traces_.back().predictionError[i] * 0.5f + error[i] * 0.5f;
    }
    
    // Adjust prediction confidence based on error
    float totalError = 0.0f;
    for (float err : traces_.back().predictionError) {
        totalError += err * err;
    }
    traces_.back().predictionConfidence = std::max(0.0f, 1.0f - std::sqrt(totalError));
}

void NeuralWorkingMemory::advancedCompetition() {
    // Enhanced competition with prediction gating
    winners_.clear();
    
    if (traces_.empty()) return;
    
    // Compute scores based on activation, confidence, and prediction
    std::vector<std::pair<float, size_t>> scores;
    for (size_t i = 0; i < traces_.size(); ++i) {
        const auto& trace = traces_[i];
        
        // Multiple factors for scoring
        float activationScore = trace.activationLevel;
        float confidenceScore = trace.predictionConfidence;
        float predictionScore = 1.0f - (computePatternSimilarity(trace.predictedNextState, 
                                               trace.actionContext.outcomeSensoryState));
        
        // Weighted combination
        float totalScore = activationScore * 0.4f + confidenceScore * 0.3f + predictionScore * 0.3f;
        
        scores.emplace_back(totalScore, i);
    }
    
    // Sort by score (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select winners (top 30% of traces)
    size_t numWinners = std::max(size_t(1), static_cast<size_t>(scores.size() * 0.3f));
    for (size_t i = 0; i < numWinners; ++i) {
        winners_.push_back(memoryNeurons_[scores[i].second]);
        
        // Apply prediction gating: winners get additional excitation
        if (brain_) {
            brain_->injectCurrent(memoryNeurons_[scores[i].second], 
                                scores[i].first * 3.0f);
        }
    }
}

void NeuralWorkingMemory::applyPredictionGating() {
    // Apply prediction-based gating to memory updates
    if (!brain_) return;
    
    for (size_t i = 0; i < traces_.size(); ++i) {
        auto& trace = traces_[i];
        
        // Check if prediction is reliable
        if (trace.predictionConfidence > 0.7f) {
            // High confidence predictions get boosted
            if (i < memoryNeurons_.size()) {
                brain_->injectCurrent(memoryNeurons_[i], 
                                    trace.activationLevel * 2.0f);
            }
        } else if (trace.predictionConfidence < 0.3f) {
            // Low confidence predictions get suppressed
            if (i < memoryNeurons_.size()) {
                brain_->injectCurrent(memoryNeurons_[i], 
                                    -trace.activationLevel * 2.0f);
            }
        }
    }
}

void NeuralWorkingMemory::completePattern(const std::vector<float>& partialPattern) {
    // Complete pattern using stored traces
    completionPattern_ = partialPattern;
    
    // Find similar traces to complete the pattern
    std::vector<size_t> similarIndices;
    for (size_t i = 0; i < traces_.size(); ++i) {
        float sim = computePatternSimilarity(partialPattern, traces_[i].patternSignature);
        if (sim > 0.5f) {  // Similarity threshold
            similarIndices.push_back(i);
        }
    }
    
    // Combine patterns from similar traces
    std::vector<float> completedPattern;
    for (size_t idx : similarIndices) {
        const auto& trace = traces_[idx];
        completedPattern.insert(completedPattern.end(), 
                               trace.neuralPattern.begin(), trace.neuralPattern.end());
    }
    
    if (!completedPattern.empty()) {
        // Normalize the completed pattern
        float norm = 0.0f;
        for (float val : completedPattern) norm += val * val;
        norm = std::sqrt(norm);
        if (norm > 0.0f) {
            for (float& val : completedPattern) val /= norm;
        }
    }
    
    // Store completed pattern in a new trace
    MemoryTrace completionTrace;
    completionTrace.neuralPattern = completedPattern;
    completionTrace.activationLevel = !completedPattern.empty() ? 
        *std::max_element(completedPattern.begin(), completedPattern.end()) : 0.0f;
    
    // Add to traces if capacity allows
    if (traces_.size() < capacity_) {
        traces_.push_back(completionTrace);
        memoryNeurons_.push_back(NeuronId(static_cast<uint64_t>(memoryNeurons_.size() + 20000)));
        memoryActivations_.push_back(completionTrace.activationLevel);
        memoryTimestamps_.push_back(0);
    }
}

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation,
                                      const SensoryPattern& sensory,
                                      const ActionContext& action) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    MemoryTrace trace;
    trace.creationTime = 0;  // Will be set by brain time
    trace.lastUpdateTime = 0;
    trace.neuralPattern = {activation};
    trace.activationLevel = activation;
    trace.predictionConfidence = 0.5f;  // Default confidence
    trace.sensoryPattern = sensory;
    trace.actionContext = action;
    
    // Encode pattern for later retrieval
    trace.patternSignature = encodeToPatternSignature({activation}, sensory, action);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        
        // Update existing trace
        if (idx < traces_.size()) {
            traces_[idx] = trace;
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        
        // Add new trace
        traces_.push_back(trace);
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
    
    // Update temporal sequence
    updateTemporalSequences();
}

std::vector<float> NeuralWorkingMemory::encodeToPatternSignature(
    const std::vector<float>& neuralPattern,
    const SensoryPattern& sensory,
    const ActionContext& action) const {
    std::vector<float> signature;
    
    // Combine neural pattern, sensory features, and action parameters
    signature.reserve(neuralPattern.size() + sensory.features.size() + 
                      action.parameters.size() + 6);  // +6 for position, orientation, reward
    
    // Add neural pattern
    signature.insert(signature.end(), neuralPattern.begin(), neuralPattern.end());
    
    // Add sensory features
    signature.insert(signature.end(), sensory.features.begin(), sensory.features.end());
    
    // Add action parameters
    signature.insert(signature.end(), action.parameters.begin(), action.parameters.end());
    
    // Add contextual features
    signature.push_back(sensory.position[0]);
    signature.push_back(sensory.position[1]);
    signature.push_back(sensory.orientation);
    signature.push_back(action.outcomeReward);
    signature.push_back(action.outcomeEnergy);
    signature.push_back(action.succeeded ? 1.0f : 0.0f);
    
    return signature;
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryNeurons_.size());
    
    // Combine activations from all traces
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (i < traces_.size()) {
            // Use enhanced activation from trace
            result.push_back(traces_[i].activationLevel);
        } else {
            result.push_back(memoryActivations_[i]);
        }
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply strong inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    return total / memoryActivations_.size();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
}

// AttentionalSelection Implementation
struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    
    Impl() {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        activities[i] = salience + bias;
        totalActivity += activities[i];
    }
    
    if (totalActivity < 0.001f) {
        // No strong competitors - all equal
        return competitors;
    }
    
    // Competition: neurons inhibit each other based on relative activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relativeActivity = activities[i] / (activities[j] + 0.001f);
            
            if (relativeActivity > 1.5f) {
                // i is much stronger than j - apply inhibition to j
                if (brain_) {
                    brain_->injectCurrent(competitors[j], -globalInhibition * inhibitionStrength_);
                }
                pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
            }
        }
    }
    
    // Winners are neurons with above-threshold activity
    float threshold = competitionThreshold_ * totalActivity / competitors.size();
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * 2.0f);
            }
        }
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = strength;
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = strength;
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = threshold;
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return excitationStrength_;
    }
    return 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience and bias over time
    for (auto& s : bottomUpSalience_) {
        s *= 0.95f;
    }
    
    for (auto& b : topDownBias_) {
        b *= 0.98f;
    }
    
    // Decay inhibition
    for (auto& i : pImpl->inhibitionLevel) {
        i *= 0.9f;
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    // Store bias in map keyed by NeuronId
    auto it = topDownBias_.find(neuron.value);
    if (it != topDownBias_.end()) {
        it->second += biasStrength;
    } else {
        topDownBias_[neuron.value] = biasStrength;
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    // Store salience in map keyed by NeuronId
    auto it = bottomUpSalience_.find(neuron.value);
    if (it != bottomUpSalience_.end()) {
        it->second += salienceStrength;
    } else {
        bottomUpSalience_[neuron.value] = salienceStrength;
    }
}

void AttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
}

} // namespace nlm