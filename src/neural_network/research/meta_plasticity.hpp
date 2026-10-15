#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

/**
 * Meta-Plasticity (Plasticity of Plasticity)
 * 
 * Plasticity of plasticity mechanisms:
 * - Meta-plasticity rules for learning plasticity itself
 * - Adaptive plasticity regulation
 * - Meta-learning algorithms
 * - Plasticity of plasticity modulation
 * - Meta-plasticity in developmental processes
 * - Meta-plasticity in reward learning
 * 
 * Controls how plasticity itself changes over time
 */
class MetaPlasticity {
public:
    MetaPlasticity();
    ~MetaPlasticity();

    // ===== META-PLASTICITY RULES =====

    // Meta-plasticity rule types
    enum class MetaPlasticityRule {
        ADAPTIVE,              // Adaptive meta-plasticity
        homeostatic,           // Homeostatic meta-plasticity
        NORMALIZING,           // Normalizing meta-plasticity
        STABILIZING,           // Stabilizing meta-plasticity
        ENHANCING,            // Enhancing meta-plasticity
        SUPPRESSING,           // Suppressing meta-plasticity
        BALANCING,             // Balancing meta-plasticity
        OPTIMIZING,            // Optimizing meta-plasticity
    };

    // ===== ADAPTIVE META-PLASTICITY =====

    // Adaptive meta-plasticity parameters
    struct AdaptiveMetaPlasticity {
        // Adaptive parameters
        float adaptationRate;           // Rate of adaptation
        float adaptationThreshold;      // Threshold for adaptation
        float adaptationMemory;         // Memory of adaptation
        float adaptationTimeConstant;   // Time constant for adaptation
        float adaptationGain;           // Adaptation gain
        
        // Adaptive organization
        bool adaptive;                  // Whether adaptation is enabled
        float adaptiveLearningRate;     // Adaptive learning rate
        float adaptiveTemperature;      // Temperature for adaptation
        float adaptiveExploration;      // Exploration rate for adaptation
        
        // Adaptive-specific parameters
        float adaptiveComplexity;       // Complexity of adaptation
        float adaptiveFlexibility;      // Flexibility of adaptation
        float adaptiveStability;        // Stability of adaptation
    };

    void setAdaptiveMetaPlasticity(const AdaptiveMetaPlasticity& params) { 
        adaptiveMetaPlasticity_ = params; 
    }

    // ===== HOMEOSTATIC META-PLASTICITY =====

    // Homeostatic meta-plasticity parameters
    struct HomeostaticMetaPlasticity {
        // Homeostatic parameters
        float homeostaticTarget;        // Target plasticity level
        float homeostaticRange;         // Acceptable plasticity range
        float homeostaticLearningRate;  // Learning rate for homeostasis
        float homeostaticTimeConstant;  // Time constant for homeostasis
        float homeostaticThreshold;     // Threshold for homeostatic plasticity
        float homeostaticStrength;      // Strength of homeostatic effect
        
        // Homeostatic organization
        bool homeostatic;               // Whether homeostasis is enabled
        float homeostaticStability;     // Homeostatic stability
        float homeostaticRobustness;    // Homeostatic robustness
        
        // Homeostatic-specific parameters
        float homeostaticPlasticityRate; // Rate of homeostatic plasticity
        float homeostaticMetaRate;      // Rate of homeostatic meta-plasticity
        float homeostaticMetaThreshold; // Threshold for homeostatic meta-plasticity
    };

    void setHomeostaticMetaPlasticity(const HomeostaticMetaPlasticity& params) { 
        homeostaticMetaPlasticity_ = params; 
    }

    // ===== NORMALIZING META-PLASTICITY =====

    // Normalizing meta-plasticity parameters
    struct NormalizingMetaPlasticity {
        // Normalizing parameters
        float normalizationTarget;      // Target for normalization
        float normalizationRate;        // Rate of normalization
        float normalizationMemory;      // Memory of normalization
        float normalizationTimeConstant; // Time constant for normalization
        float normalizationThreshold;    // Threshold for normalization
        
