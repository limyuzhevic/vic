#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Helper function to calculate activity with caching
static float calculateNeuronActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        if (n) { // Null pointer check
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
    }
    return sum / neurons.size();
}

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
        // Use neuron groups from brain regions
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue; // Null pointer check
                        
                        // Distribute motor neurons to different action groups
                        // Use modulo with total motor groups for balanced distribution
                        size_t groupIndex = motorForward_.size() + motorBackward_.size() + 
                                          motorTurnLeft_.size() + motorTurnRight_.size() +
                                          motorInteract_.size() + motorWait_.size() +
                                          motorLookLeft_.size() + motorLookRight_.size();
                        
                        switch (groupIndex % AgentConstants::TOTAL_MOTOR_GROUPS) {
                            case AgentConstants::MOTOR_FORWARD:
                                motorForward_.push_back(n); 
                                break;
                            case AgentConstants::MOTOR_BACKWARD:
                                motorBackward_.push_back(n); 
                                break;
                            case AgentConstants::MOTOR_TURN_LEFT:
                                motorTurnLeft_.push_back(n); 
                                break;
                            case AgentConstants::MOTOR_TURN_RIGHT:
                                motorTurnRight_.push_back(n); 
                                break;
                            case AgentConstants::MOTOR_INTERACT:
                                motorInteract_.push_back(n); 
                                break;
                            case AgentConstants::MOTOR_WAIT:
                                motorWait_.push_back(n); 
                                break;
                            case AgentConstants::MOTOR_LOOK_LEFT:
                                motorLookLeft_.push_back(n); 
                                break;
                            case AgentConstants::MOTOR_LOOK_RIGHT:
                                motorLookRight_.push_back(n); 
                                break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue; // Null pointer check
                        
                        // Distribute sensory neurons
                        size_t groupIndex = sensoryVision_.size() + sensoryTouch_.size() +
                                          sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (groupIndex % AgentConstants::TOTAL_SENSORY_NEURONS) {
                            case 0: // Vision
                                sensoryVision_.push_back(n); 
                                break;
                            case 1: // Touch
                                sensoryTouch_.push_back(n); 
                                break;
                            case 2: // Internal
                                sensoryInternal_.push_back(n); 
                                break;
                            case 3: // Proprioception
                                sensoryProprioception_.push_back(n); 
                                break;
                        }
                    }
                }
            }
        }
    }
    
    // Validate distribution
    if (!validateParameters()) {
        NLM_LOG_WARNING("AgentBrain: Invalid neural distribution detected");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate world pointer
    if (!&world) {
        NLM_LOG_ERROR("AgentBrain: Invalid world pointer passed to initialize");
        return;
    }
    
    // Allocate vision buffer with proper validation
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    if (visionSize > 0) {
        previousVision_.resize(visionSize, 0.0f);
    }
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = AgentConstants::INITIAL_PLASTICITY;
    
    // Log initialization with actual neuron counts
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
}

bool AgentBrain::validateParameters() const {
    // Validate motor group distribution
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() + 
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorInteract_.size() + motorWait_.size() +
                              motorLookLeft_.size() + motorLookRight_.size();
    
    // Validate sensory group distribution
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() + 
                                sensoryInternal_.size() + sensoryProprioception_.size();
    
    // Check if all neurons are accounted for
    if (totalMotorNeurons == 0 && totalSensoryNeurons == 0) {
        NLM_LOG_WARNING("AgentBrain: No neurons found in brain regions");
        return false;
    }
    
    // Log distribution statistics
    NLM_LOG_INFO("AgentBrain distribution: " + 
                 std::to_string(totalMotorNeurons) + " motor neurons, " +
                 std::to_string(totalSensoryNeurons) + " sensory neurons");
    
    return true;
}

