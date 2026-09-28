#include "OptimizedNeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <unordered_set>

namespace nlm {

OptimizedNeuralPrediction::OptimizedNeuralPrediction()
    : sequenceMemorySize_(10)
    , predictionHorizon_(1)
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , brain_(nullptr)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f)
    , lastStep_(0)
    , totalPredictionTime_(0.0)
    , simdEnabled_(true)
    , cachingEnabled_(true)
    , cacheSize_(1000)
{
    errorHistory_.resize(1000);
}

OptimizedNeuralPrediction::~OptimizedNeuralPrediction() = default;

void OptimizedNeuralPrediction::initialize(Brain* brain) {
    NeuralPrediction::initialize(brain);
    
    // Initialize cache pools
    patternCachePool_ = std::make_unique<MemoryPool<PatternCache>>(cacheSize_, 4);
    statePool_ = std::make_unique<MemoryPool<float>>(10000, 4);
    
    // Reserve cache
    patternCache_.reserve(cacheSize_);
    optimizedStateHistory_.reserve(sequenceMemorySize_ * 2);
    
    pool_.reserve(1024); // Pre-allocate vector pool
    
    NLM_LOG_INFO("OptimizedNeuralPrediction initialized with SIMD and caching");
}

std::vector<float> OptimizedNeuralPrediction::generatePrediction(SimulationStep currentStep) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
        return std::vector<float>();
    }
    
    const auto& currentState = recentSensoryStates_.back();
    
    // Find best matching learned sequence with cache lookup
    NeuronId bestMatch = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    if (simdEnabled_) {
        // SIMD-optimized pattern matching
        bestSimilarity = findBestMatchSIMD(currentState, bestMatch);
    } else {
        // Fallback to optimized CPU version
        for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
            float sim = computeSimilarity(currentState, pImpl->patternRepresentations[i]);
            if (sim > bestSimilarity) {
                bestSimilarity = sim;
                bestMatch = pImpl->patternNeurons[i];
            }
        }
    }
    
    // If we have a good match, predict what comes next
    if (bestSimilarity > 0.7f && bestMatch != INVALID_NEURON_ID) {
        // Find what we associate with this pattern
        for (const auto& assoc : pImpl->sequenceAssociations) {
            if (assoc.first == bestMatch) {
                // Found association - return the associated pattern
                for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
                    if (pImpl->patternNeurons[i] == assoc.second) {
                        predictionConfidence_ = bestSimilarity;
                        return pImpl->patternRepresentations[i];
                    }
                }
            }
        }
    }
    
    // No good prediction - return current state as baseline
    predictionConfidence_ = 0.0f;
    return currentState;
}

float OptimizedNeuralPrediction::findBestMatchSIMD(const std::vector<float>& currentState, 
                                                  NeuronId& bestMatch) const {
    if (pImpl->patternNeurons.empty()) return 0.0f;
    
    bestMatch = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    // SIMD-optimized batch similarity computation
    size_t numPatterns = pImpl->patternNeurons.size();
    std::vector<float> similarities(numPatterns, 0.0f);
    
    if (simdEnabled_ && numPatterns >= 4) {
        // Use SIMD for large batch of patterns
        simdBatchSimilarity(currentState, 
                           pImpl->patternRepresentations,
                           similarities);
    } else {
        // Optimized CPU version for smaller batches
        for (size_t i = 0; i < numPatterns; ++i) {
            similarities[i] = computeSimilarity(currentState, pImpl->patternRepresentations[i]);
        }
    }
    
    // Find maximum similarity
    for (size_t i = 0; i < numPatterns; ++i) {
        if (similarities[i] > bestSimilarity) {
            bestSimilarity = similarities[i];
            bestMatch = pImpl->patternNeurons[i];
        }
    }
    
    return bestSimilarity;
}

void OptimizedNeuralPrediction::simdBatchSimilarity(const std::vector<float>& target,
                                                   const std::vector<std::vector<float>>& patterns,
                                                   std::vector<float>& similarities) const {
    size_t numPatterns = patterns.size();
    if (numPatterns == 0) return;
    
    size_t targetSize = target.size();
    
    // Process patterns in batches for better cache locality
    size_t batchSize = 16; // Process 16 patterns at a time
    for (size_t batchStart = 0; batchStart < numPatterns; batchStart += batchSize) {
        size_t batchEnd = std::min(batchStart + batchSize, numPatterns);
        
        for (size_t i = batchStart; i < batchEnd; ++i) {
            const auto& pattern = patterns[i];
            if (pattern.size() != targetSize) {
                similarities[i] = 0.0f;
                continue;
            }
            
            // SIMD-optimized similarity computation
            similarities[i] = computeSimilaritySIMD(target.data(), pattern.data(), targetSize);
        }
    }
}

