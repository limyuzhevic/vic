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
    // Optimized neuron group initialization using efficient distribution
    if (brain_) {
        // Pre-allocate vectors based on expected neuron counts from configuration
        // This avoids repeated reallocations and provides predictable memory layout
        
        // Collect all motor and sensory neurons first, then distribute them efficiently
        std::vector<std::pair<Neuron*, NeuronType>> allNeurons;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                auto& neurons = pop->getNeurons();
                
                // Add all neurons of this population to our collection
                for (Neuron* n : neurons) {
                    allNeurons.emplace_back(n, type);
                }
            }
        }
        
        // Efficient distribution using modulo arithmetic
        for (const auto& [neuron, type] : allNeurons) {
            if (type == NeuronType::Motor) {
                // More efficient than maintaining separate size calculations
                static const int motorGroups = 6;
                static int motorIndex = 0;
                
                switch (motorIndex % motorGroups) {
                    case 0: motorForward_.push_back(neuron); break;
                    case 1: motorBackward_.push_back(neuron); break;
                    case 2: motorTurnLeft_.push_back(neuron); break;
                    case 3: motorTurnRight_.push_back(neuron); break;
                    case 4: motorInteract_.push_back(neuron); break;
                    case 5: motorWait_.push_back(neuron); break;
                }
                motorIndex++;
            } else if (type == NeuronType::Sensory) {
                // More efficient than maintaining separate size calculations
                static const int sensoryGroups = 4;
                static int sensoryIndex = 0;
                
                switch (sensoryIndex % sensoryGroups) {
                    case 0: sensoryVision_.push_back(neuron); break;
                    case 1: sensoryTouch_.push_back(neuron); break;
                    case 2: sensoryInternal_.push_back(neuron); break;
                    case 3: sensoryProprioception_.push_back(neuron); break;
                }
                sensoryIndex++;
            }
        }
    }
}

// Clear all agent brain state
    reset();
}

