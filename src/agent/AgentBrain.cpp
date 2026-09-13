#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

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
    , motorDistributionStrategy_(DistributionStrategy::Random)
    , sensoryDistributionStrategy_(DistributionStrategy::Random)
    , rng_(std::mt19937(std::random_device{}()))
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        distributeNeurons();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Re-distribute neurons based on current strategy
    distributeNeurons();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + 
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorInteract_.size() + motorWait_.size()) + " motor neurons");
}

void AgentBrain::distributeNeurons() {
    // Clear existing distributions
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    if (!brain_) return;
    
    // Count neurons available for each type
    size_t motorCount = 0;
    size_t sensoryCount = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) {
                motorCount += pop->getTotalNeuronCount();
            } else if (type == NeuronType::Sensory) {
                sensoryCount += pop->getTotalNeuronCount();
            }
        }
    }
    
    // Distribute motor neurons based on strategy
    if (motorCount > 0) {
        distributeMotorNeurons(motorCount);
    }
    
    // Distribute sensory neurons based on strategy
    if (sensoryCount > 0) {
        distributeSensoryNeurons(sensoryCount);
    }
}

void AgentBrain::distributeMotorNeurons(size_t totalMotorNeurons) {
    if (totalMotorNeurons == 0) return;
    
    switch (motorDistributionStrategy_) {
        case DistributionStrategy::Random:
            distributeMotorRandom(totalMotorNeurons);
            break;
        case DistributionStrategy::Balanced:
            distributeMotorBalanced(totalMotorNeurons);
            break;
        case DistributionStrategy::Learned:
            distributeMotorLearned(totalMotorNeurons);
            break;
        case DistributionStrategy::Adaptive:
            distributeMotorAdaptive(totalMotorNeurons);
            break;
    }
}

void AgentBrain::distributeMotorRandom(size_t totalMotorNeurons) {
    // Collect all motor neurons first
    std::vector<Neuron*> allMotorNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    allMotorNeurons.push_back(n);
                }
            }
        }
    }
    
    if (allMotorNeurons.empty()) return;
    
    // Shuffle and distribute evenly across action groups
    std::shuffle(allMotorNeurons.begin(), allMotorNeurons.end(), rng_);
    
    size_t neuronsPerAction = allMotorNeurons.size() / 6;
    size_t remainder = allMotorNeurons.size() % 6;
    
    size_t idx = 0;
    for (size_t i = 0; i < 6; ++i) {
        size_t targetCount = neuronsPerAction + (i < remainder ? 1 : 0);
        for (size_t j = 0; j < targetCount; ++j) {
            if (idx < allMotorNeurons.size()) {
                switch (i) {
                    case 0: motorForward_.push_back(allMotorNeurons[idx]); break;
                    case 1: motorBackward_.push_back(allMotorNeurons[idx]); break;
                    case 2: motorTurnLeft_.push_back(allMotorNeurons[idx]); break;
                    case 3: motorTurnRight_.push_back(allMotorNeurons[idx]); break;
                    case 4: motorInteract_.push_back(allMotorNeurons[idx]); break;
                    case 5: motorWait_.push_back(allMotorNeurons[idx]); break;
                }
                ++idx;
            }
        }
    }
}

void AgentBrain::distributeMotorBalanced(size_t totalMotorNeurons) {
    // Distribute evenly with emphasis on forward/backward (navigation)
    double forwardWeight = 0.25, backwardWeight = 0.25, leftWeight = 0.20, rightWeight = 0.20,
           interactWeight = 0.05, waitWeight = 0.05;
    
    distributeMotorWeighted(totalMotorNeurons, forwardWeight, backwardWeight, leftWeight,
                           rightWeight, interactWeight, waitWeight);
}

void AgentBrain::distributeMotorLearned(size_t totalMotorNeurons) {
    // Based on previous experience if available
    double forwardWeight = 0.3, backwardWeight = 0.2, leftWeight = 0.15, rightWeight = 0.15,
           interactWeight = 0.1, waitWeight = 0.1;
    
    // Can be modified by curiosity levels, past experience, etc.
    if (curiosityLevel_ > 0.7f) {
        // High curiosity -> more exploration
        forwardWeight -= 0.1;
        interactWeight += 0.1;
    }
    
    if (dopamineLevel_ > 0.5f) {
        // Positive reward -> strengthen goal-directed behavior
        forwardWeight += 0.15;
        backwardWeight += 0.05;
        waitWeight -= 0.05;
    }
    
    distributeMotorWeighted(totalMotorNeurons, forwardWeight, backwardWeight, leftWeight,
                           rightWeight, interactWeight, waitWeight);
}

