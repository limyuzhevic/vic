#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Enhanced maintenance connections with weights
    std::vector<std::tuple<NeuronId, NeuronId, float>> maintenanceConnections;
    
    // Memory trace metadata
    struct TraceMetadata {
        SimulationStep age;
        float lastUpdateTime;
        float strength;
        float decayCounter;
        bool isConsolidated;
    };
    
    std::vector<TraceMetadata> traceMetadata;
    
    // Competition and attention tracking
    std::unordered_map<NeuronId, float> competitionScores;
    std::vector<NeuronId> currentWinners;
    float globalInhibition;
    float attentionModulation;
    
    // Pattern recognition
    std::unordered_map<std::string, std::vector<NeuronId>> patternMap;
    float patternSimilarityThreshold;
    
    Impl() : brain(nullptr), globalInhibition(1.0f), attentionModulation(1.0f), 
             patternSimilarityThreshold(0.7f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , consolidationThreshold_(0.5f)
    , competitionEnabled_(true)
    , patternRecognitionEnabled_(true)
{
    // Initialize memory with better capacity management
    memoryNeurons_.reserve(capacity_ * 2);  // Allow temporary expansion
    memoryActivations_.reserve(capacity_ * 2);
    memoryTimestamps_.reserve(capacity_ * 2);
    traceMetadata.reserve(capacity_ * 2);
    pImpl->maintenanceConnections.reserve(capacity_);
    pImpl->patternMap.reserve(100);
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize enhanced memory systems
    NLM_LOG_INFO("NeuralWorkingMemory initialized with enhanced features");
    NLM_LOG_INFO("  Capacity: " + std::to_string(capacity_));
    NLM_LOG_INFO("  Pattern recognition: " + std::to_string(patternRecognitionEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Competition: " + std::to_string(competitionEnabled_ ? "enabled" : "disabled"));
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Pattern recognition and compression
    std::string patternKey;
    if (patternRecognitionEnabled_) {
        patternKey = createPatternKey(pattern);
        auto it = pImpl->patternMap.find(patternKey);
        if (it != pImpl->patternMap.end()) {
            // Use existing pattern - compress representation
            for (size_t i = 0; i < it->second.size() && i < pattern.size(); ++i) {
                NeuronId neuron = it->second[i];
                float activation = pattern[i] * strength;
                updateMemoryTrace(neuron, activation);
            }
            return;
        }
    }
    
    // Find or add neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation and update trace metadata
        if (brain_) {
            brain_->injectCurrent(neuron, activation * 5.0f);
        }
        updateMemoryTrace(neuron, activation);
    }
    
    // Create compressed maintenance connections for efficiency
    createCompressedConnections(strength * 0.5f);
    
    // Pattern registration for future retrieval
    if (patternRecognitionEnabled_ && !patternKey.empty()) {
        registerPattern(patternKey, pattern);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        traceMetadata[idx].lastUpdateTime = traceMetadata[idx].age;
        traceMetadata[idx].strength = activation;
        
        // Boost strength with reinforcement
        if (activation > 0.1f) {
            traceMetadata[idx].strength = std::min(1.0f, traceMetadata[idx].strength * 1.1f);
        }
    } else if (memoryNeurons_.size() < capacity_) {
        // Add new neuron with enhanced metadata
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        
        TraceMetadata meta;
        meta.age = 0;
        meta.lastUpdateTime = 0.0f;
        meta.strength = activation;
        meta.decayCounter = 0.0f;
        meta.isConsolidated = false;
        traceMetadata.push_back(meta);
        
        // Inject current to establish trace
        if (brain_) {
            brain_->injectCurrent(neuron, activation * 5.0f);
        }
    } else {
        // Replace weakest trace
        auto weakestIt = std::min_element(traceMetadata.begin(), traceMetadata.end(),
            [](const TraceMetadata& a, const TraceMetadata& b) {
                return a.strength < b.strength;
            });
        size_t idx = std::distance(traceMetadata.begin(), weakestIt);
        
        memoryNeurons_[idx] = neuron;
        memoryActivations_[idx] = activation;
        traceMetadata[idx].strength = activation;
        traceMetadata[idx].age = 0;
        traceMetadata[idx].lastUpdateTime = 0.0f;
        traceMetadata[idx].decayCounter = 0.0f;
        traceMetadata[idx].isConsolidated = false;
        
        if (brain_) {
            brain_->injectCurrent(neuron, activation * 5.0f);
        }
    }
}

void NeuralWorkingMemory::updateMemoryTrace(NeuronId neuron, float activation) {
    // Update existing memory trace or create new one
    auto neuronIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (neuronIt != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), neuronIt);
        
        // Enhanced activation update with temporal dynamics
        float oldActivation = memoryActivations_[idx];
        memoryActivations_[idx] = activation;
        traceMetadata[idx].lastUpdateTime = traceMetadata[idx].age;
        traceMetadata[idx].strength = activation;
        
        // Apply neural adaptation based on rate of change
        float activationChange = std::abs(activation - oldActivation);
        if (activationChange > 0.5f) {
            traceMetadata[idx].decayCounter += activationChange * 0.1f;
        }
        
        // Decay based on age and recent change
        traceMetadata[idx].decayCounter *= 0.95f;
        if (traceMetadata[idx].decayCounter > 0.8f) {
            traceMetadata[idx].strength *= 0.9f;
        }
    } else {
        // Add new trace with enhanced metadata
        if (memoryNeurons_.size() < capacity_) {
            memoryNeurons_.push_back(neuron);
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            
            TraceMetadata meta;
            meta.age = 0;
            meta.lastUpdateTime = 0.0f;
            meta.strength = activation;
            meta.decayCounter = 0.0f;
            meta.isConsolidated = false;
            traceMetadata.push_back(meta);
            
            if (brain_) {
                brain_->injectCurrent(neuron, activation * 5.0f);
            }
        }
    }
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update all memory traces with enhanced dynamics
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.01f) {
            // Maintain activation with adaptive gain
            float maintenanceGain = 1.0f + traceMetadata[i].strength * 0.5f;
            brain_->injectCurrent(neuron, activation * 5.0f * maintenanceGain);
            
            // Update trace age and metadata
            traceMetadata[i].age++;
            traceMetadata[i].lastUpdateTime += dt;
            
            // Enhanced decay based on age and trace properties
            float ageDecay = 1.0f / (1.0f + traceMetadata[i].age * 0.01f);
            float strengthDecay = 1.0f - std::min(0.5f, traceMetadata[i].decayCounter);
            
            memoryActivations_[i] = activation * ageDecay * strengthDecay;
            
            // Consolidation check
            if (!traceMetadata[i].isConsolidated && traceMetadata[i].strength > consolidationThreshold_) {
                consolidateTrace(i);
            }
        }
    }
    
    // Enhanced competition phase
    if (competitionEnabled_) {
        runEnhancedCompetition();
    }
    
    // Pattern consolidation and cleanup
    consolidateWeakPatterns();
    updateRecurrentConnections();
}

