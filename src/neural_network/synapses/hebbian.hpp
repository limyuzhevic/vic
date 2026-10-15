#pragma once

#include "synapse_base.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * Advanced Hebbian Learning Rules
 * 
 * Enhanced Hebbian learning with:
 * - Oja's rule (normalization)
 * - BCM rule (sliding threshold)
 * - HTM (Hierarchical Temporal Memory) learning
 * - Sparse coding
 * - Unsupervised learning
 * - Supervised learning with targets
 * - Online learning algorithms
 * 
 * Reference: Oja (1982), BCM Theory, Felleman & Van Essen (1991)
 */
class AdvancedHebbian : public EnhancedSynapse {
public:
    AdvancedHebbian(SynapseId id, NeuronId source, NeuronId destination);
    ~AdvancedHebbian() override;

    // ===== OJA'S RULE =====

    // Enable Oja's rule (normalized Hebbian learning)
    void enableOjaRule(bool enable) { ojaRule_ = enable; }
    bool isOjaRuleEnabled() const { return ojaRule_; }

    struct OjaParameters {
        float learningRate;    // Oja learning rate
        float normalizationTau; // Normalization time constant
        float targetActivity;  // Target firing rate (Hz)
        float weightDecay;     // Weight decay rate
    };

    void setOjaParameters(const OjaParameters& params) { 
        ojaParams_ = params; 
    }

    // ===== BCM RULE =====

    // Enable BCM (Bayesian Control of Optimal Metonymy) rule
    void enableBCM(bool enable) { bcmRule_ = enable; }
    bool isBCMEnabled() const { return bcmRule_; }

    struct BCMParameters {
        float phi;             // Sliding threshold (mV)
        float phi_plus;        // Upper threshold (mV)
        float phi_minus;       // Lower threshold (mV)
        float learningRate;    // BCM learning rate
        float tau_phi;         // Threshold adaptation time constant
        float theta;           // Activity threshold
    };

    void setBCMParameters(const BCMParameters& params) { 
        bcmParams_ = params; 
    }

    // ===== HTM LEARNING =====

    // Enable Hierarchical Temporal Memory (HTM) learning
    void enableHTMLearning(bool enable) { htmLearning_ = enable; }
    bool isHTMLearningEnabled() const { return htmLearning_; }

    struct HTMParameters {
        float predictionThreshold;   // Prediction threshold
        float predictionLearningRate; // Prediction learning rate
        float columnActivity;        // Column activity level
        float spatialPoolerSize;     // Spatial pooler size
        float temporalPoolerSize;    // Temporal pooler size
        float boostingStrength;      // Boosting strength
    };

    void setHTMParameters(const HTMParameters& params) { 
        htmParams_ = params; 
    }

    // ===== SPARSE CODING =====

    // Enable sparse coding
    void enableSparseCoding(bool enable) { sparseCoding_ = enable; }
    bool isSparseCodingEnabled() const { return sparseCoding_; }

    struct SparseCodingParameters {
        float sparsityTarget;      // Target sparsity level (0-1)
        float sparsityLearningRate; // Learning rate for sparsity
        float competitionStrength;  // Competition strength
        float regularization;       // L1 regularization weight
        float entropyWeight;        // Entropy weight for coding
    };

    void setSparseCodingParameters(const SparseCodingParameters& params) { 
        sparseCodingParams_ = params; 
    }

    // ===== SUPERVISED LEARNING =====

    // Enable supervised learning
    void enableSupervisedLearning(bool enable) { supervised_ = enable; }
    bool isSupervisedLearningEnabled() const { return supervised_; }

    struct SupervisedParameters {
        float learningRate;        // Supervised learning rate
        float momentum;            // Momentum factor
        float weightRegularization; // Weight regularization
        float targetEpsilon;       // Target error threshold
        float errorScaling;        // Error scaling factor
    };

    void setSupervisedParameters(const SupervisedParameters& params) { 
        supervisedParams_ = params; 
    }

