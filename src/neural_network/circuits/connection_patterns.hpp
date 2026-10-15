#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

/**
 * Connection Patterns
 * 
 * Different ways neurons connect to each other:
 * - Local connectivity (within regions)
 * - Global connectivity (between regions)
 * - Distance-dependent connectivity
 * - Activity-dependent connectivity
 * - Topographically organized connectivity
 * - State-dependent connectivity
 * - Cross-modal connectivity
 * - Feedback connectivity
 * - Feedforward connectivity
 * - Recurrent connectivity
 */
class ConnectionPatterns {
public:
    ConnectionPatterns();
    ~ConnectionPatterns();

    // ===== CONNECTION PATTERN TYPES =====

    // Connection pattern types
    enum class ConnectionType {
        LOCAL,                // Local connections only
        GLOBAL,               // Global connections only
        LOCAL_GLOBAL,         // Both local and global
        DISTANCE_DEPENDENT,   // Distance-dependent
        ACTIVITY_DEPENDED,    // Activity-dependent
        TOPOGRAPHIC,          // Topographically organized
        STATE_DEPENDENT,      // State-dependent
        CROSS_MODAL,          // Cross-modal connectivity
        FEEDBACK,             // Feedback connections
        FEEDFORWARD,          // Feedforward connections
        RECURRENT,            // Recurrent connections
        LATERAL,              // Lateral connections
        BIAS,                 // Biased connections
        ADAPTIVE,             // Adaptive connections
    };

    // ===== LOCAL CONNECTIVITY =====

    // Local connectivity parameters
    struct LocalConnectivity {
        // Spatial parameters
        float connectionRadius;          // Connection radius (μm)
        float localConnectionProbability; // Probability of local connections
        float localConnectionStrength;    // Strength of local connections
        float spatialDecay;               // Spatial decay constant
        
        // Regional parameters
        bool intraRegionOnly;            // Connections within same region only
        float regionConnectionProbability; // Probability of connections between regions
        float regionBoundaryAttraction;   // Effect of region boundaries on connectivity
        
        // Local pattern parameters
        float patternRegularity;          // Regularity of local pattern
        float patternClustering;          // Clustering of local pattern
        float patternModularity;          // Modularity of local pattern
    };

    void setLocalConnectivity(const LocalConnectivity& params) { 
        localConn_ = params; 
    }

    // ===== GLOBAL CONNECTIVITY =====

    // Global connectivity parameters
    struct GlobalConnectivity {
        // Global parameters
        float globalConnectionProbability; // Probability of global connections
        float globalConnectionStrength;    // Strength of global connections
        float globalConnectionDistance;    // Characteristic distance for global connections
        float globalSpatialScale;          // Spatial scale of global connections
        
        // Global pattern parameters
        float smallWorldGamma;             // Small-world gamma parameter
        float smallWorldBeta;              // Small-world beta parameter
        float smallWorldP;                 // Small-world rewiring probability
        
        // Global statistical parameters
        float globalClustering;            // Global clustering coefficient
        float globalPathLength;            // Global characteristic path length
        float globalEfficiency;            // Global network efficiency
    };

    void setGlobalConnectivity(const GlobalConnectivity& params) { 
        globalConn_ = params; 
    }

    // ===== DISTANCE-DEPENDENT CONNECTIVITY =====

    // Distance-dependent connectivity parameters
    struct DistanceDependentConnectivity {
        // Distance function parameters
        float decayConstant;               // Exponential decay constant (1/μm)
        float minimumDistance;             // Minimum connection distance (μm)
        float maximumDistance;             // Maximum connection distance (μm)
        float powerLawExponent;            // Power-law exponent
        
        // Distance function types
        enum class DistanceFunction {
            EXPONENTIAL,                   // Exponential decay
            POWER_LAW,                     // Power-law decay
            GAUSSIAN,                      // Gaussian decay
            LOGISTIC,                      // Logistic decay
            HILL,                          // Hill function decay
            EXPONENTIAL_POWER,             // Exponential power-law
        };

        DistanceFunction distanceFunction; // Type of distance function
        
        // Distance-dependent parameters
        float distanceDependentGain;       // Gain for distance-dependent connectivity
        float distanceDependentThreshold;  // Threshold for distance-dependent connectivity
        float distanceDependentLearningRate; // Learning rate for distance-dependent connectivity
    };

    void setDistanceDependentConnectivity(const DistanceDependentConnectivity& params) { 
        distanceDependentConn_ = params; 
    }

