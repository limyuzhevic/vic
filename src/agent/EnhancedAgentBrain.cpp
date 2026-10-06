#include "EnhancedAgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , cognitiveConfidence_(0.0f)
    , conceptStability_(0.0f)
    , attentionFocusLevel_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , planningConfidence_(0.0f)
    , attentionWeight_(0.5f)
    , conceptUpdateThreshold_(0.3f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , cognitivePlanningEnabled_(true)
    , conceptBasedBehaviorEnabled_(true)
    , attentionGuidanceEnabled_(true)
    , predictionPlanningEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , predictionAccuracy_(0.0f)
    , topDownBiasStrength_(0.0f)
    , predictionSystemInitialized_(false)
    , memorySystemInitialized_(false)
    , attentionSystemInitialized_(false)
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
    
    // Initialize cognitive systems
    initializeCognitiveSystems();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeCognitiveSystems() {
    if (!brain_) return;
    
    // Initialize NeuralPlanner
    if (brain_->getPlanner()) {
        planner_ = std::make_unique<NeuralPlanner>();
        planner_->initialize(brain_.get());
        planner_->setPlanningDepth(3);  // Look ahead 3 steps
        NLM_LOG_INFO("AgentBrain: NeuralPlanner initialized");
    }
    
    // Initialize ConceptFormation
    if (brain_->getConceptFormation()) {
        conceptFormation_ = std::make_unique<ConceptFormation>();
        conceptFormation_->initialize(brain_.get());
        NLM_LOG_INFO("AgentBrain: ConceptFormation initialized");
    }
    
    // Initialize NeuralWorkingMemory
    if (brain_->getWorkingMemory()) {
        workingMemory_ = std::make_unique<NeuralWorkingMemory>();
        workingMemory_->initialize(brain_.get());
        NLM_LOG_INFO("AgentBrain: NeuralWorkingMemory initialized");
    }
    
    // Initialize AttentionalSelection
    if (brain_->getAttention()) {
        attention_ = std::make_unique<AttentionalSelection>();
        attention_->initialize(brain_.get());
        NLM_LOG_INFO("AgentBrain: AttentionalSelection initialized");
    }
    
    predictionSystemInitialized_ = true;
    memorySystemInitialized_ = true;
    attentionSystemInitialized_ = true;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset neural state history
    previousNeuralState_.clear();
    predictedState_.clear();
    
    // Initialize cognitive system state
    planningConfidence_ = 0.0f;
    attentionFocusLevel_ = 0.0f;
    conceptStability_ = 0.0f;
    predictionAccuracy_ = 0.0f;
    
    NLM_LOG_INFO("Enhanced AgentBrain initialized with cognitive systems");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Enhanced sensory input size with cognitive features
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Enhanced motor output with cognitive action types
    // Include cognitive actions beyond basic motor commands
    return 10; // Basic 6 + 4 cognitive actions
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
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
    
    // Update curiosity based on novelty and cognitive factors
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        
        // Add cognitive curiosity (exploration based on concept uncertainty)
        if (conceptFormation_) {
            curiosityLevel_ += (1.0f - conceptStability_) * 0.3f;
        }
        
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update cognitive systems with new sensory input
    updateCognitiveSystemsWithInput(extractNeuralState());
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Enhanced motor command selection with cognitive integration
    return decodeCognitiveAction();
}

MotorCommand AgentBrain::decodeCognitiveAction() {
    if (!brain_ || !cognitivePlanningEnabled_) {
        return decodeFromMotorNeurons();
    }
    
    // Extract current neural state for cognitive evaluation
    std::vector<float> neuralState = extractNeuralState();
    
    // Evaluate with cognitive systems
    MotorCommand cognitiveAction = evaluateWithCognitiveSystems(neuralState);
    
    // If cognitive system provides a valid action, use it
    if (cognitiveAction != MotorCommand::Wait || 
        (cognitivePlanningEnabled_ && planningConfidence_ > 0.5f)) {
        return cognitiveAction;
    }
    
    // Fall back to traditional motor decoding
    return decodeFromMotorNeurons();
}

MotorCommand AgentBrain::decodeConceptBasedAction(const std::vector<float>& sensoryState) {
    if (!brain_ || !conceptBasedBehaviorEnabled_ || !conceptFormation_) {
        return MotorCommand::Wait;
    }
    
    // Get matching concept for current sensory state
    size_t conceptId = conceptFormation_->getMatchingConcept(sensoryState);
    
    if (conceptId == 0) {
        // No established concept, use traditional motor control
        return MotorCommand::Wait;
    }
    
    // Get concept prototype
    std::vector<float> prototype = conceptFormation_->getConceptPrototype(conceptId);
    
    // Determine behavior based on concept
    MotorCommand conceptAction = MotorCommand::Wait;
    
    // Simple concept-to-action mapping (could be learned)
    if (conceptId == 1) {  // Exploration concept
        conceptAction = selectWithCuriosity(MotorCommand::Wait);
    } else if (conceptId == 2) {  // Social interaction concept
        conceptAction = MotorCommand::Interact;
    } else if (conceptId == 3) {  // Goal-directed concept
        conceptAction = MotorCommand::MoveForward;
    }
    
    // Update concept behavior weight
    conceptBehaviorWeights_[conceptId] += 0.1f;
    conceptBehaviorWeights_[conceptId] = std::min(conceptBehaviorWeights_[conceptId], 1.0f);
    
    activeConcepts_.push_back(conceptId);
    
    return conceptAction;
}

MotorCommand AgentBrain::decodeAttentionGuidedAction() {
    if (!brain_ || !attentionGuidanceEnabled_ || !attention_) {
        return MotorCommand::Wait;
    }
    
    // Get currently attended regions
    std::vector<RegionId> attended = attention_->getAttendedRegions();
    
    if (attended.empty()) {
        return MotorCommand::Wait;
    }
    
    // Calculate attention focus level based on winner neurons
    const auto& winners = attention_->getWinners();
    float totalAttention = 0.0f;
    
    for (NeuronId winner : winners) {
        // Get motor action associated with attended region
        // Simplified mapping
        if (winner < motorForward_.size()) {
            totalAttention += 0.8f; // High attention for forward movement
        } else if (winner < motorBackward_.size() + motorForward_.size()) {
            totalAttention += 0.6f; // Medium attention for backward
        } else if (winner < motorTurnLeft_.size() + motorBackward_.size() + motorForward_.size()) {
            totalAttention += 0.7f; // Medium-high attention for turning left
        }
    }
    
    attentionFocusLevel_ = std::min(totalAttention, 1.0f);
    
    // Use attended regions to bias action selection
    if (attentionFocusLevel_ > 0.3f) {
        // Focus on the most attended action
        // Simplified: choose forward if any attention
        return MotorCommand::MoveForward;
    }
    
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::decodePredictionBasedAction(const std::vector<float>& currentState) {
    if (!brain_ || !predictionPlanningEnabled_ || !planner_ || !workingMemory_) {
        return MotorCommand::Wait;
    }
    
    // Use prediction system for forward planning
    MotorCommand predictedAction = MotorCommand::Wait;
    float maxPredictedReward = -1.0f;
    
    // Try to get planned action from NeuralPlanner
    ActionType plannedAction = planner_->planAction(currentState, 0.5f);
    
    // Convert ActionType to MotorCommand
    switch (plannedAction) {
        case ActionType::Forward: predictedAction = MotorCommand::MoveForward; break;
        case ActionType::Backward: predictedAction = MotorCommand::MoveBackward; break;
        case ActionType::Left: predictedAction = MotorCommand::TurnLeft; break;
        case ActionType::Right: predictedAction = MotorCommand::TurnRight; break;
        case ActionType::Interact: predictedAction = MotorCommand::Interact; break;
        case ActionType::Wait: predictedAction = MotorCommand::Wait; break;
        case ActionType::LookLeft: predictedAction = MotorCommand::LookLeft; break;
        case ActionType::LookRight: predictedAction = MotorCommand::LookRight; break;
        default: predictedAction = MotorCommand::Wait; break;
    }
    
    // Evaluate prediction quality
    if (planner_->wasRecentPlanSuccessful()) {
        predictionAccuracy_ = 1.0f;
    } else {
        predictionAccuracy_ = 0.5f; // Conservative estimate
    }
    
    return predictedAction;
}

MotorCommand AgentBrain::evaluateWithCognitiveSystems(const std::vector<float>& neuralState) {
    MotorCommand bestAction = MotorCommand::Wait;
    float bestScore = -1.0f;
    
    // Try each cognitive action selection method
    if (cognitivePlanningEnabled_) {
        MotorCommand plannedAction = decodePredictionBasedAction(neuralState);
        float plannedScore = 0.0f;
        
        if (planner_ && planner_->wasRecentPlanSuccessful()) {
            plannedScore = 0.8f;
        }
        
        if (plannedScore > bestScore) {
            bestScore = plannedScore;
            bestAction = plannedAction;
        }
    }
    
    if (conceptBasedBehaviorEnabled_) {
        MotorCommand conceptAction = decodeConceptBasedAction(neuralState);
        float conceptScore = 0.0f;
        
        // Get concept stability for scoring
        if (conceptFormation_ && !activeConcepts_.empty()) {
            for (size_t conceptId : activeConcepts_) {
                conceptScore += conceptFormation_->getConceptStability(conceptId);
            }
            conceptScore /= activeConcepts_.size();
        }
        
        if (conceptScore > bestScore) {
            bestScore = conceptScore;
            bestAction = conceptAction;
        }
    }
    
    if (attentionGuidanceEnabled_) {
        MotorCommand attentionAction = decodeAttentionGuidedAction();
        float attentionScore = attentionFocusLevel_ * 0.7f;
        
        if (attentionScore > bestScore) {
            bestScore = attentionScore;
            bestAction = attentionAction;
        }
    }
    
    // Traditional motor decoding as fallback
    MotorCommand motorAction = decodeFromMotorNeurons();
    float motorScore = 0.0f;
    
    // Calculate activity level for motor action
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
    
    // Map motor activity to score
    if (motorAction == MotorCommand::MoveForward) motorScore = forwardAct;
    else if (motorAction == MotorCommand::MoveBackward) motorScore = backwardAct;
    else if (motorAction == MotorCommand::TurnLeft) motorScore = leftAct;
    else if (motorAction == MotorCommand::TurnRight) motorScore = rightAct;
    else if (motorAction == MotorCommand::Interact) motorScore = interactAct;
    else motorScore = waitAct;
    
    if (motorScore > bestScore) {
        bestScore = motorScore;
        bestAction = motorAction;
    }
    
    cognitiveConfidence_ = bestScore;
    return bestAction;
}

std::vector<float> AgentBrain::extractNeuralState() const {
    std::vector<float> neuralState;
    
    // Extract motor neuron activities
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    neuralState.push_back(calcActivity(motorForward_));
    neuralState.push_back(calcActivity(motorBackward_));
    neuralState.push_back(calcActivity(motorTurnLeft_));
    neuralState.push_back(calcActivity(motorTurnRight_));
    neuralState.push_back(calcActivity(motorInteract_));
    neuralState.push_back(calcActivity(motorWait_));
    
    // Extract sensory neuron activities
    neuralState.push_back(calcActivity(sensoryVision_));
    neuralState.push_back(calcActivity(sensoryTouch_));
    neuralState.push_back(calcActivity(sensoryInternal_));
    neuralState.push_back(calcActivity(sensoryProprioception_));
    
    // Add neuromodulatory state
    neuralState.push_back(dopamineLevel_);
    neuralState.push_back(noveltyLevel_);
    neuralState.push_back(curiosityLevel_);
    neuralState.push_back(predictionError_);
    
    // Add cognitive system states
    neuralState.push_back(planningConfidence_);
    neuralState.push_back(attentionFocusLevel_);
    neuralState.push_back(conceptStability_);
    neuralState.push_back(predictionAccuracy_);
    
    // Store for next time
    previousNeuralState_ = neuralState;
    
    return neuralState;
}

void AgentBrain::updateCognitiveSystemsWithInput(const std::vector<float>& neuralState) {
    if (!brain_) return;
    
    // Update NeuralWorkingMemory with current neural state
    if (workingMemory_) {
        workingMemory_->store(neuralState, 1.0f);
    }
    
    // Update NeuralPlanner with current state and goal
    if (planner_) {
        // Simple goal setting based on current state
        std::vector<float> goal;
        if (!neuralState.empty()) {
            goal = neuralState;
        }
        
        if (!goal.empty()) {
            planner_->setCurrentGoal(goal);
        }
        
        // Update planning confidence based on working memory strength
        if (workingMemory_) {
            planningConfidence_ = workingMemory_->getMemoryActivity();
        }
    }
    
    // Update ConceptFormation with neural state
    if (conceptFormation_) {
        // Convert neural state to pattern for concept formation
        std::vector<float> pattern = neuralState;
        float reward = dopamineLevel_ > 0 ? 0.5f : 0.0f;
        
        conceptFormation_->presentExperience(pattern, pattern, reward, 0);
        
        // Update concept stability
        if (!activeConcepts_.empty()) {
            float totalStability = 0.0f;
            for (size_t conceptId : activeConcepts_) {
                totalStability += conceptFormation_->getConceptStability(conceptId);
            }
            conceptStability_ = totalStability / activeConcepts_.size();
        }
    }
    
    // Update AttentionalSelection with neural state
    if (attention_) {
        // Get motor neurons for attention competition
        std::vector<NeuronId> motorNeurons;
        size_t neuronId = 0;
        
        // Simplified: collect all motor neuron IDs
        for (size_t i = 0; i < motorForward_.size(); ++i) {
            motorNeurons.push_back(neuronId++);
        }
        for (size_t i = 0; i < motorBackward_.size(); ++i) {
            motorNeurons.push_back(neuronId++);
        }
        for (size_t i = 0; i < motorTurnLeft_.size(); ++i) {
            motorNeurons.push_back(neuronId++);
        }
        for (size_t i = 0; i < motorTurnRight_.size(); ++i) {
            motorNeurons.push_back(neuronId++);
        }
        
        // Process competition
        if (!motorNeurons.empty()) {
            attention_->processCompetition(motorNeurons, 0.5f);
            
            // Calculate attention focus
            const auto& winners = attention_->getWinners();
            attentionFocusLevel_ = winners.size() > 0 ? 1.0f : 0.0f;
        }
    }
}

void AgentBrain::updateCognitiveCuriosity() {
    if (!brain_ || !curiosityEnabled_) return;
    
    // Update curiosity based on concept novelty
    if (conceptFormation_) {
        float novelty = 0.0f;
        size_t conceptCount = conceptFormation_->getConceptCount();
        
        if (conceptCount > 0) {
            // Check for new concepts
            novelty = 1.0f / (conceptCount + 1);
        }
        
        curiosityLevel_ += novelty * 0.2f;
    }
    
    // Update curiosity based on prediction error
    if (predictionPlanningEnabled_) {
        float predictionCuriosity = std::abs(predictionError_) * 0.3f;
        curiosityLevel_ += predictionCuriosity;
    }
    
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void AgentBrain::integrateWithPredictionSystem() {
    if (!brain_ || !predictionPlanningEnabled_ || !planner_ || !workingMemory_) return;
    
    // Store current state in working memory
    std::vector<float> currentState = extractNeuralState();
    workingMemory_->store(currentState, 1.0f);
    
    // Use prediction system for forward planning
    if (!currentState.empty()) {
        ActionType plannedAction = planner_->planAction(currentState, 0.5f);
        
        // Store prediction in working memory
        workingMemory_->storePrediction(0, currentState, currentState, 0.0f);
    }
}

void AgentBrain::integrateWithMemorySystem() {
    if (!brain_ || !memorySystemInitialized_ || !workingMemory_) return;
    
    // Integrate working memory with cognitive systems
    float memoryActivity = workingMemory_->getMemoryActivity();
    
    // Update cognitive confidence based on memory activity
    planningConfidence_ = memoryActivity * 0.7f + planningConfidence_ * 0.3f;
    
    // Use memory to bias concept formation
    if (conceptFormation_ && memoryActivity > 0.5f) {
        conceptFormation_->setFormationThreshold(0.6f);
    }
}

void AgentBrain::integrateWithAttentionSystem() {
    if (!brain_ || !attentionSystemInitialized_ || !attention_) return;
    
    // Apply top-down attention bias based on current goals
    if (planner_ && planner_->getCurrentGoal().size() > 0) {
        // Simple bias toward forward movement when goal is set
        attention_->applyTopDownBias(0, 0.5f);
    }
    
    // Update attention focus based on cognitive state
    float cognitiveArousal = dopamineLevel_ + curiosityLevel_;
    attention_->setInhibitionStrength(0.5f - cognitiveArousal * 0.2f);
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
    // Enhanced exploration: occasionally choose random action when curiosity is high
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
    
    // Update cognitive confidence based on prediction accuracy
    if (predictionError_ < 0.1f) {
        predictionAccuracy_ = 1.0f - std::abs(predictionError_);
    }
    
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
    
    // Update cognitive systems with reward signal
    updateCognitiveCuriosity();
    integrateWithPredictionSystem();
    integrateWithMemorySystem();
    integrateWithAttentionSystem();
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
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

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

float AgentBrain::getCognitiveConfidence() const {
    return cognitiveConfidence_;
}

float AgentBrain::getConceptStability() const {
    return conceptStability_;
}

float AgentBrain::getAttentionFocusLevel() const {
    return attentionFocusLevel_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    cognitiveConfidence_ = 0.0f;
    conceptStability_ = 0.0f;
    attentionFocusLevel_ = 0.0f;
    predictionAccuracy_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    planningConfidence_ = 0.0f;
    topDownBiasStrength_ = 0.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear cognitive state
    previousNeuralState_.clear();
    predictedState_.clear();
    activeConcepts_.clear();
    conceptBehaviorWeights_.clear();
}

} // namespace nlm