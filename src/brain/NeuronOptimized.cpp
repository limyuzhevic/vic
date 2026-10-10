// NeuronOptimized.cpp
// Optimized neuron implementation with memory pools and SIMD optimizations

#include "Neuron.hpp"
#include "../core/memory_pool/MemoryPool.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Optimized neuron implementation using memory pools
// Uses Structure-of-Arrays (SoA) layout for better SIMD access
class NeuronOptimized {
public:
    struct OptimizedNeuronData {
        alignas(64) float membranePotential;
        alignas(64) float restingPotential;
        alignas(64) float threshold;
        alignas(64) float resetPotential;
        alignas(64) float leakConductance;
        alignas(64) uint32_t refractoryRemaining;
        alignas(64) uint32_t refractoryPeriod;
        alignas(64) float adaptationVariable;
        alignas(64) float lastSpikeTime;
        alignas(64) uint32_t id;
        alignas(64) float synapticInput;
        alignas(64) uint8_t firingState;
        alignas(64) NeuronType type;
        alignas(64) RegionId regionId;
        alignas(64) PopulationId populationId;
    };
    
    // Memory pool for optimized neurons
    class NeuronMemoryPool {
    public:
        NeuronMemoryPool(size_t capacity = 1024) {
            reserve(capacity);
        }
        
        size_t allocate() {
            size_t index;
            if (!m_freeList.empty()) {
                index = m_freeList.back();
                m_freeList.pop_back();
            } else {
                if (m_size >= m_capacity) {
                    reserve(m_capacity * 2);
                }
                index = m_size++;
            }
            
            m_used[index] = true;
            return index;
        }
        
        void deallocate(size_t index) {
            if (index < m_used.size() && m_used[index]) {
                m_used[index] = false;
                m_freeList.push_back(index);
            }
        }
        
        OptimizedNeuronData& getData(size_t index) {
            return m_neurons[index];
        }
        
        const OptimizedNeuronData& getData(size_t index) const {
            return m_neurons[index];
        }
        
        size_t getCapacity() const { return m_capacity; }
        size_t getSize() const { return m_size; }
        
        void clear() {
            m_freeList.clear();
            std::fill(m_used.begin(), m_used.end(), false);
            m_size = 0;
        }
        
    private:
        void reserve(size_t additionalCapacity) {
            size_t newCapacity = m_capacity + additionalCapacity;
            m_neurons.reserve(newCapacity);
            m_used.reserve(newCapacity);
            m_neurons.resize(newCapacity);
            m_used.resize(newCapacity, false);
            m_capacity = newCapacity;
        }
        
        std::vector<OptimizedNeuronData> m_neurons;
        std::vector<bool> m_used;
        std::vector<size_t> m_freeList;
        size_t m_capacity = 0;
        size_t m_size = 0;
    };
    
    NeuronOptimized() {
        m_pool.reserve(1024);
    }
    
    size_t createNeuron() {
        return m_pool.allocate();
    }
    
    void deleteNeuron(size_t index) {
        m_pool.deallocate(index);
    }
    
    // Optimized stepLIF with SIMD-friendly operations
    bool stepLIF(size_t neuronIndex, Timestamp currentTime, TimestepDuration dt) {
        OptimizedNeuronData& n = m_pool.getData(neuronIndex);
        
        // Fast path: check refractory period
        if (n.refractoryRemaining > 0) {
            --n.refractoryRemaining;
            if (n.refractoryRemaining == 0) {
                n.firingState = static_cast<uint8_t>(FiringState::Resting);
            }
            n.synapticInput = 0.0f;
            return false;
        }
        
        // LIF computation with optimized arithmetic
        float& V = n.membranePotential;
        const float V_rest = n.restingPotential;
        const float V_reset = n.resetPotential;
        const float threshold = n.threshold;
        const float tau = Impl::TIME_CONSTANT;
        const float C = Impl::MEMBRANE_CAPACITANCE;
        float& adaptation = n.adaptationVariable;
        
        // Synaptic contribution (using local variable for speed)
        float synapticContribution = n.synapticInput / C;
        
        // Leak contribution
        float leakContribution = (V_rest - V) / tau;
        
        // Combined update to minimize operations
        V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
        
        // Adaptation with conditional optimization
        if (adaptation > 0.0f) {
            V -= adaptation * 0.01f;
            adaptation = adaptation * 0.95f;
        }
        
        // Threshold check with early exit
        if (V >= threshold) {
            // Neuron fires
            n.firingState = static_cast<uint8_t>(FiringState::Active);
            n.lastSpikeTime = static_cast<float>(currentTime);
            n.refractoryRemaining = n.refractoryPeriod;
            n.firingState = static_cast<uint8_t>(FiringState::Refractory);
            n.adaptationVariable += 1.0f;
            V = V_reset;
            
            // Record spike in local history buffer
            recordSpike(neuronIndex, currentTime);
            
            return true;
        }
        
        n.firingState = static_cast<uint8_t>(FiringState::Active);
        n.synapticInput = 0.0f;
        return false;
    }
    
