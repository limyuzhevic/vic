#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

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
    , episodeActive_(false)
    , currentStep_(0)
    , currentTime_(0.0)
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

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    episodeActive_ = false;
    currentStep_ = 0;
    currentTime_ = 0.0;
    
    // Clear current experience data
    currentSensoryState_.clear();
    currentAction_ = MotorCommand::Wait;
    currentReward_ = 0.0f;
    currentEnergy_ = 1.0f;
    currentNovelty_ = 0.0f;
    currentPredictionError_ = 0.0f;
    currentNeuralActivity_.clear();
    
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
    
    // Capture current sensory state for episodic memory
    currentSensoryState_.clear();
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    currentSensoryState_.insert(currentSensoryState_.end(), vision.begin(), vision.end());
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    currentSensoryState_.insert(currentSensoryState_.end(), touch.begin(), touch.end());
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    currentSensoryState_.insert(currentSensoryState_.end(), intern.begin(), intern.end());
    
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    currentSensoryState_.insert(currentSensoryState_.end(), proprio.begin(), proprio.end());
    
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

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Store action decision in episodic memory if action is meaningful
    if (decoded != MotorCommand::Wait) {
        currentAction_ = decoded;
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

MotorCommand selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(static_cast<uint32_t>(0), static_cast<uint32_t>(7));
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

void AgentBrain::storeExperience(const SimpleWorld& world, MotorCommand action, float reward, float predictedReward) {
    // Store the current experience for episodic memory
    episodeActive_ = true;
    currentAction_ = action;
    currentReward_ = reward;
    currentEnergy_ = world.getAgentBody().energy;
    currentPredictionError_ = predictionError_;
    
    // Store neural activity pattern from working memory
    if (brain_ && brain_->getWorkingMemory()) {
        currentNeuralActivity_ = brain_->getWorkingMemory()->retrieve();
    }
    
    // Create and store episodic memory item
    float curiosity = curiosityLevel_;
    float novelty = noveltyLevel_;
    
    EpisodicMemoryItem episode = createEpisodicMemoryItem(world, action, reward, predictedReward,
                                                          curiosity, novelty, currentNeuralActivity_);
    
    if (brain_ && brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->storeEpisode(episode);
        NLM_LOG_INFO("Stored episodic memory item: step=" + std::to_string(currentStep_)
                    + ", action=" + std::to_string(static_cast<int>(action))
                    + ", reward=" + std::to_string(reward)
                    + ", novelty=" + std::to_string(novelty)
                    + ", sensoryDim=" + std::to_string(currentSensoryState_.size()));
    }
}

EpisodicMemoryItem AgentBrain::createEpisodicMemoryItem(const SimpleWorld& world, MotorCommand action, 
                                                      float reward, float predictedReward, float curiosity,
                                                      float novelty, const std::vector<float>& neuralActivity) const {
    EpisodicMemoryItem episode;
    
    // Set temporal and state information
    episode.timestamp = currentStep_;
    episode.age = 0;  // Will be aged by episodic memory system
    
    // Store current sensory state
    episode.sensoryState = currentSensoryState_;
    
    // Store position and orientation from world state
    episode.positionX = world.getAgentBody().x;
    episode.positionY = world.getAgentBody().y;
    episode.orientation = world.getAgentBody().orientation;
    
    // Store action decision
    episode.action = action;
    
    // Store internal state
    episode.reward = reward;
    episode.energy = world.getAgentBody().energy;
    episode.novelty = novelty;
    episode.resultingReward = predictedReward;
    
    // Store neural activity pattern for later pattern completion
    episode.activeNeurons.clear();
    episode.neuronActivations.clear();
    
    // Record current neuromodulation state
    if (brain_) {
        if (brain_->getDopamine()) {
            float dopamineLevel = brain_->getDopamine()->getLevel();
            episode.neuronActivations.push_back(dopamineLevel);
        }
        if (brain_->getCuriosity()) {
            float curiosityLevel = brain_->getCuriosity()->getLevel();
            episode.neuronActivations.push_back(curiosityLevel);
        }
        if (brain_->getNovelty()) {
            float noveltyLevel = brain_->getNovelty()->getLevel();
            episode.neuronActivations.push_back(noveltyLevel);
        }
    }
    
    // Store the full neural activity pattern
    episode.neuronActivations.insert(episode.neuronActivations.end(), 
                                     neuralActivity.begin(), neuralActivity.end());
    
    // Store resulting state (this would be updated after world effects)
    episode.resultingSensoryState = currentSensoryState_;
    
    return episode;
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
    
    // Update current step from brain (maintains temporal consistency)
    currentStep_ = brain_->getCurrentStep();
    currentTime_ = brain_->getCurrentTime();
    
    // Store the current experience in episodic memory
    // This is the key integration - episodic memory automatically captures the agent loop experience
    if (brain_) {
        storeExperienceSimplified(currentStep_, currentAction_, reward, predictedReward);
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
}

void AgentBrain::storeExperienceSimplified(SimulationStep step, MotorCommand action, float reward, float predictedReward) {
    // Store the current experience for episodic memory without requiring the world reference
    // This is the automatic episodic memory integration - experiences are captured during the agent loop
    episodeActive_ = true;
    currentStep_ = step;
    currentAction_ = action;
    currentReward_ = reward;
    currentPredictionError_ = predictionError_;
    
    // Store neural activity pattern from working memory
    if (brain_ && brain_->getWorkingMemory()) {
        currentNeuralActivity_ = brain_->getWorkingMemory()->retrieve();
    }
    
    // Create and store episodic memory item
    float curiosity = curiosityLevel_;
    float novelty = noveltyLevel_;
    
    // For this simplified version, we'll create a placeholder for the world reference
    // In a real implementation, this would be passed from the agent loop
    class SimpleWorld dummyWorld;
    dummyWorld.setAgentStart(0.0f, 0.0f);
    dummyWorld.configure(100.0f, 100.0f, 16, 16);
    
    EpisodicMemoryItem episode = createEpisodicMemoryItem(dummyWorld, action, reward, predictedReward,
                                                          curiosity, novelty, currentNeuralActivity_);
    
    if (brain_ && brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->storeEpisode(episode);
        NLM_LOG_INFO("Stored episodic memory item: step=" + std::to_string(step)
                    + ", action=" + std::to_string(static_cast<int>(action))
                    + ", reward=" + std::to_string(reward)
                    + ", novelty=" + std::to_string(novelty)
                    + ", sensoryDim=" + std::to_string(currentSensoryState_.size()));
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

float AgentBrain::getPredictionAccuracy() const {
    return predictionAccuracy_;
}

void AgentBrain::predictAndUpdatePredictions(const SensoryPercept& percept, SimulationStep timestep) {
    if (!brain_ || !predictionSystem_) return;
    
    // Extract current sensory state from percept
    std::vector<float> currentSensoryState;
    
    // Add vision data
    const auto& vision = percept.getVision();
    currentSensoryState.insert(currentSensoryState.end(), vision.begin(), vision.end());
    
    // Add touch data
    const auto& touch = percept.getTouch();
    currentSensoryState.insert(currentSensoryState.end(), touch.begin(), touch.end());
    
    // Add internal signals
    const auto& internal = percept.getInternal();
    currentSensoryState.insert(currentSensoryState.end(), internal.begin(), internal.end());
    
    // Add proprioception
    const auto& proprioception = percept.getProprioception();
    currentSensoryState.insert(currentSensoryState.end(), proprioception.begin(), proprioception.end());
    
    // Create SensoryInput from current state
    auto sensoryInput = std::make_unique<SensoryInput>(currentSensoryState.size());
    for (size_t i = 0; i < currentSensoryState.size() && i < sensoryInput->getData().size(); ++i) {
        sensoryInput->getData()[i] = currentSensoryState[i];
    }
    
    // Get predictions from prediction system
    auto prediction = predictionSystem_->predictNextState(*sensoryInput);
    
    // Compare predictions with actual sensory input (simple error calculation)
    float predictionError = 0.0f;
    if (prediction && prediction->getData().size() == currentSensoryState.size()) {
        for (size_t i = 0; i < prediction->getData().size(); ++i) {
            float diff = prediction->getData()[i] - currentSensoryState[i];
            predictionError += diff * diff;
        }
        predictionError = std::sqrt(predictionError / prediction->getData().size());
    }
    
    // Store prediction error
    predictionError_ = predictionError;
    predictionAccuracy_ = std::max(0.0f, 1.0f - predictionError);
    
    // Update prediction system with actual observations
    predictionSystem_->updatePredictions(*prediction, *sensoryInput);
    
    // Train prediction system with current observation
    predictionSystem_->train(*sensoryInput);
    
    // Update neural prediction system with activity patterns
    std::vector<float> neuralActivity;
    for (const auto& region : brain_->getRegions()) {
        for (const auto* pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                float activation = std::max(0.0f, std::min(1.0f, 
                    (neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f));
                neuralActivity.push_back(activation);
            }
        }
    }
    
    // Update prediction system with neural activity
    predictionSystem_->updateWithNeuralActivity(neuralActivity, timestep);
    
    // Automatic prediction system integration
    predictionSystem_->automaticUpdate(*sensoryInput, timestep);
    
    // Store current state for episodic memory
    currentSensoryState_ = currentSensoryState;
    currentPredictionError_ = predictionError;
    currentNeuralActivity_ = neuralActivity;
    
    // Update neuromodulation based on prediction accuracy
    if (predictionError_ > 0.5f) {
        // High prediction error triggers curiosity and exploration
        curiosityLevel_ = std::min(1.0f, curiosityLevel_ + 0.1f);
        dopamineLevel_ = std::max(0.0f, dopamineLevel_ - 0.1f);
    } else {
        // Successful prediction reinforces learning
        curiosityLevel_ = std::max(0.0f, curiosityLevel_ - 0.05f);
        dopamineLevel_ = std::min(1.0f, dopamineLevel_ + 0.05f);
    }
    
    // Log prediction performance
    NLM_LOG_INFO("Prediction cycle: error=" + std::to_string(predictionError_) + 
                 ", accuracy=" + std::to_string(predictionAccuracy_) + 
                 ", confidence=" + std::to_string(predictionSystem_->getConfidence()));
}
