#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Types/Types.hpp"
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

void AgentBrain::storeExperience(const SensoryPercept& percept, const MotorCommand& action,
                              float reward, SimulationStep step, Timestamp time) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Use the brain's episodic memory to store the experience
    brain_->getEpisodicMemory()->storeEpisode(convertPerceptToEpisodicItem(percept, action, reward, step));
    
    NLM_LOG_DEBUG("Stored experience in episodic memory (step " + std::to_string(step) + 
                  ", reward " + std::to_string(reward) + ", action " + std::to_string(static_cast<int>(action)) + ")");
}

nlm::EpisodicMemoryItem AgentBrain::convertPerceptToEpisodicItem(const SensoryPercept& percept, 
                                                              const MotorCommand& action,
                                                              float reward, SimulationStep step) {
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = step;
    
    // Store sensory state as flattened array
    episode.sensoryState.clear();
    const auto& vision = percept.getVision();
    episode.sensoryState.insert(episode.sensoryState.end(), vision.begin(), vision.end());
    
    const auto& touch = percept.getTouch();
    episode.sensoryState.insert(episode.sensoryState.end(), touch.begin(), touch.end());
    
    const auto& intern = percept.getInternal();
    episode.sensoryState.insert(episode.sensoryState.end(), intern.begin(), intern.end());
    
    // Add proprioception data if available
    const auto& proprio = percept.getProprioception();
    episode.sensoryState.insert(episode.sensoryState.end(), proprio.begin(), proprio.end());
    
    // Store position info (would need to be passed from world)
    // For now, default to (0,0) 
    episode.positionX = 0;
    episode.positionY = 0;
    episode.orientation = 0;
    
    // Store action
    episode.action = action;
    
    // Store internal state
    episode.reward = reward;
    episode.energy = 1.0f;  // Simplified energy estimate
    episode.novelty = noveltyLevel_;
    
    // Capture resulting state (would need to be computed after action execution)
    // For now, same as initial state
    episode.resultingSensoryState = episode.sensoryState;
    episode.resultingReward = reward;
    
    // Store neural activity pattern
    if (brain_->getWorkingMemory()) {
        auto workingMem = brain_->getWorkingMemory();
        auto memoryNeurons = workingMem->getMemoryNeurons();
        episode.activeNeurons = memoryNeurons;
        
        // Capture activations
        for (NeuronId id : memoryNeurons) {
            float activation = 0.0f;
            // Find neuron and get its activation level
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n->getId() == id) {
                            activation = std::abs(n->getState().membranePotential - n->getState().restingPotential) / 20.0f;
                            episode.neuronActivations.push_back(activation);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    episode.age = 0;
    
    return episode;
}

void AgentBrain::replayExperiences(size_t count) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Get episodes for replay
    auto episodesToReplay = brain_->getEpisodicMemory()->getEpisodesForReplay(count);
    
    // Replay each episode for memory consolidation
    for (const auto* episode : episodesToReplay) {
        // Replay the episode by reactivating the neural patterns
        brain_->getEpisodicMemory()->replayEpisode(episode);
        
        // Update developmental state based on replay
        updateDevelopment(0.1);  // Small timestep for replay
    }
    
    NLM_LOG_DEBUG("Replayed " + std::to_string(episodesToReplay.size()) + " experiences for consolidation");
}

void AgentBrain::updateWorkingMemory(const SensoryPercept& percept) {
    if (!brain_ || !brain_->getWorkingMemory()) return;
    
    // Convert sensory percept to working memory representation
    std::vector<float> sensoryPattern;
    
    // Add vision pattern (flatten vision data)
    const auto& vision = percept.getVision();
    sensoryPattern.insert(sensoryPattern.end(), vision.begin(), vision.end());
    
    // Add touch pattern
    const auto& touch = percept.getTouch();
    sensoryPattern.insert(sensoryPattern.end(), touch.begin(), touch.end());
    
    // Add internal signals
    const auto& intern = percept.getInternal();
    sensoryPattern.insert(sensoryPattern.end(), intern.begin(), intern.end());
    
    // Update working memory with new sensory input
    brain_->getWorkingMemory()->update(sensoryPattern);
}

std::vector<const EpisodicMemoryItem*> AgentBrain::getRecentExperiences(size_t count) const {
    std::vector<const EpisodicMemoryItem*> experiences;
    
    if (brain_ && brain_->getEpisodicMemory()) {
        // Get recent episodes from episodic memory
        // Note: This would need to be implemented in EpisodicMemory
        auto recentEpisodes = brain_->getEpisodicMemory()->getEpisodesForReplay(count);
        experiences = recentEpisodes;
    }
    
    return experiences;
}

bool AgentBrain::shouldStoreExperience(float novelty, float predictionError) const {
    // Store experience if there's significant novelty or prediction error
    return (novelty > 0.3f) || (std::abs(predictionError) > 0.5f);
}

} // namespace nlm
