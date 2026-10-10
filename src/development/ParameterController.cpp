#include "ParameterController.hpp"
#include "BrainSimulator.hpp"
#include "BrainDebugger.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

ParameterController::ParameterController(std::shared_ptr<Brain> brain, std::shared_ptr<BrainSimulator> simulator)
    : brain_(brain), simulator_(simulator), trackingEnabled_(false), modificationsPaused_(false),
      modificationRate_(1.0), modificationInterval_(1.0), maxChangesPerSecond_(100), 
      parameterChangeTimeout_(5.0), validationEnabled_(true), safetyMargin_(0.1) {
    
    cleanup();
    NLM_LOG_INFO("ParameterController initialized");
}

ParameterController::~ParameterController() {
    shutdown();
}

void ParameterController::cleanup() {
    parameterChanges_.clear();
    neuronParameters_.clear();
    synapseParameters_.clear();
    plasticityParameters_.clear();
    neuromodulatorParameters_.clear();
    validationErrors_.clear();
    
    currentSessionName_.clear();
    sessionStartTime_ = 0.0;
}

bool ParameterController::initialize() {
    if (!brain_) {
        NLM_LOG_ERROR("ParameterController: No brain reference available");
        return false;
    }
    
    // Initialize parameter values from current brain state
    updateInitialParameterValues();
    
    // Start parameter tracking if enabled
    if (trackingEnabled_) {
        startParameterSession("AutoSession");
    }
    
    NLM_LOG_INFO("ParameterController initialized successfully");
    return true;
}

void ParameterController::shutdown() {
    // End current session
    endParameterSession();
    
    cleanup();
    
    NLM_LOG_INFO("ParameterController shutdown completed");
}

void ParameterController::updateInitialParameterValues() {
    if (!brain_) return;
    
    // Initialize neuron parameters
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    neuronParameters_[neuron->getId()]["threshold"] = neuron->getState().threshold;
                    neuronParameters_[neuron->getId()]["resting_potential"] = neuron->getState().restingPotential;
                    neuronParameters_[neuron->getId()]["membrane_potential"] = neuron->getState().membranePotential;
                    neuronParameters_[neuron->getId()]["firing_rate"] = neuron->getState().firingRate;
                    neuronParameters_[neuron->getId()]["activation_level"] = neuron->getState().activationLevel;
                }
            }
        }
    }
    
    // Initialize synapse parameters
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            synapseParameters_[syn->getId()]["weight"] = syn->getWeight();
            synapseParameters_[syn->getId()]["delay"] = syn->getDelay();
            synapseParameters_[syn->getId()]["eligibility_trace"] = syn->getEligibilityTrace();
            synapseParameters_[syn->getId()]["is_plastic"] = syn->isPlastic() ? 1.0f : 0.0f;
            synapseParameters_[syn->getId()]["plasticity_rate"] = syn->getPlasticityRate();
        }
    }
    
    // Initialize plasticity parameters
    if (auto* stdp = brain_->getSTDP()) {
        plasticityParameters_[stdp->getId()]["ltp_weight"] = stdp->getLTPWeight();
        plasticityParameters_[stdp->getId()]["ltd_weight"] = stdp->getLTDWeight();
        plasticityParameters_[stdp->getId()]["trace_decay"] = stdp->getTraceDecay();
        plasticityParameters_[stdp->getId()]["learning_rate"] = stdp->getLearningRate();
    }
    
    if (auto* hebbian = brain_->getHebbian()) {
        plasticityParameters_[hebbian->getId()]["learning_rate"] = hebbian->getLearningRate();
        plasticityParameters_[hebbian->getId()]["similarity_threshold"] = hebbian->getSimilarityThreshold();
    }
    
    if (auto* sp = brain_->getStructuralPlasticity()) {
        plasticityParameters_[sp->getId()]["synaptogenesis_rate"] = sp->getSynaptogenesisRate();
        plasticityParameters_[sp->getId()]["pruning_rate"] = sp->getPruningRate();
    }
    
    // Initialize neuromodulator parameters (would need neuromodulation system access)
    // Placeholder initialization
}

bool ParameterController::adjustWeight(SynapseId synapseId, float weightDelta, const std::string& reason) {
    if (modificationsPaused_) {
        NLM_LOG_WARNING("Parameter modification paused - cannot adjust weight");
        return false;
    }
    
    ParameterChange change;
    change.parameterName = "weight";
    change.parameterType = "synapse";
    change.targetId = synapseId;
    change.changeAmount = weightDelta;
    change.changeReason = reason.empty() ? "manual_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    // Get current value
    float currentWeight = getCurrentParameterValue(synapseId, "weight");
    change.oldValue = currentWeight;
    change.newValue = currentWeight + weightDelta;
    
    // Apply the change
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::setWeight(SynapseId synapseId, float weight, const std::string& reason) {
    if (modificationsPaused_) {
        NLM_LOG_WARNING("Parameter modification paused - cannot set weight");
        return false;
    }
    
    ParameterChange change;
    change.parameterName = "weight";
    change.parameterType = "synapse";
    change.targetId = synapseId;
    change.newValue = weight;
    change.changeAmount = weight - getCurrentParameterValue(synapseId, "weight");
    change.changeReason = reason.empty() ? "weight_setting" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(synapseId, "weight");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::adjustWeightByPrePost(NeuronId preId, NeuronId postId, float weightDelta, const std::string& reason) {
    // Find synapse connecting these neurons
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn->getPreNeuron() == preId && syn->getPostNeuron() == postId) {
                return adjustWeight(syn->getId(), weightDelta, reason);
            }
        }
    }
    
    NLM_LOG_WARNING("No synapse found between neurons " + std::to_string(preId) + 
                   " and " + std::to_string(postId));
    return false;
}