        // Normalizing organization
        bool normalizing;                // Whether normalization is enabled
        float normalizingLearningRate;   // Normalizing learning rate
        float normalizingTemperature;    // Temperature for normalization
        float normalizingStability;      // Normalizing stability
        
        // Normalizing-specific parameters
        float normalizingPlasticityRate; // Rate of normalizing plasticity
        float normalizingMetaRate;      // Rate of normalizing meta-plasticity
        float normalizingMetaThreshold; // Threshold for normalizing meta-plasticity
    };

    void setNormalizingMetaPlasticity(const NormalizingMetaPlasticity& params) { 
        normalizingMetaPlasticity_ = params; 
    }

    // ===== STABILIZING META-PLASTICITY =====

    // Stabilizing meta-plasticity parameters
    struct StabilizingMetaPlasticity {
        // Stabilizing parameters
        float stabilizingTarget;         // Target for stability
        float stabilizingRate;           // Rate of stabilization
        float stabilizingMemory;         // Memory of stabilization
        float stabilizingTimeConstant;   // Time constant for stabilization
        float stabilizingThreshold;       // Threshold for stabilization
        
        // Stabilizing organization
        bool stabilizing;                // Whether stabilization is enabled
        float stabilizingLearningRate;   // Stabilizing learning rate
        float stabilizingTemperature;    // Temperature for stabilization
        float stabilizingRobustness;     // Stabilizing robustness
        
        // Stabilizing-specific parameters
        float stabilizingPlasticityRate;  // Rate of stabilizing plasticity
        float stabilizingMetaRate;       // Rate of stabilizing meta-plasticity
        float stabilizingMetaThreshold;  // Threshold for stabilizing meta-plasticity
    };

    void setStabilizingMetaPlasticity(const StabilizingMetaPlasticity& params) { 
        stabilizingMetaPlasticity_ = params; 
    }

    // ===== ENHANCING META-PLASTICITY =====

    // Enhancing meta-plasticity parameters
    struct EnhancingMetaPlasticity {
        // Enhancing parameters
        float enhancingTarget;           // Target for enhancement
        float enhancingRate;             // Rate of enhancement
        float enhancingMemory;           // Memory of enhancement
        float enhancingTimeConstant;     // Time constant for enhancement
        float enhancingThreshold;         // Threshold for enhancement
        
        // Enhancing organization
        bool enhancing;                   // Whether enhancement is enabled
        float enhancingLearningRate;      // Enhancing learning rate
        float enhancingTemperature;       // Temperature for enhancement
        float enhancingFlexibility;       // Enhancing flexibility
        
        // Enhancing-specific parameters
        float enhancingPlasticityRate;    // Rate of enhancing plasticity
        float enhancingMetaRate;          // Rate of enhancing meta-plasticity
        float enhancingMetaThreshold;     // Threshold for enhancing meta-plasticity
    };

    void setEnhancingMetaPlasticity(const EnhancingMetaPlasticity& params) { 
        enhancingMetaPlasticity_ = params; 
    }

    // ===== SUPPRESSING META-PLASTICITY =====

    // Suppressing meta-plasticity parameters
    struct SuppressingMetaPlasticity {
        // Suppressing parameters
        float suppressingTarget;         // Target for suppression
        float suppressingRate;           // Rate of suppression
        float suppressingMemory;         // Memory of suppression
        float suppressingTimeConstant;   // Time constant for suppression
        float suppressingThreshold;       // Threshold for suppression
        
        // Suppressing organization
        bool suppressing;                 // Whether suppression is enabled
        float suppressingLearningRate;    // Suppressing learning rate
        float suppressingTemperature;     // Temperature for suppression
        float suppressingStability;       // Suppressing stability
        
        // Suppressing-specific parameters
        float suppressingPlasticityRate;  // Rate of suppressing plasticity
        float suppressingMetaRate;       // Rate of suppressing meta-plasticity
        float suppressingMetaThreshold;  // Threshold for suppressing meta-plasticity
    };

    void setSuppressingMetaPlasticity(const SuppressingMetaPlasticity& params) { 
        suppressingMetaPlasticity_ = params; 
    }

    // ===== BALANCING META-PLASTICITY =====

