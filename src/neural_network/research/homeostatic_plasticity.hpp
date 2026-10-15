#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

/**
 * Homeostatic Plasticity
 * 
 * Homeostatic learning rules for maintaining neural stability:
 * - Synaptic scaling
 * - Intrinsic plasticity
 * - Burst-dependent homeostatic plasticity
 * - Activity-dependent synaptic scaling
 * - Homeostatic meta-plasticity
 * - Meta-homeostatic regulation
 * - Homeostatic intrinsic plasticity
 * - Network-level homeostatic mechanisms
 * 
 * Maintains stable network activity and prevents runaway excitation/inhibition
 */
class HomeostaticPlasticity {
public:
    HomeostaticPlasticity();
    ~HomeostaticPlasticity();

    // ===== HOMEOSTATIC PLASTICITY RULES =====

    // Homeostatic plasticity rule types
    enum class HomeostaticPlasticityRule {
        SYNAPTIC_SCALING,              // Synaptic scaling
        INTRINSIC_PLASTICITY,          // Intrinsic plasticity
        BURST_DEPENDENT,              // Burst-dependent homeostatic plasticity
        ACTIVITY_DEPENDENT,           // Activity-dependent synaptic scaling
        META_HOMEOSTATIC,              // Homeostatic meta-plasticity
        META_HOMEOSTATIC_REGULATION,   // Meta-homeostatic regulation
        INTRINSIC_HOMEOSTATIC,         // Homeostatic intrinsic plasticity
        NETWORK_LEVEL,                // Network-level homeostatic mechanisms
    };

    // ===== SYNAPTIC SCALING =====

    // Synaptic scaling parameters
    struct SynapticScaling {
        // Scaling parameters
        float targetFiringRate;        // Target firing rate (Hz)
        float scalingRange;            // Range of scaling (0-1)
        float scalingStrength;         // Strength of scaling
        float scalingTimeConstant;     // Time constant for scaling
        float scalingThreshold;        // Threshold for scaling
        
        // Scaling organization
        bool synapticScaling;          // Whether synaptic scaling is enabled
        float scalingLearningRate;     // Learning rate for synaptic scaling
        float scalingTemperature;      // Temperature for synaptic scaling
        float scalingEfficiency;       // Efficiency of synaptic scaling
        
        // Synaptic scaling-specific parameters
        float synapticScalingRate;     // Rate of synaptic scaling
        float synapticScalingMetaRate; // Rate of synaptic scaling meta-plasticity
        float synapticScalingMetaThreshold; // Threshold for synaptic scaling meta-plasticity
    };

    void setSynapticScaling(const SynapticScaling& params) { 
        synapticScaling_ = params; 
    }

    // ===== INTRINSIC PLASTICITY =====

    // Intrinsic plasticity parameters
    struct IntrinsicPlasticity {
        // Intrinsic parameters
        float targetIntrinsic excitability; // Target intrinsic excitability
        float intrinsicScalingRange;       // Range of intrinsic scaling
        float intrinsicScalingStrength;    // Strength of intrinsic scaling
        float intrinsicScalingTimeConstant; // Time constant for intrinsic scaling
        float intrinsicScalingThreshold;   // Threshold for intrinsic scaling
        
        // Intrinsic organization
        bool intrinsicPlasticity;           // Whether intrinsic plasticity is enabled
        float intrinsicLearningRate;        // Learning rate for intrinsic plasticity
        float intrinsicTemperature;         // Temperature for intrinsic plasticity
        float intrinsicEfficiency;          // Efficiency of intrinsic plasticity
        
        // Intrinsic plasticity-specific parameters
        float intrinsicPlasticityRate;      // Rate of intrinsic plasticity
        float intrinsicPlasticityMetaRate; // Rate of intrinsic plasticity meta-plasticity
        float intrinsicPlasticityMetaThreshold; // Threshold for intrinsic plasticity meta-plasticity
    };

    void setIntrinsicPlasticity(const IntrinsicPlasticity& params) { 
        intrinsicPlasticity_ = params; 
    }

    // ===== BURST-DEPENDENT HOMEOSTATIC PLASTICITY =====