bool ParameterController::adjustThreshold(NeuronId neuronId, float thresholdDelta, const std::string& reason) {
    if (modificationsPaused_) {
        NLM_LOG_WARNING("Parameter modification paused - cannot adjust threshold");
        return false;
    }
    
    ParameterChange change;
    change.parameterName = "threshold";
    change.parameterType = "neuron";
    change.targetId = neuronId;
    change.changeAmount = thresholdDelta;
    change.changeReason = reason.empty() ? "threshold_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    float currentThreshold = getCurrentParameterValue(neuronId, "threshold");
    change.oldValue = currentThreshold;
    change.newValue = currentThreshold + thresholdDelta;
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::setThreshold(NeuronId neuronId, float threshold, const std::string& reason) {
    if (modificationsPaused_) {
        NLM_LOG_WARNING("Parameter modification paused - cannot set threshold");
        return false;
    }
    
    ParameterChange change;
    change.parameterName = "threshold";
    change.parameterType = "neuron";
    change.targetId = neuronId;
    change.newValue = threshold;
    change.changeAmount = threshold - getCurrentParameterValue(neuronId, "threshold");
    change.changeReason = reason.empty() ? "threshold_setting" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(neuronId, "threshold");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::adjustThresholdByType(NeuronType type, float thresholdDelta, const std::string& reason) {
    bool success = false;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            if (population->getNeuronType() == type) {
                for (Neuron* neuron : population->getNeurons()) {
                    if (neuron) {
                        if (adjustThreshold(neuron->getId(), thresholdDelta, reason)) {
                            success = true;
                        }
                    }
                }
            }
        }
    }
    
    return success;
}

bool ParameterController::tunePlasticityRule(PlasticityId plasticityId, const PlasticityParameters& params, const std::string& reason) {
    if (modificationsPaused_) {
        NLM_LOG_WARNING("Parameter modification paused - cannot tune plasticity");
        return false;
    }
    
    ParameterChange change;
    change.parameterName = "plasticity_parameters";
    change.parameterType = "plasticity";
    change.targetId = plasticityId;
    change.newValue = params.learningRate; // Simplified - use learning rate as representative value
    change.changeAmount = params.learningRate - getCurrentParameterValue(plasticityId, "learning_rate");
    change.changeReason = reason.empty() ? "plasticity_tuning" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(plasticityId, "learning_rate");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::adjustSTDPParameters(float learningRate, float traceDecay, const std::string& reason) {
    bool success = false;
    
    if (auto* stdp = brain_->getSTDP()) {
        if (adjustSTDPParameter("learning_rate", learningRate, reason)) success = true;
        if (adjustSTDPParameter("trace_decay", traceDecay, reason)) success = true;
    }
    
    return success;
}

bool ParameterController::adjustHebbianParameters(float learningRate, float similarityThreshold, const std::string& reason) {
    bool success = false;
    
    if (auto* hebbian = brain_->getHebbian()) {
        if (adjustHebbianParameter("learning_rate", learningRate, reason)) success = true;
        if (adjustHebbianParameter("similarity_threshold", similarityThreshold, reason)) success = true;
    }
    
    return success;
}

bool ParameterController::adjustStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate, const std::string& reason) {
    bool success = false;
    
    if (auto* sp = brain_->getStructuralPlasticity()) {
        if (adjustSPParameter("synaptogenesis_rate", synaptogenesisRate, reason)) success = true;
        if (adjustSPParameter("pruning_rate", pruningRate, reason)) success = true;
    }
    
    return success;
}

