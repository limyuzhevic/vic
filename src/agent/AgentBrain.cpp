#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , acetylcholineLevel_(0.0f)
    , norepinephrineLevel_(0.0f)
    , serotoninLevel_(0.0f)
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
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        // Get all neuromodulators
        auto* dopamine = brain_->getDopamine();
        auto* acetylcholine = brain_->getAcetylcholine();
        auto* norepinephrine = brain_->getNorepinephrine();
        auto* serotonin = brain_->getSerotonin();
        
        // Store initial neuromodulator levels for monitoring
        dopamineLevel_ = dopamine ? dopamine->getLevel() : 0.0f;
        acetylcholineLevel_ = acetylcholine ? acetylcholine->getLevel() : 0.0f;
        norepinephrineLevel_ = norepinephrine ? norepinephrine->getLevel() : 0.0f;
        serotoninLevel_ = serotonin ? serotonin->getLevel() : 0.0f;
        
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
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // ===== NEURAL PLANNER INTEGRATION =====
    // Use neural planner for cognitive processing when available
    if (brain_->getPlanner() && curiosityEnabled_) {
        // Create state representation for neural planner
        std::vector<float> planningState;
        planningState.reserve(64);
        
        // Add vision to planning state
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < std::min(vision.size(), static_cast<size_t>(64)); ++i) {
            planningState.push_back(vision[i]);
        }
        
        // Pad state to desired size
        while (planningState.size() < 64) {
            planningState.push_back(0.0f);
        }
        
        // Use neural planner to plan action based on current state
        float targetReward = 0.5f;  // Could be goal-based or learned
        ActionType plannedAction = brain_->getPlanner()->planAction(planningState, targetReward);
        
        // Integrate planned action with motor neuron bias
        // This connects planning to action execution
        for (auto* neuron : motorForward_) {
            if (plannedAction == ActionType::MoveForward) {
                neuron->injectCurrent(1.5f);  // Boost forward movement
            }
        }
        for (auto* neuron : motorBackward_) {
            if (plannedAction == ActionType::MoveBackward) {
                neuron->injectCurrent(1.5f);  // Boost backward movement
            }
        }
        for (auto* neuron : motorTurnLeft_) {
            if (plannedAction == ActionType::TurnLeft) {
                neuron->injectCurrent(1.5f);  // Boost left turn
            }
        }
        for (auto* neuron : motorTurnRight_) {
            if (plannedAction == ActionType::TurnRight) {
                neuron->injectCurrent(1.5f);  // Boost right turn
            }
        }
        for (auto* neuron : motorInteract_) {
            if (plannedAction == ActionType::Interact) {
                neuron->injectCurrent(1.5f);  // Boost interaction
            }
        }
        
        // Update concept formation with current experience
        if (brain_->getConceptFormation()) {
            brain_->getConceptFormation()->presentExperience(
                planningState,
                vision,  // Use vision as features for now
                curiosityLevel_,
                currentStep);  // Would need actual step tracking
        }
        
        // Update self-model with actual consequences
        if (brain_->getSelfModel() && plannedAction != ActionType::Wait) {
            // Record that we took this action
            brain_->getSelfModel()->recordSelfAction(
                plannedAction,
                planningState,  // Would be before state
                vision);  // Would be after state
        }
    }
    
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
    
    // Store sensory patterns in working memory
    if (brain_->getWorkingMemory()) {
        // Create composite sensory pattern
        std::vector<float> sensoryPattern;
        
        // Add vision pattern (256 values)
        sensoryPattern.reserve(256 + 8 + 4 + 6);
        for (size_t i = 0; i < std::min(vision.size(), static_cast<size_t>(256)); ++i) {
            sensoryPattern.push_back(vision[i]);
        }
        
        // Add touch pattern (8 values)
        for (size_t i = 0; i < std::min(touch.size(), static_cast<size_t>(8)); ++i) {
            sensoryPattern.push_back(touch[i]);
        }
        
        // Add internal pattern (4 values)
        for (size_t i = 0; i < std::min(intern.size(), static_cast<size_t>(4)); ++i) {
            sensoryPattern.push_back((intern[i] * 2.0f - 1.0f));
        }
        
        // Add proprioception pattern (6 values)
        for (size_t i = 0; i < std::min(proprio.size(), static_cast<size_t>(6)); ++i) {
            sensoryPattern.push_back((proprio[i] * 2.0f - 1.0f));
        }
        
        // Store in working memory
        if (!sensoryPattern.empty()) {
            brain_->getWorkingMemory()->store(sensoryPattern, 1.0f);
        }
        
        // Trigger working memory replay
        brain_->getWorkingMemory()->updateRecurrentConnections();
    }
    
    // ===== PREDICTION SYSTEM INTEGRATION =====
    // Use prediction system for curiosity-driven exploration
    if (brain_->getPredictionSystem()) {
        // Create SensoryInput from current percept for prediction system
        auto sensoryInput = std::make_unique<Vision>(16, 16);
        
        // Fill with vision data
        for (size_t i = 0; i < std::min(vision.size(), static_cast<size_t>(256)); ++i) {
            sensoryInput->setDataAt(i, vision[i]);
        }
        
        // Get prediction and prediction error
        auto predicted = brain_->getPredictionSystem()->predictNextState(*sensoryInput);
        brain_->getPredictionSystem()->updatePredictions(*predicted, *sensoryInput);
        
        float predictionError = brain_->getPredictionSystem()->getPredictionError();
        float predictionConfidence = brain_->getPredictionSystem()->getConfidence();
        
        // Update AgentBrain's prediction error state
        predictionError_ = predictionError;
        
        // Use prediction to modulate curiosity
        if (curiosityEnabled_ && curiosityLevel_ > 0.0f) {
            // High prediction error = more curiosity = more exploration
            float errorMultiplier = 1.0f + std::abs(predictionError) * 0.5f;
            curiosityLevel_ *= errorMultiplier;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
            
            // Use prediction to guide exploration
            if (predictionError > 0.5f && predictionConfidence < 0.5f) {
                // Unpredictable state = explore more
                curiosityLevel_ = std::min(1.0f, curiosityLevel_ * 1.5f);
            }
        }
        
        // Integrate predicted state with working memory for planning
        if (brain_->getWorkingMemory() && predictionConfidence > 0.7f) {
            // High confidence predictions can guide planning
            std::vector<float> predictedPattern;
            for (size_t i = 0; i < std::min(predicted->getData().size(), static_cast<size_t>(256)); ++i) {
                predictedPattern.push_back(predicted->getDataAt(i));
            }
            
            if (!predictedPattern.empty()) {
                brain_->getWorkingMemory()->store(predictedPattern, predictionConfidence);
            }
        }
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}
}

