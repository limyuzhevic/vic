#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <string>

namespace nlm {

// Forward declarations
class PerformanceProfiler;
class RandomGenerator;

// Structural plasticity performance metrics
struct StructuralPlasticityPerformanceMetrics {
    // Basic metrics
    size_t totalSynapsesCreated{0};
    size_t totalSynapsesPruned{0};
    float synaptogenesisRate{0.0f};
    float pruningRate{0.0f};
    float currentSynapticDensity{0.0f};
    float totalSynapticStability{0.0f};
    
    // Synaptogenesis metrics
    float hebbianSynaptogenesisRatio{0.0f};
    float localConnectivityRatio{0.0f};
    float activityDependentPrunedRatio{0.0f};
    float competitivePruningRatio{0.0f};
    
    // Development metrics
    float criticalPeriodModulation{0.0f};
    bool experienceExpectantPlasticity{false};
    
    // Spatial constraints
    float maxConnectionDistance{0.0f};
    bool layerSpecificRules{false};
    bool dendriticCompartmentConstraints{false};
    
    // Neuron type-specific metrics
    float excitatorySynapseFormationRate{0.0f};
    float inhibitorySynapseFormationRate{0.0f};
    bool sensorySpecificRules{false};
    bool motorSpecificRules{false};
    
    // Development stage metrics
    DevelopmentalStage developmentStage{DevelopmentalStage::Early};
    float developmentalStageModulation{0.0f};
    
    // Efficiency metrics
    bool batchOperationsEnabled{false};
    size_t batchSize{0};
    bool efficiencyMode{false};
    
    // Performance metrics
    float computationalEfficiency{0.0f};
    float memoryEfficiency{0.0f};
    float timeEfficiency{0.0f};
    double performanceScore{0.0};
    
    // Temporal metrics
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::chrono::steady_clock::time_point firstEventTime;
    float totalActiveTime{0.0f};          // Total time structural plasticity was active
    
    // Statistical metrics
    size_t totalEvents{0};                  // Total structural events
    float averageEventDuration{0.0f};       // Average duration of events
    float stdDevEventDuration{0.0f};       // Standard deviation of event durations
    size_t stableConnections{0};           // Number of stable connections
    size_t unstableConnections{0};         // Number of unstable connections
    
    // Maturation metrics
    size_t maturationStageInitial{0};
    size_t maturationStagePotentiating{0};
    size_t maturationStageStable{0};
    size_t maturationStageDepressing{0};
    size_t maturationStagePruned{0};
    
    // Custom metrics for profiling
    std::unordered_map<std::string, double> customMetrics;
    
    // Reset metrics
    void reset() {
        totalSynapsesCreated = 0;
        totalSynapsesPruned = 0;
        synaptogenesisRate = 0.0f;
        pruningRate = 0.0f;
        currentSynapticDensity = 0.0f;
        totalSynapticStability = 0.0f;
        
        hebbianSynaptogenesisRatio = 0.0f;
        localConnectivityRatio = 0.0f;
        activityDependentPrunedRatio = 0.0f;
        competitivePruningRatio = 0.0f;
        
        criticalPeriodModulation = 0.0f;
        experienceExpectantPlasticity = false;
        
        maxConnectionDistance = 0.0f;
        layerSpecificRules = false;
        dendriticCompartmentConstraints = false;
        
        excitatorySynapseFormationRate = 0.0f;
        inhibitorySynapseFormationRate = 0.0f;
        sensorySpecificRules = false;
        motorSpecificRules = false;
        
        developmentStage = DevelopmentalStage::Early;
        developmentalStageModulation = 0.0f;
        
        batchOperationsEnabled = false;
        batchSize = 0;
        efficiencyMode = false;
        
        computationalEfficiency = 0.0f;
        memoryEfficiency = 0.0f;
        timeEfficiency = 0.0f;
        performanceScore = 0.0;
        
        lastUpdateTime = std::chrono::steady_clock::now();
        firstEventTime = lastUpdateTime;
        totalActiveTime = 0.0f;
        
        totalEvents = 0;
        averageEventDuration = 0.0f;
        stdDevEventDuration = 0.0f;
        stableConnections = 0;
        unstableConnections = 0;
        
        maturationStageInitial = 0;
        maturationStagePotentiating = 0;
        maturationStageStable = 0;
        maturationStageDepressing = 0;
        maturationStagePruned = 0;
        
        customMetrics.clear();
    }
    
