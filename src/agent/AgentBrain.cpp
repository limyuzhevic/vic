#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../motor/Action.hpp"
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
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    // Initialize NeuralPlanner if brain is available
    if (brain_) {
        brain_->getPlanner()->initialize(brain_.get());
        brain_->getPlanner()->setPlanningDepth(5);
        NLM_LOG_INFO("NeuralPlanner integrated into AgentBrain");
    }
}

void AgentBrain::processWithConcepts() {
    if (!brain_ || !getConceptFormation()) return;
    
    // Get current working memory content as activity pattern
    // Convert working memory traces to pattern vector for concept formation
    std::vector<float> currentPattern;
    std::vector<float> features;
    
    // Get working memory content
    std::vector<float> wmContent = brain_->getWorkingMemory()->retrieve();
    
    // Convert working memory to pattern vector
    // Add sensory neuron activations to pattern
    for (Neuron* n : sensoryVision_) {
        float activity = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        currentPattern.push_back(activity);
    }
    
    for (Neuron* n : sensoryTouch_) {
        float activity = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        currentPattern.push_back(activity);
    }
    
    for (Neuron* n : sensoryInternal_) {
        float activity = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        currentPattern.push_back(activity);
    }
    
    for (Neuron* n : sensoryProprioception_) {
        float activity = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        currentPattern.push_back(activity);
    }
    
    // Add working memory content
    currentPattern.insert(currentPattern.end(), wmContent.begin(), wmContent.end());
    
    // Extract features from neuromodulatory signals and developmental state
    features = {
        curiosityLevel_,
        dopamineLevel_,
        predictionError_,
        developmentalAge_,
        noveltyLevel_
    };
    
    // Normalize pattern for concept formation
    float totalActivity = 0.0f;
    for (float val : currentPattern) {
        totalActivity += val * val;
    }
    float norm = std::sqrt(totalActivity);
    if (norm > 0.0f) {
        for (float& val : currentPattern) {
            val = val / norm;
        }
    }
    
    // Present this experience to concept formation system
    size_t conceptId = getConceptFormation()->presentExperience(
        currentPattern,
        features,
        dopamineLevel_, // Use dopamine as reward signal
        0 // Current step
    );
    
    if (conceptId > 0) {
        const DiscoveredConcept* concept = getConceptFormation()->getConcept(conceptId);
        if (concept) {
            NLM_LOG_INFO("ConceptFormation: Pattern matches concept " + 
                         std::to_string(conceptId) + " (category: " + concept->categoryHint + ")");
            
            // Use concept for action selection bias
            applyConceptInfluence(concept);
        }
    }
}