void NeuralWorkingMemory::createCompressedConnections(float strength) {
    // Create sparse, weighted connections between active neurons
    std::vector<NeuronId> activeNeurons;
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] > 0.1f) {
            activeNeurons.push_back(memoryNeurons_[i]);
        }
    }
    
    // Create connections between top activated neurons
    std::sort(activeNeurons.begin(), activeNeurons.end(),
        [this](const NeuronId& a, const NeuronId& b) {
            return getNeuronActivation(a) > getNeuronActivation(b);
        });
    
    // Limit number of connections for efficiency
    size_t maxConnections = std::min(size_t(5), activeNeurons.size() / 2);
    
    for (size_t i = 0; i < maxConnections; ++i) {
        for (size_t j = i + 1; j < std::min(i + 3, activeNeurons.size()); ++j) {
            float connStrength = strength * std::pow(0.8f, j - i - 1);
            pImpl->maintenanceConnections.emplace_back(
                activeNeurons[i], activeNeurons[j], connStrength);
        }
    }
}

void NeuralWorkingMemory::consolidateTrace(size_t traceIndex) {
    if (traceIndex >= traceMetadata.size()) return;
    
    TraceMetadata& meta = traceMetadata[traceIndex];
    meta.isConsolidated = true;
    
    // Apply consolidation boost
    memoryActivations_[traceIndex] *= 1.2f;
    
    // Schedule replay for consolidated traces
    if (!meta.isConsolidated) {
        meta.isConsolidated = true;
        // In a full implementation, this would schedule replay
    }
}