    // Burst-dependent homeostatic plasticity parameters
    struct BurstDependentHomeostaticPlasticity {
        // Burst parameters
        float burstThreshold;             // Burst threshold (spikes/ms)
        float burstAdaptationRate;        // Rate of burst adaptation
        float burstAdaptationMemory;      // Memory of burst adaptation
        float burstAdaptationTimeConstant; // Time constant for burst adaptation
        float burstAdaptationThreshold;   // Threshold for burst adaptation
        
        // Burst organization
        bool burstDependentHomeostatic;   // Whether burst-dependent homeostatic plasticity is enabled
        float burstDependentLearningRate;  // Learning rate for burst-dependent homeostatic plasticity
        float burstDependentTemperature;   // Temperature for burst-dependent homeostatic plasticity
        float burstDependentEfficiency;    // Efficiency of burst-dependent homeostatic plasticity
        
        // Burst-dependent homeostatic plasticity-specific parameters
        float burstDependentPlasticityRate; // Rate of burst-dependent homeostatic plasticity
        float burstDependentPlasticityMetaRate; // Rate of burst-dependent homeostatic plasticity meta-plasticity
        float burstDependentPlasticityMetaThreshold; // Threshold for burst-dependent homeostatic plasticity meta-plasticity
    };

    void setBurstDependentHomeostaticPlasticity(const BurstDependentHomeostaticPlasticity& params) { 
        burstDependentHomeostaticPlasticity_ = params; 
    }

    // ===== ACTIVITY-DEPENDENT SYNAPTIC SCALING =====

    // Activity-dependent synaptic scaling parameters
    struct ActivityDependentSynapticScaling {
        // Activity parameters
        float activityThreshold;          // Activity threshold for scaling
        float activityScalingRate;         // Rate of activity-dependent scaling
        float activityScalingMemory;      // Memory of activity-dependent scaling
        float activityScalingTimeConstant; // Time constant for activity-dependent scaling
        float activityScalingThreshold;   // Threshold for activity-dependent scaling
        
        // Activity organization
        bool activityDependentSynapticScaling; // Whether activity-dependent synaptic scaling is enabled
        float activityDependentLearningRate;   // Learning rate for activity-dependent synaptic scaling
        float activityDependentTemperature;    // Temperature for activity-dependent synaptic scaling
        float activityDependentEfficiency;     // Efficiency of activity-dependent synaptic scaling
        
        // Activity-dependent synaptic scaling-specific parameters
        float activityDependentSynapticScalingRate; // Rate of activity-dependent synaptic scaling
        float activityDependentSynapticScalingMetaRate; // Rate of activity-dependent synaptic scaling meta-plasticity
        float activityDependentSynapticScalingMetaThreshold; // Threshold for activity-dependent synaptic scaling meta-plasticity
    };

    void setActivityDependentSynapticScaling(const ActivityDependentSynapticScaling& params) { 
        activityDependentSynapticScaling_ = params; 
    }

    // ===== META-HOMEOSTATIC =====

    // Meta-homeostatic plasticity parameters
    struct MetaHomeostaticPlasticity {
        // Meta-homeostatic parameters
        float metaHomeostaticTarget;       // Target for meta-homeostatic plasticity
        float metaHomeostaticRate;         // Rate of meta-homeostatic plasticity
        float metaHomeostaticMemory;      // Memory of meta-homeostatic plasticity
        float metaHomeostaticTimeConstant; // Time constant for meta-homeostatic plasticity
        float metaHomeostaticThreshold;    // Threshold for meta-homeostatic plasticity
        
        // Meta-homeostatic organization
        bool metaHomeostatic;               // Whether meta-homeostatic plasticity is enabled
        float metaHomeostaticLearningRate;   // Learning rate for meta-homeostatic plasticity
        float metaHomeostaticTemperature;   // Temperature for meta-homeostatic plasticity
        float metaHomeostaticEfficiency;    // Efficiency of meta-homeostatic plasticity
        
        // Meta-homeostatic-specific parameters
        float metaHomeostaticPlasticityRate; // Rate of meta-homeostatic plasticity
        float metaHomeostaticMetaRate;      // Rate of meta-homeostatic meta-plasticity
        float metaHomeostaticMetaThreshold; // Threshold for meta-homeostatic meta-plasticity
    };

