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
    
    // Initialize memory systems integration
    if (brain_) {
        NLM_LOG_INFO("Memory integration check:");
        NLM_LOG_INFO("  WorkingMemory: " + std::to_string(brain_->getWorkingMemory() != nullptr));
        NLM_LOG_INFO("  EpisodicMemory: " + std::to_string(brain_->getEpisodicMemory() != nullptr));
        NLM_LOG_INFO("  PredictionSystem: " + std::to_string(brain_->getPredictionSystem() != nullptr));
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

// ========== MEMORY SYSTEM PROCESSING METHODS ==========

void AgentBrain::processSensoryInputWithMemory(const SensoryPercept& percept) {
    // Store current sensory input in working memory
    if (getWorkingMemory() && brain_) {
        // Convert percept to neural pattern for working memory
        std::vector<float> sensoryPattern;
        
        // Vision pattern
        const auto& vision = percept.getVision();
        sensoryPattern.insert(sensoryPattern.end(), vision.begin(), vision.end());
        
        // Touch pattern
        const auto& touch = percept.getTouch();
        sensoryPattern.insert(sensoryPattern.end(), touch.begin(), touch.end());
        
        // Internal pattern
        const auto& internal = percept.getInternal();
        sensoryPattern.insert(sensoryPattern.end(), internal.begin(), internal.end());
        
        // Proprioception pattern
        const auto& proprio = percept.getProprioception();
        sensoryPattern.insert(sensoryPattern.end(), proprio.begin(), proprio.end());
        
        // Store in working memory with increasing activation
        if (!sensoryPattern.empty()) {
            float strength = 1.0f;
            
            // Create a working memory trace for this sensory pattern
            getWorkingMemory()->storeToNeuron(
                NeuronId(static_cast<uint64_t>(sensoryPattern.size()) * 10000), 
                strength
            );
        }
    }
    
    // Original sensory processing (backward compatibility)
    processSensoryInput(percept);
}

void AgentBrain::storeExperience(float reward, const std::vector<float>& sensoryState,
                                const std::vector<float>& resultingSensoryState, ActionType action) {
    auto episodicMemory = getEpisodicMemory();
    if (!episodicMemory) return;
    
    // Create episodic memory item
    EpisodicMemoryItem episode;
    episode.timestamp = static_cast<SimulationStep>(brain_ ? 
        brain_->getTotalSpikeCount() : 0);
    episode.reward = reward;
    episode.action = action;
    episode.sensoryState = sensoryState;
    episode.resultingSensoryState = resultingSensoryState;
    episode.neurons.clear();
    episode.neuronActivations.clear();
    
    // Add current brain state to episode (active neurons)
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.neurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
    }
    
    // Store episode
    episodicMemory->storeEpisode(episode);
    
    // Store in associative memory for pattern-action associations
    auto associativeMemory = getAssociativeMemory();
    if (associativeMemory) {
        // Create pattern from sensory state
        if (!sensoryState.empty()) {
            // Create action pattern from action type
            std::vector<float> actionPattern(sensoryState.size(), 0.0f);
            actionPattern[0] = static_cast<float>(action) / 10.0f;  // Scale action to pattern range
            
            // Store association
            associativeMemory->associate(sensoryState, actionPattern, 0.8f);
        }
    }
    
    NLM_LOG_INFO("Stored experience in episodic memory (reward: " + std::to_string(reward) +
                ", action: " + std::to_string(static_cast<int>(action)) + ")");
}

std::vector<const EpisodicMemoryItem*> AgentBrain::retrieveSimilarExperiences(const std::vector<float>& sensoryState,
                                                                              size_t maxResults) {
    auto episodicMemory = getEpisodicMemory();
    if (!episodicMemory) return {};
    
    return episodicMemory->retrieveSimilar(sensoryState, maxResults);
}

MotorCommand AgentBrain::decodeMotorCommandWithMemory() {
    // Use memory to guide action selection
    MotorCommand baseCommand = decodeMotorCommand();
    
    // Get working memory patterns
    auto workingMemory = getWorkingMemory();
    if (workingMemory) {
        float memoryActivity = workingMemory->getMemoryActivity();
        float memoryWeight = std::min(memoryActivity * 0.2f, 1.0f);  // Scale memory influence
        
        // If we have stored memories, use them to influence action
        if (memoryActivity > 0.3f) {
            // Higher memory activity suggests similar past experiences
            // Bias toward actions that worked in similar situations
            return selectWithCuriosity(baseCommand);
        }
    }
    
    // If no memory influence, use standard decoding
    return baseCommand;
}

void AgentBrain::applyRewardModulationWithMemory(float reward, float predictedReward) {
    // Apply standard reward modulation first
    applyRewardModulation(reward, predictedReward);
    
    // Store experience in episodic memory
    if (brain_ && brain_->getTotalSpikeCount() > 0) {
        // We need sensory state and resulting state for episodic memory
        // For now, use placeholders - in a real implementation, we would track
        // sensory states before and after actions
        std::vector<float> sensoryState;
        std::vector<float> resultingSensoryState;
        
        ActionType lastAction = ActionType::Wait;
        
        storeExperience(reward, sensoryState, resultingSensoryState, lastAction);
    }
}

void AgentBrain::updateMemorySystems(double timestep) {
    // Update development (affects plasticity)
    updateDevelopment(timestep);
    
    // Update memory systems if available
    auto workingMemory = getWorkingMemory();
    auto episodicMemory = getEpisodicMemory();
    
    if (workingMemory) {
        // Update working memory
        workingMemory->update(timestep);
    }
    
    if (episodicMemory) {
        // Update episodic memory (age episodes, run consolidation)
        episodicMemory->consolidate(0.5f);
    }
    
    // Get prediction system and update if available
    auto predictionSystem = getPredictionSystem();
    if (predictionSystem) {
        // Prediction system updates would go here
        // In a full implementation, this would:
        // 1. Process current sensory state
        // 2. Compare with prediction
        // 3. Update prediction error
        // 4. Store predictions in working memory
    }
}

} // namespace nlm
