#pragma once

#include "synapse_base.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <algorithm>

namespace nlm {

/**
 * Structural Plasticity
 * 
 * Connection formation and pruning rules:
 * - Hebbian-like connection formation
 * - Anti-Hebbian connection pruning
 * - Homeostatic structural plasticity
 * - Activity-dependent synaptogenesis
 * - Competition-based structural changes
 * - Noise-driven structural changes
 * - Developmental structural plasticity
 * - Meta-structural plasticity
 * 
 * Implements structural changes to neural connectivity
 */
class StructuralPlasticity : public EnhancedSynapse {
public:
    StructuralPlasticity(SynapseId id, NeuronId source, NeuronId destination);
    ~StructuralPlasticity() override;

    // ===== CONNECTION FORMATION =====

    // Connection formation rules
    enum class FormationRule {
        HEBBIAN,              // Hebbian-like formation
        ANTI_HEBBIAN,         // Anti-Hebbian formation
        COACTIVATED,          // Co-activation based formation
        SPATIAL,              // Spatial proximity formation
        TEMPORAL,             // Temporal proximity formation
        MIXED,                // Mixed rule formation
    };

    // Enable connection formation
    void enableConnectionFormation(bool enable) { formationEnabled_ = enable; }
    bool isConnectionFormationEnabled() const { return formationEnabled_; }

    // Formation parameters
    struct FormationParameters {
        // Base formation rate
        float formationRate;           // Base rate of new connections
        float formationThreshold;      // Threshold for formation
        float formationLearningRate;   // Learning rate for formation
        
        // Hebbian-like formation
        float hebbianStrength;         // Hebbian strength (0-1)
        float hebbianTimeWindow;       // Hebbian time window (ms)
        float hebbianWeightThreshold;  // Weight threshold for formation
        
        // Anti-Hebbian formation
        float antiHebbianStrength;     // Anti-Hebbian strength (0-1)
        float antiHebbianTimeWindow;   // Anti-Hebbian time window (ms)
        float antiHebbianWeightThreshold; // Anti-Hebbian weight threshold
        
        // Co-activation formation
        float coactivationThreshold;   // Co-activation threshold
        float coactivationWindow;      // Co-activation time window (ms)
        float coactivationStrength;    // Co-activation strength
        
        // Spatial formation
        float spatialScale;            // Spatial scale factor
        float spatialThreshold;        // Spatial threshold (μm)
        float spatialWeight;           // Spatial weight
        
        // Temporal formation
        float temporalScale;           // Temporal scale factor
        float temporalThreshold;       // Temporal threshold (ms)
        float temporalWeight;          // Temporal weight
    };

    void setFormationParameters(const FormationParameters& params) { 
        formationParams_ = params; 
    }

    // ===== CONNECTION PRUNING =====

    // Connection pruning rules
    enum class PruningRule {
        ANTI_HEBBIAN,         // Anti-Hebbian pruning
        HOMEOSTATIC,          // Homeostatic pruning
        COMPETITION,          // Competition-based pruning
        AGE,                  // Age-based pruning
        NOISE_DRIVEN,         // Noise-driven pruning
        ACTIVITY,             // Activity-dependent pruning
        MIXED,                // Mixed pruning rule
    };

    // Enable connection pruning
    void enableConnectionPruning(bool enable) { pruningEnabled_ = enable; }
    bool isConnectionPruningEnabled() const { return pruningEnabled_; }

    // Pruning parameters
    struct PruningParameters {
        // Base pruning rate
        float pruningRate;            // Base rate of pruning
        float pruningThreshold;       // Threshold for pruning
        float pruningLearningRate;    // Learning rate for pruning
        
        // Anti-Hebbian pruning
        float antiHebbianStrength;    // Anti-Hebbian pruning strength
        float antiHebbianTimeWindow;  // Anti-Hebbian pruning time window
        
        // Homeostatic pruning
        float homeostaticTarget;     // Target number of connections
        float homeostaticRange;      // Target connection range
        float homeostaticLearningRate; // Homeostatic learning rate
        
        // Competition pruning
        float competitionThreshold;   // Competition threshold
        float competitionRange;       // Competition range
        float competitionStrength;    // Competition strength
        
        // Age-based pruning
        float ageThreshold;           // Connection age threshold (steps)
        float ageDecay;               // Age decay rate
        
        // Noise-driven pruning
        float noiseRate;              // Noise-driven pruning rate
        float noiseThreshold;         // Noise threshold
        
        // Activity-dependent pruning
        float activityThreshold;      // Activity threshold for pruning
        float activityWeight;         // Activity weight
    };

    void setPruningParameters(const PruningParameters& params) { 
        pruningParams_ = params; 
    }

    // ===== HOMEOSTATIC STRUCTURAL PLASTICITY =====

