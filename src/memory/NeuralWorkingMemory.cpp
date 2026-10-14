#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Neural plasticity factors for working memory
    float plasticityFactor;
    float consolidationThreshold;
    
    Impl() : brain(nullptr), plasticityFactor(1.0f), consolidationThreshold(0.5f) {
        std::random_device rd;
        rng.seed(rd());
    }
    
    std::mt19937 rng;
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    // Initialize with random seed for biological realism
    std::random_device rd;
    pImpl->rng.seed(rd());
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Enable working memory systems
    if (brain_) {
        // Connect working memory to brain systems
        // In real implementation, this would establish actual neural connections
        NLM_LOG_INFO("NeuralWorkingMemory initialized with brain integration");
        
        // Set up initial memory traces
        memoryNeurons_.reserve(capacity_);
        memoryActivations_.reserve(capacity_);
        memoryTimestamps_.reserve(capacity_);
        activeTraces_.reserve(capacity_);
        
        // Initialize with some basic memory patterns
        createBasicMemoryPatterns();
    }
}

void NeuralWorkingMemory::createBasicMemoryPatterns() {
    // Create basic working memory patterns for common sensory inputs
    // This simulates biological preparation of working memory
    
    // Create patterns for: visual, auditory, internal signals
    std::vector<std::vector<float>> basicPatterns = {
        {0.8f, 0.6f, 0.4f, 0.2f},  // Visual pattern
        {0.3f, 0.7f, 0.5f},       // Auditory pattern  
        {0.6f, 0.4f, 0.8f, 0.1f}  // Internal pattern
    };
    
    for (const auto& pattern : basicPatterns) {
        store(pattern, 0.5f);
    }
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Calculate pattern novelty and similarity
    float novelty = calculateNovelty(pattern);
    float similarity = calculateSimilarity(pattern);
    
    // Store pattern if it's novel enough or similar to existing patterns
    if (novelty > 0.3f || similarity < 0.7f) {
        // Find neurons to encode this pattern
        size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
        
        if (neuronsNeeded > 0) {
            // Use Hebbian-like learning to update neural connections
            updateNeuralConnections(pattern, strength);
            
            // Store activations with timestamps
            for (size_t i = 0; i < neuronsNeeded; ++i) {
                NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
                float activation = pattern[i] * strength * (1.0f + novelty);
                
                storeToNeuron(neuron, activation);
                
                // Apply neuromodulation effects
                applyNeuromodulationToMemory(neuron, activation, novelty);
            }
        }
    }
    
    // Run competition if we have multiple memory traces
    if (memoryNeurons_.size() > 1) {
        runCompetition();
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        
        // Increment usage count
        activeTraces_[idx] = (activeTraces_[idx] + 1) % capacity_;
        
        // Update stability based on usage
        if (idx < activeTraces_.size()) {
            float stability = static_cast<float>(activeTraces_[idx]) / capacity_;
            if (stability > decayRate_) {
                // Stable memory trace - maintain it
                strengthenMemory(1.1f);
            }
        }
    } else if (memoryNeurons_.size() < capacity_) {
        // Add new memory trace
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        activeTraces_.push_back(1);
        
        // Create maintenance connections for new trace
        if (memoryNeurons_.size() > 1) {
            createRecurrentConnection(memoryNeurons_[memoryNeurons_.size() - 2], 
                                    memoryNeurons_[memoryNeurons_.size() - 1], 
                                    activation * 0.5f);
        }
    }
    
    // Inject current to maintain activation with biological realism
    if (brain_) {
        // Add neuromodulation to current injection
        float neuromodulatedActivation = activation * pImpl->plasticityFactor;
        brain_->injectCurrent(neuron, neuromodulatedActivation * 5.0f);
    }
}

