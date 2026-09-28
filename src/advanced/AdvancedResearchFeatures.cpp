#include "AdvancedResearchFeatures.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

// CustomPlasticityRules implementation
void CustomPlasticityRules::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("CustomPlasticityRules initialized");
}

void CustomPlasticityRules::enableCustomSTDP(float maxWeightChange, 
                                             float timeConstant,
                                             float learningRate) {
    customSTDPEnabled_ = true;
    customSTDPMaxWeightChange_ = maxWeightChange;
    customSTDPTimeConstant_ = timeConstant;
    customSTDPGrowthRate_ = learningRate;
    NLM_LOG_INFO("Custom STDP rule enabled: max_weight_change=" + std::to_string(maxWeightChange) +
                 ", time_constant=" + std::to_string(timeConstant) +
                 ", learning_rate=" + std::to_string(learningRate));
}

void CustomPlasticityRules::enableCustomHebbian(float strength,
                                               float decayRate,
                                               float competitionFactor) {
    customHebbianEnabled_ = true;
    customHebbianStrength_ = strength;
    customHebbianDecayRate_ = decayRate;
    customHebbianCompetitionFactor_ = competitionFactor;
    NLM_LOG_INFO("Custom Hebbian rule enabled: strength=" + std::to_string(strength) +
                 ", decay_rate=" + std::to_string(decayRate) +
                 ", competition_factor=" + std::to_string(competitionFactor));
}

void CustomPlasticityRules::enableCustomStructuralPlasticity(float synaptogenesisBase,
                                                           float pruningBase,
                                                           float ageModulation) {
    customStructuralPlasticityEnabled_ = true;
    customSynaptogenesisBaseRate_ = synaptogenesisBase;
    customPruningBaseRate_ = pruningBase;
    customAgeModulationFactor_ = ageModulation;
    NLM_LOG_INFO("Custom structural plasticity enabled: synaptogenesis_base=" + std::to_string(synaptogenesisBase) +
                 ", pruning_base=" + std::to_string(pruningBase) +
                 ", age_modulation=" + std::to_string(ageModulation));
}

void CustomPlasticityRules::enableCustomNeuromodulation(float dopamineGain,
                                                       float curiosityGain,
                                                       float noveltyGain,
                                                       float predictionErrorGain) {
    customNeuromodulationEnabled_ = true;
    customDopamineGain_ = dopamineGain;
    customCuriosityGain_ = curiosityGain;
    customNoveltyGain_ = noveltyGain;
    customPredictionErrorGain_ = predictionErrorGain;
    NLM_LOG_INFO("Custom neuromodulation enabled: dopamine_gain=" + std::to_string(dopamineGain) +
                 ", curiosity_gain=" + std::to_string(curiosityGain) +
                 ", novelty_gain=" + std::to_string(noveltyGain) +
                 ", prediction_error_gain=" + std::to_string(predictionErrorGain));
}

void CustomPlasticityRules::enableCustomDevelopment(float plasticityCurveExponent,
                                                   float criticalPeriodStart,
                                                   float maturityStart,
                                                   float agingStart) {
    customDevelopmentEnabled_ = true;
    customPlasticityCurveExponent_ = plasticityCurveExponent;
    customCriticalPeriodStart_ = criticalPeriodStart;
    customMaturityStart_ = maturityStart;
    customAgingStart_ = agingStart;
    NLM_LOG_INFO("Custom development system enabled: plasticity_curve_exponent=" + std::to_string(plasticityCurveExponent) +
                 ", critical_period_start=" + std::to_string(criticalPeriodStart) +
                 ", maturity_start=" + std::to_string(maturityStart) +
                 ", aging_start=" + std::to_string(agingStart));
}

void CustomPlasticityRules::configureExperimentParameters(const std::string& experimentType,
                                                         const std::map<std::string, float>& parameters) {
    NLM_LOG_INFO("Configuring experiment parameters: type=" + experimentType +
                 ", parameters count=" + std::to_string(parameters.size()));
    
    for (const auto& param : parameters) {
        NLM_LOG_INFO("  " + param.first + " = " + std::to_string(param.second));
    }
}