    // Homeostatic structural plasticity
    struct HomeostaticStructuralPlasticity {
        float targetConnectionCount;   // Target number of connections
        float homeostaticRange;       // Acceptable range around target
        float homeostaticLearningRate; // Rate of homeostatic adjustment
        float homeostaticTimeConstant; // Time constant for homeostatic dynamics
        float homeostaticThreshold;    // Threshold for homeostatic plasticity
        float homeostaticStrength;     // Strength of homeostatic effect
    };

    void setHomeostaticStructuralParams(const HomeostaticStructuralPlasticity& params) { 
        homeostaticStructParams_ = params; 
    }

    // ===== ACTIVITY-DEPENDENT SYNAPTAGENESIS =====

    // Activity-dependent synaptogenesis
    struct Synaptogenesis {
        float activityThreshold;       // Activity threshold for synaptogenesis
        float synaptogenesisRate;      // Base synaptogenesis rate
        float synaptogenesisLearningRate; // Synaptogenesis learning rate
        float activityWeight;          // Weight of activity in synaptogenesis
        float correlationThreshold;    // Correlation threshold for synaptogenesis
        float temporalWindow;          // Synaptogenesis temporal window (ms)
    };

    void setSynaptogenesisParams(const Synaptogenesis& params) { 
        synaptogenesisParams_ = params; 
    }

    // ===== COMPETITION-BASED STRUCTURAL CHANGES =====

    // Competition-based structural changes
    struct Competition {
        float competitionThreshold;    // Competition threshold
        float competitionRange;        // Competition range
        float competitionStrength;     // Competition strength
        float competitiveLearningRate;  // Competitive learning rate
        float rewardCompetition;        // Reward-modulated competition
        float noveltyCompetition;       // Novelty-modulated competition
    };

    void setCompetitionParams(const Competition& params) { 
        competitionParams_ = params; 
    }

    // ===== NOISE-DRIVEN STRUCTURAL CHANGES =====

    // Noise-driven structural changes
    struct NoiseDrivenChanges {
        float noiseLevel;              // Noise level for structural changes
        float noiseRate;               // Rate of noise-driven changes
        float noiseThreshold;          // Threshold for noise-driven changes
        float noiseSpatialScale;       // Spatial scale of noise
        float noiseTemporalScale;      // Temporal scale of noise
    };

    void setNoiseDrivenParams(const NoiseDrivenChanges& params) { 
        noiseDrivenParams_ = params; 
    }

    // ===== DEVELOPMENTAL STRUCTURAL PLASTICITY =====

    // Developmental structural plasticity
    struct DevelopmentalStructuralPlasticity {
        float developmentalStage;      // Current developmental stage (0-1)
        float criticalPeriod;           // Critical period strength
        float maturationRate;            // Rate of structural maturation
        float pruningStage;              // Pruning stage strength
        float synaptogenesisStage;       // Synaptogenesis stage strength
        float competitionStage;          // Competition stage strength
    };

    void setDevelopmentalStructuralParams(const DevelopmentalStructuralPlasticity& params) { 
        developmentalStructParams_ = params; 
    }

    // ===== META-STRUCTURAL PLASTICITY =====

    // Meta-structural plasticity (plasticity of plasticity)
    struct MetaStructuralPlasticity {
        float plasticityRate;           // Rate of structural plasticity change
        float plasticityLevel;          // Current structural plasticity level
        float plasticityTarget;         // Target structural plasticity level
        float metaTimeConstant;         // Meta-plasticity time constant
        float metaLearningRate;         // Meta-plasticity learning rate
        float metaThreshold;            // Meta-plasticity threshold
    };

    void setMetaStructuralParams(const MetaStructuralPlasticity& params) { 
        metaStructParams_ = params; 
    }

    // ===== STATE TRACKING =====

    // Connection state
    struct ConnectionState {
        float age;                      // Connection age (steps)
        float stability;                // Connection stability (0-1)
        float activityLevel;            // Recent activity level (0-1)
        float weightHistory;            // Recent weight history
        float plasticityHistory;        // Recent plasticity history
        Timestamp lastModified;         // Last modification time
        ConnectionFormationRule lastFormationRule; // Last formation rule used
        ConnectionPruningRule lastPruningRule;   // Last pruning rule used
    };

    ConnectionState getConnectionState() const { return connectionState_; }
    void updateConnectionState(bool formed, bool pruned, float weightChange, float dt);

    // ===== CALCULATION METHODS =====

    // Calculate formation probability
    float calculateFormationProbability(float sourceActivity, float targetActivity,
                                        float spatialDistance, float temporalSimilarity,
                                        float currentWeight, float dt) const;

    // Calculate pruning probability
    float calculatePruningProbability(float sourceActivity, float targetActivity,
                                      float age, float stability, float dt) const;

    // Calculate homeostatic adjustment
    float calculateHomeostaticAdjustment(int32_t currentConnectionCount,
                                        int32_t targetConnectionCount, float dt) const;

    // Calculate competition effect
    float calculateCompetitionEffect(float sourceActivity, float targetActivity,
                                     float existingConnections, float dt) const;