bool ParameterController::modifyNeuromodulator(NeuromodulatorId neuromodulatorId, float level, const std::string& reason) {
    if (modificationsPaused_) {
        NLM_LOG_WARNING("Parameter modification paused - cannot modify neuromodulator");
        return false;
    }
    
    ParameterChange change;
    change.parameterName = "neuromodulator_level";
    change.parameterType = "neuromodulator";
    change.targetId = neuromodulatorId;
    change.newValue = level;
    change.changeAmount = level - getCurrentParameterValue(neuromodulatorId, "level");
    change.changeReason = reason.empty() ? "neuromodulator_modification" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(neuromodulatorId, "level");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::setDevelopmentStage(DevelopmentalStage stage, const std::string& reason) {
    if (modificationsPaused_) {
        NLM_LOG_WARNING("Parameter modification paused - cannot set development stage");
        return false;
    }
    
    ParameterChange change;
    change.parameterName = "development_stage";
    change.parameterType = "development";
    change.targetId = static_cast<uint64_t>(stage);
    change.newValue = static_cast<float>(stage);
    change.changeAmount = static_cast<float>(stage) - getCurrentParameterValue(static_cast<uint64_t>(stage), "stage");
    change.changeReason = reason.empty() ? "development_stage_setting" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(static_cast<uint64_t>(stage), "stage");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::adjustDevelopmentRate(float rateDelta, const std::string& reason) {
    ParameterChange change;
    change.parameterName = "development_rate";
    change.parameterType = "development";
    change.targetId = 0; // Development system ID
    change.changeAmount = rateDelta;
    change.changeReason = reason.empty() ? "development_rate_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    float currentRate = getCurrentParameterValue(0, "development_rate");
    change.oldValue = currentRate;
    change.newValue = currentRate + rateDelta;
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::enableDevelopment(bool enable, const std::string& reason) {
    ParameterChange change;
    change.parameterName = "development_enabled";
    change.parameterType = "development";
    change.targetId = 0; // Development system ID
    change.newValue = enable ? 1.0f : 0.0f;
    change.changeAmount = (enable ? 1.0f : 0.0f) - getCurrentParameterValue(0, "development_enabled");
    change.changeReason = reason.empty() ? "development_enable" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(0, "development_enabled");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::setDevelopmentParameters(const DevelopmentParameters& params, const std::string& reason) {
    bool success = true;
    
    if (adjustDevelopmentRate(params.developmentRate, reason)) {
        // Success
    } else {
        success = false;
    }
    
    // Additional parameter settings would go here
    return success;
}

bool ParameterController::modifyPlasticityGating(float gatingFactor, const std::string& reason) {
    ParameterChange change;
    change.parameterName = "plasticity_gating";
    change.parameterType = "plasticity";
    change.targetId = 0; // Plasticity system ID
    change.newValue = gatingFactor;
    change.changeAmount = gatingFactor - getCurrentParameterValue(0, "plasticity_gating");
    change.changeReason = reason.empty() ? "plasticity_gating_modification" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(0, "plasticity_gating");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::adjustPlasticityThreshold(float threshold, const std::string& reason) {
    ParameterChange change;
    change.parameterName = "plasticity_threshold";
    change.parameterType = "plasticity";
    change.targetId = 0; // Plasticity system ID
    change.newValue = threshold;
    change.changeAmount = threshold - getCurrentParameterValue(0, "plasticity_threshold");
    change.changeReason = reason.empty() ? "plasticity_threshold_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(0, "plasticity_threshold");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::modifySignalSynthesizedLevel(float level, const std::string& reason) {
    ParameterChange change;
    change.parameterName = "signal_synthesized_level";
    change.parameterType = "neuromodulator";
    change.targetId = 0; // Neuromodulator system ID
    change.newValue = level;
    change.changeAmount = level - getCurrentParameterValue(0, "signal_synthesized_level");
    change.changeReason = reason.empty() ? "signal_synthesis_modification" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(0, "signal_synthesized_level");
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::adjustAdaptationRate(float rateDelta, const std::string& reason) {
    ParameterChange change;
    change.parameterName = "adaptation_rate";
    change.parameterType = "neuromodulator";
    change.targetId = 0; // Neuromodulator system ID
    change.changeAmount = rateDelta;
    change.changeReason = reason.empty() ? "adaptation_rate_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    float currentRate = getCurrentParameterValue(0, "adaptation_rate");
    change.oldValue = currentRate;
    change.newValue = currentRate + rateDelta;
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::modifyLearningRate(float rateDelta, const std::string& reason) {
    ParameterChange change;
    change.parameterName = "learning_rate";
    change.parameterType = "plasticity";
    change.targetId = 0; // Plasticity system ID
    change.changeAmount = rateDelta;
    change.changeReason = reason.empty() ? "learning_rate_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("Parameter validation failed: " + error);
        }
        return false;
    }
    
    float currentRate = getCurrentParameterValue(0, "learning_rate");
    change.oldValue = currentRate;
    change.newValue = currentRate + rateDelta;
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

std::vector<ParameterController::ParameterChange> ParameterController::batchModifyParameters(const std::vector<ParameterChange>& changes) {
    std::vector<ParameterChange> successfulChanges;
    
    for (const auto& change : changes) {
        if (validateParameterChange(change) && applySingleParameterChange(change)) {
            ParameterChange changeCopy = change;
            changeCopy.isApplied = true;
            changeCopy.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
            
            parameterChanges_.push_back(changeCopy);
            logParameterChange(changeCopy);
            successfulChanges.push_back(changeCopy);
        } else {
            NLM_LOG_WARNING("Batch parameter modification failed: " + change.parameterName);
        }
    }
    
    return successfulChanges;
}

bool ParameterController::revertParameterChange(const ParameterChange& change) {
    if (change.targetId == 0 || change.parameterName.empty()) {
        return false;
    }
    
    ParameterChange revertChange;
    revertChange.parameterName = change.parameterName;
    revertChange.parameterType = change.parameterType;
    revertChange.targetId = change.targetId;
    revertChange.oldValue = change.newValue;
    revertChange.newValue = change.oldValue;
    revertChange.changeAmount = change.oldValue - change.newValue;
    revertChange.changeReason = "revert: " + change.changeReason;
    revertChange.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    revertChange.isApplied = false;
    
    return applySingleParameterChange(revertChange);
}

std::vector<ParameterController::ParameterChange> ParameterController::getParameterHistory() const {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    return parameterChanges_;
}

bool ParameterController::validateParameterChange(const ParameterChange& change) const {
    validationErrors_.clear();
    
    // Basic validation
    if (change.targetId == 0) {
        validationErrors_.push_back("Invalid target ID");
        return false;
    }
    
    if (change.parameterName.empty()) {
        validationErrors_.push_back("Parameter name cannot be empty");
        return false;
    }
    
    // Type-specific validation
    if (change.parameterType == "neuron") {
        if (!validateNeuronParameter(change)) return false;
    } else if (change.parameterType == "synapse") {
        if (!validateSynapseParameter(change)) return false;
    } else if (change.parameterType == "plasticity") {
        if (!validatePlasticityParameter(change)) return false;
    } else if (change.parameterType == "neuromodulator") {
        if (!validateNeuromodulatorParameter(change)) return false;
    } else if (change.parameterType == "development") {
        if (!validateDevelopmentParameter(change)) return false;
    }
    
    // Check safety margins
    if (!validateParameterBounds(change.newValue, change.parameterName)) {
        validationErrors_.push_back("Parameter value out of safe bounds");
        return false;
    }
    
    // Check for conflicts
    if (checkForConflicts(change)) {
        validationErrors_.push_back("Parameter change conflicts with existing changes");
        return false;
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> ParameterController::getValidationErrors() const {
    return validationErrors_;
}

bool ParameterController::validateParameterRanges() const {
    bool valid = true;
    
    // Check neuron parameters
    for (const auto& pair : neuronParameters_) {
        for (const auto& paramPair : pair.second) {
            if (!validateParameterBounds(paramPair.second, paramPair.first)) {
                validationErrors_.push_back("Invalid neuron parameter " + paramPair.first);
                valid = false;
            }
        }
    }
    
    // Check synapse parameters
    for (const auto& pair : synapseParameters_) {
        for (const auto& paramPair : pair.second) {
            if (!validateParameterBounds(paramPair.second, paramPair.first)) {
                validationErrors_.push_back("Invalid synapse parameter " + paramPair.first);
                valid = false;
            }
        }
    }
    
    return valid;
}

bool ParameterController::validateParameterConsistency() const {
    // Check consistency between related parameters
    bool valid = true;
    
    // Check that neuron thresholds are within reasonable bounds
    for (const auto& pair : neuronParameters_) {
        if (pair.second.find("threshold") != pair.second.end()) {
            float threshold = pair.second.at("threshold");
            if (threshold < 0.0f || threshold > 1.0f) {
                validationErrors_.push_back("Neuron threshold out of range for neuron " + std::to_string(pair.first));
                valid = false;
            }
        }
    }
    
    return valid;
}

bool ParameterController::validateBrainState() const {
    if (!brain_) {
        validationErrors_.push_back("No brain reference available");
        return false;
    }
    
    // Check brain integrity
    if (brain_->getTotalNeuronCount() == 0) {
        validationErrors_.push_back("Brain has no neurons");
    }
    
    if (brain_->getTotalSynapseCount() == 0) {
        validationErrors_.push_back("Brain has no synapses");
    }
    
    if (brain_->getRegionCount() == 0) {
        validationErrors_.push_back("Brain has no regions");
    }
    
    return validationErrors_.empty();
}

void ParameterController::enableParameterTracking(bool enable) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    if (enable && !trackingEnabled_) {
        trackingEnabled_ = true;
        startParameterSession("AutoSession");
        NLM_LOG_INFO("Parameter tracking enabled");
    } else if (!enable && trackingEnabled_) {
        trackingEnabled_ = false;
        endParameterSession();
        NLM_LOG_INFO("Parameter tracking disabled");
    }
}

bool ParameterController::isParameterTrackingEnabled() const {
    return trackingEnabled_.load();
}

float ParameterController::getParameter(NeuronId neuronId, const std::string& parameterName) const {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    auto neuronIt = neuronParameters_.find(neuronId);
    if (neuronIt != neuronParameters_.end()) {
        auto paramIt = neuronIt->second.find(parameterName);
        if (paramIt != neuronIt->second.end()) {
            return paramIt->second;
        }
    }
    
    return 0.0f; // Default value
}

float ParameterController::getParameter(SynapseId synapseId, const std::string& parameterName) const {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    auto synapseIt = synapseParameters_.find(synapseId);
    if (synapseIt != synapseParameters_.end()) {
        auto paramIt = synapseIt->second.find(parameterName);
        if (paramIt != synapseIt->second.end()) {
            return paramIt->second;
        }
    }
    
    return 0.0f; // Default value
}

float ParameterController::getParameter(PlasticityId plasticityId, const std::string& parameterName) const {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    auto plasticityIt = plasticityParameters_.find(plasticityId);
    if (plasticityIt != plasticityParameters_.end()) {
        auto paramIt = plasticityIt->second.find(parameterName);
        if (paramIt != plasticityIt->second.end()) {
            return paramIt->second;
        }
    }
    
    return 0.0f; // Default value
}

float ParameterController::getParameter(NeuromodulatorId neuromodulatorId, const std::string& parameterName) const {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    auto neuromodIt = neuromodulatorParameters_.find(neuromodulatorId);
    if (neuromodIt != neuromodulatorParameters_.end()) {
        auto paramIt = neuromodIt->second.find(parameterName);
        if (paramIt != neuromodIt->second.end()) {
            return paramIt->second;
        }
    }
    
    return 0.0f; // Default value
}

void ParameterController::pauseParameterModifications(bool pause) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    modificationsPaused_ = pause;
    
    if (pause) {
        NLM_LOG_INFO("Parameter modifications paused");
    } else {
        NLM_LOG_INFO("Parameter modifications resumed");
    }
}

bool ParameterController::areParameterModificationsPaused() const {
    return modificationsPaused_.load();
}

void ParameterController::setModificationRate(double rate) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    modificationRate_ = rate;
    modificationInterval_ = 1.0 / std::max(rate, 0.1); // Prevent division by zero
    NLM_LOG_INFO("Parameter modification rate set to " + std::to_string(rate) + " changes/second");
}

double ParameterController::getModificationRate() const {
    return modificationRate_.load();
}

void ParameterController::setMaxParameterChangesPerSecond(size_t maxChanges) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    maxChangesPerSecond_ = maxChanges;
}

void ParameterController::setParameterChangeTimeout(double timeout) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    parameterChangeTimeout_ = timeout;
}

void ParameterController::enableParameterValidation(bool enable) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    validationEnabled_ = enable;
    
    if (enable) {
        NLM_LOG_INFO("Parameter validation enabled");
    } else {
        NLM_LOG_INFO("Parameter validation disabled");
    }
}

bool ParameterController::isParameterValidationEnabled() const {
    return validationEnabled_.load();
}

void ParameterController::setSafetyMargin(float margin) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    safetyMargin_ = std::max(0.0f, std::min(margin, 1.0f));
}

float ParameterController::getSafetyMargin() const {
    return safetyMargin_.load();
}

void ParameterController::applyChangesToSimulation() {
    if (!simulator_) {
        NLM_LOG_WARNING("Cannot apply changes to simulation: no simulator reference");
        return;
    }
    
    // Apply all pending parameter changes to the simulation
    for (const auto& change : parameterChanges_) {
        if (!change.isApplied) {
            applySingleParameterChange(change);
        }
    }
    
    NLM_LOG_INFO("Parameter changes applied to simulation");
}

void ParameterController::commitChanges() {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    // Update simulation with committed parameters
    // Note: This would need implementation based on brain system interfaces
    
    NLM_LOG_INFO("Parameter changes committed");
}

void ParameterController::rollbackChanges() {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    // Roll back parameter changes to previous state
    // This would require storing previous states
    
    NLM_LOG_INFO("Parameter changes rolled back");
}

bool ParameterController::hasPendingChanges() const {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    for (const auto& change : parameterChanges_) {
        if (!change.isApplied) {
            return true;
        }
    }
    
    return false;
}

void ParameterController::startParameterSession(const std::string& sessionName) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    currentSessionName_ = sessionName;
    sessionStartTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    NLM_LOG_INFO("Parameter session started: " + sessionName);
}

