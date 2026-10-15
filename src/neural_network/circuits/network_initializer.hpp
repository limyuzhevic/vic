#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

/**
 * Network Initialization Strategies
 * 
 * Different strategies for initializing neural networks:
 * - Uniform random initialization
 * - Clustered initialization
 * - Gradient-based initialization
 * - Activity-driven initialization
 * - Structured initialization
 * - Hierarchical initialization
 * - Spatiotemporal initialization
 * - Adaptive initialization
 * - Dynamic initialization
 * 
 * Used to create initial network configurations
 */
class NetworkInitializer {
public:
    NetworkInitializer();
    ~NetworkInitializer();

    // ===== INITIALIZATION STRATEGIES =====

    // Initialization strategy types
    enum class InitializationStrategy {
        UNIFORM_RANDOM,           // Uniform random initialization
        CLUSTERED,                // Clustered initialization
        GRADIENT,                 // Gradient-based initialization
        ACTIVITY_DRIVEN,          // Activity-driven initialization
        STRUCTURED,               // Structured initialization
        HIERARCHICAL,             // Hierarchical initialization
        SPATIOTEMPORAL,           // Spatiotemporal initialization
        ADAPTIVE,                 // Adaptive initialization
        DYNAMIC,                  // Dynamic initialization
        BIOLOGICAL,               // Biologically plausible initialization
        EVOLUTIONARY,             // Evolutionary initialization
        INFORMATION_THEORETICAL,  // Information-theoretical initialization
        OPTIMIZATION_BASED,       // Optimization-based initialization
    };

    // ===== UNIFORM RANDOM INITIALIZATION =====

    // Uniform random initialization parameters
    struct UniformRandomParams {
        // Random parameters
        float mean;                // Mean of random distribution
        float stddev;              // Standard deviation of random distribution
        float min;                 // Minimum value
        float max;                 // Maximum value
        float skew;                // Skewness of distribution
        float kurtosis;            // Kurtosis of distribution
        
        // Initialization parameters
        bool normalize;            // Whether to normalize
        float normalizationFactor; // Normalization factor
        bool enforceBounds;        // Whether to enforce bounds
    };

    void setUniformRandomParams(const UniformRandomParams& params) { 
        uniformRandomParams_ = params; 
    }

    // ===== CLUSTERED INITIALIZATION =====

    // Clustered initialization parameters
    struct ClusteredParams {
        // Cluster parameters
        size_t numClusters;         // Number of clusters
        float clusterMean;          // Mean of cluster distribution
        float clusterStddev;        // Standard deviation of cluster distribution
        
        // Cluster organization
        float clusterSize;          // Size of clusters
        float interClusterDistance; // Distance between clusters
        float intraClusterDensity;  // Density within clusters
        float interClusterDensity;  // Density between clusters
        
        // Cluster-specific parameters
        float clusterStability;     // Cluster stability
        float clusterPlasticity;    // Cluster plasticity
        float clusterConnectivity;  // Cluster connectivity
    };

    void setClusteredParams(const ClusteredParams& params) { 
        clusteredParams_ = params; 
    }

    // ===== GRADIENT-BASED INITIALIZATION =====

    // Gradient-based initialization parameters
    struct GradientParams {
        // Gradient parameters
        float gradientStrength;     // Gradient strength
        float gradientDirection;    // Gradient direction (1D)
        float gradientSlope;        // Gradient slope
        float gradientIntercept;    // Gradient intercept
        
        // Gradient organization
        float spatialGradient;      // Spatial gradient
        float temporalGradient;     // Temporal gradient
        float functionalGradient;   // Functional gradient
        
        // Gradient-specific parameters
        float gradientNoise;        // Gradient noise
        float gradientTimeConstant; // Gradient time constant
        float gradientLearningRate; // Gradient learning rate
    };

    void setGradientParams(const GradientParams& params) { 
        gradientParams_ = params; 
    }

    // ===== ACTIVITY-DRIVEN INITIALIZATION =====

    // Activity-driven initialization parameters
    struct ActivityDrivenParams {
        // Activity parameters
        float activityThreshold;    // Activity threshold for initialization
        float activityLearningRate; // Learning rate for activity-driven initialization
        float activityMemory;       // Memory of activities
        float activityTimeConstant; // Time constant for activity dynamics
        float activityIntegration;  // Activity integration factor
        
