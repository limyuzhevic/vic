#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>

namespace nlm {

// AgentBrain error handling constants
namespace AgentBrainErrorMessages {
    const std::string BRAIN_NULL = "AgentBrain::{}: Brain pointer is null";
    const std::string NULL_NEURON_POINTER = "AgentBrain::{}: Neuron pointer is null";
    const std::string NULL_PERCEPT = "AgentBrain::{}: Percept pointer is null";
    const std::string NULL_WORLD = "AgentBrain::{}: World pointer is null";
    const std::string INVALID_PERCEPT_SIZE = "AgentBrain::{}: Percept data size {} exceeds expected {}-{}";
    const std::string VECTOR_INDEX_OUT_OF_BOUNDS = "AgentBrain::{}: Vector index {} out of bounds (size: {})";
    const std::string DIVISION_BY_ZERO = "AgentBrain::{}: Division by zero prevented";
    const std::string NORMALIZATION_FAILED = "AgentBrain::{}: Normalization failed (invalid data)";
    const std::string NULL_RANDOM_GENERATOR = "AgentBrain::{}: Random generator is null";
}

// AgentBrain error handling constants
namespace AgentBrainErrorMessages {
    const std::string BRAIN_NULL = "AgentBrain::{}: Brain pointer is null";
    const std::string NULL_NEURON_POINTER = "AgentBrain::{}: Neuron pointer is null";
    const std::string NULL_PERCEPT = "AgentBrain::{}: Percept pointer is null";
    const std::string NULL_WORLD = "AgentBrain::{}: World pointer is null";
    const std::string INVALID_PERCEPT_SIZE = "AgentBrain::{}: Percept data size {} exceeds expected {}-{}";
    const std::string VECTOR_INDEX_OUT_OF_BOUNDS = "AgentBrain::{}: Vector index {} out of bounds (size: {})";
    const std::string DIVISION_BY_ZERO = "AgentBrain::{}: Division by zero prevented";
    const std::string NORMALIZATION_FAILED = "AgentBrain::{}: Normalization failed (invalid data)";
    const std::string NULL_RANDOM_GENERATOR = "AgentBrain::{}: Random generator is null";
}

// AgentBrain validation constants
namespace AgentBrainConstants {
    // Sensory neuron group sizes
    static constexpr size_t VISION_NEURON_GROUP_SIZE = 256;
    static constexpr size_t TOUCH_NEURON_GROUP_SIZE = 8;
    static constexpr size_t INTERNAL_NEURON_GROUP_SIZE = 4;
    static constexpr size_t PROPRIOCEPTION_NEURON_GROUP_SIZE = 6;
    static constexpr size_t MOTOR_NEURON_GROUP_SIZE = 6;
    
    // Validation ranges
    static constexpr float MIN_NOVELTY_LEVEL = 0.0f;
    static constexpr float MAX_NOVELTY_LEVEL = 1.0f;
    static constexpr float MIN_CURIOUSITY_LEVEL = 0.0f;
    static constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;
    static constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
    static constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
    static constexpr float MIN_PREDICTION_ERROR = -10.0f;
    static constexpr float MAX_PREDICTION_ERROR = 10.0f;
    static constexpr float MIN_EXPLORATION_CHANCE = 0.0f;
    static constexpr float MAX_EXPLORATION_CHANCE = 1.0f;
    static constexpr float MIN_ACTIVITY_THRESHOLD = 0.0f;
    static constexpr float MAX_ACTIVITY_THRESHOLD = 1.0f;
    
    // Vector and size limits
    static constexpr size_t MIN_VECTOR_SIZE = 1;
    static constexpr size_t MAX_VECTOR_SIZE = 10000;
    static constexpr float MIN_TIMESTEP = 0.001;
    static constexpr float MAX_TIMESTEP = 1000.0;
    
    // Processing parameters
    static constexpr float SENSORY_NOVELTY_DECAY_RATE = 0.99f;
    static constexpr float SENSORY_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float EXPLOITATION_THRESHOLD = 0.3f;
    static constexpr float NOVELTY_SENSITIVITY = 2.0f;
    static constexpr float PREDICTION_ERROR_SENSITIVITY = 0.5f;
    static constexpr float CURIOUSITY_MULTIPLIER = 2.0f;
    static constexpr float EXPLORATION_MULTIPLIER = 0.3f;
    
    // Plasticity parameters for dopamine-based modulation
    static constexpr float PLASTICITY_BASE = 0.5f;
    static constexpr float PLASTICITY_SENSITIVITY = 0.5f;
    
