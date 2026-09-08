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
    NLM_LOG_DEBUG("Initializing AgentBrain with " + 
                 std::to_string(brain_ ? 1 : 0) + " brain connection");
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        // Log initial brain state
        size_t totalPopulations = 0;
        size_t totalNeurons = 0;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                ++totalPopulations;
                const auto& neurons = pop->getNeurons();
                totalNeurons += neurons.size();
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : neurons) {
                        if (!n) {
                            NLM_LOG_WARNING("Skipping null motor neuron in population");
                            continue;
                        }
                        
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
                    for (Neuron* n : neurons) {
                        if (!n) {
                            NLM_LOG_WARNING("Skipping null sensory neuron in population");
                            continue;
                        }
                        
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
        
        NLM_LOG_INFO("AgentBrain initialization completed: " +
                     std::to_string(totalPopulations) + " populations, " +
                     std::to_string(totalNeurons) + " neurons, " +
                     std::to_string(motorForward_.size() + motorBackward_.size() +
                                   motorTurnLeft_.size() + motorTurnRight_.size() +
                                   motorInteract_.size() + motorWait_.size()) + " motor neurons, " +
                     std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                                   sensoryInternal_.size() + sensoryProprioception_.size()) + " sensory neurons");
    } else {
        NLM_LOG_ERROR("AgentBrain initialized with null brain pointer");
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
    // Robust null checks for brain and percept
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: brain is null");
        return;
    }
    
    // Check if percept is valid by verifying its components
    // Vision data is essential, so we check for that specifically
    const auto& vision = percept.getVision();
    if (vision.empty()) {
        NLM_LOG_WARNING("Cannot process sensory input: vision data is empty");
        return;
    }
    
    NLM_LOG_DEBUG("Processing sensory input: vision size = " + std::to_string(vision.size()) +
                 ", vision max = " + std::to_string(*std::max_element(vision.begin(), vision.end())));
    
    // Fix FiringState check in processSensoryInput
    // Replace the manual state check with the proper enum comparison
    for (size_t i = 0; i < visionLimit; ++i) {
        if (sensoryVision_[i] && 
            sensoryVision_[i]->getState().firingState != FiringState::Refractory) {
            // Inject current proportional to vision intensity
            // Use non-linear scaling to emphasize strong stimuli
            float intensity = vision[i];
            float current = intensity * intensity * 5.0f;  // Non-linear scaling
            
            sensoryVision_[i]->injectCurrent(current);
            ++visionNeuronsProcessed;
            
            // Log occasional activity for debugging
            if (visionNeuronsProcessed % 20 == 0) {
                NLM_LOG_DEBUG("Injecting current into vision neuron " + std::to_string(i) +
                             ", intensity = " + std::to_string(intensity) +
                             ", current = " + std::to_string(current));
            }
        } else if (sensoryVision_[i]) {
            NLM_LOG_DEBUG("Skipping vision neuron in refractory state at index " + std::to_string(i));
        } else {
            NLM_LOG_DEBUG("Skipping null vision sensory neuron at index " + std::to_string(i));
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    if (!touch.empty()) {
        size_t touchLimit = std::min(sensoryTouch_.size(), touch.size());
        for (size_t i = 0; i < touchLimit; ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    if (!intern.empty()) {
        size_t internLimit = std::min(sensoryInternal_.size(), intern.size());
        for (size_t i = 0; i < internLimit; ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty()) {
        size_t proprioLimit = std::min(sensoryProprioception_.size(), proprio.size());
        for (size_t i = 0; i < proprioLimit; ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Enhanced novelty detection
    computeNoveltyLevel(vision);
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    NLM_LOG_DEBUG("Sensory input processed: " + std::to_string(visionNeuronsProcessed) +
                 " vision neurons, novelty = " + std::to_string(noveltyLevel_));
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
    // Calculate activity for the specified command
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return neurons.empty() ? 0.0f : sum / neurons.size();
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
    // Explore using multiple strategies based on curiosity level
    if (curiosityEnabled_ && curiosityLevel_ > 0.0f) {
        // Determine exploration mode based on curiosity level
        if (curiosityLevel_ > 0.7f) {
            // High curiosity: Adaptive temperature-based exploration
            return exploreWithTemperature(defaultCmd, curiosityLevel_);
        } else if (curiosityLevel_ > 0.4f) {
            // Medium curiosity: Novelty-driven exploration with bias toward unexplored actions
            return exploreByNovelty(defaultCmd);
        } else {
            // Low curiosity: Prediction error-driven exploration
            return exploreByPredictionError(defaultCmd);
        }
    }
    
    return defaultCmd;
}

// High curiosity: Adaptive temperature-based exploration using Boltzmann distribution
MotorCommand AgentBrain::exploreWithTemperature(MotorCommand defaultCmd, float temperature) {
    if (!brain_) return defaultCmd;
    
    // Calculate Boltzmann distribution over motor commands based on curiosity temperature
    std::vector<float> logits(6);
    float maxLogit = 0.0f;
    
    // Get current activity levels for all motor commands
    std::vector<float> activities = getMotorActivityLevels();
    
    // Apply temperature scaling and compute logits
    for (int i = 0; i < 6; ++i) {
        // Use scaled activity as preference score
        logits[i] = activities[i] * (1.0f + temperature);  // Higher temp = more random
        if (logits[i] > maxLogit) maxLogit = logits[i];
    }
    
    // Convert logits to probabilities using softmax with temperature
    std::vector<float> probabilities(6, 0.0f);
    float sum = 0.0f;
    for (int i = 0; i < 6; ++i) {
        probabilities[i] = std::exp((logits[i] - maxLogit) / temperature);
        sum += probabilities[i];
    }
    
    // Normalize probabilities
    for (int i = 0; i < 6; ++i) {
        probabilities[i] /= sum;
    }
    
    // Sample from Boltzmann distribution
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    float cumulative = 0.0f;
    for (int i = 0; i < 6; ++i) {
        cumulative += probabilities[i];
        if (r <= cumulative) {
            // Convert index back to MotorCommand
            switch (i) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                case 5: return MotorCommand::Wait;
            }
        }
    }
    
    // Fallback
    return defaultCmd;
}

// Medium curiosity: Novelty-driven exploration
MotorCommand AgentBrain::exploreByNovelty(MotorCommand defaultCmd) {
    if (!brain_) return defaultCmd;
    
    // Get current activity levels for all motor commands
    std::vector<float> activities = getMotorActivityLevels();
    
    // Calculate novelty scores for each command
    // Prefer commands with lower activity (less explored) or higher novelty value
    std::vector<float> noveltyScores(6, 0.0f);
    float noveltyFactor = 1.5f; // Emphasize novelty preference
    
    for (int i = 0; i < 6; ++i) {
        // Combine low activity preference with novelty level
        noveltyScores[i] = (1.0f - activities[i]) * noveltyFactor + 
                         (noveltyLevel_ * 0.5f); // Add small novelty bias
    }
    
    // Apply exploration bonus for low-frequency actions
    // Actions with activity < 0.2 are considered "unexplored"
    for (int i = 0; i < 6; ++i) {
        if (activities[i] < 0.2f) {
            noveltyScores[i] *= 2.0f; // Double chance for unexplored actions
        }
    }
    
    // Normalize scores
    float maxScore = 0.0f;
    for (float score : noveltyScores) {
        if (score > maxScore) maxScore = score;
    }
    
    // Probabilistic selection based on novelty scores
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    float cumulative = 0.0f;
    for (int i = 0; i < 6; ++i) {
        float probability = noveltyScores[i] / maxScore;
        cumulative += probability;
        if (r <= cumulative) {
            switch (i) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                case 5: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

// Low curiosity: Prediction error-driven exploration
MotorCommand AgentBrain::exploreByPredictionError(MotorCommand defaultCmd) {
    if (!brain_) return defaultCmd;
    
    // Get current activity levels and prediction error
    std::vector<float> activities = getMotorActivityLevels();
    
    // Use prediction error to bias exploration
    // High prediction error = more exploration, low prediction error = exploitation
    float exploreBias = std::abs(predictionError_); // 0.0 to 1.0
    
    // Weighted activity scores with prediction error bias
    std::vector<float> weightedScores(6, 0.0f);
    for (int i = 0; i < 6; ++i) {
        // Lower activity gets higher score when exploring
        weightedScores[i] = (1.0f - activities[i]) * exploreBias * 2.0f + 
                          activities[i] * (1.0f - exploreBias);
    }
    
    // Normalize scores
    float maxScore = 0.0f;
    for (float score : weightedScores) {
        if (score > maxScore) maxScore = score;
    }
    
    // Probabilistic selection based on weighted scores
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    float cumulative = 0.0f;
    for (int i = 0; i < 6; ++i) {
        float probability = weightedScores[i] / maxScore;
        cumulative += probability;
        if (r <= cumulative) {
            switch (i) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                case 5: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // Safety checks for valid inputs and brain state
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Validate reward values
    if (std::isnan(reward) || std::isnan(predictedReward)) return;
    
    // Compute prediction error with improved stability
    predictionError_ = std::clamp(reward - predictedReward, -1.0f, 1.0f);
    
    // Update expected reward with exponential moving average (more stable)
    const float alpha = 0.05f; // Learning rate for reward estimation
    expectedReward_ = (1.0f - alpha) * expectedReward_ + alpha * reward;
    
    // Dopamine-like signal (based on prediction error) with bounded output
    dopamineLevel_ = std::clamp(predictionError_ * 2.0f, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;  // Safety check for null synapses
            
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

// Get novelty level
// Returns the current novelty detection signal
// @return Novelty level (0.0 to 1.0)
float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

// Get prediction error
// Returns the reward prediction error signal
// @return Prediction error (-1.0 to 1.0)
float AgentBrain::getPredictionError() const {
    return predictionError_;
}

// Get current action sequence (returns empty for now - requires NeuralPlanner integration)
std::vector<ActionType> AgentBrain::getActionSequence() const {
    // TODO: Integrate with NeuralPlanner to get actual action sequences
    // For now, return empty vector as this feature requires additional components
    return std::vector<ActionType>();
}

// Get motor activity level for specific motor command
float AgentBrain::getMotorActivityLevel(MotorCommand cmd) const {
    // Calculate activity for the specified command
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return neurons.empty() ? 0.0f : sum / neurons.size();
    };
    
    switch (cmd) {
        case MotorCommand::MoveForward: return calcActivity(motorForward_);
        case MotorCommand::MoveBackward: return calcActivity(motorBackward_);
        case MotorCommand::TurnLeft: return calcActivity(motorTurnLeft_);
        case MotorCommand::TurnRight: return calcActivity(motorTurnRight_);
        case MotorCommand::Interact: return calcActivity(motorInteract_);
        case MotorCommand::Wait: return calcActivity(motorWait_);
        default: return 0.0f;
    }
}

// Get motor activity levels for all commands
std::vector<float> AgentBrain::getMotorActivityLevels() const {
    std::vector<float> activities;
    activities.reserve(6);
    
    activities.push_back(getMotorActivityLevel(MotorCommand::MoveForward));
    activities.push_back(getMotorActivityLevel(MotorCommand::MoveBackward));
    activities.push_back(getMotorActivityLevel(MotorCommand::TurnLeft));
    activities.push_back(getMotorActivityLevel(MotorCommand::TurnRight));
    activities.push_back(getMotorActivityLevel(MotorCommand::Interact));
    activities.push_back(getMotorActivityLevel(MotorCommand::Wait));
    
    return activities;
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