        // Activity-specific parameters
        float activityHeterogeneity; // Heterogeneity of activities
        float activityCoherence;     // Coherence of activities
        float activitySynchronization; // Synchronization of activities
        float activityVariability;   // Variability of activities
    };

    void setActivityDrivenParams(const ActivityDrivenParams& params) { 
        activityDrivenParams_ = params; 
    }

    // ===== STRUCTURED INITIALIZATION =====

    // Structured initialization parameters
    struct StructuredParams {
        // Structure parameters
        float structuralRegularity;   // Regularity of structure
        float structuralPeriod;       // Period of structure
        float structuralAmplitude;    // Amplitude of structure
        float structuralPhase;        // Phase of structure
        
        // Structured organization
        bool structured;              // Whether structure is regular
        float structuredScale;        // Scale of structure
        float structuredRotation;     // Rotation of structure
        float structuredTranslation;  // Translation of structure
        
        // Structured-specific parameters
        float structuredComplexity;   // Complexity of structure
        float structuredSymmetry;     // Symmetry of structure
        float structuredAsymmetry;    // Asymmetry of structure
    };

    void setStructuredParams(const StructuredParams& params) { 
        structuredParams_ = params; 
    }

    // ===== HIERARCHICAL INITIALIZATION =====

    // Hierarchical initialization parameters
    struct HierarchicalParams {
        // Hierarchy parameters
        size_t levels;                // Number of hierarchy levels
        float levelDecay;              // Decay between levels
        float levelConnection;         // Connection strength between levels
        float levelExpansion;          // Expansion between levels
        
        // Hierarchical organization
        bool bidirectional;            // Whether hierarchy is bidirectional
        float feedbackStrength;        // Feedback strength in hierarchy
        float feedforwardStrength;     // Feedforward strength in hierarchy
        
        // Hierarchical-specific parameters
        float topLevelSize;            // Size of top level
        float bottomLevelSize;         // Size of bottom level
        float levelVariation;          // Variation between levels
    };

    void setHierarchicalParams(const HierarchicalParams& params) { 
        hierarchicalParams_ = params; 
    }

    // ===== SPATIOTEMPORAL INITIALIZATION =====

    // Spatiotemporal initialization parameters
    struct SpatiotemporalParams {
        // Spatial parameters
        float spatialScale;            // Spatial scale
        float spatialGranularity;     // Spatial granularity
        float spatialResolution;       // Spatial resolution
        float spatialExtent;           // Spatial extent
        
        // Temporal parameters
        float temporalScale;           // Temporal scale
        float temporalGranularity;    // Temporal granularity
        float temporalResolution;      // Temporal resolution
        float temporalDuration;        // Temporal duration
        
        // Spatiotemporal parameters
        float spatiotemporalCorrelation; // Spatiotemporal correlation
        float spatiotemporalCausality;  // Spatiotemporal causality
        float spatiotemporalIntegration; // Spatiotemporal integration
    };

    void setSpatiotemporalParams(const SpatiotemporalParams& params) { 
        spatiotemporalParams_ = params; 
    }

    // ===== ADAPTIVE INITIALIZATION =====

    // Adaptive initialization parameters
    struct AdaptiveParams {
        // Adaptive parameters
        float adaptationRate;          // Adaptation rate
        float adaptationThreshold;     // Adaptation threshold
        float adaptationMemory;        // Adaptation memory
        float adaptationTimeConstant;  // Adaptation time constant
        float adaptationGain;          // Adaptation gain
        
        // Adaptive organization
        bool adaptive;                 // Whether adaptation is enabled
        float adaptiveLearningRate;    // Adaptive learning rate
        float adaptiveTemperature;     // Temperature for adaptation
        float adaptiveExploration;     // Exploration rate for adaptation
        
        // Adaptive-specific parameters
        float adaptiveComplexity;      // Complexity of adaptation
        float adaptiveFlexibility;     // Flexibility of adaptation
        float adaptiveStability;       // Stability of adaptation
    };

    void setAdaptiveParams(const AdaptiveParams& params) { 
        adaptiveParams_ = params; 
    }

    // ===== DYNAMIC INITIALIZATION =====

