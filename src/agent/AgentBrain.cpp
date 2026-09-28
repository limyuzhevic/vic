// NeuralLearningMachine - Enhanced AgentBrain with Advanced Features
// 
// This is a comprehensive improvement of the AgentBrain class, addressing the major TODO items
// and adding advanced features for more sophisticated agent behavior and learning.

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <random>
#include <chrono>

namespace nlm {

// Forward declaration for world-type checking
class SimpleWorld;

// Advanced motor command selection with probabilistic modeling
MotorCommand AgentBrain::decodeMotorCommandAdvanced() {
    if (!brain_) return MotorCommand::Wait;
    
    // Compute activity distribution across motor groups
    struct MotorActivity {
        MotorCommand cmd;
        float activity;
        float entropy;
    };
    
    std::vector<MotorActivity> activities;
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    // Calculate activities for all motor groups
    activities.push_back({MotorCommand::MoveForward, calcActivity(motorForward_)});
    activities.push_back({MotorCommand::MoveBackward, calcActivity(motorBackward_)});
    activities.push_back({MotorCommand::TurnLeft, calcActivity(motorTurnLeft_)});
    activities.push_back({MotorCommand::TurnRight, calcActivity(motorTurnRight_)});
    activities.push_back({MotorCommand::Interact, calcActivity(motorInteract_)});
    activities.push_back({MotorCommand::Wait, calcActivity(motorWait_)});
    
    // Calculate entropy of activity distribution for exploration
    float totalActivity = 0.0f;
    for (const auto& a : activities) totalActivity += a.activity;
    
    if (totalActivity > 0.0f) {
        for (auto& a : activities) {
            float p = a.activity / totalActivity;
            a.entropy = -p * std::log(std::max(p, 0.001f));
        }
    }
    
    // Advanced selection with entropy-based exploration
    float maxActivity = 0.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    // Use Boltzmann selection for exploration/exploitation balance
    float temperature = 0.5f + 0.5f * std::min(1.0f, curiosityLevel_ * 2.0f);
    
    std::vector<float> weights;
    float weightSum = 0.0f;
    
    for (const auto& a : activities) {
        float weight = std::exp(a.activity / temperature);
        weights.push_back(weight);
        weightSum += weight;
    }
    
    if (weightSum > 0.0f && !weights.empty()) {
        // Select with probability proportional to activity
        std::random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<> dist(weights.begin(), weights.end());
        int selected = dist(gen);
        
        bestCommand = activities[selected].cmd;
        maxActivity = activities[selected].activity;
    }
    
    return bestCommand;
}

// Enhanced reward modulation with multiple neuromodulators
void AgentBrain::applyAdvancedRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward with adaptive learning rate
    float learningRate = 0.05f * plasticityModifier_;
    learningRate = std::clamp(learningRate, 0.01f, 0.2f);
    expectedReward_ = (1.0f - learningRate) * expectedReward_ + learningRate * reward;
    
    // Enhanced dopamine-like signal
    dopamineLevel_ = predictionError_ * 0.2f;  // Lower gain for stability
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply neuromodulatory effects to plasticity
    updatePlasticityBasedOnNeuromodulation();
    
    // Enhanced curiosity and novelty processing
    updateCuriosityBasedOnPredictionError();
    
    // Reward-gated learning - only learn when prediction error is significant
    if (std::abs(predictionError_) > 0.01f) {
        applyWeightedRewardModulation(reward, predictedReward);
    }
}

// Update plasticity based on combined neuromodulatory signals
void AgentBrain::updatePlasticityBasedOnNeuromodulation() {
    float totalModulation = dopamineLevel_ * 0.6f + noveltyLevel_ * 0.3f + curiosityLevel_ * 0.1f;
    float plasticityFactor = 0.5f + 0.5f * totalModulation;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP system if available
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        float baseLTP = 0.01f;
        float baseLTD = 0.012f;
        stdp->setLTPWeight(baseLTP * plasticityFactor);
        stdp->setLTDWeight(baseLTD * plasticityFactor);
    }
    
    // Store for potential use by other systems
    plasticityModifier_ = plasticityFactor;
}