void NeuralWorkingMemory::updateNeuralConnections(const std::vector<float>& pattern, 
                                                  float strength) {
    // Update neural connections using Hebbian learning
    if (pattern.empty() || !brain_) return;
    
    // Calculate synaptic weight changes
    float learningRate = pImpl->plasticityFactor * 0.1f;
    
    // Create new connections or strengthen existing ones
    for (size_t i = 0; i < std::min(pattern.size(), memoryNeurons_.size()); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        
        // Find target neurons for connections
        for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
            if (i != j) {
                // Hebbian-like connection strengthening
                float weight = strength * learningRate * 
                              std::exp(-std::abs(static_cast<float>(i) - j) / 5.0f);
                
                createRecurrentConnection(memoryNeurons_[j], neuron, weight);
            }
        }
    }
}

void NeuralWorkingMemory::applyNeuromodulationToMemory(NeuronId neuron, float activation, float novelty) {
    if (!brain_) return;
    
    // Apply different neuromodulation effects to working memory
    if (auto* dopamine = brain_->getDopamine()) {
        // Dopamine enhances memory consolidation for rewarding/novel events
        if (novelty > 0.5f) {
            float consolidation = dopamine->getLevel() * 0.3f;
            // In real implementation, this would affect memory consolidation timing
        }
    }
    
    if (auto* acetylcholine = brain_->getAcetylcholine()) {
        // Acetylcholine enhances attention and encoding of novel stimuli
        if (novelty > 0.3f) {
            float attentionGain = acetylcholine->getAttentionModulation();
            // In real implementation, this would increase encoding efficiency
        }
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        // Apply decay based on age
        float ageFactor = 1.0f;
        if (i < memoryTimestamps_.size()) {
            ageFactor = std::exp(-decayRate_ * memoryTimestamps_[i]);
        }
        
        float activation = memoryActivations_[i] * ageFactor;
        result.push_back(activation);
    }
    
    return result;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update memory traces with biological dynamics
    
    // 1. Decay weak traces
    decayWeakTraces();
    
    // 2. Apply neuromodulation effects
    applyMemoryNeuromodulation(dt);
    
    // 3. Update maintenance connections
    updateRecurrentConnections();
    
    // 4. Integrate with episodic memory
    integrateWithEpisodicMemory();
    
    // 5. Update concept formation integration
    integrateWithConceptFormation();
    
    // Age all traces
    for (auto& timestamp : memoryTimestamps_) {
        timestamp += dt;
    }
    
    // Remove outdated traces if over capacity
    if (memoryNeurons_.size() > capacity_) {
        removeOldestTrace();
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    // Decay traces with low activation
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] < decayRate_ && 
            i < memoryTimestamps_.size() && memoryTimestamps_[i] > 100.0f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove neuron
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        if (idx < activeTraces_.size()) {
            activeTraces_.erase(activeTraces_.begin() + idx);
        }
        
        // Remove maintenance connections
        removeMaintenanceConnections(idx);
    }
}

void NeuralWorkingMemory::applyMemoryNeuromodulation(TimestepDuration dt) {
    if (!brain_) return;
    
    // Apply neuromodulatory effects to working memory dynamics
    
    float totalModulation = 1.0f;
    
    if (auto* dopamine = brain_->getDopamine()) {
        totalModulation *= dopamine->getLevel();
        
        // Dopamine enhances memory stability
        if (dopamine->getLevel() > 0.5f) {
            // Stabilize active traces
            for (auto& activation : memoryActivations_) {
                activation *= 1.1f;  // Enhance stability
            }
        }
    }
    
    if (auto* acetylcholine = brain_->getAcetylcholine()) {
        totalModulation *= acetylcholine->getAttentionModulation();
        
        // ACh enhances encoding of novel information
        if (acetylcholine->getAttentionModulation() > 1.2f) {
            // Boost weak traces
            for (auto& activation : memoryActivations_) {
                activation *= 1.2f;
            }
        }
    }
    
    if (auto* norepinephrine = brain_->getNorepinephrine()) {
        totalModulation *= norepinephrine->getArousalModulation();
        
        // NE modulates memory updating
        if (norepinephrine->getArousalModulation() > 1.2f) {
            // Increase memory turnover
            for (size_t i = 0; i < memoryActivations_.size(); ++i) {
                memoryActivations_[i] *= 0.9f;
            }
        }
    }
    
    if (auto* serotonin = brain_->getSerotonin()) {
        totalModulation *= serotonin->getBehavioralModulation();
        
        // 5-HT modulates impulsive vs. reflective processing
        if (serotonin->getBehavioralModulation() > 1.2f) {
            // Enhance reflective memory processing
            for (auto& activation : memoryActivations_) {
                activation *= 1.1f;
            }
        }
    }
    
    // Update plasticity factor based on neuromodulation
    pImpl->plasticityFactor = totalModulation;
}