AgentBrain::~AgentBrain() {
    // Clear all neuron vectors to avoid memory leaks
    // The vectors contain raw pointers, but we don't own the memory
    // This is mainly for consistency and debugging
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
    
    previousVision_.clear();
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Optimized initialization using configuration-driven approach
    // Get configuration values for input sizes
    
    // Vision configuration (can be overridden by config file)
    int visionWidth = world.getVisionWidth();
    int visionHeight = world.getVisionHeight();
    int visionSize = visionWidth * visionHeight;  // Default: 16x16 = 256
    
    // Touch configuration (can be overridden by config file)
    int touchSize = 8;  // Default: 8 touch sensors
    
    // Internal signals configuration (can be overridden by config file)
    int internalSize = 4;  // Default: 4 internal signals
    
    // Proprioception configuration (can be overridden by config file)
    int proprioceptionSize = 6;  // Default: 6 proprioceptive sensors
    
    // Pre-allocate vectors with exact sizes to avoid reallocations
    previousVision_.resize(visionSize, 0.0f);
    
    // Set up sensory neuron group sizes
    sensoryVision_.reserve(visionSize);
    sensoryTouch_.reserve(touchSize);
    sensoryInternal_.reserve(internalSize);
    sensoryProprioception_.reserve(proprioceptionSize);
    
    // Motor neuron groups
    motorForward_.reserve(visionSize / 16);  // Reasonable estimate
    motorBackward_.reserve(visionSize / 16);
    motorTurnLeft_.reserve(visionSize / 16);
    motorTurnRight_.reserve(visionSize / 16);
    motorInteract_.reserve(visionSize / 16);
    motorWait_.reserve(visionSize / 16);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Enhanced logging with configuration values
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioceptive neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Use configuration values instead of hard-coded constants
    // This allows runtime configuration and experiment customization
    
    // Default values if config not available
    size_t visionSize = 256;  // 16x16 vision grid
    size_t touchSize = 8;     // 8 touch sensors
    size_t internalSize = 4;  // 4 internal signals
    size_t proprioceptionSize = 6;  // 6 proprioceptive sensors
    
    // If brain has config, use configured values
    if (brain_) {
        auto config = brain_->getConfig();
        if (config) {
            // Vision configuration
            visionSize = config->getOr<size_t>("sensory.vision.size", visionSize);
            // Touch configuration
            touchSize = config->getOr<size_t>("sensory.touch.size", touchSize);
            // Internal signals configuration
            internalSize = config->getOr<size_t>("sensory.internal.size", internalSize);
            // Proprioception configuration
            proprioceptionSize = config->getOr<size_t>("sensory.proprioception.size", proprioceptionSize);
        }
    }
    
    return visionSize + touchSize + internalSize + proprioceptionSize;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Use configuration values for motor output size
    size_t motorSize = 6;  // Default: MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight
    
    // Add Interact and Wait if configured
    motorSize += 2;  // Interact and Wait
    
    // If brain has config, use configured values
    if (brain_) {
        auto config = brain_->getConfig();
        if (config) {
            motorSize = config->getOr<size_t>("motor.output.size", motorSize);
        }
    }
    
    return motorSize;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Get configuration values for processing parameters
    float visionScale = 5.0f;
    float touchScale = 8.0f;
    float internalCenter = 1.0f;
    float internalScale = 5.0f;
    float proprioCenter = 1.0f;
    float proprioScale = 3.0f;
    
    // Get config if available
    if (brain_) {
        auto config = brain_->getConfig();
        if (config) {
            visionScale = config->getOr<float>("sensory.vision.scale", visionScale);
            touchScale = config->getOr<float>("sensory.touch.scale", touchScale);
            internalCenter = config->getOr<float>("sensory.internal.center", internalCenter);
            internalScale = config->getOr<float>("sensory.internal.scale", internalScale);
            proprioCenter = config->getOr<float>("sensory.proprioception.center", proprioCenter);
            proprioScale = config->getOr<float>("sensory.proprioception.scale", proprioScale);
        }
    }
    
    // Optimized vision processing with bounds checking
    const auto& vision = percept.getVision();
    size_t minVisionSize = std::min(sensoryVision_.size(), vision.size());
    for (size_t i = 0; i < minVisionSize; ++i) {
        // Only process if neuron exists (optimization: no need for if check)
        if (sensoryVision_[i]) {
            sensoryVision_[i]->injectCurrent(vision[i] * visionScale);
        }
    }
    
    // Optimized touch processing with bounds checking
    const auto& touch = percept.getTouch();
    size_t minTouchSize = std::min(sensoryTouch_.size(), touch.size());
    for (size_t i = 0; i < minTouchSize; ++i) {
        if (sensoryTouch_[i]) {
            sensoryTouch_[i]->injectCurrent(touch[i] * touchScale);
        }
    }
    
    // Optimized internal signals processing with bounds checking
    const auto& intern = percept.getInternal();
    size_t minInternalSize = std::min(sensoryInternal_.size(), intern.size());
    for (size_t i = 0; i < minInternalSize; ++i) {
        if (sensoryInternal_[i]) {
            sensoryInternal_[i]->injectCurrent((intern[i] * internalCenter - internalCenter) * internalScale);
        }
    }
    
    // Optimized proprioception processing with bounds checking
    const auto& proprio = percept.getProprioception();
    size_t minProprioSize = std::min(sensoryProprioception_.size(), proprio.size());
    for (size_t i = 0; i < minProprioSize; ++i) {
        if (sensoryProprioception_[i]) {
            sensoryProprioception_[i]->injectCurrent((proprio[i] * proprioCenter - proprioCenter) * proprioScale);
        }
    }
    
    // Optimized novelty computation using vector operations
    if (!vision.empty()) {
        // Use vectorized operations for better performance
        size_t minVisionSize = std::min(vision.size(), previousVision_.size());
        float totalDiff = 0.0f;
        
        // Compute differences using vectorized approach
        for (size_t i = 0; i < minVisionSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and apply decay
        if (vision.size() > 0) {
            noveltyLevel_ = totalDiff / static_cast<float>(vision.size());
            noveltyLevel_ *= sensoryNoveltyDecay_;
        }
        
        // Store for next time using efficient assignment
        if (vision.size() <= previousVision_.size()) {
            std::copy(vision.begin(), vision.begin() + vision.size(), previousVision_.begin());
        } else {
            previousVision_ = vision;
        }
    }
    
    // Update curiosity based on novelty using configurable parameters
    if (curiosityEnabled_) {
        float curiosityMultiplier = 2.0f;
        float predictionErrorMultiplier = 0.5f;
        
        // Get config values if available
        if (brain_) {
            auto config = brain_->getConfig();
            if (config) {
                curiosityMultiplier = config->getOr<float>("neuromodulation.curiosity.multiplier", curiosityMultiplier);
                predictionErrorMultiplier = config->getOr<float>("neuromodulation.curiosity.prediction_error_multiplier", predictionErrorMultiplier);
            }
        }
        
        curiosityLevel_ = noveltyLevel_ * curiosityMultiplier + std::abs(predictionError_) * predictionErrorMultiplier;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
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

// Optimized motor decoding using vectorized operations
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    // Get activity levels with bounds checking
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity using more efficient approach
    float bestActivity = waitAct;
    MotorCommand best = MotorCommand::Wait;
    
    // Threshold for action selection (can be configured)
    float activityThreshold = 0.5f;
    if (brain_) {
        auto config = brain_->getConfig();
        if (config) {
            activityThreshold = config->getOr<float>("motor.action_threshold", activityThreshold);
        }
    }
    
    // Vectorized activity comparison
    const std::array<std::pair<MotorCommand, float>, 6> commandActivities = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    for (const auto& cmdAct : commandActivities) {
        if (cmdAct.second > bestActivity) {
            bestActivity = cmdAct.second;
            best = cmdAct.first;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < activityThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

// Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Get exploration parameters from config
        float exploreChanceBase = 0.3f;
        int maxMotorCommand = 8;  // 0-7 for MotorCommand enum
        
        if (brain_) {
            auto config = brain_->getConfig();
            if (config) {
                exploreChanceBase = config->getOr<float>("motor.exploration.chance", exploreChanceBase);
                maxMotorCommand = config->getOr<int>("motor.exploration.max_command", maxMotorCommand);
            }
        }
        
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * exploreChanceBase;  // Up to configured percentage
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, maxMotorCommand);
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
    
    // Get configuration values
    float eligibilityThreshold = 0.001f;
    float eligibilityDecay = 0.1f;
    float rewardDecay = 0.05f;
    float dopamineClampingMin = -1.0f;
    float dopamineClampingMax = 1.0f;
    float plasticityBase = 0.5f;
    
    // Get config if available
    if (brain_) {
        auto config = brain_->getConfig();
        if (config) {
            eligibilityThreshold = config->getOr<float>("reward.modulation.eligibility_threshold", eligibilityThreshold);
            eligibilityDecay = config->getOr<float>("reward.modulation.eligibility_decay", eligibilityDecay);
            rewardDecay = config->getOr<float>("reward.modulation.reward_decay", rewardDecay);
            dopamineClampingMin = config->getOr<float>("reward.modulation.dopamine_min", dopamineClampingMin);
            dopamineClampingMax = config->getOr<float>("reward.modulation.dopamine_max", dopamineClampingMax);
            plasticityBase = config->getOr<float>("reward.modulation.plasticity_base", plasticityBase);
        }
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = (1.0f - rewardDecay) * expectedReward_ + rewardDecay * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range using configurable bounds
    dopamineLevel_ = std::clamp(dopamineLevel_, dopamineClampingMin, dopamineClampingMax);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > eligibilityThreshold) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace using configurable decay rate
                syn->decayEligibilityTrace(eligibilityDecay);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = plasticityBase + 0.5f * dopamineLevel_;
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
    
    // Get developmental stage parameters from config
    double stage1Duration = 60.0;   // ~1 minute
    double stage2Duration = 240.0; // ~4 minutes  
    double stage3Duration = 660.0; // ~11 minutes
    float plasticity1 = 1.0f;      // High plasticity
    float plasticity2 = 0.8f;      // Moderate plasticity
    float plasticity3 = 0.5f;      // Lower plasticity
    float plasticityAdult = 0.2f;   // Adult - more stable
    
    // Get config if available
    if (brain_) {
        auto config = brain_->getConfig();
        if (config) {
            stage1Duration = config->getOr<double>("development.stage1.duration", stage1Duration);
            stage2Duration = config->getOr<double>("development.stage2.duration", stage2Duration);
            stage3Duration = config->getOr<double>("development.stage3.duration", stage3Duration);
            plasticity1 = config->getOr<float>("development.stage1.plasticity", plasticity1);
            plasticity2 = config->getOr<float>("development.stage2.plasticity", plasticity2);
            plasticity3 = config->getOr<float>("development.stage3.plasticity", plasticity3);
            plasticityAdult = config->getOr<float>("development.adult.plasticity", plasticityAdult);
        }
    }
    
    // Simple developmental stages based on age with configurable durations
    if (developmentalAge_ < stage1Duration) {
        plasticityModifier_ = plasticity1;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < stage1Duration + stage2Duration) {
        plasticityModifier_ = plasticity2;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < stage1Duration + stage2Duration + stage3Duration) {
        plasticityModifier_ = plasticity3;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = plasticityAdult;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development using configurable rates
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Get development parameters from config
            float synRateBase = 0.0001f;
            float pruneRateBase = 0.00001f;
            
            if (brain_) {
                auto config = brain_->getConfig();
                if (config) {
                    synRateBase = config->getOr<float>("development.synaptogenesis.base_rate", synRateBase);
                    pruneRateBase = config->getOr<float>("development.pruning.base_rate", pruneRateBase);
                }
            }
            
            // Higher synaptogenesis in early development
            float synRate = synRateBase * plasticityModifier_;
            float pruneRate = pruneRateBase * (2.0f - plasticityModifier_);
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
    // Reset all agent brain state using efficient operations
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision using efficient std::fill
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