void NeuralWorkingMemory::runEnhancedCompetition() {
    winners_.clear();
    pImpl->competitionScores.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Normalize activations to [0, 1] for competition
    float maxActivation = *std::max_element(memoryActivations_.begin(), memoryActivations_.end());
    float minActivation = *std::min_element(memoryActivations_.begin(), memoryActivations_.end());
    float range = maxActivation - minActivation;
    
    if (range < 0.001f) {
        // All activations similar - random selection
        for (size_t i = 0; i < memoryNeurons_.size() && i < 5; ++i) {
            winners_.push_back(memoryNeurons_[i]);
            pImpl->competitionScores[memoryNeurons_[i]] = 1.0f;
        }
        return;
    }
    
    // Compute competition scores with attention modulation
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float normalized = (memoryActivations_[i] - minActivation) / range;
        
        // Apply attention modulation
        float attentionWeight = std::min(1.0f, pImpl->attentionModulation * normalized * 2.0f);
        float competitionScore = normalized * attentionWeight * (1.0f + traceMetadata[i].strength * 0.5f);
        
        pImpl->competitionScores[memoryNeurons_[i]] = competitionScore;
        
        if (competitionScore > 0.5f) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Apply global inhibition
    applyEnhancedGlobalInhibition();
}

void NeuralWorkingMemory::applyEnhancedGlobalInhibition() {
    float totalInhibition = 0.0f;
    for (const auto& score : pImpl->competitionScores) {
        totalInhibition += score.second;
    }
    
    if (winners_.empty()) return;
    
    float inhibitionPerWinner = pImpl->globalInhibition / winners_.size();
    
    for (const NeuronId& winner : winners_) {
        // Apply excitation to winners
        if (brain_) {
            brain_->injectCurrent(winner, inhibitionPerWinner * 3.0f);
        }
        
        // Apply strong inhibition to non-winners
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            NeuronId neuron = memoryNeurons_[i];
            if (neuron != winner) {
                // Compute inhibition strength based on winner's strength
                auto scoreIt = pImpl->competitionScores.find(winner);
                if (scoreIt != pImpl->competitionScores.end()) {
                    float inhibition = inhibitionPerWinner * scoreIt->second;
                    brain_->injectCurrent(neuron, -inhibition * 2.0f);
                }
            }
        }
    }
}

void NeuralWorkingMemory::consolidateWeakPatterns() {
    // Remove patterns below threshold
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < traceMetadata.size(); ++i) {
        if (!traceMetadata[i].isConsolidated && 
            memoryActivations_[i] < 0.01f && 
            traceMetadata[i].age > 100) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        traceMetadata.erase(traceMetadata.begin() + idx);
    }
    
    // Update pattern map
    if (patternRecognitionEnabled_) {
        updatePatternMap();
    }
}

void NeuralWorkingMemory::updatePatternMap() {
    pImpl->patternMap.clear();
    
    // Group similar traces into patterns
    std::vector<std::vector<size_t>> patternGroups;
    
    for (size_t i = 0; i < traceMetadata.size(); ++i) {
        bool assigned = false;
        for (auto& group : patternGroups) {
            if (computePatternSimilarity(i, group[0]) > patternSimilarityThreshold) {
                group.push_back(i);
                assigned = true;
                break;
            }
        }
        
        if (!assigned) {
            std::vector<size_t> newGroup{i};
            patternGroups.push_back(newGroup);
        }
    }
    
    // Create pattern keys
    for (size_t i = 0; i < patternGroups.size(); ++i) {
        std::string patternKey = "pattern_" + std::to_string(i);
        pImpl->patternMap[patternKey] = memoryNeurons_[patternGroups[i][0]];
    }
}

void NeuralWorkingMemory::registerPattern(const std::string& patternKey, const std::vector<float>& pattern) {
    // Extract representative neurons for this pattern
    std::vector<NeuronId> patternNeurons;
    size_t neuronsToSample = std::min(pattern.size(), memoryNeurons_.size() / 10);
    
    for (size_t i = 0; i < neuronsToSample; ++i) {
        patternNeurons.push_back(memoryNeurons_[i]);
    }
    
    pImpl->patternMap[patternKey] = patternNeurons;
}

