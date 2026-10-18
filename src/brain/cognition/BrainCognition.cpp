#include "brain/cognition/BrainCognition.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <random>

namespace nlm {

BrainCognition::BrainCognition(std::shared_ptr<Config> config)
    : config_(config)
    , activeConcepts_(0)
    , currentPlanDepth_(0)
    , attentionFocus_(0.5f)
    , planningConfidence_(0.8f)
{
    if (!config_) {
        NLM_LOG_ERROR("Configuration is null in BrainCognition constructor");
    }
    
    // Initialize action descriptions
    actionDescriptions_ = {
        "MoveForward",
        "MoveBackward", 
        "TurnLeft",
        "TurnRight",
        "Interact",
        "Wait",
        "LookLeft",
        "LookRight"
    };
    
    NLM_LOG_INFO("BrainCognition initialized");
}

BrainCognition::~BrainCognition() = default;

bool BrainCognition::initialize(const std::vector<Neuron*>& sensoryNeurons,
                               const std::vector<Neuron*>& motorNeurons) {
    NLM_LOG_INFO("Initializing brain cognition systems...");
    
    // Initialize cognition systems
    planner_ = std::make_unique<NeuralPlanner>();
    conceptFormation_ = std::make_unique<ConceptFormation>();
    attention_ = std::make_unique<AttentionalSelection>();
    
    // Initialize integration systems if provided
    if (workingMemory_) {
        // Working memory is set externally
    }
    
    curiosity_ = std::make_unique<Curiosity>();
    dopamine_ = std::make_unique<Dopamine>();
    
    // Initialize cognition systems with references if available
    if (planner_) {
        planner_->initialize(nullptr);  // Would pass brain reference
        planner_->setPlanningDepth(5);
    }
    
    if (conceptFormation_) {
        conceptFormation_->initialize(nullptr);  // Would pass brain reference
    }
    
    if (attention_) {
        attention_->initialize(nullptr);  // Would pass brain reference
        attention_->setInhibitionStrength(0.5f);
        attention_->setExcitationStrength(1.5f);
    }
    
    if (curiosity_) {
        curiosity_->initialize(nullptr);  // Would pass brain reference
    }
    
    if (dopamine_) {
        dopamine_->initialize(nullptr);  // Would pass brain reference
    }
    
    // Validate configuration
    if (!validateCognitionConfig(config_)) {
        NLM_LOG_ERROR("Cognition system configuration validation failed");
        return false;
    }
    
    NLM_LOG_INFO("Brain cognition systems initialized");
    return true;
}

void BrainCognition::reset() {
    NLM_LOG_INFO("Resetting brain cognition systems...");
    
    actionHistory_.clear();
    stimulusResponses_.clear();
    stimulusResponseMap_.clear();
    
    activeConcepts_ = 0;
    currentPlanDepth_ = 0;
    attentionFocus_ = 0.5f;
    planningConfidence_ = 0.8f;
    
    if (planner_) {
        // Reset planner state
    }
    
    if (attention_) {
        // Reset attention state
    }
    
    NLM_LOG_INFO("Brain cognition systems reset");
}

void BrainCognition::updatePlanning(double timestep) {
    if (!planner_) {
        return;
    }
    
    // Update planning based on current state
    // This would integrate with working memory, attention, etc.
    
    // Simple planning depth adjustment based on confidence
    if (planningConfidence_ < 0.5f) {
        currentPlanDepth_ = std::max(1, currentPlanDepth_ - 1);
    } else {
        currentPlanDepth_ = std::min(10, currentPlanDepth_ + 1);
    }
    
    // Update planning confidence based on attention focus
    planningConfidence_ = std::clamp(attentionFocus_, 0.1f, 1.0f);
}

void BrainCognition::updateConceptFormation(double timestep) {
    if (!conceptFormation_) {
        return;
    }
    
    // Update concept formation based on sensory input and experience
    // This would identify patterns and form abstract concepts
    
    // Simple concept activation based on stimulus-response history
    for (const auto& response : stimulusResponses_) {
        if (response.responseCount > 0) {
            activeConcepts_++;
        }
    }
    
    // Decay inactive concepts
    activeConcepts_ = std::max(0, activeConcepts_ - 1);
}

void BrainCognition::updateAttention(double timestep) {
    if (!attention_) {
        return;
    }
    
    // Update attention system
    attention_->update(timestep);
    
    // Adjust attention focus based on curiosity and prediction error
    float curiosityLevel = curiosity_ ? curiosity_->getLevel() : 0.0f;
    float predictionError = dopamine_ ? dopamine_->getLevel() : 0.0f;
    
    attentionFocus_ = 0.5f + (curiosityLevel * 0.3f) - (std::abs(predictionError) * 0.2f);
    attentionFocus_ = std::clamp(attentionFocus_, 0.0f, 1.0f);
}

void BrainCognition::processSensoryInput(const std::vector<float>& sensoryData) {
    if (sensoryData.empty()) {
        return;
    }
    
    // Store sensory input for concept formation
    if (conceptFormation_) {
        // This would be passed to concept formation system
        // For now, store in action history
        if (!actionHistory_.empty()) {
            // Process sensory input based on current action
            recordExperience(sensoryData, actionHistory_.back(), 0.0f);  // Placeholder outcome
        }
    }
    
    // Update attention based on novelty
    updateAttention(0.0);  // Placeholder timestep
}

void BrainCognition::integrateWithWorkingMemory(const std::vector<Neuron*>& activeNeurons) {
    if (!workingMemory_ || activeNeurons.empty()) {
        return;
    }
    
    // Integrate active neurons with working memory
    // This would strengthen relevant memory traces
    
    for (const auto* neuron : activeNeurons) {
        workingMemory_->storeToNeuron(neuron->getId(), 1.0f);
    }
}

size_t BrainCognition::selectAction() const {
    if (actionHistory_.empty()) {
        return 0;  // Default to first action
    }
    
    // Select action based on history and current state
    // This is a simple reinforcement learning approach
    
    // Count action frequencies
    std::map<size_t, size_t> actionCounts;
    for (size_t action : actionHistory_) {
        actionCounts[action]++;
    }
    
    // Select most frequent action, with some randomness for exploration
    std::vector<std::pair<size_t, size_t>> actionList(actionCounts.begin(), actionCounts.end());
    std::sort(actionList.begin(), actionList.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (actionList.empty()) {
        return 0;
    }
    
    // Choose best action
    size_t selectedAction = actionList[0].first;
    
    // Add exploration based on curiosity
    float curiosityLevel = curiosity_ ? curiosity_->getLevel() : 0.0f;
    if (curiosityLevel > 0.5f && actionList.size() > 1) {
        // Randomly select from top 2 actions for exploration
        size_t topAction = actionList[0].first;
        size_t secondAction = actionList[1].first;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        if (dis(gen) < curiosityLevel * 0.5f) {
            selectedAction = secondAction;
        }
    }
    
    return selectedAction;
}

std::string BrainCognition::getSelectedActionDescription() const {
    size_t actionIndex = selectAction();
    if (actionIndex < actionDescriptions_.size()) {
        return actionDescriptions_[actionIndex];
    }
    return "Unknown";
}

void BrainCognition::learnFromExperience(const std::vector<float>& experience) {
    // Store experience for learning
    // This would be used for concept formation and action selection
    
    // Simple experience recording
    std::string experienceKey = "exp_" + std::to_string(experience.size()) + "_";
    for (float val : experience) {
        experienceKey += std::to_string(static_cast<int>(val)) + "_";
    }
    
    // Record experience
    // This would integrate with concept formation system
}

void BrainCognition::updateConcepts(const std::vector<float>& patterns) {
    if (!conceptFormation_) {
        return;
    }
    
    // Update concepts based on patterns
    // This would identify and update abstract concepts
    
    // Simple pattern-based concept update
    for (float pattern : patterns) {
        if (pattern > 0.5f) {  // Significant pattern
            activeConcepts_++;
        }
    }
}

void BrainCognition::adjustAttentionWeights(float novelty, float predictionError) {
    // Adjust attention weights based on neuromodulatory signals
    attentionFocus_ = 0.5f + (novelty * 0.3f) - (predictionError * 0.2f);
    attentionFocus_ = std::clamp(attentionFocus_, 0.0f, 1.0f);
}

void BrainCognition::recordExperience(const std::vector<float>& stimulus, size_t action, float outcome) {
    StimulusResponse response;
    response.stimulus = stimulus;
    response.actionIndex = action;
    response.confidence = std::abs(outcome);
    response.responseCount = 1;
    
    stimulusResponses_.push_back(response);
    
    // Update stimulus-response map
    std::string key = "stim_";
    for (float val : stimulus) {
        key += std::to_string(static_cast<int>(val)) + "_";
    }
    
    stimulusResponseMap_[key].push_back(response);
    
    // Limit stored responses
    if (stimulusResponses_.size() > 1000) {
        stimulusResponses_.erase(stimulusResponses_.begin());
    }
}

bool BrainCognition::validateCognitionConfig(const std::shared_ptr<const Config>& config) {
    if (!config) {
        return false;
    }
    
    // Validate cognition-related configuration
    auto planningDepth = config->getOr<size_t>("planning_depth", 5);
    if (planningDepth < 1 || planningDepth > 20) {
        NLM_LOG_ERROR("Invalid planning_depth: " + std::to_string(planningDepth));
        return false;
    }
    
    auto attentionStrength = config->getOr<float>("attention_strength", 1.0f);
    if (attentionStrength < 0.0f || attentionStrength > 2.0f) {
        NLM_LOG_ERROR("Invalid attention_strength: " + std::to_string(attentionStrength));
        return false;
    }
    
    return true;
}

void BrainCognition::applyDefaultCognitionSettings(std::shared_ptr<Config> config) {
    if (!config) {
        return;
    }
    
    // Apply default cognition settings
    config->set("planning_depth", 5);
    config->set("attention_strength", 1.0f);
    config->set("curiosity_enabled", true);
    config->set("development_enabled", true);
}

} // namespace nlm