void AgentBrain::distributeMotorAdaptive(size_t totalMotorNeurons) {
    // Dynamic based on current state and goals
    double forwardWeight = 0.2, backwardWeight = 0.15, leftWeight = 0.15, rightWeight = 0.15,
           interactWeight = 0.15, waitWeight = 0.2;
    
    // Adapt based on novelty - higher novelty encourages exploration
    if (noveltyLevel_ > 0.5f) {
        // Novel environment - explore more
        forwardWeight -= 0.1;
        interactWeight += 0.1;
    }
    
    // Adapt based on prediction error
    if (std::abs(predictionError_) > 0.5f) {
        // High prediction error - try new actions
        leftWeight += 0.1;
        rightWeight += 0.1;
        waitWeight -= 0.1;
    }
    
    distributeMotorWeighted(totalMotorNeurons, forwardWeight, backwardWeight, leftWeight,
                           rightWeight, interactWeight, waitWeight);
}

void AgentBrain::distributeMotorWeighted(size_t totalMotorNeurons, 
                                         double forwardWeight, double backwardWeight,
                                         double leftWeight, double rightWeight,
                                         double interactWeight, double waitWeight) {
    // Collect all motor neurons
    std::vector<Neuron*> allMotorNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    allMotorNeurons.push_back(n);
                }
            }
        }
    }
    
    if (allMotorNeurons.empty()) return;
    
    // Shuffle
    std::shuffle(allMotorNeurons.begin(), allMotorNeurons.end(), rng_);
    
    // Calculate target sizes based on weights
    size_t forwardTarget = static_cast<size_t>(totalMotorNeurons * forwardWeight);
    size_t backwardTarget = static_cast<size_t>(totalMotorNeurons * backwardWeight);
    size_t leftTarget = static_cast<size_t>(totalMotorNeurons * leftWeight);
    size_t rightTarget = static_cast<size_t>(totalMotorNeurons * rightWeight);
    size_t interactTarget = static_cast<size_t>(totalMotorNeurons * interactWeight);
    size_t waitTarget = totalMotorNeurons - (forwardTarget + backwardTarget + leftTarget + 
                                           rightTarget + interactTarget);
    
    size_t idx = 0;
    
    // Distribute to forward group
    for (size_t i = 0; i < forwardTarget && idx < allMotorNeurons.size(); ++i) {
        motorForward_.push_back(allMotorNeurons[idx++]);
    }
    
    // Distribute to backward group
    for (size_t i = 0; i < backwardTarget && idx < allMotorNeurons.size(); ++i) {
        motorBackward_.push_back(allMotorNeurons[idx++]);
    }
    
    // Distribute to left group
    for (size_t i = 0; i < leftTarget && idx < allMotorNeurons.size(); ++i) {
        motorTurnLeft_.push_back(allMotorNeurons[idx++]);
    }
    
    // Distribute to right group
    for (size_t i = 0; i < rightTarget && idx < allMotorNeurons.size(); ++i) {
        motorTurnRight_.push_back(allMotorNeurons[idx++]);
    }
    
    // Distribute to interact group
    for (size_t i = 0; i < interactTarget && idx < allMotorNeurons.size(); ++i) {
        motorInteract_.push_back(allMotorNeurons[idx++]);
    }
    
    // Distribute remaining to wait group
    for (size_t i = 0; i < waitTarget && idx < allMotorNeurons.size(); ++i) {
        motorWait_.push_back(allMotorNeurons[idx++]);
    }
}

void AgentBrain::distributeSensoryNeurons(size_t totalSensoryNeurons) {
    if (totalSensoryNeurons == 0) return;
    
    switch (sensoryDistributionStrategy_) {
        case DistributionStrategy::Random:
            distributeSensoryRandom(totalSensoryNeurons);
            break;
        case DistributionStrategy::Organized:
            distributeSensoryOrganized(totalSensoryNeurons);
            break;
        case DistributionStrategy::ExperienceBased:
            distributeSensoryExperienceBased(totalSensoryNeurons);
            break;
        case DistributionStrategy::Adaptive:
            distributeSensoryAdaptive(totalSensoryNeurons);
            break;
    }
}

