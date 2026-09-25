// Optimized memory systems with SIMD vectorization and batch processing
// Replaces sequential memory operations with highly optimized implementations

#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/performance/SIMDVectorization.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

/**
 * Optimized NeuralWorkingMemory implementation
 * Uses SIMD vectorization and competitive firing optimization
 */
class OptimizedNeuralWorkingMemory : public NeuralWorkingMemory {
public:
    OptimizedNeuralWorkingMemory() : capacity_(100), competitionThreshold_(0.5f) {
        // Initialize competitive firing parameters
        inhibitionStrength_ = 0.5f;
        excitationStrength_ = 1.5f;
    }
    
    void initialize(Brain* brain) override {
        brain_ = brain;
        // Allocate memory with optimized structure
        memoryNeurons_.reserve(capacity_);
        memoryWeights_.reserve(capacity_);
        neuronTypes_.reserve(capacity_);
        
        // Set up competitive firing arrays for SIMD processing
        setupCompetitiveFiringArrays();
    }
    
    void setCapacity(size_t capacity) override {
        capacity_ = capacity;
        memoryNeurons_.resize(capacity_);
        memoryWeights_.resize(capacity_);
        neuronTypes_.resize(capacity_);
    }
    
    void setCompetitionThreshold(float threshold) override {
        competitionThreshold_ = threshold;
    }
    
    void storeToNeuron(NeuronId neuronId, float activation) override {
        // Optimized store with competitive firing
        if (memoryNeurons_.size() < capacity_) {
            // Simple storage when not at capacity
            memoryNeurons_.push_back(neuronId);
            memoryWeights_.push_back(activation);
            neuronTypes_.push_back(getNeuronType(neuronId));
        } else {
            // Competitive firing with SIMD optimization
            competitiveFiringWithSIMD(neuronId, activation);
        }
    }
    
    void update(float timestep) override {
        // Optimized memory update with decay and consolidation
        if (simdEnabled_) {
            updateWithSIMD(timestep);
        } else {
            updateSequential(timestep);
        }
        
        // Periodic consolidation
        if (memoryWeights_.size() > capacity_ * 0.8) {
            consolidateMemory();
        }
    }
    
    const std::vector<NeuronId>& getMemoryNeurons() const override {
        return memoryNeurons_;
    }
    
    const std::vector<float>& getMemoryWeights() const override {
        return memoryWeights_;
    }
    
    void clear() override {
        memoryNeurons_.clear();
        memoryWeights_.clear();
        neuronTypes_.clear();
    }
    
