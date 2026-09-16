#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Advanced management features
    std::vector<float> memoryImportance;  // 0-1 importance score
    std::vector<int> memoryPriorities;  // Priority enum (0=IMPORTANT, 1=RECENT, 2=WEAK)
    std::vector<std::vector<float>> memoryPatternCache;
    std::vector<std::vector<size_t>> memoryPatternIndices;
    std::vector<std::vector<NeuronId>> temporalBindingGroups;
    std::vector<std::vector<float>> memoryContexts;
    std::vector<std::vector<float>> patternLibrary;
    
    // Advanced neural dynamics
    float thetaPhase;
    float gammaPhase;
    SimulationStep replayCounter;
    float consolidationTimer;
    float gammaAmplitude;
    float thetaAmplitude;
    
    // Memory interference tracking
    std::vector<float> interferenceHistory;
    std::vector<float> resolutionScore;
    
    // Attention gating
    std::vector<bool> attentionGating;
    
    Impl() : brain(nullptr), thetaPhase(0.0f), gammaPhase(0.0f), 
             replayCounter(0), consolidationTimer(0.0f), 
             gammaAmplitude(1.0f), thetaAmplitude(1.0f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , predictionErrorThreshold_(0.5f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

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

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
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

void NeuralWorkingMemory::integrateWithAttention(const std::vector<NeuronId>& attendedNeurons) {
    if (!brain_) return;
    
    // Boost activation of attended neurons
    for (const auto& neuronId : attendedNeurons) {
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (memoryNeurons_[i] == neuronId) {
                // Increase activation for attended neurons
                memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * 1.2f);
                break;
            }
        }
    }
}

void NeuralWorkingMemory::updateWithPredictionError(float predictionError, TimestepDuration dt) {
    if (!brain_) return;
    
    // Store in working memory when prediction error is high
    if (predictionError > getPredictionErrorThreshold()) {
        // Store the current prediction error as a memory trace
        size_t maxNeurons = std::min(memoryNeurons_.size(), memoryActivations_.size());
        for (size_t i = 0; i < maxNeurons; ++i) {
            // Scale error for storage
            float errorTrace = predictionError * 0.1f * (1.0f + memoryActivations_[i]);
            memoryActivations_[i] = std::max(memoryActivations_[i], errorTrace);
        }
        
        // If we have room, add new memory traces for prediction error
        if (memoryNeurons_.size() < capacity_) {
            // Add a new memory trace representing the prediction error
            NeuronId errorNeuron(99999); // Special ID for error trace
            memoryNeurons_.push_back(errorNeuron);
            memoryActivations_.push_back(predictionError * 0.5f);
            memoryTimestamps_.push_back(0);
        }
    } else {
        // Decay prediction error memory traces
        for (size_t i = 0; i < memoryActivations_.size(); ++i) {
            if (memoryNeurons_[i].value >= 99000) { // Error trace neuron
                memoryActivations_[i] *= (1.0f - decayRate_ * 2.0f);
            }
        }
    }
}

void NeuralWorkingMemory::storeSensoryInputWithError(const std::vector<float>& sensoryInput, float predictionError) {
    if (!brain_ || sensoryInput.empty()) return;
    
    // Only store when prediction error is significant
    if (predictionError > getPredictionErrorThreshold()) {
        // Encode sensory input as neural activity pattern
        size_t neuronsToStore = std::min(sensoryInput.size(), memoryNeurons_.size());
        for (size_t i = 0; i < neuronsToStore; ++i) {
            // Normalize input
            float activation = sensoryInput[i] * 0.5f;
            
            // Find matching neuron or create new one
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), memoryNeurons_[i]);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                memoryActivations_[idx] = std::max(memoryActivations_[idx], activation);
            } else if (memoryNeurons_.size() < capacity_) {
                // Add new neuron for this sensory input
                memoryNeurons_.push_back(memoryNeurons_[i]);
                memoryActivations_.push_back(activation);
                memoryTimestamps_.push_back(0);
            }
        }
    }
}

std::vector<float> NeuralWorkingMemory::retrieveWithAttention(float attentionLevel) const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        // Apply attention modulation
        float modulatedActivation = activation * (1.0f + attentionLevel * 0.5f);
        result.push_back(std::min(modulatedActivation, 1.0f));
    }
    
    return result;
}

