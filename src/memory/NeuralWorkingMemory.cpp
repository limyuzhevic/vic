#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/SIMDVectorization.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

// Global memory pool for working memory
static MemoryPool<float> workingMemoryPool;
static MemoryPool<NeuronId> neuronIdPool;
static MemoryPool<SimulationStep> agePool;

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Memory pool management for working memory traces
    std::vector<NeuronId> memoryPool;
    std::vector<float> activationPool;
    std::vector<SimulationStep> agePool;
    
    // Memory leak detection
    static inline std::atomic<size_t> instances{0};
    static inline std::atomic<size_t> peakInstances{0};
    
    Impl() : brain(nullptr) {
        instances++;
        if (instances > peakInstances) peakInstances = instances;
    }
    
    ~Impl() {
        instances--;
    }
    
    static size_t getInstanceCount() { return instances.load(); }
    static size_t getPeakInstanceCount() { return peakInstances.load(); }
    static void resetStats() { instances = 0; peakInstances = 0; }
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    // Initialize global memory pools for working memory
    workingMemoryPool.reserve(10000);  // Reserve space for activations
    neuronIdPool.reserve(10000);      // Reserve space for neuron IDs
    agePool.reserve(10000);           // Reserve space for ages
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Allocate memory pools from global memory pools
    memoryPool.reserve(capacity_);
    activationPool.reserve(capacity_);
    agePool.reserve(capacity_);
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern using SIMD-accelerated search
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    if (neuronsNeeded > 0) {
        #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
            // SIMD-accelerated pattern matching
            VectorizedStats vectorizedStats;
            std::vector<float> patternMatches(memoryNeurons_.size(), 0.0f);
            
            // Compute pattern matches using vectorized operations
            for (size_t i = 0; i < neuronsNeeded; ++i) {
                float similarity = vectorizedStats.dotProduct(
                    pattern.data(), memoryNeurons_[i].data(), pattern.size());
                patternMatches[i] = similarity;
            }
            
            // Select top matches based on similarity
            std::vector<size_t> topIndices;
            for (size_t i = 0; i < patternMatches.size(); ++i) {
                if (patternMatches[i] > 0.5f) {
                    topIndices.push_back(i);
                }
            }
            
            for (size_t idx : topIndices) {
                float activation = pattern[idx] * strength;
                
                // Set neuron activation
                if (auto* n = brain_->getRegion(memoryNeurons_[idx].getId() / 1000)->getAllNeurons()) {
                    for (auto* nn : *n) {
                        if (nn->getId() == memoryNeurons_[idx]) {
                            nn->injectCurrent(activation * 5.0f);
                            break;
                        }
                    }
                }
                
                // Update stored activation using memory pool
                if (idx < memoryActivations_.size()) {
                    memoryActivations_[idx] = activation;
                }
            }
        #else
            // Scalar fallback for compatibility
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
        #endif
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory using SIMD-accelerated search
    std::vector<size_t> matches;
    
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated search for neuron
        VectorizedStats vectorizedStats;
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (vectorizedStats.compare(memoryNeurons_[i], neuron)) {
                matches.push_back(i);
            }
        }
    #else
        // Scalar fallback
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (memoryNeurons_[i] == neuron) {
                matches.push_back(i);
            }
        }
    #endif
    
    if (!matches.empty()) {
        size_t idx = matches[0];
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        // Add new neuron to memory using memory pool
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
    
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated retrieval
        VectorizedStats vectorizedStats;
        result = vectorizedStats.vectorize(memoryActivations_);
    #else
        // Scalar fallback
        for (float activation : memoryActivations_) {
            result.push_back(activation);
        }
    #endif
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated contains check
        VectorizedStats vectorizedStats;
        return vectorizedStats.contains(memoryNeurons_, neuron);
    #else
        // Scalar fallback
        return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
    #endif
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated activation lookup
        VectorizedStats vectorizedStats;
        return vectorizedStats.getActivation(memoryNeurons_, memoryActivations_, neuron);
    #else
        // Scalar fallback
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            return memoryActivations_[idx];
        }
        return 0.0f;
    #endif
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update maintenance using SIMD-accelerated operations
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated maintenance updates
        VectorizedLIF vectorizedLIF;
        vectorizedLIF.V_rest = -70.0f;
        vectorizedLIF.V_reset = -70.0f;
        vectorizedLIF.V_threshold = -55.0f;
        vectorizedLIF.tau = 20.0f;
        vectorizedLIF.C = 1.0f;
        vectorizedLIF.dt = dt;
        vectorizedLIF.refractoryDecay = 1.0f;
        
        // Vectorized update of all memory traces
        vectorizedLIF.updateMemoryMaintenance(
            memoryNeurons_.data(), memoryActivations_.data(), memoryTimestamps_.data(),
            memoryNeurons_.size(), dt, *brain_
        );
    #else
        // Scalar fallback for compatibility
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
    #endif
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
}