void NeuralWorkingMemory::integrateWithEpisodicMemory() {
    if (!brain_) return;
    
    // Integrate working memory with episodic memory system
    
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        // Check if working memory traces match episodic patterns
        auto currentWorkingMemory = retrieve();
        
        // Find similar episodic episodes
        auto similarEpisodes = episodicMemory->retrieveSimilar(currentWorkingMemory, 5);
        
        // Update episodic memory based on working memory
        for (const auto* episode : similarEpisodes) {
            // Enhance episode stability if working memory traces are strong
            for (float activation : currentWorkingMemory) {
                if (activation > 0.6f) {
                    // In real implementation, this would enhance episode encoding
                    break;
                }
            }
        }
    }
    
    // Store working memory to episodic memory periodically
    if (memoryTimestamps_.size() > 0 && memoryTimestamps_.back() > 1000.0f) {
        // Create new episodic memory from working memory contents
        createEpisodicFromWorkingMemory();
    }
}

void NeuralWorkingMemory::integrateWithConceptFormation() {
    if (!brain_) return;
    
    // Integrate with concept formation for pattern abstraction
    
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        auto currentWorkingMemory = retrieve();
        
        // Present working memory pattern to concept formation
        size_t conceptId = conceptFormation->presentExperience(
            currentWorkingMemory,
            std::vector<float>(5, 0.5f),  // Simplified features
            0.5f,  // Medium reward
            0  // Step 0
        );
        
        if (conceptId > 0) {
            // Concept formation accepted this pattern
            // In real implementation, this would enhance working memory connections
        }
    }
}

void NeuralWorkingMemory::runCompetition() {
    if (memoryNeurons_.size() <= 1) return;
    
    // Neural competition for memory dominance
    // Based on activation levels and maintenance connection strength
    
    std::vector<std::pair<float, size_t>> scores;  // (score, index)
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float score = 0.0f;
        
        // Base score on activation
        if (i < memoryActivations_.size()) {
            score += memoryActivations_[i];
        }
        
        // Boost score based on age (older traces compete more)
        if (i < memoryTimestamps_.size()) {
            score += std::min(1.0f, memoryTimestamps_[i] * 0.001f);
        }
        
        // Reduce score based on novelty (novel traces compete more)
        score *= (1.0f + calculateNovelty(memoryActivations_[i]) * 0.5f);
        
        scores.emplace_back(score, i);
    }
    
    // Sort by score (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Keep top traces, remove bottom traces
    std::vector<size_t> toRemove;
    for (size_t i = 1; i < scores.size() && i < capacity_ / 2; ++i) {
        toRemove.push_back(scores[i].second);
    }
    
    // Remove low-performing traces
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove neuron and associated data
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        if (idx < activeTraces_.size()) {
            activeTraces_.erase(activeTraces_.begin() + idx);
        }
        
        // Remove maintenance connections
        removeMaintenanceConnections(idx);
    }
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Create a recurrent connection for memory maintenance
    std::pair<NeuronId, NeuronId> conn(from, to);
    
    // Check if connection already exists
    for (const auto& existing : pImpl->maintenanceSynapses) {
        if (existing.first == from && existing.second == to) {
            return;  // Connection already exists
        }
    }
    
    // Add new connection
    pImpl->maintenanceSynapses.push_back(conn);
}