size_t AgentBrain::getSensoryInputSize() const {
    // Use actual distributed neurons, not hardcoded values
    return sensoryVision_.size() + AgentConstants::TOUCH_NEURONS + 
           AgentConstants::INTERNAL_NEURONS + AgentConstants::PROPRIOCEPTION_NEURONS;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Motor output should match MotorCommand enum (6 main actions + look left/right for exploration)
    return AgentConstants::TOTAL_MOTOR_GROUPS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Validate inputs
    if (!brain_ || percept.getVision().empty()) {
        return;
    }
    
    const auto& vision = percept.getVision();
    
    // Vision input (256 values -> sensoryVision_ neurons)
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * AgentConstants::VISION_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * AgentConstants::TOUCH_SCALE;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * AgentConstants::INTERNAL_SCALE;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * AgentConstants::PROPRIOCEPTION_SCALE;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision) - optimized version
    computeNovelty(vision);
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, AgentConstants::MAX_CURIOUSITY);
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty()) {
        noveltyLevel_ = AgentConstants::MIN_NOVELTY;
        return;
    }
    
    // Optimized novelty computation using single pass
    float totalDiff = 0.0f;
    
    // Only compare with previous vision if it exists
    if (!previousVision_.empty()) {
        // Use min of sizes to avoid out-of-bounds access
        size_t compareSize = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < compareSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
    }
    
    // Normalize using max possible difference (1.0) and actual number of elements
    size_t numElements = std::max<size_t>(vision.size(), 1);
    noveltyLevel_ = totalDiff / (numElements * AgentConstants::MAX_NOVELTY);
    
    // Clamp to valid range
    noveltyLevel_ = std::clamp(noveltyLevel_, AgentConstants::MIN_NOVELTY, AgentConstants::MAX_NOVELTY);
    
    // Apply decay and update
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time (atomic operation for thread safety)
    previousVision_ = vision;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentConstants::CURIOUSITY_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group - optimized with caching
    float forwardAct = calculateNeuronActivity(motorForward_);
    float backwardAct = calculateNeuronActivity(motorBackward_);
    float leftAct = calculateNeuronActivity(motorTurnLeft_);
    float rightAct = calculateNeuronActivity(motorTurnRight_);
    float interactAct = calculateNeuronActivity(motorInteract_);
    float waitAct = calculateNeuronActivity(motorWait_);
    float lookLeftAct = calculateNeuronActivity(motorLookLeft_);
    float lookRightAct = calculateNeuronActivity(motorLookRight_);
    
    // Create motor command array with all possible commands
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
        {MotorCommand::Wait, waitAct},
        {MotorCommand::LookLeft, lookLeftAct},
        {MotorCommand::LookRight, lookRightAct}
    };
    
    // Find maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < AgentConstants::MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentConstants::HIGH_CURIOUSITY_EXPLORATION) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentConstants::EXPLORATION_BASE_CHANCE;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - use proper range (0-7 for 8 MotorCommand values)
            int choice = brain_->getRandomGenerator()->uniformInt(0, AgentConstants::TOTAL_MOTOR_GROUPS - 1);
            switch (choice) {
                case AgentConstants::MOTOR_FORWARD:
                    return MotorCommand::MoveForward;
                case AgentConstants::MOTOR_BACKWARD:
                    return MotorCommand::MoveBackward;
                case AgentConstants::MOTOR_TURN_LEFT:
                    return MotorCommand::TurnLeft;
                case AgentConstants::MOTOR_TURN_RIGHT:
                    return MotorCommand::TurnRight;
                case AgentConstants::MOTOR_INTERACT:
                    return MotorCommand::Interact;
                case AgentConstants::MOTOR_WAIT:
                    return MotorCommand::Wait;
                case AgentConstants::MOTOR_LOOK_LEFT:
                    return MotorCommand::LookLeft;
                case AgentConstants::MOTOR_LOOK_RIGHT:
                    return MotorCommand::LookRight;
                default:
                    return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Validate reward ranges
    reward = std::clamp(reward, AgentConstants::MIN_PREDICTION_ERROR, AgentConstants::MAX_PREDICTION_ERROR);
    predictedReward = std::clamp(predictedReward, AgentConstants::MIN_PREDICTION_ERROR, AgentConstants::MAX_PREDICTION_ERROR);
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, AgentConstants::MIN_PREDICTION_ERROR, AgentConstants::MAX_PREDICTION_ERROR);
    
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
        plasticityModifier_ = AgentConstants::INITIAL_PLASTICITY;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = AgentConstants::CRITICAL_PERIOD_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = AgentConstants::MATURATION_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = AgentConstants::MATURE_PLASTICITY;  // Adult - more stable
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