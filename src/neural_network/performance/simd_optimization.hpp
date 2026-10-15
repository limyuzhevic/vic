#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {

/**
 * SIMD Vectorization for Performance Optimizations
 * 
 * SIMD-accelerated neural network operations:
 * - Vectorized spike propagation
 * - Vectorized synaptic computations
 * - Vectorized neuron state updates
 * - Vectorized plasticity calculations
 * - Vectorized network metrics
 * 
 * Optimizes neural network computations using SIMD instructions
 */
class SIMDVectorization {
public:
    SIMDVectorization();
    ~SIMDVectorization();

    // ===== SIMD OPERATORS =====

    // SIMD operations for neural computation
    class SIMDOperators {
    public:
        // Vectorized operations for neuron states
        struct NeuronStateVector {
            std::vector<float> membranePotential;     // Membrane potentials (mV)
            std::vector<float> threshold;            // Thresholds (mV)
            std::vector<float> restingPotential;     // Resting potentials (mV)
            std::vector<float> resetPotential;       // Reset potentials (mV)
            std::vector<float> leakConductance;       // Leak conductances (nS)
            std::vector<float> firingRate;           // Firing rates (Hz)
            std::vector<uint8_t> firingState;       // Firing states
            std::vector<uint32_t> refractoryRemaining; // Remaining refractory steps
            std::vector<uint32_t> refractoryPeriod;  // Refractory periods (steps)
            std::vector<float> lastSpikeTime;       // Last spike times (ms)
            std::vector<uint64_t> neuronType;        // Neuron types
        };

        // Vectorized operations for synaptic states
        struct SynapseStateVector {
            std::vector<uint64_t> sourceNeuron;     // Source neuron IDs
            std::vector<uint64_t> destinationNeuron; // Destination neuron IDs
            std::vector<float> weight;              // Synaptic weights
            std::vector<uint32_t> delay;            // Delays (steps)
            std::vector<uint8_t> synapseType;       // Synapse types
            std::vector<uint8_t> plasticityFlags;    // Plasticity flags
            std::vector<float> eligibilityTrace;    // Eligibility traces
            std::vector<float> efficacy;            // Synaptic efficacies
            std::vector<float> shortTermDepression; // Short-term depression
            std::vector<float> shortTermFacilitation; // Short-term facilitation
            std::vector<float> preSpikeHistory;     // Pre-synaptic spike history
            std::vector<float> postSpikeHistory;    // Post-synaptic spike history
        };

        // Vectorized operations for plasticity
        struct PlasticityVector {
            std::vector<float> hebbianWeightChange;  // Hebbian weight changes
            std::vector<float> stdpWeightChange;    // STDP weight changes
            std::vector<float> rewardWeightChange;  // Reward-modulated weight changes
            std::vector<float> homeostaticWeightChange; // Homeostatic weight changes
            std::vector<float> neuromodulatoryWeightChange; // Neuromodulatory weight changes
        };

        // Vectorized neuron state operations
        static void vectorizedLIFStep(NeuronStateVector& state, const std::vector<float>& inputs,
                                    const std::vector<float>& currents, TimestepDuration dt);

        static void vectorizedNeuronThresholdCheck(const NeuronStateVector& state,
                                                 std::vector<bool>& firing, Timestamp currentTime);

        static void vectorizedMembranePotentialUpdate(NeuronStateVector& state,
                                                    const std::vector<float>& inputs, float dt);

        static void vectorizedRefractoryUpdate(NeuronStateVector& state, float dt);

        static void vectorizedAdaptationUpdate(NeuronStateVector& state,
                                             const std::vector<bool>& firing, float dt);

        static void vectorizedShortTermPlasticityUpdate(SynapseStateVector& state,
                                                      const std::vector<bool>& preSpike,
                                                      const std::vector<bool>& postSpike, float dt);

        static void vectorizedSTDPUpdate(const SynapseStateVector& preSpikeHistory,
                                      const SynapseStateVector& postSpikeHistory,
                                      const std::vector<uint64_t>& sourceNeurons,
                                      const std::vector<uint64_t>& destNeurons,
                                      PlasticityVector& weightChanges, float dt);