void NeuralWorkingMemory::connectToNeuromodulation(float neuromodulationLevel) {
    if (!brain_) return;
    
    // Apply neuromodulation effects on working memory
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        // Neuromodulation affects memory trace stability
        if (neuromodulationLevel > 0.5f) {
            // High neuromodulation strengthens memory traces
            memoryActivations_[i] = std::min(1.0f, activation * (1.0f + neuromodulationLevel * 0.3f));
        } else {
            // Low neuromodulation causes decay
            memoryActivations_[i] *= (1.0f - neuromodulationLevel * 0.2f);
        }
        
        // Inject neuromodulated current
        if (activation > 0.1f) {
            brain_->injectCurrent(memoryNeurons_[i], activation * neuromodulationLevel * 2.0f);
        }
    }
}

// Memory prioritization system (important vs. recent vs. weak)
void NeuralWorkingMemory::prioritizeMemory() {
    if (memoryNeurons_.empty()) return;
    
    // Compute memory priorities based on multiple factors
    std::vector<float> importanceScores(memoryNeurons_.size(), 0.0f);
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        float age = static_cast<float>(memoryTimestamps_[i]);
        
        // Importance = (activation * 0.5f) + (recent boost * 0.3f) + (age penalty * 0.2f)
        float importance = activation * 0.5f;
        
        if (age < 50.0f) {
            importance += 0.3f;  // Recent memories get boost
        }
        
        if (age > 500.0f) {
            importance -= 0.2f;  // Very old memories penalized
        }
        
        importanceScores[i] = std::max(0.0f, std::min(1.0f, importance));
    }
    
    // Update priority and importance arrays
    pImpl->memoryPriorities.clear();
    pImpl->memoryImportance.clear();
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        int priority;
        if (importanceScores[i] > 0.7f) {
            priority = static_cast<int>(MemoryPriority::IMPORTANT);
            pImpl->memoryImportance.push_back(importanceScores[i]);
        } else if (importanceScores[i] > 0.4f) {
            priority = static_cast<int>(MemoryPriority::RECENT);
            pImpl->memoryImportance.push_back(importanceScores[i] * 0.8f);
        } else {
            priority = static_cast<int>(MemoryPriority::WEAK);
            pImpl->memoryImportance.push_back(importanceScores[i] * 0.5f);
        }
        pImpl->memoryPriorities.push_back(priority);
    }
}

// Memory compression for capacity optimization
void NeuralWorkingMemory::compressMemory() {
    if (memoryNeurons_.empty()) return;
    
    // Create compressed representation using pattern caching
    pImpl->memoryPatternCache.clear();
    pImpl->memoryPatternIndices.clear();
    
    // Group similar activations for compression
    std::vector<std::vector<size_t>> compressedGroups;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool foundGroup = false;
        
        for (auto& group : compressedGroups) {
            if (!group.empty()) {
                size_t refIdx = group[0];
                float similarity = 1.0f - std::abs(memoryActivations_[i] - memoryActivations_[refIdx]);
                
                if (similarity > 0.8f) {
                    // Add to existing compressed group
                    group.push_back(i);
                    foundGroup = true;
                    break;
                }
            }
        }
        
        if (!foundGroup) {
            // Create new group
            std::vector<size_t> newGroup{i};
            compressedGroups.push_back(newGroup);
        }
    }
    
    // Create pattern cache for compressed groups
    for (const auto& group : compressedGroups) {
        if (!group.empty()) {
            std::vector<float> pattern;
            std::vector<size_t> patternIndices;
            
            // Average activations for pattern
            float avgActivation = 0.0f;
            for (size_t idx : group) {
                avgActivation += memoryActivations_[idx];
                patternIndices.push_back(idx);
            }
            avgActivation /= group.size();
            
            // Fill pattern vector (all entries are the same average)
            pattern.resize(memoryNeurons_.size(), 0.0f);
            for (size_t idx : group) {
                pattern[idx] = avgActivation;
            }
            
            pImpl->memoryPatternCache.push_back(pattern);
            pImpl->memoryPatternIndices.push_back(patternIndices);
        }
    }
    
    // Update memory importance based on compression
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (pImpl->memoryImportance.size() > i) {
            pImpl->memoryImportance[i] *= 0.9f;  // Slight decay for compressed traces
        }
    }
}

