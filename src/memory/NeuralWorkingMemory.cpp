// NeuralWorkingMemory.cpp - Enhanced Neural Working Memory Implementation

#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <immintrin.h>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    // Core working memory implementation
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory management
    std::vector<size_t> activeTraces_;
    std::vector<float> traceImportance_;
    
    // Neural connectivity for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    std::vector<float> connectionStrengths_;
    
    // Competition system
    std::vector<NeuronId> winners_;
    std::vector<float> competitionScores_;
    
    // Enhanced memory systems
    std::vector<std::vector<float>> indexedPatterns_;
    std::vector<MemoryIndexEntry> index_;
    std::unordered_map<size_t, std::vector<float>> cache_;
    std::unordered_set<size_t> highPriorityCache_;
    
    // Performance tracking
    std::atomic<size_t> accessCount_;
    std::atomic<double> totalAccessTime_;
    std::atomic<size_t> compressionCount_;
    std::atomic<float> averageImportance_;
    
    // Configuration
    MemoryConfig config_;
    
    // Brain reference
    Brain* brain_;
    
    Impl() : accessCount_(0), totalAccessTime_(0.0), compressionCount_(0), 
             averageImportance_(1.0f), brain_(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("NeuralWorkingMemory: Created neural working memory system");
}

NeuralWorkingMemory::~NeuralWorkingMemory() {
    NLM_LOG_INFO("NeuralWorkingMemory: Destroyed neural working memory system");
}

void NeuralWorkingMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("NeuralWorkingMemory: Cannot initialize with null brain pointer");
        return;
    }
    
    brain_ = brain;
    
    // Initialize configuration
    config_.capacity = 100;
    config_.decayRate = 0.95f;
    config_.enableCompression = true;
    config_.enableCaching = true;
    config_.enableReconstruction = true;
    config_.indexSize = 256;
    config_.cacheSize = 64;
    
    // Initialize neural connectivity
    setupNeuralConnectivity();
    
    NLM_LOG_INFO("NeuralWorkingMemory: Initialized with brain reference");
}

void NeuralWorkingMemory::setupNeuralConnectivity() {
    // Create recurrent connections for maintenance
    size_t numNeurons = brain_->getTotalNeuronCount();
    size_t maintenanceConnections = std::min(numNeurons / 10, size_t(50));
    
    for (size_t i = 0; i < maintenanceConnections; ++i) {
        // Create self-reinforcing connections
        NeuronId source = static_cast<NeuronId>(i % numNeurons);
        NeuronId target = source;  // Self-connections for maintenance
        createRecurrentConnection(source, target, 0.5f);
    }
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (pattern.empty()) {
        return;
    }
    
    // Check capacity
    if (pImpl->memoryNeurons_.size() >= config_.capacity) {
        // Remove weakest trace to make room
        decayWeakTraces();
    }
    
    // Encode pattern as neural activity
    size_t numNeurons = brain_->getTotalNeuronCount();
    size_t patternSize = pattern.size();
    
    for (size_t i = 0; i < patternSize; ++i) {
        float activation = pattern[i] * strength;
        NeuronId neuronId = static_cast<NeuronId>(i % numNeurons);
        
        // Store in working memory
        storeToNeuron(neuronId, activation);
    }
    
    // Index the memory
    if (config_.enableIndexing) {
        indexMemory(pattern, pImpl->brain_->getCurrentStep(), strength);
    }
    
    // Update statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    double accessTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    pImpl->accessCount_.fetch_add(1, std::memory_order_relaxed);
    pImpl->totalAccessTime_.fetch_add(accessTime, std::memory_order_relaxed);
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find existing neuron in memory
    auto it = std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), neuron);
    
    if (it != pImpl->memoryNeurons_.end()) {
        size_t index = std::distance(pImpl->memoryNeurons_.begin(), it);
        pImpl->memoryActivations_[index] = activation;
        pImpl->memoryTimestamps_[index] = pImpl->brain_->getCurrentStep();
        
        // Update trace importance
        if (pImpl->index_.size() > index) {
            pImpl->index_[index].importance = std::max(pImpl->index_[index].importance, activation);
        }
    } else {
        // Add new neuron to memory
        if (pImpl->memoryNeurons_.size() < config_.capacity) {
            pImpl->memoryNeurons_.push_back(neuron);
            pImpl->memoryActivations_.push_back(activation);
            pImpl->memoryTimestamps_.push_back(pImpl->brain_->getCurrentStep());
            
            // Initialize index entry
            MemoryIndexEntry indexEntry(pImpl->brain_->getCurrentStep(), activation);
            pImpl->index_.push_back(indexEntry);
        }
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    return pImpl->memoryActivations_;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), neuron) != pImpl->memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), neuron);
    if (it != pImpl->memoryNeurons_.end()) {
        size_t index = std::distance(pImpl->memoryNeurons_.begin(), it);
        return pImpl->memoryActivations_[index];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    // Update recurrent connections
    updateRecurrentConnections();
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition
    runCompetition();
    
    // Update cached memories
    if (config_.enableCaching) {
        updateCache();
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    float threshold = config_.decayRate;
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < pImpl->memoryActivations_.size(); ++i) {
        if (pImpl->memoryActivations_[i] < threshold) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (iterate backwards to avoid index corruption)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t index = *it;
        
        // Remove from main arrays
        pImpl->memoryNeurons_.erase(pImpl->memoryNeurons_.begin() + index);
        pImpl->memoryActivations_.erase(pImpl->memoryActivations_.begin() + index);
        pImpl->memoryTimestamps_.erase(pImpl->memoryTimestamps_.begin() + index);
        
        if (index < pImpl->index_.size()) {
            pImpl->index_.erase(pImpl->index_.begin() + index);
        }
    }
}