    void setMetaHomeostaticPlasticity(const MetaHomeostaticPlasticity& params) { 
        metaHomeostaticPlasticity_ = params; 
    }

    // ===== META-HOMEOSTATIC REGULATION =====

    // Meta-homeostatic regulation parameters
    struct MetaHomeostaticRegulation {
        // Meta-homeostatic regulation parameters
        float metaHomeostaticRegulationTarget; // Target for meta-homeostatic regulation
        float metaHomeostaticRegulationRate;   // Rate of meta-homeostatic regulation
        float metaHomeostaticRegulationMemory; // Memory of meta-homeostatic regulation
        float metaHomeostaticRegulationTimeConstant; // Time constant for meta-homeostatic regulation
        float metaHomeostaticRegulationThreshold; // Threshold for meta-homeostatic regulation
        
        // Meta-homeostatic regulation organization
        bool metaHomeostaticRegulation;       // Whether meta-homeostatic regulation is enabled
        float metaHomeostaticRegulationLearningRate; // Learning rate for meta-homeostatic regulation
        float metaHomeostaticRegulationTemperature; // Temperature for meta-homeostatic regulation
        float metaHomeostaticRegulationEfficiency; // Efficiency of meta-homeostatic regulation
        
        // Meta-homeostatic regulation-specific parameters
        float metaHomeostaticRegulationRate; // Rate of meta-homeostatic regulation
        float metaHomeostaticRegulationMetaRate; // Rate of meta-homeostatic regulation meta-plasticity
        float metaHomeostaticRegulationMetaThreshold; // Threshold for meta-homeostatic regulation meta-plasticity
    };

    void setMetaHomeostaticRegulation(const MetaHomeostaticRegulation& params) { 
        metaHomeostaticRegulation_ = params; 
    }

    // ===== INTRINSIC HOMEOSTATIC =====

    // Homeostatic intrinsic plasticity parameters
    struct HomeostaticIntrinsicPlasticity {
        // Homeostatic intrinsic parameters
        float homeostaticIntrinsicTarget;    // Target for homeostatic intrinsic plasticity
        float homeostaticIntrinsicRate;      // Rate of homeostatic intrinsic plasticity
        float homeostaticIntrinsicMemory;   // Memory of homeostatic intrinsic plasticity
        float homeostaticIntrinsicTimeConstant; // Time constant for homeostatic intrinsic plasticity
        float homeostaticIntrinsicThreshold; // Threshold for homeostatic intrinsic plasticity
        
        // Homeostatic intrinsic organization
        bool homeostaticIntrinsic;            // Whether homeostatic intrinsic plasticity is enabled
        float homeostaticIntrinsicLearningRate; // Learning rate for homeostatic intrinsic plasticity
        float homeostaticIntrinsicTemperature; // Temperature for homeostatic intrinsic plasticity
        float homeostaticIntrinsicEfficiency; // Efficiency of homeostatic intrinsic plasticity
        
        // Homeostatic intrinsic plasticity-specific parameters
        float homeostaticIntrinsicPlasticityRate; // Rate of homeostatic intrinsic plasticity
        float homeostaticIntrinsicPlasticityMetaRate; // Rate of homeostatic intrinsic plasticity meta-plasticity
        float homeostaticIntrinsicPlasticityMetaThreshold; // Threshold for homeostatic intrinsic plasticity meta-plasticity
    };

    void setHomeostaticIntrinsicPlasticity(const HomeostaticIntrinsicPlasticity& params) { 
        homeostaticIntrinsicPlasticity_ = params; 
    }

    // ===== NETWORK LEVEL =====

    // Network-level homeostatic mechanisms parameters
    struct NetworkLevelHomeostaticMechanisms {
        // Network-level parameters
        float networkLevelTarget;             // Target for network-level homeostatic mechanisms
        float networkLevelRate;               // Rate of network-level homeostatic mechanisms
        float networkLevelMemory;             // Memory of network-level homeostatic mechanisms
        float networkLevelTimeConstant;       // Time constant for network-level homeostatic mechanisms
        float networkLevelThreshold;          // Threshold for network-level homeostatic mechanisms
        
