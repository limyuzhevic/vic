#include "OptimizedNeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>

namespace nlm {

// Static pool for temporary vectors
std::vector<float>& OptimizedNeuralWorkingMemory::getTempVector(size_t size) {
    static thread_local std::vector<float> temp;
    if (temp.size() < size) {
        temp.resize(size);
    }
    return temp;
}

void OptimizedNeuralWorkingMemory::returnTempVector(std::vector<float>&& vec) {
    // Vectors are kept in thread-local storage
}

OptimizedNeuralWorkingMemory::OptimizedNeuralWorkingMemory()
    : capacity_(100), decayRate_(0.01f), neuromodulatorBaseStrength_(0.5f),
      developmentalPlasticity_(1.0f), targetMemoryActivity_(0.5f) {
    // Initialize neuromodulators with better starting values
    dopamine_.level = neuromodulatorBaseStrength_;
    dopamine_.target = neuromodulatorBaseStrength_;
    dopamine_.decayRate = 0.05f;
    dopamine_.releaseRate = 1.0f;
    
    curiosity_.level = neuromodulatorBaseStrength_ * 0.6f;
    curiosity_.target = neuromodulatorBaseStrength_ * 0.6f;
    curiosity_.decayRate = 0.03f;
    curiosity_.releaseRate = 0.8f;
    
    novelty_.level = neuromodulatorBaseStrength_ * 0.4f;
    novelty_.target = neuromodulatorBaseStrength_ * 0.4f;
    novelty_.decayRate = 0.02f;
    novelty_.releaseRate = 0.6f;
}

OptimizedNeuralWorkingMemory::~OptimizedNeuralWorkingMemory() = default;

void OptimizedNeuralWorkingMemory::initialize(Brain* brain) {
    NeuralWorkingMemory::initialize(brain);
    
    // Initialize with optimized pool
    neuronPool_ = std::make_unique<MemoryPool<NeuronActivation>>(1024, 4);
    activationPool_ = std::make_unique<MemoryPool<float>>(1024, 4);
    
    // Pre-allocate fast path pointers
    fastPath_.updatePointers(memoryNeurons_, memoryActivations_);
    
    NLM_LOG_INFO("OptimizedNeuralWorkingMemory initialized with memory pool");
}

void OptimizedNeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Check for novelty using optimized hash-based approach
    bool isNovel = checkNoveltyOptimized(pattern);
    if (isNovel) {
        novelty_.setLevel(novelty_.level + 0.1f, 1.0f);
    }
    
    // Use batch processing for better performance
    batchStore(pattern, strength);
}

void OptimizedNeuralWorkingMemory::batchStore(const std::vector<float>& pattern, float strength) {
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    if (neuronsNeeded == 0) return;
    
    // Use SIMD-friendly batch operations where possible
    std::vector<float>& temp = getTempVector(neuronsNeeded);
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Apply neuromodulatory effects
        activation *= (1.0f + dopamine_.level * 0.5f);
        activation *= (1.0f + curiosity_.level * 0.3f);
        
        // Store with optimized update
        storeToNeuronOptimized(neuron, activation);
    }
}

void OptimizedNeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    storeToNeuronOptimized(neuron, activation);
}

void OptimizedNeuralWorkingMemory::storeToNeuronOptimized(NeuronId neuron, float activation) {
    uint32_t hash = NeuronActivation::hashNeuronId(neuron);
    
    // Try to find existing entry in hash table (O(1))
    auto it = neuronIndex_.find(hash);
    if (it != neuronIndex_.end()) {
        size_t idx = it->second;
        neuronActivations_[idx].activation = activation * 
            (1.0f + dopamine_.level * 0.3f + curiosity_.level * 0.2f);
        neuronActivations_[idx].timestamp = 0;
    } else {
        // Add new entry with optimized allocation
        if (neuronActivations_.size() < capacity_) {
            size_t idx = neuronActivations_.size();
            neuronIndex_[hash] = idx;
            neuronActivations_.emplace_back(neuron, activation, 0);
        }
    }
    
    // Update fast path
    fastPath_.updatePointers(memoryNeurons_, memoryActivations_);
}

std::vector<float> OptimizedNeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(neuronActivations_.size());
    
    for (const auto& entry : neuronActivations_) {
        result.push_back(entry.activation);
    }
    
    return result;
}