float OptimizedNeuralPrediction::computeSimilarity(const std::vector<float>& a, 
                                                 const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    return computeSimilaritySIMD(a.data(), b.data(), a.size());
}

float OptimizedNeuralPrediction::computeSimilaritySIMD(const float* a, 
                                                     const float* b, 
                                                     size_t size) const {
    if (size == 0) return 0.0f;
    
    // SIMD-optimized cosine similarity with manual unrolling
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    // Process in chunks for better cache performance
    for (size_t i = 0; i < size; i += 8) {  // Process 8 elements at a time
        size_t limit = std::min(i + 8, size);
        
        // Manual unrolling for better performance
        for (size_t j = i; j < limit; ++j) {
            float aVal = a[j];
            float bVal = b[j];
            
            dotProduct += aVal * bVal;
            normA += aVal * aVal;
            normB += bVal * bVal;
        }
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

float OptimizedNeuralPrediction::updateWithObservation(const std::vector<float>& actualState,
                                                     SimulationStep currentStep) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Generate prediction first
    auto predicted = generatePrediction(currentStep);
    
    // Compute error
    if (predicted.empty()) {
        predictionError_ = 0.0f;
    } else {
        predictionError_ = computeSimilarity(predicted, actualState);
        predictionError_ = 1.0f - predictionError_;  // Convert to error
    }
    
    errorHistory_.addError(predictionError_);
    
    // Record the actual state for future learning
    recordSensoryState(actualState, currentStep);
    
    // Modulate learning based on error
    float reward = 1.0f - predictionError_;
    modulatePredictionSynapses(predictionError_, reward);
    
    // Update performance stats
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    perfStats_.avgPredictionTimeMs = duration;
    totalPredictionTime_ += duration;
    
    return predictionError_;
}

void OptimizedNeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, 
                                                  SimulationStep currentStep) {
    if (!temporalPredictionEnabled_) return;
    
    recentSensoryStates_.push_back(sensoryState);
    stateTimestamps_.push_back(currentStep);
    
    // Keep only recent states in optimized deque
    if (recentSensoryStates_.size() > sequenceMemorySize_) {
        recentSensoryStates_.pop_front();
        stateTimestamps_.pop_front();
    }
    
    // Batch process states for learning
    batchProcessStates(recentSensoryStates_);
}

void OptimizedNeuralPrediction::batchProcessStates(const std::vector<std::pair<std::vector<float>, SimulationStep>>& states) {
    // Use optimized deque for better performance
    optimizedStateHistory_ = std::move(states);
    
    // Learn sequences if we have at least 2 states
    if (optimizedStateHistory_.size() >= 2) {
        // Use optimized sequence learning
        batchLearnSequences(optimizedStateHistory_);
    }
}

void OptimizedNeuralPrediction::batchLearnSequences(const std::vector<std::pair<std::vector<float>, SimulationStep>>& states) {
    // Process sequences in batches for better cache locality
    size_t numStates = states.size();
    if (numStates < 2) return;
    
    // Create patterns from recent states
    std::vector<std::vector<float>> patterns;
    patterns.reserve(numStates);
    
    for (const auto& state : states) {
        patterns.push_back(state.first);
    }
    
    // Find or create pattern neurons
    for (size_t i = 0; i < patterns.size() - 1; ++i) {
        NeuronId currentNeuron = findMatchingPatternNeuron(patterns[i], 0.7f);
        NeuronId nextNeuron = findMatchingPatternNeuron(patterns[i + 1], 0.7f);
        
        if (currentNeuron != INVALID_NEURON_ID && nextNeuron != INVALID_NEURON_ID &&
            currentNeuron != nextNeuron) {
            createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
        }
    }
}