        // Network-level organization
        bool networkLevelHomeostatic;         // Whether network-level homeostatic mechanisms are enabled
        float networkLevelLearningRate;       // Learning rate for network-level homeostatic mechanisms
        float networkLevelTemperature;        // Temperature for network-level homeostatic mechanisms
        float networkLevelEfficiency;         // Efficiency of network-level homeostatic mechanisms
        
        // Network-level homeostatic mechanisms-specific parameters
        float networkLevelHomeostaticRate;    // Rate of network-level homeostatic mechanisms
        float networkLevelHomeostaticMetaRate; // Rate of network-level homeostatic mechanisms meta-plasticity
        float networkLevelHomeostaticMetaThreshold; // Threshold for network-level homeostatic mechanisms meta-plasticity
        
        // Network-level metrics
        float excitationInhibitionRatio;      // Excitation-inhibition ratio
        float networkStability;                // Network stability
        float networkSynchronization;           // Network synchronization
    };

    void setNetworkLevelHomeostaticMechanisms(const NetworkLevelHomeostaticMechanisms& params) { 
        networkLevelHomeostaticMechanisms_ = params; 
    }

    // ===== HOMEOSTATIC PLASTICITY STATE =====

    // Homeostatic plasticity state
    struct HomeostaticPlasticityState {
        float homeostaticPlasticityLevel;    // Current homeostatic plasticity level (0-1)
        float synapticScalingLevel;           // Current synaptic scaling level (0-1)
        float intrinsicPlasticityLevel;       // Current intrinsic plasticity level (0-1)
        float burstDependentHomeostaticLevel; // Current burst-dependent homeostatic level (0-1)
        float activityDependentSynapticScalingLevel; // Current activity-dependent synaptic scaling level (0-1)
        float metaHomeostaticLevel;           // Current meta-homeostatic level (0-1)
        float metaHomeostaticRegulationLevel; // Current meta-homeostatic regulation level (0-1)
        float homeostaticIntrinsicLevel;      // Current homeostatic intrinsic level (0-1)
        float networkLevelHomeostaticLevel;   // Current network-level homeostatic level (0-1)
        Timestamp lastHomeostaticPlasticityUpdate; // Last homeostatic plasticity update
    };

    HomeostaticPlasticityState getHomeostaticPlasticityState() const { return homeostaticPlasticityState_; }

    // ===== HOMEOSTATIC PLASTICITY CALCULATION =====

    // Calculate homeostatic plasticity influence
    float calculateHomeostaticPlasticityInfluence(HomeostaticPlasticityRule rule,
                                                 float homeostaticPlasticityLevel,
                                                 float networkActivity,
                                                 float currentTime) const;

    // Calculate synaptic scaling influence
    float calculateSynapticScalingInfluence(const SynapticScaling& params,
                                           float homeostaticPlasticityLevel,
                                           float networkActivity,
                                           float dt) const;

    // Calculate intrinsic plasticity influence
    float calculateIntrinsicPlasticityInfluence(const IntrinsicPlasticity& params,
                                               float homeostaticPlasticityLevel,
                                               float networkActivity,
                                               float dt) const;

    // Calculate burst-dependent homeostatic plasticity influence
    float calculateBurstDependentHomeostaticPlasticityInfluence(
        const BurstDependentHomeostaticPlasticity& params,
        float homeostaticPlasticityLevel,
        float networkActivity,
        float burstActivity,
        float dt) const;

    // Calculate activity-dependent synaptic scaling influence
    float calculateActivityDependentSynapticScalingInfluence(
        const ActivityDependentSynapticScaling& params,
        float homeostaticPlasticityLevel,
        float networkActivity,
        float dt) const;

    // Calculate meta-homeostatic influence
    float calculateMetaHomeostaticInfluence(const MetaHomeostaticPlasticity& params,
                                           float homeostaticPlasticityLevel,
                                           float metaHomeostaticLevel,
                                           float dt) const;

    // Calculate meta-homeostatic regulation influence
    float calculateMetaHomeostaticRegulationInfluence(
        const MetaHomeostaticRegulation& params,
        float homeostaticPlasticityLevel,
        float metaHomeostaticRegulationLevel,
        float dt) const;