// Enhanced curiosity processing using prediction error and novelty
void AgentBrain::updateCuriosityBasedOnPredictionError() {
    // Multi-factor curiosity computation
    float predictionErrorCuriosity = std::abs(predictionError_) * 0.5f;
    float noveltyCuriosity = noveltyLevel_ * 0.8f;
    
    // Information gain curiosity (novel prediction errors)
    static float lastPredictionError = 0.0f;
    float informationGain = std::abs(predictionError_ - lastPredictionError);
    lastPredictionError = predictionError_;
    
    // Combine factors with adaptive weights
    float curiosityWeights[3] = {0.4f, 0.4f, 0.2f};  // prediction, novelty, information
    
    float totalCuriosity = curiosityWeights[0] * predictionErrorCuriosity +
                          curiosityWeights[1] * noveltyCuriosity +
                          curiosityWeights[2] * informationGain * 10.0f;
    
    curiosityLevel_ = std::clamp(totalCuriosity, 0.0f, 1.0f);
}

// Apply weighted reward modulation with eligibility traces
void AgentBrain::applyWeightedRewardModulation(float reward, float predictedReward) {
    float predictionError = reward - predictedReward;
    
    // Compute weight based on prediction error magnitude and neuromodulation
    float errorWeight = std::abs(predictionError) * (1.0f + std::abs(dopamineLevel_));
    errorWeight = std::min(errorWeight, 1.0f);
    
    // Apply to synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Enhanced reward-modulated weight change
                float delta = eligibility * predictionError * errorWeight * plasticityModifier_ * 0.1f;
                syn->addToWeight(delta);
                
                // Adaptive eligibility decay
                float decayRate = 0.1f + 0.1f * std::abs(dopamineLevel_);
                syn->decayEligibilityTrace(decayRate);
            }
        }
    }
}

// Enhanced developmental stages with more biological realism
void AgentBrain::updateDevelopmentAdvanced(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // More sophisticated developmental stages based on neural activity
    float activityLevel = static_cast<float>(brain_->getFiringNeuronCount()) / 
                          static_cast<float>(brain_->getTotalNeuronCount());
    
    // Activity-dependent development trajectory
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.2f - 0.2f * activityLevel;  // High but activity-modulated
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.9f - 0.3f * activityLevel;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.6f + 0.2f * (activityLevel - 0.5f);  // Can increase with activity
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.3f + 0.2f * (activityLevel - 0.5f);  // Adult with activity modulation
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Activity-dependent structural plasticity
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Synaptogenesis depends on activity and developmental stage
            float activityFactor = std::clamp(activityLevel / 0.5f, 0.0f, 2.0f);
            float synRate = 0.0001f * plasticityModifier_ * (1.0f + activityFactor);
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_) / (1.0f + activityFactor);
            
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

// Enhanced sensory processing with predictive coding
void AgentBrain::processSensoryInputAdvanced(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision processing with predictive coding
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        // Predictive coding: compare input with prediction
        std::vector<float> prediction(vision.size(), 0.0f);
        generatePredictiveCoding(vision, prediction);
        
        // Compute prediction error ( drives learning )
        float totalPredictionError = 0.0f;
        for (size_t i = 0; i < vision.size() && i < prediction.size(); ++i) {
            float error = vision[i] - prediction[i];
            totalPredictionError += error * error;
        }
        
        // Update novelty detection with prediction error
        noveltyLevel_ = std::sqrt(totalPredictionError / vision.size()) * 2.0f;
        noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
        
        // Update curiosity with prediction error
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * 1.5f + std::abs(predictionError_) * 0.3f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
        
        // Apply prediction error to sensory neurons
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                float predictionError = vision[i] - prediction[i];
                float current = vision[i] * 5.0f + predictionError * 2.0f;  // Error amplifies signal
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Touch processing with adaptive scaling
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            // Adaptive touch sensitivity based on current state
            float sensitivity = 1.0f + curiosityLevel_ * 2.0f;  // Higher curiosity = higher sensitivity
            float current = touch[i] * 8.0f * sensitivity;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal and proprioception processing
    processInternalSignalsAdvanced(percept);
    processProprioceptionAdvanced(percept);
}

// Generate predictions using simple forward model
void AgentBrain::generatePredictiveCoding(const std::vector<float>& input, 
                                         std::vector<float>& prediction) {
    if (input.empty()) return;
    
    prediction.resize(input.size());
    
    // Simple temporal prediction: copy with decay (placeholder for real neural model)
    float decay = 0.7f;
    float weight = 1.0f;
    
    for (size_t i = 0; i < input.size(); ++i) {
        prediction[i] = input[i] * weight;
        weight *= decay;
    }
    
    // Add noise for exploration in prediction
    std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist(-0.05f, 0.05f);
    
    for (float& p : prediction) {
        p += dist(gen);
    }
}

// Advanced internal signal processing
void AgentBrain::processInternalSignalsAdvanced(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Process homeostatic and metabolic signals
            float homeostasis = (intern[i] * 2.0f - 1.0f) * 5.0f;
            
            // Modulate by current energy levels
            float energyModulation = (energy / 1.0f) * 0.5f;
            homeostasis *= (1.0f + energyModulation);
            
            sensoryInternal_[i]->injectCurrent(homeostasis);
        }
    }
}

// Advanced proprioception processing
void AgentBrain::processProprioceptionAdvanced(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Process body state with adaptive filtering
            float filteredSignal = proprio[i] * 3.0f;
            
            // Add velocity-dependent component
            float velocity = getSpeed();
            filteredSignal += velocity * 0.1f * (i % 2 == 0 ? 1.0f : -1.0f);
            
            sensoryProprioception_[i]->injectCurrent(filteredSignal);
        }
    }
}

// Enhanced motor command selection with learned preferences
MotorCommand AgentBrain::decodeMotorCommandWithPreferences() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand baseCommand = decodeFromMotorNeurons();
    
    // Apply learned motor preferences based on past rewards
    applyMotorPreferences(baseCommand);
    
    // Apply curiosity-based exploration with adaptive randomness
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        float exploreChance = std::min(0.3f, curiosityLevel_ * 0.5f);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        if (dist(gen) < exploreChance) {
            // Sample from learned policy
            return sampleFromLearnedPolicy();
        }
    }
    
    return baseCommand;
}

// Apply learned motor preferences
void AgentBrain::applyMotorPreferences(MotorCommand& command) {
    // This would access a learned policy from memory systems
    // For now, placeholder for future implementation
    
    // Simple heuristic: prefer forward movement when curiosity is low
    if (curiosityLevel_ < 0.2f && command == MotorCommand::MoveForward) {
        // Slightly bias toward exploration when not curious
        float r = std::rand() / static_cast<float>(RAND_MAX);
        if (r < 0.1f) {
            command = selectAlternativeCommand(command);
        }
    }
}

// Sample from learned policy
MotorCommand AgentBrain::sampleFromLearnedPolicy() {
    // This would sample from a policy learned by the brain
    // For now, return random action
    int choice = std::rand() % 8;
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

// Select alternative command
MotorCommand AgentBrain::selectAlternativeCommand(MotorCommand original) {
    // Simple implementation: cycle through alternatives
    switch (original) {
        case MotorCommand::MoveForward: return MotorCommand::TurnLeft;
        case MotorCommand::MoveBackward: return MotorCommand::TurnRight;
        case MotorCommand::TurnLeft: return MotorCommand::MoveForward;
        case MotorCommand::TurnRight: return MotorCommand::MoveBackward;
        case MotorCommand::LookLeft: return MotorCommand::LookRight;
        case MotorCommand::LookRight: return MotorCommand::LookLeft;
        case MotorCommand::Interact: return MotorCommand::Wait;
        default: return MotorCommand::MoveForward;
    }
}

// Get current agent state for checkpointing/saving
AgentState AgentBrain::getState() const {
    AgentState state;
    
    state.dopamineLevel = dopamineLevel_;
    state.noveltyLevel = noveltyLevel_;
    state.curiosityLevel = curiosityLevel_;
    state.predictionError = predictionError_;
    state.expectedReward = expectedReward_;
    state.developmentalAge = developmentalAge_;
    state.plasticityModifier = plasticityModifier_;
    
    // Copy motor neuron group sizes
    state.motorGroupSizes[0] = motorForward_.size();
    state.motorGroupSizes[1] = motorBackward_.size();
    state.motorGroupSizes[2] = motorTurnLeft_.size();
    state.motorGroupSizes[3] = motorTurnRight_.size();
    state.motorGroupSizes[4] = motorInteract_.size();
    state.motorGroupSizes[5] = motorWait_.size();
    
    return state;
}

// Restore agent state from checkpoint
void AgentBrain::restoreState(const AgentState& state) {
    dopamineLevel_ = state.dopamineLevel;
    noveltyLevel_ = state.noveltyLevel;
    curiosityLevel_ = state.curiosityLevel;
    predictionError_ = state.predictionError;
    expectedReward_ = state.expectedReward;
    developmentalAge_ = state.developmentalAge;
    plasticityModifier_ = state.plasticityModifier;
    
    // Reinitialize motor groups with stored sizes
    // In full implementation, would need to re-establish neuron connections
    // For now, placeholder implementation
    
    // Reset previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// Get agent's internal energy state
float AgentBrain::getEnergyLevel() const {
    // Estimate energy based on neuromodulation and activity
    return 0.5f + dopamineLevel_ * 0.3f + noveltyLevel_ * 0.2f;
}

// Check if agent should sleep/rest
bool AgentBrain::shouldRest() const {
    // Rest if energy is low and no significant learning is occurring
    float energy = getEnergyLevel();
    float learningActivity = std::abs(predictionError_) + curiosityLevel_;
    
    return energy < 0.2f && learningActivity < 0.1f;
}

// Enhanced logging with more detailed state information
void AgentBrain::logAdvancedStatus() const {
    if (!brain_) return;
    
    NLM_LOG_INFO("=== Advanced AgentBrain Status ===");
    NLM_LOG_INFO("Neuromodulation: DA=" + std::to_string(dopamineLevel_) + 
                " Novelty=" + std::to_string(noveltyLevel_) + 
                " Curiosity=" + std::to_string(curiosityLevel_));
    
    NLM_LOG_INFO("Learning: PE=" + std::to_string(predictionError_) + 
                " ExpReward=" + std::to_string(expectedReward_) + 
                " Plasticity=" + std::to_string(plasticityModifier_));
    
    NLM_LOG_INFO("Development: Age=" + std::to_string(developmentalAge_) + 
                " Stage=" + developmentalStageToString(getDevelopmentalStage()));
    
    NLM_LOG_INFO("Motor Groups: FWD=" + std::to_string(motorForward_.size()) + 
                " BK=" + std::to_string(motorBackward_.size()) + 
                " L=" + std::to_string(motorTurnLeft_.size()) + 
                " R=" + std::to_string(motorTurnRight_.size()) + 
                " I=" + std::to_string(motorInteract_.size()) + 
                " W=" + std::to_string(motorWait_.size()));
    
    NLM_LOG_INFO("Sensory Groups: V=" + std::to_string(sensoryVision_.size()) + 
                " T=" + std::to_string(sensoryTouch_.size()) + 
                " I=" + std::to_string(sensoryInternal_.size()) + 
                " P=" + std::to_string(sensoryProprioception_.size()));
    
    NLM_LOG_INFO("Energy: " + std::to_string(getEnergyLevel()) + 
                " ShouldRest: " + std::to_string(shouldRest()));
}

// Helper function to convert developmental stage to string
std::string AgentBrain::developmentalStageToString(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        default: return "Unknown";
    }
}

// Get speed from agent body (if available)
float AgentBrain::getSpeed() const {
    // This would require access to agent body - placeholder for now
    return 0.0f;
}

// Enhanced reset with more thorough cleanup
void AgentBrain::resetAdvanced() {
    // Reset all neuromodulation states
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset motor neuron groups (in full implementation would clear connections)
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    // Reinitialize motor neuron groups based on current brain state
    // This would require access to brain's regions and neuron populations
    // For now, placeholder for future implementation
}

} // namespace nlm