    // Update metrics
    void recordSynapseCreated() {
        totalSynapsesCreated++;
        updateMetrics();
    }
    
    void recordSynapsePruned() {
        totalSynapsesPruned++;
        updateMetrics();
    }
    
    void updateMetrics() {
        // Update last update time
        lastUpdateTime = std::chrono::steady_clock::now();
        totalActiveTime = std::chrono::duration<double>(lastUpdateTime - firstEventTime).count();
        
        totalEvents++;
        
        // Calculate rates
        if (firstEventTime.time_since_epoch().count() > 0) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - firstEventTime).count();
            if (elapsed > 0) {
                synaptogenesisRate = static_cast<float>(totalSynapsesCreated) / elapsed;
                pruningRate = static_cast<float>(totalSynapsesPruned) / elapsed;
            }
        }
        
        // Calculate efficiency scores
        computeEfficiencyScores();
    }
    
    // Calculate efficiency scores
    void computeEfficiencyScores() {
        // Computational efficiency
        if (totalEvents > 0) {
            computationalEfficiency = 1.0f / std::max(1.0f, static_cast<float>(totalEvents));
        }
        
        // Memory efficiency
        memoryEfficiency = 1.0f / std::max(1.0f, static_cast<float>(totalSynapsesCreated) / 100.0f);
        
        // Time efficiency
        timeEfficiency = 1.0f / std::max(1.0f, totalActiveTime / 1000.0f);
        
        // Overall performance score
        performanceScore = (computationalEfficiency + memoryEfficiency + timeEfficiency) / 3.0f;
    }
    
    // Getters for specific metrics
    float getHebbianSynaptogenesisRatio() const;
    float getLocalConnectivityRatio() const;
    float getMaturationStageRatio(StructuralPlasticity::SynapseMaturationStage stage) const;
    
    std::unordered_map<std::string, double> getPerformanceSummary() const {
        std::unordered_map<std::string, double> summary;
        
        summary["total_synapses_created"] = static_cast<double>(totalSynapsesCreated);
        summary["total_synapses_pruned"] = static_cast<double>(totalSynapsesPruned);
        summary["synaptogenesis_rate"] = static_cast<double>(synaptogenesisRate);
        summary["pruning_rate"] = static_cast<double>(pruningRate);
        summary["current_synaptic_density"] = static_cast<double>(currentSynapticDensity);
        summary["total_synaptic_stability"] = static_cast<double>(totalSynapticStability);
        summary["hebbian_synaptogenesis_ratio"] = static_cast<double>(hebbianSynaptogenesisRatio);
        summary["local_connectivity_ratio"] = static_cast<double>(localConnectivityRatio);
        summary["activity_dependent_pruned_ratio"] = static_cast<double>(activityDependentPrunedRatio);
        summary["competitive_pruning_ratio"] = static_cast<double>(competitivePruningRatio);
        summary["critical_period_modulation"] = static_cast<double>(criticalPeriodModulation);
        summary["experience_expectant_plasticity"] = experienceExpectantPlasticity ? 1.0 : 0.0;
        summary["max_connection_distance"] = static_cast<double>(maxConnectionDistance);
        summary["layer_specific_rules"] = layerSpecificRules ? 1.0 : 0.0;
        summary["dendritic_compartment_constraints"] = dendriticCompartmentConstraints ? 1.0 : 0.0;
        summary["excitatory_synapse_formation_rate"] = static_cast<double>(excitatorySynapseFormationRate);
        summary["inhibitory_synapse_formation_rate"] = static_cast<double>(inhibitorySynapseFormationRate);
        summary["sensory_specific_rules"] = sensorySpecificRules ? 1.0 : 0.0;
        summary["motor_specific_rules"] = motorSpecificRules ? 1.0 : 0.0;
        summary["development_stage"] = static_cast<double>(developmentStage);
        summary["developmental_stage_modulation"] = static_cast<double>(developmentalStageModulation);
        summary["batch_operations_enabled"] = batchOperationsEnabled ? 1.0 : 0.0;
        summary["batch_size"] = static_cast<double>(batchSize);
        summary["efficiency_mode"] = efficiencyMode ? 1.0 : 0.0;
        summary["computational_efficiency"] = static_cast<double>(computationalEfficiency);
        summary["memory_efficiency"] = static_cast<double>(memoryEfficiency);
        summary["time_efficiency"] = static_cast<double>(timeEfficiency);
        summary["performance_score"] = performanceScore;
        summary["total_active_time_s"] = totalActiveTime;
        summary["total_events"] = static_cast<double>(totalEvents);
        summary["average_event_duration_ms"] = static_cast<double>(averageEventDuration * 1000.0);
        summary["stddev_event_duration_ms"] = static_cast<double>(stdDevEventDuration * 1000.0);
        summary["stable_connections"] = static_cast<double>(stableConnections);
        summary["unstable_connections"] = static_cast<double>(unstableConnections);
        
        return summary;
    }
};