    // ===== LOCAL COMPETITION =====

    // Enable local competition ( Winner-take-all )
    void enableLocalCompetition(bool enable) { localCompetition_ = enable; }
    bool isLocalCompetitionEnabled() const { return localCompetition_; }

    struct LocalCompetitionParameters {
        float inhibitionStrength;  // Inhibition strength
        float inhibitionRadius;    // Inhibition radius
        float threshold;            // Firing threshold for competition
        float decayRate;            // Inhibition decay rate
    };

    void setLocalCompetitionParameters(const LocalCompetitionParameters& params) { 
        localCompParams_ = params; 
    }

    // ===== ONLINE LEARNING =====

    // Enable online learning
    void enableOnlineLearning(bool enable) { onlineLearning_ = enable; }
    bool isOnlineLearningEnabled() const { return onlineLearning_; }

    struct OnlineLearningParameters {
        float batchSize;           // Batch size for online learning
        float learningRate;        // Online learning rate
        float momentum;            // Momentum for online learning
        float decayRate;           // Learning rate decay
        float explorationRate;     // Exploration rate
    };

    void setOnlineLearningParameters(const OnlineLearningParameters& params) { 
        onlineLearningParams_ = params; 
    }

    // ===== CO-CONNECTIVITY =====

    // Enable co-connectivity learning
    void enableCoConnectivity(bool enable) { coConnectivity_ = enable; }
    bool isCoConnectivityEnabled() const { return coConnectivity_; }

    struct CoConnectivityParameters {
        float spatialScale;        // Spatial scale for co-connectivity
        float temporalScale;       // Temporal scale for co-connectivity
        float correlationThreshold; // Correlation threshold
        float correlationStrength;  // Correlation strength
        float spatialDecay;        // Spatial decay rate
        float temporalDecay;       // Temporal decay rate
    };

    void setCoConnectivityParameters(const CoConnectivityParameters& params) { 
        coConnectivityParams_ = params; 
    }

    // ===== STATISTICAL LEARNING =====

    // Enable statistical learning
    void enableStatisticalLearning(bool enable) { statisticalLearning_ = enable; }
    bool isStatisticalLearningEnabled() const { return statisticalLearning_; }

    struct StatisticalLearningParameters {
        float mutualInformation;    // Mutual information target
        float predictability;      // Predictability target
        float transferEntropy;     // Transfer entropy
        float conditionalProbability; // Conditional probability
        float learningRate;        // Statistical learning rate
    };

    void setStatisticalLearningParameters(const StatisticalLearningParameters& params) { 
        statisticalLearningParams_ = params; 
    }

    // ===== LEARNING STATE =====

    // Learning state
    struct LearningState {
        // For Oja's rule
        float ojaWeightHistory;     // Weight history for Oja
        float ojaActivityHistory;   // Activity history for Oja
        
        // For BCM rule
        float bcmThreshold;         // Current BCM threshold
        float bcmActivityHistory;   // Activity history for BCM
        
        // For HTM
        float predictionHistory;    // Prediction history for HTM
        float errorHistory;         // Error history for HTM
        
        // For sparse coding
        float sparsityHistory;      // Sparsity history
        float codingHistory;        // Coding history
        
        // For supervised learning
        float targetHistory;        // Target history
        float errorHistorySupervised; // Supervised error history
        
        // For local competition
        float inhibitionHistory;    // Inhibition history
        
        // For online learning
        float batchHistory;         // Batch history
        float gradientHistory;      // Gradient history
        
        // For co-connectivity
        float correlationHistory;   // Correlation history
        
        // For statistical learning
        float miHistory;            // Mutual information history
        float predictabilityHistory;// Predictability history
    };

    LearningState getLearningState() const { return learningState_; }

    // ===== UPDATE METHODS =====

    // Update Oja's rule
    void updateOjaRule(float preActivity, float postActivity, float dt);

    // Update BCM rule
    void updateBCMRule(float preActivity, float postActivity, float dt);