void NeuralWorkingMemory::removeMaintenanceConnections(size_t idx) {
    // Remove maintenance connections involving a neuron
    auto neuron = memoryNeurons_[idx];
    
    pImpl->maintenanceSynapses.erase(
        std::remove_if(pImpl->maintenanceSynapses.begin(),
                      pImpl->maintenanceSynapses.end(),
                      [neuron](const std::pair<NeuronId, NeuronId>& conn) {
                          return conn.first == neuron || conn.second == neuron;
                      }),
        pImpl->maintenanceSynapses.end()
    );
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Update strength of maintenance connections based on usage
    for (auto& conn : pImpl->maintenanceSynapses) {
        // Decay connection strength over time
        float newStrength = conn.second * 0.99f;
        
        // Boost strength if neurons are active
        if (std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first) != memoryNeurons_.end() &&
            std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.second) != memoryNeurons_.end()) {
            newStrength = std::min(1.0f, newStrength * 1.05f);
        }
        
        // In real implementation, update actual neural connections
    }
}

void NeuralWorkingMemory::removeOldestTrace() {
    if (memoryNeurons_.empty()) return;
    
    // Find oldest trace
    size_t oldestIdx = 0;
    uint64_t oldestTime = memoryTimestamps_[0];
    
    for (size_t i = 1; i < memoryTimestamps_.size(); ++i) {
        if (memoryTimestamps_[i] > oldestTime) {
            oldestTime = memoryTimestamps_[i];
            oldestIdx = i;
        }
    }
    
    // Remove oldest trace
    memoryNeurons_.erase(memoryNeurons_.begin() + oldestIdx);
    memoryActivations_.erase(memoryActivations_.begin() + oldestIdx);
    memoryTimestamps_.erase(memoryTimestamps_.begin() + oldestIdx);
    if (oldestIdx < activeTraces_.size()) {
        activeTraces_.erase(activeTraces_.begin() + oldestIdx);
    }
    
    // Remove associated maintenance connections
    removeMaintenanceConnections(oldestIdx);
}

void NeuralWorkingMemory::createEpisodicFromWorkingMemory() {
    // Create an episodic memory from current working memory contents
    if (!brain_ || memoryNeurons_.empty()) return;
    
    // Collect working memory pattern
    std::vector<float> pattern = retrieve();
    
    // Create episodic memory item
    EpisodicMemoryItem episode;
    episode.timestamp = 0;  // Would use current step
    episode.sensoryState = pattern;
    episode.reward = 0.5f;  // Medium reward
    episode.location = std::make_pair(0.0f, 0.0f);  // Default location
    
    // Store in episodic memory
    if (auto* episodicMemory = brain_->getEpisodicMemory()) {
        episodicMemory->storeEpisode(episode);
    }
}

float NeuralWorkingMemory::calculateNovelty(const std::vector<float>& pattern) const {
    if (pattern.empty()) return 0.0f;
    
    // Calculate pattern novelty based on deviation from mean
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    
    // Novelty increases with variance
    return std::min(1.0f, std::sqrt(variance) * 2.0f);
}

float NeuralWorkingMemory::calculateSimilarity(const std::vector<float>& pattern) const {
    if (pattern.empty()) return 1.0f;
    
    // Simple similarity calculation
    // In real implementation, this would compare with stored patterns
    return 0.5f;  // Placeholder
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    // Strengthen memory traces (Hebbian-like)
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    
    // Clear maintenance synapses
    pImpl->maintenanceSynapses.clear();
    
    // Reset parameters
    pImpl->plasticityFactor = 1.0f;
    pImpl->consolidationThreshold = 0.5f;
}

} // namespace nlm