// Replay-based memory strengthening
void NeuralWorkingMemory::replayStrengthenMemories() {
    pImpl->replayCounter++;
    
    // Replay important memories more frequently
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (pImpl->memoryImportance.size() > i) {
            float importance = pImpl->memoryImportance[i];
            
            // Replay based on importance
            if (importance > 0.7f) {
                // Strong replay
                memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * 1.1f);
                pImpl->memoryImportance[i] = std::min(1.0f, importance * 1.05f);
                
                // Update pattern library
                if (pImpl->patternLibrary.size() > i) {
                    pImpl->patternLibrary[i].lastReplay = pImpl->replayCounter;
                }
            } else if (importance > 0.4f) {
                // Medium replay
                memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * 1.05f);
            }
            
            // Decay replay counter contribution
            if (memoryTimestamps_[i] > 100) {
                memoryTimestamps_[i] = std::max(0, static_cast<int>(memoryTimestamps_[i] - 2));
            }
        }
    }
    
    // Update memory patterns for replay
    updateMemoryPatterns();
}

// Update memory patterns for storage efficiency
void NeuralWorkingMemory::updateMemoryPatterns() {
    pImpl->patternLibrary.clear();
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        MemoryPattern pattern;
        pattern.patternData.resize(memoryNeurons_.size(), 0.0f);
        pattern.patternIndices.resize(memoryNeurons_.size(), i);
        pattern.patternStrength = memoryActivations_[i];
        pattern.lastReplay = pImpl->replayCounter;
        
        // Fill with current activation pattern
        for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
            if (j == i) {
                pattern.patternData[j] = memoryActivations_[j];
            }
        }
        
        pImpl->patternLibrary.push_back(pattern);
    }
}

// Memory sleep consolidation
void NeuralWorkingMemory::consolidateMemorySleep() {
    pImpl->consolidationTimer++;
    
    // Sleep consolidation happens periodically
    if (pImpl->consolidationTimer > 1000) {
        // Consolidate important memories
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (pImpl->memoryImportance.size() > i && pImpl->memoryImportance[i] > 0.6f) {
                // Strengthen during sleep
                memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * 1.02f);
                
                // Transfer to long-term storage (simulated)
                if (pImpl->memoryImportance[i] > 0.8f) {
                    // High importance - promote to stable storage
                    pImpl->memoryImportance[i] *= 1.05f;
                }
            }
        }
        
        // Decay unimportant memories
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (pImpl->memoryImportance.size() > i && pImpl->memoryImportance[i] < 0.3f) {
                // Weak memories decay more during sleep
                memoryActivations_[i] *= 0.95f;
            }
        }
        
        // Reset consolidation timer
        pImpl->consolidationTimer = 0;
        
        // Update temporal binding groups
        bindMemoryWithTemporalCoherence();
    }
}

// Memory interference modeling
void NeuralWorkingMemory::interferenceModeling() {
    pImpl->interferenceHistory.clear();
    pImpl->resolutionScore.clear();
    
    // Model interference between memory traces
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float interference = 0.0f;
        
        // Calculate interference from other traces
        for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
            if (i != j) {
                float similarity = 1.0f - std::abs(memoryActivations_[i] - memoryActivations_[j]);
                float timeDiff = std::abs(static_cast<float>(memoryTimestamps_[i] - memoryTimestamps_[j]));
                
                // Interference = similarity * decay_based_on_time
                float timeFactor = std::exp(-timeDiff / 200.0f);
                interference += similarity * timeFactor * 0.1f;
            }
        }
        
        pImpl->interferenceHistory.push_back(interference);
        
        // Resolution score = 1.0 - interference
        float resolutionScore = 1.0f - std::min(1.0f, interference);
        pImpl->resolutionScore.push_back(resolutionScore);
        
        // Apply interference resolution
        memoryActivations_[i] *= resolutionScore;
    }
}