void AgentBrain::decodeMotorCommand() {
    if (!brain_) return;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
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
    
    // Get all neuromodulators from brain
    auto* dopamine = brain_->getDopamine();
    auto* acetylcholine = brain_->getAcetylcholine();
    auto* norepinephrine = brain_->getNorepinephrine();
    auto* serotonin = brain_->getSerotonin();
    
    // Update all neuromodulator levels
    acetylcholineLevel_ = acetylcholine ? acetylcholine->getLevel() : 0.0f;
    norepinephrineLevel_ = norepinephrine ? norepinephrine->getLevel() : 0.0f;
    serotoninLevel_ = serotonin ? serotonin->getLevel() : 0.0f;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    acetylcholineLevel_ = std::clamp(acetylcholineLevel_, 0.0f, 1.0f);
    norepinephrineLevel_ = std::clamp(norepinephrineLevel_, 0.0f, 1.0f);
    serotoninLevel_ = std::clamp(serotoninLevel_, 0.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with neuromodulation
                // Integrate all neurotransmitters for comprehensive plasticity control
                float totalModulation = dopamineLevel_ * 0.4f + // Primary reward signal
                                       acetylcholineLevel_ * 0.3f + // Memory encoding
                                       norepinephrineLevel_ * 0.2f + // Arousal response
                                       serotoninLevel_ * 0.1f; // Mood modulation
                
                float delta = eligibility * totalModulation * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on all neuromodulators
    // Positive neuromodulation increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;  // Base dopamine modulation
    plasticityFactor += acetylcholineLevel_ * 0.2f;        // ACh enhances memory-based learning
    plasticityFactor += norepinephrineLevel_ * 0.15f;     // NE modulates arousal-driven learning
    plasticityFactor += serotoninLevel_ * 0.1f;            // 5-HT modulates mood-based learning
    
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
    // Return integrated neuromodulation level from all neurotransmitters
    // Higher overall neuromodulation indicates more learning and attention
    float totalNeuromodulation = dopamineLevel_ * 0.4f + // Primary reward and motivation
                                acetylcholineLevel_ * 0.3f + // Attention and memory
                                norepinephrineLevel_ * 0.2f + // Arousal and vigilance
                                serotoninLevel_ * 0.1f; // Mood and social behavior
    return totalNeuromodulation;
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

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    acetylcholineLevel_ = 0.0f;
    norepinephrineLevel_ = 0.0f;
    serotoninLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