    // ===== ACTIVITY-DEPENDENT CONNECTIVITY =====

    // Activity-dependent connectivity parameters
    struct ActivityDependentConnectivity {
        // Activity function parameters
        float activityThreshold;           // Activity threshold for connectivity
        float activityLearningRate;        // Learning rate for activity-dependent connectivity
        float activityMemory;              // Memory of past activities
        float activityTimeConstant;        // Time constant for activity dynamics
        float activityIntegrationWindow;   // Integration window for activity
        
        // Activity function types
        enum class ActivityFunction {
            THRESHOLD_LINEAR,               // Threshold-linear function
            SIGMOIDAL,                      // Sigmoidal function
            EXPONENTIAL,                    // Exponential function
            POWER_LAW,                      // Power-law function
            GAUSSIAN,                       // Gaussian function
            RECTIFIED_LINEAR,               // Rectified linear function
        };

        ActivityFunction activityFunction; // Type of activity function
        
        // Activity-dependent parameters
        float activityDependentGain;       // Gain for activity-dependent connectivity
        float activityDependentThreshold;  // Threshold for activity-dependent connectivity
    };

    void setActivityDependentConnectivity(const ActivityDependentConnectivity& params) { 
        activityDependentConn_ = params; 
    }

    // ===== TOPOGRAPHICALLY ORGANIZED CONNECTIVITY =====

    // Topographically organized connectivity parameters
    struct TopographicallyOrganizedConnectivity {
        // Topological parameters
        float topographicScale;            // Scale factor for topography
        float topographicPrecision;        // Precision of topographic mapping
        float topographicContinuity;       // Continuity in topography
        float topographicSmoothness;       // Smoothness of topographic transitions
        
        // Topological organization
        bool topographicRegularity;        // Whether topography is regular
        float topographicPeriod;           // Period of topographic pattern
        float topographicAmplitude;         // Amplitude of topographic pattern
        
        // Topographic connectivity parameters
        float topographicConnectionStrength; // Strength of topographic connectivity
        float topographicCoupling;           // Coupling between topographic regions
        float topographicAttraction;          // Attraction between similar topographic locations
    };

    void setTopographicallyOrganizedConnectivity(const TopographicallyOrganizedConnectivity& params) { 
        topographicConn_ = params; 
    }

    // ===== STATE-DEPENDENT CONNECTIVITY =====

    // State-dependent connectivity parameters
    struct StateDependentConnectivity {
        // State function parameters
        float stateDependency;              // State dependency factor (0-1)
        float stateThreshold;               // State threshold
        float stateMemory;                  // Memory of state
        float stateTimeConstant;            // Time constant for state dynamics
        float stateIntegration;             // State integration factor
        
        // State function types
        enum class StateFunction {
            LINEAR,                          // Linear function
            THRESHOLD_LINEAR,                // Threshold-linear function
            EXPONENTIAL,                     // Exponential function
            POWER_LAW,                       // Power-law function
            SIGMOIDAL,                       // Sigmoidal function
        };

        StateFunction stateFunction;        // Type of state function
        
        // State-dependent parameters
        float stateDependentGain;           // Gain for state-dependent connectivity
        float stateDependentThreshold;      // Threshold for state-dependent connectivity
    };

    void setStateDependentConnectivity(const StateDependentConnectivity& params) { 
        stateDependentConn_ = params; 
    }

    // ===== CROSS-MODALITY CONNECTIVITY =====

    // Cross-modality connectivity parameters
    struct CrossModalityConnectivity {
        // Cross-modality parameters
        float rewardSensoryGain;             // Reward-sensory gain
        float rewardCognitiveGain;           // Reward-cognitive gain
        float sensoryCognitiveGain;          // Sensory-cognitive gain
        float stressRewardGain;              // Stress-reward gain
        float noveltyRewardGain;             // Novelty-reward gain
        
        // Cross-modality interaction
        float crossModalityIntegration;     // Integration of cross-modality signals
        float crossModalityThreshold;        // Threshold for cross-modality connectivity
        float crossModalityTimeConstant;     // Time constant for cross-modality dynamics
        
        // Cross-modality types
        enum class CrossModalityType {
            BIDIRECTIONAL,                   // Bidirectional cross-modality
            FEEDBACK,                         // Feedback cross-modality
            FEEDFORWARD,                      // Feedforward cross-modality
            RECURRENT,                        // Recurrent cross-modality
            LATERAL,                          // Lateral cross-modality
        };

        CrossModalityType crossModalityType; // Type of cross-modality connectivity
    };