void NeuralWorkingMemory::clear() {
    pImpl->memoryNeurons_.clear();
    pImpl->memoryActivations_.clear();
    pImpl->memoryTimestamps_.clear();
    pImpl->activeTraces_.clear();
    pImpl->traceImportance_.clear();
    pImpl->winners_.clear();
    pImpl->competitionScores_.clear();
    pImpl->index_.clear();
    pImpl->cache_.clear();
    pImpl->highPriorityCache_.clear();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    pImpl->recurrentConnections_.emplace_back(from, to);
    pImpl->connectionStrengths_.push_back(strength);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    for (size_t i = 0; i < pImpl->recurrentConnections_.size(); ++i) {
        auto& conn = pImpl->recurrentConnections_[i];
        float strength = pImpl->connectionStrengths_[i];
        
        // Apply recurrent connection
        if (brain_) {
            brain_->injectCurrent(conn.first, strength * 0.1f);
        }
    }
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : pImpl->memoryActivations_) {
        activation *= factor;
    }
    
    // Update index importance
    for (auto& indexEntry : pImpl->index_) {
        indexEntry.importance *= factor;
    }
}

void NeuralWorkingMemory::runCompetition() {
    pImpl->competitionScores_.clear();
    pImpl->winners_.clear();
    
    if (pImpl->memoryActivations_.empty()) {
        return;
    }
    
    // Calculate competition scores
    float maxActivation = *std::max_element(pImpl->memoryActivations_.begin(), 
                                           pImpl->memoryActivations_.end());
    
    for (size_t i = 0; i < pImpl->memoryActivations_.size(); ++i) {
        float score = pImpl->memoryActivations_[i] / maxActivation;
        pImpl->competitionScores_.push_back(score);
        
        // Determine winners (neurons with activation > 70% of max)
        if (score > 0.7f) {
            pImpl->winners_.push_back(pImpl->memoryNeurons_[i]);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(pImpl->winners_.begin(), pImpl->winners_.end(), neuron) != pImpl->winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (pImpl->memoryActivations_.empty()) {
        return 0.0f;
    }
    
    float totalActivity = 0.0f;
    for (float activation : pImpl->memoryActivations_) {
        totalActivity += activation;
    }
    
    return totalActivity / static_cast<float>(pImpl->memoryActivations_.size());
}

// Enhanced memory capabilities implementation

void NeuralWorkingMemory::indexMemory(const std::vector<float>& pattern, SimulationStep timestamp,
                                     float importance, float novelty) {
    if (pImpl->index_.size() >= config_.indexSize) {
        // Remove oldest entries
        size_t toRemove = pImpl->index_.size() - config_.indexSize / 2;
        pImpl->index_.erase(pImpl->index_.begin(), 
                           pImpl->index_.begin() + toRemove);
        
        // Also remove corresponding patterns
        pImpl->indexedPatterns_.erase(pImpl->indexedPatterns_.begin(),
                                     pImpl->indexedPatterns_.begin() + toRemove);
        
        // Remove corresponding neurons and activations
        for (size_t i = 0; i < toRemove; ++i) {
            auto it = std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), 
                               pImpl->indexedPatterns_[i][0]); // Simplified
            if (it != pImpl->memoryNeurons_.end()) {
                size_t index = std::distance(pImpl->memoryNeurons_.begin(), it);
                pImpl->memoryNeurons_.erase(it);
                pImpl->memoryActivations_.erase(pImpl->memoryActivations_.begin() + index);
                pImpl->memoryTimestamps_.erase(pImpl->memoryTimestamps_.begin() + index);
            }
        }
    }
    
    // Calculate sequence number
    size_t sequenceNumber = pImpl->index_.size();
    
    // Create index entry
    MemoryIndexEntry indexEntry(timestamp, importance, novelty, sequenceNumber);
    pImpl->index_.push_back(indexEntry);
    
    // Store pattern for similarity search
    pImpl->indexedPatterns_.push_back(pattern);
    
    // Update average importance
    float currentAvg = pImpl->averageImportance_.load(std::memory_order_relaxed);
    float newAvg = (currentAvg * (pImpl->index_.size() - 1) + importance) / pImpl->index_.size();
    pImpl->averageImportance_.store(newAvg, std::memory_order_relaxed);
}