// Structural plasticity configuration
struct StructuralPlasticityConfig {
    // Synaptogenesis parameters
    float synaptogenesisRate{0.1f};               // Rate of new synapse formation
    float hebbianLearningRate{0.5f};             // Hebbian learning rate for synaptogenesis
    float targetSynapticDensity{1.0f};           // Target synaptic density
    float criticalPeriodModulation{1.0f};       // Critical period modulation
    bool experienceExpectantPlasticity{true};   // Experience-expectant plasticity
    
    // Pruning parameters
    float pruningRate{0.05f};                   // Rate of synapse pruning
    float pruningThreshold{0.1f};               // Threshold for pruning
    float activityThreshold{0.5f};              // Activity threshold for pruning
    float competitivePruningLevel{0.8f};        // Level of competitive pruning
    float homeostaticTargetFiringRate{2.0f};    // Target firing rate for homeostatic pruning
    
    // Spatial constraints
    float maxConnectionDistance{100.0f};        // Maximum connection distance
    bool layerSpecificRules{false};             // Enable layer-specific rules
    bool dendriticCompartmentConstraints{false}; // Enable dendritic compartment constraints
    
    // Neuron type-specific parameters
    float excitatorySynapseFormationRate{0.8f};
    float inhibitorySynapseFormationRate{0.3f};
    bool sensorySpecificRules{false};
    bool motorSpecificRules{false};
    
    // Development stage parameters
    DevelopmentalStage developmentStage{DevelopmentalStage::Early};
    float developmentalStageModulation{1.0f};  // Modulation of development stage
    
    // Efficiency parameters
    bool batchOperations{false};                 // Enable batch operations
    size_t batchSize{10};                        // Batch size
    bool efficiencyMode{false};                  // Efficiency mode
    
    // Validation
    bool validate() const {
        return synaptogenesisRate >= 0.0f && pruningRate >= 0.0f &&
               targetSynapticDensity > 0.0f && maxConnectionDistance > 0.0f &&
               batchSize > 0;
    }
};