    // Calculate homeostatic intrinsic plasticity influence
    float calculateHomeostaticIntrinsicPlasticityInfluence(
        const HomeostaticIntrinsicPlasticity& params,
        float homeostaticPlasticityLevel,
        float homeostaticIntrinsicLevel,
        float dt) const;

    // Calculate network-level homeostatic mechanisms influence
    float calculateNetworkLevelHomeostaticMechanismsInfluence(
        const NetworkLevelHomeostaticMechanisms& params,
        float homeostaticPlasticityLevel,
        float networkLevelHomeostaticLevel,
        float networkActivity,
        float dt) const;

    // ===== HOMEOSTATIC PLASTICITY UPDATE =====

    // Update homeostatic plasticity based on activity
    void updateHomeostaticPlasticity(float networkActivity, float burstActivity, float dt);

    // Update synaptic scaling
    void updateSynapticScaling(float networkActivity, float dt);

    // Update intrinsic plasticity
    void updateIntrinsicPlasticity(float networkActivity, float dt);

    // Update burst-dependent homeostatic plasticity
    void updateBurstDependentHomeostaticPlasticity(float networkActivity, float burstActivity, float dt);

    // Update activity-dependent synaptic scaling
    void updateActivityDependentSynapticScaling(float networkActivity, float dt);

    // Update meta-homeostatic plasticity
    void updateMetaHomeostaticPlasticity(float networkActivity, float dt);

    // Update meta-homeostatic regulation
    void updateMetaHomeostaticRegulation(float networkActivity, float dt);

    // Update homeostatic intrinsic plasticity
    void updateHomeostaticIntrinsicPlasticity(float networkActivity, float dt);

    // Update network-level homeostatic mechanisms
    void updateNetworkLevelHomeostaticMechanisms(float networkActivity, float dt);

    // ===== HOMEOSTATIC PLASTICITY STATISTICS =====

    // Homeostatic plasticity statistics
    struct HomeostaticPlasticityStats {
        float synapticScalingScore;                    // Score for synaptic scaling
        float intrinsicPlasticityScore;                // Score for intrinsic plasticity
        float burstDependentHomeostaticScore;          // Score for burst-dependent homeostatic plasticity
        float activityDependentSynapticScalingScore;    // Score for activity-dependent synaptic scaling
        float metaHomeostaticScore;                    // Score for meta-homeostatic plasticity
        float metaHomeostaticRegulationScore;          // Score for meta-homeostatic regulation
        float homeostaticIntrinsicScore;               // Score for homeostatic intrinsic plasticity
        float networkLevelHomeostaticScore;            // Score for network-level homeostatic mechanisms
        float overallHomeostaticPlasticityScore;        // Overall homeostatic plasticity score
        float homeostaticPlasticityEfficiency;         // Homeostatic plasticity efficiency
    };

    HomeostaticPlasticityStats getStats() const { return stats_; }

    // Update statistics
    void updateStats(const HomeostaticPlasticityState& state);

    // ===== OVERRIDE =====

    // Override Brain for brain integration
    class IntegratedHomeostaticPlasticity : public Brain {
    public:
        IntegratedHomeostaticPlasticity(std::shared_ptr<Config> config,
                                       std::shared_ptr<HomeostaticPlasticity> homeostaticPlasticity);
        ~IntegratedHomeostaticPlasticity() override;

        // Initialize homeostatic plasticity
        bool initialize() override;

        // Enable/disable homeostatic plasticity
        void enableHomeostaticPlasticity(bool enable) { homeostaticPlasticityEnabled_ = enable; }
        bool isHomeostaticPlasticityEnabled() const { return homeostaticPlasticityEnabled_; }

        // Set homeostatic plasticity rule
        void setHomeostaticPlasticityRule(HomeostaticPlasticityRule rule) { 
            currentHomeostaticPlasticityRule_ = rule; 
        }
        HomeostaticPlasticityRule getHomeostaticPlasticityRule() const { 
            return currentHomeostaticPlasticityRule_; 
        }

        // Update homeostatic plasticity
        void updateHomeostaticPlasticity(float networkActivity, float burstActivity, float dt);