std::vector<size_t> NeuralWorkingMemory::searchSimilar(const std::vector<float>& pattern,
                                                      size_t maxResults) const {
    std::vector<std::pair<float, size_t>> scores;
    
    for (size_t i = 0; i < pImpl->indexedPatterns_.size(); ++i) {
        const auto& storedPattern = pImpl->indexedPatterns_[i];
        
        // Calculate cosine similarity
        float similarity = 0.0f;
        for (size_t j = 0; j < std::min(pattern.size(), storedPattern.size()); ++j) {
            similarity += pattern[j] * storedPattern[j];
        }
        
        if (similarity > 0.0f) {
            scores.emplace_back(similarity, i);
        }
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Extract top results
    std::vector<size_t> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        results.push_back(scores[i].second);
    }
    
    return results;
}

std::vector<float> NeuralWorkingMemory::compressMemory(const std::vector<float>& pattern) const {
    if (!config_.enableCompression) {
        return pattern;
    }
    
    pImpl->compressionCount_.fetch_add(1, std::memory_order_relaxed);
    
    // Simple compression: downsample for demonstration
    // In a real implementation, this would use LZW, Huffman, or PCA
    size_t compressedSize = std::max(size_t(1), pattern.size() / 4);
    std::vector<float> compressed(compressedSize);
    
    for (size_t i = 0; i < compressedSize; ++i) {
        float sum = 0.0f;
        size_t count = 0;
        for (size_t j = 0; j < 4; ++j) {
            size_t idx = i * 4 + j;
            if (idx < pattern.size()) {
                sum += pattern[idx];
                count++;
            }
        }
        compressed[i] = sum / static_cast<float>(count);
    }
    
    return compressed;
}

bool NeuralWorkingMemory::cacheMemory(size_t index, bool highPriority) {
    if (!config_.enableCaching) {
        return false;
    }
    
    if (index >= pImpl->indexedPatterns_.size()) {
        return false;
    }
    
    const auto& pattern = pImpl->indexedPatterns_[index];
    
    // Check cache size
    if (pImpl->cache_.size() >= config_.cacheSize) {
        // Remove least recently used entries
        size_t toRemove = config_.cacheSize / 2;
        auto it = pImpl->cache_.begin();
        for (size_t i = 0; i < toRemove && it != pImpl->cache_.end(); ++i, ++it) {
            // Remove from high priority cache if present
            if (highPriority) {
                pImpl->highPriorityCache_.erase(std::next(pImpl->cache_.begin(), i));
            }
        }
        pImpl->cache_.erase(pImpl->cache_.begin(), std::next(pImpl->cache_.begin(), toRemove));
    }
    
    // Add to cache
    pImpl->cache_[index] = pattern;
    
    if (highPriority) {
        pImpl->highPriorityCache_.insert(index);
    }
    
    return true;
}