    // Input processing scaling factors
    static constexpr float VISION_SCALING_FACTOR = 5.0f;
    static constexpr float TOUCH_SCALING_FACTOR = 8.0f;
    static constexpr float INTERNAL_SCALING_FACTOR = 5.0f;
    static constexpr float PROPRIOCEPTION_SCALING_FACTOR = 3.0f;
    
    // Normalization and centering
    static constexpr float NORMALIZATION_OFFSET = -1.0f;
    static constexpr float NORMALIZATION_SCALE = 2.0f;
    
    // Plasticity and adaptation
    static constexpr float PLASTICITY_MODIFIER_CLAMP_MIN = 0.1f;
    static constexpr float PLASTICITY_MODIFIER_CLAMP_MAX = 2.0f;
    static constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
    static constexpr float ELIGIBILITY_TRACE_DECAY_RATE = 0.1f;
    static constexpr float LEARNING_RATE_BASE = 0.01f;
    static constexpr float LEARNING_RATE_STDP_LTP = 0.01f;
    static constexpr float LEARNING_RATE_STDP_LTD = 0.012f;
    
    // Developmental stages
    static constexpr float DEVELOPMENT_STAGE_1_END = 60.0;
    static constexpr float DEVELOPMENT_STAGE_2_END = 300.0;
    static constexpr float DEVELOPMENT_STAGE_3_END = 900.0;
    static constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;
    static constexpr float PLASTICITY_MODIFIER_CRITICAL_PERIOD = 0.8f;
    static constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f;
    static constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;
    
    // Synaptogenesis and pruning
    static constexpr float SYNAPTOGENESIS_RATE_INITIAL = 0.0001f;
    static constexpr float PRUNING_RATE_INITIAL = 0.00001f;
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
    if (!brain_) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::AgentBrain");
        return;
    }
    