    // Calculate noise-driven changes
    float calculateNoiseDrivenChange(float dt) const;

    // Calculate developmental effect
    float calculateDevelopmentalEffect(float developmentalStage, float dt) const;

    // Calculate meta-structural plasticity effect
    float calculateMetaStructuralEffect(float plasticityLevel, float targetPlasticity, float dt) const;

    // ===== UPDATE METHODS =====

    // Update formation dynamics
    void updateFormationDynamics(float sourceActivity, float targetActivity,
                                float spatialDistance, float temporalSimilarity,
                                float dt);

    // Update pruning dynamics
    void updatePruningDynamics(float sourceActivity, float targetActivity,
                              float age, float stability, float dt);

    // Update homeostatic dynamics
    void updateHomeostaticDynamics(int32_t currentConnectionCount, float dt);

    // Update competition dynamics
    void updateCompetitionDynamics(float sourceActivity, float targetActivity,
                                   float existingConnections, float dt);

    // Update noise-driven dynamics
    void updateNoiseDrivenDynamics(float dt);

    // Update developmental dynamics
    void updateDevelopmentalDynamics(float developmentalStage, float dt);

    // Update meta-structural dynamics
    void updateMetaStructuralDynamics(float plasticityLevel, float dt);

    // ===== STATISTICS =====

    // Structural plasticity statistics
    struct StructuralPlasticityStats {
        float formationRate;           // Connection formation rate
        float pruningRate;             // Connection pruning rate
        float homeostaticEffect;       // Homeostatic plasticity effect
        float competitionEffect;       // Competition effect strength
        float noiseEffect;             // Noise-driven effect strength
        float developmentalEffect;     // Developmental effect strength
        float metaPlasticityEffect;    // Meta-structural plasticity effect
        float stabilityDistribution;    // Distribution of connection stabilities
        float ageDistribution;         // Distribution of connection ages
    };

    StructuralPlasticityStats getStats() const { return stats_; }

    // Update statistics
    void updateStats(bool formed, bool pruned, float formationProb,
                    float pruningProb, float homeostaticEffect,
                    float competitionEffect, float noiseEffect,
                    float developmentalEffect, float metaEffect, float dt);

    // ===== UTILITY =====

    // Get connection age in normalized form
    float getNormalizedAge() const { return connectionState_.age / 1000.0f; }

    // Get connection stability
    float getStability() const { return connectionState_.stability; }

    // Update stability based on various factors
    void updateStability(float weightChange, bool formed, bool pruned, float dt);

    // ===== OVERRIDE =====

    // Override EnhancedSynapse::step
    void step(Timestamp currentTime) override {
        stepEnhanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

    // Main enhanced step function
    bool stepEnhanced(Timestamp currentTime, TimestepDuration dt);

    // Apply all structural plasticity rules
    void applyAllStructuralPlasticity(float sourceActivity, float targetActivity,
                                     float spatialDistance, float temporalSimilarity,
                                     float dt);

    // Determine if connection should be formed
    bool shouldFormConnection(float sourceActivity, float targetActivity,
                             float spatialDistance, float temporalSimilarity,
                             float currentWeight, float dt) const;

    // Determine if connection should be pruned
    bool shouldPruneConnection(float sourceActivity, float targetActivity,
                              float age, float stability, float dt) const;

private:
    // Configuration flags
    bool formationEnabled_;
    bool pruningEnabled_;

    // Parameters
    FormationParameters formationParams_;
    PruningParameters pruningParams_;
    HomeostaticStructuralPlasticity homeostaticStructParams_;
    Synaptogenesis synaptogenesisParams_;
    Competition competitionParams_;
    NoiseDrivenChanges noiseDrivenParams_;
    DevelopmentalStructuralPlasticity developmentalStructParams_;
    MetaStructuralPlasticity metaStructParams_;

    // Connection state
    ConnectionState connectionState_;

    // Statistics
    StructuralPlasticityStats stats_;

    // Helper methods
    float calculateHebbianFormation(float sourceActivity, float targetActivity) const;
    float calculateAntiHebbianFormation(float sourceActivity, float targetActivity) const;
    float calculateCoactivationFormation(float sourceActivity, float targetActivity) const;
    float calculateSpatialFormation(float spatialDistance) const;
    float calculateTemporalFormation(float temporalSimilarity) const;
    float calculateAntiHebbianPruning(float sourceActivity, float targetActivity) const;
    float calculateHomeostaticPruning(int32_t currentCount, int32_t targetCount) const;
    float calculateCompetitionPruning(float sourceActivity, float targetActivity,
                                       float existingConnections) const;
    float calculateAgePruning(float age) const;
    float calculateNoiseDrivenPruning(float dt) const;
    float calculateActivityPruning(float sourceActivity, float targetActivity) const;
    void updateConnectionStability(float weightChange, bool formed, bool pruned, float dt);
    void updateConnectionAge(float dt);
    void updateMetaStructuralPlasticity(float plasticityLevel, float dt);
};

} // namespace nlm
