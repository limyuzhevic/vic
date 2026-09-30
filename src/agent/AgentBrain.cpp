#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "AgentBrainConstants.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

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
    , sensoryNoveltyDecay_(SENSORY_NOVELTY_DECAY)
{
    // Initialize motor and sensory neuron groups with better distribution logic
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    distributeMotorNeurons(pop->getNeurons());
                } else if (type == NeuronType::Sensory) {
                    distributeSensoryNeurons(pop->getNeurons());
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

// Performance optimizations for neuron distribution
void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    // Fast path: if all groups are empty, distribute evenly
    bool allEmpty = std::all_of(motorGroups_.begin(), motorGroups_.end(),
                               [](const std::vector<Neuron*>& g) { return g.empty(); });
    if (allEmpty && !neurons.empty()) {
        // Use reserve for performance
        size_t neuronsPerGroup = neurons.size() / motorGroups_.size();
        size_t remainder = neurons.size() % motorGroups_.size();
        
        size_t startIdx = 0;
        for (size_t i = 0; i < motorGroups_.size(); ++i) {
            size_t count = neuronsPerGroup + (i < remainder ? 1 : 0);
            if (count > 0) {
                motorGroups_[i].insert(motorGroups_[i].end(), 
                                       neurons.begin() + startIdx,
                                       neurons.begin() + startIdx + count);
                startIdx += count;
            }
        }
        return;
    }
    
    // Distribute remaining neurons using round-robin
    size_t currentGroup = 0;
    for (Neuron* n : neurons) {
        motorGroups_[currentGroup].push_back(n);
        currentGroup = (currentGroup + 1) % motorGroups_.size();
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    // Fast path: if all groups are empty, distribute evenly
    bool allEmpty = std::all_of(sensoryGroups_.begin(), sensoryGroups_.end(),
                               [](const std::vector<Neuron*>& g) { return g.empty(); });
    if (allEmpty && !neurons.empty()) {
        // Use reserve for performance
        size_t neuronsPerGroup = neurons.size() / sensoryGroups_.size();
        size_t remainder = neurons.size() % sensoryGroups_.size();
        
        size_t startIdx = 0;
        for (size_t i = 0; i < sensoryGroups_.size(); ++i) {
            size_t count = neuronsPerGroup + (i < remainder ? 1 : 0);
            if (count > 0) {
                sensoryGroups_[i].insert(sensoryGroups_[i].end(),
                                        neurons.begin() + startIdx,
                                        neurons.begin() + startIdx + count);
                startIdx += count;
            }
        }
        return;
    }
    
    // Distribute remaining neurons using round-robin
    size_t currentGroup = 0;
    for (Neuron* n : neurons) {
        sensoryGroups_[currentGroup].push_back(n);
        currentGroup = (currentGroup + 1) % sensoryGroups_.size();
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryGroups_[0].size()) + " vision sensory neurons, " +
                 std::to_string(sensoryGroups_[1].size()) + " touch sensory neurons, " +
                 std::to_string(sensoryGroups_[2].size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (DEFAULT_VISION_WIDTH x DEFAULT_VISION_HEIGHT) + touch (DEFAULT_TOUCH_SENSORS) + internal (DEFAULT_INTERNAL_SENSORS) + proprioception (DEFAULT_PROPRIOCEPTORS)
    return DEFAULT_VISION_WIDTH * DEFAULT_VISION_HEIGHT + DEFAULT_TOUCH_SENSORS + DEFAULT_INTERNAL_SENSORS + DEFAULT_PROPRIOCEPTORS;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return DEFAULT_MOTOR_COMMANDS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> vision sensory neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < vision.size() && i < sensoryGroups_[0].size(); ++i) {
        if (sensoryGroups_[0][i]) {
            float current = vision[i] * SENSORY_VISION_SCALE;
            sensoryGroups_[0][i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> touch sensory neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < touch.size() && i < sensoryGroups_[1].size(); ++i) {
        if (sensoryGroups_[1][i]) {
            float current = touch[i] * SENSORY_TOUCH_SCALE;
            sensoryGroups_[1][i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> internal sensory neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < intern.size() && i < sensoryGroups_[2].size(); ++i) {
        if (sensoryGroups_[2][i]) {
            float current = (intern[i] * SIGNAL_CENTER - SIGNAL_OFFSET) * SENSORY_INTERNAL_SCALE;
            sensoryGroups_[2][i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> proprioception sensory neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < proprio.size() && i < sensoryGroups_[3].size(); ++i) {
        if (sensoryGroups_[3][i]) {
            float current = (proprio[i] * SIGNAL_CENTER - SIGNAL_OFFSET) * SENSORY_PROPRIOCEPTION_SCALE;
            sensoryGroups_[3][i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        size_t compareSize = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < compareSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max(compareSize, size_t(1));
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * CURIOUSITY_NOVELTY_WEIGHT + 
                         std::abs(predictionError_) * CURIOUSITY_PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, MAX_CURIOUSITY_LEVEL);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > MAX_EXPLORATION_CHANCE) {
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
    
    // Map motor commands to their groups for activity calculation
    const std::vector<std::pair<MotorCommand, const std::vector<Neuron*>&>> motorCommandGroups = {
        {MotorCommand::MoveForward, motorGroups_[0]},
        {MotorCommand::MoveBackward, motorGroups_[1]},
        {MotorCommand::TurnLeft, motorGroups_[2]},
        {MotorCommand::TurnRight, motorGroups_[3]},
        {MotorCommand::Interact, motorGroups_[4]},
        {MotorCommand::Wait, motorGroups_[5]}
    };
    
    // Calculate activity for each motor command group
    std::vector<std::pair<MotorCommand, float>> commandActivities;
    for (const auto& cmdGroup : motorCommandGroups) {
        float activity = calcActivity(cmdGroup.second);
        commandActivities.emplace_back(cmdGroup.first, activity);
    }
    
    // Find maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;  // Default to wait if nothing stronger
    
    for (const auto& cmdActivity : commandActivities) {
        if (cmdActivity.second > bestActivity) {
            bestActivity = cmdActivity.second;
            best = cmdActivity.first;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > MAX_CURIOUSITY_EXPLORATION) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * MAX_EXPLORATION_CHANCE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, MOTOR_COMMAND_COUNT - 1);
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
    
    // Clamp to reasonable range using constants
    dopamineLevel_ = std::clamp(dopamineLevel_, NEUROMODULATION_CLAMP_MIN, NEUROMODULATION_CLAMP_MAX);
    
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
    plasticityFactor = std::clamp(plasticityFactor, PLASTICITY_MODIFIER_MIN, PLASTICITY_MODIFIER_MAX);
    
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
    if (developmentalAge_ < DEVELOPMENT_STAGE_INITIAL) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < DEVELOPMENT_STAGE_CRITICAL) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < DEVELOPMENT_STAGE_MATURATION) {  // ~15 minutes
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
            float synRate = INITIAL_SYNAPTOGENESIS_RATE * plasticityModifier_;
            float pruneRate = INITIAL_PRUNING_RATE * (PLASTICITY_MODIFIER_MAX - plasticityModifier_);
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