        static void vectorizedHebbianUpdate(const std::vector<float>& preActivities,
                                         const std::vector<float>& postActivities,
                                         PlasticityVector& weightChanges, float dt);

        static void vectorizedRewardModulatedUpdate(const std::vector<float>& rewards,
                                                 const std::vector<float>& eligibilityTraces,
                                                 PlasticityVector& weightChanges, float dt);

        static void vectorizedHomeostaticUpdate(const std::vector<float>& firingRates,
                                              const std::vector<float>& targetRates,
                                              PlasticityVector& weightChanges, float dt);

        static void vectorizedNeuromodulatoryUpdate(const std::vector<float>& neuromodulators,
                                                 PlasticityVector& weightChanges, float dt);

        // Vectorized network operations
        static void vectorizedSpikePropagation(const SynapseStateVector& synapseState,
                                             const NeuronStateVector& neuronState,
                                             std::vector<std::vector<float>>& spikePropagation);

        static void vectorizedNetworkMetrics(const NeuronStateVector& neuronState,
                                           const SynapseStateVector& synapseState,
                                           std::vector<float>& metrics);

        // Vectorized memory operations
        static void vectorizedMemoryCopy(NeuronStateVector& dest, const NeuronStateVector& src);
        static void vectorizedMemoryZero(NeuronStateVector& state);
        static void vectorizedMemoryTranspose(std::vector<std::vector<float>>& matrix);
    };

    // ===== SIMD ACCELERATORS =====

    // SIMD accelerators for different operations
    class SIMDAccelerators {
    public:
        // Neuron accelerators
        struct NeuronAccelerators {
            // Vectorized LIF accelerators
            class LIFAccelerator {
            public:
                static void accelerateLIFStep(std::vector<float>& v_mem, const std::vector<float>& I_syn,
                                            const std::vector<float>& I_inj, const std::vector<float>& tau,
                                            const std::vector<float>& v_reset, const std::vector<float>& v_thresh,
                                            const std::vector<float>& g_leak, std::vector<bool>& spiked,
                                            TimestepDuration dt);

                static void accelerateNeuronThresholdCheck(const std::vector<float>& v_mem,
                                                         const std::vector<float>& v_thresh,
                                                         std::vector<bool>& spiked);

                static void accelerateMembranePotentialUpdate(std::vector<float>& v_mem,
                                                            const std::vector<float>& I_total,
                                                            const std::vector<float>& tau, float dt);
            };

            // Vectorized synaptic accelerators
            class SynapseAccelerator {
            public:
                static void accelerateSynapticTransmission(const std::vector<float>& weights,
                                                          const std::vector<bool>& preSpike,
                                                          std::vector<float>& postSynaptic);

                static void accelerateShortTermPlasticity(std::vector<float>& u, std::vector<float>& x,
                                                        const std::vector<bool>& preSpike,
                                                        const std::vector<bool>& postSpike, float dt);

                static void accelerateSTDP(const std::vector<float>& preTraces,
                                         const std::vector<float>& postTraces,
                                         std::vector<float>& weightChanges, float dt);
            };

            // Vectorized plasticity accelerators
            class PlasticityAccelerator {
            public:
                static void accelerateHebbianLearning(const std::vector<float>& preActivity,
                                                    const std::vector<float>& postActivity,
                                                    std::vector<float>& weightChanges, float dt);

                static void accelerateRewardModulatedLearning(const std::vector<float>& reward,
                                                            const std::vector<float>& eligibility,
                                                            std::vector<float>& weightChanges, float dt);

                static void accelerateHomeostaticLearning(const std::vector<float>& firingRates,
                                                         const std::vector<float>& targetRates,
                                                         std::vector<float>& weightChanges, float dt);
            };
        };

        // Network accelerators
        struct NetworkAccelerators {
            // Spike propagation accelerators
            class SpikePropagationAccelerator {
            public:
                static void accelerateSparseSpikePropagation(const std::vector<size_t>& adjacencyList,
                                                            const std::vector<float>& weights,
                                                            const std::vector<bool>& neuronStates,
                                                            std::vector<float>& postSynaptic);