// Structural plasticity: biologically realistic creation and removal of synapses and neurons
// Advanced implementation with activity-dependent synaptogenesis, competitive pruning,
// development-stage plasticity, spatial constraints, and neuron type-specific rules

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Disable copying
    StructuralPlasticity(const StructuralPlasticity&) = delete;
    StructuralPlasticity& operator=(const StructuralPlasticity&) = delete;
    
    // Performance monitoring
    void setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    StructuralPlasticityPerformanceMetrics getPerformanceMetrics() const;
    void recordPerformanceMetrics();
    std::unordered_map<std::string, double> getPerformanceSummary() const;
    
    // ========== SYNAPTOGENESIS ALGORITHMS ==========
    
    // Create new synapse with biologically realistic rules
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapseType type = SynapseType::Excitatory,
                           float baseWeight = 0.1f);
    
    // Activity-dependent synaptogenesis based on Hebbian rule
    bool createSynapseHebbian(Brain* brain, NeuronId pre, NeuronId post, 
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              RandomGenerator& rng);
    
    // Spatially-constrained synaptogenesis (short-range connections)
    bool createSynapseLocal(Brain* brain, NeuronId source, NeuronId destination,
                           float maxDistance, RandomGenerator& rng);
    
    // Neuron type-specific synaptogenesis rules
    bool createSynapseForType(Brain* brain, NeuronId source, NeuronId destination,
                              NeuronType sourceType, NeuronType destType,
                              RandomGenerator& rng);
    
    // Development-stage modulated synaptogenesis
    bool createSynapseForDevelopment(Brain* brain, NeuronId source, NeuronId destination,
                                    DevelopmentalStage stage, RandomGenerator& rng);
    
    // ========== PRUNING ALGORITHMS ==========
    
    // Remove synapse with homeostatic regulation
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Competitive pruning for stability (total synaptic competition)
    void pruneCompetitively(Brain* brain, RandomGenerator& rng, float competitionLevel);
    
    // Per-neuron competitive pruning (maintain stable firing rates)
    void prunePerNeuron(Brain* brain, NeuronId neuron, RandomGenerator& rng);
    
    // Activity-dependent pruning (eliminate unused synapses)
    void pruneUnused(Brain* brain, RandomGenerator& rng);
    
    // Development-stage based pruning rates
    void pruneForDevelopment(Brain* brain, DevelopmentalStage stage, RandomGenerator& rng);
    
    // Remove neuron and all its synapses
    NeuronId createNeuron(Brain* brain, NeuronType type, float metabolicCost = 1.0f);
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // ========== SYNAPSE PROPERTIES AND MATURATION ==========
    
    // Synapse maturation stages
    enum class SynapseMaturationStage {
        Initial,      // Newly formed, weak connection
        Potentiating, // Strengthening through activity
        Stable,       // Fully functional, stable weight
        Depressing,   // Weakening due to lack of use
        Pruned        // Marked for elimination
    };
    
    // Track synapse stability and maturity
    bool stabilizeSynapse(Brain* brain, SynapseId synapse, float stabilityThreshold);
    bool matureSynapse(Brain* brain, SynapseId synapse, SynapseMaturationStage stage,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes);
    
    // Synaptic weakening before pruning
    bool weakenSynapse(Brain* brain, SynapseId synapse, float decayRate);
    
    // ========== CONFIGURATION AND PARAMETERS ==========
    
    // Synaptogenesis configuration
    void setSynaptogenesisRate(float rate);
    void setHebbianLearningRate(float rate);
    void setTargetSynapticDensity(float density);
    void setCriticalPeriodModulation(float modulation);
    void setExperienceExpectantPlasticity(bool enabled);
    \n    // Pruning configuration
    void setPruningRate(float rate);
    void setPruningThreshold(float threshold);
    void setActivityThreshold(float threshold);
    void setCompetitivePruningLevel(float level);
    void setHomeostaticTargetFiringRate(float rate);
    
    // Spatial constraints
    void setMaxConnectionDistance(float distance);
    void setLayerSpecificRules(bool enabled);
    void setDendriticCompartmentConstraints(bool enabled);
    
    // Neuron type-specific rules
    void setExcitatorySynapseFormationRate(float rate);
    void setInhibitorySynapseFormationRate(float rate);
    void setSensorySpecificRules(bool enabled);
    void setMotorSpecificRules(bool enabled);
    
    // Development stage configuration
    void setDevelopmentStage(DevelopmentalStage stage);
    void setDevelopmentalStageModulation(float modulation);
    
    // Efficiency and batch operations
    void enableBatchOperations(bool enabled);
    void setBatchSize(size_t size);
    void setEfficiencyMode(bool enabled);
    
    // Statistics and monitoring
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    float getCurrentSynapticDensity() const;
    float getTotalSynapticStability() const;
    size_t getTotalSynapsesCreated() const;
    size_t getTotalSynapsesPruned() const;
    float getActivityDependentPrunedRatio() const;
    float getCompetitivePruningRatio() const;
    
    // Getters for specific metrics
    float getHebbianSynaptogenesisRatio() const;
    float getLocalConnectivityRatio() const;
    float getMaturationStageRatio(SynapseMaturationStage stage) const;
    size_t getStablyConnectedSynapses() const;
    size_t getUnstableSynapses() const;
    
    // ========== UPDATE AND LIFECYCLE ==========
    
    // Update structural plasticity with all algorithms
    void update(Brain* brain, RandomGenerator& rng);
    
    // Batch synaptogenesis for efficiency
    std::vector<SynapseId> batchCreateSynapses(Brain* brain,
                                            const std::vector<std::pair<NeuronId, NeuronId>>& connections,
                                            RandomGenerator& rng);
    
    // Batch pruning for efficiency
    void batchRemoveSynapses(Brain* brain,
                           const std::vector<SynapseId>& synapseIds);
    
    // Reset structural plasticity state
    void reset();
    
    // Validation of configuration parameters
    bool validateConfiguration() const;
    
    // Reorganize network for structural plasticity
    void reorganizeNetwork(Brain* brain, RandomGenerator& rng);
    
    // Structural memory tracking
    void recordStructuralEvent(SynapseId synapse, float weightChange, Timestamp timestamp);
    float getStructuralMemoryScore(NeuronId neuron) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Performance monitoring integration
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable StructuralPlasticityPerformanceMetrics perfMetrics;
};

} // namespace nlm