// Memory interference resolution
void NeuralWorkingMemory::resolveMemoryInterference() {
    // Resolve high interference conflicts
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (pImpl->interferenceHistory.size() > i && pImpl->interferenceHistory[i] > 0.7f) {
            // High interference - need resolution
            if (pImpl->memoryImportance.size() > i && pImpl->memoryImportance[i] > 0.6f) {
                // Important memory wins
                memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * 1.1f);
                pImpl->resolutionScore[i] = std::min(1.0f, pImpl->resolutionScore[i] * 1.05f);
            } else {
                // Less important memory suppressed
                memoryActivations_[i] *= 0.5f;
                pImpl->resolutionScore[i] = std::max(0.0f, pImpl->resolutionScore[i] * 0.9f);
            }
        }
    }
}

// Enhanced neuromodulation integration
void NeuralWorkingMemory::enhancedNeuromodulation(float neuromodulationLevel) {
    connectToNeuromodulation(neuromodulationLevel);
    
    // Apply advanced neuromodulation effects
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        // Phase-dependent neuromodulation
        float thetaPhaseFactor = sin(thetaPhase_);
        float gammaPhaseFactor = sin(gammaPhase_);
        
        // Neuromodulation affects plasticity
        if (neuromodulationLevel > 0.5f) {
            // High neuromodulation - enhanced plasticity
            memoryActivations_[i] = std::min(1.0f, activation * (1.0f + neuromodulationLevel * thetaPhaseFactor * 0.3f));
            
            // Store phase information in context
            if (memoryContexts_.size() > i) {
                memoryContexts_[i].push_back(thetaPhaseFactor * 0.1f);
                memoryContexts_[i].push_back(gammaPhaseFactor * 0.1f);
            }
        } else {
            // Low neuromodulation - selective stabilization
            memoryActivations_[i] *= (1.0f - neuromodulationLevel * 0.2f);
        }
        
        // Apply oscillatory modulation to neuromodulation
        float oscillation = thetaPhaseFactor * 0.3f + gammaPhaseFactor * 0.7f;
        memoryActivations_[i] *= (1.0f + oscillation * 0.05f);
    }
}

// Update with advanced features
void NeuralWorkingMemory::updateWithAdvancedFeatures(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update memory prioritization
    prioritizeMemory();
    
    // Update oscillatory patterns
    updateOscillatoryPatterns();
    
    // Apply attention gating
    applyAttentionGating(std::vector<NeuronId>());  // Will be updated by integrateWithAttention
    
    // Memory compression
    compressMemory();
    
    // Memory replay strengthening
    replayStrengthenMemories();
    
    // Memory consolidation
    consolidateMemorySleep();
    
    // Memory interference modeling
    interferenceModeling();
    
    // Interference resolution
    resolveMemoryInterference();
    
    // Update recurrent connections
    updateRecurrentConnections();
    
    // Update regular mechanisms
    update(dt);
}

// Oscillatory patterns (theta/gamma coupling)
void NeuralWorkingMemory::updateOscillatoryPatterns() {
    if (!brain_) return;
    
    // Update theta phase
    thetaPhase_ += 0.01f;
    if (thetaPhase_ > 2.0f * M_PI) thetaPhase_ -= 2.0f * M_PI;
    
    // Update gamma phase with theta coupling
    float thetaModulation = (1.0f + thetaAmplitude_ * 0.5f) * sin(thetaPhase_);
    gammaPhase_ = fmod(gammaPhase_ + 0.05f * thetaModulation + thetaModulation * 0.5f, 2.0f * M_PI);
    
    // Apply oscillatory modulation to memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float theta = sin(thetaPhase_);
        float gamma = sin(gammaPhase_);
        
        // Combined modulation
        float oscillation = (theta * 0.3f + gamma * 0.7f) * thetaModulation;
        
        // Apply to neuron activation
        memoryActivations_[i] *= (1.0f + oscillation * 0.1f);
        memoryActivations_[i] = std::min(1.0f, memoryActivations_[i]);
    }
}

// Memory binding with temporal coherence
void NeuralWorkingMemory::bindMemoryWithTemporalCoherence() {
    if (!brain_ || memoryNeurons_.size() < 2) return;
    
    // Find temporally related memories
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        for (size_t j = i + 1; j < memoryNeurons_.size(); ++j) {
            // Check temporal proximity
            float timeDiff = std::abs(static_cast<float>(memoryTimestamps_[i] - memoryTimestamps_[j]));
            
            if (timeDiff < 100.0f) {  // Within 100 timesteps
                // Create temporal binding
                float coherence = 1.0f - (timeDiff / 100.0f);
                
                // Strengthen both memories
                memoryActivations_[i] *= (1.0f + coherence * 0.2f);
                memoryActivations_[j] *= (1.0f + coherence * 0.2f);
                
                // Add binding group
                pImpl->temporalBindingGroups[i].push_back(j);
                pImpl->temporalBindingGroups[j].push_back(i);
            }
        }
    }
}