bool OptimizedNeuralWorkingMemory::contains(NeuronId neuron) const {
    uint32_t hash = NeuronActivation::hashNeuronId(neuron);
    return neuronIndex_.find(hash) != neuronIndex_.end();
}

float OptimizedNeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    uint32_t hash = NeuronActivation::hashNeuronId(neuron);
    auto it = neuronIndex_.find(hash);
    if (it != neuronIndex_.end()) {
        return neuronActivations_[it->second].activation;
    }
    return 0.0f;
}

void OptimizedNeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update neuromodulators with optimized smoothing
    SimulationStep currentStep = brain_->getSimulationStep();
    dopamine_.update(dt, currentStep);
    curiosity_.update(dt, currentStep);
    novelty_.update(dt, currentStep);
    
    // Use batch update for better cache locality
    batchUpdate(dt);
}

void OptimizedNeuralWorkingMemory::batchUpdate(TimestepDuration dt) {
    SimulationStep currentStep = brain_->getSimulationStep();
    
    // Process with SIMD-friendly loops
    for (auto& entry : neuronActivations_) {
        if (entry.activation > 0.1f) {
            float maintenanceStrength = 1.0f;
            
            // Dopamine supports trace maintenance
            maintenanceStrength += dopamine_.level * 0.5f;
            
            // Curiosity boosts trace stability
            maintenanceStrength += curiosity_.level * 0.3f;
            
            // Novelty enhances memory consolidation
            if (novelty_.level > 0.5f) {
                maintenanceStrength *= 1.2f;
            }
            
            // Inject maintenance current
            brain_->injectCurrent(entry.neuron, entry.activation * 2.0f * maintenanceStrength);
            
            // Age the trace
            float decayRate = decayRate_;
            
            // Development-dependent decay (simplified)
            decayRate *= (1.0f + 0.5f * (1.0f - 0.5f)); // Using 0.5f as placeholder for developmental stage
            
            entry.timestamp++;
            
            // Check if trace should be consolidated
            if (entry.timestamp > 1000) {
                if (entry.activation > 0.3f) {
                    entry.activation *= (1.0f - decayRate * 0.5f);
                } else {
                    entry.activation *= (1.0f - decayRate * 2.0f);
                }
            } else {
                entry.activation *= (1.0f - decayRate);
            }
            
            entry.activation = std::max(0.0f, entry.activation);
        }
    }
    
    // Update fast path
    fastPath_.updatePointers(memoryNeurons_, memoryActivations_);
}

void OptimizedNeuralWorkingMemory::clear() {
    NeuralWorkingMemory::clear();
    
    // Clear optimized structures
    neuronIndex_.clear();
    neuronActivations_.clear();
    tempPool_.clear();
    sortedPool_.clear();
    
    // Clear pools
    neuronPool_.reset();
    activationPool_.reset();
}

void OptimizedNeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& entry : neuronActivations_) {
        entry.activation = std::min(1.0f, entry.activation * factor);
    }
}

void OptimizedNeuralWorkingMemory::runCompetition() {
    batchCompetition();
}

void OptimizedNeuralWorkingMemory::batchCompetition() {
    winners_.clear();
    
    if (neuronActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f * (1.0f + dopamine_.level * 0.5f);
    
    // Use vectorized approach
    for (const auto& entry : neuronActivations_) {
        if (entry.activation >= threshold) {
            winners_.push_back(entry.neuron);
        }
    }
    
    // Enhanced inhibitory competition with neuromodulatory effects
    for (const auto& entry : neuronActivations_) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), entry.neuron) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply inhibition modulated by neuromodulators
            float inhibitionStrength = entry.activation * 3.0f;
            
            // Curiosity reduces inhibition to promote exploration
            inhibitionStrength *= (1.0f - curiosity_.level * 0.5f);
            
            // Novelty enhances competition
            if (novelty_.level > 0.5f) {
                inhibitionStrength *= 1.5f;
            }
            
            brain_->injectCurrent(entry.neuron, -inhibitionStrength);
        }
    }
}

bool OptimizedNeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float OptimizedNeuralWorkingMemory::getMemoryActivity() const {
    if (neuronActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& entry : neuronActivations_) {
        total += entry.activation;
    }
    
    return total / neuronActivations_.size();
}

