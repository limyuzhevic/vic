// AblationSystem.cpp - Implementation of the ablation framework

#include "experiments/AblationSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

AblationResult::AblationResult() 
    : totalReward(0.0f)
    , avgRewardPerStep(0.0f)
    , learningEfficiency(0.0f)
    , finalPerformance(0.0f)
    , avgFiringRate(0.0f)
    , spikeCount(0.0f)
    , synapticWeightSum(0.0f)
    , excitatoryInhibitoryRatio(0.0f)
    , memoryRetention(0.0f)
    , memoryRecall(0.0f)
    , predictionAccuracy(0.0f)
    , predictionError(0.0f)
    , activityStability(0.0f)
    , weightStability(0.0f)
    , explorationRate(0.0f)
    , goalDirectedness(0.0f)
    , adaptationSpeed(0.0f)
    , performanceVsFull(0.0f)
    , statisticalSignificance(0.0f)
    , isSignificant(false)
    , stepsRun(0)
    , computationTime(0.0)
    , memoryUsageMB(0) {}

AblationSystem::AblationSystem() {
    // Initialize mechanism handlers for each ablation target
    mechanismHandlers_[AblationTarget::STDP] = [this](bool enable) {
        return handleSTDP(enable);
    };
    mechanismHandlers_[AblationTarget::Hebbian] = [this](bool enable) {
        return handleHebbian(enable);
    };
    mechanismHandlers_[AblationTarget::RewardModulation] = [this](bool enable) {
        return handleRewardModulation(enable);
    };
    mechanismHandlers_[AblationTarget::EligibilityTraces] = [this](bool enable) {
        return handleEligibilityTraces(enable);
    };
    mechanismHandlers_[AblationTarget::Synaptogenesis] = [this](bool enable) {
        return handleSynaptogenesis(enable);
    };
    mechanismHandlers_[AblationTarget::Pruning] = [this](bool enable) {
        return handlePruning(enable);
    };
    mechanismHandlers_[AblationTarget::StructuralPlasticity] = [this](bool enable) {
        return handleStructuralPlasticity(enable);
    };
    mechanismHandlers_[AblationTarget::Dopamine] = [this](bool enable) {
        return handleDopamine(enable);
    };
    mechanismHandlers_[AblationTarget::Novelty] = [this](bool enable) {
        return handleNovelty(enable);
    };
    mechanismHandlers_[AblationTarget::Curiosity] = [this](bool enable) {
        return handleCuriosity(enable);
    };
    mechanismHandlers_[AblationTarget::Neuromodulation] = [this](bool enable) {
        return handleNeuromodulation(enable);
    };
    mechanismHandlers_[AblationTarget::WorkingMemory] = [this](bool enable) {
        return handleWorkingMemory(enable);
    };
    mechanismHandlers_[AblationTarget::EpisodicMemory] = [this](bool enable) {
        return handleEpisodicMemory(enable);
    };
    mechanismHandlers_[AblationTarget::SemanticMemory] = [this](bool enable) {
        return handleSemanticMemory(enable);
    };
    mechanismHandlers_[AblationTarget::ProceduralMemory] = [this](bool enable) {
        return handleProceduralMemory(enable);
    };
    mechanismHandlers_[AblationTarget::Memory] = [this](bool enable) {
        return handleMemory(enable);
    };
    mechanismHandlers_[AblationTarget::Prediction] = [this](bool enable) {
        return handlePrediction(enable);
    };
    mechanismHandlers_[AblationTarget::Attention] = [this](bool enable) {
        return handleAttention(enable);
    };
    mechanismHandlers_[AblationTarget::Planning] = [this](bool enable) {
        return handlePlanning(enable);
    };
    mechanismHandlers_[AblationTarget::SelfModel] = [this](bool enable) {
        return handleSelfModel(enable);
    };
    mechanismHandlers_[AblationTarget::Vision] = [this](bool enable) {
        return handleVision(enable);
    };
    mechanismHandlers_[AblationTarget::Audio] = [this](bool enable) {
        return handleAudio(enable);
    };
    mechanismHandlers_[AblationTarget::Touch] = [this](bool enable) {
        return handleTouch(enable);
    };
    mechanismHandlers_[AblationTarget::Proprioception] = [this](bool enable) {
        return handleProprioception(enable);
    };
    mechanismHandlers_[AblationTarget::InternalSensing] = [this](bool enable) {
        return handleInternalSensing(enable);
    };
    mechanismHandlers_[AblationTarget::ExcitatoryNeurons] = [this](bool enable) {
        return handleExcitatoryNeurons(enable);
    };
    mechanismHandlers_[AblationTarget::InhibitoryNeurons] = [this](bool enable) {
        return handleInhibitoryNeurons(enable);
    };
    mechanismHandlers_[AblationTarget::SensoryNeurons] = [this](bool enable) {
        return handleSensoryNeurons(enable);
    };
    mechanismHandlers_[AblationTarget::MotorNeurons] = [this](bool enable) {
        return handleMotorNeurons(enable);
    };
    
    // Group targets
    mechanismHandlers_[AblationTarget::AllPlasticity] = [this](bool enable) {
        return handleAllPlasticity(enable);
    };
    mechanismHandlers_[AblationTarget::AllNeuromodulation] = [this](bool enable) {
        return handleAllNeuromodulation(enable);
    };
    mechanismHandlers_[AblationTarget::AllMemory] = [this](bool enable) {
        return handleAllMemory(enable);
    };
    mechanismHandlers_[AblationTarget::AllSensation] = [this](bool enable) {
        return handleAllSensation(enable);
    };
    mechanismHandlers_[AblationTarget::AllCognition] = [this](bool enable) {
        return handleAllCognition(enable);
    };
}