std::string NeuralWorkingMemory::createPatternKey(const std::vector<float>& pattern) {
    // Create a simple hash key for pattern (in real implementation would use more robust hashing)
    std::string key = "pattern_";
    for (size_t i = 0; i < std::min(pattern.size(), size_t(4)); ++i) {
        key += std::to_string(static_cast<int>(pattern[i] * 100)) + "_";
    }
    return key;
}

float NeuralWorkingMemory::computePatternSimilarity(size_t traceIndex1, size_t traceIndex2) const {
    if (traceIndex1 >= memoryActivations_.size() || traceIndex2 >= memoryActivations_.size()) {
        return 0.0f;
    }
    
    float act1 = memoryActivations_[traceIndex1];
    float act2 = memoryActivations_[traceIndex2];
    
    return 1.0f - std::abs(act1 - act2);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Update maintenance connections based on current activity
    for (auto& [from, to, strength] : pImpl->maintenanceConnections) {
        float fromActivation = getNeuronActivation(from);
        float toActivation = getNeuronActivation(to);
        
        if (fromActivation > 0.1f) {
            // Update connection strength
            strength = std::min(1.0f, strength * 1.1f);
            
            // Apply maintenance signal
            if (brain_) {
                brain_->injectCurrent(to, fromActivation * strength * 2.0f);
            }
        } else {
            // Decay unused connections
            strength *= 0.95f;
        }
        
        // Remove very weak connections
        if (strength < 0.01f) {
            strength = 0.0f;
        }
    }
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceConnections.clear();
    pImpl->competitionScores.clear();
    winners_.clear();
    pImpl->patternMap.clear();
    
    // Reset metadata
    traceMetadata.clear();
    traceMetadata.shrink_to_fit();
}

// NeuralAssociativeMemory Implementation

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized with enhanced features");
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Enhanced action-state associations
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern with richer encoding
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        actionPattern[1] = episode.reward;  // Include reward in action pattern
        actionPattern[2] = episode.success ? 1.0f : 0.0f;  // Include success in action pattern
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
    
    // Time-based associations
    if (episode.timestamp > 0) {
        std::vector<float> timePattern(1, episode.timestamp / 10000.0f);
        associate(timePattern, episode.sensoryState, 0.5f);
    }
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                 const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Enhanced similarity with multiple measures
    float cosineSimilarity = computeCosineSimilarity(a, b);
    float euclideanDistance = computeEuclideanDistance(a, b);
    float correlation = computeCorrelation(a, b);
    
    // Combine similarities
    float combinedSimilarity = cosineSimilarity * 0.5f + 
                               (1.0f - euclideanDistance / 10.0f) * 0.3f + 
                               correlation * 0.2f;
    
    return std::max(0.0f, std::min(1.0f, combinedSimilarity));
}

float NeuralAssociativeMemory::computeCosineSimilarity(const std::vector<float>& a,
                                                       const std::vector<float>& b) const {
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

float NeuralAssociativeMemory::computeEuclideanDistance(const std::vector<float>& a,
                                                       const std::vector<float>& b) const {
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

float NeuralAssociativeMemory::computeCorrelation(const std::vector<float>& a,
                                                  const std::vector<float>& b) const {
    if (a.size() != b.size() || a.size() < 2) return 0.0f;
    
    float meanA = std::accumulate(a.begin(), a.end(), 0.0f) / a.size();
    float meanB = std::accumulate(b.begin(), b.end(), 0.0f) / b.size();
    
    float numerator = 0.0f, denomA = 0.0f, denomB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diffA = a[i] - meanA;
        float diffB = b[i] - meanB;
        numerator += diffA * diffB;
        denomA += diffA * diffA;
        denomB += diffB * diffB;
    }
    
    if (denomA < 0.0001f || denomB < 0.0001f) return 0.0f;
    
    return numerator / (std::sqrt(denomA) * std::sqrt(denomB));
}

// Forward declarations for the remaining classes
void NeuralWorkingMemory::enablePatternRecognition(bool enable) {
    patternRecognitionEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Pattern recognition enabled");
    }
}