void NeuralWorkingMemory::createMemoryTrace(const std::vector<float>& pattern, 
                                           float initialImportance) {
    // Create a new memory trace
    store(pattern, initialImportance);
    
    // Add to active traces
    pImpl->activeTraces_.push_back(pImpl->activeTraces_.size());
    pImpl->traceImportance_.push_back(initialImportance);
    
    // Schedule for decay
    if (config_.enableDecay) {
        // Will be processed during update
    }
}

void NeuralWorkingMemory::consolidate(float relevanceThreshold) {
    // Find and promote high-relevance memories
    std::vector<size_t> toConsolidate;
    
    for (size_t i = 0; i < pImpl->index_.size(); ++i) {
        if (pImpl->index_[i].importance >= relevanceThreshold) {
            toConsolidate.push_back(i);
        }
    }
    
    // Promote consolidated memories
    for (size_t index : toConsolidate) {
        // Mark for long-term storage
        pImpl->index_[index].sequenceNumber += 1000000; // Large offset for long-term
        
        // Update importance to ensure retention
        pImpl->index_[index].importance = 1.0f;
    }
}

void NeuralWorkingMemory::decay(float decayRate, float minImportance) {
    // Decay memory activations
    for (auto& activation : pImpl->memoryActivations_) {
        activation *= decayRate;
        if (activation < minImportance) {
            activation = 0.0f;
        }
    }
    
    // Update index importance
    for (auto& indexEntry : pImpl->index_) {
        indexEntry.importance *= decayRate;
        if (indexEntry.importance < minImportance) {
            indexEntry.importance = 0.0f;
        }
    }
}

void NeuralWorkingMemory::enhanceWithPlasticity(float plasticityFactor) {
    // Enhance memory with neural plasticity
    for (auto& connection : pImpl->connectionStrengths_) {
        connection *= (1.0f + plasticityFactor * 0.1f);
    }
    
    // Update recurrent connections
    for (auto& conn : pImpl->recurrentConnections_) {
        if (brain_) {
            brain_->injectCurrent(conn.first, conn.second * 0.05f);
        }
    }
}

void NeuralWorkingMemory::adaptToEnvironment(const std::vector<float>& environmentalFactors) {
    // Adapt memory based on environmental factors
    for (size_t i = 0; i < pImpl->memoryActivations_.size(); ++i) {
        // Modulate activation based on environment
        pImpl->memoryActivations_[i] *= (1.0f + environmentalFactors[0] * 0.1f);
        
        // Update index relevance
        if (i < pImpl->index_.size()) {
            pImpl->index_[i].relevanceScore = std::min(1.0f, 
                                                      pImpl->index_[i].relevanceScore + 
                                                      environmentalFactors[0] * 0.05f);
        }
    }
}

void NeuralWorkingMemory::coordinateAccess(MemorySystemType type, 
                                           const std::vector<float>& pattern,
                                           AccessType accessType) {
    // Coordinate memory access with other systems
    if (coordinator_) {
        coordinator_->coordinateAccess(type, pattern, accessType);
    }
}

void NeuralWorkingMemory::adjustPriority(const std::vector<float>& pattern, float priorityDelta) {
    // Adjust priority of memory based on pattern
    size_t bestMatch = findBestPatternMatch(pattern);
    
    if (bestMatch < pImpl->index_.size()) {
        pImpl->index_[bestMatch].importance += priorityDelta;
        pImpl->index_[bestMatch].importance = std::clamp(pImpl->index_[bestMatch].importance, 
                                                        0.0f, 1.0f);
    }
}

