#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Neural constant thresholds and parameters
namespace Constants {
    constexpr float CURIOUSITY_THRESHOLD = 0.5f;                    // Threshold for curiosity-based exploration
    constexpr float EXPLORATION_CHANCE_MULTIPLIER = 0.3f;            // Base multiplier for exploration chance
    constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;            // Minimum eligibility trace magnitude to process
    constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;                  // Decay rate for eligibility traces per timestep
    constexpr float EXPECTED_REWARD_SMOOTHING = 0.95f;               // Smoothing factor for expected reward updates
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;                      // Minimum allowable dopamine level
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;                       // Maximum allowable dopamine level
    constexpr float PLASTICITY_FACTOR_MIN = 0.1f;                    // Minimum plasticity factor
    constexpr float PLASTICITY_FACTOR_MAX = 2.0f;                    // Maximum plasticity factor
    constexpr float BASIC_ACTIVITY_THRESHOLD = 0.5f;                 // Minimum neural activity required for action
    constexpr float DEFAULT_EPS = 0.001f;                           // Default tolerance for numerical comparisons
    
    // STDP (Spike-Timing-Dependent Plasticity) weights
    constexpr float STDP_LTP_WEIGHT = 0.01f;                         // Long-Term Potentiation weight
    constexpr float STDP_LTD_WEIGHT = 0.012f;                        // Long-Term Depression weight
    
    // Developmental system parameters
    constexpr float SYNAPTOGENESIS_RATE_BASE = 0.0001f;              // Base synaptogenesis rate
    constexpr float PRUNING_RATE_BASE = 0.00001f;                   // Base pruning rate
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
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
    // Validate input pointer
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain created with null brain pointer");
        return;
    }
    
    // Initialize motor and sensory neuron groups with improved distribution
    // Cache frequently accessed brain components for better performance
    const auto& regions = brain_->getRegions();
    if (!regions.empty()) {
        for (const auto& region : regions) {
            if (!region) continue;  // Safety check
            
            const auto& populations = region->getPopulations();
            if (populations.empty()) continue;  // Optimization: skip empty populations
            
            for (const auto& pop : populations) {
                if (!pop) continue;  // Safety check
                
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

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate world pointer
    if (!world.getVisionWidth() || !world.getVisionHeight()) {
        NLM_LOG_WARNING("AgentBrain initialized with invalid world dimensions");
        return;
    }
    
    // Resize sensory arrays with validation
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.resize(visionSize, 0.0f);
    
    // Initialize development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization for debugging
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
    // Validate inputs
    if (!brain_ || !percept.getVisionWidth() || !percept.getVisionHeight()) {
        return;
    }
    
    // Cache frequently accessed vectors for performance
    const auto& vision = percept.getVision();
    const size_t visionSize = vision.size();
    
    // Vision input (256 values -> sensoryVision_ neurons)
    if (visionSize > 0 && !sensoryVision_.empty()) {
        const size_t minSize = std::min(visionSize, sensoryVision_.size());
        for (size_t i = 0; i < minSize; ++i) {
            Neuron* neuron = sensoryVision_[i];
            if (neuron) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    if (!touch.empty() && !sensoryTouch_.empty()) {
        const size_t minSize = std::min(touch.size(), sensoryTouch_.size());
        for (size_t i = 0; i < minSize; ++i) {
            Neuron* neuron = sensoryTouch_[i];
            if (neuron) {
                float current = touch[i] * 8.0f;  // Collision signal
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    if (!intern.empty() && !sensoryInternal_.empty()) {
        const size_t minSize = std::min(intern.size(), sensoryInternal_.size());
        for (size_t i = 0; i < minSize; ++i) {
            Neuron* neuron = sensoryInternal_[i];
            if (neuron) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty() && !sensoryProprioception_.empty()) {
        const size_t minSize = std::min(proprio.size(), sensoryProprioception_.size());
        for (size_t i = 0; i < minSize; ++i) {
            Neuron* neuron = sensoryProprioception_[i];
            if (neuron) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        const size_t minVisionSize = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < minVisionSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / static_cast<float>(std::max<size_t>(vision.size(), 1));
        
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
    // Validate brain state
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > Constants::CURIOUSITY_THRESHOLD) {
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
            if (!n) continue;  // Safety check
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / static_cast<float>(neurons.size());
    };
    
    // Cache frequently accessed vectors for performance
    const std::vector<Neuron*>& forward = motorForward_;
    const std::vector<Neuron*>& backward = motorBackward_;
    const std::vector<Neuron*>& left = motorTurnLeft_;
    const std::vector<Neuron*>& right = motorTurnRight_;
    const std::vector<Neuron*>& interact = motorInteract_;
    const std::vector<Neuron*>& wait = motorWait_;
    
    float forwardAct = calcActivity(forward);
    float backwardAct = calcActivity(backward);
    float leftAct = calcActivity(left);
    float rightAct = calcActivity(right);
    float interactAct = calcActivity(interact);
    float waitAct = calcActivity(wait);
    
    // Find maximum activity using improved algorithm
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
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
    if (bestActivity < Constants::BASIC_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > Constants::CURIOUSITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * Constants::EXPLORATION_CHANCE_MULTIPLIER;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 6);  // Fixed: 0-6 to match switch cases
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
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 6);  // Fixed: 0-6 to match switch cases
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
    // Validate inputs and brain state
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = Constants::EXPECTED_REWARD_SMOOTHING * expectedReward_ + 
                     (1.0f - Constants::EXPECTED_REWARD_SMOOTHING) * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, Constants::DOPAMINE_CLAMP_MIN, Constants::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    // Cache frequently accessed regions for performance
    const auto& regions = brain_->getRegions();
    if (regions.empty()) return;  // Optimization: skip if no regions
    
    for (const auto& region : regions) {
        if (!region) continue;  // Safety check
        
        const auto& synapses = region->getSynapses();
        if (synapses.empty()) continue;  // Optimization: skip empty region
        
        for (auto* syn : synapses) {
            if (!syn) continue;  // Safety check
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > Constants::ELIGIBILITY_TRACE_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(Constants::ELIGIBILITY_TRACE_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, Constants::PLASTICITY_FACTOR_MIN, Constants::PLASTICITY_FACTOR_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(Constants::STDP_LTP_WEIGHT * plasticityFactor);
        stdp->setLTDWeight(Constants::STDP_LTD_WEIGHT * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    // Validate inputs and development system state
    if (!brain_ || !developmentEnabled_) return;
    
    if (timestep <= 0.0) {
        NLM_LOG_WARNING("Invalid timestep provided to updateDevelopment: " + std::to_string(timestep));
        return;
    }
    
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
            float synRate = Constants::SYNAPTOGENESIS_RATE_BASE * plasticityModifier_;
            float pruneRate = Constants::PRUNING_RATE_BASE * (2.0f - plasticityModifier_);
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
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous sensory state
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