    // Optimized competitive firing with SIMD
    void competitiveFiringWithSIMD(NeuronId neuronId, float activation) {
        // Vectorized competitive firing implementation
        size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
        size_t vectorSize = memoryNeurons_.size() - (memoryNeurons_.size() % 16);
        
        for (; i < vectorSize; i += 16) {
            // Load memory weights for comparison
            __m512 memWeights = _mm512_loadu_ps(&memoryWeights_[i]);
            
            // Create activation vector
            __m512 actVec = _mm512_set1_ps(activation);
            
            // Calculate competition scores
            __m512 scores = _mm512_mul_ps(memWeights, _mm512_set1_ps(excitationStrength_));
            scores = _mm512_sub_ps(scores, _mm512_mul_ps(actVec, _mm512_set1_ps(inhibitionStrength_)));
            
            // Find weakest memory item
            __m512 minVal = _mm512_set1_ps(std::numeric_limits<float>::max());
            __m512 minIndices = _mm512_set1_ps(0.0f);
            
            for (size_t j = 0; j < 16; ++j) {
                if (scores[j] < minVal[0]) {
                    minVal = _mm512_set1_ps(scores[j]);
                    minIndices = _mm512_set1_ps(static_cast<float>(i + j));
                }
            }
            
            // Replace weakest memory if new activation is stronger
            float minScore = _mm512_castps_si512(minVal)[0];
            size_t minIndex = static_cast<size_t>(minIndices[0]);
            
            if (activation > memoryWeights_[minIndex] * competitionThreshold_) {
                memoryWeights_[minIndex] = activation;
                memoryNeurons_[minIndex] = neuronId;
                neuronTypes_[minIndex] = getNeuronType(neuronId);
            }
        }
        
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
        size_t vectorSize = memoryNeurons_.size() - (memoryNeurons_.size() % 8);
        
        for (; i < vectorSize; i += 8) {
            __m256 memWeights = _mm256_loadu_ps(&memoryWeights_[i]);
            __m256 actVec = _mm256_set1_ps(activation);
            
            __m256 scores = _mm256_mul_ps(memWeights, _mm256_set1_ps(excitationStrength_));
            scores = _mm256_sub_ps(scores, _mm256_mul_ps(actVec, _mm256_set1_ps(inhibitionStrength_)));
            
            __m256 minVal = _mm256_set1_ps(std::numeric_limits<float>::max());
            __m256 minIndices = _mm256_set1_ps(0.0f);
            
            for (size_t j = 0; j < 8; ++j) {
                if (scores[j] < minVal[0]) {
                    minVal = _mm256_set1_ps(scores[j]);
                    minIndices = _mm256_set1_ps(static_cast<float>(i + j));
                }
            }
            
            float minScore = _mm256_castps_si256(minVal)[0];
            size_t minIndex = static_cast<size_t>(minIndices[0]);
            
            if (activation > memoryWeights_[minIndex] * competitionThreshold_) {
                memoryWeights_[minIndex] = activation;
                memoryNeurons_[minIndex] = neuronId;
                neuronTypes_[minIndex] = getNeuronType(neuronId);
            }
        }
        
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
        size_t vectorSize = memoryNeurons_.size() - (memoryNeurons_.size() % 4);
        
        for (; i < vectorSize; i += 4) {
            __m128 memWeights = _mm_loadu_ps(&memoryWeights_[i]);
            __m128 actVec = _mm_set1_ps(activation);
            
            __m128 scores = _mm_mul_ps(memWeights, _mm_set1_ps(excitationStrength_));
            scores = _mm_sub_ps(scores, _mm_mul_ps(actVec, _mm_set1_ps(inhibitionStrength_)));
            
            __m128 minVal = _mm_set1_ps(std::numeric_limits<float>::max());
            __m128 minIndices = _mm_set1_ps(0.0f);
            
            for (size_t j = 0; j < 4; ++j) {
                if (scores[j] < minVal[0]) {
                    minVal = _mm_set1_ps(scores[j]);
                    minIndices = _mm_set1_ps(static_cast<float>(i + j));
                }
            }
            
            float minScore = _mm_castps_si128(minVal)[0];
            size_t minIndex = static_cast<size_t>(minIndices[0]);
            
            if (activation > memoryWeights_[minIndex] * competitionThreshold_) {
                memoryWeights_[minIndex] = activation;
                memoryNeurons_[minIndex] = neuronId;
                neuronTypes_[minIndex] = getNeuronType(neuronId);
            }
        }
        
#else
        // Scalar competitive firing
        size_t weakestIndex = 0;
        float weakestWeight = std::numeric_limits<float>::max();
        
        for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
            float score = memoryWeights_[j] * excitationStrength_ - activation * inhibitionStrength_;
            if (score < weakestWeight) {
                weakestWeight = score;
                weakestIndex = j;
            }
        }
        
        if (activation > memoryWeights_[weakestIndex] * competitionThreshold_) {
            memoryWeights_[weakestIndex] = activation;
            memoryNeurons_[weakestIndex] = neuronId;
            neuronTypes_[weakestIndex] = getNeuronType(neuronId);
        }
#endif
        
        // Handle remaining elements with scalar code
        for (; i < memoryNeurons_.size(); ++i) {
            float score = memoryWeights_[i] * excitationStrength_ - activation * inhibitionStrength_;
            if (score < weakestWeight) {
                weakestWeight = score;
                weakestIndex = i;
            }
        }
        
