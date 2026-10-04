#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Helper functions
namespace {
    // Calculate average neural activity for a group of neurons
    float calculateNeuralActivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    }
    
    // Scale and center sensor value
    float scaleAndCenter(float value, float scale, float centerOffset = 1.0f) {
        return (value * 2.0f - centerOffset) * scale;
    }
    
    // Compute novelty as difference between current and previous vision
    float computeNovelty(const std::vector<float>& vision, std::vector<float>& previousVision, 
                       float decayRate) {
        if (vision.empty()) return 0.0f;
        
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision[i]);
            totalDiff += diff;
        }
        
        float novelty = totalDiff / std::max<size_t>(vision.size(), 1);
        return novelty * decayRate;
    }
}

// AgentBrain implementation with improved structure and memory safety
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
    // Initialize motor and sensory neuron groups with proper validation
    initializeNeuronGroups();
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
    // Use constants for better maintainability
    return AgentBrainConstants::VISION_NEUTRAL_SIZE +
           AgentBrainConstants::TOUCH_INPUT_COUNT +
           AgentBrainConstants::INTERNAL_INPUT_COUNT +
           AgentBrainConstants::PROPRIOCEPTION_INPUT_COUNT;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Motor output size is based on available motor groups
    return AgentBrainConstants::MOTOR_GROUP_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    processVisionInput(percept.getVision());
    processTouchInput(percept.getTouch());
    processInternalInput(percept.getInternal());
    processProprioceptionInput(percept.getProprioception());
    
    // Compute novelty using helper function
    noveltyLevel_ = computeNovelty(percept.getVision(), previousVision_, sensoryNoveltyDecay_);
    
    // Update curiosity based on novelty
    updateCuriosity();
    
    // Store vision for next novelty computation
    previousVision_ = percept.getVision();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration if enabled
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::CURIOSITY_EXPLORATION_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward using exponential moving average
    expectedReward_ = AgentBrainConstants::EXPECTED_REWARD_DECAY * expectedReward_ +
                     AgentBrainConstants::REWARD_LEARNING_RATE * reward;
    
    // Compute dopamine-like signal based on prediction error
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, 
                               AgentBrainConstants::NEUROMODULATION_CLAMP_MIN,
                               AgentBrainConstants::NEUROMODULATION_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    applyRewardToSynapses();
    
    // Modulate plasticity based on dopamine
    modulatePlasticity();
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Update developmental stage and plasticity
    updateDevelopmentalStage();
    
    // Update structural plasticity if enabled
    if (structuralPlasticityEnabled_) {
        updateStructuralPlasticity();
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

// Private helper methods
void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
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

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        if (!n) continue;
        
        size_t idx = motorForward_.size() + motorBackward_.size() + 
                    motorTurnLeft_.size() + motorTurnRight_.size() +
                    motorInteract_.size() + motorWait_.size();
        
        switch (idx % AgentBrainConstants::MOTOR_GROUP_COUNT) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
            default: break;
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        if (!n) continue;
        
        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                    sensoryInternal_.size() + sensoryProprioception_.size();
        
        switch (idx % 4) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
            default: break;
        }
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    const auto& visionInput = vision;
    for (size_t i = 0; i < sensoryVision_.size() && i < visionInput.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = visionInput[i] * AgentBrainConstants::VISION_INPUT_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * AgentBrainConstants::TOUCH_INPUT_SCALE;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = scaleAndCenter(internal[i], AgentBrainConstants::INTERNAL_INPUT_SCALE);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = scaleAndCenter(proprio[i], AgentBrainConstants::PROPRIOCEPTION_INPUT_SCALE);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::updateCuriosity() {
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * AgentBrainConstants::CURIOSITY_NOVELTY_WEIGHT +
                         std::abs(predictionError_) * AgentBrainConstants::CURIOSITY_PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

void AgentBrain::applyRewardToSynapses() {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > AgentBrainConstants::PREDICTION_ERROR_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(AgentBrainConstants::ELIGIBILITY_DECAY_RATE);
            }
        }
    }
}

void AgentBrain::modulatePlasticity() {
    // Compute plasticity factor based on dopamine
    float plasticityFactor = AgentBrainConstants::PLASTICITY_MODULATION_FACTOR +
                             AgentBrainConstants::PLASTICITY_MODULATION_FACTOR * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(AgentBrainConstants::STDP_LTP_WEIGHT_BASE * plasticityFactor);
        stdp->setLTDWeight(AgentBrainConstants::STDP_LTD_WEIGHT_BASE * plasticityFactor);
    }
}

void AgentBrain::updateDevelopmentalStage() {
    // Simple developmental stages based on age
    if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_INITIAL) {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_INITIAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_CRITICAL) {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_CRITICAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_MATURE) {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_MATURE;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_ADULT;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

void AgentBrain::updateStructuralPlasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Higher synaptogenesis in early development
        float synRate = AgentBrainConstants::SYNAPTOGENESIS_RATE_BASE * plasticityModifier_;
        float pruneRate = AgentBrainConstants::PRUNING_RATE_BASE * (2.0f - plasticityModifier_);
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using helper function
    float forwardAct = calculateNeuralActivity(motorForward_);
    float backwardAct = calculateNeuralActivity(motorBackward_);
    float leftAct = calculateNeuralActivity(motorTurnLeft_);
    float rightAct = calculateNeuralActivity(motorTurnRight_);
    float interactAct = calculateNeuralActivity(motorInteract_);
    float waitAct = calculateNeuralActivity(motorWait_);
    
    // Create motor command structure for easy comparison
    struct MotorCommandData {
        MotorCommand cmd;
        float activity;
    };
    
    MotorCommandData commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct; // Default to wait if nothing stronger
    
    for (const auto& cmdData : commands) {
        if (cmdData.activity > bestActivity) {
            bestActivity = cmdData.activity;
            best = cmdData.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < AgentBrainConstants::MOTOR_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConstants::CURIOSITY_EXPLORATION_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConstants::CURIOSITY_EXPLORATION_CHANCE_MAX;
        
        // Use a secure random number generator
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float r = dist(rng);
        
        if (r < exploreChance) {
            // Random motor command
            std::uniform_int_distribution<int> intDist(0, AgentBrainConstants::MOTOR_GROUP_COUNT - 1);
            int choice = intDist(rng);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Jump;
                case 8: return MotorCommand::Crouch;
                case 9: return MotorCommand::Grab;
                case 10: return MotorCommand::Throw;
                case 11: return MotorCommand::Eat;
                case 12: return MotorCommand::Speak;
                case 13: return MotorCommand::UseItem;
                case 14: return MotorCommand::Wait;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

} // namespace nlm