    void setCrossModalityConnectivity(const CrossModalityConnectivity& params) { 
        crossModalityConn_ = params; 
    }

    // ===== FEEDBACK CONNECTIVITY =====

    // Feedback connectivity parameters
    struct FeedbackConnectivity {
        // Feedback parameters
        float feedbackStrength;             // Feedback strength
        float feedbackDelay;                 // Feedback delay (steps)
        float feedbackGain;                  // Feedback gain
        float feedbackTimeConstant;          // Feedback time constant
        float feedbackDecay;                 // Feedback decay rate
        
        // Feedback types
        enum class FeedbackType {
            POSITIVE,                         // Positive feedback
            NEGATIVE,                         // Negative feedback
            RECIPROCAL,                       // Reciprocal feedback
            HOPF,                             // Hopf feedback
            DAMPED,                           // Damped feedback
            UNSTABLE,                         // Unstable feedback
        };

        FeedbackType feedbackType;          // Type of feedback
        
        // Feedback control
        float feedbackThreshold;             // Feedback activation threshold
        float feedbackTarget;                // Feedback target level
    };

    void setFeedbackConnectivity(const FeedbackConnectivity& params) { 
        feedbackConn_ = params; 
    }

    // ===== FEEDFORWARD CONNECTIVITY =====

    // Feedforward connectivity parameters
    struct FeedforwardConnectivity {
        // Feedforward parameters
        float feedforwardStrength;           // Feedforward strength
        float feedforwardDelay;               // Feedforward delay (steps)
        float feedforwardGain;                // Feedforward gain
        float feedforwardTimeConstant;        // Feedforward time constant
        float feedforwardIntegration;          // Feedforward integration factor
        
        // Feedforward types
        enum class FeedforwardType {
            DIRECT,                           // Direct feedforward
            DELAYED,                          // Delayed feedforward
            CHAIN,                            // Chain feedforward
            BRANCHED,                         // Branched feedforward
            PARALLEL,                         // Parallel feedforward
        };

        FeedforwardType feedforwardType;     // Type of feedforward connectivity
        
        // Feedforward control
        float feedforwardThreshold;            // Feedforward activation threshold
        float feedforwardTarget;               // Feedforward target level
    };

    void setFeedforwardConnectivity(const FeedforwardConnectivity& params) { 
        feedforwardConn_ = params; 
    }

    // ===== RECURRENT CONNECTIVITY =====

    // Recurrent connectivity parameters
    struct RecurrentConnectivity {
        // Recurrent parameters
        float recurrentStrength;              // Recurrent connection strength
        float recurrentDelay;                  // Recurrent delay (steps)
        float recurrentGain;                   // Recurrent gain
        float recurrentTimeConstant;           // Recurrent time constant
        float recurrentSelfCoupling;            // Recurrent self-coupling
        
        // Recurrent types
        enum class RecurrentType {
            SHORT_RANGE,                       // Short-range recurrent
            LONG_RANGE,                        // Long-range recurrent
            MIXED_RANGE,                       // Mixed-range recurrent
            CIRCULAR,                          // Circular recurrent
            EXPONENTIAL,                       // Exponential recurrent
        };

        RecurrentType recurrentType;          // Type of recurrent connectivity
        
        // Recurrent control
        float recurrentThreshold;               // Recurrent activation threshold
        float recurrentStability;               // Recurrent stability factor
    };

    void setRecurrentConnectivity(const RecurrentConnectivity& params) { 
        recurrentConn_ = params; 
    }

    // ===== LATERAL CONNECTIVITY =====

    // Lateral connectivity parameters
    struct LateralConnectivity {
        // Lateral parameters
        float lateralStrength;                // Lateral connection strength
        float lateralRange;                    // Lateral connection range
        float lateralGain;                     // Lateral gain
        float lateralTimeConstant;             // Lateral time constant
        float lateralInhibition;               // Lateral inhibition strength
        
        // Lateral types
        enum class LateralType {
            INHIBITORY,                         // Inhibitory lateral
            EXCITATORY,                         // Excitatory lateral
            BALANCED,                           // Balanced lateral
            MODULATED,                          // Modulated lateral
            ADAPTIVE,                           // Adaptive lateral
        };

        LateralType lateralType;               // Type of lateral connectivity
        
        // Lateral control
        float lateralThreshold;                 // Lateral activation threshold
        float lateralSurround;                 // Lateral surround suppression
    };