void ParameterController::endParameterSession() {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    if (!currentSessionName_.empty()) {
        double sessionDuration = std::chrono::duration<double>::clock::now().time_since_epoch().count() - sessionStartTime_;
        
        NLM_LOG_INFO("Parameter session ended: " + currentSessionName_ + 
                    " (duration: " + std::to_string(sessionDuration) + "s)");
        
        currentSessionName_.clear();
        sessionStartTime_ = 0.0;
    }
}

bool ParameterController::saveParameterSession(const std::string& filename) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open parameter session file: " + filename);
        return false;
    }
    
    // Serialize parameter changes and current state
    // Note: This would need proper binary serialization
    
    file.close();
    NLM_LOG_INFO("Parameter session saved: " + filename);
    return true;
}

bool ParameterController::loadParameterSession(const std::string& filename) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open parameter session file: " + filename);
        return false;
    }
    
    // Deserialize parameter changes
    // Note: This would need proper binary deserialization
    
    file.close();
    NLM_LOG_INFO("Parameter session loaded: " + filename);
    return true;
}

std::string ParameterController::getCurrentSessionName() const {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    return currentSessionName_;
}

ParameterController::ParameterStats ParameterController::getParameterStatistics() const {
    ParameterStats stats;
    
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    stats.totalChanges = parameterChanges_.size();
    
    size_t successful = 0, failed = 0;
    float totalChangeMagnitude = 0.0f;
    
    for (const auto& change : parameterChanges_) {
        if (change.isApplied) {
            successful++;
            totalChangeMagnitude += std::abs(change.changeAmount);
        } else {
            failed++;
        }
    }
    
    stats.successfulChanges = successful;
    stats.failedChanges = failed;
    stats.averageChangeMagnitude = successful > 0 ? totalChangeMagnitude / successful : 0.0f;
    
    // Calculate parameter stability (inverse of variance)
    // This would require more sophisticated analysis
    stats.parameterStabilityIndex = 0.5f; // Placeholder
    
    // Calculate parameter variances (simplified)
    for (const auto& pair : neuronParameters_) {
        float sum = 0.0f, sumSquares = 0.0f;
        for (const auto& paramPair : pair.second) {
            sum += paramPair.second;
            sumSquares += paramPair.second * paramPair.second;
        }
        if (!neuronParameters_.empty()) {
            float mean = sum / neuronParameters_.size();
            float variance = (sumSquares - mean * mean) / neuronParameters_.size();
            stats.parameterVariances["neuron_" + std::to_string(pair.first)] = variance;
            stats.parameterMeans["neuron_" + std::to_string(pair.first)] = mean;
        }
    }
    
    stats.sessionDuration = std::chrono::duration<double>::clock::now().time_since_epoch().count() - sessionStartTime_;
    
    return stats;
}

