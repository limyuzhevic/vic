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
    , sensoryNoveltyDecay_(DEFAULT_NOVELTY_DECAY)
{
    // Initialize motor and sensory neuron groups using improved structure
    distributeMotorNeurons();
    distributeSensoryNeurons();
}

void AgentBrain::distributeMotorNeurons() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute motor neurons using array indexing for better organization
                    size_t totalUsed = 0;
                    for (size_t i = 0; i < NUM_MOTOR_ACTIONS; ++i) {
                        totalUsed += motorGroups_[i].size();
                    }
                    
                    size_t idx = totalUsed % NUM_MOTOR_ACTIONS;
                    motorGroups_[idx].push_back(n);
                }
            }
        }
    }
}

void AgentBrain::distributeSensoryNeurons() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute sensory neurons using array indexing
                    size_t totalUsed = 0;
                    for (size_t i = 0; i < NUM_SENSORY_MODALITIES; ++i) {
                        totalUsed += sensoryGroups_[i].size();
                    }
                    
                    size_t idx = totalUsed % NUM_SENSORY_MODALITIES;
                    sensoryGroups_[idx].push_back(n);
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

AgentBrain::~AgentBrain() = default;

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
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return NUM_MOTOR_ACTIONS;
}

float AgentBrain::calculateAverageActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand AgentBrain::findCommandWithMaxActivity(const std::array<MotorCommand, NUM_MOTOR_ACTIONS>& commands, 
                                                   const std::array<float, NUM_MOTOR_ACTIONS>& activities) const {
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = activities[0];  // Default to first command
    
    for (size_t i = 1; i < NUM_MOTOR_ACTIONS; ++i) {
        if (activities[i] > bestActivity) {
            bestActivity = activities[i];
            best = commands[i];
        }
    }
    
    return best;
}

float AgentBrain::computeNovelty(const std::vector<float>& currentVision) const {
    if (currentVision.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < currentVision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(currentVision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    return totalDiff / std::max<size_t>(currentVision.size(), 1);
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryGroups_[0] neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryGroups_[0].size() && i < vision.size(); ++i) {
        if (sensoryGroups_[0][i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryGroups_[0][i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryGroups_[1] neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryGroups_[1].size() && i < touch.size(); ++i) {
        if (sensoryGroups_[1][i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryGroups_[1][i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryGroups_[2] neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryGroups_[2].size() && i < intern.size(); ++i) {
        if (sensoryGroups_[2][i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryGroups_[2][i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryGroups_[3] neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryGroups_[3].size() && i < proprio.size(); ++i) {
        if (sensoryGroups_[3][i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryGroups_[3][i]->injectCurrent(current);
        }
    }
    
    // Compute novelty using helper function
    if (!vision.empty()) {
        noveltyLevel_ = computeNovelty(vision);
        
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
    if (curiosityEnabled_ && curiosityLevel_ > CURIOUS_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using helper function
    std::array<float, NUM_MOTOR_ACTIONS> activities;
    for (size_t i = 0; i < NUM_MOTOR_ACTIONS; ++i) {
        activities[i] = calculateAverageActivity(motorGroups_[i]);
    }
    
    return findCommandWithMaxActivity(MOTOR_ACTIONS, activities);
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > HIGH_CURIOUS_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * EXPLORATION_MAX_CHANCE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 8);  // Extended action space
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;  // Added for advanced users
                case 5: return MotorCommand::LookRight;  // Added for advanced users
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

// Advanced configuration methods implementation
void AgentBrain::setMotorNeuronDistribution(const std::array<size_t, NUM_MOTOR_ACTIONS>& distribution) {
    // Clear existing motor groups and redistribute based on new distribution
    motorGroups_.fill(std::vector<Neuron*>());
    
    if (!brain_) return;
    
    size_t idx = 0;
    for (size_t count : distribution) {
        auto& motorGroup = motorGroups_[idx];
        motorGroup.reserve(count);
        
        // Distribute from brain regions - simplified implementation
        if (idx < NUM_MOTOR_ACTIONS) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    NeuronType type = pop->getNeuronType();
                    if (type == NeuronType::Motor) {
                        size_t toAdd = std::min(count, pop->getNeurons().size());
                        for (size_t j = 0; j < toAdd; ++j) {
                            motorGroup.push_back(pop->getNeurons()[j]);
                        }
                        count -= toAdd;
                        if (count == 0) break;
                    }
                }
                if (count == 0) break;
            }
        }
        idx++;
    }
}

void AgentBrain::setSensoryNeuronDistribution(const std::array<size_t, NUM_SENSORY_MODALITIES>& distribution) {
    // Clear existing sensory groups and redistribute based on new distribution
    sensoryGroups_.fill(std::vector<Neuron*>());
    
    if (!brain_) return;
    
    size_t idx = 0;
    for (size_t count : distribution) {
        auto& sensoryGroup = sensoryGroups_[idx];
        sensoryGroup.reserve(count);
        
        // Distribute from brain regions - simplified implementation
        if (idx < NUM_SENSORY_MODALITIES) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    NeuronType type = pop->getNeuronType();
                    if (type == NeuronType::Sensory) {
                        size_t toAdd = std::min(count, pop->getNeurons().size());
                        for (size_t j = 0; j < toAdd; ++j) {
                            sensoryGroup.push_back(pop->getNeurons()[j]);
                        }
                        count -= toAdd;
                        if (count == 0) break;
                    }
                }
                if (count == 0) break;
            }
        }
        idx++;
    }
}

const std::vector<Neuron*>& AgentBrain::getMotorNeurons(MotorCommand action) const {
    size_t idx = 0;
    for (; idx < NUM_MOTOR_ACTIONS; ++idx) {
        if (MOTOR_ACTIONS[idx] == action) {
            break;
        }
    }
    return motorGroups_[idx];
}

const std::vector<Neuron*>& AgentBrain::getSensoryNeurons(size_t modality) const {
    if (modality >= NUM_SENSORY_MODALITIES) {
        static const std::vector<Neuron*> empty;
        return empty;
    }
    return sensoryGroups_[modality];
}

size_t AgentBrain::getMotorNeuronCount() const {
    size_t total = 0;
    for (const auto& group : motorGroups_) {
        total += group.size();
    }
    return total;
}

size_t AgentBrain::getSensoryNeuronCount() const {
    size_t total = 0;
    for (const auto& group : sensoryGroups_) {
        total += group.size();
    }
    return total;
}

} // namespace nlm