void NeuralWorkingMemory::enableCompetition(bool enable) {
    competitionEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Competition enabled");
    }
}

void NeuralWorkingMemory::setCapacity(size_t capacity) {
    capacity_ = capacity;
    // Trim memory if over capacity
    while (memoryNeurons_.size() > capacity_) {
        // Remove weakest trace
        auto weakestIt = std::min_element(traceMetadata.begin(), traceMetadata.end(),
            [](const TraceMetadata& a, const TraceMetadata& b) {
                return a.strength < b.strength;
            });
        size_t idx = std::distance(traceMetadata.begin(), weakestIt);
        
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        traceMetadata.erase(traceMetadata.begin() + idx);
    }
}

void NeuralWorkingMemory::setDecayRate(float rate) {
    decayRate_ = std::max(0.0f, std::min(1.0f, rate));
}

void NeuralWorkingMemory::setConsolidationThreshold(float threshold) {
    consolidationThreshold_ = std::max(0.0f, std::min(1.0f, threshold));
}

void NeuralWorkingMemory::setGlobalInhibition(float inhibition) {
    pImpl->globalInhibition = std::max(0.0f, inhibition);
}

void NeuralWorkingMemory::setAttentionModulation(float modulation) {
    pImpl->attentionModulation = std::max(0.0f, modulation);
}

void NeuralWorkingMemory::setPatternSimilarityThreshold(float threshold) {
    pImpl->patternSimilarityThreshold = std::max(0.0f, std::min(1.0f, threshold));
}

// Helper methods
float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

bool NeuralWorkingMemory::isMemoryConsolidated(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return traceMetadata[idx].isConsolidated;
    }
    return false;
}

void NeuralWorkingMemory::setMemoryConsolidation(NeuronId neuron, bool consolidated) {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        traceMetadata[idx].isConsolidated = consolidated;
    }
}

float NeuralWorkingMemory::getConsolidationProgress(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return traceMetadata[idx].strength / consolidationThreshold_;
    }
    return 0.0f;
}

void NeuralWorkingMemory::boostConsolidation(NeuronId neuron, float factor) {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        traceMetadata[idx].strength *= factor;
        traceMetadata[idx].isConsolidated = true;
    }
}

std::vector<NeuronId> NeuralWorkingMemory::getActiveTraces() const {
    std::vector<NeuronId> active;
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] > 0.01f) {
            active.push_back(memoryNeurons_[i]);
        }
    }
    return active;
}

std::vector<NeuronId> NeuralWorkingMemory::getConsolidatedTraces() const {
    std::vector<NeuronId> consolidated;
    for (size_t i = 0; i < traceMetadata.size(); ++i) {
        if (traceMetadata[i].isConsolidated) {
            consolidated.push_back(memoryNeurons_[i]);
        }
    }
    return consolidated;
}

float NeuralWorkingMemory::getMemoryUtilization() const {
    if (capacity_ == 0) return 0.0f;
    float utilization = static_cast<float>(memoryNeurons_.size()) / capacity_;
    return std::min(1.0f, utilization);
}

void NeuralWorkingMemory::printMemoryStats() const {
    NLM_LOG_INFO("=== NeuralWorkingMemory Statistics ===");
    NLM_LOG_INFO("Total traces: " + std::to_string(memoryNeurons_.size()));
    NLM_LOG_INFO("Active traces: " + std::to_string(getActiveTraces().size()));
    NLM_LOG_INFO("Consolidated traces: " + std::to_string(getConsolidatedTraces().size()));
    NLM_LOG_INFO("Memory utilization: " + std::to_string(getMemoryUtilization() * 100.0f) + "%");
    NLM_LOG_INFO("Global inhibition: " + std::to_string(pImpl->globalInhibition));
    NLM_LOG_INFO("Attention modulation: " + std::to_string(pImpl->attentionModulation));
    NLM_LOG_INFO("Pattern recognition: " + std::to_string(patternRecognitionEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("Competition: " + std::to_string(competitionEnabled_ ? "enabled" : "disabled"));
}

} // namespace nlm
