#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
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
    , sensoryNoveltyDecay_(0.99f)
    , attentionEnabled_(true)
    , conceptFormationEnabled_(true)
    , predictionEnabled_(true)
    , episodicMemoryEnabled_(true)
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
    
    // Initialize cognitive systems from brain (Phase 6: Full Integration)
    if (brain_) {
        // Instantiate and configure cognitive systems
        neuralPlanner_ = brain_->getPlanner();
        conceptFormation_ = brain_->getConceptFormation();
        predictionSystem_ = brain_->getPredictionSystem();
        attentionSystem_ = brain_->getAttention();
        episodicMemory_ = brain_->getEpisodicMemory();
        workingMemory_ = brain_->getWorkingMemory();
        
        // Configure cognitive systems
        if (neuralPlanner_) {
            neuralPlanner_->initialize(brain_.get());
            neuralPlanner_->setPlanningDepth(5);  // Look 5 steps ahead
            neuralPlanner_->setCurrentGoal(std::vector<float>(getSensoryInputSize(), 0.0f));
        }
        
        if (conceptFormation_) {
            conceptFormation_->initialize(brain_.get());
        }
        
        if (predictionSystem_) {
            // PredictionSystem doesn't have initialize method currently
        }
        
        if (attentionSystem_) {
            attentionSystem_->initialize(brain_.get());
            attentionSystem_->setInhibitionStrength(0.5f);
            attentionSystem_->setExcitationStrength(1.5f);
        }
        
        // Initialize self-model and social learning if available
        // SelfModel* selfModel = brain_->getSelfModel();  // TODO: Add getter method
        // SocialLearning* socialLearning = brain_->getSocialLearning();  // TODO: Add getter method
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
    
    // Initialize cognitive systems
    if (neuralPlanner_) {
        neuralPlanner_->setCurrentGoal(std::vector<float>(getSensoryInputSize(), 0.0f));
        NLM_LOG_INFO("  - Neural Planner initialized (depth: " + 
                     std::to_string(neuralPlanner_->getPlanningDepth()) + ")");
    }
    
    if (conceptFormation_) {
        NLM_LOG_INFO("  - Concept Formation system initialized");
    }
    
    if (predictionSystem_) {
        NLM_LOG_INFO("  - Prediction System initialized");
    }
    
    if (attentionSystem_) {
        NLM_LOG_INFO("  - Attention System initialized");
    }
    
    if (workingMemory_) {
        NLM_LOG_INFO("  - Working Memory system initialized");
    }
    
    if (episodicMemory_) {
        NLM_LOG_INFO("  - Episodic Memory system initialized");
    }
    
    NLM_LOG_INFO("AgentBrain cognitive integration complete - Phase 6 Ready!");
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
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Store sensory input in working memory for attention and concept formation
    if (workingMemory_ && attentionEnabled_ && conceptFormationEnabled_) {
        std::vector<float> sensoryPattern = percept.getAllSignals();
        workingMemory_->storeToNeuron(
            sensoryVision_.empty() ? 0 : sensoryVision_[0]->getId(),
            sensoryPattern.size() > 0 ? sensoryPattern[0] : 0.0f
        );
        
        // Store in episodic memory with timestamp
        if (episodicMemoryEnabled_ && episodicMemory_) {
            EpisodicMemoryItem episode;
            episode.timestamp = brain_->getCurrentStep();
            episode.reward = dopamineLevel_;
            
            // Store active sensory neurons
            for (size_t i = 0; i < std::min(sensoryVision_.size(), size_t(10)); ++i) {
                if (sensoryVision_[i] && sensoryVision_[i]->isFiring()) {
                    episode.activeNeurons.push_back(sensoryVision_[i]->getId());
                    episode.neuronActivations.push_back(
                        std::abs(sensoryVision_[i]->getState().membranePotential - 
                                sensoryVision_[i]->getState().restingPotential) / 10.0f);
                }
            }
            
            // Store the sensory percept as a pattern
            std::vector<float> episodePattern = percept.getAllSignals();
            if (!episodePattern.empty()) {
                episodicMemory_->storeEpisode(episode);
            }
        }
    }
    
    // Process through attention system for selective focus
    if (attentionEnabled_ && attentionSystem_ && workingMemory_) {
        std::vector<NeuronId> competitors = workingMemory_->getMemoryNeurons();
        if (!competitors.empty()) {
            attentionSystem_->processCompetition(competitors);
            
            // Use attention to guide concept formation
            if (conceptFormationEnabled_ && conceptFormation_) {
                // Get the attended concept based on working memory
                size_t conceptId = conceptFormation_->getMatchingConcept(
                    std::vector<float>(percept.getAllSignals()), 0.7f);
                
                if (conceptId > 0) {
                    // Store experience in this concept
                    conceptFormation_->presentExperience(
                        percept.getAllSignals(),
                        std::vector<float>(percept.getAllSignals().size(), 0.0f),  // dummy features
                        dopamineLevel_,
                        brain_->getTotalStepCount()
                    );
                }
            }
        }
    }
    
    // Novelty detection
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
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update prediction system
    if (predictionEnabled_ && predictionSystem_) {
        // Predict next state based on current sensory input
        // This is a simplified prediction - real implementation would use sensory encoding
        predictionError_ = noveltyLevel_;  // Simple heuristic: high novelty = high prediction error
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded;
    
    // Use Neural Planner for action selection (Phase 6: Full Integration)
    if (neuralPlanner_ && predictionEnabled_) {
        // Get current state from sensory input for planning
        std::vector<float> currentState;
        if (!previousVision_.empty()) {
            currentState = previousVision_;
        } else {
            currentState = std::vector<float>(256, 0.0f);  // Default vision size
        }
        
        // Get planned action from neural planner
        // Note: NeuralPlanner expects ActionType, but we'll use MotorCommand
        // This is a simplified integration - in reality, ActionType and MotorCommand should be unified
        ActionType plannedAction = neuralPlanner_->planAction(currentState, 0.5f);
        
        // Convert ActionType to MotorCommand
        switch (plannedAction) {
            case ActionType::MoveForward: decoded = MotorCommand::MoveForward; break;
            case ActionType::MoveBackward: decoded = MotorCommand::MoveBackward; break;
            case ActionType::TurnLeft: decoded = MotorCommand::TurnLeft; break;
            case ActionType::TurnRight: decoded = MotorCommand::TurnRight; break;
            case ActionType::LookLeft: decoded = MotorCommand::LookLeft; break;
            case ActionType::LookRight: decoded = MotorCommand::LookRight; break;
            case ActionType::Interact: decoded = MotorCommand::Interact; break;
            default: decoded = MotorCommand::Wait; break;
        }
        
        NLM_LOG_INFO("  - Neural Planner selected action: " + std::to_string(static_cast<int>(decoded)));
    } else {
        // Fallback to basic motor decoding (for backward compatibility)
        decoded = decodeFromMotorNeurons();
    }
    
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
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply neuromodulation effects on cognitive systems
    applyNeuromodulationToCognitiveSystems();
    
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

void AgentBrain::applyNeuromodulationToCognitiveSystems() {
    if (!brain_) return;
    
    // Apply neuromodulation effects to cognitive systems
    if (attentionSystem_ && attentionEnabled_) {
        // ACh-like modulation of attention (enhanced attention with positive dopamine)
        float attentionMod = 0.5f + 0.5f * std::max(0.0f, dopamineLevel_);
        // TODO: Implement actual ACh effects on attention
        // This would modulate inhibition/excitation balance
    }
    
    if (neuralPlanner_ && predictionEnabled_) {
        // NE-like modulation of planning (arousal affects planning horizon)
        float arousal = std::max(0.0f, dopamineLevel_) + curiosityLevel_;
        // TODO: Implement NE effects on planning depth and confidence
        // Higher arousal increases exploration in planning
    }
    
    if (curiosityEnabled_) {
        // Novelty/modulation affects learning and exploration
        float noveltyInfluence = noveltyLevel_ * curiosityLevel_;
        // TODO: Implement novelty effects on concept formation
        // Novel experiences trigger new concept formation
    }
    
    if (conceptFormationEnabled_ && conceptFormation_) {
        // Dopamine effects on concept stability and formation
        float conceptPlasticity = 0.1f + 0.1f * std::max(0.0f, dopamineLevel_);
        // TODO: Implement dopamine effects on concept learning rates
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

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
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
