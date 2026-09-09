#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/MotorCommand.hpp"
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
                        // Use modulo 12 to distribute evenly among all 12 motor actions
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                      motorTurnLeft_.size() + motorTurnRight_.size() +
                                      motorLookLeft_.size() + motorLookRight_.size() +
                                      motorInteract_.size() + motorEat_.size() +
                                      motorDrink_.size() + motorRest_.size() + motorWait_.size();
                        
                        switch (idx % 12) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorLookLeft_.push_back(n); break;
                            case 5: motorLookRight_.push_back(n); break;
                            case 6: motorInteract_.push_back(n); break;
                            case 7: motorWait_.push_back(n); break;
                            case 8: motorEat_.push_back(n); break;
                            case 9: motorDrink_.push_back(n); break;
                            case 10: motorRest_.push_back(n); break;
                            case 11: // Custom - use for Wait or any extra motor neurons
                                // Custom command gets any leftover motor neurons
                                motorWait_.push_back(n); break;
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
    lastSelectedAction_ = static_cast<size_t>(MotorCommand::Wait);
    actionEntropy_ = 0.0f;
    metaCuriosityLevel_ = 0.0f;
    lastActionTime_ = 0.0;
    
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
    return 12;
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
        
        // Update meta-curiosity (curiosity about curiosity)
        if (config_.enableMetaCuriosity_) {
            metaCuriosityLevel_ = curiosityLevel_ * 0.3f + metaCuriosityLevel_ * 0.7f;
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > config_.curiosityThreshold_) {
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
    float lookLeftAct = calcActivity(motorLookLeft_);
    float lookRightAct = calcActivity(motorLookRight_);
    float interactAct = calcActivity(motorInteract_);
    float eatAct = calcActivity(motorEat_);
    float drinkAct = calcActivity(motorDrink_);
    float restAct = calcActivity(motorRest_);
    float waitAct = calcActivity(motorWait_);
    
    // Advanced action selection with temperature-based exploration
    struct CommandWithActivity {
        MotorCommand cmd;
        float activity;
        float preference; // Optional preference from external sources
    };
    
    std::vector<CommandWithActivity> commands = {
        {MotorCommand::MoveForward, forwardAct, 1.0f},
        {MotorCommand::MoveBackward, backwardAct, 1.0f},
        {MotorCommand::TurnLeft, leftAct, 1.0f},
        {MotorCommand::TurnRight, rightAct, 1.0f},
        {MotorCommand::LookLeft, lookLeftAct, 1.0f},
        {MotorCommand::LookRight, lookRightAct, 1.0f},
        {MotorCommand::Interact, interactAct, 1.0f},
        {MotorCommand::Eat, eatAct, 1.0f},
        {MotorCommand::Drink, drinkAct, 1.0f},
        {MotorCommand::Rest, restAct, 1.0f},
        {MotorCommand::Wait, waitAct, 1.0f}
    };
    
    // Apply temperature-based softmax action selection for exploration
    if (config_.enableActionSelection_ && config_.actionSelectionTemperature_ > 0.0f) {
        // Calculate softmax probabilities
        std::vector<float> expScores;
        float maxScore = 0.0f;
        
        for (const auto& cmd : commands) {
            float score = cmd.activity * config_.actionSelectionTemperature_;
            expScores.push_back(std::exp(score));
            if (score > maxScore) maxScore = score;
        }
        
        // Normalize probabilities
        float sumExp = 0.0f;
        for (float expScore : expScores) {
            sumExp += expScore;
        }
        
        if (sumExp > 0.0f) {
            std::vector<float> probabilities;
            for (float expScore : expScores) {
                probabilities.push_back(expScore / sumExp);
            }
            
            // Sample action based on probabilities
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            float cumulative = 0.0f;
            for (size_t i = 0; i < commands.size(); ++i) {
                cumulative += probabilities[i];
                if (r <= cumulative) {
                    lastSelectedAction_ = i;
                    actionEntropy_ = -std::log(probabilities[i]) + 0.001f; // Add small epsilon to avoid log(0)
                    return commands[i].cmd;
                }
            }
        }
    }
    
    // Fallback: traditional maximum activity selection
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& cmd : commands) {
        if (cmd.activity > bestActivity) {
            bestActivity = cmd.activity;
            best = cmd.cmd;
            lastSelectedAction_ = &cmd - commands.data();
        }
    }
    
    // Update action entropy based on decision certainty
    if (bestActivity > 0.1f) {
        float certainty = bestActivity / (bestActivity + 0.1f); // Sigmoid-like function
        actionEntropy_ = -certainty * std::log(certainty) - (1.0f - certainty) * std::log(1.0f - certainty + 0.001f);
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < config_.noveltyThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Meta-curiosity: consider whether we should explore more
    float curiosityFactor = 1.0f;
    if (config_.enableMetaCuriosity_) {
        curiosityFactor = std::min(2.0f, 1.0f + metaCuriosityLevel_);
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > config_.curiosityThreshold_ * curiosityFactor) {
        // Higher curiosity = more exploration
        float exploreChance = std::min(config_.explorationRate_ * curiosityFactor, 0.8f);  // Cap at 80%
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - sample from all available actions
            int choice = brain_->getRandomGenerator()->uniformInt(0, static_cast<int>(MotorCommand::Custom));
            switch (static_cast<MotorCommand>(choice)) {
                case MotorCommand::MoveForward: return MotorCommand::MoveForward;
                case MotorCommand::MoveBackward: return MotorCommand::MoveBackward;
                case MotorCommand::TurnLeft: return MotorCommand::TurnLeft;
                case MotorCommand::TurnRight: return MotorCommand::TurnRight;
                case MotorCommand::LookLeft: return MotorCommand::LookLeft;
                case MotorCommand::LookRight: return MotorCommand::LookRight;
                case MotorCommand::Interact: return MotorCommand::Interact;
                case MotorCommand::Eat: return MotorCommand::Eat;
                case MotorCommand::Drink: return MotorCommand::Drink;
                case MotorCommand::Rest: return MotorCommand::Rest;
                case MotorCommand::Wait: return MotorCommand::Wait;
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
    
    // Apply developmental time scaling
    double scaledTimestep = timestep * config_.developmentalTimeScale_;
    
    developmentalAge_ += scaledTimestep;
    
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
    } else if (developmentalAge_ < 1800.0) {  // ~30 minutes
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    } else {
        plasticityModifier_ = 0.1f;  // Aging - reduced plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Aging);
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
    lastSelectedAction_ = static_cast<size_t>(MotorCommand::Wait);
    actionEntropy_ = 0.0f;
    metaCuriosityLevel_ = 0.0f;
    lastActionTime_ = 0.0;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