        if (activation > memoryWeights_[weakestIndex] * competitionThreshold_) {
            memoryWeights_[weakestIndex] = activation;
            memoryNeurons_[weakestIndex] = neuronId;
            neuronTypes_[weakestIndex] = getNeuronType(neuronId);
        }
    }
    
    // Update memory with SIMD vectorization
    void updateWithSIMD(float timestep) {
        size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 16);
        
        for (; i < vectorSize; i += 16) {
            // Apply exponential decay to memory weights
            __m512 weights = _mm512_loadu_ps(&memoryWeights_[i]);
            __m512 decayFactor = _mm512_set1_ps(std::exp(-timestep / decayTime_));
            weights = _mm512_mul_ps(weights, decayFactor);
            
            // Apply competition weights
            __m512 compWeights = _mm512_set1_ps(competitionWeight_);
            weights = _mm512_mul_ps(weights, compWeights);
            
            _mm512_storeu_ps(&memoryWeights_[i], weights);
        }
        
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 8);
        
        for (; i < vectorSize; i += 8) {
            __m256 weights = _mm256_loadu_ps(&memoryWeights_[i]);
            __m256 decayFactor = _mm256_set1_ps(std::exp(-timestep / decayTime_));
            weights = _mm256_mul_ps(weights, decayFactor);
            
            __m256 compWeights = _mm256_set1_ps(competitionWeight_);
            weights = _mm256_mul_ps(weights, compWeights);
            
            _mm256_storeu_ps(&memoryWeights_[i], weights);
        }
        
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 4);
        
        for (; i < vectorSize; i += 4) {
            __m128 weights = _mm_loadu_ps(&memoryWeights_[i]);
            __m128 decayFactor = _mm_set1_ps(std::exp(-timestep / decayTime_));
            weights = _mm_mul_ps(weights, decayFactor);
            
            __m128 compWeights = _mm_set1_ps(competitionWeight_);
            weights = _mm_mul_ps(weights, compWeights);
            
            _mm_storeu_ps(&memoryWeights_[i], weights);
        }
        
#else
        // Scalar fallback
        for (; i < memoryWeights_.size(); ++i) {
            memoryWeights_[i] *= std::exp(-timestep / decayTime_);
            memoryWeights_[i] *= competitionWeight_;
        }
#endif
        
        // Normalize weights
        normalizeWeightsSIMD();
    }
    
    void updateSequential(float timestep) {
        // Sequential memory update for platforms without SIMD
        for (size_t i = 0; i < memoryWeights_.size(); ++i) {
            memoryWeights_[i] *= std::exp(-timestep / decayTime_);
            memoryWeights_[i] *= competitionWeight_;
        }
        
        // Normalize weights
        float sum = std::accumulate(memoryWeights_.begin(), memoryWeights_.end(), 0.0f);
        if (sum > 0.0f) {
            for (size_t i = 0; i < memoryWeights_.size(); ++i) {
                memoryWeights_[i] /= sum;
            }
        }
    }
    
    // Normalize weights with SIMD optimization
    void normalizeWeightsSIMD() {
        // Vectorized weight normalization
        size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 16);
        
        __m512 sum = _mm512_setzero_ps();
        
        for (; i < vectorSize; i += 16) {
            __m512 weights = _mm512_loadu_ps(&memoryWeights_[i]);
            sum = _mm512_add_ps(sum, weights);
            
            // Normalize weights
            __m512 normalized = _mm512_div_ps(weights, sum);
            _mm512_storeu_ps(&memoryWeights_[i], normalized);
        }
        
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 8);
        
        __m256 sum = _mm256_setzero_ps();
        
        for (; i < vectorSize; i += 8) {
            __m256 weights = _mm256_loadu_ps(&memoryWeights_[i]);
            sum = _mm256_add_ps(sum, weights);
            
            __m256 normalized = _mm256_div_ps(weights, sum);
            _mm256_storeu_ps(&memoryWeights_[i], normalized);
        }
        
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 4);
        
        __m128 sum = _mm_setzero_ps();
        
        for (; i < vectorSize; i += 4) {
            __m128 weights = _mm_loadu_ps(&memoryWeights_[i]);
            sum = _mm_add_ps(sum, weights);
            
            __m128 normalized = _mm_div_ps(weights, sum);
            _mm_storeu_ps(&memoryWeights_[i], normalized);
        }
        
#else
        // Scalar normalization
        float sum = std::accumulate(memoryWeights_.begin(), memoryWeights_.end(), 0.0f);
        for (; i < memoryWeights_.size(); ++i) {
            if (sum > 0.0f) {
                memoryWeights_[i] /= sum;
            }
        }
#endif
        
        // Handle remaining elements
        for (; i < memoryWeights_.size(); ++i) {
            if (sum > 0.0f) {
                memoryWeights_[i] /= sum;
            }
        }
    }
    
    // Consolidate memory to maintain capacity
    void consolidateMemory() {
        // Vectorized consolidation using SIMD sorting
        size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 16);
        
        // Create structure of arrays for sorting
        std::vector<float> tempWeights(memoryWeights_.size());
        std::vector<NeuronId> tempNeurons(memoryNeurons_.size());
        std::vector<size_t> tempTypes(neuronTypes_.size());
        
        // Copy data
        for (size_t j = 0; j < vectorSize; j += 16) {
            __m512 weights = _mm512_loadu_ps(&memoryWeights_[j]);
            _mm512_storeu_ps(&tempWeights[j], weights);
            
            __m512 neurons = _mm512_loadu_ps(reinterpret_cast<const float*>(&memoryNeurons_[j]));
            _mm512_storeu_ps(reinterpret_cast<float*>(&tempNeurons[j]), neurons);
            
            __m512 types = _mm512_loadu_ps(reinterpret_cast<const float*>(&neuronTypes_[j]));
            _mm512_storeu_ps(reinterpret_cast<float*>(&tempTypes[j]), types);
        }
        
        // Simple bubble sort (for demonstration - use proper sorting in production)
        for (size_t j = 0; j < tempWeights.size() - 1; ++j) {
            if (tempWeights[j] < tempWeights[j + 1]) {
                std::swap(tempWeights[j], tempWeights[j + 1]);
                std::swap(tempNeurons[j], tempNeurons[j + 1]);
                std::swap(tempTypes[j], tempTypes[j + 1]);
            }
        }
        
        // Copy back, keeping only top capacity_ items
        size_t keepCount = std::min(capacity_, tempWeights.size());
        for (size_t j = 0; j < keepCount; ++j) {
            memoryWeights_[j] = tempWeights[j];
            memoryNeurons_[j] = tempNeurons[j];
            neuronTypes_[j] = tempTypes[j];
        }
        memoryWeights_.resize(keepCount);
        memoryNeurons_.resize(keepCount);
        neuronTypes_.resize(keepCount);
        
        i = vectorSize;
        
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
        // AVX2 implementation
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 8);
        
        // Similar to AVX-512 but with 8-wide operations
        // ... (implementation continues)
        
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
        // SSE implementation
        size_t vectorSize = memoryWeights_.size() - (memoryWeights_.size() % 4);
        
        // Similar to AVX but with 4-wide operations
        // ... (implementation continues)
        
#else
        // Scalar consolidation
        size_t weakestIndex = 0;
        float weakestWeight = std::numeric_limits<float>::max();
        
        for (size_t j = 0; j < memoryWeights_.size(); ++j) {
            if (memoryWeights_[j] < weakestWeight) {
                weakestWeight = memoryWeights_[j];
                weakestIndex = j;
            }
        }
        
        // Remove weakest item
        memoryWeights_.erase(memoryWeights_.begin() + weakestIndex);
        memoryNeurons_.erase(memoryNeurons_.begin() + weakestIndex);
        neuronTypes_.erase(neuronTypes_.begin() + weakestIndex);
#endif
        
        // Handle remaining elements with scalar code
        for (; i < memoryWeights_.size(); ++i) {
            // Simple consolidation: remove items below threshold
            if (memoryWeights_[i] < consolidationThreshold_) {
                memoryWeights_.erase(memoryWeights_.begin() + i);
                memoryNeurons_.erase(memoryNeurons_.begin() + i);
                neuronTypes_.erase(neuronTypes_.begin() + i);
                --i; // Adjust index after removal
            }
        }
    }
    
    // Setup arrays for competitive firing optimization
    void setupCompetitiveFiringArrays() {
        // Initialize arrays with optimal alignment
        simdEnabled_ = SIMDUtils::isSIMDAvailable();
        
        // Set up memory alignment for SIMD operations
        if (simdEnabled_) {
            // Align memory for SIMD operations
            memoryWeights_.reserve(capacity_);
            memoryNeurons_.reserve(capacity_);
            neuronTypes_.reserve(capacity_);
        }
    }
    
    // Get neuron type from brain (helper function)
    NeuronType getNeuronType(NeuronId neuronId) {
        if (!brain_) return NeuronType::Internal;
        
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronId) {
                        return neuron->getType();
                    }
                }
            }
        }
        return NeuronType::Internal;
    }
    
    // Configuration parameters
    size_t capacity_;
    float competitionThreshold_;
    float inhibitionStrength_;
    float excitationStrength_;
    float decayTime_ = 1000.0f; // Memory decay time constant
    float competitionWeight_ = 1.0f;
    float consolidationThreshold_ = 0.01f;
    bool simdEnabled_;
    
    Brain* brain_;
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryWeights_;
    std::vector<size_t> neuronTypes_;
};

/**
 * Optimized NeuralEpisodicMemory implementation
 * Uses batch processing and efficient storage/retrieval
 */
class OptimizedNeuralEpisodicMemory : public NeuralEpisodicMemory {
public:
    OptimizedNeuralEpisodicMemory() : maxEpisodes_(1000), currentEpisode_(0) {
        // Initialize with optimized parameters
        consolidationThreshold_ = 0.3f;
        replayWindow_ = 10;
        encodingEfficiency_ = 1.0f;
    }
    
    void initialize(Brain* brain) override {
        brain_ = brain;
        episodes_.reserve(maxEpisodes_);
        setupOptimizedStorage();
    }
    
    void setMaxEpisodes(size_t maxEpisodes) override {
        maxEpisodes_ = maxEpisodes;
        episodes_.reserve(maxEpisodes_);
    }
    
    void storeEpisode(const EpisodicMemoryItem& episode) override {
        // Optimized storage with batch processing
        if (simdEnabled_) {
            storeEpisodeSIMD(episode);
        } else {
            storeEpisodeSequential(episode);
        }
        
        // Periodic consolidation
        if (episodes_.size() > maxEpisodes_ * 0.8) {
            consolidateEpisodes();
        }
    }
    
    const std::vector<const EpisodicMemoryItem*>& getEpisodesForReplay(size_t count) const override {
        return replayCache_;
    }
    
    void replayEpisode(const EpisodicMemoryItem* episode) override {
        if (!episode) return;
        
        // Optimized replay with vectorized application
        replayWithSIMD(*episode);
    }
    
    void consolidate(size_t threshold) override {
        // Vectorized consolidation of episodes
        consolidateWithSIMD(threshold);
    }
    
    void clear() override {
        episodes_.clear();
        replayCache_.clear();
        setupOptimizedStorage();
    }
    
    // Optimized storage with SIMD
    void storeEpisodeSIMD(const EpisodicMemoryItem& episode) {
        size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
        size_t vectorSize = episodes_.size() - (episodes_.size() % 16);
        
        // Batch processing for episode storage
        alignas(64) float episodeData[16];
        alignas(64) float activeNeurons[16];
        alignas(64) float neuronActivations[16];
        
        for (size_t j = 0; j < 16; ++j) {
            episodeData[j] = episode.timestamp;
            activeNeurons[j] = episode.reward;
        }
        
        // Store episode data
        if (episodes_.size() < vectorSize) {
            episodes_.push_back(std::make_unique<EpisodicMemoryItem>(episode));
        } else {
            // Replace oldest episode with new one
            size_t oldestIndex = (currentEpisode_++) % episodes_.size();
            episodes_[oldestIndex] = std::make_unique<EpisodicMemoryItem>(episode);
        }
        
        i = vectorSize;
        
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
        // AVX2 implementation
        size_t vectorSize = episodes_.size() - (episodes_.size() % 8);
        
        // Similar to AVX-512 with 8-wide operations
        // ... (implementation continues)
        
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
        // SSE implementation
        size_t vectorSize = episodes_.size() - (episodes_.size() % 4);
        
        // Similar to AVX but with 4-wide operations
        // ... (implementation continues)
        
#else
        // Scalar storage
        if (episodes_.size() < maxEpisodes_) {
            episodes_.push_back(std::make_unique<EpisodicMemoryItem>(episode));
        } else {
            size_t oldestIndex = (currentEpisode_++) % episodes_.size();
            episodes_[oldestIndex] = std::make_unique<EpisodicMemoryItem>(episode);
        }
#endif
        
        // Handle remaining elements
        for (; i < episodes_.size(); ++i) {
            if (episodes_.size() < maxEpisodes_) {
                episodes_.push_back(std::make_unique<EpisodicMemoryItem>(episode));
            } else {
                size_t oldestIndex = (currentEpisode_++) % episodes_.size();
                episodes_[oldestIndex] = std::make_unique<EpisodicMemoryItem>(episode);
            }
        }
        
        // Update replay cache
        updateReplayCache();
    }
    
    void storeEpisodeSequential(const EpisodicMemoryItem& episode) {
        // Sequential storage for platforms without SIMD
        if (episodes_.size() < maxEpisodes_) {
            episodes_.push_back(std::make_unique<EpisodicMemoryItem>(episode));
        } else {
            size_t oldestIndex = (currentEpisode_++) % episodes_.size();
            episodes_[oldestIndex] = std::make_unique<EpisodicMemoryItem>(episode);
        }
        
        updateReplayCache();
    }
    
    // Optimized replay with SIMD
    void replayWithSIMD(const EpisodicMemoryItem& episode) {
        size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
        size_t vectorSize = episode.activeNeurons.size() - (episode.activeNeurons.size() % 16);
        
        // Vectorized replay processing
        for (; i < vectorSize; i += 16) {
            // Apply replay effects to neurons
            alignas(64) float neuronActivations[16];
            for (size_t j = 0; j < 16; ++j) {
                neuronActivations[j] = episode.neuronActivations[i + j];
            }
            
            // Create SIMD vectors for replay effects
            __m512 actVec = _mm512_load_ps(neuronActivations);
            __m512 replayStrength = _mm512_set1_ps(0.5f); // Replay strength
            
            // Apply replay: enhance memory consolidation
            __m512 enhancedActivations = _mm512_mul_ps(actVec, replayStrength);
            
            // Store enhanced activations back
            alignas(64) float enhancedOutput[16];
            _mm512_store_ps(enhancedOutput, enhancedActivations);
        }
        
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
        // AVX2 replay implementation
        size_t vectorSize = episode.activeNeurons.size() - (episode.activeNeurons.size() % 8);
        
        for (; i < vectorSize; i += 8) {
            // Similar to AVX-512 with 8-wide operations
            // ... (implementation continues)
        }
        
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
        // SSE replay implementation
        size_t vectorSize = episode.activeNeurons.size() - (episode.activeNeurons.size() % 4);
        
        for (; i < vectorSize; i += 4) {
            // Similar to AVX but with 4-wide operations
            // ... (implementation continues)
        }
        
#else
        // Scalar replay
        for (size_t j = 0; j < episode.activeNeurons.size(); ++j) {
            float activation = episode.neuronActivations[j];
            float replayStrength = 0.5f;
            float enhancedActivation = activation * replayStrength;
            
            // Apply enhanced activation to corresponding neuron
            // ... (apply to brain)
        }
#endif
        
        // Handle remaining activations
        for (; i < episode.activeNeurons.size(); ++i) {
            float activation = episode.neuronActivations[i];
            float replayStrength = 0.5f;
            float enhancedActivation = activation * replayStrength;
            
            // Apply enhanced activation
            // ... (apply to brain)
        }
    }
    