// Context-dependent memory retrieval
void NeuralWorkingMemory::contextDependentRetrieval() {
    if (!brain_ || memoryContexts_.empty()) return;
    
    // Find current context
    SimulationStep currentStep = memoryReplayCounter_;
    
    for (size_t i = 0; i < memoryContexts_.size(); ++i) {
        float contextMatch = 0.0f;
        
        // Compare with stored contexts
        for (float tag : memoryContexts_[i]) {
            // Simple context matching - in real implementation would be more sophisticated
            contextMatch += tag * 0.1f;
        }
        
        // Modulate retrieval based on context match
        memoryActivations_[i] *= (1.0f + contextMatch * 0.5f);
    }
}

// Pattern completion capabilities
void NeuralWorkingMemory::patternCompletion() {
    if (memoryPatternCache_.empty()) return;
    
    // For each neuron, check pattern cache
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float completionBoost = 0.0f;
        
        // Check all cached patterns for potential completion
        for (size_t p = 0; p < memoryPatternCache_.size(); ++p) {
            if (memoryPatternCache_[p].size() > i) {
                float patternValue = memoryPatternCache_[p][i];
                if (patternValue > 0.5f) {  // Strong pattern match
                    completionBoost += (patternValue - 0.5f) * 0.3f;
                }
            }
        }
        
        // Apply pattern completion boost
        memoryActivations_[i] *= (1.0f + completionBoost);
        memoryActivations_[i] = std::min(1.0f, memoryActivations_[i]);
    }
}

// Attention-based memory gating
void NeuralWorkingMemory::applyAttentionGating(const std::vector<NeuronId>& attendedNeurons) {
    if (!brain_) return;
    
    // Update gating state
    pImpl->attentionGating.clear();
    pImpl->attentionGating.resize(memoryNeurons_.size(), false);
    
    // Gating for attended neurons
    for (const auto& attendedNeuron : attendedNeurons) {
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (memoryNeurons_[i] == attendedNeuron) {
                pImpl->attentionGating[i] = true;
                // Boost attended neurons
                memoryActivations_[i] *= 1.5f;
                break;
            }
        }
    }
}

// Focused memory encoding/decoding
void NeuralWorkingMemory::encodeFocusedMemory(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Create compressed encoding
    std::vector<float> compressedPattern;
    for (float val : pattern) {
        // Apply attention-based encoding
        float encoded = val * strength;
        encoded = std::min(1.0f, encoded * (1.0f + thetaAmplitude_ * 0.1f));
        compressedPattern.push_back(encoded);
    }
    
    // Store in pattern cache for future decoding
    if (memoryPatternCache_.size() < capacity_) {
        memoryPatternCache_.push_back(compressedPattern);
    }
    
    // Encode to neurons with focus
    for (size_t i = 0; i < compressedPattern.size(); ++i) {
        if (i < memoryNeurons_.size()) {
            NeuronId neuron = memoryNeurons_[i];
            float activation = compressedPattern[i];
            
            // Enhanced encoding with neuromodulation
            if (gammaAmplitude_ > 0.5f) {
                activation *= (1.0f + gammaAmplitude_ * 0.2f);
            }
            
            // Inject focused current
            brain_->injectCurrent(neuron, activation * 5.0f);
        }
    }
}

void NeuralWorkingMemory::decodeFocusedMemory() {
    std::vector<float> decodedPattern;
    
    // Decode from active neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        // Apply oscillatory decoding
        float theta = sin(thetaPhase_);
        float gamma = sin(gammaPhase_);
        
        // Phase-dependent decoding
        float phaseFactor = (theta * 0.3f + gamma * 0.7f);
        activation *= (1.0f + phaseFactor * 0.1f);
        
        decodedPattern.push_back(activation);
    }
    
    // For now, decodedPattern is available locally
    // In real implementation would return via callback or be stored
}