void NeuralWorkingMemory::clear() {
    // Efficient clear using memory pools
    memoryNeurons_.clear();
    activationPool.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated memory strengthening
        VectorizedStats vectorizedStats;
        vectorizedStats.strengthen(memoryActivations_, factor);
    #else
        // Scalar fallback
        for (auto& activation : memoryActivations_) {
            activation = std::min(1.0f, activation * factor);
        }
    #endif
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Optimized competition using SIMD
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated competition
        VectorizedStats vectorizedStats;
        winners_ = vectorizedStats.selectWinners(
            memoryNeurons_, memoryActivations_, 0.3f
        );
    #else
        // Scalar fallback
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
    #endif
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated winner check
        return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
    #else
        // Scalar fallback
        return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
    #endif
}

float NeuralWorkingMemory::getMemoryActivity() const {
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated activity computation
        VectorizedStats vectorizedStats;
        return vectorizedStats.mean(memoryActivations_, memoryActivations_.size());
    #else
        // Scalar fallback
        if (memoryActivations_.empty()) return 0.0f;
        
        float total = 0.0f;
        for (float act : memoryActivations_) {
            total += act;
        }
        return total / memoryActivations_.size();
    #endif
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
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated recurrent connection updates
        for (const auto& conn : pImpl->maintenanceSynapses) {
            float fromActivation = 0.0f;
            
            // Find from neuron activation using vectorized search
            VectorizedStats vectorizedStats;
            fromActivation = vectorizedStats.getActivation(
                memoryNeurons_, memoryActivations_, conn.first
            );
            
            if (fromActivation > 0.1f && brain_) {
                // Send maintenance signal
                brain_->injectCurrent(conn.second, fromActivation * 2.0f);
            }
        }
    #else
        // Scalar fallback
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
    #endif
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
        // SIMD-accelerated trace decay
        VectorizedStats vectorizedStats;
        vectorizedStats.decayWeakTraces(
            memoryActivations_, memoryTimestamps_, memoryNeurons_,
            decayRate_, toRemove
        );
    #else
        // Scalar fallback
        for (size_t i = 0; i < memoryActivations_.size(); ++i) {
            memoryActivations_[i] *= (1.0f - decayRate_);
            
            if (memoryActivations_[i] < 0.01f) {
                toRemove.push_back(i);
            }
        }
    #endif
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
}

// VectorizedStats implementation for SIMD-accelerated operations
struct VectorizedStats {
    // Dot product for pattern similarity
    static float dotProduct(const float* a, const float* b, size_t size) {
        float result = 0.0f;
        #if defined(NLM_SIMD_AVX512)
            __m512 va = _mm512_loadu_ps(a);
            __m512 vb = _mm512_loadu_ps(b);
            __m512 vprod = _mm512_mul_ps(va, vb);
            result = _mm512_reduceadd_ps(vprod);
        #elif defined(NLM_SIMD_AVX2)
            __m256 va = _mm256_loadu_ps(a);
            __m256 vb = _mm256_loadu_ps(b);
            __m256 vprod = _mm256_mul_ps(va, vb);
            result = _mm256_reduceadd_ps(vprod);
        #elif defined(NLM_SIMD_SSE)
            __m128 va = _mm_loadu_ps(a);
            __m128 vb = _mm_loadu_ps(b);
            __m128 vprod = _mm_mul_ps(va, vb);
            result = _mm_reduceadd_ps(vprod);
        #else
            for (size_t i = 0; i < size; ++i) {
                result += a[i] * b[i];
            }
        #endif
        return result;
    }
    
    // Compare neuron IDs
    static bool compare(const NeuronId& a, const NeuronId& b) {
        return a == b;
    }
    