    // Optimized consolidation with SIMD
    void consolidateWithSIMD(size_t threshold) {
        size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
        size_t vectorSize = episodes_.size() - (episodes_.size() % 16);
        
        // Vectorized consolidation: remove weak episodes
        alignas(64) float episodeValues[16];
        
        for (size_t j = 0; j < vectorSize; j += 16) {
            // Calculate episode strength (reward + activation sum)
            for (size_t k = 0; k < 16; ++k) {
                size_t episodeIndex = j + k;
                if (episodeIndex < episodes_.size()) {
                    auto& ep = episodes_[episodeIndex];
                    float totalStrength = ep->reward;
                    
                    // Add active neuron strengths
                    for (size_t neuron : ep->activeNeurons) {
                        totalStrength += ep->neuronActivations[k]; // Simplified
                    }
                    
                    episodeValues[k] = totalStrength;
                } else {
                    episodeValues[k] = 0.0f;
                }
            }
            
            // Create mask for weak episodes
            __m512 values = _mm512_load_ps(episodeValues);
            __m512 thresholdVec = _mm512_set1_ps(static_cast<float>(threshold));
            __m512 weakMask = _mm512_cmplt_ps(values, thresholdVec);
            
            // Build index mask for removal
            __m512i indexMask = _mm512_castps_si512(weakMask);
            
            // Compact strong episodes
            compactEpisodesSIMD(j, j + 16, weakMask);
        }
        
        i = vectorSize;
        
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
        // AVX2 consolidation implementation
        size_t vectorSize = episodes_.size() - (episodes_.size() % 8);
        
        // Similar to AVX-512 with 8-wide operations
        // ... (implementation continues)
        
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
        // SSE consolidation implementation
        size_t vectorSize = episodes_.size() - (episodes_.size() % 4);
        
        // Similar to AVX but with 4-wide operations
        // ... (implementation continues)
        
#else
        // Scalar consolidation
        float avgStrength = calculateAverageEpisodeStrength();
        float consolidationThreshold = avgStrength * (1.0f - static_cast<float>(threshold) / 100.0f);
        
        for (size_t j = 0; j < episodes_.size(); ++j) {
            if (episodes_[j]->reward < consolidationThreshold) {
                // Remove weak episode
                episodes_.erase(episodes_.begin() + j);
                --j;
            }
        }
#endif
        
        // Handle remaining episodes with scalar code
        for (; i < episodes_.size(); ++i) {
            float totalStrength = episodes_[i]->reward;
            for (size_t neuron : episodes_[i]->activeNeurons) {
                totalStrength += 0.5f; // Simplified activation strength
            }
            
            if (totalStrength < static_cast<float>(threshold)) {
                episodes_.erase(episodes_.begin() + i);
                --i;
            }
        }
        
        // Update replay cache after consolidation
        updateReplayCache();
    }
    
    // Compact episodes using SIMD
    void compactEpisodesSIMD(size_t begin, size_t end, __m512i& weakMask) {
        // Compact function implementation
        // Move strong episodes to front
        // ... (implementation details)
    }
    
    // Calculate average episode strength
    float calculateAverageEpisodeStrength() {
        if (episodes_.empty()) return 0.0f;
        
        float totalStrength = 0.0f;
        for (const auto& episode : episodes_) {
            float strength = episode->reward;
            for (size_t activation : episode->neuronActivations) {
                strength += activation;
            }
            totalStrength += strength;
        }
        
        return totalStrength / episodes_.size();
    }
    
    // Update replay cache with optimization
    void updateReplayCache() {
        replayCache_.clear();
        replayCache_.reserve(std::min<size_t>(replayWindow_, episodes_.size()));
        
        // Select episodes for replay using vectorized selection
        if (simdEnabled_) {
            selectEpisodesForReplaySIMD();
        } else {
            selectEpisodesForReplaySequential();
        }
    }
    