    try {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_ERROR(AgentBrainErrorMessages::NULL_NEURON_POINTER, "AgentBrain::AgentBrain");
                            continue;
                        }
                        
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        if (idx >= SIZE_MAX) {
                            NLM_LOG_ERROR("AgentBrain: Too many motor neurons, skipping one");
                            continue;
                        }
                        
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
                        if (!n) {
                            NLM_LOG_ERROR(AgentBrainErrorMessages::NULL_NEURON_POINTER, "AgentBrain::AgentBrain");
                            continue;
                        }
                        
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        if (idx >= SIZE_MAX) {
                            NLM_LOG_ERROR("AgentBrain: Too many sensory neurons, skipping one");
                            continue;
                        }
                        
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("AgentBrain initialization failed: ") + e.what(), "AgentBrain::AgentBrain");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::initialize");
        return;
    }
    
    if (!world.getVisionWidth() || !world.getVisionHeight()) {
        NLM_LOG_ERROR("AgentBrain: Invalid world dimensions", "AgentBrain::initialize");
        return;
    }
    
    try {
        // Ensure vision buffer is large enough for world size
        size_t expectedVisionSize = static_cast<size_t>(world.getVisionWidth()) * world.getVisionHeight();
        previousVision_.resize(expectedVisionSize, 0.0f);
        
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Validate neuron groups
        size_t totalVisionNeurons = 0;
        for (Neuron* n : sensoryVision_) {
            if (n) totalVisionNeurons++;
        }
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(totalVisionNeurons) + " vision sensory neurons");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("AgentBrain initialization failed: ") + e.what(), "AgentBrain::initialize");
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    if (!brain_) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::getSensoryInputSize");
        return 0;
    }
    
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return AgentBrainConstants::VISION_NEURON_GROUP_SIZE + 
           AgentBrainConstants::TOUCH_NEURON_GROUP_SIZE +
           AgentBrainConstants::INTERNAL_NEURON_GROUP_SIZE +
           AgentBrainConstants::PROPRIOCEPTION_NEURON_GROUP_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return AgentBrainConstants::MOTOR_NEURON_GROUP_SIZE;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::processSensoryInput");
        return;
    }
    
    if (!percept) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::NULL_PERCEPT, "AgentBrain::processSensoryInput");
        return;
    }
    
    try {
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        
        // Validate vision data size
        size_t expectedVisionSize = AgentBrainConstants::VISION_NEURON_GROUP_SIZE;
        if (vision.size() != expectedVisionSize) {
            NLM_LOG_WARNING(AgentBrainErrorMessages::INVALID_PERCEPT_SIZE, "AgentBrain::processSensoryInput",
                           vision.size(), vision.size(), expectedVisionSize);
            // Continue with available data
        }
        
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Validate neuron pointer
                float current = vision[i] * AgentBrainConstants::VISION_SCALING_FACTOR;  // Scale factor
                
                // Clamp current to valid range
                if (current < NLMConstants::MIN_CURRENT) current = NLMConstants::MIN_CURRENT;
                if (current > NLMConstants::MAX_CURRENT) current = NLMConstants::MAX_CURRENT;
                
                sensoryVision_[i]->injectCurrent(current);
            }
        }
        
        // Touch input (8 values -> sensoryTouch_ neurons)
        const auto& touch = percept.getTouch();
        
        size_t expectedTouchSize = AgentBrainConstants::TOUCH_NEURON_GROUP_SIZE;
        if (touch.size() != expectedTouchSize) {
            NLM_LOG_WARNING(AgentBrainErrorMessages::INVALID_PERCEPT_SIZE, "AgentBrain::processSensoryInput",
                           touch.size(), touch.size(), expectedTouchSize);
        }
        
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * AgentBrainConstants::TOUCH_SCALING_FACTOR;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
        
        // Internal signals (4 values -> sensoryInternal_ neurons)
        const auto& intern = percept.getInternal();
        
        size_t expectedInternalSize = AgentBrainConstants::INTERNAL_NEURON_GROUP_SIZE;
        if (intern.size() != expectedInternalSize) {
            NLM_LOG_WARNING(AgentBrainErrorMessages::INVALID_PERCEPT_SIZE, "AgentBrain::processSensoryInput",
                           intern.size(), intern.size(), expectedInternalSize);
        }
        
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * AgentBrainConstants::NORMALIZATION_SCALE + AgentBrainConstants::NORMALIZATION_OFFSET) * AgentBrainConstants::INTERNAL_SCALING_FACTOR;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
        
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        const auto& proprio = percept.getProprioception();
        
        size_t expectedProprioceptionSize = AgentBrainConstants::PROPRIOCEPTION_NEURON_GROUP_SIZE;
        if (proprio.size() != expectedProprioceptionSize) {
            NLM_LOG_WARNING(AgentBrainErrorMessages::INVALID_PERCEPT_SIZE, "AgentBrain::processSensoryInput",
                           proprio.size(), proprio.size(), expectedProprioceptionSize);
        }
        
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * AgentBrainConstants::NORMALIZATION_SCALE + AgentBrainConstants::NORMALIZATION_OFFSET) * AgentBrainConstants::PROPRIOCEPTION_SCALING_FACTOR;  // Center and scale
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
        
        // Compute novelty (difference from previous vision)
        if (!vision.empty()) {
            float totalDiff = 0.0f;
            size_t validComparisonCount = 0;
            
            for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
                float diff = std::abs(vision[i] - previousVision_[i]);
                totalDiff += diff;
                validComparisonCount++;
            }
            
            if (validComparisonCount > 0) {
                // Normalize
                noveltyLevel_ = totalDiff / validComparisonCount;
            } else {
                noveltyLevel_ = 0.0f;
            }
            
            // Decay and update
            noveltyLevel_ *= sensoryNoveltyDecay_;
            
            // Clamp novelty level to valid range
            noveltyLevel_ = std::clamp(noveltyLevel_, AgentBrainConstants::MIN_NOVELTY_LEVEL, 
                                       AgentBrainConstants::MAX_NOVELTY_LEVEL);
            
            // Store for next time
            previousVision_ = vision;
        }
        
// Update curiosity based on novelty
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * AgentBrainConstants::CURIOUSITY_MULTIPLIER + std::abs(predictionError_) * AgentBrainConstants::PREDICTION_ERROR_SENSITIVITY;
            curiosityLevel_ = std::clamp(curiosityLevel_, AgentBrainConstants::MIN_CURIOUSITY_LEVEL,
                                         AgentBrainConstants::MAX_CURIOUSITY_LEVEL);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("AgentBrain processSensoryInput failed: ") + e.what(), "AgentBrain::processSensoryInput");
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::decodeMotorCommand");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::EXPLOITATION_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    if (!brain_) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::decodeFromMotorNeurons");
        return MotorCommand::Wait;
    }
    
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) {
                NLM_LOG_ERROR(AgentBrainErrorMessages::NULL_NEURON_POINTER, "AgentBrain::decodeFromMotorNeurons");
                continue;
            }
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
    if (bestActivity < AgentBrainConstants::SENSORY_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (!brain_) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::selectWithCuriosity");
        return defaultCmd;
    }
    
    if (!brain_->getRandomGenerator()) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::NULL_RANDOM_GENERATOR, "AgentBrain::selectWithCuriosity");
        return defaultCmd;
    }
    
// Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConstants::EXPLOITATION_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConstants::EXPLORATION_MULTIPLIER;  // Up to 30% random
        
        // Validate exploration chance
        exploreChance = std::clamp(exploreChance, AgentBrainConstants::MIN_EXPLORATION_CHANCE,
                                   AgentBrainConstants::MAX_EXPLORATION_CHANCE);
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, AgentBrainConstants::MOTOR_NEURON_GROUP_SIZE);
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
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::applyRewardModulation");
        }
        return;
    }
    
    try {
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Clamp prediction error
        predictionError_ = std::clamp(predictionError_, AgentBrainConstants::MIN_PREDICTION_ERROR,
                                      AgentBrainConstants::MAX_PREDICTION_ERROR);
        
        // Update expected reward (exponential moving average)
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        
        // Clamp dopamine to reasonable range
        dopamineLevel_ = std::clamp(dopamineLevel_, AgentBrainConstants::MIN_DOPAMINE_LEVEL,
                                    AgentBrainConstants::MAX_DOPAMINE_LEVEL);
        
        // Apply to all synapses with eligibility traces
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (!syn) continue;
                
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > AgentBrainConstants::ELIGIBILITY_TRACE_THRESHOLD) {
                    // Apply reward-modulated weight change
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(AgentBrainConstants::ELIGIBILITY_TRACE_DECAY_RATE);
                }
            }
        }
        
        // Modulate plasticity based on dopamine
        // Positive dopamine increases plasticity, negative decreases
        float plasticityFactor = AgentBrainConstants::PLASTICITY_BASE + AgentBrainConstants::PLASTICITY_SENSITIVITY * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, AgentBrainConstants::PLASTICITY_MODIFIER_CLAMP_MIN, AgentBrainConstants::PLASTICITY_MODIFIER_CLAMP_MAX);
        
        // Apply to STDP
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(AgentBrainConstants::LEARNING_RATE_STDP_LTP * plasticityFactor);
            stdp->setLTDWeight(AgentBrainConstants::LEARNING_RATE_STDP_LTD * plasticityFactor);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("AgentBrain applyRewardModulation failed: ") + e.what(), "AgentBrain::applyRewardModulation");
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::updateDevelopment");
        }
        return;
    }
    
    // Validate timestep
    if (timestep < AgentBrainConstants::MIN_TIMESTEP || timestep > AgentBrainConstants::MAX_TIMESTEP) {
        NLM_LOG_ERROR(std::string("AgentBrain: Invalid timestep ") + std::to_string(timestep), "AgentBrain::updateDevelopment");
        return;
    }
    
    try {
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
        // This is a biologically inspired approximation
        if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_1_END) {  // ~1 minute
            plasticityModifier_ = AgentBrainConstants::PLASTICITY_MODIFIER_INITIAL;  // High plasticity
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_2_END) {  // ~5 minutes
            plasticityModifier_ = AgentBrainConstants::PLASTICITY_MODIFIER_CRITICAL_PERIOD;
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_3_END) {  // ~15 minutes
            plasticityModifier_ = AgentBrainConstants::PLASTICITY_MODIFIER_MATURATION;
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            plasticityModifier_ = AgentBrainConstants::PLASTICITY_MODIFIER_ADULT;  // Adult - more stable
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
        
        // Structural plasticity changes with development
        if (structuralPlasticityEnabled_) {
            auto* sp = brain_->getStructuralPlasticity();
            if (sp) {
                // Higher synaptogenesis in early development
                float synRate = AgentBrainConstants::SYNAPTOGENESIS_RATE_INITIAL * plasticityModifier_;
                float pruneRate = AgentBrainConstants::PRUNING_RATE_INITIAL * (AgentBrainConstants::PLASTICITY_MODIFIER_MAX - plasticityModifier_);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("AgentBrain updateDevelopment failed: ") + e.what(), "AgentBrain::updateDevelopment");
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_ERROR(AgentBrainErrorMessages::BRAIN_NULL, "AgentBrain::getDevelopmentalStage");
        return DevelopmentalStage::Initial;
    }
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