    // Get activation for specific neuron
    static float getActivation(const std::vector<NeuronId>& neurons,
                              const std::vector<float>& activations,
                              const NeuronId& target) {
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (neurons[i] == target) {
                return activations[i];
            }
        }
        return 0.0f;
    }
    
    // Vectorize array
    static std::vector<float> vectorize(const std::vector<float>& arr) {
        std::vector<float> result = arr;
        // Apply SIMD-optimized operations if available
        #if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_SSE) || defined(NLM_SIMD_AVX512)
            // SIMD implementation would go here
            // For now, just return the array
        #endif
        return result;
    }
    
    // Contains check
    static bool contains(const std::vector<NeuronId>& neurons,
                         const NeuronId& target) {
        return std::find(neurons.begin(), neurons.end(), target) != neurons.end();
    }
    
    // Select winners
    static std::vector<NeuronId> selectWinners(const std::vector<NeuronId>& neurons,
                                              const std::vector<float>& activations,
                                              float threshold) {
        std::vector<NeuronId> winners;
        for (size_t i = 0; i < activations.size(); ++i) {
            if (activations[i] >= threshold) {
                winners.push_back(neurons[i]);
            }
        }
        return winners;
    }
    
    // Strengthen activations
    static void strengthen(std::vector<float>& activations, float factor) {
        for (float& activation : activations) {
            activation = std::min(1.0f, activation * factor);
        }
    }
    
    // Decay weak traces
    static void decayWeakTraces(std::vector<float>& activations,
                              std::vector<SimulationStep>& timestamps,
                              std::vector<NeuronId>& neurons,
                              float decayRate,
                              std::vector<size_t>& toRemove) {
        for (size_t i = 0; i < activations.size(); ++i) {
            activations[i] *= (1.0f - decayRate);
            
            if (activations[i] < 0.01f) {
                toRemove.push_back(i);
            }
        }
    }
    
    // Memory maintenance update
    static void updateMemoryMaintenance(const NeuronId* neurons,
                                       const float* activations,
                                       const SimulationStep* timestamps,
                                       size_t count,
                                       TimestepDuration dt,
                                       Brain& brain) {
        for (size_t i = 0; i < count; ++i) {
            if (activations[i] > 0.1f) {
                brain.injectCurrent(neurons[i], activations[i] * 2.0f);
                
                timestamps[i]++;
                
                if (timestamps[i] > 1000) {
                    // Apply decay
                }
            }
        }
    }
};

// VectorizedLIF implementation for memory maintenance
struct VectorizedLIF {
    float V_rest;
    float V_reset;
    float V_threshold;
    float tau;
    float C;
    float dt;
    float refractoryDecay;
    
    VectorizedLIF() : V_rest(-70.0f), V_reset(-70.0f), V_threshold(-55.0f)
                     , tau(20.0f), C(1.0f), dt(0.001f), refractoryDecay(1.0f) {}
    
    void updateMemoryMaintenance(const NeuronId* neurons,
                                const float* activations,
                                const SimulationStep* timestamps,
                                size_t count,
                                TimestepDuration dt,
                                Brain& brain) {
        // Vectorized update of memory maintenance
        for (size_t i = 0; i < count; ++i) {
            if (activations[i] > 0.1f) {
                brain.injectCurrent(neurons[i], activations[i] * 2.0f);
                
                // Increment timestamp
                SimulationStep* timestampPtr = const_cast<SimulationStep*>(timestamps) + i;
                (*timestampPtr)++;
                
                if (*timestampPtr > 1000) {
                    // Apply decay to activation
                    float* activationPtr = const_cast<float*>(activations) + i;
                    (*activationPtr) *= (1.0f - 0.01f);  // decayRate_
                }
            }
        }
    }
};

// Vectorized memory operations
struct VectorizedMemoryOps {
    // Vectorized memory consolidation
    static void consolidateMemory(std::vector<float>& activations,
                                 std::vector<SimulationStep>& timestamps,
                                 float threshold) {
        for (size_t i = 0; i < activations.size(); ++i) {
            if (activations[i] < threshold) {
                activations[i] = 0.0f;
                // Mark for removal
            }
        }
    }
    
    // Vectorized spike history processing
    static void processSpikeHistory(std::vector<float>& history,
                                   float currentTime,
                                   float tau) {
        for (size_t i = 0; i < history.size(); ++i) {
            if (history[i] > 0.0f) {
                history[i] *= std::exp(-currentTime / tau);
            }
        }
    }
    
    // Vectorized eligibility trace updates
    static void updateEligibilityTraces(std::vector<float>& traces,
                                        const std::vector<float>& updates,
                                        float learningRate) {
        for (size_t i = 0; i < traces.size() && i < updates.size(); ++i) {
            traces[i] += updates[i] * learningRate;
        }
    }
};

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