AblationSystem::~AblationSystem() = default;

bool AblationSystem::applyAblation(const AblationConfig& config) {
    bool success = true;
    
    // Store current config
    currentConfig_ = config;
    
    // Apply each disabled target
    for (auto target : config.disabledTargets) {
        if (target == AblationTarget::None) continue;
        
        auto it = mechanismHandlers_.find(target);
        if (it != mechanismHandlers_.end()) {
            bool result = it->second(false); // false = disable mechanism
            if (!result) {
                NLM_LOG_ERROR("Failed to disable mechanism: " + getTargetName(target));
                success = false;
            }
        }
    }
    
    return success;
}

bool AblationSystem::resetToFull() {
    currentConfig_.enableAll();
    return true;
}

bool AblationSystem::isMechanismDisabled(AblationTarget target) const {
    return currentConfig_.isDisabled(target);
}

void AblationSystem::setBaseline(const std::string& name) {
    baselineName_ = name;
    NLM_LOG_INFO("Baseline set to: " + name);
}

bool AblationSystem::handleSTDP(bool enable) {
    NLM_LOG_INFO(std::string("STDP ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleHebbian(bool enable) {
    NLM_LOG_INFO(std::string("Hebbian ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleRewardModulation(bool enable) {
    NLM_LOG_INFO(std::string("Reward modulation ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleEligibilityTraces(bool enable) {
    NLM_LOG_INFO(std::string("Eligibility traces ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleSynaptogenesis(bool enable) {
    NLM_LOG_INFO(std::string("Synaptogenesis ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handlePruning(bool enable) {
    NLM_LOG_INFO(std::string("Pruning ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleStructuralPlasticity(bool enable) {
    NLM_LOG_INFO(std::string("Structural plasticity ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleDopamine(bool enable) {
    NLM_LOG_INFO(std::string("Dopamine ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleNovelty(bool enable) {
    NLM_LOG_INFO(std::string("Novelty ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleCuriosity(bool enable) {
    NLM_LOG_INFO(std::string("Curiosity ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleNeuromodulation(bool enable) {
    NLM_LOG_INFO(std::string("Neuromodulation ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleWorkingMemory(bool enable) {
    NLM_LOG_INFO(std::string("Working memory ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleEpisodicMemory(bool enable) {
    NLM_LOG_INFO(std::string("Episodic memory ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleSemanticMemory(bool enable) {
    NLM_LOG_INFO(std::string("Semantic memory ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleProceduralMemory(bool enable) {
    NLM_LOG_INFO(std::string("Procedural memory ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleMemory(bool enable) {
    NLM_LOG_INFO(std::string("Memory ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handlePrediction(bool enable) {
    NLM_LOG_INFO(std::string("Prediction ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleAttention(bool enable) {
    NLM_LOG_INFO(std::string("Attention ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handlePlanning(bool enable) {
    NLM_LOG_INFO(std::string("Planning ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleSelfModel(bool enable) {
    NLM_LOG_INFO(std::string("Self model ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleVision(bool enable) {
    NLM_LOG_INFO(std::string("Vision ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleAudio(bool enable) {
    NLM_LOG_INFO(std::string("Audio ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleTouch(bool enable) {
    NLM_LOG_INFO(std::string("Touch ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleProprioception(bool enable) {
    NLM_LOG_INFO(std::string("Proprioception ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleInternalSensing(bool enable) {
    NLM_LOG_INFO(std::string("Internal sensing ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleExcitatoryNeurons(bool enable) {
    NLM_LOG_INFO(std::string("Excitatory neurons ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleInhibitoryNeurons(bool enable) {
    NLM_LOG_INFO(std::string("Inhibitory neurons ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleSensoryNeurons(bool enable) {
    NLM_LOG_INFO(std::string("Sensory neurons ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleMotorNeurons(bool enable) {
    NLM_LOG_INFO(std::string("Motor neurons ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleAllPlasticity(bool enable) {
    NLM_LOG_INFO(std::string("All plasticity ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleAllNeuromodulation(bool enable) {
    NLM_LOG_INFO(std::string("All neuromodulation ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleAllMemory(bool enable) {
    NLM_LOG_INFO(std::string("All memory ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleAllSensation(bool enable) {
    NLM_LOG_INFO(std::string("All sensation ") + (enable ? "enabled" : "disabled"));
    return true;
}

bool AblationSystem::handleAllCognition(bool enable) {
    NLM_LOG_INFO(std::string("All cognition ") + (enable ? "enabled" : "disabled"));
    return true;
}

AblationResult AblationSystem::createBaselineResult() {
    AblationResult result;
    result.name = "Baseline (full system)";
    return result;
}

std::string AblationSystem::getTargetName(AblationTarget target) {
    switch (target) {
        case AblationTarget::STDP: return "STDP";
        case AblationTarget::Hebbian: return "Hebbian";
        case AblationTarget::RewardModulation: return "RewardModulation";
        case AblationTarget::EligibilityTraces: return "EligibilityTraces";
        case AblationTarget::Synaptogenesis: return "Synaptogenesis";
        case AblationTarget::Pruning: return "Pruning";
        case AblationTarget::StructuralPlasticity: return "StructuralPlasticity";
        case AblationTarget::Dopamine: return "Dopamine";
        case AblationTarget::Novelty: return "Novelty";
        case AblationTarget::Curiosity: return "Curiosity";
        case AblationTarget::Neuromodulation: return "Neuromodulation";
        case AblationTarget::WorkingMemory: return "WorkingMemory";
        case AblationTarget::EpisodicMemory: return "EpisodicMemory";
        case AblationTarget::SemanticMemory: return "SemanticMemory";
        case AblationTarget::ProceduralMemory: return "ProceduralMemory";
        case AblationTarget::Memory: return "Memory";
        case AblationTarget::Prediction: return "Prediction";
        case AblationTarget::Attention: return "Attention";
        case AblationTarget::Planning: return "Planning";
        case AblationTarget::SelfModel: return "SelfModel";
        case AblationTarget::Vision: return "Vision";
        case AblationTarget::Audio: return "Audio";
        case AblationTarget::Touch: return "Touch";
        case AblationTarget::Proprioception: return "Proprioception";
        case AblationTarget::InternalSensing: return "InternalSensing";
        case AblationTarget::ExcitatoryNeurons: return "ExcitatoryNeurons";
        case AblationTarget::InhibitoryNeurons: return "InhibitoryNeurons";
        case AblationTarget::SensoryNeurons: return "SensoryNeurons";
        case AblationTarget::MotorNeurons: return "MotorNeurons";
        case AblationTarget::Region1: return "Region1";
        case AblationTarget::Region2: return "Region2";
        case AblationTarget::Region3: return "Region3";
        case AblationTarget::AllPlasticity: return "AllPlasticity";
        case AblationTarget::AllNeuromodulation: return "AllNeuromodulation";
        case AblationTarget::AllMemory: return "AllMemory";
        case AblationTarget::AllSensation: return "AllSensation";
        case AblationTarget::AllCognition: return "AllCognition";
        case AblationTarget::None: return "None";
        default: return "Unknown";
    }
}

std::string AblationSystem::getTargetDescription(AblationTarget target) {
    switch (target) {
        case AblationTarget::STDP: return "Spike-Timing-Dependent Plasticity";
        case AblationTarget::Hebbian: return "Hebbian learning";
        case AblationTarget::RewardModulation: return "Reward-based neuromodulation";
        case AblationTarget::EligibilityTraces: return "Eligibility traces for learning";
        case AblationTarget::Synaptogenesis: return "Formation of new synapses";
        case AblationTarget::Pruning: return "Elimination of synapses";
        case AblationTarget::StructuralPlasticity: return "Overall structural plasticity";
        case AblationTarget::Dopamine: return "Dopamine neuromodulator";
        case AblationTarget::Novelty: return "Novelty detection";
        case AblationTarget::Curiosity: return "Curiosity-driven exploration";
        case AblationTarget::Neuromodulation: return "All neuromodulators";
        case AblationTarget::WorkingMemory: return "Working memory system";
        case AblationTarget::EpisodicMemory: return "Episodic memory system";
        case AblationTarget::SemanticMemory: return "Semantic memory system";
        case AblationTarget::ProceduralMemory: return "Procedural memory system";
        case AblationTarget::Memory: return "All memory systems";
        case AblationTarget::Prediction: return "Prediction system";
        case AblationTarget::Attention: return "Attention mechanism";
        case AblationTarget::Planning: return "Planning system";
        case AblationTarget::SelfModel: return "Self-model";
        case AblationTarget::Vision: return "Vision sensory system";
        case AblationTarget::Audio: return "Audio sensory system";
        case AblationTarget::Touch: return "Touch sensory system";
        case AblationTarget::Proprioception: return "Proprioception sensing";
        case AblationTarget::InternalSensing: return "Internal sensing";
        case AblationTarget::ExcitatoryNeurons: return "Excitatory neurons";
        case AblationTarget::InhibitoryNeurons: return "Inhibitory neurons";
        case AblationTarget::SensoryNeurons: return "All sensory neurons";
        case AblationTarget::MotorNeurons: return "All motor neurons";
        case AblationTarget::Region1: return "Brain region 1";
        case AblationTarget::Region2: return "Brain region 2";
        case AblationTarget::Region3: return "Brain region 3";
        case AblationTarget::AllPlasticity: return "All plasticity mechanisms";
        case AblationTarget::AllNeuromodulation: return "All neuromodulation";
        case AblationTarget::AllMemory: return "All memory systems";
        case AblationTarget::AllSensation: return "All sensory systems";
        case AblationTarget::AllCognition: return "All cognitive systems";
        case AblationTarget::None: return "No ablation";
        default: return "Unknown mechanism";
    }
}

std::vector<AblationSystem::AblationTarget> AblationSystem::getAllTargets() {
    std::vector<AblationTarget> targets;
    for (auto pair : mechanismHandlers_) {
        targets.push_back(pair.first);
    }
    return targets;
}

AblationExperiment AblationSystem::createStandardExperiment() {
    AblationExperiment experiment;
    experiment.name = "Standard Ablation Experiment";
    experiment.description = "Standard experiment testing each mechanism individually";
    experiment.baseline = AblationConfig();
    experiment.stepsPerTrial = 10000;
    experiment.trialsPerCondition = 5;
    experiment.randomSeed = 42;
    
    // Single ablations for each mechanism
    for (auto target : getAllTargets()) {
        if (target == AblationTarget::None) continue;
        AblationConfig config;
        config.disable(target);
        config.customDescriptions[target] = "Test single mechanism ablation: " + getTargetName(target);
        experiment.ablations.push_back(config);
    }
    
    return experiment;
}

AblationExperiment AblationSystem::createPlasticityExperiment() {
    AblationExperiment experiment;
    experiment.name = "Plasticity Ablation Experiment";
    experiment.description = "Test all plasticity mechanisms together";
    experiment.baseline = AblationConfig();
    experiment.stepsPerTrial = 10000;
    experiment.trialsPerCondition = 5;
    experiment.randomSeed = 42;
    
    // Single ablations for plasticity targets
    std::vector<AblationTarget> plasticityTargets = {
        AblationTarget::STDP,
        AblationTarget::Hebbian,
        AblationTarget::RewardModulation,
        AblationTarget::EligibilityTraces,
        AblationTarget::Synaptogenesis,
        AblationTarget::Pruning,
        AblationTarget::StructuralPlasticity
    };
    
    for (auto target : plasticityTargets) {
        AblationConfig config;
        config.disable(target);
        config.customDescriptions[target] = "Plasticity ablation: " + getTargetName(target);
        experiment.ablations.push_back(config);
    }
    
    return experiment;
}

AblationExperiment AblationSystem::createNeuromodulationExperiment() {
    AblationExperiment experiment;
    experiment.name = "Neuromodulation Ablation Experiment";
    experiment.description = "Test all neuromodulation mechanisms together";
    experiment.baseline = AblationConfig();
    experiment.stepsPerTrial = 10000;
    experiment.trialsPerCondition = 5;
    experiment.randomSeed = 42;
    
    // Single ablations for neuromodulation targets
    std::vector<AblationTarget> neuromodulationTargets = {
        AblationTarget::Dopamine,
        AblationTarget::Novelty,
        AblationTarget::Curiosity,
        AblationTarget::Neuromodulation
    };
    
    for (auto target : neuromodulationTargets) {
        AblationConfig config;
        config.disable(target);
        config.customDescriptions[target] = "Neuromodulation ablation: " + getTargetName(target);
        experiment.ablations.push_back(config);
    }
    
    return experiment;
}

AblationExperiment AblationSystem::createMemoryExperiment() {
    AblationExperiment experiment;
    experiment.name = "Memory Ablation Experiment";
    experiment.description = "Test all memory mechanisms together";
    experiment.baseline = AblationConfig();
    experiment.stepsPerTrial = 10000;
    experiment.trialsPerCondition = 5;
    experiment.randomSeed = 42;
    
    // Single ablations for memory targets
    std::vector<AblationTarget> memoryTargets = {
        AblationTarget::WorkingMemory,
        AblationTarget::EpisodicMemory,
        AblationTarget::SemanticMemory,
        AblationTarget::ProceduralMemory,
        AblationTarget::Memory
    };
    
    for (auto target : memoryTargets) {
        AblationConfig config;
        config.disable(target);
        config.customDescriptions[target] = "Memory ablation: " + getTargetName(target);
        experiment.ablations.push_back(config);
    }
    
    return experiment;
}

std::string AblationSystem::AblationResult::toCSV() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss << name << ",";
    ss << totalReward << ",";
    ss << avgRewardPerStep << ",";
    ss << learningEfficiency << ",\",";
    ss << finalPerformance << ",\",";
    ss << avgFiringRate << ",\",";
    ss << spikeCount << ",\",";
    ss << synapticWeightSum << ",\",";
    ss << excitatoryInhibitoryRatio << ",\",";
    ss << predictionAccuracy << ",\",";
    ss << predictionError << ",\",";
    ss << stepsRun << ",\",";
    ss << computationTime << ",\",";
    ss << memoryUsageMB;
    return ss.str();
}

std::string AblationSystem::AblationResult::toJSON() const {
    std::stringstream ss;
    ss << "{\"name\": \"" << name << "\",";
    ss << "\"totalReward\": " << totalReward << ",";
    ss << "\"avgRewardPerStep\": " << avgRewardPerStep << ",";
    ss << "\"learningEfficiency\": " << learningEfficiency << ",\"";
    ss << "\"finalPerformance\": " << finalPerformance << ",\"";
    ss << "\"avgFiringRate\": " << avgFiringRate << ",\"";
    ss << "\"spikeCount\": " << spikeCount << ",\"";
    ss << "\"synapticWeightSum\": " << synapticWeightSum << ",\"";
    ss << "\"excitatoryInhibitoryRatio\": " << excitatoryInhibitoryRatio << ",\"";
    ss << "\"predictionAccuracy\": " << predictionAccuracy << ",\"";
    ss << "\"predictionError\": " << predictionError << ",\"";
    ss << "\"stepsRun\": " << stepsRun << ",\"";
    ss << "\"computationTime\": " << computationTime << ",\"";
    ss << "\"memoryUsageMB\": " << memoryUsageMB << "}";
    return ss.str();
}

std::string AblationExperimentRunner::generateConditionName(const AblationConfig& config) {
    std::stringstream ss;
    bool first = true;
    for (auto target : config.disabledTargets) {
        if (target == AblationTarget::None) continue;
        if (!first) ss << "_";
        ss << AblationSystem::getTargetName(target);
        first = false;
    }
    return first ? "Baseline" : ss.str();
}

AblationComparison AblationExperimentRunner::compare(const AblationResult& full, const AblationResult& ablated) {
    AblationComparison comparison;
    comparison.fullSystemName = full.name;
    comparison.ablatedSystemName = ablated.name;
    
    // Calculate ratios
    comparison.rewardRatio = ablated.totalReward / (full.totalReward > 0 ? full.totalReward : 1.0f);
    comparison.firingRateRatio = ablated.avgFiringRate / (full.avgFiringRate > 0 ? full.avgFiringRate : 1.0f);
    comparison.stabilityRatio = ablated.activityStability / (full.activityStability > 0 ? full.activityStability : 1.0f);
    comparison.memoryRatio = ablated.memoryRetention / (full.memoryRetention > 0 ? full.memoryRetention : 1.0f);
    comparison.predictionRatio = ablated.predictionAccuracy / (full.predictionAccuracy > 0 ? full.predictionAccuracy : 1.0f);
    
    // Calculate differences
    comparison.rewardDifference = ablated.totalReward - full.totalReward;
    comparison.firingRateDifference = ablated.avgFiringRate - full.avgFiringRate;
    
    // Generate interpretation
    std::vector<std::string> findings;
    
    if (comparison.rewardRatio < 0.5f) {
        findings.push_back("Severe performance reduction (less than 50% of baseline)");
    } else if (comparison.rewardRatio < 0.8f) {
        findings.push_back("Moderate performance reduction (50-80% of baseline)");
    } else if (comparison.rewardRatio > 1.2f) {
        findings.push_back("Unexpected performance improvement (over 120% of baseline)");
    }
    
    if (comparison.firingRateDifference < -100.0f) {
        findings.push_back("Significant reduction in neural firing rate");
    }
    
    if (comparison.predictionRatio < 0.3f) {
        findings.push_back("Severe prediction impairment");
    }
    
    comparison.interpretation = findings.empty() ? "Minimal impact detected" : findings[0];
    comparison.keyFindings = findings;
    
    return comparison;
}

std::string AblationExperimentRunner::generateReport(const std::vector<AblationResult>& results) {
    if (results.empty()) {
        return "No results to report";
    }
    
    std::stringstream ss;
    ss << "=== ABATION EXPERIMENT REPORT ===" << std::endl;
    ss << "Total conditions tested: " << results.size() << std::endl;
    ss << std::endl;
    
    // Find baseline
    auto baselineIt = std::find_if(results.begin(), results.end(), 
        [](const AblationResult& r) { return r.name.find("Baseline") != std::string::npos; });
    
    if (baselineIt != results.end()) {
        ss << "=== BASELINE RESULTS ===" << std::endl;
        ss << "Condition: " << baselineIt->name << std::endl;
        ss << "Total Reward: " << baselineIt->totalReward << std::endl;
        ss << "Average Firing Rate: " << baselineIt->avgFiringRate << std::endl;
        ss << "Spike Count: " << baselineIt->spikeCount << std::endl;
        ss << "Learning Efficiency: " << baselineIt->learningEfficiency << std::endl;
        ss << std::endl;
    }
    
    ss << "=== ABLATION RESULTS ===" << std::endl;
    
    for (const auto& result : results) {
        if (result.name.find("Baseline") != std::string::npos) continue;
        
        ss << "Condition: " << result.name << std::endl;
        ss << "Total Reward: " << result.totalReward << " (" 
           << (result.totalReward / (baselineIt->totalReward > 0 ? baselineIt->totalReward : 1.0f) * 100) << "% of baseline)" << std::endl;
        ss << "Average Firing Rate: " << result.avgFiringRate << std::endl;
        ss << "Spike Count: " << result.spikeCount << std::endl;
        ss << "Activity Stability: " << result.activityStability << std::endl;
        ss << "Prediction Accuracy: " << result.predictionAccuracy << std::endl;
        ss << "Memory Retention: " << result.memoryRetention << std::endl;
        ss << "Computation Time: " << result.computationTime << "s" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

bool AblationExperimentRunner::exportToCSV(const std::string& filepath, const std::vector<AblationResult>& results) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open CSV file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "Condition,TotalReward,AvgRewardPerStep,LearningEfficiency,FinalPerformance,AvgFiringRate,SpikeCount,SynapticWeightSum,ExcitatoryInhibitoryRatio,PredictionAccuracy,PredictionError,StepsRun,ComputationTime,MemoryUsageMB" << std::endl;
    
    // Write results
    for (const auto& result : results) {
        file << result.toCSV() << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Exported " << results.size() << " ablation results to CSV: " << filepath);
    return true;
}

bool AblationExperimentRunner::exportToJSON(const std::string& filepath, const std::vector<AblationResult>& results) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file for writing: " + filepath);
        return false;
    }
    
    file << "["; // Start array
    
    for (size_t i = 0; i < results.size(); ++i) {
        file << results[i].toJSON();
        if (i < results.size() - 1) {
            file << ","; // Add comma between objects
        }
    }
    
    file << "]"; // End array
    
    file.close();
    NLM_LOG_INFO("Exported " << results.size() << " ablation results to JSON: " << filepath);
    return true;
}

std::vector<AblationResult> AblationExperimentRunner::runExperiment(
    const AblationExperiment& experiment,
    std::function<bool(uint64_t steps, const AblationConfig& config)> runFunction
) {
    std::vector<AblationResult> results;
    
    // Run baseline (full system)
    NLM_LOG_INFO("Running baseline condition: " << experiment.name);
    bool baselineSuccess = runFunction(experiment.stepsPerTrial, experiment.baseline);
    
    AblationResult baselineResult = createBaselineResult();
    baselineResult.config = experiment.baseline;
    baselineResult.name = "Baseline (full system)";
    
    if (baselineSuccess) {
        results.push_back(baselineResult);
        NLM_LOG_INFO("Baseline completed successfully");
    } else {
        NLM_LOG_ERROR("Baseline failed");
    }
    
    // Run each ablation condition
    for (size_t i = 0; i < experiment.ablations.size(); ++i) {
        const auto& ablationConfig = experiment.ablations[i];
        
        std::string conditionName = generateConditionName(ablationConfig);
        NLM_LOG_INFO("Running ablation condition: " << conditionName);
        
        // Apply ablation
        bool ablationSuccess = applyAblation(ablationConfig);
        
        if (ablationSuccess) {
            bool runSuccess = runFunction(experiment.stepsPerTrial, ablationConfig);
            
            AblationResult ablationResult = createAblationResult(conditionName, ablationConfig, runSuccess);
            ablationResult.config = ablationConfig;
            ablationResult.name = conditionName;
            
            if (runSuccess) {
                results.push_back(ablationResult);
                NLM_LOG_INFO("Ablation condition " << conditionName << " completed successfully");
            } else {
                NLM_LOG_ERROR("Ablation condition " << conditionName << " failed");
            }
        } else {
            NLM_LOG_ERROR("Failed to apply ablation configuration: " << conditionName);
        }
    }
    
    return results;
}

std::vector<AblationResult> AblationExperimentRunner::runWithMultipleSeeds(
    const AblationExperiment& experiment,
    size_t numSeeds,
    std::function<bool(uint64_t steps, const AblationConfig& config, uint64_t seed)> runFunction
) {
    std::vector<AblationResult> allResults;
    
    for (size_t seed = 0; seed < numSeeds; ++seed) {
        NLM_LOG_INFO("Running with random seed: " << seed);
        
        std::vector<AblationResult> seedResults = runExperiment(
            experiment,
            [runFunction, seed](uint64_t steps, const AblationConfig& config) {
                return runFunction(steps, config, static_cast<uint64_t>(seed));
            }
        );
        
        allResults.insert(allResults.end(), seedResults.begin(), seedResults.end());
    }
    
    return allResults;
}

AblationResult AblationExperimentRunner::createAblationResult(const std::string& name, const AblationConfig& config, bool success) {
    AblationResult result;
    result.name = name;
    result.config = config;
    
    if (!success) {
        // Set default values for failed runs
        result.totalReward = 0.0f;
        result.avgRewardPerStep = 0.0f;
        result.learningEfficiency = 0.0f;
        result.finalPerformance = 0.0f;
        result.avgFiringRate = 0.0f;
        result.spikeCount = 0.0f;
        result.synapticWeightSum = 0.0f;
        result.excitatoryInhibitoryRatio = 1.0f;
        result.predictionAccuracy = 0.0f;
        result.predictionError = 1.0f;
        result.activityStability = 0.0f;
        result.weightStability = 0.0f;
        result.memoryRetention = 0.0f;
        result.memoryRecall = 0.0f;
        result.explorationRate = 0.0f;
        result.goalDirectedness = 0.0f;
        result.adaptationSpeed = 0.0f;
        result.performanceVsFull = 0.0f;
        result.statisticalSignificance = 0.0f;
        result.isSignificant = false;
        result.stepsRun = 0;
        result.computationTime = 0.0;
        result.memoryUsageMB = 0;
    }
    
    return result;
}

AblationSystem* AblationSystem::getInstance() {
    static AblationSystem instance;
    return &instance;
}

void AblationSystem::cleanup() {
    delete getInstance();
}

AblationSystem& AblationSystem::getGlobalAblationSystem() {
    static AblationSystem instance;
    return instance;
}

} // namespace nlm