    // Balancing meta-plasticity parameters
    struct BalancingMetaPlasticity {
        // Balancing parameters
        float balancingTarget;            // Target for balance
        float balancingRate;              // Rate of balancing
        float balancingMemory;            // Memory of balancing
        float balancingTimeConstant;      // Time constant for balancing
        float balancingThreshold;          // Threshold for balancing
        
        // Balancing organization
        bool balancing;                    // Whether balancing is enabled
        float balancingLearningRate;       // Balancing learning rate
        float balancingTemperature;        // Temperature for balancing
        float balancingEquilibrium;        // Equilibrium point for balancing
        
        // Balancing-specific parameters
        float balancingPlasticityRate;     // Rate of balancing plasticity
        float balancingMetaRate;          // Rate of balancing meta-plasticity
        float balancingMetaThreshold;     // Threshold for balancing meta-plasticity
    };

    void setBalancingMetaPlasticity(const BalancingMetaPlasticity& params) { 
        balancingMetaPlasticity_ = params; 
    }

    // ===== OPTIMIZING META-PLASTICITY =====

    // Optimizing meta-plasticity parameters
    struct OptimizingMetaPlasticity {
        // Optimizing parameters
        float optimizingTarget;            // Target for optimization
        float optimizingRate;              // Rate of optimization
        float optimizingMemory;            // Memory of optimization
        float optimizingTimeConstant;      // Time constant for optimization
        float optimizingThreshold;         // Threshold for optimization
        
        // Optimizing organization
        bool optimizing;                   // Whether optimization is enabled
        float optimizingLearningRate;      // Optimizing learning rate
        float optimizingTemperature;       // Temperature for optimization
        float optimizingEfficiency;        // Optimization efficiency
        
        // Optimizing-specific parameters
        float optimizingPlasticityRate;    // Rate of optimizing plasticity
        float optimizingMetaRate;          // Rate of optimizing meta-plasticity
        float optimizingMetaThreshold;     // Threshold for optimizing meta-plasticity
        
        // Optimization metrics
        float optimizationScore;           // Current optimization score
        float convergenceRate;             // Rate of convergence
    };

    void setOptimizingMetaPlasticity(const OptimizingMetaPlasticity& params) { 
        optimizingMetaPlasticity_ = params; 
    }

    // ===== META-PLASTICITY STATE =====

    // Meta-plasticity state
    struct MetaPlasticityState {
        float plasticityLevel;            // Current plasticity level (0-1)
        float metaPlasticityLevel;        // Current meta-plasticity level (0-1)
        float adaptationHistory;          // History of adaptation
        float homeostasisHistory;         // History of homeostasis
        float normalizationHistory;       // History of normalization
        float stabilizationHistory;       // History of stabilization
        float enhancementHistory;         // History of enhancement
        float suppressionHistory;         // History of suppression
        float balanceHistory;             // History of balance
        float optimizationHistory;        // History of optimization
        Timestamp lastMetaPlasticityUpdate; // Last meta-plasticity update
    };

    MetaPlasticityState getMetaPlasticityState() const { return metaPlasticityState_; }

    // ===== META-PLASTICITY CALCULATION =====

    // Calculate meta-plasticity influence
    float calculateMetaPlasticityInfluence(MetaPlasticityRule rule, 
                                         float plasticityLevel, 
                                         float metaPlasticityLevel,
                                         float currentTime) const;

    // Calculate adaptive meta-plasticity influence
    float calculateAdaptiveMetaPlasticityInfluence(const AdaptiveMetaPlasticity& params,
                                                  float plasticityLevel,
                                                  float metaPlasticityLevel,
                                                  float dt) const;

    // Calculate homeostatic meta-plasticity influence
    float calculateHomeostaticMetaPlasticityInfluence(const HomeostaticMetaPlasticity& params,
                                                     float plasticityLevel,
                                                     float metaPlasticityLevel,
                                                     float dt) const;

    // Calculate normalizing meta-plasticity influence
    float calculateNormalizingMetaPlasticityInfluence(const NormalizingMetaPlasticity& params,
                                                    float plasticityLevel,
                                                    float metaPlasticityLevel,
                                                    float dt) const;