    // Dynamic initialization parameters
    struct DynamicParams {
        // Dynamic parameters
        float dynamicsRate;             // Dynamics rate
        float dynamicsThreshold;        // Dynamics threshold
        float dynamicsMemory;           // Dynamics memory
        float dynamicsTimeConstant;     // Dynamics time constant
        float dynamicsGain;             // Dynamics gain
        
        // Dynamic organization
        bool dynamic;                   // Whether dynamics is enabled
        float dynamicLearningRate;      // Dynamic learning rate
        float dynamicTemperature;       // Temperature for dynamics
        float dynamicExploration;       // Exploration rate for dynamics
        
        // Dynamic-specific parameters
        float dynamicComplexity;        // Complexity of dynamics
        float dynamicFlexibility;       // Flexibility of dynamics
        float dynamicStability;         // Stability of dynamics
    };

    void setDynamicParams(const DynamicParams& params) { 
        dynamicParams_ = params; 
    }

    // ===== BIOLOGICAL INITIALIZATION =====

    // Biological initialization parameters
    struct BiologicalParams {
        // Biological parameters
        float biologicalRate;           // Biological rate
        float biologicalThreshold;      // Biological threshold
        float biologicalMemory;         // Biological memory
        float biologicalTimeConstant;   // Biological time constant
        float biologicalGain;           // Biological gain
        
        // Biological organization
        bool biological;                // Whether biological is enabled
        float biologicalLearningRate;   // Biological learning rate
        float biologicalTemperature;    // Temperature for biological processes
        float biologicalEvolutionRate;   // Rate of biological evolution
        
        // Biological-specific parameters
        float biologicalComplexity;     // Complexity of biological processes
        float biologicalFlexibility;    // Flexibility of biological processes
        float biologicalStability;      // Stability of biological processes
        
        // Neurobiological parameters
        float neuronSize;               // Neuron size (μm)
        float neuronDensity;             // Neuron density (neurons/mm³)
        float synapseDensity;            // Synapse density (synapses/neuron)
        float connectionProbability;    // Connection probability
    };

    void setBiologicalParams(const BiologicalParams& params) { 
        biologicalParams_ = params; 
    }

    // ===== EVOLUTIONARY INITIALIZATION =====

    // Evolutionary initialization parameters
    struct EvolutionaryParams {
        // Evolutionary parameters
        float populationSize;            // Population size
        float mutationRate;              // Mutation rate
        float crossoverRate;             // Crossover rate
        float selectionPressure;         // Selection pressure
        float fitnessThreshold;          // Fitness threshold
        
        // Evolutionary organization
        bool evolutionary;              // Whether evolution is enabled
        float evolutionLearningRate;    // Evolution learning rate
        float evolutionTemperature;     // Temperature for evolution
        float evolutionDiversity;       // Population diversity
        
        // Evolutionary-specific parameters
        float evolutionComplexity;      // Complexity of evolution
        float evolutionFlexibility;     // Flexibility of evolution
        float evolutionStability;       // Stability of evolution
        
        // Genetic parameters
        float geneLength;                // Length of genetic code
        float geneMutationRate;          // Gene mutation rate
        float chromosomeSize;             // Chromosome size
        float genomeSize;                 // Genome size
    };

    void setEvolutionaryParams(const EvolutionaryParams& params) { 
        evolutionaryParams_ = params; 
    }

    // ===== INFORMATION-THEORETICAL INITIALIZATION =====

    // Information-theoretical initialization parameters
    struct InformationTheoreticalParams {
        // Information parameters
        float entropyTarget;             // Target entropy
        float mutualInformation;         // Mutual information
        float conditionalEntropy;       // Conditional entropy
        float entropyRate;               // Entropy rate
        float informationContent;       // Information content
        
        // Information organization
        bool information;                // Whether information theory is enabled
        float informationLearningRate;  // Information learning rate
        float informationTemperature;   // Temperature for information theory
        float informationComplexity;    // Information complexity
        
        // Information-specific parameters
        float informationFlexibility;    // Flexibility of information
        float informationStability;     // Stability of information
        float informationEfficiency;     // Efficiency of information
        
        // Coding parameters
        float codingRate;                // Coding rate
        float codingCapacity;            // Coding capacity
        float codingRedundancy;          // Coding redundancy
        float codingEfficiency;          // Coding efficiency
    };