        // Get homeostatic plasticity state
        HomeostaticPlasticityState getHomeostaticPlasticityState() const;

        // Get homeostatic plasticity statistics
        HomeostaticPlasticityStats getHomeostaticPlasticityStats() const;

        // Enable specific homeostatic plasticity rules
        void enableSynapticScaling(bool enable);
        void enableIntrinsicPlasticity(bool enable);
        void enableBurstDependentHomeostaticPlasticity(bool enable);
        void enableActivityDependentSynapticScaling(bool enable);
        void enableMetaHomeostatic(bool enable);
        void enableMetaHomeostaticRegulation(bool enable);
        void enableHomeostaticIntrinsicPlasticity(bool enable);
        void enableNetworkLevelHomeostaticMechanisms(bool enable);

        // Set homeostatic plasticity parameters
        void setSynapticScalingParams(const SynapticScaling& params);
        void setIntrinsicPlasticityParams(const IntrinsicPlasticity& params);
        void setBurstDependentHomeostaticPlasticityParams(
            const BurstDependentHomeostaticPlasticity& params);
        void setActivityDependentSynapticScalingParams(
            const ActivityDependentSynapticScaling& params);
        void setMetaHomeostaticParams(const MetaHomeostaticPlasticity& params);
        void setMetaHomeostaticRegulationParams(const MetaHomeostaticRegulation& params);
        void setHomeostaticIntrinsicPlasticityParams(const HomeostaticIntrinsicPlasticity& params);
        void setNetworkLevelHomeostaticMechanismsParams(
            const NetworkLevelHomeostaticMechanisms& params);

        // Get current homeostatic plasticity efficiency
        float getCurrentHomeostaticPlasticityEfficiency() const { 
            return currentHomeostaticPlasticityEfficiency_; 
        }

        // Update homeostatic plasticity efficiency
        void updateHomeostaticPlasticityEfficiency(float efficiency);

    private:
        std::shared_ptr<HomeostaticPlasticity> homeostaticPlasticity_;
        bool homeostaticPlasticityEnabled_;
        HomeostaticPlasticityRule currentHomeostaticPlasticityRule_;
        float currentHomeostaticPlasticityEfficiency_;
        HomeostaticPlasticityStats currentStats_;
    };

private:
    // Homeostatic plasticity rule configuration
    bool synapticScalingEnabled_;
    bool intrinsicPlasticityEnabled_;
    bool burstDependentHomeostaticPlasticityEnabled_;
    bool activityDependentSynapticScalingEnabled_;
    bool metaHomeostaticEnabled_;
    bool metaHomeostaticRegulationEnabled_;
    bool homeostaticIntrinsicEnabled_;
    bool networkLevelHomeostaticMechanismsEnabled_;

    // Homeostatic plasticity parameters
    SynapticScaling synapticScaling_;
    IntrinsicPlasticity intrinsicPlasticity_;
    BurstDependentHomeostaticPlasticity burstDependentHomeostaticPlasticity_;
    ActivityDependentSynapticScaling activityDependentSynapticScaling_;
    MetaHomeostaticPlasticity metaHomeostaticPlasticity_;
    MetaHomeostaticRegulation metaHomeostaticRegulation_;
    HomeostaticIntrinsicPlasticity homeostaticIntrinsicPlasticity_;
    NetworkLevelHomeostaticMechanisms networkLevelHomeostaticMechanisms_;

    // Homeostatic plasticity state
    HomeostaticPlasticityState homeostaticPlasticityState_;

    // Statistics
    HomeostaticPlasticityStats stats_;

    // Helper methods
    float calculateHomeostaticInfluence(float homeostaticPlasticityLevel,
                                       const HomeostaticPlasticityRule& rule,
                                       float networkActivity) const;
    float updateHomeostaticPlasticityLevel(float currentLevel, float targetLevel,
                                          float rate, float dt) const;
    float calculateHomeostaticPlasticityEfficiency(
        const HomeostaticPlasticityState& state) const;
    void updateHomeostaticPlasticityState(float networkActivity, float burstActivity, float dt);
    void enableAllHomeostaticPlasticityRules(bool enable);
};

} // namespace nlm