                static void accelerateDenseSpikePropagation(const std::vector<std::vector<float>>& connectivity,
                                                          const std::vector<bool>& neuronStates,
                                                          std::vector<float>& postSynaptic);
            };

            // Metrics computation accelerators
            class MetricsAccelerator {
            public:
                static void accelerateFiringRateCalculation(const std::vector<bool>& spiked,
                                                          std::vector<float>& firingRates, float dt);

                static void accelerateConnectionMetrics(const std::vector<std::vector<float>>& connectivity,
                                                      std::vector<float>& metrics);

                static void accelerateNetworkEfficiency(const std::vector<std::vector<float>>& connectivity,
                                                      std::vector<float>& efficiency);
            };
        };
    };

    // ===== CACHE-OPTIMIZED LAYOUT =====

    // Cache-optimized data layout for SIMD operations
    class CacheOptimizedLayout {
    public:
        // Structure-of-Arrays (SoA) layout
        struct SOA 
        {
            std::vector<float> membranePotential;    // All neurons' membrane potentials
            std::vector<float> threshold;           // All neurons' thresholds
            std::vector<float> restingPotential;    // All neurons' resting potentials
            std::vector<float> resetPotential;      // All neurons' reset potentials
            std::vector<float> leakConductance;      // All neurons' leak conductances
            std::vector<float> firingRate;          // All neurons' firing rates
            std::vector<uint8_t> firingState;       // All neurons' firing states
            std::vector<uint32_t> refractoryRemaining; // All neurons' refractory remaining
            std::vector<uint32_t> refractoryPeriod;  // All neurons' refractory periods
            std::vector<float> lastSpikeTime;       // All neurons' last spike times
            std::vector<uint64_t> neuronType;       // All neurons' types
            
            // Active neuron tracking
            std::vector<uint32_t> activeNeuronIndices;  // Indices of active neurons
            std::vector<uint32_t> firingNeuronIndices;  // Indices of firing neurons
            
            void initialize(size_t size);
            void clear();
            size_t memoryUsage() const;
        };

        // Column-major layout for matrix operations
        struct ColumnMajorLayout {
            std::vector<std::vector<float>> data;   // Data in column-major format
            
            void initialize(size_t rows, size_t cols);
            void clear();
            float getElement(size_t row, size_t col) const;
            void setElement(size_t row, size_t col, float value);
        };

        // Cache-aligned structures
        struct CacheAligned {
            alignas(64) std::vector<float> alignedMembranePotential;
            alignas(64) std::vector<float> alignedThreshold;
            alignas(64) std::vector<float> alignedCurrent;
            alignas(64) std::vector<float> alignedWeight;
            
            void initialize(size_t size);
            void clear();
        };
    };

    // ===== SIMD SPECIALIZATION =====

    // SIMD specialization for different neuron types
    class SIMDSpecialization {
    public:
        // LIF-specific SIMD operations
        struct LIFSpecialization {
            static void specializedLIFStep(std::vector<float>& v_mem, const std::vector<float>& I_syn,
                                         const std::vector<float>& I_inj, const std::vector<float>& tau,
                                         const std::vector<float>& v_reset, const std::vector<float>& v_thresh,
                                         const std::vector<float>& g_leak, std::vector<bool>& spiked,
                                         TimestepDuration dt);

            static void specializedNeuronThresholdCheck(const std::vector<float>& v_mem,
                                                      const std::vector<float>& v_thresh,
                                                      std::vector<bool>& spiked);

            static void specializedRefractoryUpdate(std::vector<uint32_t>& refractoryRemaining,
                                                   const std::vector<bool>& spiked, float dt);
        };

        // AdEx-specific SIMD operations
        struct AdexSpecialization {
            static void specializedAdExStep(std::vector<float>& v_mem, std::vector<float>& w,
                                          const std::vector<float>& I_syn,
                                          const std::vector<float>& I_inj,
                                          const std::vector<float>& delta_v,
                                          const std::vector<float>& tau_mem,
                                          const std::vector<float>& C_m,
                                          const std::vector<float>& a,
                                          const std::vector<float>& b,
                                          const std::vector<float>& tau_w,
                                          const std::vector<float>& E_L,
                                          std::vector<bool>& spiked, TimestepDuration dt);
        };

        // Stochastic-specific SIMD operations
        struct StochasticSpecialization {
            static void specializedStochasticStep(std::vector<float>& v_mem,
                                                 const std::vector<float>& I_syn,
                                                 const std::vector<float>& noise,
                                                 const std::vector<float>& threshold,
                                                 const std::vector<float>& reset,
                                                 std::vector<bool>& spiked, TimestepDuration dt);

            static void specializedBurstingStep(std::vector<bool>& inBurst,
                                               const std::vector<float>& v_mem,
                                               const std::vector<float>& v_thresh,
                                               std::vector<uint32_t>& burstCounter, float dt);
        };

        // Plasticity-specific SIMD operations
        struct PlasticitySpecialization {
            static void specializedSTDPStep(const std::vector<float>& preTraces,
                                          const std::vector<float>& postTraces,
                                          std::vector<float>& weightChanges, float dt);

            static void specializedHebbianStep(const std::vector<float>& preActivity,
                                              const std::vector<float>& postActivity,
                                              std::vector<float>& weightChanges, float dt);

            static void specializedRewardModulatedStep(const std::vector<float>& reward,
                                                     const std::vector<float>& eligibility,
                                                     std::vector<float>& weightChanges, float dt);
        };
    };

    // ===== PERFORMANCE MONITORING =====

    // SIMD performance monitoring
    struct SIMDPerformanceMetrics {
        double vectorizationEfficiency;     // How well vectorization is utilized
        double simdInstructionUtilization;  // Utilization of SIMD instructions
        double memoryBandwidthUtilization;  // Memory bandwidth utilization
        double cacheHitRate;                 // Cache hit rate
        double instructionThroughput;       // Instructions per cycle
        double floatingPointOperations;      // FLOPs
        double memoryAccesses;               // Memory accesses
    };

    SIMDPerformanceMetrics getPerformanceMetrics() const { return metrics_; }

    // Update performance metrics
    void updatePerformanceMetrics(const SIMDOperators::NeuronStateVector& neuronState,
                                 const SIMDOperators::SynapseStateVector& synapseState,
                                 double executionTime);

    // ===== OVERRIDE =====

    // Override Brain for brain integration
    class IntegratedSIMDVectorization : public Brain {
    public:
        IntegratedSIMDVectorization(std::shared_ptr<Config> config,
                                   std::shared_ptr<SIMDVectorization> vectorization);
        ~IntegratedSIMDVectorization() override;

        // Initialize SIMD vectorization
        bool initialize() override;

        // Enable/disable SIMD optimizations
        void enableSIMDOptimizations(bool enable) { simdEnabled_ = enable; }
        bool isSIMDEnabled() const { return simdEnabled_; }

        // Set SIMD vector size
        void setSIMDVectorSize(size_t size) { vectorSize_ = size; }
        size_t getSIMDVectorSize() const { return vectorSize_; }

        // Get current vectorization level
        size_t getCurrentVectorizationLevel() const { return vectorizationLevel_; }

        // Update SIMD vectorization
        void updateSIMDVectorization(TimestepDuration dt);

        // Get performance metrics
        SIMDPerformanceMetrics getSIMDPerformanceMetrics() const;

    private:
        std::shared_ptr<SIMDVectorization> vectorization_;
        bool simdEnabled_;
        size_t vectorSize_;
        size_t vectorizationLevel_;
        SIMDPerformanceMetrics currentMetrics_;
    };

private:
    // Configuration
    bool simdEnabled_;
    size_t vectorSize_;
    size_t vectorizationLevel_;

    // Performance metrics
    SIMDPerformanceMetrics metrics_;

    // Cache-optimized layouts
    CacheOptimizedLayout::SOA soaLayout_;
    CacheOptimizedLayout::ColumnMajorLayout columnMajorLayout_;
    CacheOptimizedLayout::CacheAligned cacheAlignedLayout_;

    // Helper methods
    size_t calculateOptimalVectorSize() const;
    bool checkSIMDAvailability() const;
    void setupSIMDInstructions();
    void optimizeCacheLayout();
    void profileSIMDPerformance(double executionTime);
};

} // namespace nlm