std::string CustomPlasticityRules::getConfiguration() const {
    std::stringstream ss;
    ss << "Custom Plasticity Rules Configuration:\n";
    ss << "  Custom STDP: " << (customSTDPEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (customSTDPEnabled_) {
        ss << "    Max weight change: " << customSTDPMaxWeightChange_ << "\n";
        ss << "    Time constant: " << customSTDPTimeConstant_ << "\n";
        ss << "    Growth rate: " << customSTDPGrowthRate_ << "\n";
    }
    
    ss << "  Custom Hebbian: " << (customHebbianEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (customHebbianEnabled_) {
        ss << "    Strength: " << customHebbianStrength_ << "\n";
        ss << "    Decay rate: " << customHebbianDecayRate_ << "\n";
        ss << "    Competition factor: " << customHebbianCompetitionFactor_ << "\n";
    }
    
    ss << "  Custom Structural Plasticity: " << (customStructuralPlasticityEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (customStructuralPlasticityEnabled_) {
        ss << "    Synaptogenesis base rate: " << customSynaptogenesisBaseRate_ << "\n";
        ss << "    Pruning base rate: " << customPruningBaseRate_ << "\n";
        ss << "    Age modulation factor: " << customAgeModulationFactor_ << "\n";
    }
    
    ss << "  Custom Neuromodulation: " << (customNeuromodulationEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (customNeuromodulationEnabled_) {
        ss << "    Dopamine gain: " << customDopamineGain_ << "\n";
        ss << "    Curiosity gain: " << customCuriosityGain_ << "\n";
        ss << "    Novelty gain: " << customNoveltyGain_ << "\n";
        ss << "    Prediction error gain: " << customPredictionErrorGain_ << "\n";
    }
    
    ss << "  Custom Development: " << (customDevelopmentEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (customDevelopmentEnabled_) {
        ss << "    Plasticity curve exponent: " << customPlasticityCurveExponent_ << "\n";
        ss << "    Critical period start: " << customCriticalPeriodStart_ << "\n";
        ss << "    Maturity start: " << customMaturityStart_ << "\n";
        ss << "    Aging start: " << customAgingStart_ << "\n";
    }
    
    return ss.str();
}

void CustomPlasticityRules::resetToDefaults() {
    customSTDPEnabled_ = false;
    customHebbianEnabled_ = false;
    customStructuralPlasticityEnabled_ = false;
    customNeuromodulationEnabled_ = false;
    customDevelopmentEnabled_ = false;
    NLM_LOG_INFO("Custom plasticity rules reset to defaults");
}

// AdvancedMemorySystems implementation
void AdvancedMemorySystems::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("AdvancedMemorySystems initialized");
}

void AdvancedMemorySystems::enableTemporalWorkingMemory(float temporalDecay,
                                                        float predictionGain,
                                                        float interferenceStrength) {
    temporalWorkingMemoryEnabled_ = true;
    temporalDecay_ = temporalDecay;
    predictionGain_ = predictionGain;
    interferenceStrength_ = interferenceStrength;
    NLM_LOG_INFO("Temporal working memory enabled: temporal_decay=" + std::to_string(temporalDecay) +
                 ", prediction_gain=" + std::to_string(predictionGain) +
                 ", interference_strength=" + std::to_string(interferenceStrength));
}

void AdvancedMemorySystems::enableReplayDynamics(float replayStrength,
                                                 float consolidationThreshold,
                                                 float replayFrequency) {
    replayDynamicsEnabled_ = true;
    replayStrength_ = replayStrength;
    consolidationThreshold_ = consolidationThreshold;
    replayFrequency_ = replayFrequency;
    NLM_LOG_INFO("Replay dynamics enabled: replay_strength=" + std::to_string(replayStrength) +
                 ", consolidation_threshold=" + std::to_string(consolidationThreshold) +
                 ", replay_frequency=" + std::to_string(replayFrequency));
}

void AdvancedMemorySystems::enableSemanticMemory(float vectorDimension,
                                                 float similarityThreshold,
                                                 float learningRate) {
    semanticMemoryEnabled_ = true;
    vectorDimension_ = vectorDimension;
    similarityThreshold_ = similarityThreshold;
    semanticLearningRate_ = learningRate;
    NLM_LOG_INFO("Semantic memory enabled: vector_dimension=" + std::to_string(vectorDimension) +
                 ", similarity_threshold=" + std::to_string(similarityThreshold) +
                 ", learning_rate=" + std::to_string(learningRate));
}

void AdvancedMemorySystems::enableProceduralMemory(float eligibilityTraceDecay,
                                                   float rewardPredictionGain,
                                                   float policyLearningRate) {
    proceduralMemoryEnabled_ = true;
    eligibilityTraceDecay_ = eligibilityTraceDecay;
    rewardPredictionGain_ = rewardPredictionGain;
    policyLearningRate_ = policyLearningRate;
    NLM_LOG_INFO("Procedural memory enabled: eligibility_trace_decay=" + std::to_string(eligibilityTraceDecay) +
                 ", reward_prediction_gain=" + std::to_string(rewardPredictionGain) +
                 ", policy_learning_rate=" + std::to_string(policyLearningRate));
}

void AdvancedMemorySystems::configureMemoryIntegration(const std::string& integrationPattern,
                                                      const std::map<std::string, float>& weights) {
    NLM_LOG_INFO("Configuring memory integration: pattern=" + integrationPattern +
                 ", weights count=" + std::to_string(weights.size()));
    
    for (const auto& weight : weights) {
        NLM_LOG_INFO("  " + weight.first + " = " + std::to_string(weight.second));
    }
}

std::string AdvancedMemorySystems::getMemoryStatistics() const {
    std::stringstream ss;
    ss << "Advanced Memory Systems Statistics:\n";
    ss << "  Temporal Working Memory: " << (temporalWorkingMemoryEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (temporalWorkingMemoryEnabled_) {
        ss << "    Temporal decay: " << temporalDecay_ << "\n";
        ss << "    Prediction gain: " << predictionGain_ << "\n";
        ss << "    Interference strength: " << interferenceStrength_ << "\n";
    }
    
    ss << "  Replay Dynamics: " << (replayDynamicsEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (replayDynamicsEnabled_) {
        ss << "    Replay strength: " << replayStrength_ << "\n";
        ss << "    Consolidation threshold: " << consolidationThreshold_ << "\n";
        ss << "    Replay frequency: " << replayFrequency_ << "\n";
    }
    
    ss << "  Semantic Memory: " << (semanticMemoryEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (semanticMemoryEnabled_) {
        ss << "    Vector dimension: " << vectorDimension_ << "\n";
        ss << "    Similarity threshold: " << similarityThreshold_ << "\n";
        ss << "    Learning rate: " << semanticLearningRate_ << "\n";
    }
    
    ss << "  Procedural Memory: " << (proceduralMemoryEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (proceduralMemoryEnabled_) {
        ss << "    Eligibility trace decay: " << eligibilityTraceDecay_ << "\n";
        ss << "    Reward prediction gain: " << rewardPredictionGain_ << "\n";
        ss << "    Policy learning rate: " << policyLearningRate_ << "\n";
    }
    
    return ss.str();
}

void AdvancedMemorySystems::resetMemorySystems() {
    temporalWorkingMemoryEnabled_ = false;
    replayDynamicsEnabled_ = false;
    semanticMemoryEnabled_ = false;
    proceduralMemoryEnabled_ = false;
    NLM_LOG_INFO("Advanced memory systems reset");
}

// AdvancedCognitionSystems implementation
void AdvancedCognitionSystems::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("AdvancedCognitionSystems initialized");
}

void AdvancedCognitionSystems::enableTemporalAbstractionPlanning(float planningHorizon,
                                                                 float temporalDiscount,
                                                                 float uncertaintyWeight) {
    temporalAbstractionPlanningEnabled_ = true;
    planningHorizon_ = planningHorizon;
    temporalDiscount_ = temporalDiscount;
    uncertaintyWeight_ = uncertaintyWeight;
    NLM_LOG_INFO("Temporal abstraction planning enabled: planning_horizon=" + std::to_string(planningHorizon) +
                 ", temporal_discount=" + std::to_string(temporalDiscount) +
                 ", uncertainty_weight=" + std::to_string(uncertaintyWeight));
}

void AdvancedCognitionSystems::enableHierarchicalConceptFormation(float hierarchyDepth,
                                                                   float clusteringThreshold,
                                                                   float conceptStability) {
    hierarchicalConceptFormationEnabled_ = true;
    hierarchyDepth_ = hierarchyDepth;
    clusteringThreshold_ = clusteringThreshold;
    conceptStability_ = conceptStability;
    NLM_LOG_INFO("Hierarchical concept formation enabled: hierarchy_depth=" + std::to_string(hierarchyDepth) +
                 ", clustering_threshold=" + std::to_string(clusteringThreshold) +
                 ", concept_stability=" + std::to_string(conceptStability));
}

void AdvancedCognitionSystems::enableResourceAllocationAttention(float allocationEfficiency,
                                                                float noveltyAllocation,
                                                                float habituationRate) {
    resourceAllocationAttentionEnabled_ = true;
    allocationEfficiency_ = allocationEfficiency;
    noveltyAllocation_ = noveltyAllocation;
    habituationRate_ = habituationRate;
    NLM_LOG_INFO("Resource allocation attention enabled: allocation_efficiency=" + std::to_string(allocationEfficiency) +
                 ", novelty_allocation=" + std::to_string(noveltyAllocation) +
                 ", habituation_rate=" + std::to_string(habituationRate));
}

void AdvancedCognitionSystems::configureCognitiveArchitecture(const std::string& architectureType,
                                                              const std::map<std::string, float>& parameters) {
    NLM_LOG_INFO("Configuring cognitive architecture: type=" + architectureType +
                 ", parameters count=" + std::to_string(parameters.size()));
    
    for (const auto& param : parameters) {
        NLM_LOG_INFO("  " + param.first + " = " + std::to_string(param.second));
    }
}

std::string AdvancedCognitionSystems::getCognitionStatistics() const {
    std::stringstream ss;
    ss << "Advanced Cognition Systems Statistics:\n";
    ss << "  Temporal Abstraction Planning: " << (temporalAbstractionPlanningEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (temporalAbstractionPlanningEnabled_) {
        ss << "    Planning horizon: " << planningHorizon_ << "\n";
        ss << "    Temporal discount: " << temporalDiscount_ << "\n";
        ss << "    Uncertainty weight: " << uncertaintyWeight_ << "\n";
    }
    
    ss << "  Hierarchical Concept Formation: " << (hierarchicalConceptFormationEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (hierarchicalConceptFormationEnabled_) {
        ss << "    Hierarchy depth: " << hierarchyDepth_ << "\n";
        ss << "    Clustering threshold: " << clusteringThreshold_ << "\n";
        ss << "    Concept stability: " << conceptStability_ << "\n";
    }
    
    ss << "  Resource Allocation Attention: " << (resourceAllocationAttentionEnabled_ ? "ENABLED" : "DISABLED") << "\n";
    if (resourceAllocationAttentionEnabled_) {
        ss << "    Allocation efficiency: " << allocationEfficiency_ << "\n";
        ss << "    Novelty allocation: " << noveltyAllocation_ << "\n";
        ss << "    Habituation rate: " << habituationRate_ << "\n";
    }
    
    return ss.str();
}

void AdvancedCognitionSystems::resetCognitionSystems() {
    temporalAbstractionPlanningEnabled_ = false;
    hierarchicalConceptFormationEnabled_ = false;
    resourceAllocationAttentionEnabled_ = false;
    NLM_LOG_INFO("Advanced cognition systems reset");
}
// AdvancedResearchFeatures implementation
void AdvancedResearchFeatures::initialize(Brain* brain) {
    brain_ = brain;
    customPlasticityRules_.initialize(brain);
    advancedMemorySystems_.initialize(brain);
    advancedCognitionSystems_.initialize(brain);
    
    NLM_LOG_INFO("AdvancedResearchFeatures initialized");
}

void AdvancedResearchFeatures::enableAllAdvancedFeatures() {
    customPlasticityRules_.enableCustomSTDP();
    customPlasticityRules_.enableCustomHebbian();
    customPlasticityRules_.enableCustomStructuralPlasticity();
    customPlasticityRules_.enableCustomNeuromodulation();
    customPlasticityRules_.enableCustomDevelopment();
    
    advancedMemorySystems_.enableTemporalWorkingMemory();
    advancedMemorySystems_.enableReplayDynamics();
    advancedMemorySystems_.enableSemanticMemory();
    advancedMemorySystems_.enableProceduralMemory();
    
    advancedCognitionSystems_.enableTemporalAbstractionPlanning();
    advancedCognitionSystems_.enableHierarchicalConceptFormation();
    advancedCognitionSystems_.enableResourceAllocationAttention();
    
    featureStatus_["CustomPlasticityRules"] = true;
    featureStatus_["AdvancedMemorySystems"] = true;
    featureStatus_["AdvancedCognitionSystems"] = true;
    
    NLM_LOG_INFO("All advanced features enabled");
}
void AdvancedResearchFeatures::configureExperimentalProtocol(const std::string& protocolName,
                                                              const std::map<std::string, std::string>& settings) {
    NLM_LOG_INFO("Configuring experimental protocol: name=" + protocolName +
                 ", settings count=" + std::to_string(settings.size()));
    
    for (const auto& setting : settings) {
        NLM_LOG_INFO("  " + setting.first + " = " + setting.second);
    }
}

bool AdvancedResearchFeatures::isFeatureEnabled(const std::string& featureName) const {
    auto it = featureStatus_.find(featureName);
    return (it != featureStatus_.end()) ? it->second : false;
}

std::string AdvancedResearchFeatures::getSystemReport() const {
    std::stringstream ss;
    ss << "=== Advanced Research Features System Report ===\n";
    ss << "Custom Plasticity Rules:\n";
    ss << customPlasticityRules_.getConfiguration();
    ss << "\nAdvanced Memory Systems:\n";
    ss << advancedMemorySystems_.getMemoryStatistics();
    ss << "\nAdvanced Cognition Systems:\n";
    ss << advancedCognitionSystems_.getCognitionStatistics();
    
    ss << "\n=== Feature Status ===\n";
    for (const auto& feature : featureStatus_) {
        ss << feature.first << ": " << (feature.second ? "ENABLED" : "DISABLED") << "\n";
    }
    
    return ss.str();
}

void AdvancedResearchFeatures::resetToStandardConfiguration() {
    customPlasticityRules_.resetToDefaults();
    advancedMemorySystems_.resetMemorySystems();
    advancedCognitionSystems_.resetCognitionSystems();
    
    featureStatus_.clear();
    NLM_LOG_INFO("Advanced research features reset to standard configuration");
}

} // namespace nlm