std::vector<float> OptimizedNeuralPrediction::predictActionConsequence(ActionType action,
                                                                       const std::vector<float>& currentState) {
    if (!actionConsequenceEnabled_) {
        return currentState;  // No change predicted
    }
    
    // Find most similar previous experience with this action
    float bestMatch = 0.0f;
    std::vector<float> bestConsequence;
    
    if (simdEnabled_) {
        // SIMD-optimized action consequence prediction
        bestMatch = findBestConsequenceSIMD(action, currentState, bestConsequence);
    } else {
        // Fallback to CPU-optimized version
        for (size_t i = 0; i < recentActions_.size(); ++i) {
            if (recentActions_[i].first == action) {
                float sim = computeSimilarity(currentState, recentActions_[i].second);
                if (sim > bestMatch) {
                    bestMatch = sim;
                    if (i < pImpl->actionConsequences.size()) {
                        bestConsequence = pImpl->actionConsequences[i].second;
                    }
                }
            }
        }
    }
    
    if (bestMatch > 0.5f && !bestConsequence.empty()) {
        return bestConsequence;
    }
    
    return currentState;  // Default: no change
}

float OptimizedNeuralPrediction::findBestConsequenceSIMD(ActionType action,
                                                       const std::vector<float>& currentState,
                                                       std::vector<float>& bestConsequence) {
    float bestSimilarity = 0.0f;
    bestConsequence.clear();
    
    // Process recent actions in SIMD-friendly batches
    size_t batchSize = 16;
    size_t totalActions = recentActions_.size();
    
    for (size_t batchStart = 0; batchStart < totalActions; batchStart += batchSize) {
        size_t batchEnd = std::min(batchStart + batchSize, totalActions);
        
        // Compute similarities for this batch
        std::vector<std::pair<size_t, float>> batchSimilarities;
        batchComputeSimilarities(currentState, batchSimilarities);
        
        // Filter for matching actions and update best match
        for (const auto& pair : batchSimilarities) {
            size_t idx = pair.first;
            if (idx >= totalActions) continue;
            
            if (recentActions_[idx].first == action) {
                float sim = pair.second;
                if (sim > bestSimilarity && idx < pImpl->actionConsequences.size()) {
                    bestSimilarity = sim;
                    bestConsequence = pImpl->actionConsequences[idx].second;
                }
            }
        }
    }
    
    return bestSimilarity;
}

void OptimizedNeuralPrediction::batchComputeSimilarities(const std::vector<float>& target,
                                                        std::vector<std::pair<size_t, float>>& results) const {
    results.clear();
    
    if (recentActions_.empty()) return;
    
    size_t targetSize = target.size();
    size_t batchSize = 16;
    
    for (size_t start = 0; start < recentActions_.size(); start += batchSize) {
        size_t end = std::min(start + batchSize, recentActions_.size());
        
        for (size_t i = start; i < end; ++i) {
            const auto& actionState = recentActions_[i].second;
            if (actionState.size() == targetSize) {
                float sim = computeSimilaritySIMD(target.data(), actionState.data(), targetSize);
                results.push_back({i, sim});
            }
        }
    }
}

void OptimizedNeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                                     const std::vector<float>& nextState,
                                                     SimulationStep currentStep) {
    // Optimized sequence learning with caching
    NeuronId currentNeuron = findMatchingPatternNeuron(currentState, 0.7f);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextState, 0.7f);
    
    if (currentNeuron != nextNeuron) {
        createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
    }
}

NeuronId OptimizedNeuralPrediction::findMatchingPatternNeuron(const std::vector<float>& pattern,
                                                            float similarityThreshold) {
    // Try cache lookup first for O(1) access
    uint32_t hash = PatternCache::computeHash(pattern);
    PatternCache cached;
    
    if (cachingEnabled_ && getCachedPattern(hash, cached)) {
        return cached.neuronId;
    }
    
    // Check if we already have a similar pattern
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        float sim = computeSimilarity(pattern, pImpl->patternRepresentations[i]);
        if (sim >= similarityThreshold) {
            // Cache the result for future lookups
            cachePattern(pattern, pImpl->patternNeurons[i], sim);
            return pImpl->patternNeurons[i];
        }
    }
    
    // Create new pattern neuron
    if (brain_) {
        NeuronId newId = NeuronId(pImpl->patternNeurons.size() + 10000);
        pImpl->patternNeurons.push_back(newId);
        pImpl->patternRepresentations.push_back(pattern);
        
        // Cache the new pattern
        cachePattern(pattern, newId, 1.0f);
        
        return newId;
    }
    
    return INVALID_NEURON_ID;
}