    // Calculate stabilizing meta-plasticity influence
    float calculateStabilizingMetaPlasticityInfluence(const StabilizingMetaPlasticity& params,
                                                     float plasticityLevel,
                                                     float metaPlasticityLevel,
                                                     float dt) const;

    // Calculate enhancing meta-plasticity influence
    float calculateEnhancingMetaPlasticityInfluence(const EnhancingMetaPlasticity& params,
                                                  float plasticityLevel,
                                                  float metaPlasticityLevel,
                                                  float dt) const;

    // Calculate suppressing meta-plasticity influence
    float calculateSuppressingMetaPlasticityInfluence(const SuppressingMetaPlasticity& params,
                                                     float plasticityLevel,
                                                     float metaPlasticityLevel,
                                                     float dt) const;

    // Calculate balancing meta-plasticity influence
    float calculateBalancingMetaPlasticityInfluence(const BalancingMetaPlasticity& params,
                                                   float plasticityLevel,
                                                   float metaPlasticityLevel,
                                                   float dt) const;

    // Calculate optimizing meta-plasticity influence
    float calculateOptimizingMetaPlasticityInfluence(const OptimizingMetaPlasticity& params,
                                                   float plasticityLevel,
                                                   float metaPlasticityLevel,
                                                   float dt) const;

    // ===== META-PLASTICITY UPDATE =====

    // Update meta-plasticity based on plasticity
    void updateMetaPlasticity(float plasticityChange, float metaPlasticityChange, float dt);

    // Update adaptive meta-plasticity
    void updateAdaptiveMetaPlasticity(float plasticityChange, float dt);

    // Update homeostatic meta-plasticity
    void updateHomeostaticMetaPlasticity(float plasticityChange, float dt);

    // Update normalizing meta-plasticity
    void updateNormalizingMetaPlasticity(float plasticityChange, float dt);

    // Update stabilizing meta-plasticity
    void updateStabilizingMetaPlasticity(float plasticityChange, float dt);

    // Update enhancing meta-plasticity
    void updateEnhancingMetaPlasticity(float plasticityChange, float dt);

    // Update suppressing meta-plasticity
    void updateSuppressingMetaPlasticity(float plasticityChange, float dt);

    // Update balancing meta-plasticity
    void updateBalancingMetaPlasticity(float plasticityChange, float dt);

    // Update optimizing meta-plasticity
    void updateOptimizingMetaPlasticity(float plasticityChange, float dt);

    // ===== META-PLASTICITY STATISTICS =====

    // Meta-plasticity statistics
    struct MetaPlasticityStats {
        float adaptiveMetaPlasticityScore;       // Score for adaptive meta-plasticity
        float homeostaticMetaPlasticityScore;   // Score for homeostatic meta-plasticity
        float normalizingMetaPlasticityScore;   // Score for normalizing meta-plasticity
        float stabilizingMetaPlasticityScore;   // Score for stabilizing meta-plasticity
        float enhancingMetaPlasticityScore;     // Score for enhancing meta-plasticity
        float suppressingMetaPlasticityScore;   // Score for suppressing meta-plasticity
        float balancingMetaPlasticityScore;     // Score for balancing meta-plasticity
        float optimizingMetaPlasticityScore;    // Score for optimizing meta-plasticity
        float overallMetaPlasticityScore;        // Overall meta-plasticity score
        float metaPlasticityEfficiency;         // Meta-plasticity efficiency
    };

    MetaPlasticityStats getStats() const { return stats_; }

    // Update statistics
    void updateStats(const MetaPlasticityState& state);

    // ===== OVERRIDE =====

    // Override Brain for brain integration
    class IntegratedMetaPlasticity : public Brain {
    public:
        IntegratedMetaPlasticity(std::shared_ptr<Config> config,
                               std::shared_ptr<MetaPlasticity> metaPlasticity);
        ~IntegratedMetaPlasticity() override;

        // Initialize meta-plasticity
        bool initialize() override;

        // Enable/disable meta-plasticity
        void enableMetaPlasticity(bool enable) { metaPlasticityEnabled_ = enable; }
        bool isMetaPlasticityEnabled() const { return metaPlasticityEnabled_; }