void AgentBrain::distributeSensoryRandom(size_t totalSensoryNeurons) {
    std::vector<Neuron*> allSensoryNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    allSensoryNeurons.push_back(n);
                }
            }
        }
    }
    
    if (allSensoryNeurons.empty()) return;
    
    std::shuffle(allSensoryNeurons.begin(), allSensoryNeurons.end(), rng_);
    
    size_t visionTarget = totalSensoryNeurons / 4;
    size_t touchTarget = totalSensoryNeurons / 4;
    size_t internalTarget = totalSensoryNeurons / 4;
    size_t proprioceptionTarget = totalSensoryNeurons - (visionTarget + touchTarget + internalTarget);
    
    size_t idx = 0;
    
    for (size_t i = 0; i < visionTarget && idx < allSensoryNeurons.size(); ++i) {
        sensoryVision_.push_back(allSensoryNeurons[idx++]);
    }
    
    for (size_t i = 0; i < touchTarget && idx < allSensoryNeurons.size(); ++i) {
        sensoryTouch_.push_back(allSensoryNeurons[idx++]);
    }
    
    for (size_t i = 0; i < internalTarget && idx < allSensoryNeurons.size(); ++i) {
        sensoryInternal_.push_back(allSensoryNeurons[idx++]);
    }
    
    for (size_t i = 0; i < proprioceptionTarget && idx < allSensoryNeurons.size(); ++i) {
        sensoryProprioception_.push_back(allSensoryNeurons[idx++]);
    }
}

void AgentBrain::distributeSensoryOrganized(size_t totalSensoryNeurons) {
    // Organize by sensory modality: vision dominates for exploration
    double visionWeight = 0.5, touchWeight = 0.2, internalWeight = 0.15, proprioceptionWeight = 0.15;
    
    distributeSensoryWeighted(totalSensoryNeurons, visionWeight, touchWeight, internalWeight, proprioceptionWeight);
}

void AgentBrain::distributeSensoryExperienceBased(size_t totalSensoryNeurons) {
    // Based on which sensory inputs have been most useful
    double visionWeight = 0.3, touchWeight = 0.3, internalWeight = 0.2, proprioceptionWeight = 0.2;
    
    // Adjust based on novelty - if environment is new, rely more on vision
    if (noveltyLevel_ > 0.5f) {
        visionWeight += 0.2;
        touchWeight -= 0.1;
    }
    
    // Adjust based on curiosity - high curiosity encourages exploration
    if (curiosityLevel_ > 0.7f) {
        visionWeight += 0.15;
        proprioceptionWeight += 0.1;
    }
    
    distributeSensoryWeighted(totalSensoryNeurons, visionWeight, touchWeight, internalWeight, proprioceptionWeight);
}

void AgentBrain::distributeSensoryAdaptive(size_t totalSensoryNeurons) {
    // Dynamically adjust based on current needs
    double visionWeight = 0.4, touchWeight = 0.2, internalWeight = 0.2, proprioceptionWeight = 0.2;
    
    // If prediction error is high, rely more on internal signals for prediction
    if (std::abs(predictionError_) > 0.5f) {
        internalWeight += 0.2;
        visionWeight -= 0.1;
    }
    
    // If dopamine is high (reward), rely more on touch for object interaction
    if (dopamineLevel_ > 0.5f) {
        touchWeight += 0.15;
        internalWeight -= 0.1;
    }
    
    distributeSensoryWeighted(totalSensoryNeurons, visionWeight, touchWeight, internalWeight, proprioceptionWeight);
}

void AgentBrain::distributeSensoryWeighted(size_t totalSensoryNeurons,
                                         double visionWeight, double touchWeight,
                                         double internalWeight, double proprioceptionWeight) {
    std::vector<Neuron*> allSensoryNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    allSensoryNeurons.push_back(n);
                }
            }
        }
    }
    
    if (allSensoryNeurons.empty()) return;
    
    std::shuffle(allSensoryNeurons.begin(), allSensoryNeurons.end(), rng_);
    
    size_t visionTarget = static_cast<size_t>(totalSensoryNeurons * visionWeight);
    size_t touchTarget = static_cast<size_t>(totalSensoryNeurons * touchWeight);
    size_t internalTarget = static_cast<size_t>(totalSensoryNeurons * internalWeight);
    size_t proprioceptionTarget = totalSensoryNeurons - (visionTarget + touchTarget + internalTarget);
    
    size_t idx = 0;
    
    for (size_t i = 0; i < visionTarget && idx < allSensoryNeurons.size(); ++i) {
        sensoryVision_.push_back(allSensoryNeurons[idx++]);
    }
    
    for (size_t i = 0; i < touchTarget && idx < allSensoryNeurons.size(); ++i) {
        sensoryTouch_.push_back(allSensoryNeurons[idx++]);
    }
    
    for (size_t i = 0; i < internalTarget && idx < allSensoryNeurons.size(); ++i) {
        sensoryInternal_.push_back(allSensoryNeurons[idx++]);
    }
    
    for (size_t i = 0; i < proprioceptionTarget && idx < allSensoryNeurons.size(); ++i) {
        sensoryProprioception_.push_back(allSensoryNeurons[idx++]);
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

} // namespace nlm