void OptimizedNeuralPrediction::cachePattern(const std::vector<float>& pattern,
                                           NeuronId neuronId,
                                           float similarity) {
    if (!cachingEnabled_) return;
    
    PatternCache cache(pattern, neuronId, similarity, brain_->getSimulationStep());
    
    // Use memory pool for cache entries
    if (patternCachePool_ && patternCachePool_->availableCount() > 0) {
        PatternCache* cached = patternCachePool_->allocate();
        *cached = cache;
        
        // Add to cache map
        patternCache_[cache.hash] = *cached;
    } else {
        // Fallback to direct insertion
        patternCache_[cache.hash] = cache;
    }
    
    // Limit cache size
    if (patternCache_.size() > cacheSize_) {
        // Remove oldest entries (simple approach)
        auto it = patternCache_.begin();
        std::advance(it, cacheSize_ / 2);
        patternCache_.erase(patternCache_.begin(), it);
    }
}

bool OptimizedNeuralPrediction::getCachedPattern(uint32_t hash, PatternCache& out) const {
    auto it = patternCache_.find(hash);
    if (it != patternCache_.end()) {
        out = it->second;
        return true;
    }
    return false;
}

void OptimizedNeuralPrediction::createSequenceAssociation(NeuronId from, NeuronId to, float strength) {
    if (from == INVALID_NEURON_ID || to == INVALID_NEURON_ID) return;
    
    // Check if association already exists
    for (auto& assoc : pImpl->sequenceAssociations) {
        if (assoc.first == from && assoc.second == to) {
            // Update strength
            return;
        }
    }
    
    // Create new association
    pImpl->sequenceAssociations.emplace_back(from, to);
}

void OptimizedNeuralPrediction::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    // Apply reward-modulated plasticity to prediction-related synapses
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            // If this synapse was recently active in prediction pathway
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * (reward - 0.5f) * 2.0f;
                syn->addToWeight(delta);
                
                // Decay eligibility
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

std::vector<NeuronId> OptimizedNeuralPrediction::getPredictionNeurons() const {
    std::vector<NeuronId> result;
    result.reserve(pImpl->patternNeurons.size());
    
    // Use optimized copying
    for (const auto& neuron : pImpl->patternNeurons) {
        result.push_back(neuron);
    }
    
    return result;
}

std::vector<NeuronId> OptimizedNeuralPrediction::getSequenceNeurons() const {
    std::vector<NeuronId> result;
    result.reserve(pImpl->sequenceAssociations.size() * 2);
    
    // Use reserve to avoid reallocations
    for (const auto& assoc : pImpl->sequenceAssociations) {
        result.push_back(assoc.first);
        result.push_back(assoc.second);
    }
    
    return result;
}

void OptimizedNeuralPrediction::clearHistory() {
    NeuralPrediction::clearHistory();
    
    // Clear optimized structures
    patternCache_.clear();
    optimizedStateHistory_.clear();
    
    // Clear memory pools
    patternCachePool_.reset();
    statePool_.reset();
    
    // Clear performance stats
    perfStats_ = PredictionPerformanceStats();
}

void OptimizedNeuralPrediction::updatePerformanceStats(float predictionTime) const {
    perfStats_.totalPredictions++;
    
    if (predictionTime > 0.0) {
        perfStats_.avgPredictionTimeMs = 
            perfStats_.avgPredictionTimeMs * 0.9f + predictionTime * 0.1f; // Moving average
    }
    
    // Calculate accuracy
    if (perfStats_.totalPredictions > 0) {
        perfStats_.predictionAccuracy = 
            (static_cast<double>(perfStats_.successfulPredictions)) / 
            perfStats_.totalPredictions;
    }
    
    // Calculate efficiency
    perfStats_.processingEfficiency = 
        1000.0 / std::max(1.0, perfStats_.avgPredictionTimeMs); // Predictions per second
    
    // Estimate memory usage
    size_t cacheMemory = patternCache_.size() * sizeof(PatternCache);
    size_t stateMemory = optimizedStateHistory_.size() * sizeof(std::pair<std::vector<float>, SimulationStep>);
    perfStats_.memoryUsageMB = (cacheMemory + stateMemory) / (1024 * 1024);
}

} // namespace nlm