void AgentBrain::applyConceptInfluence(const DiscoveredConcept* concept) {
    if (!concept) return;
    
    // For now, simple concept-based modulation
    // Higher stability concepts have more influence
    float stability = concept->avgStability;
    if (stability > 0.7f) {
        // Boost curiosity when stable concepts are present
        curiosityLevel_ = std::min(1.0f, curiosityLevel_ * 1.1f);
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
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
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

void AgentBrain::integratePlanner(const std::vector<float>& state) {
    if (!brain_ || !brain_->getPlanner()) return;
    
    // Get the planner
    NeuralPlanner* planner = brain_->getPlanner();
    
    // Set current state for planning
    // TODO: Need to get target goals from somewhere (could be from episodic memory or world state)
    std::vector<float> targetGoal = state;
    if (!targetGoal.empty()) {
        planner->setCurrentGoal(targetGoal);
    }
    
    // Plan an action
    ActionType plannedAction = planner->planAction(state);
    
    // Store the planned action for use in motor decoding
    // This could be stored in a temporary buffer or directly used
    // For now, we just log it
    NLM_LOG_INFO("NeuralPlanner selected action: " + std::to_string(static_cast<int>(plannedAction)) + 
                 " with confidence: " + std::to_string(planner->getPlanningConfidence()));
    
    // Update action quality based on planner feedback
    // TODO: Integrate with episodic memory if available
    
    // Check if plan was successful
    if (planner->wasRecentPlanSuccessful()) {
        NLM_LOG_INFO("NeuralPlanner plan was successful");
    } else {
        NLM_LOG_INFO("NeuralPlanner plan failed - increasing curiosity");
        // Increase curiosity to encourage exploration
        curiosityLevel_ = std::min(1.0f, curiosityLevel_ * 1.2f);
    }
}

float AgentBrain::getPlanningConfidence() const {
    if (!brain_ || !brain_->getPlanner()) return 0.0f;
    return brain_->getPlanner()->getPlanningConfidence();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Integrate NeuralPlanner for action selection
    if (brain_->getPlanner() && getPlanningConfidence() > 0.3f) {
        // Get current state for planning (simplified - use current sensory input)
        std::vector<float> currentState = {
            dopamineLevel_,
            curiosityLevel_,
            noveltyLevel_,
            predictionError_,
            developmentalAge_
        };
        
        // Plan actions
        integratePlanner(currentState);
        
        // If planner has high confidence and selected action, use it
        if (getPlanningConfidence() > 0.7f) {
            // Get planned action from planner
            ActionType plannedAction = brain_->getPlanner()->planAction(currentState);
            
            // Convert planned action to motor command
            switch (plannedAction) {
                case ActionType::MoveForward: return MotorCommand::MoveForward;
                case ActionType::MoveBackward: return MotorCommand::MoveBackward;
                case ActionType::TurnLeft: return MotorCommand::TurnLeft;
                case ActionType::TurnRight: return MotorCommand::TurnRight;
                case ActionType::Interact: return MotorCommand::Interact;
                case ActionType::Wait: return MotorCommand::Wait;
                case ActionType::LookLeft: return MotorCommand::LookLeft;
                case ActionType::LookRight: return MotorCommand::LookRight;
                default: break;
            }
        }
    }
    
    // Process with concepts for abstract pattern-based decision making
    processWithConcepts();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
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
    
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ > 0.5f) {
        float exploreChance = curiosityLevel_ * 0.3f;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
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
    
    predictionError_ = reward - predictedReward;
    
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    dopamineLevel_ = predictionError_;
    
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
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

float AgentBrain::getAcetylcholineLevel() const {
    if (!brain_ || !brain_->getAcetylcholine()) return 0.0f;
    return brain_->getAcetylcholine()->getLevel();
}

float AgentBrain::getNorepinephrineLevel() const {
    if (!brain_ || !brain_->getNorepinephrine()) return 0.0f;
    return brain_->getNorepinephrine()->getLevel();
}

float AgentBrain::getSerotoninLevel() const {
    if (!brain_ || !brain_->getSerotonin()) return 0.0f;
    return brain_->getSerotonin()->getLevel();
}

float AgentBrain::getDopamineLevel() const {
    if (!brain_ || !brain_->getDopamine()) return 0.0f;
    return brain_->getDopamine()->getLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::saveEpisodeWithReward(const SimpleWorld& world) {
    if (!brain_) return;
    
    NLM_LOG_INFO("Saving episodic memory with reward: " + std::to_string(dopamineLevel_));
    
    // Create episodic memory item with current state
    EpisodicMemoryItem episode;
    episode.timestamp = brain_->getTotalSpikeCount();
    episode.reward = dopamineLevel_;
    
    // Capture current neural activity patterns
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || 
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                }
            }
        }
    }
    
    // Store world state
    episode.agentX = world.getAgentBody().x;
    episode.agentY = world.getAgentBody().y;
    episode.agentOrientation = world.getAgentBody().orientation;
    episode.agentEnergy = world.getAgentBody().energy;
    
    // Store reward information
    episode.reward = dopamineLevel_;
    
    // Store in episodic memory
    brain_->getEpisodicMemory()->storeEpisode(episode);
    
    NLM_LOG_INFO("Episodic memory saved with " + 
                 std::to_string(episode.activeNeurons.size()) + " active neurons, " +
                 std::to_string(episode.reward) + " reward value");
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm