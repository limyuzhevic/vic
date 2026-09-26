#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Enhanced implementation with full cognitive integration
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
    
    // Initialize integrated systems references
    if (brain_) {
        workingMemory_ = brain_->getWorkingMemory();
        episodicMemory_ = brain_->getEpisodicMemory();
        predictionSystem_ = brain_->getPredictionSystem();
        neuralPlanner_ = brain_->getPlanner();
        conceptFormation_ = brain_->getConceptFormation();
        attentionSystem_ = brain_->getAttention();
        dopamineSystem_ = brain_->getDopamine();
        curiositySystem_ = brain_->getCuriosity();
        noveltySystem_ = brain_->getNovelty();
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
    
    // Initialize integrated systems
    if (brain_) {
        if (workingMemory_) {
            workingMemory_->initialize(brain_.get());
            workingMemory_->setCapacity(brain_->getTotalNeuronCount() / 20);
        }
        if (episodicMemory_) {
            episodicMemory_->initialize(brain_.get());
            episodicMemory_->setMaxEpisodes(1000);
        }
        if (predictionSystem_) {
            NLM_LOG_INFO("Prediction system integrated with AgentBrain");
        }
        if (neuralPlanner_) {
            NLM_LOG_INFO("Neural planner integrated with AgentBrain");
        }
        if (conceptFormation_) {
            NLM_LOG_INFO("Concept formation integrated with AgentBrain");
        }
        if (attentionSystem_) {
            NLM_LOG_INFO("Attention system integrated with AgentBrain");
        }
    }
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
    
    // Extract sensory data
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Vision input (256 values -> sensoryVision_ neurons)
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
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
        
        // Update novelty system if available
        if (noveltySystem_) {
            noveltySystem_->detectNovelty(vision, previousVision_);
        }
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Update curiosity system if available
        if (curiositySystem_) {
            curiositySystem_->update(noveltyLevel_, predictionError_, 0.001f);
        }
    }
    
    // Enhanced integration with brain memory systems
    integrateWithMemorySystems(percept);
    integrateWithPredictionSystem(percept);
    integrateWithCognitionSystems(percept);
}

void AgentBrain::integrateWithMemorySystems(const SensoryPercept& percept) {
    if (!brain_ || !workingMemory_ || !episodicMemory_) return;
    
    // Store sensory input in working memory
    storeInWorkingMemory(percept);
    
    // Create episodic memory from current experience
    createEpisodicMemory(percept);
    
    // Update attention system with working memory state
    if (attentionSystem_ && !workingMemory_->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory_->getMemoryNeurons();
        attentionSystem_->processCompetition(competitors);
    }
}

void AgentBrain::storeInWorkingMemory(const SensoryPercept& percept) {
    if (!workingMemory_) return;
    
    // Store vision data in working memory
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < vision.size() && i < sensoryVision_.size(); ++i) {
        if (sensoryVision_[i] && vision[i] > 0.5f) {
            workingMemory_->store(sensoryVision_[i]->getId(), vision[i]);
        }
    }
    
    // Store touch data in working memory
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < touch.size() && i < sensoryTouch_.size(); ++i) {
        if (sensoryTouch_[i] && touch[i] > 0.5f) {
            workingMemory_->store(sensoryTouch_[i]->getId(), touch[i]);
        }
    }
}

void AgentBrain::createEpisodicMemory(const SensoryPercept& percept) {
    if (!episodicMemory_) return;
    
    // Create episodic memory item from current sensory percept
    EpisodicMemoryItem episode;
    episode.timestamp = brain_->getTotalSpikeCount();  // Use spike count as episode identifier
    
    // Add sensory neuron activations to episode
    for (size_t i = 0; i < sensoryVision_.size() && i < percept.getVision().size(); ++i) {
        if (sensoryVision_[i] && percept.getVision()[i] > 0.5f) {
            episode.activeNeurons.push_back(sensoryVision_[i]->getId());
            episode.neuronActivations.push_back(percept.getVision()[i]);
        }
    }
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < percept.getTouch().size(); ++i) {
        if (sensoryTouch_[i] && percept.getTouch()[i] > 0.5f) {
            episode.activeNeurons.push_back(sensoryTouch_[i]->getId());
            episode.neuronActivations.push_back(percept.getTouch()[i]);
        }
    }
    
    // Add reward signal to episode
    episode.reward = dopamineLevel_;
    
    // Store episode
    episodicMemory_->storeEpisode(episode);
}

void AgentBrain::integrateWithPredictionSystem(const SensoryPercept& percept) {
    if (!predictionSystem_) return;
    
    // Generate prediction based on current sensory input
    generatePrediction(percept);
    
    // Update prediction based on actual sensory input
    updatePrediction(percept);
}

void AgentBrain::generatePrediction(const SensoryPercept& percept) {
    if (!predictionSystem_) return;
    
    // For now, use average vision as prediction
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        float predictedValue = std::accumulate(vision.begin(), vision.end(), 0.0f) / vision.size();
        // Store prediction for comparison
        predictionSystem_->updatePrediction(predictedValue);
    }
}