    void setInformationTheoreticalParams(const InformationTheoreticalParams& params) { 
        informationTheoreticalParams_ = params; 
    }

    // ===== OPTIMIZATION-BASED INITIALIZATION =====

    // Optimization-based initialization parameters
    struct OptimizationBasedParams {
        // Optimization parameters
        float objectiveFunction;        // Objective function value
        float constraint;                // Constraint value
        float penalty;                   // Penalty function
        float regularization;            // Regularization parameter
        float optimizationRate;          // Optimization rate
        
        // Optimization organization
        bool optimization;               // Whether optimization is enabled
        float optimizationLearningRate; // Optimization learning rate
        float optimizationTemperature;  // Temperature for optimization
        float optimizationComplexity;   // Optimization complexity
        
        // Optimization-specific parameters
        float optimizationFlexibility;   // Flexibility of optimization
        float optimizationStability;    // Stability of optimization
        float optimizationEfficiency;    // Efficiency of optimization
        
        // Gradient parameters
        float gradientMagnitude;         // Gradient magnitude
        float gradientDirection;         // Gradient direction
        float gradientTimeConstant;      // Gradient time constant
    };

    void setOptimizationBasedParams(const OptimizationBasedParams& params) { 
        optimizationBasedParams_ = params; 
    }

    // ===== CALCULATION METHODS =====

    // Initialize network uniformly at random
    void initializeUniformRandom(std::vector<float>& values, size_t size) const;

    // Initialize network with clustering
    void initializeClustered(std::vector<float>& values, size_t size, RandomGenerator& rng) const;

    // Initialize network with gradient
    void initializeGradient(std::vector<float>& values, size_t size, float start, float end) const;

    // Initialize network with activity
    void initializeActivityDriven(std::vector<float>& values, size_t size, 
                                  RandomGenerator& rng, float targetActivity) const;

    // Initialize network with structure
    void initializeStructured(std::vector<float>& values, size_t size, 
                              float regularity, float period, float amplitude) const;

    // Initialize network with hierarchy
    void initializeHierarchical(std::vector<float>& values, size_t size, 
                               size_t levels, float decay) const;

    // Initialize network with spatiotemporal organization
    void initializeSpatiotemporal(std::vector<float>& values, size_t size,
                                  float spatialScale, float temporalScale) const;

    // Initialize network with adaptation
    void initializeAdaptive(std::vector<float>& values, size_t size, 
                           float adaptationRate, RandomGenerator& rng) const;

    // Initialize network dynamically
    void initializeDynamic(std::vector<float>& values, size_t size,
                           float dynamicsRate, RandomGenerator& rng) const;

    // Initialize network biologically
    void initializeBiological(std::vector<float>& values, size_t size,
                             float neuronSize, float neuronDensity) const;

    // Initialize network evolutionarily
    void initializeEvolutionary(std::vector<float>& values, size_t size, 
                               float mutationRate, RandomGenerator& rng) const;

    // Initialize network information-theoretically
    void initializeInformationTheoretical(std::vector<float>& values, size_t size,
                                          float entropyTarget) const;

    // Initialize network optimization-based
    void initializeOptimizationBased(std::vector<float>& values, size_t size,
                                    float objectiveFunction, float constraint) const;

    // ===== STATISTICS =====

    // Initialization statistics
    struct InitializationStats {
        float initializationRate;          // Initialization rate
        float initializationAccuracy;       // Initialization accuracy
        float initializationStability;      // Initialization stability
        float initializationComplexity;     // Initialization complexity
        float initializationEfficiency;     // Initialization efficiency
        float initializationFlexibility;    // Initialization flexibility
        float initializationSpeed;          // Initialization speed
    };

    InitializationStats getStats() const { return stats_; }

    // Update statistics
    void updateStats(float rate, float accuracy, float stability,
                    float complexity, float efficiency, float flexibility, float speed);

    // ===== UTILITY =====

    // Get initialization strategy
    InitializationStrategy getInitializationStrategy() const { return strategy_; }
    void setInitializationStrategy(InitializationStrategy strategy) { strategy_ = strategy; }