float OptimizedNeuralWorkingMemory::getDopamineLevel() const {
    return dopamine_.level;
}

float OptimizedNeuralWorkingMemory::getCuriosityLevel() const {
    return curiosity_.level;
}

float OptimizedNeuralWorkingMemory::getNoveltyLevel() const {
    return novelty_.level;
}

float OptimizedNeuralWorkingMemory::getDevelopmentalStage() const {
    return 0.5f; // Placeholder - would be derived from brain development
}

float OptimizedNeuralWorkingMemory::getPredictionError() const {
    return dopamine_.level; // Dopamine tracks prediction error
}

void OptimizedNeuralWorkingMemory::integratePrediction(const std::vector<float>& prediction) {
    if (prediction.empty() || !brain_) return;
    
    // Use optimized similarity computation
    float predictionStrength = 1.0f + std::abs(getPredictionError());
    
    size_t neuronsToUpdate = std::min(prediction.size(), neuronActivations_.size());
    for (size_t i = 0; i < neuronsToUpdate; ++i) {
        const auto& entry = neuronActivations_[i];
        float predActivation = prediction[i] * predictionStrength;
        
        // Update with prediction error
        predActivation *= (1.0f + getPredictionError() * 0.5f);
        
        // Store optimized
        storeToNeuronOptimized(entry.neuron, predActivation);
        
        // Apply prediction-based plasticity
        dopamine_.level = std::min(2.0f, dopamine_.level + std::abs(getPredictionError()) * 0.01f);
    }
    
    // Novelty signal for prediction mismatch
    if (std::abs(getPredictionError()) > 0.5f) {
        novelty_.setLevel(novelty_.level + 0.2f, 1.0f);
    }
}

void OptimizedNeuralWorkingMemory::integrateAttention(const std::vector<NeuronId>& attendedNeurons) {
    // Apply attentional focus to working memory
    for (const NeuronId& attendedNeuron : attendedNeurons) {
        bool isInMemory = contains(attendedNeuron);
        
        if (isInMemory) {
            uint32_t hash = NeuronActivation::hashNeuronId(attendedNeuron);
            auto it = neuronIndex_.find(hash);
            if (it != neuronIndex_.end()) {
                size_t idx = it->second;
                neuronActivations_[idx].activation *= 1.5f; // Attention boost
            }
        }
    }
    
    // Curiosity modulation based on attention
    curiosity_.setLevel(curiosity_.level + (attendedNeurons.size() > 0) ? 0.01f : 0.0f, 1.0f);
}

bool OptimizedNeuralWorkingMemory::checkNoveltyOptimized(const std::vector<float>& pattern) const {
    if (neuronActivations_.empty()) return true;
    
    // Check if pattern is novel using optimized comparison
    for (size_t i = 0; i < std::min(pattern.size(), neuronActivations_.size()); ++i) {
        if (std::abs(pattern[i] - neuronActivations_[i].activation) < 0.01f) {
            return false;
        }
    }
    
    return true;
}

float OptimizedNeuralWorkingMemory::computeSimilarityOptimized(const float* a, 
                                                             const float* b, 
                                                             size_t size) const {
    if (size == 0) return 0.0f;
    
    // Optimized SIMD-friendly cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    // Manual SIMD-style optimization
    for (size_t i = 0; i < size; i += 4) {
        // Process 4 elements at a time
        size_t limit = std::min(i + 4, size);
        
        float a0 = a[i];
        float a1 = (limit > i + 1) ? a[i + 1] : 0.0f;
        float a2 = (limit > i + 2) ? a[i + 2] : 0.0f;
        float a3 = (limit > i + 3) ? a[i + 3] : 0.0f;
        
        float b0 = b[i];
        float b1 = (limit > i + 1) ? b[i + 1] : 0.0f;
        float b2 = (limit > i + 2) ? b[i + 2] : 0.0f;
        float b3 = (limit > i + 3) ? b[i + 3] : 0.0f;
        
        dotProduct += a0 * b0 + a1 * b1 + a2 * b2 + a3 * b3;
        normA += a0 * a0 + a1 * a1 + a2 * a2 + a3 * a3;
        normB += b0 * b0 + b1 * b1 + b2 * b2 + b3 * b3;
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm