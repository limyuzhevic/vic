#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , predictionEnabled_(true)
    , conceptFormationEnabled_(true)
    , planningEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.visionWidth_ * world.visionHeight_, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize agent body with world dimensions
    agentBody_.x = 0.0f;
    agentBody_.y = 0.0f;
    agentBody_.orientation = 0.0f;
    agentBody_.velocityX = 0.0f;
    agentBody_.velocityY = 0.0f;
    agentBody_.angularVelocity = 0.0f;
    agentBody_.energy = 1.0f;
    agentBody_.health = 1.0f;
    agentBody_.age = 0.0f;
    agentBody_.isMoving = false;
    agentBody_.isTurning = false;
    agentBody_.lastActionTime = 0.0f;
    
    // Initialize new subsystem flags
    predictionEnabled_ = true;
    conceptFormationEnabled_ = true;
    planningEnabled_ = true;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Subsystem management methods
void AgentBrain::enableSubsystem(SubsystemType type) {
    switch (type) {
        case SubsystemType::RewardModulation:
            rewardModulationEnabled_ = true;
            break;
        case SubsystemType::StructuralPlasticity:
            structuralPlasticityEnabled_ = true;
            break;
        case SubsystemType::Development:
            developmentEnabled_ = true;
            break;
        case SubsystemType::Curiosity:
            curiosityEnabled_ = true;
            break;
        case SubsystemType::Prediction:
            predictionEnabled_ = true;
            break;
        case SubsystemType::ConceptFormation:
            conceptFormationEnabled_ = true;
            break;
        case SubsystemType::Planning:
            planningEnabled_ = true;
            break;
        case SubsystemType::All:
            rewardModulationEnabled_ = true;
            structuralPlasticityEnabled_ = true;
            developmentEnabled_ = true;
            curiosityEnabled_ = true;
            predictionEnabled_ = true;
            conceptFormationEnabled_ = true;
            planningEnabled_ = true;
            break;
    }
}

void AgentBrain::disableSubsystem(SubsystemType type) {
    switch (type) {
        case SubsystemType::RewardModulation:
            rewardModulationEnabled_ = false;
            break;
        case SubsystemType::StructuralPlasticity:
            structuralPlasticityEnabled_ = false;
            break;
        case SubsystemType::Development:
            developmentEnabled_ = false;
            break;
        case SubsystemType::Curiosity:
            curiosityEnabled_ = false;
            break;
        case SubsystemType::Prediction:
            predictionEnabled_ = false;
            break;
        case SubsystemType::ConceptFormation:
            conceptFormationEnabled_ = false;
            break;
        case SubsystemType::Planning:
            planningEnabled_ = false;
            break;
        case SubsystemType::All:
            rewardModulationEnabled_ = false;
            structuralPlasticityEnabled_ = false;
            developmentEnabled_ = false;
            curiosityEnabled_ = false;
            predictionEnabled_ = false;
            conceptFormationEnabled_ = false;
            planningEnabled_ = false;
            break;
    }
}

void AgentBrain::toggleSubsystem(SubsystemType type) {
    switch (type) {
        case SubsystemType::RewardModulation:
            rewardModulationEnabled_ = !rewardModulationEnabled_;
            break;
        case SubsystemType::StructuralPlasticity:
            structuralPlasticityEnabled_ = !structuralPlasticityEnabled_;
            break;
        case SubsystemType::Development:
            developmentEnabled_ = !developmentEnabled_;
            break;
        case SubsystemType::Curiosity:
            curiosityEnabled_ = !curiosityEnabled_;
            break;
        case SubsystemType::Prediction:
            predictionEnabled_ = !predictionEnabled_;
            break;
        case SubsystemType::ConceptFormation:
            conceptFormationEnabled_ = !conceptFormationEnabled_;
            break;
        case SubsystemType::Planning:
            planningEnabled_ = !planningEnabled_;
            break;
        case SubsystemType::All:
            rewardModulationEnabled_ = !rewardModulationEnabled_;
            structuralPlasticityEnabled_ = !structuralPlasticityEnabled_;
            developmentEnabled_ = !developmentEnabled_;
            curiosityEnabled_ = !curiosityEnabled_;
            predictionEnabled_ = !predictionEnabled_;
            conceptFormationEnabled_ = !conceptFormationEnabled_;
            planningEnabled_ = !planningEnabled_;
            break;
    }
}

bool AgentBrain::isSubsystemEnabled(SubsystemType type) const {
    switch (type) {
        case SubsystemType::RewardModulation:
            return rewardModulationEnabled_;
        case SubsystemType::StructuralPlasticity:
            return structuralPlasticityEnabled_;
        case SubsystemType::Development:
            return developmentEnabled_;
        case SubsystemType::Curiosity:
            return curiosityEnabled_;
        case SubsystemType::Prediction:
            return predictionEnabled_;
        case SubsystemType::ConceptFormation:
            return conceptFormationEnabled_;
        case SubsystemType::Planning:
            return planningEnabled_;
        case SubsystemType::All:
            return rewardModulationEnabled_ && structuralPlasticityEnabled_ &&
                   developmentEnabled_ && curiosityEnabled_ &&
                   predictionEnabled_ && conceptFormationEnabled_ &&
                   planningEnabled_;
    }
    return false;
}

// System status methods
SystemStatus AgentBrain::getSystemStatus() const {
    SystemStatus status;
    status.curiosity = curiosityLevel_;
    status.novelty = noveltyLevel_;
    status.predictionError = predictionError_;
    status.dopamineLevel = dopamineLevel_;
    status.expectedReward = expectedReward_;
    status.developmentalAge = developmentalAge_;
    status.developmentalStage = getDevelopmentalStage();
    status.plasticityModifier = plasticityModifier_;
    status.isHealthy = checkSystemHealth();
    return status;
}

// Convenience methods
bool AgentBrain::isActive() const {
    return brain_ != nullptr && checkSystemHealth();
}

std::string AgentBrain::getStatusSummary() const {
    SystemStatus status = getSystemStatus();
    std::string summary = "AgentBrain: ";
    summary += "Stage=" + std::to_string(static_cast<int>(status.developmentalStage)) + " ";
    summary += "Age=" + std::to_string(status.developmentalAge) + " ";
    summary += "Curiosity=" + std::to_string(status.curiosity) + " ";
    summary += "Novelty=" + std::to_string(status.novelty) + " ";
    summary += "Healthy=" + std::string(status.isHealthy ? "Yes" : "No");
    return summary;
}

MotorCommand AgentBrain::getRecommendedAction() const {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand recommended = decodeMotorCommandRobust();
    
    // Apply curiosity if enabled
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        recommended = selectWithCuriosity(recommended);
    }
    
    return recommended;
}

// Enhanced motor decoding methods
MotorCommand AgentBrain::decodeMotorCommandRobust() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Use neural planner for better action selection
    auto* planner = brain_->getPlanner();
    if (planner) {
        // Create state vector from current neural activity
        std::vector<float> currentState;
        float totalActivity = 0.0f;
        
        for (auto* neuron : motorForward_) {
            totalActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
        for (auto* neuron : motorBackward_) {
            totalActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
        for (auto* neuron : motorTurnLeft_) {
            totalActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
        for (auto* neuron : motorTurnRight_) {
            totalActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
        
        // Normalize to [0,1]
        currentState.push_back(std::clamp(totalActivity / 10.0f, 0.0f, 1.0f));
        
        // Set goal based on current action quality
        auto* cf = brain_->getConceptFormation();
        if (cf) {
            // Use highest quality action as goal
            float bestQuality = -1.0f;
            ActionType bestAction = ActionType::Wait;
            
            for (size_t i = 0; i < cf->getConceptCount(); ++i) {
                const auto* concept = cf->getConcept(i);
                if (concept) {
                    // This is simplified - real implementation would use action planning
                    float quality = concept->avgStability * concept->avgReward;
                    if (quality > bestQuality) {
                        bestQuality = quality;
                        bestAction = concept->categoryHint == "rewarding" ? ActionType::MoveForward : ActionType::Wait;
                    }
                }
            }
            
            if (bestQuality > 0.3f) {
                currentState.push_back(bestQuality);
            } else {
                currentState.push_back(0.5f); // Default to neutral goal
            }
        } else {
            currentState.push_back(0.5f); // Default goal
        }
        
        // Plan action using neural planner
        ActionType plannedAction = planner->planAction(currentState, 0.5f);
        
        // Use planner's action if it's different from basic decoding
        // This allows planning to override reflexive responses
        if (plannedAction != ActionType::Wait && plannedAction != decoded) {
            // Log planning override
            NLM_LOG_INFO("Planner override: " + std::to_string(static_cast<int>(decoded)) + " -> " + std::to_string(static_cast<int>(plannedAction)));
            return plannedAction;
        }
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

float AgentBrain::getMotorConflictLevel() const {
    // Calculate conflict between competing motor commands
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Calculate maximum activity
    float maxActivity = std::max({forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct});
    
    // Calculate conflict as 1 - (max / sum of activities)
    float totalActivity = forwardAct + backwardAct + leftAct + rightAct + interactAct + waitAct;
    if (totalActivity > 0.0f) {
        return 1.0f - (maxActivity / totalActivity);
    }
    
    return 0.0f;
}

// Agent-brain integration methods
void AgentBrain::syncAgentBrain(const AgentBody& agentBody) {
    agentBody_ = agentBody;
    agentBody_.age += 1.0f; // Increment age each sync
    
    NLM_LOG_INFO("AgentBrain synced with agent body: pos(" + std::to_string(agentBody_.x) + ", " + 
                 std::to_string(agentBody_.y) + "), energy=" + std::to_string(agentBody_.energy));
}

const AgentBody& AgentBrain::getAgentBody() const {
    return agentBody_;
}

void AgentBrain::updateAgentFromWorld(const SimpleWorld& world) {
    // Update agent state from world simulation
    const auto& body = world.getAgentBody();
    agentBody_ = body;
    
    // Update world energy decay on agent
    agentBody_.energy = std::max(0.0f, agentBody_.energy - world.energyDecayRate_ * 0.01f);
    
    // Check for resource collection
    auto* obj = world.getObjectAt(agentBody_.x, agentBody_.y);
    if (obj && obj->type == WorldObjectType::Resource) {
        agentBody_.energy = std::min(1.0f, agentBody_.energy + obj->value * 0.5f);
        world.removeObject(agentBody_.x, agentBody_.y);
        NLM_LOG_INFO("Agent collected resource: energy increased to " + std::to_string(agentBody_.energy));
    }
}

// Development and learning control methods
void AgentBrain::advanceDevelopment(double timestep) {
    if (!developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

void AgentBrain::resetForEpisode() {
    // Reset brain for new episode
    if (brain_) {
        brain_->reset();
    }
    
    // Reset internal state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset agent body
    agentBody_.reset();
    agentBody_.age = 0.0f;
    
    // Reset vision history
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain reset for new episode");
}

void AgentBrain::optimizeForTask(const std::string& taskType) {
    // Apply task-specific optimizations
    if (taskType == "exploration") {
        // Enable all subsystems for exploration
        enableSubsystem(SubsystemType::All);
        NLM_LOG_INFO("Agent optimized for exploration tasks");
    } else if (taskType == "exploitation") {
        // Disable curiosity for more focused behavior
        disableSubsystem(SubsystemType::Curiosity);
        NLM_LOG_INFO("Agent optimized for exploitation tasks");
    } else if (taskType == "learning") {
        // Enable development and plasticity
        enableSubsystem(SubsystemType::Development);
        enableSubsystem(SubsystemType::StructuralPlasticity);
        NLM_LOG_INFO("Agent optimized for learning tasks");
    }
}

// Health monitoring methods
bool AgentBrain::checkSystemHealth() const {
    // Check multiple health metrics
    bool isHealthy = true;
    
    // Check brain existence
    if (!brain_) isHealthy = false;
    
    // Check dopamine balance (should be in reasonable range)
    if (std::abs(dopamineLevel_) > 1.0f) isHealthy = false;
    
    // Check plasticity factor
    if (plasticityModifier_ < 0.1f || plasticityModifier_ > 2.0f) isHealthy = false;
    
    // Check novelty decay
    if (sensoryNoveltyDecay_ < 0.5f || sensoryNoveltyDecay_ > 1.0f) isHealthy = false;
    
    // Check agent body state
    if (agentBody_.health <= 0.0f || agentBody_.energy <= 0.0f) isHealthy = false;
    
    return isHealthy;
}

std::vector<std::string> AgentBrain::getHealthIssues() const {
    std::vector<std::string> issues;
    
    // Check various health metrics
    if (!brain_) {
        issues.push_back("Brain not initialized");
    }
    
    if (std::abs(dopamineLevel_) > 1.0f) {
        issues.push_back("Dopamine imbalance: " + std::to_string(dopamineLevel_));
    }
    
    if (plasticityModifier_ < 0.1f) {
        issues.push_back("Plasticity too low: " + std::to_string(plasticityModifier_));
    } else if (plasticityModifier_ > 2.0f) {
        issues.push_back("Plasticity too high: " + std::to_string(plasticityModifier_));
    }
    
    if (sensoryNoveltyDecay_ < 0.5f) {
        issues.push_back("Novelty decay too low: " + std::to_string(sensoryNoveltyDecay_));
    }
    
    if (agentBody_.health <= 0.0f) {
        issues.push_back("Agent health depleted");
    }
    
    if (agentBody_.energy <= 0.0f) {
        issues.push_back("Agent energy depleted");
    }
    
    return issues;
}

// Serialization/Reset control methods
void AgentBrain::checkpoint() {
    NLM_LOG_INFO("AgentBrain checkpoint: Age=" + std::to_string(developmentalAge_) + 
                 ", Plasticity=" + std::to_string(plasticityModifier_) + 
                 ", Curiosity=" + std::to_string(curiosityLevel_));
}

void AgentBrain::restore() {
    // In a real implementation, this would restore from saved state
    NLM_LOG_INFO("AgentBrain restore: State restored from checkpoint");
}

// Getters
float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

float AgentBrain::getExpectedReward() const {
    return expectedReward_;
}

double AgentBrain::getDevelopmentalAge() const {
    return developmentalAge_;
}

float AgentBrain::getPlasticityModifier() const {
    return plasticityModifier_;
}

// Reset agent for new episode
void AgentBrain::reset() {
    // Reset brain for new episode
    if (brain_) {
        brain_->reset();
    }
    
    // Reset internal state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset agent body
    agentBody_.reset();
    agentBody_.age = 0.0f;
    
    // Reset vision history
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain reset for new episode");
}

MotorCommand AgentBrain::decodeMotorCommand() {
    return decodeMotorCommandRobust();
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    advanceDevelopment(timestep);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    return brain_ ? brain_->getDevelopmentalStage() : DevelopmentalStage::Initial;
}

void AgentBrain::syncAgentWithWorld(const SimpleWorld& world) {
    updateAgentFromWorld(world);
}

void AgentBrain::enableAllSubsystems() {
    enableSubsystem(SubsystemType::All);
}

void AgentBrain::disableAllSubsystems() {
    disableSubsystem(SubsystemType::All);
}

bool AgentBrain::areAllSubsystemsEnabled() const {
    return isSubsystemEnabled(SubsystemType::All);
}

void AgentBrain::emergencyShutdown() {
    disableAllSubsystems();
    if (brain_) {
        brain_->reset();
    }
    NLM_LOG_INFO("AgentBrain emergency shutdown complete");
}

void AgentBrain::activateSurvivalMode() {
    // Prioritize survival over exploration
    disableSubsystem(SubsystemType::Curiosity);
    disableSubsystem(SubsystemType::Prediction);
    enableSubsystem(SubsystemType::RewardModulation);
    NLM_LOG_INFO("AgentBrain survival mode activated");
}

void AgentBrain::activateLearningMode() {
    // Prioritize development and plasticity
    enableSubsystem(SubsystemType::Development);
    enableSubsystem(SubsystemType::StructuralPlasticity);
    enableSubsystem(SubsystemType::ConceptFormation);
    enableSubsystem(SubsystemType::Planning);
    NLM_LOG_INFO("AgentBrain learning mode activated");
}

void AgentBrain::activateExplorationMode() {
    // Maximize curiosity and exploration
    enableSubsystem(SubsystemType::All);
    enableSubsystem(SubsystemType::Curiosity);
    NLM_LOG_INFO("AgentBrain exploration mode activated");
}

void AgentBrain::activateExploitationMode() {
    // Focus on goal-directed behavior
    disableSubsystem(SubsystemType::Curiosity);
    disableSubsystem(SubsystemType::Prediction);
    enableSubsystem(SubsystemType::RewardModulation);
    NLM_LOG_INFO("AgentBrain exploitation mode activated");
}

void AgentBrain::setTaskPriority(const std::string& taskType, float priority) {
    // Store task priority for use in planning and decision making
    // This is a placeholder for task-specific optimization
    if (brain_) {
        NLM_LOG_INFO("AgentBrain task priority set: " + taskType + " = " + std::to_string(priority));
    }
}

void AgentBrain::resetTaskPriority() {
    // Reset task priorities to default
    if (brain_) {
        NLM_LOG_INFO("AgentBrain task priorities reset");
    }
}

void AgentBrain::emergencySave() {
    checkpoint();
    if (brain_) {
        NLM_LOG_INFO("AgentBrain emergency save complete");
    }
}

void AgentBrain::emergencyLoad() {
    restore();
    NLM_LOG_INFO("AgentBrain emergency load complete");
}

void AgentBrain::resetSystem() {
    resetForEpisode();
    NLM_LOG_INFO("AgentBrain complete system reset");
}

void AgentBrain::initializeDevelopmentSystem() {
    if (brain_) {
        brain_->develop();
    }
    NLM_LOG_INFO("AgentBrain development system initialized");
}

void AgentBrain::applyNeuromodulationSignal(float signalValue, NeuromodulatorType type) {
    // Apply neuromodulatory signal based on type
    switch (type) {
        case NeuromodulatorType::Dopamine:
            dopamineLevel_ = signalValue;
            break;
        case NeuromodulatorType::Serotonin:
            // Implement serotonin modulation
            break;
        case NeuromodulatorType::Acetylcholine:
            // Implement acetylcholine modulation
            break;
        case NeuromodulatorType::Noradrenaline:
            // Implement noradrenaline modulation
            break;
    }
    NLM_LOG_INFO("AgentBrain neuromodulation signal applied: " + std::to_string(signalValue));
}

void AgentBrain::configureForEnvironment(const EnvironmentType& envType) {
    // Apply environment-specific configuration
    switch (envType) {
        case EnvironmentType::Safe:
            // Enable all subsystems for safe exploration
            enableSubsystem(SubsystemType::All);
            break;
        case EnvironmentType::Dangerous:
            // Prioritize survival
            activateSurvivalMode();
            break;
        case EnvironmentType::ResourceRich:
            // Focus on resource acquisition
            disableSubsystem(SubsystemType::Curiosity);
            enableSubsystem(SubsystemType::RewardModulation);
            break;
        case EnvironmentType::ResourcePoor:
            // Maximize exploration
            activateExplorationMode();
            break;
        case EnvironmentType::Complex:
            // Enable all cognitive subsystems
            enableSubsystem(SubsystemType::Prediction);
            enableSubsystem(SubsystemType::ConceptFormation);
            enableSubsystem(SubsystemType::Planning);
            break;
    }
    NLM_LOG_INFO("AgentBrain configured for environment: " + std::to_string(static_cast<int>(envType)));
}

void AgentBrain::activateReflexArc(bool enable) {
    // Enable/disable reflex arcs (quick, automatic responses)
    if (enable) {
        NLM_LOG_INFO("AgentBrain reflex arc activated");
    } else {
        NLM_LOG_INFO("AgentBrain reflex arc deactivated");
    }
}

void AgentBrain::enablePredictiveControl(bool enable) {
    // Enable/disable predictive control
    if (enable) {
        enableSubsystem(SubsystemType::Prediction);
        NLM_LOG_INFO("AgentBrain predictive control enabled");
    } else {
        disableSubsystem(SubsystemType::Prediction);
        NLM_LOG_INFO("AgentBrain predictive control disabled");
    }
}

void AgentBrain::enableCognitiveMapping(bool enable) {
    // Enable/disable cognitive mapping (internal representation of environment)
    if (enable) {
        enableSubsystem(SubsystemType::ConceptFormation);
        NLM_LOG_INFO("AgentBrain cognitive mapping enabled");
    } else {
        disableSubsystem(SubsystemType::ConceptFormation);
        NLM_LOG_INFO("AgentBrain cognitive mapping disabled");
    }
}

void AgentBrain::enableActionPlanning(bool enable) {
    // Enable/disable action planning
    if (enable) {
        enableSubsystem(SubsystemType::Planning);
        NLM_LOG_INFO("AgentBrain action planning enabled");
    } else {
        disableSubsystem(SubsystemType::Planning);
        NLM_LOG_INFO("AgentBrain action planning disabled");
    }
}

void AgentBrain::enableWorkingMemory(bool enable) {
    // Enable/disable working memory
    if (enable) {
        NLM_LOG_INFO("AgentBrain working memory enabled");
    } else {
        NLM_LOG_INFO("AgentBrain working memory disabled");
    }
}

void AgentBrain::enableEpisodicMemory(bool enable) {
    // Enable/disable episodic memory
    if (enable) {
        NLM_LOG_INFO("AgentBrain episodic memory enabled");
    } else {
        NLM_LOG_INFO("AgentBrain episodic memory disabled");
    }
}

void AgentBrain::enableAssociativeMemory(bool enable) {
    // Enable/disable associative memory
    if (enable) {
        NLM_LOG_INFO("AgentBrain associative memory enabled");
    } else {
        NLM_LOG_INFO("AgentBrain associative memory disabled");
    }
}

void AgentBrain::enableRewardLearning(bool enable) {
    // Enable/disable reward learning
    if (enable) {
        enableSubsystem(SubsystemType::RewardModulation);
        NLM_LOG_INFO("AgentBrain reward learning enabled");
    } else {
        disableSubsystem(SubsystemType::RewardModulation);
        NLM_LOG_INFO("AgentBrain reward learning disabled");
    }
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    // Enable/disable structural plasticity
    if (enable) {
        enableSubsystem(SubsystemType::StructuralPlasticity);
        NLM_LOG_INFO("AgentBrain structural plasticity enabled");
    } else {
        disableSubsystem(SubsystemType::StructuralPlasticity);
        NLM_LOG_INFO("AgentBrain structural plasticity disabled");
    }
}

void AgentBrain::enableDevelopment(bool enable) {
    // Enable/disable development
    if (enable) {
        enableSubsystem(SubsystemType::Development);
        NLM_LOG_INFO("AgentBrain development enabled");
    } else {
        disableSubsystem(SubsystemType::Development);
        NLM_LOG_INFO("AgentBrain development disabled");
    }
}

void AgentBrain::enableCuriosity(bool enable) {
    // Enable/disable curiosity
    if (enable) {
        enableSubsystem(SubsystemType::Curiosity);
        NLM_LOG_INFO("AgentBrain curiosity enabled");
    } else {
        disableSubsystem(SubsystemType::Curiosity);
        NLM_LOG_INFO("AgentBrain curiosity disabled");
    }
}

void AgentBrain::enablePrediction(bool enable) {
    // Enable/disable prediction
    if (enable) {
        enableSubsystem(SubsystemType::Prediction);
        NLM_LOG_INFO("AgentBrain prediction enabled");
    } else {
        disableSubsystem(SubsystemType::Prediction);
        NLM_LOG_INFO("AgentBrain prediction disabled");
    }
}

void AgentBrain::enableConceptFormation(bool enable) {
    // Enable/disable concept formation
    if (enable) {
        enableSubsystem(SubsystemType::ConceptFormation);
        NLM_LOG_INFO("AgentBrain concept formation enabled");
    } else {
        disableSubsystem(SubsystemType::ConceptFormation);
        NLM_LOG_INFO("AgentBrain concept formation disabled");
    }
}

void AgentBrain::enablePlanning(bool enable) {
    // Enable/disable planning
    if (enable) {
        enableSubsystem(SubsystemType::Planning);
        NLM_LOG_INFO("AgentBrain planning enabled");
    } else {
        disableSubsystem(SubsystemType::Planning);
        NLM_LOG_INFO("AgentBrain planning disabled");
    }
}

bool AgentBrain::isRewardModulationEnabled() const {
    return rewardModulationEnabled_;
}

bool AgentBrain::isStructuralPlasticityEnabled() const {
    return structuralPlasticityEnabled_;
}

bool AgentBrain::isDevelopmentEnabled() const {
    return developmentEnabled_;
}

bool AgentBrain::isCuriosityEnabled() const {
    return curiosityEnabled_;
}

bool AgentBrain::isPredictionEnabled() const {
    return predictionEnabled_;
}

bool AgentBrain::isConceptFormationEnabled() const {
    return conceptFormationEnabled_;
}

bool AgentBrain::isPlanningEnabled() const {
    return planningEnabled_;
}

// Implementation of pure virtual methods from interface
void AgentBrain::reset() {
    resetForEpisode();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    return decodeMotorCommandRobust();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Get sensory patterns from world
    auto& brain = *brain_;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    std::vector<float> visionPattern(vision.begin(), vision.end());
    visionPattern.resize(256, 0.0f); // Normalize to 256
    
    // INTEGRATION FIX: WorkingMemory integration
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Store full sensory pattern for short-term processing
        std::vector<float> sensoryPattern = visionPattern;
        const auto& touch = percept.getTouch();
        const auto& internal = percept.getInternal();
        const auto& proprio = percept.getProprioception();
        
        sensoryPattern.insert(sensoryPattern.end(), touch.begin(), touch.end());
        sensoryPattern.insert(sensoryPattern.end(), internal.begin(), internal.end());
        sensoryPattern.insert(sensoryPattern.end(), proprio.begin(), proprio.end());
        
        // Convert to normalized features for working memory
        for (size_t i = 0; i < sensoryPattern.size(); ++i) {
            float normalized = (sensoryPattern[i] + 1.0f) * 0.5f;  // [0, 1] range
            workingMemory->storeToNeuron(10000 + i, normalized);  // Store with offset IDs
        }
    }
    
    // Create neural pattern from vision for concept formation
    // This represents the raw sensory input that the brain will process
    std::vector<float> sensoryPattern = visionPattern;
    
    // Add other sensory modalities to pattern
    const auto& touch = percept.getTouch();
    std::copy(touch.begin(), touch.end(), std::back_inserter(sensoryPattern));
    
    const auto& internal = percept.getInternal();
    std::copy(internal.begin(), internal.end(), std::back_inserter(sensoryPattern));
    
    const auto& proprio = percept.getProprioception();
    std::copy(proprio.begin(), proprio.end(), std::back_inserter(sensoryPattern));
    
    // Process through neural computation
    brain.receiveSensoryInput(SensoryInput(visionPattern));
    
    // Inject into sensory neurons as before
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Extract features for concept formation (every 4th value)
    std::vector<float> features;
    features.reserve(sensoryPattern.size() / 4);
    for (size_t i = 0; i < sensoryPattern.size(); i += 4) {
        if (i < sensoryPattern.size()) {
            features.push_back(sensoryPattern[i]);
        }
    }
    
    // NEW: Send to concept formation for pattern discovery
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation && conceptFormation->getConceptCount() > 0) {
        conceptFormation->presentExperience(sensoryPattern, features, predictionError_, brain.getTotalSpikeCount());
    }
    
    // NEW: Send to prediction system for training
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        predictionSystem->train(SensoryInput(visionPattern));
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
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
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

} // namespace nlm
