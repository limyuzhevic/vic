#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
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

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Use brain's development system for developmental stages
    if (getDevelopmentSystem()) {
        // Advance development system based on simulation time
        getDevelopmentSystem()->update(brain_.get(), developmentalAge_);
        
        // Get plasticity modifier from development system
        plasticityModifier_ = getDevelopmentSystem()->getPlasticityModifier();
    } else {
        // Fallback to local computation
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

void AgentBrain::recordExperience(const SensoryPercept& percept, 
                                 float reward,
                                 float predictedReward) {
    if (!brain_ || !getEpisodicMemory()) return;
    
    // Store the complete sensory observation for episodic memory
    // This captures the actual sensory input patterns that drove neural activity
    
    // Process and store experience for each sensory modality
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Create episodic memory item with complete sensory context
    EpisodicMemoryItem episode;
    episode.timestamp = brain_->getCurrentStep();
    
    // Store all sensory inputs as complete state vectors
    episode.sensoryState.reserve(vision.size() + touch.size() + intern.size() + proprio.size());
    episode.sensoryState.insert(episode.sensoryState.end(), vision.begin(), vision.end());
    episode.sensoryState.insert(episode.sensoryState.end(), touch.begin(), touch.end());
    episode.sensoryState.insert(episode.sensoryState.end(), intern.begin(), intern.end());
    episode.sensoryState.insert(episode.sensoryState.end(), proprio.begin(), proprio.end());
    
    // Store position and orientation for spatial context
    episode.positionX = 0.0f;  // Should come from world state
    episode.positionY = 0.0f;  // Should come from world state
    episode.orientation = 0.0f;  // Should come from world state
    
    // Store reward prediction error
    episode.reward = reward;
    episode.resultingReward = predictedReward;
    
    // Calculate novelty (difference from previous experience)
    if (!previousVision_.empty() && !vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            totalDiff += std::abs(vision[i] - previousVision_[i]);
        }
        episode.novelty = totalDiff / std::max<size_t>(vision.size(), 1);
    } else {
        episode.novelty = 0.0f;
    }
    
    // Store action (will be determined from motor activity)
    // This should be updated by AgentBrain after action selection
    episode.action = ActionType::Wait;  // Default, will be updated
    
    // Get current neural activity pattern for the experience
    std::vector<float> neuralPattern;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Capture the neural state that resulted from this sensory input
    // This represents the internal representation of the experience
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || 
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                    activeNeurons.push_back(neuron->getId());
                    neuronActivations.push_back(
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                }
            }
        }
    }
    
    episode.activeNeurons = activeNeurons;
    episode.neuronActivations = neuronActivations;
    
    // Store resulting state for action-reward association
    episode.resultingSensoryState = vision;  // For now, just store vision
    
    // Store in episodic memory with working memory integration
    getEpisodicMemory()->storeEpisode(episode);
    
    // Update working memory traces with this new experience
    if (getWorkingMemory()) {
        // Strengthen working memory trace with this experience
        getWorkingMemory()->strengthenMemory(1.5f);
        
        // Update attention based on experience value (reward/novelty)
        if (getAttention()) {
            if (reward > 0.5f) {
                // Positive reward reinforces the experience
                getAttention()->applyTopDownBias(
                    static_cast<NeuronId>(episode.timestamp), 
                    0.8f * reward
                );
            }
            if (episode.novelty > 0.3f) {
                // Novelty captures attention
                getAttention()->applyBottomUpSalience(
                    static_cast<NeuronId>(episode.timestamp),
                    0.7f * episode.novelty
                );
            }
        }
    }
    
    // Update concept formation with this new experience
    if (getConceptFormation()) {
        getConceptFormation()->presentExperience(
            episode.sensoryState,
            vision,  // For concept formation
            episode.reward,
            episode.timestamp
        );
    }
    
    // Update prediction system with outcome
    if (getPredictionSystem()) {
        getPredictionSystem()->recordOutcome(
            episode.sensoryState,
            reward
        );
    }
    
    // Store for next experience comparison
    previousVision_ = vision;
    
    NLM_LOG_INFO("Recorded episodic experience with reward: " + std::to_string(reward) + 
                 ", novelty: " + std::to_string(episode.novelty) + 
                 ", total neurons active: " + std::to_string(activeNeurons.size()));
}
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize brain's cognition systems
    if (getConceptFormation()) {
        getConceptFormation()->initialize(brain_.get());
    }
    
    if (getAttention()) {
        getAttention()->initialize(brain_.get());
    }
    
    if (getWorkingMemory()) {
        getWorkingMemory()->initialize(brain_.get());
    }
    
    if (getPlanner()) {
        getPlanner()->initialize(brain_.get());
    }
    
    // Note: DevelopmentSystem doesn't have an initialize method, it's initialized in step()
    // Note: PredictionSystem doesn't have an initialize method
    
    if (getDopamine()) {
        getDopamine()->initialize(brain_.get());
    }
    
    if (getCuriosity()) {
        getCuriosity()->initialize(brain_.get());
    }
    
    if (getNovelty()) {
        getNovelty()->initialize(brain_.get());
    }
    
    if (getPredictionErrorSignal()) {
        getPredictionErrorSignal()->initialize(brain_.get());
    }
    
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
        
        // Connect to concept formation system
        if (getConceptFormation() && curiosityEnabled_) {
            // Extract features from vision for concept formation
            std::vector<float> features;
            for (float v : vision) {
                features.push_back(std::clamp(v, 0.0f, 1.0f));
            }
            
            // Present experience to concept formation
            getConceptFormation()->presentExperience(
                vision,  // Pattern
                features,  // Features
                expectedReward_,  // Reward
                brain_ ? brain_->getCurrentStep() : 0  // Current time
            );
            
            // Connect to attention system for working memory
            if (getWorkingMemory()) {
                // Store relevant sensory patterns in working memory
                getWorkingMemory()->store(vision, noveltyLevel_);
            }
            
            // Apply attentional selection based on novelty
            if (getAttention() && noveltyLevel_ > 0.3f) {
                // Focus attention on novel sensory input
                for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
                    if (sensoryVision_[i] && std::abs(vision[i] - previousVision_[i]) > 0.5f) {
                        // Apply attentional bias to this neuron
                        getAttention()->applyBottomUpSalience(
                            sensoryVision_[i]->getId(), 
                            noveltyLevel_ * 2.0f
                        );
                    }
                }
            }
        }
    }
    
    // Update curiosity based on novelty using brain's curiosity system
    if (curiosityEnabled_) {
        // Use brain's curiosity system for neuromodulation
        if (getCuriosity() && getPredictionErrorSignal() && getNovelty()) {
            // Update brain's curiosity system with current levels
            getCuriosity()->update(
                noveltyLevel_,  // Novelty
                getPredictionErrorSignal()->getError(),  // Prediction error
                1.0f  // dt (simplified)
            );
            
            // Get curiosity level from brain's system
            curiosityLevel_ = getCuriosity()->getLevel();
        } else {
            // Fallback to local computation
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        }
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Try to use neural planner for action planning
    MotorCommand plannedAction = MotorCommand::Wait;
    
    // Convert sensory percept to state vector for planning
    std::vector<float> currentState;
    currentState.reserve(sensoryVision_.size() + sensoryTouch_.size() + 
                         sensoryInternal_.size() + sensoryProprioception_.size());
    
    for (Neuron* n : sensoryVision_) {
        currentState.push_back(n->getState().membranePotential / 20.0f);
    }
    for (Neuron* n : sensoryTouch_) {
        currentState.push_back(n->getState().membranePotential / 20.0f);
    }
    for (Neuron* n : sensoryInternal_) {
        currentState.push_back(n->getState().membranePotential / 20.0f);
    }
    for (Neuron* n : sensoryProprioception_) {
        currentState.push_back(n->getState().membranePotential / 20.0f);
    }
    
    // Use neural planner if available
    if (getPlanner() && !currentState.empty()) {
        ActionType planned = getPlanner()->planAction(currentState, expectedReward_);
        plannedAction = static_cast<MotorCommand>(planned);
    }
    
    // If planner returned Wait, fall back to neural decoding
    if (plannedAction == MotorCommand::Wait) {
        plannedAction = decodeFromMotorNeurons();
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        plannedAction = selectWithCuriosity(plannedAction);
    }
    
    return plannedAction;
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
    
    // Connect to prediction system for error computation
    if (getPredictionErrorSignal()) {
        // Compute error using brain's prediction system
        getPredictionErrorSignal()->computeError(predictedReward, reward);
        
        // Connect to prediction system for higher-level prediction error
        predictionError_ = getPredictionErrorSignal()->getError();
    }
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    // Use brain's dopamine system for neuromodulation
    if (getDopamine() && getPredictionErrorSignal()) {
        // Signal reward prediction error using brain's dopamine system
        getDopamine()->signalRewardPredictionError(predictionError_);
        
        // Get dopamine level from brain system
        dopamineLevel_ = getDopamine()->getLevel();
    } else {
        // Fallback to local computation
        dopamineLevel_ = predictionError_;
    }
    
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
    
    // Modulate plasticity based on dopamine using brain's development system
    // Positive dopamine increases plasticity, negative decreases
    if (getDevelopmentSystem()) {
        // Get plasticity modifier from development system
        plasticityModifier_ = getDevelopmentSystem()->getPlasticityModifier();
        
        // Adapt based on dopamine level
        plasticityModifier_ = 0.5f + 0.5f * dopamineLevel_;
        plasticityModifier_ = std::clamp(plasticityModifier_, 0.1f, 2.0f);
    }
    
    // Update novelty using brain's novelty system
    if (getNovelty() && !vision.empty()) {
        // Detect novelty using brain's novelty system
        getNovelty()->detectNovelty(vision, previousVision_);
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    // Use brain's dopamine system for neuromodulation
    if (getDopamine()) {
        return getDopamine()->getLevel();
    }
    // Fallback to local computation
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    // Use brain's curiosity system for neuromodulation
    if (getCuriosity()) {
        return getCuriosity()->getLevel();
    }
    // Fallback to local computation
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    // Use brain's novelty system
    if (getNovelty()) {
        return getNovelty()->getLevel();
    }
    // Fallback to local computation
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    // Use brain's prediction error signal system
    if (getPredictionErrorSignal()) {
        return getPredictionErrorSignal()->getError();
    }
    // Fallback to local computation
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
    
    // Reset brain's cognition systems
    if (getConceptFormation()) {
        getConceptFormation()->clear();
    }
    
    if (getWorkingMemory()) {
        getWorkingMemory()->clear();
    }
    
    if (getPredictionSystem()) {
        getPredictionSystem()->clearHistory();
    }
    
    if (getPlanner()) {
        getPlanner()->clearCache();
    }
    
    if (getNovelty()) {
        getNovelty()->clearHistory();
    }
    
    if (getPredictionErrorSignal()) {
        getPredictionErrorSignal()->clearHistory();
    }
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