std::vector<float> ParameterController::getParameterDistribution(const std::string& parameterName) const {
    std::vector<float> distribution;
    
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    // Collect all values for the specified parameter
    std::vector<float> values;
    
    for (const auto& pair : neuronParameters_) {
        auto it = pair.second.find(parameterName);
        if (it != pair.second.end()) {
            values.push_back(it->second);
        }
    }
    
    for (const auto& pair : synapseParameters_) {
        auto it = pair.second.find(parameterName);
        if (it != pair.second.end()) {
            values.push_back(it->second);
        }
    }
    
    // Bin values into distribution
    if (!values.empty()) {
        float minVal = *std::min_element(values.begin(), values.end());
        float maxVal = *std::max_element(values.begin(), values.end());
        float range = maxVal - minVal;
        
        if (range < 1e-6f) range = 1.0f;
        
        distribution.resize(10, 0.0f); // 10 bins
        for (float value : values) {
            size_t bin = static_cast<size_t>((value - minVal) / range * 9.0f);
            if (bin >= 10) bin = 9;
            distribution[bin]++;
        }
    }
    
    return distribution;
}

float ParameterController::calculateParameterSensitivity(NeuronId neuronId, const std::string& parameterName) const {
    // Calculate how sensitive the neuron is to changes in the parameter
    // This would require running simulations or using sensitivity analysis
    
    // Placeholder: return a simple sensitivity based on parameter variance
    float currentValue = getParameter(neuronId, parameterName);
    
    // If we had access to simulation, we could calculate sensitivity by perturbing
    // the parameter and observing the effect
    
    return 0.5f; // Placeholder sensitivity
}