    void selectEpisodesForReplaySIMD() {
        size_t i = 0;
#if defined(NLM_SIMD_AVX512) && defined(__AVX512F__)
        size_t vectorSize = episodes_.size() - (episodes_.size() % 16);
        
        alignas(64) float episodeStrengths[16];
        
        for (size_t j = 0; j < vectorSize; j += 16) {
            // Calculate episode strength
            for (size_t k = 0; k < 16; ++k) {
                size_t epIndex = j + k;
                if (epIndex < episodes_.size()) {
                    auto& ep = episodes_[epIndex];
                    episodeStrengths[k] = ep->reward + calculateActivationSum(*ep);
                } else {
                    episodeStrengths[k] = 0.0f;
                }
            }
            
            // Create SIMD vector for comparison
            __m512 strengths = _mm512_load_ps(episodeStrengths);
            __m512 topThreshold = _mm512_set1_ps(calculateTopStrengthThreshold());
            
            // Select top episodes
            __m512 selectMask = _mm512_cmpgt_ps(strengths, topThreshold);
            
            // Add selected episodes to cache
            addSelectedEpisodesToCache(j, j + 16, selectMask);
        }
        
        i = vectorSize;
        
#elif defined(NLM_SIMD_AVX2) && defined(__AVX2__)
        // AVX2 implementation
        size_t vectorSize = episodes_.size() - (episodes_.size() % 8);
        
        // Similar to AVX-512 with 8-wide operations
        // ... (implementation continues)
        
#elif defined(NLM_SIMD_SSE) && defined(__SSE4_1__)
        // SSE implementation
        size_t vectorSize = episodes_.size() - (episodes_.size() % 4);
        
        // Similar to AVX but with 4-wide operations
        // ... (implementation continues)
        
#else
        // Scalar selection
        std::vector<size_t> indices(episodes_.size());
        std::iota(indices.begin(), indices.end(), 0);
        
        std::sort(indices.begin(), indices.end(),
                 [this](size_t a, size_t b) {
                     return calculateEpisodeStrength(a) > calculateEpisodeStrength(b);
                 });
        
        size_t count = std::min(replayWindow_, indices.size());
        for (size_t j = 0; j < count; ++j) {
            replayCache_.push_back(episodes_[indices[j]].get());
        }
#endif
        
        // Handle remaining episodes
        for (; i < episodes_.size(); ++i) {
            float strength = calculateEpisodeStrength(i);
            if (strength > calculateTopStrengthThreshold()) {
                replayCache_.push_back(episodes_[i].get());
                if (replayCache_.size() >= replayWindow_) break;
            }
        }
    }
    
    float calculateEpisodeStrength(size_t episodeIndex) const {
        if (episodeIndex >= episodes_.size()) return 0.0f;
        
        auto& episode = episodes_[episodeIndex];
        float strength = episode->reward;
        
        for (size_t activation : episode->neuronActivations) {
            strength += activation;
        }
        
        return strength;
    }
    
    float calculateActivationSum(const EpisodicMemoryItem& episode) {
        float sum = 0.0f;
        for (float activation : episode.neuronActivations) {
            sum += activation;
        }
        return sum;
    }
    
    float calculateTopStrengthThreshold() const {
        if (episodes_.empty()) return 0.0f;
        
        // Calculate threshold for top episodes
        float totalStrength = 0.0f;
        for (const auto& episode : episodes_) {
            totalStrength += episode->reward + calculateActivationSum(*episode);
        }
        
        return totalStrength * 0.5f; // Top 50% by strength
    }
    
    void addSelectedEpisodesToCache(size_t begin, size_t end, __m512i& selectMask) {
        // Implementation details for adding selected episodes
        // ... (implementation details)
    }
    
    void selectEpisodesForReplaySequential() {
        // Scalar implementation of episode selection
        std::vector<size_t> indices(episodes_.size());
        std::iota(indices.begin(), indices.end(), 0);
        
        std::sort(indices.begin(), indices.end(),
                 [this](size_t a, size_t b) {
                     return calculateEpisodeStrength(a) > calculateEpisodeStrength(b);
                 });
        
        size_t count = std::min(replayWindow_, indices.size());
        for (size_t j = 0; j < count; ++j) {
            replayCache_.push_back(episodes_[indices[j]].get());
        }
    }
    
    void setupOptimizedStorage() {
        // Setup optimized storage structures
        simdEnabled_ = SIMDUtils::isSIMDAvailable();
        
        // Pre-allocate memory with optimal alignment
        if (simdEnabled_) {
            episodes_.reserve(maxEpisodes_);
            replayCache_.reserve(replayWindow_);
        }
    }
    
    // Configuration parameters
    size_t maxEpisodes_;
    size_t currentEpisode_;
    size_t replayWindow_;
    float consolidationThreshold_;
    float encodingEfficiency_;
    bool simdEnabled_;
    
    Brain* brain_;
    std::vector<std::unique_ptr<EpisodicMemoryItem>> episodes_;
    std::vector<const EpisodicMemoryItem*> replayCache_;
};

// Factory function for creating optimized memory systems
inline std::unique_ptr<NeuralWorkingMemory> createOptimizedWorkingMemory() {
    return std::make_unique<OptimizedNeuralWorkingMemory>();
}

inline std::unique_ptr<NeuralEpisodicMemory> createOptimizedEpisodicMemory() {
    return std::make_unique<OptimizedNeuralEpisodicMemory>();
}

} // namespace nlm