void AgentBrain::updatePrediction(const SensoryPercept& percept) {
    if (!predictionSystem_) return;
    
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        float predictedValue = 0.0f;
        // Get current prediction from system
        if (predictionSystem_->getHistory().size() > 0) {
            predictedValue = predictionSystem_->getHistory().back();
        } else {
            predictedValue = std::accumulate(vision.begin(), vision.end(), 0.0f) / vision.size();
        }
        
        float actualValue = std::accumulate(vision.begin(), vision.end(), 0.0f) / vision.size();
        
        // Compute prediction error
        predictionError_ = actualValue - predictedValue;
        
        // Update prediction system
        predictionSystem_->computeError(predictedValue, actualValue);
    }
}

void AgentBrain::integrateWithCognitionSystems(const SensoryPercept& percept) {
    if (!neuralPlanner_ || !conceptFormation_ || !attentionSystem_) return;
    
    // Update concept formation with current sensory data
    formConcepts(percept);
    
    // Generate plan based on current concepts and goals
    generatePlan();
    
    // Apply attention to select relevant concepts
    applyAttention();
}

void AgentBrain::formConcepts(const SensoryPercept& percept) {
    if (!conceptFormation_) return;
    
    // Extract features for concept formation
    std::vector<float> currentPattern;
    const auto& vision = percept.getVision();
    
    // Simple feature extraction (average vision value for now)
    if (!vision.empty()) {
        float avgVision = std::accumulate(vision.begin(), vision.end(), 0.0f) / vision.size();
        currentPattern.push_back(avgVision);
        
        // Store pattern for concept learning
        conceptFormation_->learnPattern(currentPattern);
    }
}

void AgentBrain::generatePlan() {
    if (!neuralPlanner_) return;
    
    // Use neural planner to generate action plan
    neuralPlanner_->planActions();
    
    // Extract planned actions
    std::vector<ActionType> plannedActions = neuralPlanner_->getPlannedActions();
    
    if (!plannedActions.empty()) {
        NLM_LOG_INFO("Generated plan with " + std::to_string(plannedActions.size()) + " actions");
        // Could modify motor decoding based on plan
    }
}

void AgentBrain::applyAttention() {
    if (!attentionSystem_) return;
    
    // Get current attention focus
    NeuronId focusedNeuron = attentionSystem_->getFocus();
    
    if (focusedNeuron.index() > 0) {
        NLM_LOG_INFO("Attention focused on neuron " + std::to_string(focusedNeuron.index()));
        // Apply attention to relevant neural processing
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply plan-based action selection if planning is available
    if (neuralPlanner_ && neuralPlanner_->hasPlan()) {
        ActionType plannedAction = neuralPlanner_->getNextAction();
        decoded = actionTypeToMotorCommand(plannedAction);
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

MotorCommand AgentBrain::actionTypeToMotorCommand(ActionType actionType) {
    switch (actionType) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::LookLeft: return MotorCommand::LookLeft;
        case ActionType::LookRight: return MotorCommand::LookRight;
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Wait: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Apply enhanced neuromodulation using brain systems
    applyEnhancedNeuromodulation(reward, predictedReward);
    
    // Also update our legacy state for compatibility
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward-modulated weight change to synapses
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
    
    // Modulate plasticity based on dopamine using enhanced system
    if (dopamineSystem_) {
        dopamineSystem_->setLevel(dopamineLevel_);
        float plasticityFactor = dopamineSystem_->getPlasticityFactor();
        
        // Apply plasticity factor to STDP
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            // Adjust STDP weights based on enhanced dopamine modulation
            float originalLTP = 0.01f;
            float originalLTD = 0.012f;
            stdp->setLTPWeight(originalLTP * plasticityFactor);
            stdp->setLTDWeight(originalLTD * plasticityFactor);
        }
    }
}

void AgentBrain::applyEnhancedNeuromodulation(float reward, float predictedReward) {
    if (!brain_ || !dopamineSystem_ || !curiositySystem_ || !noveltySystem_) return;
    
    // Enhanced neuromodulation integration
    
    // 1. Compute prediction error for dopamine system
    float predictionError = reward - predictedReward;
    dopamineSystem_->signalPredictionError(predictionError);
    
    // 2. Update dopamine level based on reward
    dopamineSystem_->signalReward(reward);
    
    // 3. Update curiosity based on novelty and prediction error
    curiositySystem_->update(noveltyLevel_, predictionError, 0.001f);
    
    // 4. Update attention based on curiosity and reward
    if (dopamineLevel_ > 0.5f) {
        // High dopamine enhances attention to rewarding stimuli
    } else if (dopamineLevel_ < -0.5f) {
        // Low dopamine reduces attention
    }
    
    // 5. Update working memory consolidation based on dopamine
    if (workingMemory_ && dopamineSystem_->isBurstMode()) {
        // During dopamine bursts, consolidate working memory
        workingMemory_->decay(0.9f);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
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
    
    // Update development system if available
    if (brain_->getDevelopmentSystem()) {
        brain_->getDevelopmentSystem()->update(brain_.get(), *brain_->getRandomGenerator(), timestep);
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
    
    // Reset integrated systems
    if (dopamineSystem_) {
        dopamineSystem_->reset();
    }
    if (curiositySystem_) {
        curiositySystem_->reset();
    }
    if (noveltySystem_) {
        noveltySystem_->clearHistory();
    }
}

} // namespace nlm