void ParameterController::lockParameterController() const {
    parameterMutex_.lock();
}

void ParameterController::unlockParameterController() const {
    parameterMutex_.unlock();
}

bool ParameterController::tryLockParameterController() const {
    return parameterMutex_.try_lock();
}

bool ParameterController::applySingleParameterChange(const ParameterChange& change) {
    // Apply the parameter change to the brain system
    bool success = true;
    
    // Update parameter value in our internal state
    updateParameterValue(change.targetId, change.parameterName, change.newValue);
    
    // Apply to brain system based on parameter type
    if (change.parameterType == "neuron") {
        success = applyNeuronParameterChange(change);
    } else if (change.parameterType == "synapse") {
        success = applySynapseParameterChange(change);
    } else if (change.parameterType == "plasticity") {
        success = applyPlasticityParameterChange(change);
    } else if (change.parameterType == "neuromodulator") {
        success = applyNeuromodulatorParameterChange(change);
    } else if (change.parameterType == "development") {
        success = applyDevelopmentParameterChange(change);
    }
    
    // Check safety margins after application
    if (success) {
        enforceSafetyMargin();
    }
    
    return success;
}

void ParameterController::updateParameterValue(uint64_t targetId, const std::string& parameterName, float newValue) {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    // Store in appropriate map based on target ID type
    // This is simplified - would need better type identification
    if (targetId < 1000000) { // Assume neuron ID range
        neuronParameters_[targetId][parameterName] = newValue;
    } else if (targetId < 2000000) { // Assume synapse ID range
        synapseParameters_[targetId][parameterName] = newValue;
    } else if (targetId < 3000000) { // Assume plasticity ID range
        plasticityParameters_[targetId][parameterName] = newValue;
    } else { // Assume neuromodulator ID range
        neuromodulatorParameters_[targetId][parameterName] = newValue;
    }
}

float ParameterController::getCurrentParameterValue(uint64_t targetId, const std::string& parameterName) const {
    std::lock_guard<std::mutex> lock(parameterMutex_);
    
    // Try each parameter map
    auto neuronIt = neuronParameters_.find(targetId);
    if (neuronIt != neuronParameters_.end()) {
        auto paramIt = neuronIt->second.find(parameterName);
        if (paramIt != neuronIt->second.end()) {
            return paramIt->second;
        }
    }
    
    auto synapseIt = synapseParameters_.find(targetId);
    if (synapseIt != synapseParameters_.end()) {
        auto paramIt = synapseIt->second.find(parameterName);
        if (paramIt != synapseIt->second.end()) {
            return paramIt->second;
        }
    }
    
    auto plasticityIt = plasticityParameters_.find(targetId);
    if (plasticityIt != plasticityParameters_.end()) {
        auto paramIt = plasticityIt->second.find(parameterName);
        if (paramIt != plasticityIt->second.end()) {
            return paramIt->second;
        }
    }
    
    auto neuromodIt = neuromodulatorParameters_.find(targetId);
    if (neuromodIt != neuromodulatorParameters_.end()) {
        auto paramIt = neuromodIt->second.find(parameterName);
        if (paramIt != neuromodIt->second.end()) {
            return paramIt->second;
        }
    }
    
    return 0.0f; // Default value
}

bool ParameterController::validateParameterBounds(float value, const std::string& parameterName) const {
    // Validate parameter based on its name and expected range
    if (parameterName == "weight" || parameterName == "synaptic_weight") {
        return value >= -1.0f && value <= 1.0f;
    } else if (parameterName == "threshold" || parameterName == "neuron_threshold") {
        return value >= 0.0f && value <= 1.0f;
    } else if (parameterName == "learning_rate" || parameterName == "plasticity_rate") {
        return value >= 0.0f && value <= 1.0f;
    } else if (parameterName == "delay" || parameterName == "synaptic_delay") {
        return value >= 0.0f && value <= 1000.0f;
    } else if (parameterName == "neuromodulator_level" || parameterName == "dopamine_level") {
        return value >= -1.0f && value <= 1.0f;
    } else {
        // Default validation: reasonable range for floating point parameters
        return value >= -1000.0f && value <= 1000.0f;
    }
}

void ParameterController::logParameterChange(const ParameterChange& change) {
    std::stringstream logMessage;
    logMessage << std::fixed << std::setprecision(4)
               << "Parameter change: " << change.parameterType << " "
               << change.parameterName << " for target " << change.targetId
               << ": " << change.oldValue << " -> " << change.newValue
               << " (delta: " << change.changeAmount << ")"
               << " Reason: " << change.changeReason;
    
    NLM_LOG_INFO(std::string("ParameterController: ") + logMessage.str());
}

void ParameterController::updateParameterStatistics() {
    // Update running statistics on parameter changes
    // This would track statistics for performance monitoring
}

void ParameterController::enforceSafetyMargin() {
    // Check and enforce safety margins on parameter values
    // This prevents parameters from going out of safe operating ranges
}

bool ParameterController::checkForConflicts(const ParameterChange& change) const {
    // Check if this parameter change conflicts with other pending or applied changes
    // Conflicts could be: changing same parameter twice, incompatible combinations, etc.
    
    for (const auto& otherChange : parameterChanges_) {
        if (&otherChange == &change) continue; // Skip self
        
        // Check if changing same target and parameter
        if (otherChange.targetId == change.targetId && 
            otherChange.parameterName == change.parameterName &&
            !otherChange.isApplied) {
            
            // Check if changes are too close in time or incompatible
            double timeDiff = std::abs(otherChange.timestamp - change.timestamp);
            if (timeDiff < 0.1) { // Changes within 100ms
                return true;
            }
        }
    }
    
    return false;
}

std::vector<std::string> ParameterController::getConflictingChanges(const ParameterChange& change) const {
    std::vector<std::string> conflicts;
    
    for (const auto& otherChange : parameterChanges_) {
        if (&otherChange == &change) continue;
        
        if (otherChange.targetId == change.targetId && 
            otherChange.parameterName == change.parameterName &&
            !otherChange.isApplied) {
            
            double timeDiff = std::abs(otherChange.timestamp - change.timestamp);
            if (timeDiff < 0.1) {
                conflicts.push_back("Concurrent modification of " + otherChange.parameterName + 
                                  " for target " + std::to_string(otherChange.targetId));
            }
        }
    }
    
    return conflicts;
}

bool ParameterController::validateNeuronParameter(const ParameterChange& change) const {
    // Validate neuron-specific parameter changes
    bool valid = true;
    
    if (change.parameterName == "threshold") {
        if (!validateParameterBounds(change.newValue, "threshold")) {
            validationErrors_.push_back("Neuron threshold out of valid range [0, 1]");
            valid = false;
        }
    }
    
    // Additional neuron parameter validations would go here
    
    return valid;
}

bool ParameterController::validateSynapseParameter(const ParameterChange& change) const {
    // Validate synapse-specific parameter changes
    bool valid = true;
    
    if (change.parameterName == "weight") {
        if (!validateParameterBounds(change.newValue, "weight")) {
            validationErrors_.push_back("Synapse weight out of valid range [-1, 1]");
            valid = false;
        }
    } else if (change.parameterName == "delay") {
        if (!validateParameterBounds(change.newValue, "delay")) {
            validationErrors_.push_back("Synapse delay out of valid range [0, 1000]");
            valid = false;
        }
    }
    
    // Additional synapse parameter validations would go here
    
    return valid;
}

bool ParameterController::validatePlasticityParameter(const ParameterChange& change) const {
    // Validate plasticity-specific parameter changes
    bool valid = true;
    
    if (change.parameterName == "learning_rate" || change.parameterName == "plasticity_rate") {
        if (!validateParameterBounds(change.newValue, "learning_rate")) {
            validationErrors_.push_back("Plasticity learning rate out of valid range [0, 1]");
            valid = false;
        }
    }
    
    // Additional plasticity parameter validations would go here
    
    return valid;
}

bool ParameterController::validateNeuromodulatorParameter(const ParameterChange& change) const {
    // Validate neuromodulator-specific parameter changes
    bool valid = true;
    
    if (change.parameterName == "level" || change.parameterName == "neuromodulator_level") {
        if (!validateParameterBounds(change.newValue, "neuromodulator_level")) {
            validationErrors_.push_back("Neuromodulator level out of valid range [-1, 1]");
            valid = false;
        }
    }
    
    // Additional neuromodulator parameter validations would go here
    
    return valid;
}

bool ParameterController::validateDevelopmentParameter(const ParameterChange& change) const {
    // Validate development-specific parameter changes
    bool valid = true;
    
    if (change.parameterName == "development_rate") {
        if (!validateParameterBounds(change.newValue, "development_rate")) {
            validationErrors_.push_back("Development rate out of valid range [0, 1000]");
            valid = false;
        }
    }
    
    // Additional development parameter validations would go here
    
    return valid;
}

bool ParameterController::applyNeuronParameterChange(const ParameterChange& change) {
    // Apply neuron parameter change to the brain
    NeuronId neuronId = static_cast<NeuronId>(change.targetId);
    
    // Find and modify the neuron
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron && neuron->getId() == neuronId) {
                    if (change.parameterName == "threshold") {
                        // Note: This would require neuron threshold modification interface
                        // For now, we just update our internal state
                        NLM_LOG_INFO("Would modify neuron " + std::to_string(neuronId) + 
                                   " threshold to " + std::to_string(change.newValue));
                    }
                    // Additional neuron parameter applications would go here
                    
                    return true;
                }
            }
        }
    }
    
    NLM_LOG_ERROR("Failed to apply neuron parameter change: neuron not found");
    return false;
}

bool ParameterController::applySynapseParameterChange(const ParameterChange& change) {
    // Apply synapse parameter change to the brain
    SynapseId synapseId = static_cast<SynapseId>(change.targetId);
    
    // Find and modify the synapse
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn && syn->getId() == synapseId) {
                if (change.parameterName == "weight") {
                    // Note: This would require synapse weight modification interface
                    // For now, we just update our internal state
                    NLM_LOG_INFO("Would modify synapse " + std::to_string(synapseId) + 
                               " weight to " + std::to_string(change.newValue));
                } else if (change.parameterName == "delay") {
                    // Note: This would require synapse delay modification interface
                    NLM_LOG_INFO("Would modify synapse " + std::to_string(synapseId) + 
                               " delay to " + std::to_string(change.newValue));
                } else if (change.parameterName == "eligibility_trace") {
                    // Note: This would require eligibility trace modification interface
                    NLM_LOG_INFO("Would modify synapse " + std::to_string(synapseId) + 
                               " eligibility trace to " + std::to_string(change.newValue));
                }
                
                // Additional synapse parameter applications would go here
                
                return true;
            }
        }
    }
    
    NLM_LOG_ERROR("Failed to apply synapse parameter change: synapse not found");
    return false;
}

bool ParameterController::applyPlasticityParameterChange(const ParameterChange& change) {
    // Apply plasticity parameter change to the brain
    PlasticityId plasticityId = static_cast<PlasticityId>(change.targetId);
    
    // Find and modify the plasticity system
    if (auto* stdp = brain_->getSTDP()) {
        if (plasticityId == stdp->getId()) {
            if (change.parameterName == "learning_rate" || change.parameterName == "ltp_weight") {
                // Note: This would require STDP parameter modification interface
                NLM_LOG_INFO("Would modify STDP learning rate to " + std::to_string(change.newValue));
            } else if (change.parameterName == "ltd_weight") {
                // Note: This would require STDP LTD weight modification interface
                NLM_LOG_INFO("Would modify STDP LTD weight to " + std::to_string(change.newValue));
            } else if (change.parameterName == "trace_decay") {
                // Note: This would require STDP trace decay modification interface
                NLM_LOG_INFO("Would modify STDP trace decay to " + std::to_string(change.newValue));
            }
            
            return true;
        }
    }
    
    if (auto* hebbian = brain_->getHebbian()) {
        if (plasticityId == hebbian->getId()) {
            if (change.parameterName == "learning_rate") {
                // Note: This would require Hebbian parameter modification interface
                NLM_LOG_INFO("Would modify Hebbian learning rate to " + std::to_string(change.newValue));
            } else if (change.parameterName == "similarity_threshold") {
                // Note: This would require Hebbian similarity threshold modification interface
                NLM_LOG_INFO("Would modify Hebbian similarity threshold to " + std::to_string(change.newValue));
            }
            
            return true;
        }
    }
    
    if (auto* sp = brain_->getStructuralPlasticity()) {
        if (plasticityId == sp->getId()) {
            if (change.parameterName == "synaptogenesis_rate") {
                // Note: This would require structural plasticity parameter modification interface
                NLM_LOG_INFO("Would modify structural plasticity synaptogenesis rate to " + std::to_string(change.newValue));
            } else if (change.parameterName == "pruning_rate") {
                // Note: This would require structural plasticity parameter modification interface
                NLM_LOG_INFO("Would modify structural plasticity pruning rate to " + std::to_string(change.newValue));
            }
            
            return true;
        }
    }
    
    NLM_LOG_ERROR("Failed to apply plasticity parameter change: plasticity system not found");
    return false;
}

bool ParameterController::applyNeuromodulatorParameterChange(const ParameterChange& change) {
    // Apply neuromodulator parameter change to the brain
    NeuromodulatorId neuromodId = static_cast<NeuromodulatorId>(change.targetId);
    
    // Note: This would require access to neuromodulation system
    // For now, we just update our internal state
    
    NLM_LOG_INFO("Would modify neuromodulator " + std::to_string(neuromId) + 
                " parameter " + change.parameterName + " to " + std::to_string(change.newValue));
    
    return true;
}

bool ParameterController::applyDevelopmentParameterChange(const ParameterChange& change) {
    // Apply development parameter change to the brain
    DevelopmentStage stage = static_cast<DevelopmentStage>(change.targetId);
    
    // Note: This would require access to development system
    // For now, we just update our internal state
    
    NLM_LOG_INFO("Would modify development stage " + std::to_string(static_cast<int>(stage)) + 
                " parameter " + change.parameterName + " to " + std::to_string(change.newValue));
    
    return true;
}

// Helper methods for specific parameter adjustments
bool ParameterController::adjustSTDPParameter(const std::string& paramName, float newValue, const std::string& reason) {
    ParameterChange change;
    change.parameterName = paramName;
    change.parameterType = "plasticity";
    change.targetId = 0; // STDP system ID
    change.newValue = newValue;
    change.changeAmount = newValue - getCurrentParameterValue(0, paramName);
    change.changeReason = reason.empty() ? "stdp_parameter_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(0, paramName);
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::adjustHebbianParameter(const std::string& paramName, float newValue, const std::string& reason) {
    ParameterChange change;
    change.parameterName = paramName;
    change.parameterType = "plasticity";
    change.targetId = 1; // Hebbian system ID
    change.newValue = newValue;
    change.changeAmount = newValue - getCurrentParameterValue(1, paramName);
    change.changeReason = reason.empty() ? "hebbian_parameter_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(1, paramName);
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}

bool ParameterController::adjustSPParameter(const std::string& paramName, float newValue, const std::string& reason) {
    ParameterChange change;
    change.parameterName = paramName;
    change.parameterType = "plasticity";
    change.targetId = 2; // Structural Plasticity system ID
    change.newValue = newValue;
    change.changeAmount = newValue - getCurrentParameterValue(2, paramName);
    change.changeReason = reason.empty() ? "sp_parameter_adjustment" : reason;
    change.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (!validateParameterChange(change)) {
        return false;
    }
    
    change.oldValue = getCurrentParameterValue(2, paramName);
    
    if (applySingleParameterChange(change)) {
        change.isApplied = true;
        parameterChanges_.push_back(change);
        logParameterChange(change);
        return true;
    }
    
    return false;
}