    void setLateralConnectivity(const LateralConnectivity& params) { 
        lateralConn_ = params; 
    }

    // ===== BIAS CONNECTIVITY =====

    // Bias connectivity parameters
    struct BiasConnectivity {
        // Bias parameters
        float biasStrength;                    // Bias connection strength
        float biasDirection;                   // Bias direction (1D)
        float biasGain;                        // Bias gain
        float biasTimeConstant;                // Bias time constant
        float biasModulation;                  // Bias modulation
        
        // Bias types
        enum class BiasType {
            UNIFORM,                           // Uniform bias
            GRADIENT,                          // Gradient bias
            NON_UNIFORM,                       // Non-uniform bias
            SPATIALLY_VARYING,                // Spatially varying bias
            TEMPORALLY_VARYING,               // Temporally varying bias
        };

        BiasType biasType;                    // Type of bias connectivity
        
        // Bias control
        float biasThreshold;                   // Bias activation threshold
        float biasSlope;                       // Bias slope
    };

    void setBiasConnectivity(const BiasConnectivity& params) { 
        biasConn_ = params; 
    }

    // ===== ADAPTIVE CONNECTIVITY =====

    // Adaptive connectivity parameters
    struct AdaptiveConnectivity {
        // Adaptive parameters
        float adaptationRate;                  // Adaptation rate
        float adaptationThreshold;             // Adaptation threshold
        float adaptationMemory;                // Adaptation memory
        float adaptationTimeConstant;          // Adaptation time constant
        float adaptationGain;                  // Adaptation gain
        
        // Adaptive types
        enum class AdaptiveType {
            FEEDBACK_ADAPTIVE,                  // Feedback adaptive
            FEEDFORWARD_ADAPTIVE,               // Feedforward adaptive
            RECURRENT_ADAPTIVE,                // Recurrent adaptive
            LATERAL_ADAPTIVE,                   // Lateral adaptive
            BIAS_ADAPTIVE,                      // Bias adaptive
        };

        AdaptiveType adaptiveType;             // Type of adaptive connectivity
        
        // Adaptive control
        float adaptiveThreshold;                // Adaptive activation threshold
        float adaptiveTarget;                   // Adaptive target level
    };

    void setAdaptiveConnectivity(const AdaptiveConnectivity& params) { 
        adaptiveConn_ = params; 
    }

    // ===== CALCULATION METHODS =====

    // Calculate connectivity between two neurons
    float calculateConnectivity(float distance, float activity1, float activity2,
                                float state1, float state2, float time) const;

    // Calculate local connectivity strength
    float calculateLocalConnectivity(float distance, float spatialCorrelation) const;

    // Calculate global connectivity strength
    float calculateGlobalConnectivity(float distance, float networkProperty) const;

    // Calculate distance-dependent connectivity strength
    float calculateDistanceDependentConnectivity(float distance) const;

    // Calculate activity-dependent connectivity strength
    float calculateActivityDependentConnectivity(float activity1, float activity2) const;

    // Calculate topographically organized connectivity strength
    float calculateTopographicallyOrganizedConnectivity(float topographicSimilarity) const;

    // Calculate state-dependent connectivity strength
    float calculateStateDependentConnectivity(float state1, float state2) const;

    // Calculate cross-modality connectivity strength
    float calculateCrossModalityConnectivity(float reward, float sensory,
                                             float cognitive, float stress, float novelty) const;

    // Calculate feedback connectivity strength
    float calculateFeedbackConnectivity(float currentState, float previousState,
                                        float feedbackDelay) const;

    // Calculate feedforward connectivity strength
    float calculateFeedforwardConnectivity(float previousState, float currentState,
                                           float feedforwardDelay) const;

    // Calculate recurrent connectivity strength
    float calculateRecurrentConnectivity(float currentState, float previousState,
                                         float recurrentDelay) const;

    // Calculate lateral connectivity strength
    float calculateLateralConnectivity(float surroundingActivity, float currentActivity,
                                       float lateralRange) const;

    // Calculate bias connectivity strength
    float calculateBiasConnectivity(float spatialPosition, float biasDirection,
                                    float biasStrength) const;

    // Calculate adaptive connectivity strength
    float calculateAdaptiveConnectivity(float previousConnectivity, float adaptationRate,
                                        float adaptationTarget) const;

    // ===== STATISTICS =====

    // Connection pattern statistics
    struct ConnectionPatternStats {
        float averageLocalConnectivity;         // Average local connectivity strength
        float averageGlobalConnectivity;        // Average global connectivity strength
        float averageDistanceDependent;         // Average distance-dependent connectivity
        float averageActivityDependent;         // Average activity-dependent connectivity
        float averageTopographicallyOrganized;  // Average topographic connectivity
        float averageStateDependent;             // Average state-dependent connectivity
        float averageCrossModality;              // Average cross-modality connectivity
        float averageFeedback;                   // Average feedback connectivity
        float averageFeedforward;                // Average feedforward connectivity
        float averageRecurrent;                  // Average recurrent connectivity
        float averageLateral;                    // Average lateral connectivity
        float averageBias;                       // Average bias connectivity
        float averageAdaptive;                   // Average adaptive connectivity
    };

    ConnectionPatternStats getStats() const { return stats_; }

    // Update statistics
    void updateStats(float localConn, float globalConn, float distanceConn,
                    float activityConn, float topographicConn, float stateConn,
                    float crossModalityConn, float feedbackConn, float feedforwardConn,
                    float recurrentConn, float lateralConn, float biasConn, float adaptiveConn);

    // ===== UTILITY =====

    // Get connection pattern type
    ConnectionType getConnectionType() const { return connectionType_; }
    void setConnectionType(ConnectionType type) { connectionType_ = type; }

    // Calculate network connectivity metrics
    struct ConnectivityMetrics {
        float averageConnectionStrength;         // Average connection strength
        float connectivityHeterogeneity;        // Heterogeneity of connectivity
        float connectivityPlasticity;            // Plasticity of connectivity
        float connectivityStability;             // Stability of connectivity
        float connectivityRobustness;            // Robustness of connectivity
        float connectivityEfficiency;            // Efficiency of connectivity
    };

    ConnectivityMetrics calculateConnectivityMetrics() const;

    // Determine if connection should exist based on pattern
    bool shouldConnect(float distance, float activity1, float activity2,
                      float state1, float state2, float time) const;

    // ===== OVERRIDE =====

    // Override Brain for brain integration
    class IntegratedConnectionPatterns : public Brain {
    public:
        IntegratedConnectionPatterns(std::shared_ptr<Config> config,
                                     std::shared_ptr<ConnectionPatterns> patterns);
        ~IntegratedConnectionPatterns() override;

        // Initialize connection patterns
        bool initialize() override;

        // Apply connection patterns to brain
        void applyConnectionPatterns();

        // Update connection patterns
        void updateConnectionPatterns(TimestepDuration dt);

        // Get connection pattern metrics
        ConnectivityMetrics getConnectionPatternMetrics() const;

    private:
        std::shared_ptr<ConnectionPatterns> patterns_;
        ConnectivityMetrics currentMetrics_;
    };

private:
    // Connection pattern type
    ConnectionType connectionType_;

    // Connection pattern parameters
    LocalConnectivity localConn_;
    GlobalConnectivity globalConn_;
    DistanceDependentConnectivity distanceDependentConn_;
    ActivityDependentConnectivity activityDependentConn_;
    TopographicallyOrganizedConnectivity topographicConn_;
    StateDependentConnectivity stateDependentConn_;
    CrossModalityConnectivity crossModalityConn_;
    FeedbackConnectivity feedbackConn_;
    FeedforwardConnectivity feedforwardConn_;
    RecurrentConnectivity recurrentConn_;
    LateralConnectivity lateralConn_;
    BiasConnectivity biasConn_;
    AdaptiveConnectivity adaptiveConn_;

    // Statistics
    ConnectionPatternStats stats_;

    // Helper methods
    float calculateDistanceDecay(float distance) const;
    float calculateActivitySigmoid(float activity) const;
    float calculateTopographicSimilarity(float position1, float position2) const;
    float calculateStateModulation(float state) const;
    float calculateCrossModalityFactor(float reward, float sensory, float cognitive, 
                                       float stress, float novelty) const;
    float calculateFeedbackEffect(float currentState, float previousState, 
                                 float feedbackDelay) const;
    float calculateFeedforwardEffect(float previousState, float currentState,
                                    float feedforwardDelay) const;
    float calculateRecurrentEffect(float currentState, float previousState,
                                  float recurrentDelay) const;
    float calculateLateralEffect(float surroundingActivity, float currentActivity,
                                float lateralRange) const;
    float calculateBiasEffect(float spatialPosition, float biasDirection,
                             float biasStrength) const;
    float calculateAdaptiveEffect(float previousConnectivity, float adaptationRate,
                                 float adaptationTarget) const;
};

} // namespace nlm