void NeuralWorkingMemory::replay(const std::vector<float>& pattern, size_t repetitions) {
    // Replay memory for consolidation
    for (size_t i = 0; i < repetitions; ++i) {
        // Replay the pattern
        store(pattern, 1.0f);
        
        // Wait a bit between repetitions
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void NeuralWorkingMemory::strengthenAssociations(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float strengthFactor) {
    // Create associations between patterns
    if (brain_) {
        // Get associative memory system
        auto* associativeMemory = brain_->getAssociativeMemory();
        if (associativeMemory) {
            associativeMemory->associate(patternA, patternB, strengthFactor);
        }
    }
}

void NeuralWorkingMemory::detectInterference(const std::vector<float>& pattern) {
    // Detect interference with existing memories
    size_t bestMatch = findBestPatternMatch(pattern);
    
    if (bestMatch < pImpl->index_.size()) {
        // Check for potential interference
        float similarity = calculatePatternSimilarity(pattern, pImpl->indexedPatterns_[bestMatch]);
        
        if (similarity > config_.interferenceThreshold) {
            // Signal interference detected
            if (coordinator_) {
                coordinator_->detectInterference(bestMatch);
            }
        }
    }
}

void NeuralWorkingMemory::resolveInterference(const std::vector<float>& pattern) {
    // Resolve interference with existing memories
    if (coordinator_) {
        coordinator_->resolveInterference(pattern);
    }
}

void NeuralWorkingMemory::optimizeAccessPatterns() {
    // Optimize memory access patterns
    if (pImpl->cache_.size() > 0) {
        // Move frequently accessed patterns to high priority cache
        size_t highPriorityCount = pImpl->highPriorityCache_.size();
        size_t targetHighPriority = config_.cacheSize / 4;
        
        if (highPriorityCount < targetHighPriority) {
            size_t toPromote = targetHighPriority - highPriorityCount;
            for (auto& [index, pattern] : pImpl->cache_) {
                if (toPromote-- <= 0) break;
                pImpl->highPriorityCache_.insert(index);
            }
        }
    }
}

void NeuralWorkingMemory::optimizeMemoryLayout() {
    // Optimize memory layout for better cache locality
    // Reorganize memory traces based on importance and usage
    std::vector<size_t> indices(pImpl->index_.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    // Sort by importance (descending)
    std::sort(indices.begin(), indices.end(),
              [this](size_t a, size_t b) {
                  return pImpl->index_[a].importance > pImpl->index_[b].importance;
              });
    
    // Reorganize data structures
    std::vector<size_t> reorderedIndices;
    std::vector<NeuronId> reorderedNeurons;
    std::vector<float> reorderedActivations;
    std::vector<SimulationStep> reorderedTimestamps;
    
    for (size_t index : indices) {
        reorderedIndices.push_back(index);
        if (index < pImpl->memoryNeurons_.size()) {
            reorderedNeurons.push_back(pImpl->memoryNeurons_[index]);
            reorderedActivations.push_back(pImpl->memoryActivations_[index]);
            reorderedTimestamps.push_back(pImpl->memoryTimestamps_[index]);
        }
    }
    
    // Replace with reorganized data
    pImpl->memoryNeurons_ = std::move(reorderedNeurons);
    pImpl->memoryActivations_ = std::move(reorderedActivations);
    pImpl->memoryTimestamps_ = std::move(reorderedTimestamps);
}

size_t NeuralWorkingMemory::getTotalMemoryUsage() const {
    size_t total = 0;
    
    // Main memory
    total += pImpl->memoryNeurons_.size() * sizeof(NeuronId);
    total += pImpl->memoryActivations_.size() * sizeof(float);
    total += pImpl->memoryTimestamps_.size() * sizeof(SimulationStep);
    
    // Index
    total += pImpl->index_.size() * sizeof(MemoryIndexEntry);
    
    // Cache
    for (const auto& [index, pattern] : pImpl->cache_) {
        total += pattern.size() * sizeof(float);
    }
    
    return total;
}

double NeuralWorkingMemory::getAverageAccessTime() const {
    if (pImpl->accessCount_.load(std::memory_order_relaxed) == 0) {
        return 0.0;
    }
    
    double totalTime = pImpl->totalAccessTime_.load(std::memory_order_relaxed);
    size_t count = pImpl->accessCount_.load(std::memory_order_relaxed);
    
    return totalTime / count;
}

NeuralWorkingMemory::MemoryStats NeuralWorkingMemory::getStats() const {
    MemoryStats stats;
    
    stats.activeTraces = pImpl->activeTraces_.size();
    stats.cachedMemories = pImpl->cache_.size();
    stats.averageImportance = pImpl->averageImportance_.load(std::memory_order_relaxed);
    
    // Calculate compression ratio
    size_t totalCompressed = 0;
    size_t totalOriginal = 0;
    for (const auto& [index, pattern] : pImpl->cache_) {
        totalCompressed += pattern.size();
    }
    for (const auto& pattern : pImpl->indexedPatterns_) {
        totalOriginal += pattern.size();
    }
    
    if (totalOriginal > 0) {
        stats.compressionRatio = static_cast<float>(totalCompressed) / totalOriginal;
    } else {
        stats.compressionRatio = 1.0f;
    }
    
    // Estimate interference level (simplified)
    stats.interferenceLevel = 1.0f - stats.compressionRatio;
    
    // Estimate plasticity influence
    stats.plasticityInfluence = std::min(1.0f, static_cast<float>(pImpl->compressionCount_.load()) / 100.0f);
    
    return stats;
}

void NeuralWorkingMemory::setMemoryConfig(const MemoryConfig& config) {
    config_ = config;
}

void NeuralWorkingMemory::autoTune(float performanceThreshold) {
    // Auto-tune memory configuration based on performance
    analyzePerformance();
    
    float currentScore = getPerformanceScore();
    
    if (currentScore < performanceThreshold) {
        // Adjust configuration for better performance
        if (config_.capacity > 10) {
            config_.capacity = static_cast<size_t>(config_.capacity * 0.9f);
        }
        if (config_.indexSize > 64) {
            config_.indexSize = static_cast<size_t>(config_.indexSize * 0.9f);
        }
        if (config_.cacheSize > 16) {
            config_.cacheSize = static_cast<size_t>(config_.cacheSize * 0.9f);
        }
        
        // Decay rate adjustment
        config_.decayRate = std::min(1.0f, config_.decayRate * 1.1f);
    } else {
        // Improve configuration for better performance
        if (config_.capacity < 500) {
            config_.capacity = static_cast<size_t>(config_.capacity * 1.1f);
        }
        if (config_.indexSize < 512) {
            config_.indexSize = static_cast<size_t>(config_.indexSize * 1.1f);
        }
        if (config_.cacheSize < 128) {
            config_.cacheSize = static_cast<size_t>(config_.cacheSize * 1.1f);
        }
        
        // Decay rate adjustment
        config_.decayRate = std::max(0.5f, config_.decayRate * 0.9f);
    }
}

void NeuralWorkingMemory::analyzePerformance() {
    // Analyze performance metrics
    size_t accessCount = pImpl->accessCount_.load(std::memory_order_relaxed);
    float avgAccessTime = getAverageAccessTime();
    
    // Check if performance is within acceptable bounds
    if (avgAccessTime > 100.0) {  // 100ms threshold
        NLM_LOG_WARN("NeuralWorkingMemory: High average access time: " + 
                    std::to_string(avgAccessTime) + "ms");
    }
    
    if (accessCount > 1000) {
        float compressionEfficiency = static_cast<float>(pImpl->compressionCount_.load()) / 
                                     static_cast<float>(accessCount);
        NLM_LOG_INFO("NeuralWorkingMemory: Compression efficiency: " + 
                    std::to_string(compressionEfficiency * 100.0f) + "%");
    }
}

float NeuralWorkingMemory::getPerformanceScore() const {
    float accessTimeScore = std::min(1.0f, 100.0f / static_cast<float>(getAverageAccessTime()));
    float importanceScore = pImpl->averageImportance_.load(std::memory_order_relaxed);
    float compressionScore = std::min(1.0f, static_cast<float>(pImpl->compressionCount_.load()) / 100.0f);
    
    return (accessTimeScore + importanceScore + compressionScore) / 3.0f;
}

// Helper functions for pattern matching and similarity

size_t NeuralWorkingMemory::findBestPatternMatch(const std::vector<float>& pattern) const {
    size_t bestMatch = 0;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < pImpl->indexedPatterns_.size(); ++i) {
        float similarity = calculatePatternSimilarity(pattern, pImpl->indexedPatterns_[i]);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = i;
        }
    }
    
    return bestMatch;
}

float NeuralWorkingMemory::calculatePatternSimilarity(const std::vector<float>& patternA,
                                                    const std::vector<float>& patternB) const {
    if (patternA.size() != patternB.size()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < patternA.size(); ++i) {
        dotProduct += patternA[i] * patternB[i];
        normA += patternA[i] * patternA[i];
        normB += patternB[i] * patternB[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    
    return dotProduct / std::sqrt(normA * normB);
}

void NeuralWorkingMemory::updateCache() {
    // Update cache based on usage patterns
    // Move frequently accessed items to high priority cache
    for (auto& [index, pattern] : pImpl->cache_) {
        // Simple LRU simulation: all items have equal priority for now
        pImpl->highPriorityCache_.insert(index);
    }
}

} // namespace nlm