    // Update HTM learning
    void updateHTMLearning(float preActivity, float postActivity, float prediction, float dt);

    // Update sparse coding
    void updateSparseCoding(float preActivity, float postActivity, float dt);

    // Update supervised learning
    void updateSupervisedLearning(float preActivity, float postActivity,
                                  float target, float dt);

    // Update local competition
    void updateLocalCompetition(float preActivity, float postActivity, float dt);

    // Update online learning
    void updateOnlineLearning(float preActivity, float postActivity, float dt);

    // Update co-connectivity
    void updateCoConnectivity(float preActivity, float postActivity, float dt);

    // Update statistical learning
    void updateStatisticalLearning(float preActivity, float postActivity, float dt);

    // Calculate combined learning signal
    float calculateCombinedLearningSignal(float preActivity, float postActivity,
                                         float target, float prediction, float correlation) const;

    // Apply learning with all rules
    void applyAllHebbianRules(float preActivity, float postActivity, float target,
                            float prediction, float correlation, float dt);

    // ===== STATISTICS =====

    // Learning statistics
    struct HebbianStatistics {
        float ojaLearningRate;          // Oja learning rate statistics
        float bcmThresholdHistory;      // BCM threshold history
        float sparsityLevel;            // Current sparsity level
        float predictionAccuracy;       // HTM prediction accuracy
        float coConnectivityStrength;   // Co-connectivity strength
        float mutualInformation;        // Mutual information value
        float learningSpeed;            // Overall learning speed
    };

    HebbianStatistics getStatistics() const { return statistics_; }

    // Update statistics
    void updateStatistics(float learningRate, float sparsity, float prediction,
                         float coConnectivity, float mutualInfo);

    // ===== UTILITY =====

    // Get normalized weight (for Oja's rule)
    float getNormalizedWeight(float currentWeight, float activity) const;

    // Get BCM sliding threshold
    float getBCMSlidingThreshold() const { return bcmParams_.phi; }

    // Update BCM threshold
    void updateBCMSlidingThreshold(float activity, float dt);

    // ===== OVERRIDE =====

    // Override EnhancedSynapse::step
    void step(Timestamp currentTime) override {
        stepEnhanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

    // Main enhanced Hebbian step
    bool stepEnhanced(Timestamp currentTime, TimestepDuration dt);

private:
    // Configuration flags
    bool ojaRule_;
    bool bcmRule_;
    bool htmLearning_;
    bool sparseCoding_;
    bool supervised_;
    bool localCompetition_;
    bool onlineLearning_;
    bool coConnectivity_;
    bool statisticalLearning_;

    // Parameters
    OjaParameters ojaParams_;
    BCMParameters bcmParams_;
    HTMParameters htmParams_;
    SparseCodingParameters sparseCodingParams_;
    SupervisedParameters supervisedParams_;
    LocalCompetitionParameters localCompParams_;
    OnlineLearningParameters onlineLearningParams_;
    CoConnectivityParameters coConnectivityParams_;
    StatisticalLearningParameters statisticalLearningParams_;

    // Learning state
    LearningState learningState_;

    // Statistics
    HebbianStatistics statistics_;

    // Helper methods
    float calculateOjaWeightChange(float preActivity, float postActivity) const;
    float calculateBCMWeightChange(float preActivity, float postActivity) const;
    float calculateHTMLearningSignal(float preActivity, float postActivity,
                                    float prediction) const;
    float calculateSparseCodingChange(float preActivity, float postActivity) const;
    float calculateSupervisedLearningChange(float preActivity, float postActivity,
                                           float target) const;
    float calculateLocalCompetitionInhibition(float activity) const;
    float calculateOnlineLearningChange(float preActivity, float postActivity) const;
    float calculateCoConnectivityChange(float preActivity, float postActivity) const;
    float calculateStatisticalLearningChange(float preActivity, float postActivity) const;
    void updateBCMThreshold(float activity, float dt);
};

} // namespace nlm