    void recordSpike(size_t neuronIndex, Timestamp timestamp) {
        // Use circular buffer for spike history
        OptimizedNeuronData& n = m_pool.getData(neuronIndex);
        // Store recent spike times
        if (m_spikeHistory.size() < MAX_SPIKE_HISTORY) {
            m_spikeHistory.push_back({neuronIndex, timestamp});
        } else {
            m_spikeHistory[spikeHistoryIndex] = {neuronIndex, timestamp};
        }
        ++spikeHistoryIndex;
        if (spikeHistoryIndex >= MAX_SPIKE_HISTORY) {
            spikeHistoryIndex = 0;
        }
    }
    
    // Get spike history for plasticity
    const std::vector<std::pair<size_t, Timestamp>>& getSpikeHistory() const {
        return m_spikeHistory;
    }
    
    // Set neuron properties
    void setNeuronProperties(size_t neuronIndex, 
                             NeuronType type,
                             RegionId region,
                             PopulationId population) {
        OptimizedNeuronData& n = m_pool.getData(neuronIndex);
        n.type = type;
        n.regionId = region;
        n.populationId = population;
        n.firingState = static_cast<uint8_t>(FiringState::Resting);
        n.refractoryRemaining = 0;
        n.adaptationVariable = 0.0f;
        n.lastSpikeTime = -1.0f;
    }
    
    // Batch process multiple neurons for better performance
    void batchProcess(size_t startIndex, size_t count,
                      Timestamp currentTime, TimestepDuration dt) {
        // Process a batch of neurons for better cache locality
        for (size_t i = 0; i < count; ++i) {
            size_t idx = startIndex + i;
            stepLIF(idx, currentTime, dt);
        }
    }
    
    void clear() {
        m_pool.clear();
        m_spikeHistory.clear();
    }
    
private:
    NeuronMemoryPool m_pool;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    std::vector<std::pair<size_t, Timestamp>> m_spikeHistory;
    size_t spikeHistoryIndex = 0;
};

// Factory function to create optimized brain with memory pools
NeuronOptimized* createOptimizedBrain(size_t neuronCount) {
    NeuronOptimized* brain = new NeuronOptimized();
    
    // Pre-allocate neurons
    for (size_t i = 0; i < neuronCount; ++i) {
        size_t idx = brain->createNeuron();
        // Initialize with biological parameters
        brain->setNeuronProperties(idx, NeuronType::Internal, 
                                   RegionId(i % 4 + 1), 
                                   PopulationId(i % 2 + 1));
        
        // Set LIF parameters
        OptimizedNeuronData& n = brain->getNeuronData(idx);
        n.membranePotential = -70.0f;
        n.restingPotential = -70.0f;
        n.threshold = -55.0f;
        n.resetPotential = -70.0f;
        n.leakConductance = 10.0f;
        n.refractoryPeriod = 5;
        n.adaptationVariable = 0.0f;
        n.lastSpikeTime = -1.0f;
    }
    
    return brain;
}

// Performance benchmark for optimized neurons
void benchmarkNeuronPerformance() {
    // Create optimized brain
    NeuronOptimized* brain = createOptimizedBrain(10000);
    
    // Simulate 1000 steps
    Timestamp startTime = 0.0f;
    Timestamp endTime = 1000.0f;
    Timestamp currentTime = startTime;
    TimestepDuration dt = 0.001f;
    
    // Process steps in batches for better performance
    size_t batchSize = 100;
    for (size_t step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 10000; i += batchSize) {
            brain->batchProcess(i, std::min(batchSize, 10000U - i), 
                               currentTime, dt);
        }
        currentTime += dt;
    }
    
    // Clean up
    delete brain;
}

} // namespace nlm