    // Get initialization parameters
    const UniformRandomParams& getUniformRandomParams() const { return uniformRandomParams_; }
    const ClusteredParams& getClusteredParams() const { return clusteredParams_; }
    const GradientParams& getGradientParams() const { return gradientParams_; }
    const ActivityDrivenParams& getActivityDrivenParams() const { return activityDrivenParams_; }
    const StructuredParams& getStructuredParams() const { return structuredParams_; }
    const HierarchicalParams& getHierarchicalParams() const { return hierarchicalParams_; }
    const SpatiotemporalParams& getSpatiotemporalParams() const { return spatiotemporalParams_; }
    const AdaptiveParams& getAdaptiveParams() const { return adaptiveParams_; }
    const DynamicParams& getDynamicParams() const { return dynamicParams_; }
    const BiologicalParams& getBiologicalParams() const { return biologicalParams_; }
    const EvolutionaryParams& getEvolutionaryParams() const { return evolutionaryParams_; }
    const InformationTheoreticalParams& getInformationTheoreticalParams() const { return informationTheoreticalParams_; }
    const OptimizationBasedParams& getOptimizationBasedParams() const { return optimizationBasedParams_; }

    // ===== OVERRIDE =====

    // Override Brain for brain integration
    class IntegratedNetworkInitializer : public Brain {
    public:
        IntegratedNetworkInitializer(std::shared_ptr<Config> config,
                                     std::shared_ptr<NetworkInitializer> initializer);
        ~IntegratedNetworkInitializer() override;

        // Initialize network with strategy
        bool initialize() override;

        // Initialize network with specific strategy
        bool initializeWithStrategy(InitializationStrategy strategy);

        // Initialize network with custom parameters
        bool initializeWithParams(const std::string& strategyName,
                                 const std::map<std::string, float>& params);

        // Get initialization strategy
        InitializationStrategy getCurrentStrategy() const { return currentStrategy_; }

        // Update initialization
        void updateInitialization(TimestepDuration dt);

        // Get initialization metrics
        InitializationStats getInitializationMetrics() const;

    private:
        std::shared_ptr<NetworkInitializer> initializer_;
        InitializationStrategy currentStrategy_;
        InitializationStats currentMetrics_;
    };

private:
    // Initialization strategy
    InitializationStrategy strategy_;

    // Strategy-specific parameters
    UniformRandomParams uniformRandomParams_;
    ClusteredParams clusteredParams_;
    GradientParams gradientParams_;
    ActivityDrivenParams activityDrivenParams_;
    StructuredParams structuredParams_;
    HierarchicalParams hierarchicalParams_;
    SpatiotemporalParams spatiotemporalParams_;
    AdaptiveParams adaptiveParams_;
    DynamicParams dynamicParams_;
    BiologicalParams biologicalParams_;
    EvolutionaryParams evolutionaryParams_;
    InformationTheoreticalParams informationTheoreticalParams_;
    OptimizationBasedParams optimizationBasedParams_;

    // Statistics
    InitializationStats stats_;

    // Helper methods
    float applyUniformRandomDistribution(float mean, float stddev, RandomGenerator& rng) const;
    float applyClusteredDistribution(float clusterMean, float clusterStddev, RandomGenerator& rng) const;
    float applyGradientDistribution(float start, float end, float position) const;
    float applyActivityDrivenDistribution(float targetActivity, RandomGenerator& rng) const;
    float applyStructuredDistribution(float regularity, float period, float amplitude,
                                     float position) const;
    float applyHierarchicalDistribution(float decay, size_t level, size_t totalLevels) const;
    float applySpatiotemporalDistribution(float spatialScale, float temporalScale,
                                        float spatialPos, float temporalPos) const;
    float applyAdaptiveDistribution(float adaptationRate, float currentValue, 
                                   RandomGenerator& rng) const;
    float applyDynamicDistribution(float dynamicsRate, float currentValue,
                                   RandomGenerator& rng) const;
    float applyBiologicalDistribution(float neuronSize, float neuronDensity) const;
    float applyEvolutionaryDistribution(float mutationRate, RandomGenerator& rng) const;
    float applyInformationTheoreticalDistribution(float entropyTarget) const;
    float applyOptimizationBasedDistribution(float objectiveFunction, float constraint) const;
};

} // namespace nlm