        // Set meta-plasticity rule
        void setMetaPlasticityRule(MetaPlasticityRule rule) { currentMetaPlasticityRule_ = rule; }
        MetaPlasticityRule getMetaPlasticityRule() const { return currentMetaPlasticityRule_; }

        // Update meta-plasticity
        void updateMetaPlasticity(float plasticityChange, float dt);

        // Get meta-plasticity state
        MetaPlasticityState getMetaPlasticityState() const;

        // Get meta-plasticity statistics
        MetaPlasticityStats getMetaPlasticityStats() const;

        // Enable specific meta-plasticity rules
        void enableAdaptiveMetaPlasticity(bool enable);
        void enableHomeostaticMetaPlasticity(bool enable);
        void enableNormalizingMetaPlasticity(bool enable);
        void enableStabilizingMetaPlasticity(bool enable);
        void enableEnhancingMetaPlasticity(bool enable);
        void enableSuppressingMetaPlasticity(bool enable);
        void enableBalancingMetaPlasticity(bool enable);
        void enableOptimizingMetaPlasticity(bool enable);

        // Set meta-plasticity parameters
        void setAdaptiveMetaPlasticityParams(const AdaptiveMetaPlasticity& params);
        void setHomeostaticMetaPlasticityParams(const HomeostaticMetaPlasticity& params);
        void setNormalizingMetaPlasticityParams(const NormalizingMetaPlasticity& params);
        void setStabilizingMetaPlasticityParams(const StabilizingMetaPlasticity& params);
        void setEnhancingMetaPlasticityParams(const EnhancingMetaPlasticity& params);
        void setSuppressingMetaPlasticityParams(const SuppressingMetaPlasticity& params);
        void setBalancingMetaPlasticityParams(const BalancingMetaPlasticity& params);
        void setOptimizingMetaPlasticityParams(const OptimizingMetaPlasticity& params);

        // Get current meta-plasticity efficiency
        float getCurrentMetaPlasticityEfficiency() const { return currentMetaPlasticityEfficiency_; }

        // Update meta-plasticity efficiency
        void updateMetaPlasticityEfficiency(float efficiency);

    private:
        std::shared_ptr<MetaPlasticity> metaPlasticity_;
        bool metaPlasticityEnabled_;
        MetaPlasticityRule currentMetaPlasticityRule_;
        float currentMetaPlasticityEfficiency_;
        MetaPlasticityStats currentStats_;
    };

private:
    // Meta-plasticity rule configuration
    bool adaptiveMetaPlasticityEnabled_;
    bool homeostaticMetaPlasticityEnabled_;
    bool normalizingMetaPlasticityEnabled_;
    bool stabilizingMetaPlasticityEnabled_;
    bool enhancingMetaPlasticityEnabled_;
    bool suppressingMetaPlasticityEnabled_;
    bool balancingMetaPlasticityEnabled_;
    bool optimizingMetaPlasticityEnabled_;

    // Meta-plasticity parameters
    AdaptiveMetaPlasticity adaptiveMetaPlasticity_;
    HomeostaticMetaPlasticity homeostaticMetaPlasticity_;
    NormalizingMetaPlasticity normalizingMetaPlasticity_;
    StabilizingMetaPlasticity stabilizingMetaPlasticity_;
    EnhancingMetaPlasticity enhancingMetaPlasticity_;
    SuppressingMetaPlasticity suppressingMetaPlasticity_;
    BalancingMetaPlasticity balancingMetaPlasticity_;
    OptimizingMetaPlasticity optimizingMetaPlasticity_;

    // Meta-plasticity state
    MetaPlasticityState metaPlasticityState_;

    // Statistics
    MetaPlasticityStats stats_;

    // Helper methods
    float calculatePlasticityInfluence(float plasticityLevel, float metaPlasticityLevel,
                                      const MetaPlasticityRule& rule) const;
    float updateMetaPlasticityLevel(float currentLevel, float targetLevel, float rate, float dt) const;
    float calculateMetaPlasticityEfficiency(const MetaPlasticityState& state) const;
    void updateMetaPlasticityState(float plasticityChange, float metaPlasticityChange, float dt);
    void enableAllMetaPlasticityRules(bool enable);
};

} // namespace nlm
